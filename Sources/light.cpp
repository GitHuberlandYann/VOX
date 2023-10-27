#include "vox.h"

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void Chunk::light_spread( GLchar *arr, int posX, int posY, int posZ )
{
	char level = arr[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ];
	// std::cout << "light_spread, level is " << (int)level << std::endl;
	if (!(level & 0x10)) { // not a source block, we check if level should change
		char maxLevel = 0;
		for (int index = 0; index < 6; index++) {
			const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
			if (posX + delta[0] < 0 || posX + delta[0] > CHUNK_SIZE + 1 || posY + delta[1] < 0 || posY + delta[1] > CHUNK_SIZE + 1 || posZ + delta[2] < 0 || posZ + delta[2] > 255) {
			} else if (!air_flower(_blocks[((posX + delta[0]) * (CHUNK_SIZE + 2) + posY + delta[1]) * WORLD_HEIGHT + posZ + delta[2]], true, false)) {
				maxLevel = maxc(maxLevel, arr[((posX + delta[0]) * (CHUNK_SIZE + 2) + posY + delta[1]) * WORLD_HEIGHT + posZ + delta[2]] & 0xF);
			}
		}
		int saveLevel = level;
		if ((maxLevel && maxLevel - 1 != level) || (!maxLevel && level)) {
			level = maxc(0, maxLevel - 1);
			arr[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ] = level;
		} else if (level == saveLevel) {
			return ; // stop spread when not source and level is unchanged
		}
	}
	// std::cout << "level is " << (int)level << std::endl;

	// spread in all 6 directions
	for (int index = 0; index < 6; index++) {
		const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
		if (posX + delta[0] < 0 || posX + delta[0] > CHUNK_SIZE + 1 || posY + delta[1] < 0 || posY + delta[1] > CHUNK_SIZE + 1 || posZ + delta[2] < 0 || posZ + delta[2] > 255) {
		} else if (!air_flower(_blocks[((posX + delta[0]) * (CHUNK_SIZE + 2) + posY + delta[1]) * WORLD_HEIGHT + posZ + delta[2]], true, false)) {
			int neighbour = arr[((posX + delta[0]) * (CHUNK_SIZE + 2) + posY + delta[1]) * WORLD_HEIGHT + posZ + delta[2]];
			// TODO once source blocks of diff levels exist, handle source 15 next ot source 3 correctly..
			if (!(neighbour & 0x10) && ((neighbour & 0xF) != maxc(0, level - 1))) { // || (level & 0x10) only spread to non source blocks whose value is not expected value. source block always spread
				light_spread(arr, posX + delta[0], posY + delta[1], posZ + delta[2]);
			}
		}
	}
}

void Chunk::generate_sky_light( void )
{
	// fill arr with 0, because light_spread uses neighbours - even if not set yet
	for (int index = 0; index < (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * WORLD_HEIGHT; index++) {
		_sky_light[index] = 0;
	}

	for (int row = 0; row < CHUNK_SIZE + 2; row++) {
		for (int col = 0; col < CHUNK_SIZE + 2; col++) {
			char light_level = 15;
			for (int level = WORLD_HEIGHT - 1; level > 0; level--) {
				if (light_level) {
					_sky_light[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = light_level + ((light_level == 0xF) << 4); // we consider blocks directly under sky as light source
					int value = _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level];
					if (air_flower(value, true, false)) { // block hit
						// spread_light watch out for leaves and water light damping..
						light_level = 0;
					} else if (value == blocks::OAK_LEAVES || value >= blocks::WATER) {
						--light_level;
					}
				}
			}
		}
	}
}

void Chunk::generate_block_light( void )
{
	// fill arr with 0, because light_spread uses neighbours - even if not set yet
	for (int index = 0; index < (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * WORLD_HEIGHT; index++) {
		_block_light[index] = 0;
	}

	for (int row = 0; row < CHUNK_SIZE + 2; row++) {
		for (int col = 0; col < CHUNK_SIZE + 2; col++) {
			for (int level = 0; level < WORLD_HEIGHT; level++) {
				char light_level = s_blocks[_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level]].light_level;
				if (light_level) {
					// std::cout << "LIGHT SOURCE [" << _startX << ", " << _startY << "] at [" << row - 1 << ", " << col - 1 << ", " << level << "]" << std::endl;
					_block_light[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = light_level + (1 << 4); // 0x10 = light source. & 0xF = light level
					light_spread(_block_light, row, col, level);
				}
			}
		}
	}
}

// uses sky light and block light to output a shade value
int Chunk::computeLight( int row, int col, int level )
{
	// (void)row;(void)col;(void)level;return (0);
	int blockLightAmplitude = 5; // amount by which light decreases on each block
	int blockLight = _block_light[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] & 0xF;
	int skyLight = _sky_light[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] & 0xF;
	return (blockLightAmplitude * (15 - maxi(skyLight, blockLight)));
}

// https://minecraft.fandom.com/wiki/Light#AO_texture_pattern fct returns (side1 + side2 + corner)
int Chunk::computeShade( int row, int col, int level, std::array<int, 9> offsets )
{
	// (void)row;(void)col;(void)level;(void)offsets;return (0);
	int cornerAmplitude = 17;
	return (cornerAmplitude
			* (!!air_flower(_blocks[((row + offsets[0]) * (CHUNK_SIZE + 2) + col + offsets[1]) * WORLD_HEIGHT + level + offsets[2]], true, false)
			+ !!air_flower(_blocks[((row + offsets[3]) * (CHUNK_SIZE + 2) + col + offsets[4]) * WORLD_HEIGHT + level + offsets[5]], true, false)
			+ !!air_flower(_blocks[((row + offsets[6]) * (CHUNK_SIZE + 2) + col + offsets[7]) * WORLD_HEIGHT + level + offsets[8]], true, false)));
}

void Chunk::fill_vertex_array( void )
{
	int minLightLevel = 30;
	size_t index = 0;
	for (int row = 0; row < CHUNK_SIZE; row++) {
		for (int col = 0; col < CHUNK_SIZE; col++) {
			for (int level = 0; level < WORLD_HEIGHT; level++) {
				GLint block_type = _blocks[((row + 1) * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level];
				if (block_type > blocks::AIR && block_type < blocks::WATER) {
					glm::ivec3 p0 = {_startX + row + 0, _startY + col + 0, level + 1};
					glm::ivec3 p1 = {_startX + row + 1, _startY + col + 0, level + 1};
					glm::ivec3 p2 = {_startX + row + 0, _startY + col + 0, level + 0};
					glm::ivec3 p3 = {_startX + row + 1, _startY + col + 0, level + 0};

					glm::ivec3 p4 = {_startX + row + 0, _startY + col + 1, level + 1};
					glm::ivec3 p5 = {_startX + row + 1, _startY + col + 1, level + 1};
					glm::ivec3 p6 = {_startX + row + 0, _startY + col + 1, level + 0};
					glm::ivec3 p7 = {_startX + row + 1, _startY + col + 1, level + 0};

					if (block_type < blocks::POPPY) {
						int orientation = -1, litFurnace = 0;
						if (block_type >= blocks::CRAFTING_TABLE && block_type < blocks::BEDROCK) {
							auto o = _orientations.find(((row + 1) * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level);
							if (o != _orientations.end()) {
								orientation = (o->second & 0xF);
								litFurnace = ((o->second >> 4) & 0xF) == furnace_state::ON;
							}
						}
						bool isNotLeaves = (block_type != blocks::OAK_LEAVES);
						if (!air_flower(_blocks[((row + 1 - 1) * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level], true, false)) {
							int spec = blockGridX(block_type, 2 * (orientation == face_dir::MINUSX) + litFurnace) + (blockGridY(block_type) << 4) + (0 << 12);
							int faceLight = maxi(minLightLevel, 84 - computeLight(row + 1 - 1, col + 1, level));
							int shade = maxi(0, faceLight - computeShade(row + 1 - 1, col + 1, level, {0, 1, 0, 0, 1, 1, 0, 0, 1}));
							glm::ivec4 v0 = {spec + (shade << 16), p4};
							shade = maxi(0, faceLight - computeShade(row + 1 - 1, col + 1, level, {0, -1, 0, 0, -1, 1, 0, 0, 1}));
							glm::ivec4 v1 = {spec + (shade << 16) + 1 + (1 << 9) + (1 << 8), p0};
							shade = maxi(0, faceLight - computeShade(row + 1 - 1, col + 1, level, {0, 1, 0, 0, 1, -1, 0, 0, -1}));
							glm::ivec4 v2 = {spec + (shade << 16) + (1 << 4) + (1 << 10) + (1 << 12), p6};
							shade = maxi(0, faceLight - computeShade(row + 1 - 1, col + 1, level, {0, -1, 0, 0, -1, -1, 0, 0, -1}));
							glm::ivec4 v3 = {spec + (shade << 16) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p2};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (!air_flower(_blocks[((row + 1 + 1) * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level], isNotLeaves, false)) {
							int spec = blockGridX(block_type, 2 * (orientation == face_dir::PLUSX) + litFurnace) + (blockGridY(block_type) << 4) + (0 << 12);
							int faceLight = maxi(minLightLevel, 80 - computeLight(row + 1 + 1, col + 1, level));
							int shade = maxi(0, faceLight - computeShade(row + 1 + 1, col + 1, level, {0, -1, 0, 0, -1, 1, 0, 0, 1}));
							glm::ivec4 v0 = {spec + (shade << 16), p1};
							shade = maxi(0, faceLight - computeShade(row + 1 + 1, col + 1, level, {0, 1, 0, 0, 1, 1, 0, 0, 1}));
							glm::ivec4 v1 = {spec + (shade << 16) + 1 + (1 << 9) + (1 << 8), p5};
							shade = maxi(0, faceLight - computeShade(row + 1 + 1, col + 1, level, {0, -1, 0, 0, -1, -1, 0, 0, -1}));
							glm::ivec4 v2 = {spec + (shade << 16) + (1 << 4) + (1 << 10) + (1 << 12), p3};
							shade = maxi(0, faceLight - computeShade(row + 1 + 1, col + 1, level, {0, 1, 0, 0, 1, -1, 0, 0, -1}));
							glm::ivec4 v3 = {spec + (shade << 16) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p7};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (!air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col + 1 - 1) * WORLD_HEIGHT + level], true, false)) {
							int spec = blockGridX(block_type, 2 * (orientation == face_dir::MINUSY) + litFurnace) + (blockGridY(block_type) << 4) + (0 << 12);
							int faceLight = maxi(minLightLevel, 92 - computeLight(row + 1, col + 1 - 1, level));
							int shade = maxi(0, faceLight - computeShade(row + 1, col + 1 - 1, level, {-1, 0, 0, -1, 0, 1, 0, 0, 1}));
							glm::ivec4 v0 = {spec + (shade << 16), p0};
							shade = maxi(0, faceLight - computeShade(row + 1, col + 1 - 1, level, {1, 0, 0, 1, 0, 1, 0, 0, 1}));
							glm::ivec4 v1 = {spec + (shade << 16) + 1 + (1 << 9) + (1 << 8), p1};
							shade = maxi(0, faceLight - computeShade(row + 1, col + 1 - 1, level, {-1, 0, 0, -1, 0, -1, 0, 0, -1}));
							glm::ivec4 v2 = {spec + (shade << 16) + (1 << 4) + (1 << 10) + (1 << 12), p2};
							shade = maxi(0, faceLight - computeShade(row + 1, col + 1 - 1, level, {1, 0, 0, 1, 0, -1, 0, 0, -1}));
							glm::ivec4 v3 = {spec + (shade << 16) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p3};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (!air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col + 1 + 1) * WORLD_HEIGHT + level], isNotLeaves, false)) {
							int spec = blockGridX(block_type, 2 * (orientation == face_dir::PLUSY) + litFurnace) + (blockGridY(block_type) << 4) + (0 << 12);
							int faceLight = maxi(minLightLevel, 88 - computeLight(row + 1, col + 1 + 1, level));
							int shade = maxi(0, faceLight - computeShade(row + 1, col + 1 + 1, level, {1, 0, 0, 1, 0, 1, 0, 0, 1}));
							glm::ivec4 v0 = {spec + (shade << 16), p5};
							shade = maxi(0, faceLight - computeShade(row + 1, col + 1 + 1, level, {-1, 0, 0, -1, 0, 1, 0, 0, 1}));
							glm::ivec4 v1 = {spec + (shade << 16) + 1 + (1 << 9) + (1 << 8), p4};
							shade = maxi(0, faceLight - computeShade(row + 1, col + 1 + 1, level, {1, 0, 0, 1, 0, -1, 0, 0, -1}));
							glm::ivec4 v2 = {spec + (shade << 16) + (1 << 4) + (1 << 10) + (1 << 12), p7};
							shade = maxi(0, faceLight - computeShade(row + 1, col + 1 + 1, level, {-1, 0, 0, -1, 0, -1, 0, 0, -1}));
							glm::ivec4 v3 = {spec + (shade << 16) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p6};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (!air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level + 1], isNotLeaves, false)) {
							int spec = blockGridX(block_type, 1) + (blockGridY(block_type) << 4) + (0 << 12);
							int faceLight = maxi(minLightLevel, 100 - computeLight(row + 1, col + 1, level + 1));
							int shade = maxi(0, faceLight - computeShade(row + 1, col + 1, level + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0}));
							// if (shade & 0xFF)std::cout << "shade is " << shade << std::endl;
							// if (shade & 0xFFFFFF00)std::cout << "problem" << std::endl;
							glm::ivec4 v0 = {spec + (shade << 16), p4};
							shade = maxi(0, faceLight - computeShade(row + 1, col + 1, level + 1, {0, 1, 0, 1, 1, 0, 1, 0, 0}));
							glm::ivec4 v1 = {spec + (shade << 16) + 1 + (1 << 9) + (1 << 8), p5};
							shade = maxi(0, faceLight - computeShade(row + 1, col + 1, level + 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0}));
							glm::ivec4 v2 = {spec + (shade << 16) + (1 << 4) + (1 << 10) + (1 << 12), p0};
							shade = maxi(0, faceLight - computeShade(row + 1, col + 1, level + 1, {0, -1, 0, 1, -1, 0, 1, 0, 0}));
							glm::ivec4 v3 = {spec + (shade << 16) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p1};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (!air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level - 1], true, false)) {
							int spec = blockGridX(block_type, 1 + (block_type == blocks::GRASS_BLOCK)) + (blockGridY(block_type) << 4) + (0 << 12);
							int faceLight = maxi(minLightLevel, 74 - computeLight(row + 1, col + 1, level - 1));
							int shade = maxi(0, faceLight - computeShade(row + 1, col + 1, level - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0}));
							glm::ivec4 v0 = {spec + (shade << 16), p2};
							shade = maxi(0, faceLight - computeShade(row + 1, col + 1, level - 1, {0, -1, 0, 1, -1, 0, 1, 0, 0}));
							glm::ivec4 v1 = {spec + (shade << 16) + 1 + (1 << 9) + (1 << 8), p3};
							shade = maxi(0, faceLight - computeShade(row + 1, col + 1, level - 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0}));
							glm::ivec4 v2 = {spec + (shade << 16) + (1 << 4) + (1 << 10) + (1 << 12), p6};
							shade = maxi(0, faceLight - computeShade(row + 1, col + 1, level - 1, {0, 1, 0, 1, 1, 0, 1, 0, 0}));
							glm::ivec4 v3 = {spec + (shade << 16) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p7};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
					} else if (block_type == blocks::TORCH) {
						// TODO for now torches only in default middle-of-block configuration
						int baseSpec = blockGridX(block_type, 0) + (blockGridY(block_type) << 4);
						int spec = baseSpec + (84 << 16) + (7 << 24);
						glm::ivec4 v0 = {spec, p4};
						glm::ivec4 v1 = {spec + 1 + (1 << 8), p0};
						glm::ivec4 v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p6};
						glm::ivec4 v3 = {spec + 1 + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p2};
						face_vertices(_vertices, v0, v1, v2, v3, index); // -x
						spec = baseSpec + (80 << 16) + (9 << 24);
						v0 = {spec, p0};
						v1 = {spec + 1 + (1 << 8), p4};
						v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p2};
						v3 = {spec + 1 + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p6};
						face_vertices(_vertices, v0, v1, v2, v3, index); // +x
						spec = baseSpec + (92 << 16) + (7 << 28);
						v0 = {spec, p0};
						v1 = {spec + 1 + (1 << 8), p1};
						v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p2};
						v3 = {spec + 1 + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p3};
						face_vertices(_vertices, v0, v1, v2, v3, index); // -y
						spec = baseSpec + (88 << 16) + (9 << 28);
						v0 = {spec, p1};
						v1 = {spec + 1 + (1 << 8), p0};
						v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p3};
						v3 = {spec + 1 + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p2};
						face_vertices(_vertices, v0, v1, v2, v3, index); // +y
					} else { // flowers
						int spec = blockGridX(block_type, 0) + (blockGridY(block_type) << 4) + (0 << 12);
						spec += (maxi(minLightLevel, 100 - computeLight(row + 1, col + 1, level)) << 16);
						glm::ivec4 v0 = {spec, p0};
						glm::ivec4 v1 = {spec + 1 + (1 << 9) + (1 << 8), p5};
						glm::ivec4 v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p2};
						glm::ivec4 v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p7};
						face_vertices(_vertices, v0, v1, v2, v3, index);
						v0 = {spec, p1};
						v1 = {spec + 1 + (1 << 9) + (1 << 8), p4};
						v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p3};
						v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p6};
						face_vertices(_vertices, v0, v1, v2, v3, index);
					}
				}
			}
		}
	}
	// if (index != _displayed_faces * 24) { // TODO FIND OUT WHY LEAVES BREAK THIS CONDITION
	// 	std::cout << "index at end is " << index << " vs " << _displayed_faces << " | " << _displayed_faces * 4 * 6 << std::endl;
	// }
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

GLint Chunk::getSkyLightLevel( glm::ivec3 location )
{
	int posX = location.x - _startX, posY = location.y - _startY;
	return (_sky_light[((posX + 1) * (CHUNK_SIZE + 2) + posY + 1) * WORLD_HEIGHT + location.z] & 0xF);
}

GLint Chunk::getBlockLightLevel( glm::ivec3 location )
{
	int posX = location.x - _startX, posY = location.y - _startY;
	return (_block_light[((posX + 1) * (CHUNK_SIZE + 2) + posY + 1) * WORLD_HEIGHT + location.z] & 0xF);
}
