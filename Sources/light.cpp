#include "vox.h"

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void Chunk::handle_border_light_spread( int posX, int posY, int posZ, char lightLevel )
{
	// std::cout << "handle border light spread " << posX << " " << posY << " " << posZ << " | "  << _startX << " " << _startY << std::endl;
	if (!posX && _neighbours[face_dir::MINUSX]) {
		_neighbours[face_dir::MINUSX]->update_border_light_spread(CHUNK_SIZE, posY, posZ, lightLevel, face_dir::PLUSX);
	} else if (posX == CHUNK_SIZE + 1 && _neighbours[face_dir::PLUSX]) {
		_neighbours[face_dir::PLUSX]->update_border_light_spread(1, posY, posZ, lightLevel, face_dir::MINUSX);
	}
	if (!posY && _neighbours[face_dir::MINUSY]) {
		_neighbours[face_dir::MINUSY]->update_border_light_spread(posX, CHUNK_SIZE, posZ, lightLevel, face_dir::PLUSY);
	} else if (posY == CHUNK_SIZE + 1 && _neighbours[face_dir::PLUSY]) {
		_neighbours[face_dir::PLUSY]->update_border_light_spread(posX, 1, posZ, lightLevel, face_dir::MINUSY);
	}
}

void Chunk::handle_border_light( glm::ivec3 pos, char lightLevel )
{
	// std::cout << "handle border light " << pos.x << " " << pos.y << " " << pos.z << " | "  << _startX << " " << _startY << std::endl;
	if (!pos.x && _neighbours[face_dir::MINUSX]) {
		_neighbours[face_dir::MINUSX]->update_border_light(CHUNK_SIZE + 1, pos.y + 1, pos.z, lightLevel);
	} else if (pos.x == CHUNK_SIZE - 1 && _neighbours[face_dir::PLUSX]) {
		_neighbours[face_dir::PLUSX]->update_border_light(0, pos.y + 1, pos.z, lightLevel);
	}
	if (!pos.y && _neighbours[face_dir::MINUSY]) {
		_neighbours[face_dir::MINUSY]->update_border_light(pos.x + 1, CHUNK_SIZE + 1, pos.z, lightLevel);
	} else if (pos.y == CHUNK_SIZE - 1 && _neighbours[face_dir::PLUSY]) {
		_neighbours[face_dir::PLUSY]->update_border_light(pos.x + 1, 0, pos.z, lightLevel);
	}
}

void Chunk::light_spread( GLchar *arr, int posX, int posY, int posZ )
{
	char level = arr[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ];
	// std::cout << "light_spread, level is " << (int)level << std::endl;
	// TODO once diff light sources exist with diff light_level, rework this fonction
	if (!(level & 0xF0)) { // not a source block, we check if level should change
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
			_light_update = true;
			if (posX == 1 || posX == CHUNK_SIZE || posY == 1 || posY == CHUNK_SIZE) {
				// neighbours update
	// std::cout << "handle border light " << posX << " " << posY << " " << posZ << " | "  << _startX << " " << _startY << std::endl;
	// 			handle_border_light({posX - 1, posY - 1, posZ}, level);
			}
			if (posX == 0 || posX == CHUNK_SIZE + 1 || posY == 0 || posY == CHUNK_SIZE + 1) {
				// neighbours spread
	// std::cout << "handle border light spread " << posX << " " << posY << " " << posZ << " | "  << _startX << " " << _startY << std::endl;
	// 			handle_border_light_spread(posX, posY, posZ, level);
				return ;
			}
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
			char neighbour = arr[((posX + delta[0]) * (CHUNK_SIZE + 2) + posY + delta[1]) * WORLD_HEIGHT + posZ + delta[2]];
			if (!(neighbour & 0xF0) && ((neighbour & 0xF) != maxc(0, (level & 0xF) - 1))) { // || (level & 0xF0) only spread to non source blocks whose value is not expected value. source block always spread
				light_spread(arr, posX + delta[0], posY + delta[1], posZ + delta[2]);
			}
		}
	}
}

void Chunk::generate_sky_light( void )
{
	// fill sky_light with 0 for underground and 15 (- leaves and water damper) for rest
	// voxel is considered underground if there is at least 1 solid block above it
	for (int row = 0; row < CHUNK_SIZE + 2; row++) {
		for (int col = 0; col < CHUNK_SIZE + 2; col++) {
			char light_level = 15;
			for (int level = WORLD_HEIGHT - 1; level > 0; level--) {
				_sky_light[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = light_level + (light_level << 4); // we consider blocks directly under sky as light source
				if (light_level) {
					int value = _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level];
					if (air_flower(value, true, false)) { // block hit
						light_level = 0;
					} else if (value == blocks::OAK_LEAVES || value >= blocks::WATER) {
						--light_level;
					}
				}
			}
		}
	}

	// second loop, this time spread light underground
	for (int row = 0; row < CHUNK_SIZE + 2; row++) {
		for (int col = 0; col < CHUNK_SIZE + 2; col++) {
			for (int level = WORLD_HEIGHT - 1; level > 0; level--) {
				if (!_sky_light[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level]) {
					int value = _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level];
					if (!air_flower(value, true, false)) { // underground hole
						// spread_light watch out for leaves and water light damping..
						// std::cout << "light_spread" << std::endl;
						light_spread(_sky_light, row, col, level);
						// std::cout << "over" << std::endl;
					}
				}
			}
		}
	}
	_light_update = false;
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
					_block_light[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = light_level + (light_level << 4); // 0xF0 = light source. & 0xF = light level
					light_spread(_block_light, row, col, level);
				}
			}
		}
	}
	_light_update = false;
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

void Chunk::update_border_light( int posX, int posY, int posZ, char lightLevel )
{
	// std::cout << "got into update border light of chunk " << _startX << ", " << _startY << std::endl;
	// std::cout << "args: " << posX << ", " << posY << ", " << level << ": " << ((adding) ? "add" : "rm") << " " << s_blocks[type].name << " | real pos is " << _startX + posX - 1 << ", " << _startY + posY - 1 << std::endl;
	if (!(!posX || !posY || posX == CHUNK_SIZE + 1 || posY == CHUNK_SIZE + 1)) {
		std::cout << "ERROR update_border_light not border block " << posX << ", " << posY << std::endl;
		return ;
	}
	if (_thread.joinable()) {
		_thread.join();
	}
	if (!posX) {
		if (posY == 1 && _neighbours[face_dir::MINUSY]) {
			// std::cout << "corner " << _startX << ", " << _startY - CHUNK_SIZE << std::endl;
			_neighbours[face_dir::MINUSY]->update_border_light(posX, CHUNK_SIZE + 1, posZ, lightLevel);
		} else if (posY == CHUNK_SIZE && _neighbours[face_dir::PLUSY]) {
			// std::cout << "corner " << _startX << ", " << _startY + CHUNK_SIZE << std::endl;
			_neighbours[face_dir::PLUSY]->update_border_light(posX, 0, posZ, lightLevel);
		}
	} else if (posX == CHUNK_SIZE + 1) {
		if (posY == 1 && _neighbours[face_dir::MINUSY]) {
			// std::cout << "corner " << _startX << ", " << _startY - CHUNK_SIZE << std::endl;
			_neighbours[face_dir::MINUSY]->update_border_light(posX, CHUNK_SIZE + 1, posZ, lightLevel);
		} else if (posY == CHUNK_SIZE && _neighbours[face_dir::PLUSY]) {
			// std::cout << "corner " << _startX << ", " << _startY + CHUNK_SIZE << std::endl;
			_neighbours[face_dir::PLUSY]->update_border_light(posX, 0, posZ, lightLevel);
		}
	}
	if (_block_light[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ] < lightLevel) {
		_block_light[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ] = lightLevel;
	}
}

void Chunk::update_border_light_spread( int posX, int posY, int posZ, char lightLevel, face_dir origin )
{
	if (!(posX == 1 || posY == 1 || posX == CHUNK_SIZE || posY == CHUNK_SIZE)) {
		std::cout << "ERROR update_border_light spread not border block " << posX << ", " << posY << std::endl;
		return ;
	}
	if (_thread.joinable()) {
		_thread.join();
	}
	std::cout << "update border light spread " << _startX << ", " << _startY << " | rpos " << _startX + posX - 1 << ", " << _startY + posY - 1 << std::endl;
	if (posX == 1) {
		if (posY == 1) {
			if (origin == face_dir::MINUSX && _neighbours[face_dir::MINUSY]) {
				// std::cout << "light spread corner " << s_blocks[wlevel].name << " " << _startX << ", " << _startY - CHUNK_SIZE << std::endl;
				_neighbours[face_dir::MINUSY]->update_border_light(posX, CHUNK_SIZE + 1, posZ, lightLevel);
			} else if (origin == face_dir::MINUSY && _neighbours[face_dir::MINUSX]) {
				// std::cout << "light spread corner " << s_blocks[wlevel].name << " " << _startX - CHUNK_SIZE << ", " << _startY << std::endl;
				_neighbours[face_dir::MINUSX]->update_border_light(CHUNK_SIZE + 1, posY, posZ, lightLevel);
			}
		} else if (posY == CHUNK_SIZE) {
			if (origin == face_dir::MINUSX && _neighbours[face_dir::PLUSY]) {
				// std::cout << "light spread corner " << s_blocks[wlevel].name << " " << _startX << ", " << _startY + CHUNK_SIZE << std::endl;
				_neighbours[face_dir::PLUSY]->update_border_light(posX, 0, posZ, lightLevel);
			} else if (origin == face_dir::PLUSY && _neighbours[face_dir::MINUSX]) {
				// std::cout << "light spread corner " << s_blocks[wlevel].name << " " << _startX - CHUNK_SIZE << ", " << _startY << std::endl;
				_neighbours[face_dir::MINUSX]->update_border_light(CHUNK_SIZE + 1, posY, posZ, lightLevel);
			}
		}
	} else if (posX == CHUNK_SIZE) {
		if (posY == 1) {
			if (origin == face_dir::PLUSX && _neighbours[face_dir::MINUSY]) {
				// std::cout << "light spread corner " << s_blocks[wlevel].name << " " << _startX << ", " << _startY - CHUNK_SIZE << std::endl;
				_neighbours[face_dir::MINUSY]->update_border_light(posX, CHUNK_SIZE + 1, posZ, lightLevel);
			} else if (origin == face_dir::MINUSY && _neighbours[face_dir::PLUSX]) {
				// std::cout << "light spread corner " << s_blocks[wlevel].name << " " << _startX + CHUNK_SIZE << ", " << _startY << std::endl;
				_neighbours[face_dir::PLUSX]->update_border_light(0, posY, posZ, lightLevel);
			}
		} else if (posY == CHUNK_SIZE) {
			if (origin == face_dir::PLUSX && _neighbours[face_dir::PLUSY]) {
				// std::cout << "light spread corner " << s_blocks[wlevel].name << " " << _startX << ", " << _startY + CHUNK_SIZE << std::endl;
				_neighbours[face_dir::PLUSY]->update_border_light(posX, 0, posZ, lightLevel);
			} else if (origin == face_dir::PLUSY && _neighbours[face_dir::PLUSX]) {
				// std::cout << "light spread corner " << s_blocks[wlevel].name << " " << _startX + CHUNK_SIZE << ", " << _startY << std::endl;
				_neighbours[face_dir::PLUSX]->update_border_light(0, posY, posZ, lightLevel);
			}
		}
	}

			// std::cout << '[' << _startX << ", " << _startY << "] light spread at border " << posX << ", " << posY << ", " << posZ << " >>" << value << " into " << wlevel << std::endl;
	_block_light[((posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT) + posZ] = lightLevel;
	light_spread(_block_light, posX, posY, posZ);
}
