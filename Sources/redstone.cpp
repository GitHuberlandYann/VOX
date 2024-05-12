#include "Chunk.hpp"

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
	for (int dx = -1; dx <= 1; ++dx) {
		for (int dy = -1; dy <= 1; ++dy) {
			for (int dz = -1; dz <= 1; ++dz) {
				if (!dx && !dy && !dz) continue ;
				int block_value = getBlockAt(pos.x + dx, pos.y + dy, pos.z + dz, true);
				switch (s_blocks[block_value & 0xFF]->geometry) {
					case GEOMETRY::TRAPDOOR:
						(state) ? setBlockAt(block_value | REDSTONE::POWERED, pos.x + dx, pos.y + dy, pos.z + dz, true)
								: setBlockAt(block_value ^ (1 << REDSTONE::POWERED_OFFSET), pos.x + dx, pos.y + dy, pos.z + dz, true);
						break ;
					case GEOMETRY::DOOR:
						(state) ? setBlockAt(block_value | REDSTONE::POWERED, pos.x + dx, pos.y + dy, pos.z + dz, true)
								: setBlockAt(block_value ^ (1 << REDSTONE::POWERED_OFFSET), pos.x + dx, pos.y + dy, pos.z + dz, true);
						dz += ((block_value >> 12) & DOOR::UPPER_HALF) ? -1 : 1;
						block_value ^= (DOOR::UPPER_HALF << 12);
						(state) ? setBlockAt(block_value | REDSTONE::POWERED, pos.x + dx, pos.y + dy, pos.z + dz, true)
								: setBlockAt(block_value ^ (1 << REDSTONE::POWERED_OFFSET), pos.x + dx, pos.y + dy, pos.z + dz, true);
						break ;
					case GEOMETRY::CUBE:
						if ((block_value & 0xFF) == blocks::REDSTONE_LAMP) {
							int offset = ((((pos.x + dx) << CHUNK_SHIFT) + (pos.y + dy)) << WORLD_SHIFT) + pos.z + dz;
							_lights[offset] &= 0xFF00;
							_lights[offset] += s_blocks[blocks::REDSTONE_LAMP]->light_level + ((state) ? (s_blocks[blocks::REDSTONE_LAMP]->light_level << 4) : 0);
							light_spread(pos.x + dx, pos.y + dy, pos.z + dz, false);
							_light_update = false;
		
							(state) ? block_value |= REDSTONE::ACTIVATED : block_value ^= (1 << REDSTONE::ACTIVATED_OFFSET);
							if (dx == target.x && dy == target.y && dz == target.z) {
								(state) ? block_value |= REDSTONE::POWERED : block_value ^= (1 << REDSTONE::POWERED_OFFSET);
							}
							setBlockAt(block_value, pos.x + dx, pos.y + dy, pos.z + dz, true);
						}
						break ;
				}
			}
		}
	}
}
