#include "Chunk.hpp"

/**
 * @brief updates blocks around lever at 'pos' according to its state
 * @param pos relative position of lever inside chunk
 * @param state new state of lever
*/
void Chunk::flickLever( glm::ivec3 pos, bool state )
{
	for (int dx = -1; dx <= 1; ++dx) {
		for (int dy = -1; dy <= 1; ++dy) {
			for (int dz = -1; dz <= 1; ++dz) {
				if (!dx && !dy && !dz) continue ;
				// int offset = ((((pos.x + dx) << CHUNK_SHIFT) + (pos.y + dy)) << WORLD_SHIFT) + pos.z;
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
							(state) ? setBlockAt(block_value | REDSTONE::POWERED, pos.x + dx, pos.y + dy, pos.z + dz, true)
									: setBlockAt(block_value ^ (1 << REDSTONE::POWERED_OFFSET), pos.x + dx, pos.y + dy, pos.z + dz, true);
						}
						break ;
				}
			}
		}
	}
}
