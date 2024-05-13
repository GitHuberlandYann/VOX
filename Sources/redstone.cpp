#include "Chunk.hpp"

/*
 Some info:

 - To strongly power a block: it will weakly power its own adj blocks
 - To weakly power a block:   it activates if it is a redstone componant, else does nothing
 - adj blocks == 6 cardinals only

 - A redstone lamp:
	- activates instantly, but takes 2 redstone ticks to turn off.

 - redstone torch:
	- can be deactivated by powering block it is placed on
	- has 1 tick of delay
	- does NOT power block it is placed on
	- strongly powers block ABOVE it, weakly powers adj blocks

 - lever:
	- stronly powers block it is placed on, weakly powers ajd blocks

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
		// case blocks::REDSTONE_TORCH:
		// 	break ;
	}
	return target;
}

/**
 * @brief check adj blocks to return whether block at given pos is currently powered.
 * to future me: no you can't just stock info in its data_value because one block can be
 * powered from several sources at the same time, so when you turn off one of the sources,
 * it can still be powered afterwards
 * @param pos block to get state of
 * @param except this given dir is not checked
 * @param state this is state of given dir, if on, no need to check further
 * @param weak true if a weakly powered state is accepted
 * @return true if given pos is ((weak) ? WEAKLY : STRONGLY | WEAKLY) powered
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
		switch (adj & 0xFF) {
			case blocks::LEVER:
				if (adj & REDSTONE::POWERED) {
					if (getAttachedDir(adj) == -delta || weak) {
						return (REDSTONE::ON);
					}
				}
				break ;
			// case blocks::REDSTONE_TORCH:
			// 	if (delta.z == -1) {
			// 		return (REDSTONE::ON);
			// 	} else if (weak && delta != torch placement)
			// 		return (REDSTONE::ON);
			// 	break ;
			default:
				if (weak && (adj & REDSTONE::POWERED)) {
					return (REDSTONE::ON);
				}
				break ;
		}
	}
	return (REDSTONE::OFF);
}

/**
 * @brief check adj blocks to given pos, and update redstone componants if found
 * @param pos source of power
 * @param except this given dir is not checked
 * @param state whether weak power is turned on or off
*/
void Chunk::weaklyPower( glm::ivec3 pos, glm::ivec3 except, bool state )
{
	for (int index = 0; index < 6; ++index) {
		const glm::ivec3 delta = adj_blocks[index];
		if (delta == except) continue ;

		int adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z, true);
		if (s_blocks[adj & 0xFF]->redstoneComponant) {
			switch (adj & 0xFF) {
				case blocks::REDSTONE_LAMP:
					if (state && !(adj & REDSTONE::ACTIVATED)) {
						std::cout << "\tactivate redstone lamp at " << pos.x + delta.x << ", " << pos.y + delta.y << ", " << pos.z + delta.z << std::endl;
						// turn lamp on instantly
						short level = getLightLevel(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z) & 0xFF00;
						level += s_blocks[blocks::REDSTONE_LAMP]->light_level + (s_blocks[blocks::REDSTONE_LAMP]->light_level << 4);
						setLightLevel(level, pos.x + delta.x, pos.y + delta.y, pos.z + delta.z, true);
						startLightSpread(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z, false);
						setBlockAt(adj | REDSTONE::ACTIVATED, pos.x + delta.x, pos.y + delta.y, pos.z + delta.z, true);
					} else if (!state && (adj & REDSTONE::ACTIVATED)) {
						// add lamp to tick delay machine
						std::cout << "\tschedule redstone lamp at " << pos.x + delta.x << ", " << pos.y + delta.y << ", " << pos.z + delta.z << std::endl;
						_redstone_schedule.push_back({pos + delta, 2 * REDSTONE::TICK});
					}
					break ;
				case blocks::TNT:
					handleHit(false, blocks::TNT, pos + delta + glm::ivec3(_startX, _startY, 0), Modif::LITNT);
					break ;
			}
		}
	}
}


	// for (int dx = -1; dx <= 1; ++dx) {
	// 	for (int dy = -1; dy <= 1; ++dy) {
	// 		for (int dz = -1; dz <= 1; ++dz) {
	// 			if (!dx && !dy && !dz) continue ;
	// 			int block_value = getBlockAt(pos.x + dx, pos.y + dy, pos.z + dz, true);
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
	// 				case GEOMETRY::CUBE:
	// 					if ((block_value & 0xFF) == blocks::REDSTONE_LAMP) {
	// 						int offset = ((((pos.x + dx) << CHUNK_SHIFT) + (pos.y + dy)) << WORLD_SHIFT) + pos.z + dz;
	// 						_lights[offset] &= 0xFF00;
	// 						_lights[offset] += s_blocks[blocks::REDSTONE_LAMP]->light_level + ((state) ? (s_blocks[blocks::REDSTONE_LAMP]->light_level << 4) : 0);
	// 						light_spread(pos.x + dx, pos.y + dy, pos.z + dz, false);
	// 						_light_update = false;
		
	// 						(state) ? block_value |= REDSTONE::ACTIVATED : block_value ^= (1 << REDSTONE::ACTIVATED_OFFSET);
	// 						if (dx == target.x && dy == target.y && dz == target.z) {
	// 							(state) ? block_value |= REDSTONE::POWERED : block_value ^= (1 << REDSTONE::POWERED_OFFSET);
	// 						}
	// 						setBlockAt(block_value, pos.x + dx, pos.y + dy, pos.z + dz, true);
	// 					} else if ((block_value && 0xFF) == blocks::TNT) {
	// 						this->handleHit(Modif::LITNT);
	// 					}
	// 					break ;
	// 			}
	// 		}
	// 	}
	// }

/**
 * @brief updates blocks around lever at 'pos' according to its state
 * @param pos relative position of lever inside chunk
 * @param value Data value of block containing lever, used to get its orientation
 * @param state new state of lever
*/
void Chunk::flickLever( glm::ivec3 pos, int value, bool state )
{
	glm::ivec3 target = getAttachedDir(value);
std::cout << "flick lever " << state << std::endl;
	// if target block is !transparent, we 'strongly' power it, it in turns weakly powers its adjacent redstone componants
	int target_value = getBlockAt(pos.x + target.x, pos.y + target.y, pos.z + target.z, true);
	if (!s_blocks[target_value & 0xFF]->transparent) {
		bool target_state = getRedstoneState(pos + target, -target, state, false);
		(target_state) ? target_value |= REDSTONE::POWERED : target_value &= (INT_MAX - REDSTONE::POWERED);
		(target_state) ? std::cout << "\tpower block at " << pos.x + target.x << ", " << pos.y + target.y << ", " << pos.z + target.z << std::endl
						: std::cout << "\tunpower block at " << pos.x + target.x << ", " << pos.y + target.y << ", " << pos.z + target.z << std::endl;
		setBlockAt(target_value, pos.x + target.x, pos.y + target.y, pos.z + target.z, true);
		weaklyPower(pos + target, -target, target_state);
	}

	// also, adjacent redstone componants are weakly powered
	weaklyPower(pos, {0, 0, 0}, state);
std::cout << "END flick lever " << state << std::endl;
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
					std::cout << "update redstone lamp at " << red.pos.x << ", " << red.pos.y << ", " << red.pos.z << std::endl;
					state = getRedstoneState(red.pos, {0, 0, 0}, false, true);
					if (!state && (value & REDSTONE::ACTIVATED)) { // turn it off
						short level = getLightLevel(red.pos.x, red.pos.y, red.pos.z) & 0xFF0F;
						setLightLevel(level, red.pos.x, red.pos.y, red.pos.z, true);
						startLightSpread(red.pos.x, red.pos.y, red.pos.z, false);
						setBlockAt(value & (INT_MAX - REDSTONE::ACTIVATED), red.pos.x, red.pos.y, red.pos.z, true);
					}
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
