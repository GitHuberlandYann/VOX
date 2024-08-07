#include "Chunk.hpp"
#include "logs.hpp"

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
	- if alimented when already powered, we don't update it
	- when it depowers, checks if should power again on its own

 - comparator:
	- outputs same signal as rear input
	- if COMPARE mode:   output is rear if rear > left && rear > right else 0
	- if SUBSTRACT mode: output is max(0, rear - max(left, right))
	- left and right can only come from dust, repeater, comparator, and redstone block

 - piston:
	- no delay to start movement
	- 3 game ticks to extend, 2 game ticks to retract
	- can't move more than 12 other blocks
	- also checks above on the side for power source == quasi-connectivity
	- piston head and moved blocks are entities until end of movement
	- if sticky piston retracts before finishing to push, doesn't retract block it pushed

 - observer:
	- 1 tick delay
	- generates 1 tick pulse of strength 15 when block in front is updated
	- if updated again when didn't finish pulse, abort update

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
	int orientation = (value >> offset::blocks::orientation) & 0x7;
	int placement = (value >> offset::blocks::bitfield) & 0x3;
	switch (TYPE(value)) {
		case blocks::lever:
		case blocks::stone_button:
		case blocks::oak_button:
		case blocks::item_frame:
			switch (placement) {
				case placement::ceiling:
					target = glm::ivec3(0, 0, 1);
					break ;
				case placement::floor:
					target = glm::ivec3(0, 0, -1);
					break ;
				case placement::wall:
					switch (orientation) {
						case face_dir::minus_x:
							target = glm::ivec3(1, 0, 0);
							break ;
						case face_dir::plus_x:
							target = glm::ivec3(-1, 0, 0);
							break ;
						case face_dir::minus_y:
							target = glm::ivec3(0, 1, 0);
							break ;
						case face_dir::plus_y:
							target = glm::ivec3(0, -1, 0);
							break ;
					}
					break ;
			}
			break ;
		case blocks::torch:
		case blocks::redstone_torch:
		case blocks::oak_sign:
			switch (orientation) {
				case face_dir::minus_x:
					target = glm::ivec3(-1, 0, 0);
					break ;
				case face_dir::plus_x:
					target = glm::ivec3(1, 0, 0);
					break ;
				case face_dir::minus_y:
					target = glm::ivec3(0, -1, 0);
					break ;
				case face_dir::plus_y:
					target = glm::ivec3(0, 1, 0);
					break ;
				case face_dir::minus_z:
					target = glm::ivec3(0, 0, -1);
					break ;
			}
			break ;
		case blocks::redstone_dust:
		case blocks::repeater:
		case blocks::comparator:
		case blocks::poppy:
		case blocks::dandelion:
		case blocks::cornflower:
		case blocks::dead_bush:
		case blocks::allium:
		case blocks::pink_tulip:
		case blocks::blue_orchid:
		case blocks::sugar_cane:
		case blocks::cactus:
		case blocks::grass:
			target = glm::ivec3(0, 0, -1);
			break ;
	}
	return target;
}

/**
 * @brief check adj blocks to return whether block at given pos is currently weakdy powered.
 * @param pos block to get state of
 * @param except this given dir is not checked
 * @return strength received by adj dust
*/
int Chunk::getWeakdyState( glm::ivec3 pos, glm::ivec3 except )
{
	int res = redstone::off;
	for (int index = 0; index < 6; ++index) {
		const glm::ivec3 delta = adj_blocks[index];
		if (index == face_dir::minus_z || delta == except) continue ; // we don't check below

		int adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z);
		// std::cout << "\tchecking " << s_blocks[TYPE(adj)]->name << " at " << pos.x + delta.x << ", " << pos.y + delta.y << ", " << pos.z + delta.z << std::endl;	
		if (TYPE(adj) == blocks::redstone_dust && (adj & mask::redstone::strength)
				&& ((delta.x == 1 && (adj & (redstone::dust::side << offset::redstone::dust::mx)))
				|| (delta.x == -1 && (adj & (redstone::dust::side << offset::redstone::dust::px)))
				|| (delta.y == 1 && (adj & (redstone::dust::side << offset::redstone::dust::my)))
				|| (delta.y == -1 && (adj & (redstone::dust::side << offset::redstone::dust::py)))
				|| delta.z == 1
				)) { // red dust gives signal only to its connected blocks
			if (((adj >> offset::redstone::strength) & 0xF) > res) {
				res = ((adj >> offset::redstone::strength) & 0xF);
			}
		}
	}
	return (res);
}

/**
 * @brief called by comparator rear
 * check if item frame attached to block and compute its output power
 */
int Chunk::getRedstoneSignalItemFrame( glm::ivec3 pos, glm::ivec3 except )
{
	int res = 0;
	for (int index = 0; index < 6; ++index) {
		const glm::ivec3 delta = adj_blocks[index];
		if (delta == except) continue ;

		int value = getBlockAt(pos + delta);
	// std::cout << "\tredstone item frame check " << s_blocks[TYPE(value)]->name << std::endl;
	// std::cout << "\tredstone item frame dir " << POS(getAttachedDir(value)) << " vs " << POS(-delta) << std::endl;
		if (TYPE(value) == blocks::item_frame && getAttachedDir(value) == -delta) {
			auto search = std::find_if(_entities.begin(), _entities.end(), [this, pos, delta](auto e) { return (e->isAt(pos + delta + glm::ivec3(_startX, _startY, .0f))); });
			if (search != _entities.end()) {
				res = glm::max(res, static_cast<ItemFrameEntity*>(search->get())->getRotation());
			} else {
				LOGERROR("404 item frame not found");
			}
		}
	}
	// std::cout << "computed redstone item frame at " << POS(pos) << ": " << res << std::endl;
	return (res);
}

/**
 * @brief called by comparator rear
 * get signal emitted by lectern at given pos
 */
int Chunk::getRedstoneSignalLectern( glm::ivec3 pos )
{
	if (pos.x < 0) {
		if (_neighbours[face_dir::minus_x]) {
			pos.x += settings::consts::chunk_size;
			return (_neighbours[face_dir::minus_x]->getRedstoneSignalLectern(pos));
		}
	} else if (pos.x >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_x]) {
			pos.x -= settings::consts::chunk_size;
			return (_neighbours[face_dir::plus_x]->getRedstoneSignalLectern(pos));
		}
	} else if (pos.y < 0) {
		if (_neighbours[face_dir::minus_y]) {
			pos.y += settings::consts::chunk_size;
			return (_neighbours[face_dir::minus_y]->getRedstoneSignalLectern(pos));
		}
	} else if (pos.y >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_y]) {
			pos.y -= settings::consts::chunk_size;
			return (_neighbours[face_dir::plus_y]->getRedstoneSignalLectern(pos));
		}
	} else {
		COMPLOG(LOG("Lectern check at " << POS(pos)));
		auto search = std::find_if(_entities.begin(), _entities.end(), [this, pos](auto e) { return (e->isAt(pos + glm::ivec3(_startX, _startY, 0))); });
		if (search != _entities.end()) {
			COMPLOG(LOG("Lectern found at " << POS(pos)));
			return (static_cast<LecternEntity*>(search->get())->getSignal());
		} else {
			LOGERROR("404 lectern entity not found in getRedstoneSignalLectern");
		}
	}
	return (redstone::off);
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
	int res = redstone::off;
	int adj = getBlockAt(pos.x, pos.y, pos.z);
	// std::cout << "\tcompChecking " << s_blocks[TYPE(adj)]->name << " at " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;	
	switch (TYPE(adj)) {
		case blocks::repeater:
			if (target == adj_blocks[(adj >> offset::blocks::orientation) & 0x7] && (adj & mask::redstone::powered)) {
				return (0xF);
			}
			break ;
		case blocks::comparator:
			if (target == adj_blocks[(adj >> offset::blocks::orientation) & 0x7]) {
				return ((adj >> offset::redstone::strength) & 0xF);
			}
			break ;
		case blocks::observer:
			if (target == -adj_blocks[(adj >> offset::blocks::orientation) & 0x7] && (adj & mask::redstone::activated)) {
				return (0xF);
			}
			break ;
		case blocks::redstone_torch:
				if (!side && !(adj & mask::redstone::powered)) {
					return (0xF);
				}
				break ;
		case blocks::lever:
			if (!side && (adj & mask::redstone::powered)) {
				return (0xF);
			}
			break ;
		case blocks::redstone_dust:
			return ((repeater & side) ? 0 : (adj >> offset::redstone::strength) & 0xF);
		case blocks::redstone_block:
			return ((repeater & side) ? 0 : 0xF);
		case blocks::lectern:
			COMPLOG(LOG("Lectern found near comparator."));
			if (!repeater && !side) {
				COMPLOG(LOG("and it is at rear"));
				return (getRedstoneSignalLectern(pos));
			}
		default:
			if (!side) {
				if (!repeater && !s_blocks[TYPE(adj)]->transparent) { // fun called by comparator rear
					res = getRedstoneSignalItemFrame(pos, target);
				}
				if (adj & mask::redstone::powered) {
					res = glm::max(res, ((adj >>offset::redstone::strength) & 0xF));
				}
				if (adj & mask::redstone::weakdy_powered) {
					res = glm::max(res, getWeakdyState(pos, target));
				}
			}
	}
	return (res);
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
		// std::cout << "\tchecking " << s_blocks[TYPE(adj)]->name << " {" << state << "}at " << _startX + pos.x + delta.x << ", " << _startY + pos.y + delta.y << ", " << pos.z + delta.z << std::endl;	
		// if (TYPE(adj) == blocks::moving_piston) {
		// 	std::cout << "\t\t" << ((adj & mask::redstone::powered) ? "powered" : "") << ((adj & mask::redstone::weakdy_powered) ? "weakdy powered" : "") << " currently " << ((adj & mask::redstone::piston::retracting) ? "retracting" : "extending") << ((s_blocks[TYPE(adj)]->isTransparent(adj)) ? " transparent" : " not transparent") << std::endl;
		// }
		switch (TYPE(adj)) {
			case blocks::lever:
				if (adj & mask::redstone::powered) {
					if (getAttachedDir(adj) == -delta || weak) {
						return (0xF);
					}
				}
				break ;
			case blocks::repeater:
				if (delta == -adj_blocks[(adj >> offset::blocks::orientation) & 0x7] && (adj & mask::redstone::powered)) {
					return (0xF);
				}
				break ;
			case blocks::observer:
				if (delta == adj_blocks[(adj >> offset::blocks::orientation) & 0x7] && (adj & mask::redstone::activated)) {
					return (0xF);
				}
				break ;
			case blocks::comparator:
				if (delta == -adj_blocks[(adj >> offset::blocks::orientation) & 0x7] && (adj & mask::redstone::powered)) {
					state = glm::max(state, ((adj >> offset::redstone::strength) & 0xF));
				}
				break ;
			case blocks::redstone_torch:
				if (!(adj & mask::redstone::powered) && (delta.z == -1 || (weak && delta != -getAttachedDir(adj)))) {
					return (0xF);
				}
				break ;
			case blocks::redstone_dust:
				if (weak && (adj & mask::redstone::strength)
					&& ((delta.x == 1 && (adj & (mask::redstone::dust::connect << offset::redstone::dust::mx)))
					|| (delta.x == -1 && (adj & (mask::redstone::dust::connect << offset::redstone::dust::px)))
					|| (delta.y == 1 && (adj & (mask::redstone::dust::connect << offset::redstone::dust::my)))
					|| (delta.y == -1 && (adj & (mask::redstone::dust::connect << offset::redstone::dust::py)))
					|| delta.z == 1
					)) { // red dust gives signal only to its connected blocks and to the block below
					return (redstone::on);
				}
				break ;
			case blocks::stone_button:
			case blocks::oak_button:
				if ((adj & mask::redstone::powered) && delta == -getAttachedDir(adj)) {
					return (0xF);
				}
				break ;
			default: // redstone block behaves like default
				if (weak && (adj & (mask::redstone::powered | mask::redstone::weakdy_powered))) {
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
	int valueAbove = getBlockAt(pos.x, pos.y, pos.z + 1);
	bool solidAbove = !s_blocks[TYPE(valueAbove)]->isTransparent(valueAbove);
	int res = redstone::off;
	for (int index = 0; index < 6; ++index) {
		const glm::ivec3 delta = adj_blocks[index];
		int adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z);
		switch (TYPE(adj)) {
			case blocks::redstone_torch:
				if (!(adj & mask::redstone::powered)) {
					return (0xF);
				}
				break ;
			case blocks::repeater:
				if (delta == -adj_blocks[(adj >> offset::blocks::orientation) & 0x7] && (adj & mask::redstone::powered)) {
					return (0xF);
				}
				break ;
			case blocks::observer:
				if (delta == adj_blocks[(adj >> offset::blocks::orientation) & 0x7] && (adj & mask::redstone::activated)) {
					return (0xF);
				}
				break ;
			case blocks::comparator:
				if (delta == -adj_blocks[(adj >> offset::blocks::orientation) & 0x7] && (adj & mask::redstone::powered)) {
					if (((adj >> offset::redstone::strength) & 0xF) > res) {
						res = ((adj >> offset::redstone::strength) & 0xF);
					}
				}
				break ;
			case blocks::redstone_dust:
				if (((adj >> offset::redstone::strength) & 0xF) > res + 1) {
					res = ((adj >> offset::redstone::strength) & 0xF) - 1;
				}
				break ;
			case blocks::lever:
			case blocks::redstone_block:
				if (adj & mask::redstone::powered) {
					return (0xF);
				}
			default:
				if ((adj & mask::redstone::powered) && ((adj >> offset::redstone::strength) & 0xF) > res) {
					res = ((adj >> offset::redstone::strength) & 0xF);
				}
				break ;
		}
		if (delta.z) {
			continue ;
		} 
		if (s_blocks[TYPE(adj)]->isTransparent(adj)) {
			adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z - 1);
			if (TYPE(adj) == blocks::redstone_dust && (((adj >> offset::redstone::strength) & 0xF) > res + 1)) {
				res = ((adj >> offset::redstone::strength) & 0xF) - 1;
			}
		} else {
			if (solidAbove) {
				continue ;
			}
			adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z + 1);
			if (TYPE(adj) == blocks::redstone_dust && (((adj >> offset::redstone::strength) & 0xF) > res + 1)) {
				res = ((adj >> offset::redstone::strength) & 0xF) - 1;
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
	int previous = getBlockAt(pos);
	// std::cout << "strongly power block " << s_blocks[TYPE(previous)]->name << " at " << pos.x << ", " << pos.y << ", " << pos.z << ": " << state << std::endl;
	if (!s_blocks[TYPE(previous)]->isTransparent(previous) && TYPE(previous) != blocks::redstone_block) {
		int strongly_powered = getRedstoneStrength(pos, source, state, false);
		// std::cout << "\tactual state is " << strongly_powered << std::endl;
		int value = previous & (mask::all_bits - mask::redstone::powered - mask::redstone::weakdy_powered - mask::redstone::strength);
		if (strongly_powered) { value |= mask::redstone::powered; }
		int weakdy_powered = getWeakdyState(pos, {0, 0, 0});
		if (weakdy_powered)   { value |= mask::redstone::weakdy_powered; }
		value |= (strongly_powered << offset::redstone::strength);
		if (TYPE(value) == blocks::redstone_lamp) {
			if (value == previous) {
				return ;
			} else if (!strongly_powered && !weakdy_powered) { // lamp is unpowered with delay
				setBlockAt(value, pos, false, false);
				return ;
			}
		}
		setBlockAt(value, pos, false, TYPE(value) != blocks::redstone_lamp);
		if (strongly_powered) {
			weaklyPower(pos, source, redstone::on, false);
		} else if (weakdy_powered) {
			weaklyPower(pos, source, redstone::on, true);
			for (int index = 0; index < 6; ++index) { // still power off adj redstone dusts
				const glm::ivec3 delta = adj_blocks[index];
				if (delta == source) continue ;
				int adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z);
				if (TYPE(adj) == blocks::redstone_dust) {
					updateRedstoneDust(pos + delta);
				}
			}
		} else {
			weaklyPower(pos, {0, 0, 0}, redstone::off, false);
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
	switch (TYPE(adj)) {
		case blocks::redstone_lamp:
			if (state && !(adj & mask::redstone::activated)) {
				// std::cout << "\tactivate redstone lamp at " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
				// turn lamp on instantly
				short level = getLightLevel(pos.x, pos.y, pos.z) & 0xFF00;
				level += s_blocks[blocks::redstone_lamp]->light_level + (s_blocks[blocks::redstone_lamp]->light_level << 4);
				setLightLevel(level, pos.x, pos.y, pos.z, true);
				startLightSpread(pos.x, pos.y, pos.z, false);
				setBlockAt(adj | mask::redstone::activated, pos.x, pos.y, pos.z, false);
			} else if (!state && (adj & mask::redstone::activated)) {
				// add lamp to tick delay machine
				// std::cout << "\tschedule redstone lamp at " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
				scheduleRedstoneTick({pos, 2 * redstone::tick});
			}
			break ;
		case blocks::repeater:
			if (source == -adj_blocks[(adj >> offset::blocks::orientation) & 0x7]) { // signal comes from behind repeater
				// std::cout << "scheduling repeater from " << source.x << ", " << source.y << ", " << source.z << std::endl;
				if (state && (adj & mask::redstone::powered)) {
					break ;
				}
				scheduleRedstoneTick({pos, redstone::tick * (((adj >> offset::redstone::repeater::ticks) & 0x3) + 1), state});
			} else if (!source.x == !!adj_blocks[(adj >> offset::blocks::orientation) & 0x7].x && !source.y == !!adj_blocks[(adj >> offset::blocks::orientation) & 0x7].y) { // signal comes from side
				// we check if it comes from repeater and (un)lock the repeater
				src = getBlockAt(pos.x + source.x, pos.y + source.y, pos.z + source.z);
				if ((TYPE(src) == blocks::repeater || TYPE(src) == blocks::comparator) && source == -adj_blocks[(src >> offset::blocks::orientation) & 0x7]) {
					if ((src & mask::redstone::powered) && !(adj & mask::redstone::repeater::lock)) { // if not locked but should be, we lock it
						setBlockAt(adj | mask::redstone::repeater::lock, pos.x, pos.y, pos.z, false);
					} else if (!(src & mask::redstone::powered) && (adj & mask::redstone::repeater::lock)) { // if locked but shouldn't be, we unlock it
						adj &= (mask::all_bits - mask::redstone::repeater::lock);
						initRepeater(pos, adj, false);
						setBlockAt(adj, pos.x, pos.y, pos.z, false);
					}
				}
			}
			break ;
		case blocks::comparator:
			if (source.z || source == adj_blocks[(adj >> offset::blocks::orientation) & 0x7]) { // signal update come from up/down, front -> we discard
				break ;
			}
			updateComparator(pos, adj, false);
			break ;
		case blocks::piston:
		case blocks::sticky_piston:
			// if (source == adj_blocks[(adj >> offset::blocks::orientation) & 0x7]) {
			// 	break ;
			// }
			updatePiston(pos, adj);
			break ;
		case blocks::redstone_torch:
			if (getAttachedDir(adj) == source) {
				scheduleRedstoneTick({pos, redstone::tick});
			}
			break ;
		case blocks::redstone_dust:
			if (weakdy) {
				break ;
			}
			if (!state && !(adj & mask::redstone::strength)) { // we tell turned off dust to turn off, no update
				break ;
			}
			updateRedstoneDust(pos);
			break ;
		case blocks::tnt:
			if (state == redstone::on) {
				handleHit(false, blocks::tnt, pos + glm::ivec3(_startX, _startY, 0), Modif::litnt);
			}
			break ;
	}
	if (weakdy) {
		return ;
	}
	// quasi-connectivity
	int below = getBlockAt(pos.x, pos.y, pos.z - 1, true);
	if (TYPE(below) == blocks::piston || TYPE(below) == blocks::sticky_piston) {
		PISTONLOG(LOG("\t\tquasi-connectivity at " << POSDXYZ(pos, _startX, _startY, - 1)));
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


	// 			switch (s_blocks[TYPE(block_value)]->geometry) {
	// 				case geometry::trapdoor:
	// 					(state) ? setBlockAt(block_value | mask::redstone::powered, pos.x + dx, pos.y + dy, pos.z + dz)
	// 							: setBlockAt(block_value ^ (1 << offset::redstone::powered), pos.x + dx, pos.y + dy, pos.z + dz);
	// 					break ;
	// 				case geometry::door:
	// 					(state) ? setBlockAt(block_value | mask::redstone::powered, pos.x + dx, pos.y + dy, pos.z + dz)
	// 							: setBlockAt(block_value ^ (1 << offset::redstone::powered), pos.x + dx, pos.y + dy, pos.z + dz);
	// 					dz += ((block_value >> 12) & door::upper_half) ? -1 : 1;
	// 					block_value ^= (door::upper_half << 12);
	// 					(state) ? setBlockAt(block_value | mask::redstone::powered, pos.x + dx, pos.y + dy, pos.z + dz)
	// 							: setBlockAt(block_value ^ (1 << offset::redstone::powered), pos.x + dx, pos.y + dy, pos.z + dz);
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
	REDUPLOG(LOG("flick lever " << state));
	// DayCycle::Get()->setTicks(0); // for debuging purposes
	// strongly power block the lever is attached to
	glm::ivec3 target = getAttachedDir(value);
	stronglyPower(pos + target, -target, (state) ? 0xF : redstone::off);

	// also, adjacent redstone componants are weakly powered
	weaklyPower(pos, {0, 0, 0}, state, false);
	REDUPLOG(LOG("END flick lever " << state));
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
	if (TYPE(actual_value) != blocks::redstone_torch) {
		state = true; // if state on, torch off
	} else {
		glm::ivec3 target = getAttachedDir(actual_value);
		state = getBlockAt(pos.x + target.x, pos.y + target.y, pos.z + target.z) & (mask::redstone::powered | mask::redstone::weakdy_powered);
		if (state && !(actual_value & mask::redstone::powered)) {
			setBlockAt(actual_value | mask::redstone::powered, pos.x, pos.y, pos.z, false);
			short level = getLightLevel(pos.x, pos.y, pos.z) & 0xFF00;
			setLightLevel(level, pos.x, pos.y, pos.z, true);
			startLightSpread(pos.x, pos.y, pos.z, false);
		} else if (!state && (actual_value & mask::redstone::powered)) {
			setBlockAt(actual_value & (mask::all_bits - mask::redstone::powered), pos.x, pos.y, pos.z, false);
			short level = getLightLevel(pos.x, pos.y, pos.z) + 7 + (7 << 4);
			setLightLevel(level, pos.x, pos.y, pos.z, true);
			startLightSpread(pos.x, pos.y, pos.z, false);
		}
	}

	REDUPLOG(LOG("addRedstoneTorch " << !state));
	// strongly power block directly above torch
	stronglyPower(pos + glm::ivec3(0, 0, 1), {0, 0, -1}, (!state) ? 0xF : redstone::off);

	// also, adjacent redstone componants are weakly powered, but not attachement block
	glm::ivec3 attachement = getAttachedDir(value);
	weaklyPower(pos, attachement, !state, false);
	REDUPLOG(LOG("END addRestone " << !state));
}

/**
 * @brief updates blocks around redstone dust at 'pos'.
 * @param pos relative position of redstone torch inside chunk
*/
void Chunk::updateRedstoneDust( glm::ivec3 pos )
{
	int actual_value = getBlockAt(pos.x, pos.y, pos.z), strength, signal_received = 0;
	// std::cout << "updateRestoneDust at " << pos.x << ", " << pos.y << ", " << pos.z << " connect: " << (actual_value >> offset::redstone::dust::my) << std::endl;
	if (TYPE(actual_value) != blocks::redstone_dust) {
		strength = 0;
		actual_value |= (mask::redstone::dust::connect << offset::redstone::dust::mx) | (mask::redstone::dust::connect << offset::redstone::dust::px)
				| (mask::redstone::dust::connect << offset::redstone::dust::my) | (mask::redstone::dust::connect << offset::redstone::dust::py);
	} else {
		strength = (actual_value >> offset::redstone::strength) & 0xF;
		signal_received = getDustStrength(pos);
		// std::cout << "\tupdateRedstoneDust strength " << strength << " vs signal " << signal_received << std::endl;
		if (signal_received != strength) {
			strength = (strength > signal_received) ? 0 : signal_received; // shortcut recursion by cutting signal
			actual_value &= (mask::all_bits - mask::redstone::strength);
			actual_value |= (strength << offset::redstone::strength);
			setBlockAt(actual_value, pos.x, pos.y, pos.z, false);
		} else {
			// std::cout << "END updateRestoneDust " << strength << std::endl;
			if (strength > 1) { // because of shortcut above, we need to restart recurse if adj dust still powered
				for (int index = 0; index < 6; ++index) {
					const glm::ivec3 delta = adj_blocks[index];
					int adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z);
					if (TYPE(adj) == blocks::redstone_dust && !((adj >> offset::redstone::strength) & 0xF)) {
						updateRedstoneDust(pos + delta);
					}
				}
			}
			return ;
		}
	}

	// std::cout << "\tupdateRestoneDust strength " << strength << ", at " << _startX + pos.x << ", " << _startY + pos.y << ", " << pos.z << std::endl;
	// use dust's connection to set/unset connected blocks as WEAKLY_POWERED, which is used by red_torches and red_componants
	if ((actual_value >> offset::redstone::dust::mx) & redstone::dust::up) {
		weaklyPowerTarget(pos + glm::ivec3(-1, 0, 1), {1, 0, -1}, (actual_value >> offset::redstone::strength) & 0xF, false);
		actual_value = getBlockAt(pos.x, pos.y, pos.z);
	}
	if ((actual_value >> offset::redstone::dust::mx) & redstone::dust::side) {
		stronglyPower(pos + glm::ivec3(-1, 0, 0), {1, 0, 0}, redstone::off);
		weaklyPowerTarget(pos + glm::ivec3(-1, 0, 0), {1, 0, 0}, (actual_value >> offset::redstone::strength) & 0xF, false);
		actual_value = getBlockAt(pos.x, pos.y, pos.z);
	}
	if ((actual_value >> offset::redstone::dust::px) & redstone::dust::up) {
		weaklyPowerTarget(pos + glm::ivec3(1, 0, 1), {-1, 0, -1}, (actual_value >> offset::redstone::strength) & 0xF, false);
		actual_value = getBlockAt(pos.x, pos.y, pos.z);
	}
	if ((actual_value >> offset::redstone::dust::px) & redstone::dust::side) {
		stronglyPower(pos + glm::ivec3(1, 0, 0), {-1, 0, 0}, redstone::off);
		weaklyPowerTarget(pos + glm::ivec3(1, 0, 0), {-1, 0, 0}, (actual_value >> offset::redstone::strength) & 0xF, false);
		actual_value = getBlockAt(pos.x, pos.y, pos.z);
	}
	if ((actual_value >> offset::redstone::dust::my) & redstone::dust::up) {
		weaklyPowerTarget(pos + glm::ivec3(0, -1, 1), {0, 1, -1}, (actual_value >> offset::redstone::strength) & 0xF, false);
		actual_value = getBlockAt(pos.x, pos.y, pos.z);
	}
	if ((actual_value >> offset::redstone::dust::my) & redstone::dust::side) {
		stronglyPower(pos + glm::ivec3(0, -1, 0), {0, 1, 0}, redstone::off);
		weaklyPowerTarget(pos + glm::ivec3(0, -1, 0), {0, 1, 0}, (actual_value >> offset::redstone::strength) & 0xF, false);
		actual_value = getBlockAt(pos.x, pos.y, pos.z);
	}
	if ((actual_value >> offset::redstone::dust::py) & redstone::dust::up) {
		weaklyPowerTarget(pos + glm::ivec3(0, 1, 1), {0, -1, -1}, (actual_value >> offset::redstone::strength) & 0xF, false);
		actual_value = getBlockAt(pos.x, pos.y, pos.z);
	}
	if ((actual_value >> offset::redstone::dust::py) & redstone::dust::side) {
		stronglyPower(pos + glm::ivec3(0, 1, 0), {0, -1, 0}, redstone::off);
		weaklyPowerTarget(pos + glm::ivec3(0, 1, 0), {0, -1, 0}, (actual_value >> offset::redstone::strength) & 0xF, false);
		actual_value = getBlockAt(pos.x, pos.y, pos.z);
	}
	stronglyPower(pos + glm::ivec3(0, 0, -1), {0, 0, 1}, redstone::off);
	weaklyPowerTarget(pos + glm::ivec3(0, 0, -1), {0, 0, 1}, (actual_value >> offset::redstone::strength) & 0xF, false);
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
	const glm::ivec3 front = adj_blocks[(value >> offset::blocks::orientation) & 0x7];
	int rear = getRedstoneSignalTarget(pos - front, front, false, true);
	const glm::ivec3 side = {!front.x, !front.y, front.z};
	int left = getRedstoneSignalTarget(pos + side, -side, true, true);
	int right = getRedstoneSignalTarget(pos - side, side, true, true);
	bool lock = left | right;
	if (lock) {
		value |= mask::redstone::repeater::lock;
	} else {
		if (value & mask::redstone::repeater::lock) { value &= (mask::all_bits - mask::redstone::repeater::lock); }
		if (rear && !(value & mask::redstone::powered)) {
			REPEATLOG(LOG("initRepeater ON"));
			scheduleRedstoneTick({pos, redstone::tick * (((value >> offset::redstone::repeater::ticks) & 0x3) + 1), (init) ? redstone::on : redstone::on | mask::redstone::repeater::lock});
		} else if (!rear && (value & mask::redstone::powered)) {
			REPEATLOG(LOG("initRepeater OFF"));
			scheduleRedstoneTick({pos, redstone::tick * (((value >> offset::redstone::repeater::ticks) & 0x3) + 1), (init) ? redstone::off : redstone::off | mask::redstone::repeater::lock});
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
	const glm::ivec3 front = adj_blocks[(value >> offset::blocks::orientation) & 0x7];
	int rear = getRedstoneSignalTarget(pos - front, front, false, false);
	const glm::ivec3 side = {!front.x, !front.y, front.z};
	int left = getRedstoneSignalTarget(pos + side, -side, true, false);
	int right = getRedstoneSignalTarget(pos - side, side, true, false);

	bool mode = value & mask::redstone::comparator::mode;
	int output = (mode == redstone::comparator::compare) ? rear * (left <= rear && right <= rear)
											: glm::max(0, rear - glm::max(left, right));
	COMPLOG(LOG("updateComparator mode " << mode << ": rear " << rear << ", left " << left << ", right " << right << " -> ouput " << output << " vs " << ((value >> offset::redstone::strength) & 0xF)));
	
	if (!scheduledUpdate) {
		if (output != ((value >> offset::redstone::strength) & 0xF)) {
			return scheduleRedstoneTick({pos, redstone::tick, output});
		}
		// no change in ouput, we still check for scheduled update, in which case we cancel it
		abortComparatorScheduleTick(pos);
	} else if (output != ((value >> offset::redstone::strength) & 0xF)) {
		if (output) {
			value &= (mask::all_bits - mask::redstone::strength);
			value |= (output << offset::redstone::strength);
			setBlockAt(value | (mask::redstone::powered), pos.x, pos.y, pos.z, false);
			stronglyPower(pos + front, -front, output);
			weaklyPowerTarget(pos + front, -front, redstone::on, false);
		} else {
			setBlockAt(value & (mask::all_bits - mask::redstone::powered - mask::redstone::strength), pos.x, pos.y, pos.z, false);
			stronglyPower(pos + front, -front, redstone::off);
			weaklyPowerTarget(pos + front, -front, redstone::off, false);
		}
	}
}

/**
 * @brief called when item frame's content is updated, check if comparators are reading attachement block and update them
 */
void Chunk::updateItemFrame( glm::ivec3 pos )
{
	for (int index = 0; index < 4; ++index) {
		const glm::ivec3 delta = adj_blocks[index];
		int value = getBlockAt(pos + delta);
		if (TYPE(value) == blocks::comparator && delta == adj_blocks[(value >> offset::blocks::orientation) & 0x7]) {
			updateComparator(pos + delta, value, false);
		}
	}
}

/**
 * @brief called when lectern's content is updated, check if comparators are reading lectern and update them
 */
void Chunk::updateLectern( glm::ivec3 pos )
{
	for (int index = 0; index < 4; ++index) {
		const glm::ivec3 delta = adj_blocks[index];
		int value = getBlockAt(pos + delta);
		if (TYPE(value) == blocks::comparator && delta == adj_blocks[(value >> offset::blocks::orientation) & 0x7]) {
			updateComparator(pos + delta, value, false);
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
	const glm::ivec3 front = adj_blocks[(value >> offset::blocks::orientation) & 0x7];
	int count = 0;
	for (int i = 0; i < 13; ++i) {
		int adj = getBlockAt(pos + front * (i + 1));
		PISTONLOG(LOG("counting " << s_blocks[TYPE(adj)]->name << " at " << POS2(pos, front * (i + 1))));
		switch (s_blocks[TYPE(adj)]->geometry) {
			case geometry::none:
				if (TYPE(adj) == blocks::chest) {
					return (13);
				}
			case geometry::cross:
				if (TYPE(adj) == blocks::piston_head) {
					return (13);
				}
			case geometry::crop:
			case geometry::torch:
			case geometry::button:
			case geometry::lever:
			case geometry::dust:
			case geometry::repeater:
				if (value & mask::redstone::piston::moving) { // did not finish retraction
					size_t eSize = _entities.size(), delCount = 0;
					int res = 0;
					for (size_t index = 0; index < eSize; ++index) {
						int status = _entities[index]->pistonedBy(pos, pos + front);
						if (status == redstone::piston::force_retraction) { // another piston pushed a block in front of this one before it finished retracting
							PISTONLOG(LOG("(void)FORCE FINISH RETRACTION BY OTHER PISTON"));
							res = 13;
							++delCount;
						} else if (status) { // if true, pistonedBy places back the moving blocks, _softKill set to true
							PISTONLOG(LOG("(void)FORCE FINISH RETRACTION"));
							++delCount;
							if (status == redstone::piston::cancel_retraction) {
								res = (res == 13) ? 13 : status;
							}
						}
					}
					if (delCount > 0) {
						return ((res) ? res : delCount - 1);
					}
				}
				return (count);
			case geometry::piston: // can't move extended piston nor blocks being moved
				if (TYPE(adj) == blocks::moving_piston) {
					if (!count && (adj & mask::redstone::piston::retracting)) { // we check if extend called before own retraction finished
						size_t eSize = _entities.size(), delCount = 0;
						int res = 0;
						for (size_t index = 0; index < eSize; ++index) {
							int status = _entities[index]->pistonedBy(pos, pos + front);
							if (status == redstone::piston::force_retraction) { // another piston pushed a block in front of this one before it finished retracting
								PISTONLOG(LOG("FORCE FINISH RETRACTION BY OTHER PISTON"));
								res = 13;
								++delCount;
							} else if (status) { // if true, pistonedBy places back the moving blocks, _softKill set to true
								PISTONLOG(LOG("FORCE FINISH RETRACTION"));
								++delCount;
								if (status == redstone::piston::cancel_retraction) {
									res = (res == 13) ? 13 : status;
								}
							}
						}
						if (delCount > 0) {
							return ((res) ? res : delCount - 1);
						}
					}
					return (13);
				}
				if (adj & mask::redstone::piston::moving) {
					return (13);
				}
				break ;
			case geometry::lectern:
				return (13);
			case geometry::cube:
				switch (TYPE(adj)) {
					case blocks::bedrock:
					case blocks::furnace:
						return (13);
				}
				break ;
			default:
				break ;
		}
		++count;
	}
	return (count);
}

/**
 * @brief called once we know piston can be moved << update piston targeted blocks.
 * Transform all moved blocks into entities and replace their world value with blocks::moving_piston
 * after 2 ticks the MOVING_PISTON will be replaced by the entities
 * @param pos relative pos of piston
 * @param value data_value of piston
 * @param count nbr of blocks impacted by movement
*/
void Chunk::extendPiston( glm::ivec3 pos, int value, int count )
{
	const glm::ivec3 front = adj_blocks[(value >> offset::blocks::orientation) & 0x7];
	PISTONLOG(LOG("extend piston, front is " << POS(front)));

	for (int i = count - 1; i >= 0; --i) {
		int adj = getBlockAt(pos + front * (i + 1));
		// creates moving entity for the blocks being pushed
		PISTONLOG(LOG(s_blocks[TYPE(adj)]->name << " turned into entity at " << POS2(pos, front * (i + 1))));
		_entities.push_back(std::make_shared<MovingPistonEntity>(this, pos, pos + front * (i + 1), front, false, false, adj & (mask::all_bits - mask::blocks::notVisible)));
		setBlockAt(blocks::moving_piston, pos + front * (i + 2), true);
	}
	// this one is for the piston head
	PISTONLOG(LOG("piston head at " << POSDXY(pos, _startX, _startY)));
	_entities.push_back(std::make_shared<MovingPistonEntity>(this, pos, pos, front, false, false, blocks::piston_head | (value & (0x7 << offset::blocks::orientation)) | ((TYPE(value) == blocks::sticky_piston) ? mask::redstone::piston::sticky : 0)));
	setBlockAt(blocks::moving_piston, pos + front, true);
}

/**
 * @brief checks if given block can be pulled by sticky piston
*/
static bool pullableBlock( int value )
{
	int type = TYPE(value);
	switch (s_blocks[type]->geometry) {
		case geometry::none:
		case geometry::cross:
		case geometry::crop:
		case geometry::torch:
		case geometry::button:
		case geometry::lever:
		case geometry::dust:
		case geometry::lectern:
		case geometry::repeater:
		case geometry::door:
		case geometry::trapdoor:
			return (false);
		case geometry::piston:
			return (type != blocks::moving_piston && !(value & mask::redstone::piston::moving));
		default:
			break ;
	}
	switch (type) {
		case blocks::moving_piston:
		case blocks::bedrock:
		case blocks::furnace:
		case blocks::crafting_table:
			return (false);
	}
	return (true);
}

/**
 * @brief retract piston at given pos, first check if piston finished its extension,
 * if not place entities being moved at their final pos and retract piston head
 * (this allows sticky_piston to not retract blocks if updated within 1 redstone tick)
*/
void Chunk::retractPiston( glm::ivec3 pos, int value )
{
	const glm::ivec3 front = adj_blocks[(value >> offset::blocks::orientation) & 0x7];
	int front_value = getBlockAt(pos.x + front.x, pos.y + front.y, pos.z + front.z);
	size_t delCount = 0;
	if (TYPE(front_value) == blocks::moving_piston) { // force finish extension
		size_t eSize = _entities.size();
		for (size_t index = 0; index < eSize; ++index) {
			if (_entities[index]->pistonedBy(pos, {0, 0, -1000})) { // if true, pistonedBy places back the moving blocks, _softKill set to true
				PISTONLOG(LOG("FORCE FINISH EXTENSION"));
				++delCount;
			}
		}
	} else if (TYPE(front_value) != blocks::piston_head) { // retract called before extend could even begin
		setBlockAt(value, pos, false, false); // restore value
		PISTONLOG(LOG("\t.. no change (after double change)"));
		return ;
	}
	if (TYPE(value) == blocks::sticky_piston && delCount < 2) { // retract block only if had time to finish push or didn't push block in the first place
		int front2_value = getBlockAt(pos.x + front.x * 2, pos.y + front.y * 2, pos.z + front.z * 2);
		if (pullableBlock(front2_value)) {
		// if ((!s_blocks[TYPE(front2_value)]->transparent && TYPE(front2_value) != blocks::moving_piston && TYPE(front2_value) != blocks::bedrock)
		// 	|| TYPE(front2_value) == blocks::observer
		// 	|| ((TYPE(front2_value) == blocks::piston || TYPE(front2_value) == blocks::sticky_piston) && !(front2_value & mask::redstone::piston::moving))) {
			setBlockAt(blocks::air, pos + front * 2, true);
			setBlockAt(blocks::moving_piston | mask::redstone::piston::retracting, pos + front, true);
			_entities.push_back(std::make_shared<MovingPistonEntity>(this, pos, pos + front * 2, -front, false, true, front2_value));
		} else {
			setBlockAt(blocks::air, pos + front, true);
		}
	} else {
		setBlockAt(blocks::air, pos + front, true);
	}
	_entities.push_back(std::make_shared<MovingPistonEntity>(this, pos, pos + front, -front, true, true, blocks::piston_head | (value & (0x7 << offset::blocks::orientation))| ((TYPE(value) == blocks::sticky_piston) ? mask::redstone::piston::sticky : 0)));
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
		if (_neighbours[face_dir::minus_x]) {
			pos.x += settings::consts::chunk_size;
			_neighbours[face_dir::minus_x]->updatePiston(pos, value);
		}
		return ;
	} else if (pos.x >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_x]) {
			pos.x -= settings::consts::chunk_size;
			_neighbours[face_dir::plus_x]->updatePiston(pos, value);
		}
		return ;
	} else if (pos.y < 0) {
		if (_neighbours[face_dir::minus_y]) {
			pos.y += settings::consts::chunk_size;
			_neighbours[face_dir::minus_y]->updatePiston(pos, value);
		}
		return ;
	} else if (pos.y >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_y]) {
			pos.y -= settings::consts::chunk_size;
			_neighbours[face_dir::plus_y]->updatePiston(pos, value);
		}
		return ;
	}

	const glm::ivec3 front = adj_blocks[(value >> offset::blocks::orientation) & 0x7];
	bool powered = getRedstoneStrength(pos, front, redstone::off, true);
	if (!powered && (!(value & mask::redstone::piston::moving) || front.z != 1 || TYPE(getBlockAt(pos + front)) != blocks::piston_head)) {
		// not powered AND (closed piston OR not oriented towards +z OR still extending)
		powered = getRedstoneStrength(pos + glm::ivec3(0, 0, 1), {0, 0, -1}, redstone::off, true);
		// if (powered) std::cout << "\tbtw, piston on next line powered by block above" << std::endl;
	}
	PISTONLOG(LOG("piston update " << POSDXY(pos, _startX, _startY) << " [" << DayCycle::Get()->getGameTicks() << "] powered: " << powered));
	if (powered && !(value & mask::redstone::activated)) {
		// check if extension possible
		int count = pistonExtendCount(pos, value);
	PISTONLOG(LOG("count is " << count));
		if (count <= 12) {
			setBlockAt(value | mask::redstone::activated | mask::redstone::piston::moving, pos, false);
			extendPiston(pos, value, count);
		} else if (count == redstone::piston::cancel_retraction) { // 0 tick update between retraction and extension -> we don't retract
			setBlockAt(value | mask::redstone::activated | mask::redstone::piston::moving, pos, false);
		}
	} else if (!powered && (value & mask::redstone::activated)) {
		setBlockAt((value & (mask::all_bits - mask::redstone::activated)) | mask::redstone::piston::moving, pos, false, false);
		retractPiston(pos, value);
	} else {
		PISTONLOG(LOG("\t.. no change"));
	}
}

/**
 * @brief called by schedule::tickUpdate, execute given schedule
*/
void Chunk::updateRedstone( int priority, schedule::t_redstoneTick red )
{
	int state;
	glm::ivec3 front;

	int value = getBlockAt(red.pos);
	REDUPLOG(LOG("(" << (-3 + priority) <<  ") [" << DayCycle::Get()->getGameTicks() << "] " << _startX << " " << _startY << " schedule " << s_blocks[TYPE(value)]->name << " at " << POSDXY(red.pos, _startX, _startY) << ": " << (red.state & 0xF)));;

	switch (TYPE(value)) {
		case blocks::redstone_lamp: // only scheduled to be turned off
			// std::cout << "update redstone lamp at " << red.pos.x << ", " << red.pos.y << ", " << red.pos.z << std::endl;
			state = getRedstoneStrength(red.pos, {0, 0, 0}, redstone::off, true);
			if (!state && (value & mask::redstone::activated)) { // turn it off
				short level = getLightLevel(red.pos.x, red.pos.y, red.pos.z) & 0xFF0F;
				setLightLevel(level, red.pos.x, red.pos.y, red.pos.z, true);
				startLightSpread(red.pos.x, red.pos.y, red.pos.z, false);
				setBlockAt(value & (mask::all_bits - mask::redstone::activated - mask::redstone::powered - mask::redstone::weakdy_powered - mask::redstone::strength), red.pos.x, red.pos.y, red.pos.z, false);
				weaklyPower(red.pos, {0, 0, 0}, redstone::off, false);
			}
			break ;
		case blocks::repeater:
			if (value & mask::redstone::repeater::lock) { // repeater is locked, we don't update it
				// std::cout << "repeater is locked." << std::endl;
				break ;
			}
			front = adj_blocks[(value >> offset::blocks::orientation) & 0x7];
			state = red.state & 0xF;  // get rid of info 'scheduled tick caused by lock/unlock'
			if (state) {
				schedule::extendRepeaterDelay(priority, value, {this, red});
				if (!(value & mask::redstone::powered)) { // turn repeater on
					// std::cout << "repeater on" << std::endl;
					value |= mask::redstone::powered;
					setBlockAt(value, red.pos.x, red.pos.y, red.pos.z, false);
					stronglyPower(red.pos + front, -front, 0xF);
					weaklyPowerTarget(red.pos + front, -front, redstone::on, false);
					if (red.state & mask::redstone::repeater::lock) { initRepeater(red.pos, value, true); }
				}
			} else if (value & mask::redstone::powered) { // turn repeater off
				// std::cout << "repeater off" << std::endl;
				value &= (mask::all_bits - mask::redstone::powered);
				setBlockAt(value, red.pos.x, red.pos.y, red.pos.z, false);
				stronglyPower(red.pos + front, -front, redstone::off);
				weaklyPowerTarget(red.pos + front, -front, redstone::off, false);
				initRepeater(red.pos, value, true);
			}
			break ;
		case blocks::comparator:
			updateComparator(red.pos, value, true);
			break ;
		case blocks::observer:
			front = adj_blocks[(value >> offset::blocks::orientation) & 0x7];
			REDUPLOG(REDLOG("observer front " << POS(front)));
			if (red.state && !(value & mask::redstone::activated)) {
				REDUPLOG(REDLOG("observer activation"));
				setBlockAt(value | mask::redstone::activated, red.pos, false);
				stronglyPower(red.pos - front, front, 0xF);
				weaklyPowerTarget(red.pos - front, front, redstone::on, false);
				scheduleRedstoneTick({red.pos, redstone::tick, redstone::off});
			} else if (value & mask::redstone::activated) {
				REDUPLOG(REDLOG("observer shut down"));
				setBlockAt(value & (mask::all_bits - mask::redstone::activated), red.pos, false);
				stronglyPower(red.pos - front, front, redstone::off);
				weaklyPowerTarget(red.pos - front, front, redstone::off, false);
			}
			break ;
		case blocks::redstone_torch:
			updateRedstoneTorch(red.pos, value);
			break ;
		case blocks::stone_button:
		case blocks::oak_button:
			front = getAttachedDir(value);
			setBlockAt(value & (-1 - mask::redstone::powered - mask::redstone::strength), red.pos, false);
			stronglyPower(red.pos + front, -front, redstone::off);
			weaklyPower(red.pos, {0, 0, 0}, redstone::off, false);
			break ;
		default:
			REDUPLOG(LOG("Chunk::updateRedstone updated: " << s_blocks[TYPE(value)]->name));
			break ;
	}
	REDUPLOG(LOG("\t\t" << s_blocks[TYPE(value)]->name << " schedule over."));
}

/**
 * @brief push_back redstone schedule in appropriate chunk
 * @param red schedule to be pushed
*/
void Chunk::scheduleRedstoneTick( schedule::t_redstoneTick red )
{
	if (red.pos.z < 0 || red.pos.z >= settings::consts::world_height) {
		return ;
	}
	if (red.pos.x < 0) {
		if (_neighbours[face_dir::minus_x]) {
			red.pos.x += settings::consts::chunk_size;
			_neighbours[face_dir::minus_x]->scheduleRedstoneTick(red);
			return ;
		}
	} else if (red.pos.x >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_x]) {
			red.pos.x -= settings::consts::chunk_size;
			_neighbours[face_dir::plus_x]->scheduleRedstoneTick(red);
			return ;
		}
	} else if (red.pos.y < 0) {
		if (_neighbours[face_dir::minus_y]) {
			red.pos.y += settings::consts::chunk_size;
			_neighbours[face_dir::minus_y]->scheduleRedstoneTick(red);
			return ;
		}
	} else if (red.pos.y >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_y]) {
			red.pos.y -= settings::consts::chunk_size;
			_neighbours[face_dir::plus_y]->scheduleRedstoneTick(red);
			return ;
		}
	} else {
		int priority = schedule::priority::other;
		int target = getBlockAt(red.pos.x, red.pos.y, red.pos.z);
		if (TYPE(target) == blocks::repeater) {
			const glm::vec3 front = adj_blocks[(target >> offset::blocks::orientation) & 0x7];
			int front_value = getBlockAt(red.pos.x + front.x, red.pos.y + front.y, red.pos.z + front.z);
			priority = (TYPE(front_value) == blocks::repeater || TYPE(front_value) == blocks::comparator) ? schedule::priority::repeat_diode
						: (red.state) ? schedule::priority::repeat_on : schedule::priority::repeat_off;
			int other = (priority == schedule::priority::repeat_diode) ? schedule::priority::repeat_diode : (priority ^ 0x3);
			if (schedule::abortRepeaterScheduleTick(other, {this, red})) {
				return ;
			}
		} else if (TYPE(target) == blocks::comparator) {
			priority = schedule::priority::comparator; // comparator always has priority -1 (but I do -0.5f instead to prioretize repeaters)
		}
		REDUPLOG(LOG("new schedule " << s_blocks[TYPE(target)]->name << " (" << (-3 + priority) << ") [" << DayCycle::Get()->getGameTicks() << "] " << _startX << " " << _startY << " pos " << POSDXY(red.pos, _startX, _startY) << " ticks: " << red.ticks << " state " << (red.state & 0xF) << ((red.state & mask::redstone::repeater::lock) ? " lock" : "")));
		schedule::scheduleRedstoneTick(priority, target, {this, red});
	}
}

/**
 * @brief look if comparator at given pos has already been scheduled this tick
 * in which case we abort it
 * @param pos relative pos of comparator
*/
void Chunk::abortComparatorScheduleTick( glm::ivec3 pos )
{
	if (pos.z < 0 || pos.z >= settings::consts::world_height) {
		return ;
	}
	if (pos.x < 0) {
		if (_neighbours[face_dir::minus_x]) {
			pos.x += settings::consts::chunk_size;
			_neighbours[face_dir::minus_x]->abortComparatorScheduleTick(pos);
			return ;
		}
	} else if (pos.x >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_x]) {
			pos.x -= settings::consts::chunk_size;
			_neighbours[face_dir::plus_x]->abortComparatorScheduleTick(pos);
			return ;
		}
	} else if (pos.y < 0) {
		if (_neighbours[face_dir::minus_y]) {
			pos.y += settings::consts::chunk_size;
			_neighbours[face_dir::minus_y]->abortComparatorScheduleTick(pos);
			return ;
		}
	} else if (pos.y >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_y]) {
			pos.y -= settings::consts::chunk_size;
			_neighbours[face_dir::plus_y]->abortComparatorScheduleTick(pos);
			return ;
		}
	} else {
		schedule::abortComparatorScheduleTick(this, pos);
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
	int value_mx = getBlockAt(pos.x - 1, pos.y, pos.z), tmx = TYPE(value_mx);
	if (tmx == blocks::lever || tmx == blocks::redstone_dust || tmx == blocks::redstone_torch || tmx == blocks::target
		|| tmx == blocks::comparator || (tmx == blocks::repeater && ((value_mx >> offset::blocks::orientation) & 0x2))) {
		connect_mx |= redstone::dust::side;
		if (placed && tmx == blocks::redstone_dust && !(value_mx & (redstone::dust::side << offset::redstone::dust::px))) { // adj dust not linked towards new dust
			connectRedstoneDust(pos + glm::ivec3(-1, 0, 0), value_mx, false);
			setBlockAt(value_mx, pos.x - 1, pos.y, pos.z, false);
		}
	} else if (s_blocks[tmx]->transparent) {
		value_mx = getBlockAt(pos.x - 1, pos.y, pos.z - 1);
		if (TYPE(value_mx) == blocks::redstone_dust) {
			connect_mx |= redstone::dust::side;
			if (placed) {
				connectRedstoneDust(pos + glm::ivec3(-1, 0, -1), value_mx, false);
				setBlockAt(value_mx, pos.x - 1, pos.y, pos.z - 1, false);
			}
		}
		if (tmx == blocks::glass) { // hardcoded for now ..
			goto GLASS_MX;
		}
	} else {
GLASS_MX:
		if (s_blocks[TYPE(getBlockAt(pos.x, pos.y, pos.z + 1))]->transparent) {
			value_mx = getBlockAt(pos.x - 1, pos.y, pos.z + 1);
			if (TYPE(value_mx) == blocks::redstone_dust) {
				connect_mx |= redstone::dust::up;
				if (placed) {
					connectRedstoneDust(pos + glm::ivec3(-1, 0, 1), value_mx, false);
					setBlockAt(value_mx, pos.x - 1, pos.y, pos.z + 1, false);
				}
			}
		}
	}

	int value_px = getBlockAt(pos.x + 1, pos.y, pos.z), tpx = TYPE(value_px);
	if (tpx == blocks::lever || tpx == blocks::redstone_dust || tpx == blocks::redstone_torch || tpx == blocks::target
		|| tpx == blocks::comparator || (tpx == blocks::repeater && ((value_px >> offset::blocks::orientation) & 0x2))) {
		connect_px |= redstone::dust::side;
		if (placed && tpx == blocks::redstone_dust && !(value_px & (redstone::dust::side << offset::redstone::dust::mx))) { // adj dust not linked towards new dust
			connectRedstoneDust(pos + glm::ivec3(1, 0, 0), value_px, false);
			setBlockAt(value_px, pos.x + 1, pos.y, pos.z, false);
		}
	} else if (s_blocks[tpx]->transparent) {
		value_px = getBlockAt(pos.x + 1, pos.y, pos.z - 1);
		if (TYPE(value_px) == blocks::redstone_dust) {
			connect_px |= redstone::dust::side;
			if (placed) {
				connectRedstoneDust(pos + glm::ivec3(1, 0, -1), value_px, false);
				setBlockAt(value_px, pos.x + 1, pos.y, pos.z - 1, false);
			}
		}
		if (tpx == blocks::glass) { // hardcoded for now ..
			goto GLASS_PX;
		}
	} else {
GLASS_PX:
		if (s_blocks[TYPE(getBlockAt(pos.x, pos.y, pos.z + 1))]->transparent) {
			value_px = getBlockAt(pos.x + 1, pos.y, pos.z + 1);
			if (TYPE(value_px) == blocks::redstone_dust) {
				connect_px |= redstone::dust::up;
				if (placed) {
					connectRedstoneDust(pos + glm::ivec3(1, 0, 1), value_px, false);
					setBlockAt(value_px, pos.x + 1, pos.y, pos.z + 1, false);
				}
			}
		}
	}

	int value_my = getBlockAt(pos.x, pos.y - 1, pos.z), tmy = TYPE(value_my);
	if (tmy == blocks::lever || tmy == blocks::redstone_dust || tmy == blocks::redstone_torch || tmy == blocks::target
		|| tmy == blocks::comparator || (tmy == blocks::repeater && !((value_my >> offset::blocks::orientation) & 0x2))) {
		connect_my |= redstone::dust::side;
		if (placed && tmy == blocks::redstone_dust && !(value_my & (redstone::dust::side << offset::redstone::dust::py))) { // adj dust not linked towards new dust
			connectRedstoneDust(pos + glm::ivec3(0, -1, 0), value_my, false);
			setBlockAt(value_my, pos.x, pos.y - 1, pos.z, false);
		}
	} else if (s_blocks[tmy]->transparent) {
		value_my = getBlockAt(pos.x, pos.y - 1, pos.z - 1);
		if (TYPE(value_my) == blocks::redstone_dust) {
			connect_my |= redstone::dust::side;
			if (placed) {
				connectRedstoneDust(pos + glm::ivec3(0, -1, -1), value_my, false);
				setBlockAt(value_my, pos.x, pos.y - 1, pos.z - 1, false);
			}
		}
		if (tmy == blocks::glass) { // hardcoded for now ..
			goto GLASS_MY;
		}
	} else {
GLASS_MY:
		if (s_blocks[TYPE(getBlockAt(pos.x, pos.y, pos.z + 1))]->transparent) {
			value_my = getBlockAt(pos.x, pos.y - 1, pos.z + 1);
			if (TYPE(value_my) == blocks::redstone_dust) {
				connect_my |= redstone::dust::up;
				if (placed) {
					connectRedstoneDust(pos + glm::ivec3(0, -1, 1), value_my, false);
					setBlockAt(value_my, pos.x, pos.y - 1, pos.z + 1, false);
				}
			}
		}
	}

	int value_py = getBlockAt(pos.x, pos.y + 1, pos.z), tpy = TYPE(value_py);
	if (tpy == blocks::lever || tpy == blocks::redstone_dust || tpy == blocks::redstone_torch || tpy == blocks::target
		|| tpy == blocks::comparator || (tpy == blocks::repeater && !((value_py >> offset::blocks::orientation) & 0x2))) {
		connect_py |= redstone::dust::side;
		if (placed && tpy == blocks::redstone_dust && !(value_py & (redstone::dust::side << offset::redstone::dust::my))) { // adj dust not linked towards new dust
			connectRedstoneDust(pos + glm::ivec3(0, 1, 0), value_py, false);
			setBlockAt(value_py, pos.x, pos.y + 1, pos.z, false);
		}
	} else if (s_blocks[tpy]->transparent) {
		value_py = getBlockAt(pos.x, pos.y + 1, pos.z - 1);
		if (TYPE(value_py) == blocks::redstone_dust) {
			connect_py |= redstone::dust::side;
			if (placed) {
				connectRedstoneDust(pos + glm::ivec3(0, 1, -1), value_py, false);
				setBlockAt(value_py, pos.x, pos.y + 1, pos.z - 1, false);
			}
		}
		if (tpy == blocks::glass) { // hardcoded for now ..
			goto GLASS_PY;
		}
	} else {
GLASS_PY:
		if (s_blocks[TYPE(getBlockAt(pos.x, pos.y, pos.z + 1))]->transparent) {
			value_py = getBlockAt(pos.x, pos.y + 1, pos.z + 1);
			if (TYPE(value_py) == blocks::redstone_dust) {
				connect_py |= redstone::dust::up;
				if (placed) {
					connectRedstoneDust(pos + glm::ivec3(0, 1, 1), value_py, false);
					setBlockAt(value_py, pos.x, pos.y + 1, pos.z + 1, false);
				}
			}
		}
	}

	if (connect_mx) {
		value |= (connect_mx << offset::redstone::dust::mx);
		if (!(connect_my | connect_py | connect_px)) {
			value |= (redstone::dust::side << offset::redstone::dust::px);
		}
	}
	if (connect_px) {
		value |= (connect_px << offset::redstone::dust::px);
		if (!(connect_my | connect_py | connect_mx)) {
			value |= (redstone::dust::side << offset::redstone::dust::mx);
		}
	}
	if (connect_my) {
		value |= (connect_my << offset::redstone::dust::my);
		if (!(connect_mx | connect_px | connect_py)) {
			value |= (redstone::dust::side << offset::redstone::dust::py);
		}
	}
	if (connect_py) {
		value |= (connect_py << offset::redstone::dust::py);
		if (!(connect_mx | connect_px | connect_my)) {
			value |= (redstone::dust::side << offset::redstone::dust::my);
		}
	}
}
