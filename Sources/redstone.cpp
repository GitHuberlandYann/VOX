#include "Chunk.hpp"

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

 - lever:
	- stronly powers block it is placed on, weakly powers ajd blocks
	- no delay

 - dust:
	- outputs signal it receives minus one
	- weakdy powers blocks it is connected to
	- always connects itself with adj red dusts and repeaters, otherwise connects if in line with other side,
				i.e. dust-dust-block -> middle dust connects to block
	- no delay

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

		int adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z, true);
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
 * @brief check adj blocks to return whether block at given pos is currently powered.
 * @param pos block to get state of
 * @param except this given dir is not checked
 * @param state this is state of 'exept' dir, if on, no need to check further
 * @param weak true if a weakly powered state is accepted
 * @return if given pos is ((weak) ? WEAKLY : STRONGLY | WEAKLY) powered
*/
bool Chunk::getRedstoneState( glm::ivec3 pos, glm::ivec3 except, bool state, bool weak )
{
	if (state == REDSTONE::ON) {
		return (state);
	}

	for (int index = 0; index < 6; ++index) {
		const glm::ivec3 delta = adj_blocks[index];
		if (delta == except) continue ;

		int adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z, true);
		// std::cout << "\tchecking " << s_blocks[adj & 0xFF]->name << " at " << pos.x + delta.x << ", " << pos.y + delta.y << ", " << pos.z + delta.z << std::endl;	
		switch (adj & 0xFF) {
			case blocks::LEVER:
				if (adj & REDSTONE::POWERED) {
					if (getAttachedDir(adj) == -delta || weak) {
						return (REDSTONE::ON);
					}
				}
				break ;
			case blocks::REDSTONE_TORCH:
				if (!(adj & REDSTONE::POWERED) && (delta.z == -1 || (weak && delta != -getAttachedDir(adj)))) {
					return (REDSTONE::ON);
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
			default: // redstone block behaves like default
				if (weak && (adj & (REDSTONE::POWERED | REDSTONE::WEAKDY_POWERED))) {
					return (REDSTONE::ON);
				}
				break ;
		}
	}
	return (REDSTONE::OFF);
}

/**
 * @brief same as getRedstoneState, but returns signal level [0:15] received by dust at 'pos'
 * and doesn't distinguish between strong or weak signal, ignores weakdy signal, only called by red dust
 * @param pos dust to get signal strength of
 * @return signal level received by dust at 'pos'
*/
int Chunk::getDustStrength( glm::ivec3 pos )
{
	int res = REDSTONE::OFF;
	for (int index = 0; index < 6; ++index) {
		const glm::ivec3 delta = adj_blocks[index];
		int adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z, true);
		switch (adj & 0xFF) {
			case blocks::REDSTONE_TORCH:
				if (!(adj & REDSTONE::POWERED)) {
					return (0xF);
				}
				break ;
			case blocks::REDSTONE_DUST:
				if (((adj >> REDSTONE::STRENGTH_OFFSET) & 0xF) > res + 1) {
					res = ((adj >> REDSTONE::STRENGTH_OFFSET) & 0xF) - 1;
				}
				break ;
			default: // lever and redstone block behave like default
				if (adj & REDSTONE::POWERED) {
					return (0xF);
				}
				break ;
		}
		if (s_blocks[adj & 0xFF]->transparent) {
			adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z - 1, true);
			if ((adj & 0xFF) == blocks::REDSTONE_DUST && (((adj >> REDSTONE::STRENGTH_OFFSET) & 0xF) > res + 1)) {
				res = ((adj >> REDSTONE::STRENGTH_OFFSET) & 0xF) - 1;
			}
		} else {
			adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z + 1, true);
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
 * @param state whether strong power is turned on or off
*/
void Chunk::stronglyPower( glm::ivec3 pos, glm::ivec3 source, bool state )
{
	int value = getBlockAt(pos.x, pos.y, pos.z, true);
	// std::cout << "strongly power block " << s_blocks[value & 0xFF]->name << " at " << pos.x << ", " << pos.y << ", " << pos.z << ": " << state << std::endl;
	if (!s_blocks[value & 0xFF]->transparent && (value & 0xFF) != blocks::REDSTONE_BLOCK) {
		bool strongly_powered = getRedstoneState(pos, source, state, false);
		// std::cout << "\tactual state is " << strongly_powered << std::endl;
		(strongly_powered) ? value |= REDSTONE::POWERED : value &= (REDSTONE::ALL_BITS - REDSTONE::POWERED);
		bool weakdy_powered = getWeakdyState(pos, {0, 0, 0}, state);
		(weakdy_powered) ? value |= REDSTONE::WEAKDY_POWERED : value &= (REDSTONE::ALL_BITS - REDSTONE::WEAKDY_POWERED);
		setBlockAt(value, pos.x, pos.y, pos.z, true);
		if (strongly_powered) {
			weaklyPower(pos, source, REDSTONE::ON, false);
		} else {
			weaklyPower(pos, source, REDSTONE::OFF, false);
			if (weakdy_powered) {
				weaklyPower(pos, source, REDSTONE::ON, true);
			}
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
	int adj = getBlockAt(pos.x, pos.y, pos.z, true);
	switch (adj & 0xFF) {
		case blocks::REDSTONE_LAMP:
			if (state && !(adj & REDSTONE::ACTIVATED)) {
				// std::cout << "\tactivate redstone lamp at " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
				// turn lamp on instantly
				short level = getLightLevel(pos.x, pos.y, pos.z) & 0xFF00;
				level += s_blocks[blocks::REDSTONE_LAMP]->light_level + (s_blocks[blocks::REDSTONE_LAMP]->light_level << 4);
				setLightLevel(level, pos.x, pos.y, pos.z, true);
				startLightSpread(pos.x, pos.y, pos.z, false);
				setBlockAt(adj | REDSTONE::ACTIVATED, pos.x, pos.y, pos.z, true);
			} else if (!state && (adj & REDSTONE::ACTIVATED)) {
				// add lamp to tick delay machine
				// std::cout << "\tschedule redstone lamp at " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
				_redstone_schedule.push_back({pos, 2 * REDSTONE::TICK});
			}
			break ;
		case blocks::REDSTONE_TORCH:
			if (getAttachedDir(adj) == source) {
				_redstone_schedule.push_back({pos, REDSTONE::TICK});
			}
			break ;
		case blocks::REDSTONE_DUST:
			if (!weakdy) {
				updateRedstoneDust(pos);
			}
			break ;
		case blocks::TNT:
			if (state == REDSTONE::ON) {
				handleHit(false, blocks::TNT, pos + glm::ivec3(_startX, _startY, 0), Modif::LITNT);
			}
			break ;
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
	// 					(state) ? setBlockAt(block_value | REDSTONE::POWERED, pos.x + dx, pos.y + dy, pos.z + dz, true)
	// 							: setBlockAt(block_value ^ (1 << REDSTONE::POWERED_OFFSET), pos.x + dx, pos.y + dy, pos.z + dz, true);
	// 					break ;
	// 				case GEOMETRY::DOOR:
	// 					(state) ? setBlockAt(block_value | REDSTONE::POWERED, pos.x + dx, pos.y + dy, pos.z + dz, true)
	// 							: setBlockAt(block_value ^ (1 << REDSTONE::POWERED_OFFSET), pos.x + dx, pos.y + dy, pos.z + dz, true);
	// 					dz += ((block_value >> 12) & DOOR::UPPER_HALF) ? -1 : 1;
	// 					block_value ^= (DOOR::UPPER_HALF << 12);
	// 					(state) ? setBlockAt(block_value | REDSTONE::POWERED, pos.x + dx, pos.y + dy, pos.z + dz, true)
	// 							: setBlockAt(block_value ^ (1 << REDSTONE::POWERED_OFFSET), pos.x + dx, pos.y + dy, pos.z + dz, true);
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
// std::cout << "flick lever " << state << std::endl;
	// strongly power block the lever is attached to
	glm::ivec3 target = getAttachedDir(value);
	stronglyPower(pos + target, -target, state);

	// also, adjacent redstone componants are weakly powered
	weaklyPower(pos, {0, 0, 0}, state, false);
// std::cout << "END flick lever " << state << std::endl;
}

/**
 * @brief updates blocks around redstone torch at 'pos' according to its attachement block.
 * Behaves similarily to flip Lever, but we always strongly power block above and never
 * power attachement block
 * @param pos relative position of redstone torch inside chunk
*/
void Chunk::updateRedstoneTorch( glm::ivec3 pos, int value )
{
	int actual_value = getBlockAt(pos.x, pos.y, pos.z, true), state;
	if ((actual_value & 0xFF) != blocks::REDSTONE_TORCH) {
		state = true; // if state on, torch off
	} else {
		glm::ivec3 target = getAttachedDir(actual_value);
		state = getBlockAt(pos.x + target.x, pos.y + target.y, pos.z + target.z, true) & (REDSTONE::POWERED | REDSTONE::WEAKDY_POWERED);
		if (state && !(actual_value & REDSTONE::POWERED)) {
			setBlockAt(actual_value | REDSTONE::POWERED, pos.x, pos.y, pos.z, true);
			short level = getLightLevel(pos.x, pos.y, pos.z) & 0xFF00;
			setLightLevel(level, pos.x, pos.y, pos.z, true);
			startLightSpread(pos.x, pos.y, pos.z, false);
		} else if (!state && (actual_value & REDSTONE::POWERED)) {
			setBlockAt(actual_value & (REDSTONE::ALL_BITS - REDSTONE::POWERED), pos.x, pos.y, pos.z, true);
			short level = getLightLevel(pos.x, pos.y, pos.z) + 7 + (7 << 4);
			setLightLevel(level, pos.x, pos.y, pos.z, true);
			startLightSpread(pos.x, pos.y, pos.z, false);
		}
	}

// std::cout << "addRedstoneTorch " << state << std::endl;
	// strongly power block directly above torch
	stronglyPower(pos + glm::ivec3(0, 0, 1), {0, 0, -1}, !state);

	// also, adjacent redstone componants are weakly powered, but not attachement block
	glm::ivec3 attachement = getAttachedDir(value);
	weaklyPower(pos, attachement, !state, false);
// std::cout << "END addRestone " << state << std::endl;
}

/**
 * @brief updates blocks around redstone dust at 'pos'.
 * @param pos relative position of redstone torch inside chunk
*/
void Chunk::updateRedstoneDust( glm::ivec3 pos )
{
	int actual_value = getBlockAt(pos.x, pos.y, pos.z, true), strength;
std::cout << "updateRestoneDust at " << pos.x << ", " << pos.y << ", " << pos.z << " connect: " << (actual_value >> REDSTONE::DUST_MY) << std::endl;
	if ((actual_value & 0xFF) != blocks::REDSTONE_DUST) {
		strength = 0;
		actual_value |= (REDSTONE::DUST_CONNECT << REDSTONE::DUST_MX) | (REDSTONE::DUST_CONNECT << REDSTONE::DUST_PX)
				| (REDSTONE::DUST_CONNECT << REDSTONE::DUST_MY) | (REDSTONE::DUST_CONNECT << REDSTONE::DUST_PY);
	} else {
		strength = (actual_value >> REDSTONE::STRENGTH_OFFSET) & 0xF;
		int signal_received = getDustStrength(pos);
		// std::cout << "\tupdateRedstoneDust strength " << strength << " vs signal " << signal_received << std::endl;
		if (signal_received != strength) {
			strength = signal_received;
			actual_value &= (REDSTONE::ALL_BITS - REDSTONE::STRENGTH);
			setBlockAt(actual_value | (strength << REDSTONE::STRENGTH_OFFSET), pos.x, pos.y, pos.z, true);
		} else {
			return ;
		}
	}

std::cout << "\tupdateRestoneDust strength " << strength << ", connect: " << (actual_value >> REDSTONE::DUST_MY) << std::endl;
	// use dust's connection to set/unset connected blocks as WEAKLY_POWERED, which is used by red_torches and red_componants
	if ((actual_value >> REDSTONE::DUST_MX) & REDSTONE::DUST_UP) {
		weaklyPowerTarget(pos + glm::ivec3(-1, 0, 1), {1, 0, -1}, strength, false);
	}
	if ((actual_value >> REDSTONE::DUST_MX) & REDSTONE::DUST_SIDE) {
		stronglyPower(pos + glm::ivec3(-1, 0, 0), {1, 0, 0}, REDSTONE::OFF);
		weaklyPowerTarget(pos + glm::ivec3(-1, 0, 0), {1, 0, 0}, strength, false);
	}
	if ((actual_value >> REDSTONE::DUST_PX) & REDSTONE::DUST_UP) {
		weaklyPowerTarget(pos + glm::ivec3(1, 0, 1), {-1, 0, -1}, strength, false);
	}
	if ((actual_value >> REDSTONE::DUST_PX) & REDSTONE::DUST_SIDE) {
		stronglyPower(pos + glm::ivec3(1, 0, 0), {-1, 0, 0}, REDSTONE::OFF);
		weaklyPowerTarget(pos + glm::ivec3(1, 0, 0), {-1, 0, 0}, strength, false);
	}
	if ((actual_value >> REDSTONE::DUST_MY) & REDSTONE::DUST_UP) {
		weaklyPowerTarget(pos + glm::ivec3(0, -1, 1), {0, 1, -1}, strength, false);
	}
	if ((actual_value >> REDSTONE::DUST_MY) & REDSTONE::DUST_SIDE) {
		stronglyPower(pos + glm::ivec3(0, -1, 0), {0, 1, 0}, REDSTONE::OFF);
		weaklyPowerTarget(pos + glm::ivec3(0, -1, 0), {0, 1, 0}, strength, false);
	}
	if ((actual_value >> REDSTONE::DUST_PY) & REDSTONE::DUST_UP) {
		weaklyPowerTarget(pos + glm::ivec3(0, 1, 1), {0, -1, -1}, strength, false);
	}
	if ((actual_value >> REDSTONE::DUST_PY) & REDSTONE::DUST_SIDE) {
		stronglyPower(pos + glm::ivec3(0, 1, 0), {0, -1, 0}, REDSTONE::OFF);
		weaklyPowerTarget(pos + glm::ivec3(0, 1, 0), {0, -1, 0}, strength, false);
	}
	stronglyPower(pos + glm::ivec3(0, 0, -1), {0, 0, 1}, REDSTONE::OFF);
	weaklyPowerTarget(pos + glm::ivec3(0, 0, -1), {0, 0, 1}, strength, false);
// std::cout << "END updateRestoneDust " << strength << std::endl;
}

/**
 * @brief loop through _redstone_schedule and reduce ticks by 1,
 * if ticks reach 0, scheduled redstone componant is updated
*/
void Chunk::updateRedstone( void )
{
	size_t size = _redstone_schedule.size();
	if (!size) {
		return ;
	}

	size_t delCount = 0;
	int state;
	for (size_t index = 0; index < size; ++index) {
		t_redstoneTick &red = _redstone_schedule[index - delCount];
		if (--red.ticks == 0) {
			int value = getBlockAt(red.pos.x, red.pos.y, red.pos.z, true);

			switch (value & 0xFF) {
				case blocks::REDSTONE_LAMP: // only scheduled to be turned off
					// std::cout << "update redstone lamp at " << red.pos.x << ", " << red.pos.y << ", " << red.pos.z << std::endl;
					state = getRedstoneState(red.pos, {0, 0, 0}, REDSTONE::OFF, true);
					if (!state && (value & REDSTONE::ACTIVATED)) { // turn it off
						short level = getLightLevel(red.pos.x, red.pos.y, red.pos.z) & 0xFF0F;
						setLightLevel(level, red.pos.x, red.pos.y, red.pos.z, true);
						startLightSpread(red.pos.x, red.pos.y, red.pos.z, false);
						setBlockAt(value & (REDSTONE::ALL_BITS - REDSTONE::ACTIVATED), red.pos.x, red.pos.y, red.pos.z, true);
					}
					break ;
				case blocks::REDSTONE_TORCH:
					updateRedstoneTorch(red.pos, value);
					break ;
				default:
					std::cout << "Chunk::updateRedstone updated: " << s_blocks[value & 0xFF]->name << std::endl;
					break ;
			}

			_redstone_schedule.erase(_redstone_schedule.begin() + (index - delCount));
			++delCount;
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
	std::cout << "connectRedstoneDust" << std::endl;
	value &= 0xFFFFFF;
	int connect_mx = 0, connect_px = 0, connect_my = 0, connect_py = 0;
	int value_mx = getBlockAt(pos.x - 1, pos.y, pos.z, true), tmx = (value_mx & 0xFF);
	if (tmx == blocks::LEVER || tmx == blocks::REDSTONE_DUST || tmx == blocks::REDSTONE_TORCH) {
		connect_mx |= REDSTONE::DUST_SIDE;
		if (placed && tmx == blocks::REDSTONE_DUST && !(value_mx & (REDSTONE::DUST_SIDE << REDSTONE::DUST_PX))) { // adj dust not linked towards new dust
			connectRedstoneDust(pos + glm::ivec3(-1, 0, 0), value_mx, false);
			setBlockAt(value_mx, pos.x - 1, pos.y, pos.z, true);
		}
	} else if (s_blocks[tmx]->transparent) {
		value_mx = getBlockAt(pos.x - 1, pos.y, pos.z - 1, true);
		if ((value_mx & 0xFF) == blocks::REDSTONE_DUST) {
			connect_mx |= REDSTONE::DUST_SIDE;
			if (placed) {
				connectRedstoneDust(pos + glm::ivec3(-1, 0, -1), value_mx, false);
				setBlockAt(value_mx, pos.x - 1, pos.y, pos.z - 1, true);
			}
		}
	} else if (s_blocks[getBlockAt(pos.x, pos.y, pos.z + 1, true) & 0xFF]->transparent) {
		value_mx = getBlockAt(pos.x - 1, pos.y, pos.z + 1, true);
		if ((value_mx & 0xFF) == blocks::REDSTONE_DUST) {
			connect_mx |= REDSTONE::DUST_UP;
			if (placed) {
				connectRedstoneDust(pos + glm::ivec3(-1, 0, 1), value_mx, false);
				setBlockAt(value_mx, pos.x - 1, pos.y, pos.z + 1, true);
			}
		}
	}

	int value_px = getBlockAt(pos.x + 1, pos.y, pos.z, true), tpx = (value_px & 0xFF);
	if (tpx == blocks::LEVER || tpx == blocks::REDSTONE_DUST || tpx == blocks::REDSTONE_TORCH) {
		connect_px |= REDSTONE::DUST_SIDE;
		if (placed && tpx == blocks::REDSTONE_DUST && !(value_px & (REDSTONE::DUST_SIDE << REDSTONE::DUST_MX))) { // adj dust not linked towards new dust
			connectRedstoneDust(pos + glm::ivec3(1, 0, 0), value_px, false);
			setBlockAt(value_px, pos.x + 1, pos.y, pos.z, true);
		}
	} else if (s_blocks[tpx]->transparent) {
		value_px = getBlockAt(pos.x + 1, pos.y, pos.z - 1, true);
		if ((value_px & 0xFF) == blocks::REDSTONE_DUST) {
			connect_px |= REDSTONE::DUST_SIDE;
			if (placed) {
				connectRedstoneDust(pos + glm::ivec3(1, 0, -1), value_px, false);
				setBlockAt(value_px, pos.x + 1, pos.y, pos.z - 1, true);
			}
		}
	} else if (s_blocks[getBlockAt(pos.x, pos.y, pos.z + 1, true) & 0xFF]->transparent) {
		value_px = getBlockAt(pos.x + 1, pos.y, pos.z + 1, true);
		if ((value_px & 0xFF) == blocks::REDSTONE_DUST) {
			connect_px |= REDSTONE::DUST_UP;
			if (placed) {
				connectRedstoneDust(pos + glm::ivec3(1, 0, 1), value_px, false);
				setBlockAt(value_px, pos.x + 1, pos.y, pos.z + 1, true);
			}
		}
	}

	int value_my = getBlockAt(pos.x, pos.y - 1, pos.z, true), tmy = (value_my & 0xFF);
	if (tmy == blocks::LEVER || tmy == blocks::REDSTONE_DUST || tmy == blocks::REDSTONE_TORCH) {
		connect_my |= REDSTONE::DUST_SIDE;
		if (placed && tmy == blocks::REDSTONE_DUST && !(value_my & (REDSTONE::DUST_SIDE << REDSTONE::DUST_PY))) { // adj dust not linked towards new dust
			connectRedstoneDust(pos + glm::ivec3(0, -1, 0), value_my, false);
			setBlockAt(value_my, pos.x, pos.y - 1, pos.z, true);
		}
	} else if (s_blocks[tmy]->transparent) {
		value_my = getBlockAt(pos.x, pos.y - 1, pos.z - 1, true);
		if ((value_my & 0xFF) == blocks::REDSTONE_DUST) {
			connect_my |= REDSTONE::DUST_SIDE;
			if (placed) {
				connectRedstoneDust(pos + glm::ivec3(0, -1, -1), value_my, false);
				setBlockAt(value_my, pos.x, pos.y - 1, pos.z - 1, true);
			}
		}
	} else if (s_blocks[getBlockAt(pos.x, pos.y, pos.z + 1, true) & 0xFF]->transparent) {
		value_my = getBlockAt(pos.x, pos.y - 1, pos.z + 1, true);
		if ((value_my & 0xFF) == blocks::REDSTONE_DUST) {
			connect_my |= REDSTONE::DUST_UP;
			if (placed) {
				connectRedstoneDust(pos + glm::ivec3(0, -1, 1), value_my, false);
				setBlockAt(value_my, pos.x, pos.y - 1, pos.z + 1, true);
			}
		}
	}

	int value_py = getBlockAt(pos.x, pos.y + 1, pos.z, true), tpy = (value_py & 0xFF);
	if (tpy == blocks::LEVER || tpy == blocks::REDSTONE_DUST || tpy == blocks::REDSTONE_TORCH) {
		connect_py |= REDSTONE::DUST_SIDE;
		if (placed && tpy == blocks::REDSTONE_DUST && !(value_py & (REDSTONE::DUST_SIDE << REDSTONE::DUST_MY))) { // adj dust not linked towards new dust
			connectRedstoneDust(pos + glm::ivec3(0, 1, 0), value_py, false);
			setBlockAt(value_py, pos.x, pos.y + 1, pos.z, true);
		}
	} else if (s_blocks[tpy]->transparent) {
		value_py = getBlockAt(pos.x, pos.y + 1, pos.z - 1, true);
		if ((value_py & 0xFF) == blocks::REDSTONE_DUST) {
			connect_py |= REDSTONE::DUST_SIDE;
			if (placed) {
				connectRedstoneDust(pos + glm::ivec3(0, 1, -1), value_py, false);
				setBlockAt(value_py, pos.x, pos.y + 1, pos.z - 1, true);
			}
		}
	} else if (s_blocks[getBlockAt(pos.x, pos.y, pos.z + 1, true) & 0xFF]->transparent) {
		value_py = getBlockAt(pos.x, pos.y + 1, pos.z + 1, true);
		if ((value_py & 0xFF) == blocks::REDSTONE_DUST) {
			connect_py |= REDSTONE::DUST_UP;
			if (placed) {
				connectRedstoneDust(pos + glm::ivec3(0, 1, 1), value_py, false);
				setBlockAt(value_py, pos.x, pos.y + 1, pos.z + 1, true);
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
