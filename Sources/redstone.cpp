#include "Chunk.hpp"
#include "DayCycle.hpp"

/*
 Some info:

 - To strongly power a block: - it will weakly power its own adj blocks
							  - only torches and levers can strongly power a block
 - To weakdy power a block:   - it will weakly power its own adj blocks, but NOT if they are redstone dusts
							  - only a red dust can weakdy power a block (hence the name I coined)
 - To weakly power a block:   it activates if it is a redstone componant, else does nothing
 - adj blocks == 6 cardinals only

 - A redstone lamp:
	- activates instantly, but takes 2 redstone ticks to turn off.

 - redstone torch:
	- can be deactivated by strongly/weakdy powering block it is placed on
	- has 1 tick of delay
	- does NOT power block it is placed on
	- strongly powers block ABOVE it, weakly powers adj blocks

 - repeater:
	- delay in range [1:4] ticks
	- only receive signal from block behind it
	- accepts strong, weak, weakdy signals
	- strongly powers (signal 15) block in front of it
	- can be blocked by powered repeater/comparator from side
	- if lock/unlock causes a scheduled tick and another scheduled tick is called on the same tick (input signal change),
			the second schedule is aborted

 - comparator:
	- outputs same signal as rear input
	- if COMPARE mode:   output is rear if rear > left && rear > right else 0
	- if SUBSTRACT mode: output is max(0, rear - max(left, right))
	- left and right can only come from dust, repeater, comparator, and redstone block

 - piston:
	- no delay to start movement
	- 3 game ticks to extend, 2 game ticks to retract
	- can't move more than 12 other blocks
	- also checks above on the side for power source
	- piston head and moved blocks are entities until end of movement
	- if sticky piston retracts before finishing to push, doesn't retract block it pushed

 - lever:
	- stronly powers block it is placed on, weakly powers ajd blocks
	- no delay

 - dust:
	- outputs signal it receives minus one
	- weakdy powers blocks it is connected to
	- always connects itself with adj red dusts and repeaters, otherwise connects if in line with other side,
				i.e. dust-dust-block -> middle dust connects to block
	- no delay

 - schedule update priority: (lower number updated first)
	* -3 = repeaters with diode in front of them
	* -2 = repeaters turning off
	* -1 = repeaters turning on
			* -0.5 any comparator update (supposed to be -1 but then priority issues)
	*  0 = redstone torches, redstone lamps turning off
*/

// ************************************************************************** //
//                                                                            //
// ************************************************************************** //

/**
 * @brief levers and redstone torches behave differently towards their
 * attachement block than towards other adj blocks, this fun allows to
 * get the attachement block's dir relative to componant's pos
 * @param value data_value of componant to get attached dir from
 * @return glm::ivec3 dir of block it is attached to, or {0, 0, 0} if not an attached componant
*/
glm::ivec3 Chunk::getAttachedDir( int value )
{
	glm::ivec3 target(0);
	int orientation = (value >> 9) & 0x7;
	int placement = (value >> 12) & 0x3;
	switch (value & 0xFF) {
		case blocks::LEVER:
		case blocks::STONE_BUTTON:
		case blocks::OAK_BUTTON:
			switch (placement) {
				case PLACEMENT::CEILING:
					target = glm::ivec3(0, 0, 1);
					break ;
				case PLACEMENT::FLOOR:
					target = glm::ivec3(0, 0, -1);
					break ;
				case PLACEMENT::WALL:
					switch (orientation) {
						case face_dir::MINUSX:
							target = glm::ivec3(1, 0, 0);
							break ;
						case face_dir::PLUSX:
							target = glm::ivec3(-1, 0, 0);
							break ;
						case face_dir::MINUSY:
							target = glm::ivec3(0, 1, 0);
							break ;
						case face_dir::PLUSY:
							target = glm::ivec3(0, -1, 0);
							break ;
					}
					break ;
			}
			break ;
		case blocks::TORCH:
		case blocks::REDSTONE_TORCH:
			switch (orientation) {
				case face_dir::MINUSX:
					target = glm::ivec3(-1, 0, 0);
					break ;
				case face_dir::PLUSX:
					target = glm::ivec3(1, 0, 0);
					break ;
				case face_dir::MINUSY:
					target = glm::ivec3(0, -1, 0);
					break ;
				case face_dir::PLUSY:
					target = glm::ivec3(0, 1, 0);
					break ;
				case face_dir::MINUSZ:
					target = glm::ivec3(0, 0, -1);
					break ;
			}
			break ;
		case blocks::REDSTONE_DUST:
		case blocks::REPEATER:
		case blocks::COMPARATOR:
		case blocks::POPPY:
		case blocks::DANDELION:
		case blocks::CORNFLOWER:
		case blocks::DEAD_BUSH:
		case blocks::ALLIUM:
		case blocks::PINK_TULIP:
		case blocks::BLUE_ORCHID:
		case blocks::SUGAR_CANE:
		case blocks::CACTUS:
		case blocks::GRASS:
			target = glm::ivec3(0, 0, -1);
			break ;
	}
	return target;
}

/**
 * @brief check adj blocks to return whether block at given pos is currently weakdy powered.
 * @param pos block to get state of
 * @param except this given dir is not checked
 * @param state this is state of 'exept' dir, if on, no need to check further
 * @return if given pos is WEAKDY powered (i.e. by dust connected to it or above it)
*/
bool Chunk::getWeakdyState( glm::ivec3 pos, glm::ivec3 except, bool state )
{
	if (state == REDSTONE::ON) {
		return (state);
	}

	for (int index = 0; index < 6; ++index) {
		const glm::ivec3 delta = adj_blocks[index];
		if (index == face_dir::MINUSZ || delta == except) continue ; // we don't check below

		int adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z);
		// std::cout << "\tchecking " << s_blocks[adj & 0xFF]->name << " at " << pos.x + delta.x << ", " << pos.y + delta.y << ", " << pos.z + delta.z << std::endl;	
		if ((adj & 0xFF) == blocks::REDSTONE_DUST && (adj & REDSTONE::STRENGTH)
				&& ((delta.x == 1 && (adj & (REDSTONE::DUST_SIDE << REDSTONE::DUST_MX)))
				|| (delta.x == -1 && (adj & (REDSTONE::DUST_SIDE << REDSTONE::DUST_PX)))
				|| (delta.y == 1 && (adj & (REDSTONE::DUST_SIDE << REDSTONE::DUST_MY)))
				|| (delta.y == -1 && (adj & (REDSTONE::DUST_SIDE << REDSTONE::DUST_PY)))
				|| delta.z == 1
				)) { // red dust gives signal only to its connected blocks
			return (REDSTONE::ON);
		}
	}
	return (REDSTONE::OFF);
}

/**
 * @brief get exact signal [0:15] pos outputs towards target diode
 * this is only called by comparators and repeaters
 * @param pos block to get output from
 * @param target dir the output is going to
 * @param side if true, only take input from comparator, dust, repeater, and redstone block
 * @param repeater if true, fun called by repeater, if side and repeater, only take input from comparator and repeater
 * @return signal in range [0:15] received by target diode from pos
*/
int Chunk::getRedstoneSignalTarget( glm::ivec3 pos, glm::ivec3 target, bool side, bool repeater )
{
	int adj = getBlockAt(pos.x, pos.y, pos.z);
	// std::cout << "\tcompChecking " << s_blocks[adj & 0xFF]->name << " at " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;	
	switch (adj & 0xFF) {
		case blocks::REPEATER:
			if (target == adj_blocks[(adj >> 9) & 0x7] && (adj & REDSTONE::POWERED)) {
				return (0xF);
			}
			break ;
		case blocks::COMPARATOR:
			if (target == adj_blocks[(adj >> 9) & 0x7]) {
				return ((adj >> REDSTONE::STRENGTH_OFFSET) & 0xF);
			}
			break ;
		case blocks::REDSTONE_TORCH:
				if (!side && !(adj & REDSTONE::POWERED)) {
					return (0xF);
				}
				break ;
		case blocks::REDSTONE_DUST:
			return ((repeater & side) ? 0 : (adj >> REDSTONE::STRENGTH_OFFSET) & 0xF);
		case blocks::REDSTONE_BLOCK:
			return ((repeater & side) ? 0 : 0xF);
		default:
			if (!side && (adj & (REDSTONE::POWERED | REDSTONE::WEAKDY_POWERED))) {
				return (0xF);
			}
	}
	return (REDSTONE::OFF);
}

/**
 * @brief check adj blocks to return whether block at given pos is currently powered.
 * @param pos block to get state of
 * @param except this given dir is not checked
 * @param state this is state of 'exept' dir, if on, no need to check further
 * @param weak true if a weakly powered state is accepted
 * @return if given pos is ((weak) ? WEAKLY : STRONGLY | WEAKLY) powered
 * if strongly powered, return signal level
*/
int Chunk::getRedstoneStrength( glm::ivec3 pos, glm::ivec3 except, int state, bool weak )
{
	if (state == 0xF) {
		return (0xF);
	}

	for (int index = 0; index < 6; ++index) {
		const glm::ivec3 delta = adj_blocks[index];
		if (delta == except) continue ;

		int adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z);
		// std::cout << "\tchecking " << s_blocks[adj & 0xFF]->name << " at " << pos.x + delta.x << ", " << pos.y + delta.y << ", " << pos.z + delta.z << std::endl;	
		switch (adj & 0xFF) {
			case blocks::LEVER:
				if (adj & REDSTONE::POWERED) {
					if (getAttachedDir(adj) == -delta || weak) {
						return (0xF);
					}
				}
				break ;
			case blocks::REPEATER:
				if (delta == -adj_blocks[(adj >> 9) & 0x7] && (adj & REDSTONE::POWERED)) {
					return (0xF);
				}
				break ;
			case blocks::COMPARATOR:
				if (delta == -adj_blocks[(adj >> 9) & 0x7] && (adj & REDSTONE::POWERED)) {
					state = glm::max(state, ((adj >> REDSTONE::STRENGTH_OFFSET) & 0xF));
				}
				break ;
			case blocks::REDSTONE_TORCH:
				if (!(adj & REDSTONE::POWERED) && (delta.z == -1 || (weak && delta != -getAttachedDir(adj)))) {
					return (0xF);
				}
				break ;
			case blocks::REDSTONE_DUST:
				if (weak && (adj & REDSTONE::STRENGTH)
					&& ((delta.x == 1 && (adj & (REDSTONE::DUST_CONNECT << REDSTONE::DUST_MX)))
					|| (delta.x == -1 && (adj & (REDSTONE::DUST_CONNECT << REDSTONE::DUST_PX)))
					|| (delta.y == 1 && (adj & (REDSTONE::DUST_CONNECT << REDSTONE::DUST_MY)))
					|| (delta.y == -1 && (adj & (REDSTONE::DUST_CONNECT << REDSTONE::DUST_PY)))
					|| delta.z == 1
					)) { // red dust gives signal only to its connected blocks and to the block below
					return (REDSTONE::ON);
				}
				break ;
			case blocks::STONE_BUTTON:
			case blocks::OAK_BUTTON:
				if (adj & REDSTONE::POWERED) {
					return (0xF);
				}
				break ;
			default: // redstone block behaves like default
				if (weak && (adj & (REDSTONE::POWERED | REDSTONE::WEAKDY_POWERED))) {
					return (0xF);
				}
				break ;
		}
	}
	return (state);
}

/**
 * @brief same as getRedstoneState, but returns signal level [0:15] received by dust at 'pos'
 * and doesn't distinguish between strong or weak signal, ignores weakdy signal, only called by red dust
 * if signal comes from other dust, it is reduced by one
 * @param pos dust to get signal strength of
 * @return signal level received by dust at 'pos'
*/
int Chunk::getDustStrength( glm::ivec3 pos )
{
	bool solidAbove = !s_blocks[getBlockAt(pos.x, pos.y, pos.z + 1) & 0xFF]->transparent;
	int res = REDSTONE::OFF;
	for (int index = 0; index < 6; ++index) {
		const glm::ivec3 delta = adj_blocks[index];
		int adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z);
		switch (adj & 0xFF) {
			case blocks::REDSTONE_TORCH:
				if (!(adj & REDSTONE::POWERED)) {
					return (0xF);
				}
				break ;
			case blocks::REPEATER:
				if (delta == -adj_blocks[(adj >> 9) & 0x7] && (adj & REDSTONE::POWERED)) {
					return (0xF);
				}
				break ;
			case blocks::COMPARATOR:
				if (delta == -adj_blocks[(adj >> 9) & 0x7] && (adj & REDSTONE::POWERED)) {
					if (((adj >> REDSTONE::STRENGTH_OFFSET) & 0xF) > res) {
						res = ((adj >> REDSTONE::STRENGTH_OFFSET) & 0xF);
					}
				}
				break ;
			case blocks::REDSTONE_DUST:
				if (((adj >> REDSTONE::STRENGTH_OFFSET) & 0xF) > res + 1) {
					res = ((adj >> REDSTONE::STRENGTH_OFFSET) & 0xF) - 1;
				}
				break ;
			case blocks::LEVER:
			case blocks::REDSTONE_BLOCK:
				if (adj & REDSTONE::POWERED) {
					return (0xF);
				}
			default:
				if (adj & REDSTONE::POWERED) {
					return ((adj >> REDSTONE::STRENGTH_OFFSET) & 0xF);
				}
				break ;
		}
		if (delta.z) {
			continue ;
		} 
		if (s_blocks[adj & 0xFF]->transparent) {
			adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z - 1);
			if ((adj & 0xFF) == blocks::REDSTONE_DUST && (((adj >> REDSTONE::STRENGTH_OFFSET) & 0xF) > res + 1)) {
				res = ((adj >> REDSTONE::STRENGTH_OFFSET) & 0xF) - 1;
			}
		} else {
			if (solidAbove) {
				continue ;
			}
			adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z + 1);
			if ((adj & 0xFF) == blocks::REDSTONE_DUST && (((adj >> REDSTONE::STRENGTH_OFFSET) & 0xF) > res + 1)) {
				res = ((adj >> REDSTONE::STRENGTH_OFFSET) & 0xF) - 1;
			}
		}
	}
	return (res);
}

/**
 * @brief strongly update block at given pos, and weakly update its adj blocks
 * @param pos source of power
 * @param source this given dir is not weakly updated, as it is source of power
 * @param state whether strong power is turned on or off - strength level of source block
*/
void Chunk::stronglyPower( glm::ivec3 pos, glm::ivec3 source, int state )
{
	int value = getBlockAt(pos.x, pos.y, pos.z);
	// std::cout << "strongly power block " << s_blocks[value & 0xFF]->name << " at " << pos.x << ", " << pos.y << ", " << pos.z << ": " << state << std::endl;
	if (!s_blocks[value & 0xFF]->transparent && (value & 0xFF) != blocks::REDSTONE_BLOCK) {
		int strongly_powered = getRedstoneStrength(pos, source, state, false);
		// std::cout << "\tactual state is " << strongly_powered << std::endl;
		value &= (REDSTONE::ALL_BITS - REDSTONE::POWERED - REDSTONE::STRENGTH);
		if (strongly_powered) { value |= REDSTONE::POWERED | (strongly_powered << REDSTONE::STRENGTH_OFFSET); }
		bool weakdy_powered = getWeakdyState(pos, {0, 0, 0}, state);
		(weakdy_powered) ? value |= REDSTONE::WEAKDY_POWERED : value &= (REDSTONE::ALL_BITS - REDSTONE::WEAKDY_POWERED);
		setBlockAt(value, pos.x, pos.y, pos.z, false);
		if (strongly_powered) {
			weaklyPower(pos, source, REDSTONE::ON, false);
		} else if (weakdy_powered) {
			weaklyPower(pos, source, REDSTONE::ON, true);
			for (int index = 0; index < 6; ++index) { // still power off adj redstone dusts
				const glm::ivec3 delta = adj_blocks[index];
				if (delta == source) continue ;
				int adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z);
				if ((adj & 0xFF) == blocks::REDSTONE_DUST) {
					updateRedstoneDust(pos + delta);
				}
			}
		} else {
			weaklyPower(pos, source, REDSTONE::OFF, false);
		}
	}
}

/**
 * @brief check block at given pos, and update redstone componants if found
 * @param pos targeted block
 * @param source dir of block calling this function
 * @param state whether weak power is turned on or off
 * @param weakdy whether power comes from weakdy powered block, in which case dust is not updated
*/
void Chunk::weaklyPowerTarget( glm::ivec3 pos, glm::ivec3 source, bool state, bool weakdy )
{
	int adj = getBlockAt(pos.x, pos.y, pos.z), src;
	switch (adj & 0xFF) {
		case blocks::REDSTONE_LAMP:
			if (state && !(adj & REDSTONE::ACTIVATED)) {
				// std::cout << "\tactivate redstone lamp at " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
				// turn lamp on instantly
				short level = getLightLevel(pos.x, pos.y, pos.z) & 0xFF00;
				level += s_blocks[blocks::REDSTONE_LAMP]->light_level + (s_blocks[blocks::REDSTONE_LAMP]->light_level << 4);
				setLightLevel(level, pos.x, pos.y, pos.z, true);
				startLightSpread(pos.x, pos.y, pos.z, false);
				setBlockAt(adj | REDSTONE::ACTIVATED, pos.x, pos.y, pos.z, false);
			} else if (!state && (adj & REDSTONE::ACTIVATED)) {
				// add lamp to tick delay machine
				// std::cout << "\tschedule redstone lamp at " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
				scheduleRedstoneTick({pos, 2 * REDSTONE::TICK});
				// state = getRedstoneStrength(pos, {0, 0, 0}, REDSTONE::OFF, true);
				// 		if (!state && (adj & REDSTONE::ACTIVATED)) { // turn it off
				// 			short level = getLightLevel(pos.x, pos.y, pos.z) & 0xFF0F;
				// 			setLightLevel(level, pos.x, pos.y, pos.z, true);
				// 			startLightSpread(pos.x, pos.y, pos.z, false);
				// 			setBlockAt(adj & (REDSTONE::ALL_BITS - REDSTONE::ACTIVATED), pos.x, pos.y, pos.z);
				// 		}
			}
			break ;
		case blocks::REPEATER:
			if (source == -adj_blocks[(adj >> 9) & 0x7]) { // signal comes from behind repeater
				// std::cout << "scheduling repeater from " << source.x << ", " << source.y << ", " << source.z << std::endl;
				scheduleRedstoneTick({pos, REDSTONE::TICK * (((adj >> REDSTONE::REPEAT_TICKS) & 0x3) + 1), state});
			} else if (!source.x == !!adj_blocks[(adj >> 9) & 0x7].x && !source.y == !!adj_blocks[(adj >> 9) & 0x7].y) { // signal comes from side
				// we check if it comes from repeater and (un)lock the repeater
				src = getBlockAt(pos.x + source.x, pos.y + source.y, pos.z + source.z);
				if (((src & 0xFF) == blocks::REPEATER || (src & 0xFF) == blocks::COMPARATOR) && source == -adj_blocks[(src >> 9) & 0x7]) {
					if ((src & REDSTONE::POWERED) && !(adj & REDSTONE::REPEAT_LOCK)) { // if not locked but should be, we lock it
						setBlockAt(adj | REDSTONE::REPEAT_LOCK, pos.x, pos.y, pos.z, false);
					} else if (!(src & REDSTONE::POWERED) && (adj & REDSTONE::REPEAT_LOCK)) { // if locked but shouldn't be, we unlock it
						adj &= (REDSTONE::ALL_BITS - REDSTONE::REPEAT_LOCK);
						initRepeater(pos, adj, false);
						setBlockAt(adj, pos.x, pos.y, pos.z, false);
					}
				}
			}
			break ;
		case blocks::COMPARATOR:
			if (source.z || source == adj_blocks[(adj >> 9) & 0x7]) { // signal update come from up/down, front -> we discard
				break ;
			}
			updateComparator(pos, adj, false);
			break ;
		case blocks::PISTON:
		case blocks::STICKY_PISTON:
			if (source == adj_blocks[(adj >> 9) & 0x7]) {
				break ;
			}
			updatePiston(pos, adj);
			break ;
		case blocks::REDSTONE_TORCH:
			if (getAttachedDir(adj) == source) {
				scheduleRedstoneTick({pos, REDSTONE::TICK});
			}
			break ;
		case blocks::REDSTONE_DUST:
			if (weakdy) {
				break ;
			}
			if (!state && !(adj & REDSTONE::STRENGTH)) { // we tell turned off dust to turn off, no update
				break ;
			}
			updateRedstoneDust(pos);
			break ;
		case blocks::TNT:
			if (state == REDSTONE::ON) {
				handleHit(false, blocks::TNT, pos + glm::ivec3(_startX, _startY, 0), Modif::LITNT);
			}
			break ;
	}
	if (weakdy) {
		return ;
	}
	// quasi-connectivity
	int below = getBlockAt(pos.x, pos.y, pos.z - 1, true);
	if ((below & 0xFF) == blocks::PISTON || (below & 0xFF) == blocks::STICKY_PISTON) {
		std::cout << "\t\tquasi-connectivity at " << _startX + pos.x << ", " << _startY + pos.y << ", " << pos.z - 1 << std::endl;
		updatePiston(pos + glm::ivec3(0, 0, -1), below);
	}
}

/**
 * @brief check adj blocks to given pos, and update redstone componants if found
 * @param pos source of power
 * @param except this given dir is not checked
 * @param state whether weak power is turned on or off
 * @param weakdy whether power comes from weakdy powered block, in which case dust is not updated
*/
void Chunk::weaklyPower( glm::ivec3 pos, glm::ivec3 except, bool state, bool weakdy )
{
	for (int index = 0; index < 6; ++index) {
		const glm::ivec3 delta = adj_blocks[index];
		if (delta == except) continue ;

		weaklyPowerTarget(pos + delta, -delta, state, weakdy);
	}
}


	// 			switch (s_blocks[block_value & 0xFF]->geometry) {
	// 				case GEOMETRY::TRAPDOOR:
	// 					(state) ? setBlockAt(block_value | REDSTONE::POWERED, pos.x + dx, pos.y + dy, pos.z + dz)
	// 							: setBlockAt(block_value ^ (1 << REDSTONE::POWERED_OFFSET), pos.x + dx, pos.y + dy, pos.z + dz);
	// 					break ;
	// 				case GEOMETRY::DOOR:
	// 					(state) ? setBlockAt(block_value | REDSTONE::POWERED, pos.x + dx, pos.y + dy, pos.z + dz)
	// 							: setBlockAt(block_value ^ (1 << REDSTONE::POWERED_OFFSET), pos.x + dx, pos.y + dy, pos.z + dz);
	// 					dz += ((block_value >> 12) & DOOR::UPPER_HALF) ? -1 : 1;
	// 					block_value ^= (DOOR::UPPER_HALF << 12);
	// 					(state) ? setBlockAt(block_value | REDSTONE::POWERED, pos.x + dx, pos.y + dy, pos.z + dz)
	// 							: setBlockAt(block_value ^ (1 << REDSTONE::POWERED_OFFSET), pos.x + dx, pos.y + dy, pos.z + dz);
	// 					break ;
	// 			}


/**
 * @brief updates blocks around lever at 'pos' according to its state
 * @param pos relative position of lever inside chunk
 * @param value Data value of block containing lever, used to get its orientation
 * @param state new state of lever
*/
void Chunk::flickLever( glm::ivec3 pos, int value, bool state )
{
std::cout << "flick lever " << state << std::endl;
	// strongly power block the lever is attached to
	glm::ivec3 target = getAttachedDir(value);
	stronglyPower(pos + target, -target, (state) ? 0xF : REDSTONE::OFF);

	// also, adjacent redstone componants are weakly powered
	weaklyPower(pos, {0, 0, 0}, state, false);
std::cout << "END flick lever " << state << std::endl;
}

/**
 * @brief updates blocks around redstone torch at 'pos' according to its attachement block.
 * Behaves similarily to flip Lever, but we always strongly power block above and never
 * power attachement block
 * @param pos relative position of redstone torch inside chunk
*/
void Chunk::updateRedstoneTorch( glm::ivec3 pos, int value )
{
	int actual_value = getBlockAt(pos.x, pos.y, pos.z), state;
	if ((actual_value & 0xFF) != blocks::REDSTONE_TORCH) {
		state = true; // if state on, torch off
	} else {
		glm::ivec3 target = getAttachedDir(actual_value);
		state = getBlockAt(pos.x + target.x, pos.y + target.y, pos.z + target.z) & (REDSTONE::POWERED | REDSTONE::WEAKDY_POWERED);
		if (state && !(actual_value & REDSTONE::POWERED)) {
			setBlockAt(actual_value | REDSTONE::POWERED, pos.x, pos.y, pos.z, false);
			short level = getLightLevel(pos.x, pos.y, pos.z) & 0xFF00;
			setLightLevel(level, pos.x, pos.y, pos.z, true);
			startLightSpread(pos.x, pos.y, pos.z, false);
		} else if (!state && (actual_value & REDSTONE::POWERED)) {
			setBlockAt(actual_value & (REDSTONE::ALL_BITS - REDSTONE::POWERED), pos.x, pos.y, pos.z, false);
			short level = getLightLevel(pos.x, pos.y, pos.z) + 7 + (7 << 4);
			setLightLevel(level, pos.x, pos.y, pos.z, true);
			startLightSpread(pos.x, pos.y, pos.z, false);
		}
	}

// std::cout << "addRedstoneTorch " << !state << std::endl;
	// strongly power block directly above torch
	stronglyPower(pos + glm::ivec3(0, 0, 1), {0, 0, -1}, (!state) ? 0xF : REDSTONE::OFF);

	// also, adjacent redstone componants are weakly powered, but not attachement block
	glm::ivec3 attachement = getAttachedDir(value);
	weaklyPower(pos, attachement, !state, false);
// std::cout << "END addRestone " << !state << std::endl;
}

/**
 * @brief updates blocks around redstone dust at 'pos'.
 * @param pos relative position of redstone torch inside chunk
*/
void Chunk::updateRedstoneDust( glm::ivec3 pos )
{
	int actual_value = getBlockAt(pos.x, pos.y, pos.z), strength, signal_received = 0;
// std::cout << "updateRestoneDust at " << pos.x << ", " << pos.y << ", " << pos.z << " connect: " << (actual_value >> REDSTONE::DUST_MY) << std::endl;
	if ((actual_value & 0xFF) != blocks::REDSTONE_DUST) {
		strength = 0;
		actual_value |= (REDSTONE::DUST_CONNECT << REDSTONE::DUST_MX) | (REDSTONE::DUST_CONNECT << REDSTONE::DUST_PX)
				| (REDSTONE::DUST_CONNECT << REDSTONE::DUST_MY) | (REDSTONE::DUST_CONNECT << REDSTONE::DUST_PY);
	} else {
		strength = (actual_value >> REDSTONE::STRENGTH_OFFSET) & 0xF;
		signal_received = getDustStrength(pos);
		// std::cout << "\tupdateRedstoneDust strength " << strength << " vs signal " << signal_received << std::endl;
		if (signal_received != strength) {
			strength = (strength > signal_received) ? 0 : signal_received; // shortcut recursion by cutting signal
			actual_value &= (REDSTONE::ALL_BITS - REDSTONE::STRENGTH);
			actual_value |= (strength << REDSTONE::STRENGTH_OFFSET);
			setBlockAt(actual_value, pos.x, pos.y, pos.z, false);
		} else {
			// std::cout << "END updateRestoneDust " << strength << std::endl;
			if (strength > 1) { // because of shortcut above, we need to restart recurse if adj dust still powered
				for (int index = 0; index < 6; ++index) {
					const glm::ivec3 delta = adj_blocks[index];
					int adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z);
					if ((adj & 0xFF) == blocks::REDSTONE_DUST && !((adj >> REDSTONE::STRENGTH_OFFSET) & 0xF)) {
						updateRedstoneDust(pos + delta);
					}
				}
			}
			return ;
		}
	}

// std::cout << "\tupdateRestoneDust strength " << strength << ", at " << _startX + pos.x << ", " << _startY + pos.y << ", " << pos.z << std::endl;
	// use dust's connection to set/unset connected blocks as WEAKLY_POWERED, which is used by red_torches and red_componants
	if ((actual_value >> REDSTONE::DUST_MX) & REDSTONE::DUST_UP) {
		weaklyPowerTarget(pos + glm::ivec3(-1, 0, 1), {1, 0, -1}, (actual_value >> REDSTONE::STRENGTH_OFFSET) & 0xF, false);
		actual_value = getBlockAt(pos.x, pos.y, pos.z);
	}
	if ((actual_value >> REDSTONE::DUST_MX) & REDSTONE::DUST_SIDE) {
		stronglyPower(pos + glm::ivec3(-1, 0, 0), {1, 0, 0}, REDSTONE::OFF);
		weaklyPowerTarget(pos + glm::ivec3(-1, 0, 0), {1, 0, 0}, (actual_value >> REDSTONE::STRENGTH_OFFSET) & 0xF, false);
		actual_value = getBlockAt(pos.x, pos.y, pos.z);
	}
	if ((actual_value >> REDSTONE::DUST_PX) & REDSTONE::DUST_UP) {
		weaklyPowerTarget(pos + glm::ivec3(1, 0, 1), {-1, 0, -1}, (actual_value >> REDSTONE::STRENGTH_OFFSET) & 0xF, false);
		actual_value = getBlockAt(pos.x, pos.y, pos.z);
	}
	if ((actual_value >> REDSTONE::DUST_PX) & REDSTONE::DUST_SIDE) {
		stronglyPower(pos + glm::ivec3(1, 0, 0), {-1, 0, 0}, REDSTONE::OFF);
		weaklyPowerTarget(pos + glm::ivec3(1, 0, 0), {-1, 0, 0}, (actual_value >> REDSTONE::STRENGTH_OFFSET) & 0xF, false);
		actual_value = getBlockAt(pos.x, pos.y, pos.z);
	}
	if ((actual_value >> REDSTONE::DUST_MY) & REDSTONE::DUST_UP) {
		weaklyPowerTarget(pos + glm::ivec3(0, -1, 1), {0, 1, -1}, (actual_value >> REDSTONE::STRENGTH_OFFSET) & 0xF, false);
		actual_value = getBlockAt(pos.x, pos.y, pos.z);
	}
	if ((actual_value >> REDSTONE::DUST_MY) & REDSTONE::DUST_SIDE) {
		stronglyPower(pos + glm::ivec3(0, -1, 0), {0, 1, 0}, REDSTONE::OFF);
		weaklyPowerTarget(pos + glm::ivec3(0, -1, 0), {0, 1, 0}, (actual_value >> REDSTONE::STRENGTH_OFFSET) & 0xF, false);
		actual_value = getBlockAt(pos.x, pos.y, pos.z);
	}
	if ((actual_value >> REDSTONE::DUST_PY) & REDSTONE::DUST_UP) {
		weaklyPowerTarget(pos + glm::ivec3(0, 1, 1), {0, -1, -1}, (actual_value >> REDSTONE::STRENGTH_OFFSET) & 0xF, false);
		actual_value = getBlockAt(pos.x, pos.y, pos.z);
	}
	if ((actual_value >> REDSTONE::DUST_PY) & REDSTONE::DUST_SIDE) {
		stronglyPower(pos + glm::ivec3(0, 1, 0), {0, -1, 0}, REDSTONE::OFF);
		weaklyPowerTarget(pos + glm::ivec3(0, 1, 0), {0, -1, 0}, (actual_value >> REDSTONE::STRENGTH_OFFSET) & 0xF, false);
		actual_value = getBlockAt(pos.x, pos.y, pos.z);
	}
	stronglyPower(pos + glm::ivec3(0, 0, -1), {0, 0, 1}, REDSTONE::OFF);
	weaklyPowerTarget(pos + glm::ivec3(0, 0, -1), {0, 0, 1}, (actual_value >> REDSTONE::STRENGTH_OFFSET) & 0xF, false);
// std::cout << "END updateRestoneDust " << strength << " at " << _startX + pos.x << ", " << _startY + pos.y << ", " << pos.z << std::endl;

	if (!strength && signal_received) { // turn dust back on if needed despite of the shortcut
		if (getDustStrength(pos)) {
			updateRedstoneDust(pos); // praydge infinite recurse
		}

	}
}

/**
 * @brief set or reset repeater's state.
 * called when placing a repeater or when unlocking a repeater
 * @param pos relative pos of repeater in chunk
 * @param value data_value of initialized repeater
 * @param init means fun is called when repeater is placed, else fun
 * called by lock/unlock update
*/
void Chunk::initRepeater( glm::ivec3 pos, int &value, bool init )
{
	const glm::ivec3 front = adj_blocks[(value >> 9) & 0x7];
	int rear = getRedstoneSignalTarget(pos - front, front, false, true);
	const glm::ivec3 side = {!front.x, !front.y, front.z};
	int left = getRedstoneSignalTarget(pos + side, -side, true, true);
	int right = getRedstoneSignalTarget(pos - side, side, true, true);
	bool lock = left | right;
	if (lock) {
		value |= REDSTONE::REPEAT_LOCK;
	} else {
		if (value & REDSTONE::REPEAT_LOCK) { value &= (REDSTONE::ALL_BITS - REDSTONE::REPEAT_LOCK); }
		if (rear && !(value & REDSTONE::POWERED)) {
			// std::cout << "initRepeater ON" << std::endl;
			scheduleRedstoneTick({pos, REDSTONE::TICK * (((value >> REDSTONE::REPEAT_TICKS) & 0x3) + 1), (init) ? REDSTONE::ON : REDSTONE::ON | REDSTONE::REPEAT_LOCK});
		} else if (!rear && (value & REDSTONE::POWERED)) {
			// std::cout << "initRepeater OFF" << std::endl;
			scheduleRedstoneTick({pos, REDSTONE::TICK * (((value >> REDSTONE::REPEAT_TICKS) & 0x3) + 1), (init) ? REDSTONE::OFF : REDSTONE::OFF | REDSTONE::REPEAT_LOCK});
		}
	}
}

/**
 * @brief compute output of comparator and schedule to update it if diff from current output
 * @param pos relative pos of comparator in chunk
 * @param value current data_value of comparator
 * @param scheduledUpdate if true, fun is called from updateRedstone, and we actually apply change if needed
 * else we schedule tick if needed
*/
void Chunk::updateComparator( glm::ivec3 pos, int value, bool scheduledUpdate )
{
	const glm::ivec3 front = adj_blocks[(value >> 9) & 0x7];
	int rear = getRedstoneSignalTarget(pos - front, front, false, false);
	const glm::ivec3 side = {!front.x, !front.y, front.z};
	int left = getRedstoneSignalTarget(pos + side, -side, true, false);
	int right = getRedstoneSignalTarget(pos - side, side, true, false);

	bool mode = value & REDSTONE::COMPARATOR_MODE;
	int output = (mode == REDSTONE::COMPARE) ? rear * (left <= rear && right <= rear)
											: glm::max(0, rear - glm::max(left, right));
	std::cout << "updateComparator mode " << mode << ": rear " << rear << ", left " << left << ", right " << right << " -> ouput " << output << " vs " << ((value >> REDSTONE::STRENGTH_OFFSET) & 0xF) << std::endl;
	
	if (!scheduledUpdate) {
		if (output != ((value >> REDSTONE::STRENGTH_OFFSET) & 0xF)) {
			return scheduleRedstoneTick({pos, REDSTONE::TICK, output});
		}
		// no change in ouput, we still check for scheduled update, in which case we cancel it
		abortComparatorScheduleTick(pos);
	} else if (output != ((value >> REDSTONE::STRENGTH_OFFSET) & 0xF)) {
		if (output) {
			value &= (REDSTONE::ALL_BITS - REDSTONE::STRENGTH);
			value |= (output << REDSTONE::STRENGTH_OFFSET);
			setBlockAt(value | (REDSTONE::POWERED), pos.x, pos.y, pos.z, false);
			stronglyPower(pos + front, -front, output);
			weaklyPowerTarget(pos + front, -front, REDSTONE::ON, false);
		} else {
			setBlockAt(value & (REDSTONE::ALL_BITS - REDSTONE::POWERED - REDSTONE::STRENGTH), pos.x, pos.y, pos.z, false);
			stronglyPower(pos + front, -front, REDSTONE::OFF);
			weaklyPowerTarget(pos + front, -front, REDSTONE::OFF, false);
		}
	}
}

/**
 * @brief counts how many blocks piston has to move in order to extend
 * @param pos relative pos of piston
 * @param value data_value of piston
 * @return nbr of blocks to be moved, stop counting at 13
*/
int Chunk::pistonExtendCount( glm::ivec3 pos, int value )
{
	const glm::ivec3 front = adj_blocks[(value >> 9) & 0x7];
	int count = 0;
	for (int i = 0; i < 13; ++i) {
		int adj = getBlockAt(pos.x + front.x * (i + 1), pos.y + front.y * (i + 1), pos.z + front.z * (i + 1));
		std::cout << "counting " << s_blocks[adj & 0xFF]->name << " at " << pos.x + front.x * (i + 1) << ", " << pos.y + front.y * (i + 1) << ", " << pos.z + front.z * (i + 1) << std::endl;
		switch (s_blocks[adj & 0xFF]->geometry) {
			case GEOMETRY::NONE:
				if ((adj & 0xFF) == blocks::CHEST) {
					return (13);
				}
			case GEOMETRY::CROSS:
				if ((adj & 0xFF) == blocks::PISTON_HEAD) {
					return (13);
				}
			case GEOMETRY::CROP:
			case GEOMETRY::TORCH:
			case GEOMETRY::BUTTON:
			case GEOMETRY::LEVER:
			case GEOMETRY::DUST:
			case GEOMETRY::REPEATER:
				return (count);
			case GEOMETRY::PISTON: // can't move extended piston nor blocks being moved
				 if ((adj & 0xFF) == blocks::MOVING_PISTON) {
					return (13);
				}
				if (adj & REDSTONE::ACTIVATED) {
					return (13);
				}
				break ;
			case GEOMETRY::CUBE:
				switch (adj & 0xFF) {
					case blocks::BEDROCK:
					case blocks::FURNACE:
						return (13);
				}
				break ;
		}
		++count;
	}
	return (count);
}

/**
 * @brief called once we know piston can be moved << update piston targeted blocks.
 * Transform all moved blocks into entities and replace their world value with blocks::MOVING_PISTON
 * after 2 ticks the MOVING_PISTON will be replaced by the entities
 * @param pos relative pos of piston
 * @param value data_value of piston
 * @param count nbr of blocks impacted by movement
*/
void Chunk::extendPiston( glm::ivec3 pos, int value, int count )
{
	const glm::ivec3 front = adj_blocks[(value >> 9) & 0x7];
	std::cout << "extend piston, front is " << front.x << ", " << front.y << ", " << front.z << std::endl;

	for (int i = count - 1; i >= 0; --i) {
		int adj = getBlockAt(pos.x + front.x * (i + 1), pos.y + front.y * (i + 1), pos.z + front.z * (i + 1));
		// creates moving entity for the blocks being pushed
		std::cout << s_blocks[adj & 0xFF]->name << " turned into entity at " << pos.x + front.x * (i + 1) << ", " << pos.y + front.y * (i + 1) << ", " << pos.z + front.z * (i + 1) << std::endl;
		_entities.push_back(new MovingPistonEntity(this, pos, pos + front * (i + 1), front, false, false, adj));
		setBlockAt(blocks::MOVING_PISTON, pos.x + front.x * (i + 1), pos.y + front.y * (i + 1), pos.z + front.z * (i + 1), true);
		// TODO block update neighbours
	}
	// this one is for the piston head
	std::cout << "piston head at " << _startX + pos.x << ", " << _startY + pos.y << ", " << pos.z << std::endl;
	_entities.push_back(new MovingPistonEntity(this, pos, pos, front, false, false, blocks::PISTON_HEAD | (value & (0x7 << 9)) | (((value & 0xFF) == blocks::STICKY_PISTON) ? REDSTONE::STICKY : 0)));
}

/**
 * @brief retract piston at given pos, first check if piston finished its extension,
 * if not place entities being moved at their final pos and retract piston head
 * (this allows sticky_piston to not retract blocks if updated within 1 redstone tick)
*/
void Chunk::retractPiston( glm::ivec3 pos, int value )
{
	const glm::ivec3 front = adj_blocks[(value >> 9) & 0x7];
	int front_value = getBlockAt(pos.x + front.x, pos.y + front.y, pos.z + front.z);
	size_t delCount = 0;
	if ((front_value & 0xFF) == blocks::MOVING_PISTON) { // force finish extension
		size_t eSize = _entities.size();
		for (size_t index = 0; index < eSize; ++index) {
			Entity *entity = _entities[index - delCount];
			if (entity->pistonedBy(pos)) { // if true, pistonedBy places back the moving blocks
				std::cout << "FORCE FINISH EXTENSION" << std::endl;
				delete entity;
				_entities.erase(_entities.begin() + index - delCount);
				++delCount;
			}
		}
	}
	if ((value & 0xFF) == blocks::STICKY_PISTON && delCount < 2) { // retract block only if had time to finish push or didn't push block in the first place
		int front2_value = getBlockAt(pos.x + front.x * 2, pos.y + front.y * 2, pos.z + front.z * 2);
		if ((!s_blocks[front2_value & 0xFF]->transparent && (front2_value & 0xFF) != blocks::MOVING_PISTON)
			|| (((front2_value & 0xFF) == blocks::PISTON || (front2_value & 0xFF) == blocks::STICKY_PISTON) && !(front2_value & REDSTONE::ACTIVATED))) {
			setBlockAt(blocks::MOVING_PISTON, pos + front * 2, true);
			_entities.push_back(new MovingPistonEntity(this, pos, pos + front * 2, -front, false, true, front2_value));
		}
	}
	setBlockAt(blocks::MOVING_PISTON, pos + front, true);
	_entities.push_back(new MovingPistonEntity(this, pos, pos + front, -front, true, true, blocks::PISTON_HEAD | (value & (0x7 << 9))| (((value & 0xFF) == blocks::STICKY_PISTON) ? REDSTONE::STICKY : 0)));
}

/**
 * @brief update piston at given pos, if it is activated, count blocks it pushes and if > 12 abort.
 * calls neighbours to always use the correct _entities vector
 * @param pos relative pos of piston being updated
 * @param value data_value of piston being updated
*/
void Chunk::updatePiston( glm::ivec3 pos, int value )
{
	if (pos.x < 0) {
		if (_neighbours[face_dir::MINUSX]) {
			pos.x += CHUNK_SIZE;
			_neighbours[face_dir::MINUSX]->updatePiston(pos, value);
		}
		return ;
	} else if (pos.x >= CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSX]) {
			pos.x -= CHUNK_SIZE;
			_neighbours[face_dir::PLUSX]->updatePiston(pos, value);
		}
		return ;
	} else if (pos.y < 0) {
		if (_neighbours[face_dir::MINUSY]) {
			pos.y += CHUNK_SIZE;
			_neighbours[face_dir::MINUSY]->updatePiston(pos, value);
		}
		return ;
	} else if (pos.y >= CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSY]) {
			pos.y -= CHUNK_SIZE;
			_neighbours[face_dir::PLUSY]->updatePiston(pos, value);
		}
		return ;
	}

	const glm::ivec3 front = adj_blocks[(value >> 9) & 0x7];
	bool powered = getRedstoneStrength(pos, front, REDSTONE::OFF, true);
	if (!powered && front.z != 1) {
		powered = getRedstoneStrength(pos + glm::ivec3(0, 0, 1), {0, 0, -1}, REDSTONE::OFF, true);
	} // TODO pison can be powered through their extended head
	std::cout << "piston update " << _startX + pos.x << ", " << _startY + pos.y << ", " << pos.z << " powered: " << powered << std::endl;
	if (powered && !(value & REDSTONE::ACTIVATED)) {
		// check if extension possible
		int count = pistonExtendCount(pos, value);
	std::cout << "count is " << count << std::endl;
		if (count <= 12) {
			setBlockAt(value | REDSTONE::ACTIVATED, pos.x, pos.y, pos.z, false);
			extendPiston(pos, value, count);
		}
	} else if (!powered && (value & REDSTONE::ACTIVATED)) {
		setBlockAt(value & (REDSTONE::ALL_BITS - REDSTONE::ACTIVATED), pos.x, pos.y, pos.z, false);
		retractPiston(pos, value);
	} else {
		std::cout << "\t.. no change" << std::endl;
	}
}

/**
 * @brief loop through _redstone_schedule and reduce ticks by 1,
 * if ticks reach 0, scheduled redstone componant is updated
*/
void Chunk::updateRedstone( void )
{
	std::array<size_t, SCHEDULE::SIZE> sizes = {_redstone_schedule[SCHEDULE::REPEAT_DIODE].size(), _redstone_schedule[SCHEDULE::REPEAT_OFF].size(), _redstone_schedule[SCHEDULE::REPEAT_ON].size(), _redstone_schedule[SCHEDULE::COMPARATOR].size(), _redstone_schedule[SCHEDULE::OTHER].size()};
	for (int schedIndex = 0; schedIndex < SCHEDULE::SIZE; ++schedIndex) {
		if (!sizes[schedIndex]) {
			continue ;
		}
		// std::cout << "[" << DayCycle::Get()->getGameTicks() << "] --ticks on updateRedstone priority " << (-3 + schedIndex) << std::endl;

		size_t delCount = 0;
		int state;
		glm::ivec3 front;
		for (size_t index = 0; index < sizes[schedIndex]; ++index) {
			t_redstoneTick &redRef = _redstone_schedule[schedIndex][index - delCount];
			if (--redRef.ticks == 0) {
				t_redstoneTick red = redRef;
				int value = getBlockAt(red.pos.x, red.pos.y, red.pos.z);
				std::cout << "(" << (-3 + schedIndex) <<  ") [" << DayCycle::Get()->getGameTicks() << "] " << _startX << " " << _startY << " schedule " << s_blocks[value & 0xFF]->name << " at " << _startX + red.pos.x << ", " << _startY + red.pos.y << ", " << red.pos.z << ": " << (red.state & 0xF) << std::endl;

				switch (value & 0xFF) {
					case blocks::REDSTONE_LAMP: // only scheduled to be turned off
						// std::cout << "update redstone lamp at " << red.pos.x << ", " << red.pos.y << ", " << red.pos.z << std::endl;
						state = getRedstoneStrength(red.pos, {0, 0, 0}, REDSTONE::OFF, true);
						if (!state && (value & REDSTONE::ACTIVATED)) { // turn it off
							short level = getLightLevel(red.pos.x, red.pos.y, red.pos.z) & 0xFF0F;
							setLightLevel(level, red.pos.x, red.pos.y, red.pos.z, true);
							startLightSpread(red.pos.x, red.pos.y, red.pos.z, false);
							setBlockAt(value & (REDSTONE::ALL_BITS - REDSTONE::ACTIVATED), red.pos.x, red.pos.y, red.pos.z, false);
						}
						break ;
					case blocks::REPEATER:
						if (value & REDSTONE::REPEAT_LOCK) { // repeater is locked, we don't update it
							// std::cout << "repeater is locked." << std::endl;
							break ;
						}
						front = adj_blocks[(value >> 9) & 0x7];
						red.state &= 0xF; // get rid of info 'scheduled tick caused by lock/unlock'
						if (red.state) {
							for (auto &schedule : _redstone_schedule[(schedIndex == SCHEDULE::REPEAT_ON) ? SCHEDULE::REPEAT_OFF : SCHEDULE::REPEAT_DIODE]) {
								// loop through schedule, if repeater scheduled to turn off, extend its delay
								if (schedule.pos == red.pos && schedule.state == REDSTONE::OFF) {
									// std::cout << "extending delay of scheduled repeater to " << (((value >> REDSTONE::REPEAT_TICKS) & 0x3) + 1) * REDSTONE::TICK << std::endl;
									schedule.ticks = (((value >> REDSTONE::REPEAT_TICKS) & 0x3) + 1) * REDSTONE::TICK;
								}
							}
							if (!(value & REDSTONE::POWERED)) { // turn repeater on
								// std::cout << "repeater on" << std::endl;
								setBlockAt(value | REDSTONE::POWERED, red.pos.x, red.pos.y, red.pos.z, false);
								stronglyPower(red.pos + front, -front, 0xF);
								weaklyPowerTarget(red.pos + front, -front, REDSTONE::ON, false);
							}
						} else if (!red.state && (value & REDSTONE::POWERED)) { // turn repeater off
							// std::cout << "repeater off" << std::endl;
							setBlockAt(value & (REDSTONE::ALL_BITS - REDSTONE::POWERED), red.pos.x, red.pos.y, red.pos.z, false);
							stronglyPower(red.pos + front, -front, REDSTONE::OFF);
							weaklyPowerTarget(red.pos + front, -front, REDSTONE::OFF, false);
						}
						break ;
					case blocks::COMPARATOR:
						updateComparator(red.pos, value, true);
						break ;
					case blocks::REDSTONE_TORCH:
						updateRedstoneTorch(red.pos, value);
						break ;
					case blocks::STONE_BUTTON:
					case blocks::OAK_BUTTON:
						front = getAttachedDir(value);
						setBlockAt(value & (-1 - REDSTONE::POWERED - REDSTONE::STRENGTH), red.pos.x, red.pos.y, red.pos.z, false);
						stronglyPower(red.pos + front, -front, REDSTONE::OFF);
						weaklyPower(red.pos, {0, 0, 0}, REDSTONE::OFF, false);
						break ;
					default:
						std::cout << "Chunk::updateRedstone updated: " << s_blocks[value & 0xFF]->name << std::endl;
						break ;
				}
				std::cout << "\t\t" << s_blocks[value & 0xFF]->name << " schedule over." << std::endl;

				_redstone_schedule[schedIndex].erase(_redstone_schedule[schedIndex].begin() + (index - delCount));
				++delCount;
			}
		}
	}
}

/**
 * @brief push_back redstone schedule in appropriate chunk
 * @param red schedule to be pushed
*/
void Chunk::scheduleRedstoneTick( t_redstoneTick red )
{
	if (red.pos.z < 0 || red.pos.z >= WORLD_HEIGHT) {
		return ;
	}
	if (red.pos.x < 0) {
		if (_neighbours[face_dir::MINUSX]) {
			red.pos.x += CHUNK_SIZE;
			_neighbours[face_dir::MINUSX]->scheduleRedstoneTick(red);
			return ;
		}
	} else if (red.pos.x >= CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSX]) {
			red.pos.x -= CHUNK_SIZE;
			_neighbours[face_dir::PLUSX]->scheduleRedstoneTick(red);
			return ;
		}
	} else if (red.pos.y < 0) {
		if (_neighbours[face_dir::MINUSY]) {
			red.pos.y += CHUNK_SIZE;
			_neighbours[face_dir::MINUSY]->scheduleRedstoneTick(red);
			return ;
		}
	} else if (red.pos.y >= CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSY]) {
			red.pos.y -= CHUNK_SIZE;
			_neighbours[face_dir::PLUSY]->scheduleRedstoneTick(red);
			return ;
		}
	} else {
		int priority = SCHEDULE::OTHER;
		int target = getBlockAt(red.pos.x, red.pos.y, red.pos.z);
		if ((target & 0xFF) == blocks::REPEATER) {
			const glm::vec3 front = adj_blocks[(target >> 9) & 0x7];
			int front_value = getBlockAt(red.pos.x + front.x, red.pos.y + front.y, red.pos.z + front.z);
			priority = ((front_value & 0xFF) == blocks::REPEATER || (front_value & 0xFF) == blocks::COMPARATOR) ? SCHEDULE::REPEAT_DIODE
						: (red.state) ? SCHEDULE::REPEAT_ON : SCHEDULE::REPEAT_OFF;
			int other = (priority == SCHEDULE::REPEAT_DIODE) ? SCHEDULE::REPEAT_DIODE : (priority ^ 0x3);
			for (size_t index = 0; index < _redstone_schedule[other].size(); ++index) {
				t_redstoneTick sched = _redstone_schedule[other][index];
				if (sched.pos == red.pos && sched.ticks == red.ticks) {
					if (sched.state & REDSTONE::REPEAT_LOCK) {
						std::cout << "abort because lock/unlock schedule this frame." << std::endl;
						return ;
					} else {
						std::cout << "[next new schedule:] abort previous instruction of priority (" << (-3 + other) << ") " << sched.state << std::endl;
						_redstone_schedule[other].erase(_redstone_schedule[other].begin() + index);
					}
					break ;
				}
			}
		} else if ((target & 0xFF) == blocks::COMPARATOR) {
			priority = SCHEDULE::COMPARATOR; // comparator always has priority -1 (but I do -0.5f instead to prioretize repeaters)
		}
		std::cout << "new schedule " << s_blocks[target & 0xFF]->name << " (" << (-3 + priority) << ") [" << DayCycle::Get()->getGameTicks() << "] " << _startX << " " << _startY << " pos " << _startX + red.pos.x << ", " << _startY + red.pos.y << ", " << red.pos.z << " ticks: " << red.ticks << " state " << (red.state & 0xF) << ((red.state & REDSTONE::REPEAT_LOCK) ? " lock" : "") << std::endl;
		for (auto &sched : _redstone_schedule[priority]) {
			if (sched.pos == red.pos && sched.ticks == red.ticks) {
				if (sched.state == red.state) {
					std::cout << "aborted." << std::endl;
				} else if ((target & 0xFF) == blocks::COMPARATOR) {
					sched.state = red.state;
					std::cout << "abort previous state." << std::endl;
				}
				return ;
			}
		}
		_redstone_schedule[priority].push_back(red);
	}
}

/**
 * @brief look if comparator at given pos has already been scheduled this tick
 * in which case we abort it
 * @param pos relative pos of comparator
*/
void Chunk::abortComparatorScheduleTick( glm::ivec3 pos )
{
	if (pos.z < 0 || pos.z >= WORLD_HEIGHT) {
		return ;
	}
	if (pos.x < 0) {
		if (_neighbours[face_dir::MINUSX]) {
			pos.x += CHUNK_SIZE;
			_neighbours[face_dir::MINUSX]->abortComparatorScheduleTick(pos);
			return ;
		}
	} else if (pos.x >= CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSX]) {
			pos.x -= CHUNK_SIZE;
			_neighbours[face_dir::PLUSX]->abortComparatorScheduleTick(pos);
			return ;
		}
	} else if (pos.y < 0) {
		if (_neighbours[face_dir::MINUSY]) {
			pos.y += CHUNK_SIZE;
			_neighbours[face_dir::MINUSY]->abortComparatorScheduleTick(pos);
			return ;
		}
	} else if (pos.y >= CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSY]) {
			pos.y -= CHUNK_SIZE;
			_neighbours[face_dir::PLUSY]->abortComparatorScheduleTick(pos);
			return ;
		}
	} else {
		int index = 0;
		for (auto &sched : _redstone_schedule[SCHEDULE::REPEAT_ON]) {
			// std::cout << "abortComparatorScheduleTick checking " << sched.pos.x << ", " << sched.pos.y << ", " << sched.pos.z << " ticks " << sched.ticks << std::endl;
			if (sched.pos == pos && sched.ticks == REDSTONE::TICK) {
				// std::cout << "abort previous comparator schedule " << sched.state << "." << std::endl;
				_redstone_schedule[SCHEDULE::REPEAT_ON].erase(_redstone_schedule[SCHEDULE::REPEAT_ON].begin() + index);
				return ;
			}
			++index;
		}
	}
}

/**
 * @brief called when new red dust is placed, used to connect it to its neighbours.
 * it will connect to adj dust and red componants that read signal (ie repeaters)
 * also modifies adj dusts to connect them to this dust
 * @param pos relative pos of dust in chunk
 * @param value reference to dust value
 * @param placed true if pos is the new dust being placed. used during recursion to avoid loop
*/
void Chunk::connectRedstoneDust( glm::ivec3 pos, int &value, bool placed )
{
	// std::cout << "connectRedstoneDust" << std::endl;
	value &= 0xFFFFFF;
	int connect_mx = 0, connect_px = 0, connect_my = 0, connect_py = 0;
	int value_mx = getBlockAt(pos.x - 1, pos.y, pos.z), tmx = (value_mx & 0xFF);
	if (tmx == blocks::LEVER || tmx == blocks::REDSTONE_DUST || tmx == blocks::REDSTONE_TORCH || tmx == blocks::TARGET || tmx == blocks::PISTON || tmx == blocks::STICKY_PISTON
		|| tmx == blocks::COMPARATOR || (tmx == blocks::REPEATER && ((value_mx >> 10) & 0x1))) {
		connect_mx |= REDSTONE::DUST_SIDE;
		if (placed && tmx == blocks::REDSTONE_DUST && !(value_mx & (REDSTONE::DUST_SIDE << REDSTONE::DUST_PX))) { // adj dust not linked towards new dust
			connectRedstoneDust(pos + glm::ivec3(-1, 0, 0), value_mx, false);
			setBlockAt(value_mx, pos.x - 1, pos.y, pos.z, false);
		}
	} else if (s_blocks[tmx]->transparent) {
		value_mx = getBlockAt(pos.x - 1, pos.y, pos.z - 1);
		if ((value_mx & 0xFF) == blocks::REDSTONE_DUST) {
			connect_mx |= REDSTONE::DUST_SIDE;
			if (placed) {
				connectRedstoneDust(pos + glm::ivec3(-1, 0, -1), value_mx, false);
				setBlockAt(value_mx, pos.x - 1, pos.y, pos.z - 1, false);
			}
		}
		if (tmx == blocks::GLASS) { // hardcoded for now ..
			goto GLASS_MX;
		}
	} else {
GLASS_MX:
		if (s_blocks[getBlockAt(pos.x, pos.y, pos.z + 1) & 0xFF]->transparent) {
			value_mx = getBlockAt(pos.x - 1, pos.y, pos.z + 1);
			if ((value_mx & 0xFF) == blocks::REDSTONE_DUST) {
				connect_mx |= REDSTONE::DUST_UP;
				if (placed) {
					connectRedstoneDust(pos + glm::ivec3(-1, 0, 1), value_mx, false);
					setBlockAt(value_mx, pos.x - 1, pos.y, pos.z + 1, false);
				}
			}
		}
	}

	int value_px = getBlockAt(pos.x + 1, pos.y, pos.z), tpx = (value_px & 0xFF);
	if (tpx == blocks::LEVER || tpx == blocks::REDSTONE_DUST || tpx == blocks::REDSTONE_TORCH || tpx == blocks::TARGET || tpx == blocks::PISTON || tpx == blocks::STICKY_PISTON
		|| tpx == blocks::COMPARATOR || (tpx == blocks::REPEATER && ((value_px >> 10) & 0x1))) {
		connect_px |= REDSTONE::DUST_SIDE;
		if (placed && tpx == blocks::REDSTONE_DUST && !(value_px & (REDSTONE::DUST_SIDE << REDSTONE::DUST_MX))) { // adj dust not linked towards new dust
			connectRedstoneDust(pos + glm::ivec3(1, 0, 0), value_px, false);
			setBlockAt(value_px, pos.x + 1, pos.y, pos.z, false);
		}
	} else if (s_blocks[tpx]->transparent) {
		value_px = getBlockAt(pos.x + 1, pos.y, pos.z - 1);
		if ((value_px & 0xFF) == blocks::REDSTONE_DUST) {
			connect_px |= REDSTONE::DUST_SIDE;
			if (placed) {
				connectRedstoneDust(pos + glm::ivec3(1, 0, -1), value_px, false);
				setBlockAt(value_px, pos.x + 1, pos.y, pos.z - 1, false);
			}
		}
		if (tpx == blocks::GLASS) { // hardcoded for now ..
			goto GLASS_PX;
		}
	} else {
GLASS_PX:
		if (s_blocks[getBlockAt(pos.x, pos.y, pos.z + 1) & 0xFF]->transparent) {
			value_px = getBlockAt(pos.x + 1, pos.y, pos.z + 1);
			if ((value_px & 0xFF) == blocks::REDSTONE_DUST) {
				connect_px |= REDSTONE::DUST_UP;
				if (placed) {
					connectRedstoneDust(pos + glm::ivec3(1, 0, 1), value_px, false);
					setBlockAt(value_px, pos.x + 1, pos.y, pos.z + 1, false);
				}
			}
		}
	}

	int value_my = getBlockAt(pos.x, pos.y - 1, pos.z), tmy = (value_my & 0xFF);
	if (tmy == blocks::LEVER || tmy == blocks::REDSTONE_DUST || tmy == blocks::REDSTONE_TORCH || tmy == blocks::TARGET || tmy == blocks::PISTON || tmy == blocks::STICKY_PISTON
		|| tmy == blocks::COMPARATOR || (tmy == blocks::REPEATER && !((value_my >> 10) & 0x1))) {
		connect_my |= REDSTONE::DUST_SIDE;
		if (placed && tmy == blocks::REDSTONE_DUST && !(value_my & (REDSTONE::DUST_SIDE << REDSTONE::DUST_PY))) { // adj dust not linked towards new dust
			connectRedstoneDust(pos + glm::ivec3(0, -1, 0), value_my, false);
			setBlockAt(value_my, pos.x, pos.y - 1, pos.z, false);
		}
	} else if (s_blocks[tmy]->transparent) {
		value_my = getBlockAt(pos.x, pos.y - 1, pos.z - 1);
		if ((value_my & 0xFF) == blocks::REDSTONE_DUST) {
			connect_my |= REDSTONE::DUST_SIDE;
			if (placed) {
				connectRedstoneDust(pos + glm::ivec3(0, -1, -1), value_my, false);
				setBlockAt(value_my, pos.x, pos.y - 1, pos.z - 1, false);
			}
		}
		if (tmy == blocks::GLASS) { // hardcoded for now ..
			goto GLASS_MY;
		}
	} else {
GLASS_MY:
		if (s_blocks[getBlockAt(pos.x, pos.y, pos.z + 1) & 0xFF]->transparent) {
			value_my = getBlockAt(pos.x, pos.y - 1, pos.z + 1);
			if ((value_my & 0xFF) == blocks::REDSTONE_DUST) {
				connect_my |= REDSTONE::DUST_UP;
				if (placed) {
					connectRedstoneDust(pos + glm::ivec3(0, -1, 1), value_my, false);
					setBlockAt(value_my, pos.x, pos.y - 1, pos.z + 1, false);
				}
			}
		}
	}

	int value_py = getBlockAt(pos.x, pos.y + 1, pos.z), tpy = (value_py & 0xFF);
	if (tpy == blocks::LEVER || tpy == blocks::REDSTONE_DUST || tpy == blocks::REDSTONE_TORCH || tpy == blocks::TARGET || tpy == blocks::PISTON || tpy == blocks::STICKY_PISTON
		|| tpy == blocks::COMPARATOR || (tpy == blocks::REPEATER && !((value_py >> 10) & 0x1))) {
		connect_py |= REDSTONE::DUST_SIDE;
		if (placed && tpy == blocks::REDSTONE_DUST && !(value_py & (REDSTONE::DUST_SIDE << REDSTONE::DUST_MY))) { // adj dust not linked towards new dust
			connectRedstoneDust(pos + glm::ivec3(0, 1, 0), value_py, false);
			setBlockAt(value_py, pos.x, pos.y + 1, pos.z, false);
		}
	} else if (s_blocks[tpy]->transparent) {
		value_py = getBlockAt(pos.x, pos.y + 1, pos.z - 1);
		if ((value_py & 0xFF) == blocks::REDSTONE_DUST) {
			connect_py |= REDSTONE::DUST_SIDE;
			if (placed) {
				connectRedstoneDust(pos + glm::ivec3(0, 1, -1), value_py, false);
				setBlockAt(value_py, pos.x, pos.y + 1, pos.z - 1, false);
			}
		}
		if (tpy == blocks::GLASS) { // hardcoded for now ..
			goto GLASS_PY;
		}
	} else {
GLASS_PY:
		if (s_blocks[getBlockAt(pos.x, pos.y, pos.z + 1) & 0xFF]->transparent) {
			value_py = getBlockAt(pos.x, pos.y + 1, pos.z + 1);
			if ((value_py & 0xFF) == blocks::REDSTONE_DUST) {
				connect_py |= REDSTONE::DUST_UP;
				if (placed) {
					connectRedstoneDust(pos + glm::ivec3(0, 1, 1), value_py, false);
					setBlockAt(value_py, pos.x, pos.y + 1, pos.z + 1, false);
				}
			}
		}
	}

	if (connect_mx) {
		value |= (connect_mx << REDSTONE::DUST_MX);
		if (!(connect_my | connect_py | connect_px)) {
			value |= (REDSTONE::DUST_SIDE << REDSTONE::DUST_PX);
		}
	}
	if (connect_px) {
		value |= (connect_px << REDSTONE::DUST_PX);
		if (!(connect_my | connect_py | connect_mx)) {
			value |= (REDSTONE::DUST_SIDE << REDSTONE::DUST_MX);
		}
	}
	if (connect_my) {
		value |= (connect_my << REDSTONE::DUST_MY);
		if (!(connect_mx | connect_px | connect_py)) {
			value |= (REDSTONE::DUST_SIDE << REDSTONE::DUST_PY);
		}
	}
	if (connect_py) {
		value |= (connect_py << REDSTONE::DUST_PY);
		if (!(connect_mx | connect_px | connect_my)) {
			value |= (REDSTONE::DUST_SIDE << REDSTONE::DUST_MY);
		}
	}
}
