#include "vox.h"

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// if skySpread, _lights[offset] is bitshifted by 4 to work on sky light and not block light
// posX, posY are in [0; CHUNK_SIZE[
void Chunk::light_spread( int posX, int posY, int posZ, bool skySpread )
{
	// if (skySpread && _startX == 256 && _startY == 320) {
	// 	static int cnt;
	// 	std::cout << _startX << ", " << _startY << " at " << posX << ", " << posY << ", " << posZ << " light_spread " << ((skySpread) ? "SKY" : "BLOCK") << " recurse level " << ++cnt << std::endl;
	// }
	int shift = 8 * skySpread;
	int offset = (((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + posZ;
	short saveLight = _lights[offset];
	short level = ((saveLight >> shift) & 0xFF);
	// std::cout << "light_spread, level is " << (int)level << std::endl;
	// TODO once diff light sources exist with diff light_level, rework this condition
	if (!(level & 0xF0)) { // not a source block, we check if level should change
		short maxLevel = 0;
		for (int index = 0; index < 6; index++) {
			const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
			if (posZ + delta[2] < 0 || posZ + delta[2] > 255) {
			} else if (posX + delta[0] < 0 || posX + delta[0] >= CHUNK_SIZE || posY + delta[1] < 0 || posY + delta[1] >= CHUNK_SIZE) {
				// ask neighbour
				short light = 0;
				if (posX + delta[0] == -1) {
					if (_neighbours[face_dir::MINUSX]) {
						light = _neighbours[face_dir::MINUSX]->getLightLevel(CHUNK_SIZE - 1, posY + delta[1], posZ + delta[2]);
					}
				} else if (posY + delta[1] == -1) {
					if (_neighbours[face_dir::MINUSY]) {
						light = _neighbours[face_dir::MINUSY]->getLightLevel(posX + delta[0], CHUNK_SIZE - 1, posZ + delta[2]);
					}
				} else if (posX + delta[0] == CHUNK_SIZE) {
					if (_neighbours[face_dir::PLUSX]) {
						light = _neighbours[face_dir::PLUSX]->getLightLevel(0, posY + delta[1], posZ + delta[2]);
					}
				} else if (posY + delta[1] == CHUNK_SIZE) {
					if (_neighbours[face_dir::PLUSY]) {
						light = _neighbours[face_dir::PLUSY]->getLightLevel(posX + delta[0], 0, posZ + delta[2]);
					}
				}
				maxLevel = maxs(maxLevel, (light >> shift) & 0xF);
			} else {
				short adj = _lights[((((posX + delta[0]) << CHUNK_SHIFT) + posY + delta[1]) << WORLD_SHIFT) + posZ + delta[2]] >> shift;
				maxLevel = maxs(maxLevel, adj & 0xF);
				if (skySpread && index == face_dir::PLUSZ && (adj & 0xF0)) {
					maxLevel = (adj & 0xFF) + 1; // if sky light above is source, own sky light becomes source too
				}
			}
		}
		if ((maxLevel && maxLevel - 1 != level) || (!maxLevel && level)) {
			level = maxs(0, maxLevel - 1);
			// if (level < ((saveLight >> shift) & 0xFF)) { // if light gets dimmer, we accelerate process and put it to 0, it will spread to a nearby light source which will spread again
			// 	level = 0;
			// }
			_lights[offset] = (saveLight & (0xFF << (8 - shift))) + (level << shift);
			_light_update = true;
		} else {
			return ; // stop spread when not source and level is unchanged
		}
	} else if (skySpread && posZ + 1 < 254) { // skySpread + source block
		short above = _lights[offset + 1] >> shift;
		if (!(above & 0xF0)) { // if block above is not a source anymore
			_lights[offset] &= (0xFF << (8 - shift));
			return (light_spread(posX, posY, posZ, skySpread));
		}
	}
	// std::cout << "level is " << (int)level << std::endl;

	// spread in all 6 directions
	for (int index = 0; index < 6; index++) {
		const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
		light_try_spread(posX + delta[0], posY + delta[1], posZ + delta[2], level, skySpread);
	}
}

void Chunk::generate_lights( void )
{
	// fill sky_light with 0 for underground and 15 (- leaves and water damper) for rest
	// voxel is considered underground if there is at least 1 solid block above it
	// this also fills block_light with 0
	for (int row = 0; row < CHUNK_SIZE; row++) {
		for (int col = 0; col < CHUNK_SIZE; col++) {
			char light_level = 15;
			for (int level = WORLD_HEIGHT - 1; level > 0; level--) {
				if (light_level) {
					int value = _blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level];
					if (air_flower(value, true, true, false) && value != blocks::OAK_SLAB) { // block hit
						light_level = 0;
					}
					_lights[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = (light_level + (light_level << 4)) << 8; // we consider blocks directly under sky as light source
					if (value == blocks::OAK_LEAVES || value >= blocks::WATER) {
						--light_level;
					}
				} else {
					_lights[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = 0;
				}
			}
		}
	}
}

// uses sky light and block light to output a shade value 
// -> obsolete, now compute on shader
// now returns 0xFF00 skylight + 0xFF blocklight
// row and col are in [-1:CHUNK_SIZE]
int Chunk::computeLight( int row, int col, int level )
{
	// (void)row;(void)col;(void)level;return (0);
	short light = 0xF00;
	if (level == 256) {
	} else if (row == -1) {
		if (_neighbours[face_dir::MINUSX]) {
			light = _neighbours[face_dir::MINUSX]->getLightLevel(CHUNK_SIZE - 1, col, level);
		}
	} else if (col == -1) {
		if (_neighbours[face_dir::MINUSY]) {
			light = _neighbours[face_dir::MINUSY]->getLightLevel(row, CHUNK_SIZE - 1, level);
		}
	} else if (row == CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSX]) {
			light = _neighbours[face_dir::PLUSX]->getLightLevel(0, col, level);
		}
	} else if (col == CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSY]) {
			light = _neighbours[face_dir::PLUSY]->getLightLevel(row, 0, level);
		}
	} else {
		light = _lights[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level];
	}
	// int blockLightAmplitude = 5; // amount by which light decreases on each block
	int blockLight = light & 0xF;
	int skyLight = (light >> 8) & 0xF;
	return (blockLight + (skyLight << 4));
}

// https://minecraft.fandom.com/wiki/Light#AO_texture_pattern fct returns (side1 + side2 + corner)
int Chunk::computeShade( int row, int col, int level, std::array<int, 9> offsets )
{
	// (void)row;(void)col;(void)level;(void)offsets;return (0);
	return (!!air_flower(getBlockAt(row + offsets[0], col + offsets[1], level + offsets[2], true), true, true, false)
			+ !!air_flower(getBlockAt(row + offsets[3], col + offsets[4], level + offsets[5], true), true, true, false)
			+ !!air_flower(getBlockAt(row + offsets[6], col + offsets[7], level + offsets[8], true), true, true, false));
}

void Chunk::fill_vertex_array( void )
{
	// std::cout << "filling " << _startX << ", " << _startY << "; expecting " << _displayed_faces << std::endl;
	_mtx.lock();
	size_t index = 0;
	for (int row = 0; row < CHUNK_SIZE; row++) {
		for (int col = 0; col < CHUNK_SIZE; col++) {
			for (int level = 0; level < WORLD_HEIGHT; level++) {
				GLint block_type = _blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level];
				if (block_type > blocks::AIR && block_type < blocks::WATER && block_type != blocks::GLASS) {
					float zSize = 1.0f - 0.5f * (block_type == blocks::OAK_SLAB);
					glm::vec3 p0 = {_startX + row + 0, _startY + col + 0, level + zSize};
					glm::vec3 p1 = {_startX + row + 1, _startY + col + 0, level + zSize};
					glm::vec3 p2 = {_startX + row + 0, _startY + col + 0, level + 0};
					glm::vec3 p3 = {_startX + row + 1, _startY + col + 0, level + 0};

					glm::vec3 p4 = {_startX + row + 0, _startY + col + 1, level + zSize};
					glm::vec3 p5 = {_startX + row + 1, _startY + col + 1, level + zSize};
					glm::vec3 p6 = {_startX + row + 0, _startY + col + 1, level + 0};
					glm::vec3 p7 = {_startX + row + 1, _startY + col + 1, level + 0};

					if (block_type == blocks::OAK_SLAB) {
						int tex_block = blocks::OAK_PLANKS;
						if (visible_face(block_type, getBlockAt(row - 1, col, level, true), face_dir::MINUSX)) {
							int spec = blockGridX(tex_block, 0) + (blockGridY(tex_block) << 4) + (3 << 19);
							int faceLight = computeLight(row - 1, col, level);
							int shade = 0;//computeShade(row - 1, col, level, {0, 1, 0, 0, 1, 1, 0, 0, 1});
							spec += (faceLight << 24);
							std::pair<int, glm::vec3> v0 = {spec + (shade << 22), p4};
							std::pair<int, glm::vec3> v1 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 8), p0};
							std::pair<int, glm::vec3> v2 = {spec + (shade << 22) + (1 << 4) + (1 << 10) + (1 << 12), p6};
							std::pair<int, glm::vec3> v3 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p2};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (visible_face(block_type, getBlockAt(row + 1, col, level, true), face_dir::PLUSX)) {
							int spec = blockGridX(tex_block, 0) + (blockGridY(tex_block) << 4) + (4 << 19);
							int faceLight = computeLight(row + 1, col, level);
							int shade = 0;//computeShade(row + 1, col, level, {0, -1, 0, 0, -1, 1, 0, 0, 1});
							spec += (faceLight << 24);
							std::pair<int, glm::vec3> v0 = {spec + (shade << 22), p1};
							std::pair<int, glm::vec3> v1 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 8), p5};
							std::pair<int, glm::vec3> v2 = {spec + (shade << 22) + (1 << 4) + (1 << 10) + (1 << 12), p3};
							std::pair<int, glm::vec3> v3 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p7};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (visible_face(block_type, getBlockAt(row, col - 1, level, true), face_dir::MINUSY)) {
							int spec = blockGridX(tex_block, 0) + (blockGridY(tex_block) << 4) + (1 << 19);
							int faceLight = computeLight(row, col - 1, level);
							int shade = 0;//computeShade(row, col - 1, level, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
							spec += (faceLight << 24);
							std::pair<int, glm::vec3> v0 = {spec + (shade << 22), p0};
							std::pair<int, glm::vec3> v1 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 8), p1};
							std::pair<int, glm::vec3> v2 = {spec + (shade << 22) + (1 << 4) + (1 << 10) + (1 << 12), p2};
							std::pair<int, glm::vec3> v3 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p3};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (visible_face(block_type, getBlockAt(row, col + 1, level, true), face_dir::PLUSY)) {
							int spec = blockGridX(tex_block, 0) + (blockGridY(tex_block) << 4) + (2 << 19);
							int faceLight = computeLight(row, col + 1, level);
							int shade = 0;//computeShade(row, col + 1, level, {1, 0, 0, 1, 0, 1, 0, 0, 1});
							spec += (faceLight << 24);
							std::pair<int, glm::vec3> v0 = {spec + (shade << 22), p5};
							std::pair<int, glm::vec3> v1 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 8), p4};
							std::pair<int, glm::vec3> v2 = {spec + (shade << 22) + (1 << 4) + (1 << 10) + (1 << 12), p7};
							std::pair<int, glm::vec3> v3 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p6};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (visible_face(block_type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ)) {
							int spec = blockGridX(tex_block, 1) + (blockGridY(tex_block) << 4) + (0 << 19);
							int faceLight = computeLight(row, col, level + 1);
							int shade = 0;//computeShade(row, col, level + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
							spec += (faceLight << 24);
							// if (shade & 0xFF)std::cout << "shade is " << shade << std::endl;
							// if (shade & 0xFFFFFF00)std::cout << "problem" << std::endl;
							std::pair<int, glm::vec3> v0 = {spec + (shade << 22), p4};
							std::pair<int, glm::vec3> v1 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 8), p5};
							std::pair<int, glm::vec3> v2 = {spec + (shade << 22) + (1 << 4) + (1 << 10) + (1 << 12), p0};
							std::pair<int, glm::vec3> v3 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p1};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (visible_face(block_type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ)) {
							int spec = blockGridX(tex_block, 1) + (blockGridY(tex_block) << 4) + (5 << 19);
							int faceLight = computeLight(row, col, level - 1);
							int shade = 0;//computeShade(row, col, level - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
							spec += (faceLight << 24);
							std::pair<int, glm::vec3> v0 = {spec + (shade << 22), p2};
							std::pair<int, glm::vec3> v1 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 8), p3};
							std::pair<int, glm::vec3> v2 = {spec + (shade << 22) + (1 << 4) + (1 << 10) + (1 << 12), p6};
							std::pair<int, glm::vec3> v3 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p7};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
					} else if (block_type < blocks::POPPY) {
						int orientation = -1, litFurnace = 0;
						if (block_type >= blocks::CRAFTING_TABLE && block_type < blocks::BEDROCK) {
							auto o = _orientations.find((((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level);
							if (o != _orientations.end()) {
								orientation = (o->second & 0xF);
								litFurnace = ((o->second >> 4) & 0xF) == furnace_state::ON;
							}
						}
						if (visible_face(block_type, getBlockAt(row - 1, col, level, true), face_dir::MINUSX)) {
							int spec = blockGridX(block_type, 2 * (orientation == face_dir::MINUSX) + litFurnace) + (blockGridY(block_type) << 4) + (3 << 19);
							int faceLight = computeLight(row - 1, col, level);
							int shade = computeShade(row - 1, col, level, {0, 1, 0, 0, 1, 1, 0, 0, 1});
							spec += (faceLight << 24);
							std::pair<int, glm::vec3> v0 = {spec + (shade << 22), p4};
							shade = computeShade(row - 1, col, level, {0, -1, 0, 0, -1, 1, 0, 0, 1});
							std::pair<int, glm::vec3> v1 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 8), p0};
							shade = computeShade(row - 1, col, level, {0, 1, 0, 0, 1, -1, 0, 0, -1});
							std::pair<int, glm::vec3> v2 = {spec + (shade << 22) + (1 << 4) + (1 << 10) + (1 << 12), p6};
							shade = computeShade(row - 1, col, level, {0, -1, 0, 0, -1, -1, 0, 0, -1});
							std::pair<int, glm::vec3> v3 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p2};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (visible_face(block_type, getBlockAt(row + 1, col, level, true), face_dir::PLUSX)) {
							int spec = blockGridX(block_type, 2 * (orientation == face_dir::PLUSX) + litFurnace) + (blockGridY(block_type) << 4) + (4 << 19);
							int faceLight = computeLight(row + 1, col, level);
							int shade = computeShade(row + 1, col, level, {0, -1, 0, 0, -1, 1, 0, 0, 1});
							spec += (faceLight << 24);
							std::pair<int, glm::vec3> v0 = {spec + (shade << 22), p1};
							shade = computeShade(row + 1, col, level, {0, 1, 0, 0, 1, 1, 0, 0, 1});
							std::pair<int, glm::vec3> v1 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 8), p5};
							shade = computeShade(row + 1, col, level, {0, -1, 0, 0, -1, -1, 0, 0, -1});
							std::pair<int, glm::vec3> v2 = {spec + (shade << 22) + (1 << 4) + (1 << 10) + (1 << 12), p3};
							shade = computeShade(row + 1, col, level, {0, 1, 0, 0, 1, -1, 0, 0, -1});
							std::pair<int, glm::vec3> v3 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p7};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (visible_face(block_type, getBlockAt(row, col - 1, level, true), face_dir::MINUSY)) {
							int spec = blockGridX(block_type, 2 * (orientation == face_dir::MINUSY) + litFurnace) + (blockGridY(block_type) << 4) + (1 << 19);
							int faceLight = computeLight(row, col - 1, level);
							int shade = computeShade(row, col - 1, level, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
							spec += (faceLight << 24);
							std::pair<int, glm::vec3> v0 = {spec + (shade << 22), p0};
							shade = computeShade(row, col - 1, level, {1, 0, 0, 1, 0, 1, 0, 0, 1});
							std::pair<int, glm::vec3> v1 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 8), p1};
							shade = computeShade(row, col - 1, level, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
							std::pair<int, glm::vec3> v2 = {spec + (shade << 22) + (1 << 4) + (1 << 10) + (1 << 12), p2};
							shade = computeShade(row, col - 1, level, {1, 0, 0, 1, 0, -1, 0, 0, -1});
							std::pair<int, glm::vec3> v3 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p3};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (visible_face(block_type, getBlockAt(row, col + 1, level, true), face_dir::PLUSY)) {
							int spec = blockGridX(block_type, 2 * (orientation == face_dir::PLUSY) + litFurnace) + (blockGridY(block_type) << 4) + (2 << 19);
							int faceLight = computeLight(row, col + 1, level);
							int shade = computeShade(row, col + 1, level, {1, 0, 0, 1, 0, 1, 0, 0, 1});
							spec += (faceLight << 24);
							std::pair<int, glm::vec3> v0 = {spec + (shade << 22), p5};
							shade = computeShade(row, col + 1, level, {-1, 0, 0, -1, 0, 1, 0, 0, 1});
							std::pair<int, glm::vec3> v1 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 8), p4};
							shade = computeShade(row, col + 1, level, {1, 0, 0, 1, 0, -1, 0, 0, -1});
							std::pair<int, glm::vec3> v2 = {spec + (shade << 22) + (1 << 4) + (1 << 10) + (1 << 12), p7};
							shade = computeShade(row, col + 1, level, {-1, 0, 0, -1, 0, -1, 0, 0, -1});
							std::pair<int, glm::vec3> v3 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p6};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (visible_face(block_type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ)) {
							int spec = blockGridX(block_type, 1) + (blockGridY(block_type) << 4) + (0 << 19);
							int faceLight = computeLight(row, col, level + 1);
							int shade = computeShade(row, col, level + 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
							spec += (faceLight << 24);
							// if (shade & 0xFF)std::cout << "shade is " << shade << std::endl;
							// if (shade & 0xFFFFFF00)std::cout << "problem" << std::endl;
							std::pair<int, glm::vec3> v0 = {spec + (shade << 22), p4};
							shade = computeShade(row, col, level + 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
							std::pair<int, glm::vec3> v1 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 8), p5};
							shade = computeShade(row, col, level + 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
							std::pair<int, glm::vec3> v2 = {spec + (shade << 22) + (1 << 4) + (1 << 10) + (1 << 12), p0};
							shade = computeShade(row, col, level + 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
							std::pair<int, glm::vec3> v3 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p1};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (visible_face(block_type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ)) {
							int spec = blockGridX(block_type, 1 + (block_type == blocks::GRASS_BLOCK)) + (blockGridY(block_type) << 4) + (5 << 19);
							int faceLight = computeLight(row, col, level - 1);
							int shade = computeShade(row, col, level - 1, {-1, 0, 0, -1, -1, 0, 0, -1, 0});
							spec += (faceLight << 24);
							std::pair<int, glm::vec3> v0 = {spec + (shade << 22), p2};
							shade = computeShade(row, col, level - 1, {0, -1, 0, 1, -1, 0, 1, 0, 0});
							std::pair<int, glm::vec3> v1 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 8), p3};
							shade = computeShade(row, col, level - 1, {-1, 0, 0, -1, 1, 0, 0, 1, 0});
							std::pair<int, glm::vec3> v2 = {spec + (shade << 22) + (1 << 4) + (1 << 10) + (1 << 12), p6};
							shade = computeShade(row, col, level - 1, {0, 1, 0, 1, 1, 0, 1, 0, 0});
							std::pair<int, glm::vec3> v3 = {spec + (shade << 22) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p7};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
					} else if (block_type == blocks::TORCH) {
						// TODO for now torches only in default middle-of-block configuration
						int spec = blockGridX(block_type, 0) + (blockGridY(block_type) << 4) + (0 << 19) + (15 << 24);
						std::pair<int, glm::vec3> v0 = {spec, {p4.x + 7.0 / 16, p4.y, p4.z}};
						std::pair<int, glm::vec3> v1 = {spec + 1 + (1 << 8), {p0.x + 7.0 / 16, p0.y, p0.z}};
						std::pair<int, glm::vec3> v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), {p6.x + 7.0 / 16, p6.y, p6.z}};
						std::pair<int, glm::vec3> v3 = {spec + 1 + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), {p2.x + 7.0 / 16, p2.y, p2.z}};
						face_vertices(_vertices, v0, v1, v2, v3, index); // -x
						v0 = {spec, {p0.x + 9.0 / 16, p0.y, p0.z}};
						v1 = {spec + 1 + (1 << 8), {p4.x + 9.0 / 16, p4.y, p4.z}};
						v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), {p2.x + 9.0 / 16, p2.y, p2.z}};
						v3 = {spec + 1 + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), {p6.x + 9.0 / 16, p6.y, p6.z}};
						face_vertices(_vertices, v0, v1, v2, v3, index); // +x
						v0 = {spec, {p0.x, p0.y + 7.0 / 16, p0.z}};
						v1 = {spec + 1 + (1 << 8), {p1.x, p1.y + 7.0 / 16, p1.z}};
						v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), {p2.x, p2.y + 7.0 / 16, p2.z}};
						v3 = {spec + 1 + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), {p3.x, p3.y + 7.0 / 16, p3.z}};
						face_vertices(_vertices, v0, v1, v2, v3, index); // -y
						v0 = {spec, {p1.x, p1.y + 9.0 / 16, p1.z}};
						v1 = {spec + 1 + (1 << 8), {p0.x, p0.y + 9.0 / 16, p0.z}};
						v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), {p3.x, p3.y + 9.0 / 16, p3.z}};
						v3 = {spec + 1 + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), {p2.x, p2.y + 9.0 / 16, p2.z}};
						face_vertices(_vertices, v0, v1, v2, v3, index); // +y
					} else { // flowers
						int spec = blockGridX(block_type, 0) + (blockGridY(block_type) << 4) + (0 << 19);
						int faceLight = computeLight(row, col, level);
						spec += (faceLight << 24);
						std::pair<int, glm::vec3> v0 = {spec, p0};
						std::pair<int, glm::vec3> v1 = {spec + 1 + (1 << 9) + (1 << 8), p5};
						std::pair<int, glm::vec3> v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p2};
						std::pair<int, glm::vec3> v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p7};
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
	_mtx.unlock();
	// if (index != _displayed_faces * 24) { // TODO if segfault on fill_vertex_array, use this condition to try and debug it
	// 	std::cout << "ERROR fill_vertex_array " << _startX << ", " << _startY << "\n\tindex at end is " << (index >> 2) / 6 << " | " << index << " vs " << _displayed_faces << " | " << _displayed_faces * 4 * 6 << std::endl;
	// }
	_light_update = false;
	_vaoReset = false;
	_vaoVIP = true;
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

GLint Chunk::getSkyLightLevel( glm::ivec3 location )
{
	if (location.z > 255 || location.z < 0) {
		return (0xF * (location.z > 255));
	}
	int posX = location.x - _startX, posY = location.y - _startY;
	return ((_lights[(((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + location.z] >> 8) & 0xF);
}

GLint Chunk::getBlockLightLevel( glm::ivec3 location )
{
	if (location.z > 255 || location.z < 0) {
		return (0);
	}
	int posX = location.x - _startX, posY = location.y - _startY;
	return (_lights[(((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + location.z] & 0xF);
}

short Chunk::getLightLevel( int posX, int posY, int posZ )
{
	if (!_lights) {
		return (0);
		// std::cout << "getLightLevel too soon " << _startX << ", " << _startY << std::endl;
		// while (!_genDone); // hello neighbour please wait for me to finish to load my light and blocks
		// if (_lights) {
		// 	std::cout << "done waiting " << _startX << ", " << _startY << std::endl;
		// } else {
		// 	std::cout << "ABORT" << std::endl;
		// }
	}
	return (_lights[(((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + posZ]);
}

void Chunk::light_try_spread( int posX, int posY, int posZ, short level, bool skySpread )
{
	if (posZ < 0 || posZ > 255) {
		return ;
	}
	if (posX < 0 || posX >= CHUNK_SIZE || posY < 0 || posY >= CHUNK_SIZE) {
		// spread neighbour
		if (posX == -1) {
			if (_neighbours[face_dir::MINUSX]) {
				_neighbours[face_dir::MINUSX]->light_try_spread(CHUNK_SIZE - 1, posY, posZ, level, skySpread);
			}
		} else if (posY == -1) {
			if (_neighbours[face_dir::MINUSY]) {
				_neighbours[face_dir::MINUSY]->light_try_spread(posX, CHUNK_SIZE - 1, posZ, level, skySpread);
			}
		} else if (posX == CHUNK_SIZE) {
			if (_neighbours[face_dir::PLUSX]) {
				_neighbours[face_dir::PLUSX]->light_try_spread(0, posY, posZ, level, skySpread);
			}
		} else if (posY == CHUNK_SIZE) {
			if (_neighbours[face_dir::PLUSY]) {
				_neighbours[face_dir::PLUSY]->light_try_spread(posX, 0, posZ, level, skySpread);
			}
		}
	} else {
		int value = _blocks[(((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + posZ];
		if (air_flower(value, true, true, false) && value != blocks::OAK_SLAB) {
			return ;
		}
		short neighbour = (_lights[(((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + posZ] >> (8 * skySpread));
		if ((!(neighbour & 0xF0) || (skySpread && !(level & 0xF0))) && ((neighbour & 0xF) != maxs(0, (level & 0xF) - 1))) { // only spread to non source blocks whose value is not expected value. Also spread to source block if from non source block and skySpread
			light_spread(posX, posY, posZ, skySpread);
		}
	}
}
