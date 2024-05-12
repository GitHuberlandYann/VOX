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
					return (REDSTONE::ON);
				}
				break ;
			// case blocks::REDSTONE_TORCH:
			// 	if (delta.z == -1) {
			// 		return (REDSTONE::ON);
			// 	}
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
						// turn lamp on instantly
						// TODO handle chunk border!!!!! perhaps with getLightLevel and new? setLightLevel and light_spread_butCheckIfInGoodChunkInSomeWay
						int offset = ((((pos.x + delta.x) << CHUNK_SHIFT) + (pos.y + delta.y)) << WORLD_SHIFT) + pos.z + delta.z;
						_lights[offset] &= 0xFF00;
						_lights[offset] += s_blocks[blocks::REDSTONE_LAMP]->light_level + (s_blocks[blocks::REDSTONE_LAMP]->light_level << 4);
						light_spread(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z, false);
						_light_update = false;
						setBlockAt(adj | REDSTONE::ACTIVATED, pos.x + delta.x, pos.y + delta.y, pos.z + delta.z, true);
					} else if (!state && (adj & REDSTONE::ACTIVATED)) {
						// add lamp to tick delay machine

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
	int orientation = (value >> 9) & 0x7;
	int placement = (value >> 12) & 0x3;
	glm::ivec3 target;
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

	// if target block is !transparent, we 'strongly' power it, it in turns weakly powers its adjacent redstone componants
	int target_value = getBlockAt(pos.x + target.x, pos.y + target.y, pos.z + target.z, true);
	if (!s_blocks[target_value & 0xFF]->transparent) {
		bool target_state = getRedstoneState(pos + target, -target, state, false);
		(target_state) ? target_value |= REDSTONE::POWERED : target_value &= (INT_MAX - REDSTONE::POWERED);
		setBlockAt(target_value, pos.x + target.x, pos.y + target.y, pos.z + target.z, true);
		weaklyPower(pos + target, -target, target_state);
	}

	// also, adjacent redstone componants are weakly powered
	weaklyPower(pos, {0, 0, 0}, state);
}
