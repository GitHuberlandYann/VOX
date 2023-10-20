#include "vox.h"

Chunk::Chunk( Camera *camera, int posX, int posY, std::vector<Chunk *> *perimeter_chunks )
	: _isVisible(true), _vaoSet(false), _vaoVIP(false),
	_waterVaoSet(false), _waterVaoVIP(false),
	_skyVaoSet(false), _skyVaoVIP(false),
	_vaoReset(false), _waterVaoReset(false), _skyVaoReset(false),
	_startX(posX), _startY(posY),
	_blocks(NULL), _vertices(NULL), _water_vert(NULL), _sky_vert(NULL), _sky(NULL), _hasWater(true), _displayed_faces(0), _water_count(0), _sky_count(0),
	_vis_chunks(perimeter_chunks), _camera(camera)
{
}

Chunk::~Chunk( void )
{
	if (_thread.joinable()) {
		_thread.join();
	}

	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_skyVbo);
	glDeleteVertexArrays(1, &_skyVao);

	delete [] _blocks;
	delete [] _vertices;
	delete [] _water_vert;
	delete [] _sky;
	delete [] _sky_vert;

	_orientations.clear();
	_added.clear();
	_removed.clear();
	// std::cout << "chunk deleted " << _startX << ", " << _startY << std::endl;
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void Chunk::gen_ore_blob( int ore_type, int row, int col, int level, int & blob_size, int dir)
{
	if (!row || !col || !level || !blob_size
		|| row == CHUNK_SIZE + 1 || col == CHUNK_SIZE + 1 || level == WORLD_HEIGHT
		|| _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] != blocks::STONE) {
		return ;
	}
	_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = ore_type;
	--blob_size;
	for (int i = 0; i < 6; i++) {
		if (dir == 6) {
			dir = 0;
		}
		const GLint delta[3] = {adj_blocks[dir][0], adj_blocks[dir][1], adj_blocks[dir][2]};
		++dir;
		gen_ore_blob(ore_type, row + delta[0], col + delta[1], level + delta[2], blob_size, dir);
	}
}

GLint Chunk::face_count( int type, int row, int col, int level, bool isNotLeaves )
{
	if (type >= blocks::POPPY) {
		return (2);
	}
	GLint res = !air_flower(_blocks[((row - 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], true, false)
				+ !air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], isNotLeaves, false)
				+ !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col - 1) * WORLD_HEIGHT + level], true, false)
				+ !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level], isNotLeaves, false);
	switch (level) {
		case 0:
			res += !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1], isNotLeaves, false);
			break ;
		case 255:
			res += !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], true, false);
			res += 1;
			break ;
		default:
			res += !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], true, false);
			res += !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1], isNotLeaves, false);
	}
	return (res);
}

GLint Chunk::get_empty_faces( int type, int row, int col, int level, bool isNotLeaves )
{
	GLint res = !!air_flower(_blocks[((row - 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], true, false) * (1 << 2)
				+ !!air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], isNotLeaves, false) * (1 << 3)
				+ !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col - 1) * WORLD_HEIGHT + level], true, false) * (1 << 0)
				+ !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level], isNotLeaves, false) * (1 << 1);
	switch (level) {
		case 0:
			res += (1 << 5);
			res += !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1], isNotLeaves, false) * (1 << 4);
			break ;
		case 255:
			res += !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], true, false) * (1 << 5);
			// res += (1 << 4); // don't wan't to hide face from above, because we can be at pos 257 and looking at block 255
			break ;
		default:
			res += !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], true, false) * (1 << 5);
			res += !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1], isNotLeaves, false) * (1 << 4);
	}
	if (type >= blocks::CRAFTING_TABLE && type < blocks::BEDROCK) { // oriented block
		std::map<int, int>::iterator search = _orientations.find((row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level);
		if (search != _orientations.end()) {
			res += (1 << (6 + search->second));
		}
	}
	return (res);
}

// takes a block and check if player can see it at some point
bool Chunk::exposed_block( int row, int col, int level, bool isNotLeaves )
{
	// isNotLeaves = true;
	if (!isNotLeaves) {
		return (true);
	}
	bool below, above;
	switch (level) {
		case 0:
			below = false;
			above = !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1], isNotLeaves, false);
			break ;
		case 255:
			below = !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], isNotLeaves, false);
			above = true;
			break ;
		default:
			below = !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], isNotLeaves, false);
			above = !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1], isNotLeaves, false);
	}
	return (!air_flower(_blocks[((row - 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], isNotLeaves, false)
		|| !air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], isNotLeaves, false)
		|| !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col - 1) * WORLD_HEIGHT + level], isNotLeaves, false)
		|| !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level], isNotLeaves, false)
		|| below || above);
}

int Chunk::get_block_type_cave( int row, int col, int level, int ground_level,
	bool poppy, bool dandelion, bool blue_orchid, bool allium, bool cornflower, bool pink_tulip,
	bool grass, bool tree_gen, std::vector<glm::ivec3> & trees )
{
	int value = blocks::AIR;
	if (level == ground_level + 1 && ground_level > 9) {
		value = blocks::GRASS_BLOCK;
	} else if (level < 12) {
		// value = blocks::WATER;
	} else if (tree_gen && level <= ground_level + 6 && ground_level > 9) {
		value = blocks::OAK_LOG;
		if (level == ground_level + 6) {
			trees.push_back(glm::ivec3(row, col, level));
		}
	} else if (level == ground_level + 2 && ground_level > 9) {
		if (pink_tulip) {
			value = blocks::POPPY;
		} else if (blue_orchid) {
			value = blocks::DANDELION;
		} else if (dandelion) {
			value = blocks::BLUE_ORCHID;
		} else if (cornflower) {
			value = blocks::ALLIUM;
		} else if (allium) {
			value = blocks::CORNFLOWER;
		} else if (poppy) {
			value = blocks::PINK_TULIP;
		} else if (grass) {
			value = blocks::GRASS;
		}
	}
	return (value);
}

int Chunk::get_block_type( siv::PerlinNoise perlin, int row, int col, int level, int surface_level,
	bool poppy, bool dandelion, bool blue_orchid, bool allium, bool cornflower, bool pink_tulip,
	bool grass, bool tree_gen, std::vector<glm::ivec3> & trees )
{
	if (level == 0) {
		return (blocks::BEDROCK);
	}
	int value = (level > surface_level) ? blocks::AIR : (level < surface_level - 2 || level < SEA_LEVEL) ? blocks::STONE : blocks::GRASS_BLOCK;
	if (value) {
		if (_continent == cont::CONT_MUSHROOM_FIELDS) {
			return (blocks::DIAMOND_ORE);
		}
		if (value == blocks::STONE) {
			double undergound = perlin.noise3D_01((_startX - 1 + row) / 100.0f, (_startY - 1 + col) / 100.0f, level / 100.0f);
			(undergound < 0.55) ? : value = blocks::GRAVEL;//blocks::SAND;
		} else if (value == blocks::GRASS_BLOCK && _continent <= cont::CONT_COAST) {
			value = blocks::SAND;
		}
	} else if (level <= SEA_LEVEL) {
		value = blocks::WATER;
	} else if (_continent <= cont::CONT_COAST) {
		if (surface_level >= SEA_LEVEL && level <= surface_level + 3) {
			if (tree_gen) {
				value = blocks::CACTUS;
			} else if (dandelion && surface_level == SEA_LEVEL) {
				value = blocks::SUGAR_CANE;
			} else if (level == surface_level + 1) {
				if (poppy) {
					value = blocks::DEAD_BUSH;
				}
			}
		}
	} else if (tree_gen && surface_level > 65 && level <= surface_level + 5 && surface_level < 220) {
		value = blocks::OAK_LOG;
		if (level == surface_level + 5) {
			trees.push_back(glm::ivec3(row, col, level));
		}
	} else if (level == surface_level + 1 && surface_level > 65 && surface_level < 220) {
		if (poppy) {
			value = blocks::POPPY;
		} else if (dandelion) {
			value = blocks::DANDELION;
		} else if (blue_orchid) {
			value = blocks::BLUE_ORCHID;
		} else if (allium) {
			value = blocks::ALLIUM;
		} else if (cornflower) {
			value = blocks::CORNFLOWER;
		} else if (pink_tulip) {
			value = blocks::PINK_TULIP;
		} else if (grass) {
			value = blocks::GRASS;
		}
	}
	return (value);
}

int Chunk::surfaceLevel( int row, int col, siv::PerlinNoise perlin )
{
	_continent = cont::CONT_MID_INLAND;
	// return glm::floor(SEA_LEVEL + (perlin.octave2D_01((_startX - 1 + row) / 100.0f, (_startY - 1 + col) / 100.0f, 4) - 0.5) * 100);
	double continentalness = (perlin.octave2D_01((_startX - 1 + row) / 1000.0f, (_startY - 1 + col) / 1000.0f, 2) - 0.5) * 2 * 3.80;
	int offset;
	if (continentalness < -2.05) {//-1.05) {
		offset = 50;	// mushroom
		_continent = cont::CONT_MUSHROOM_FIELDS;
	} else if (continentalness < -0.455) {
		// offset = -20;	// deep ocean
		offset = gradient(continentalness, -1.05, -0.455, -40, -10);
		_continent = cont::CONT_DEEP_OCEAN;
	} else if (continentalness < -0.19) {
		// offset = -10;	// ocean
		offset = gradient(continentalness, -0.455, -0.19, -10, 0);
		_continent = cont::CONT_OCEAN;
	} else if (continentalness < -0.11) {
		// offset = 0;	// coast
		offset = gradient(continentalness, -0.19, -0.11, 0, 10);
		_continent = cont::CONT_COAST;
	} else if (continentalness < 0.03) {
		// offset = 10; // near inland
		offset = gradient(continentalness, -0.11, 0.03, 10, 30);
		_continent = cont::CONT_NEAR_INLAND;
	} else if (continentalness < 0.3) {
		// offset = 20;	// mid inland
		offset = gradient(continentalness, 0.03, 0.3, 30, 40);
		_continent = cont::CONT_MID_INLAND;
	} else if (continentalness < 0.4) {
		// high diff in short span
		offset = gradient(continentalness, 0.3, 0.4, 40, 80);
		_continent = cont::CONT_FAR_INLAND;
	} else {
		offset = 80;	// far-inland
		// offset = gradient(continentalness, 0.3, 3.80, 80, 110);
		_continent = cont::CONT_FAR_INLAND;
	}
	// if (continentalness > -0.11) { // inland
		// double weirdness = (perlin.octave2D_01((_startX - 1 + row) / 200.0f, (_startY - 1 + col) / 200.0f, 4) - 0.5) * 2 * 2.97;
		// if (weirdness > -1.0 && weirdness < 1.0) {
			// double peaks_valleys = 1 - glm::abs(3 * glm::abs(weirdness) - 2);
			double peaks_valleys = (perlin.octave2D_01((_startX - 1 + row) / 100.0f, (_startY - 1 + col) / 100.0f, 4) - 0.5) * 2 * 2.97;
			offset += 4 * peaks_valleys;
		// }
	// }

	if (_continent == cont::CONT_MUSHROOM_FIELDS) { // no rivers in mushroom biomes
		return (SEA_LEVEL + offset);
	}
	double erosion = glm::abs(perlin.octave2D_01((_startX - 1000 + row) / 1000.0f, (_startY - 1000 + col) / 1000.0f, 7) - 0.5);
	return (gradient(erosion, 0, 0.05, SEA_LEVEL - 6, SEA_LEVEL + offset));
	// return (SEA_LEVEL + offset);
}

void Chunk::generate_blocks( void )
{
	const siv::PerlinNoise perlin{ perlin_seed };
	std::minstd_rand0  generator((_startX * 19516 + _startY * 56849) * perlin_seed);
	// std::minstd_rand0  generator(perlin.noise2D(_startX * _startX, _startY * _startY) * perlin_seed);
	std::uniform_int_distribution<int> distribution(0, 1000);

	std::vector<glm::ivec3> trees;

	// generating base terrain
	for (int row = 0; row < (CHUNK_SIZE + 2); row++) {
		for (int col = 0; col < (CHUNK_SIZE + 2); col++) {
			// int surface_level = glm::floor(SEA_LEVEL + (perlin.octave2D_01((_startX - 1 + row) / 100.0f, (_startY - 1 + col) / 100.0f, 4) - 0.5) * 100);
			int surface_level = surfaceLevel(row, col, perlin);
			bool tree_gen = (distribution(generator) <= 2 && row > 2 && row < CHUNK_SIZE - 1 && col > 2 && col < CHUNK_SIZE - 1);
			bool poppy = (distribution(generator) <= 2 && row >= 1 && row <= CHUNK_SIZE && col >= 1 && col <= CHUNK_SIZE);
			bool dandelion = (distribution(generator) <= 2 && row >= 1 && row <= CHUNK_SIZE && col >= 1 && col <= CHUNK_SIZE);
			bool blue_orchid = (distribution(generator) <= 2 && row >= 1 && row <= CHUNK_SIZE && col >= 1 && col <= CHUNK_SIZE);
			bool allium = (distribution(generator) <= 2 && row >= 1 && row <= CHUNK_SIZE && col >= 1 && col <= CHUNK_SIZE);
			bool cornflower = (distribution(generator) <= 2 && row >= 1 && row <= CHUNK_SIZE && col >= 1 && col <= CHUNK_SIZE);
			bool pink_tulip = (distribution(generator) <= 2 && row >= 1 && row <= CHUNK_SIZE && col >= 1 && col <= CHUNK_SIZE);
			bool grass = (distribution(generator) <= 100 && row >= 1 && row <= CHUNK_SIZE && col >= 1 && col <= CHUNK_SIZE);
			double pillar = perlin.noise2D_01((_startX + row) / 20.0f, (_startY + col) / 20.0f);
			int ground_cave = 7 + perlin.octave2D_01((_startX - 1 + row) / 100.0f, (_startY - 1 + col) / 100.0f, 4) * 14;
			for (int level = 0; level < WORLD_HEIGHT; level++) {
				// // double cave = perlin.octave3D_01((_startX + row) / 100.0f, (_startY + col) / 100.0f, (level) / 100.0f, 4);
				// // double cheese = perlin.octave3D_01((_startX + row) / 100.0f, (_startY + col) / 100.0f, (level) / 100.0f, 4);
				// // double bridge = perlin.noise2D_01((_startX + row) / 60.0f, (level) / 15.0f);
				// // double obridge = perlin.noise2D_01((_startY + col) / 60.0f, (level) / 15.0f);
				// // (level < surface_level - 5 && ((cave >= 0.459 && cave <= 0.551f) && !(pillar < 0.3f - 0.7f * (surface_level / 2 - glm::abs(level - surface_level / 2)) / (5.0f * surface_level)) && !(bridge < 0.2f) && !(obridge < 0.2f)) && level > ground_cave)
				// // (level < surface_level - 5 && cave <= 0.2f && level > 0) //  * (1 - (0.5 + glm::abs(level - surface_level / 2) / surface_level))
				double cave = perlin.octave3D_01((_startX + row) / 750.0f, (_startY + col) / 750.0f, (level) / 750.0f, 4); // big holes
				(level < surface_level - 5 && ((cave >= 0.459 && cave <= 0.551f) && !(pillar < 0.3f - 0.7f * (surface_level / 2 - glm::abs(level - surface_level / 2)) / (5.0f * surface_level))) && level > ground_cave)
					? _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = blocks::AIR//get_block_type_cave(row, col, level, ground_cave, poppy, dandelion, blue_orchid, allium, cornflower, pink_tulip, grass, tree_gen, trees)
					: _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = get_block_type(perlin, row, col, level, surface_level, poppy, dandelion, blue_orchid, allium, cornflower, pink_tulip, grass, tree_gen, trees);
				// _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = get_block_type(perlin, row, col, level, surface_level, poppy, dandelion, blue_orchid, allium, cornflower, pink_tulip, grass, tree_gen, trees);
				// GLfloat squashing_factor;
				// (level < 64)
				// 	? squashing_factor = (64 - level) / 64.0f
				// 	: squashing_factor = -(level - 64) / 192.0f;
				// // // std::cout << "squash at " << level << ": " << squashing_factor << std::endl;
				// _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = ((perlin.octave3D_01((_startX - 1 + row) / 100.0f, (_startY - 1 + col) / 100.0f, level / 20.0f, 4) + squashing_factor) > 0.5f);
				// if (row == 9 && col == 9 && level == surface_level + 5) {
				// 	_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = 1;
				// }
			}
		}
	}

	// adding leaves to trees
	for (auto& tree: trees) { // TODO put trees on chunk border and communicate with border chunks
		for (int index = 0; index < 61; index++) {
			const GLint delta[3] = {oak_normal[index][0], oak_normal[index][1], oak_normal[index][2]};
			if (_blocks[((tree.x + delta[0]) * (CHUNK_SIZE + 2) + tree.y + delta[1]) * WORLD_HEIGHT + tree.z + delta[2]] == blocks::AIR) {
				std::set<int>::iterator search = _removed.find(((tree.x + delta[0]) * (CHUNK_SIZE + 2) + tree.y + delta[1]) * WORLD_HEIGHT + tree.z + delta[2]);
				if (search == _removed.end()) {
					_blocks[((tree.x + delta[0]) * (CHUNK_SIZE + 2) + tree.y + delta[1]) * WORLD_HEIGHT + tree.z + delta[2]] = blocks::OAK_LEAVES;
				}
			}
		}
	}
	trees.clear();

	// generating ores
	bool gravel_blob = false;
	int coal_blobs = 0, iron_blobs = 0, diamond_blobs = 0;
	for (int row = 1; row < CHUNK_SIZE + 1; row++) {
		for (int col = 1; col < CHUNK_SIZE + 1; col++) {
			for (int level = 0; level < WORLD_HEIGHT; level++) {
				GLint value = _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level];
				if (value == blocks::STONE) {
					if (coal_blobs < 20 && distribution(generator) <= 1) {
						int size = (distribution(generator) + 3) / 30;
						gen_ore_blob(blocks::COAL_ORE, row, col, level, size, 0);
						coal_blobs++;
					} else if (iron_blobs < 10 && (level <= 56 || level > 80) && distribution(generator) <= 1) {
						int size = (distribution(generator) + 3) / 60;
						gen_ore_blob(blocks::IRON_ORE, row, col, level, size, 0);
						iron_blobs++;
					} else if (!gravel_blob && distribution(generator) <= 1) {
						int size = 60;
						gen_ore_blob(blocks::GRAVEL, row, col, level, size, 0);
						gravel_blob = true;
					} else if (diamond_blobs < 7 && level < 15 && distribution(generator) <= 1) {
						int size = (distribution(generator) + 5) / 200;
						gen_ore_blob(blocks::DIAMOND_ORE, row, col, level, size, 0);
						diamond_blobs++;
					}
				}
				// turning grass into dirt if under other block
				if (value == blocks::GRASS_BLOCK && level != 255) {
					int block_above = _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1];
					if (block_above && block_above < blocks::POPPY) {
						_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = blocks::DIRT;
					}
				}
			}
		}
	}

	// reverting ores back to stone if they are exposed
	for (int row = 1; row < CHUNK_SIZE + 1; row++) {
		for (int col = 1; col < CHUNK_SIZE + 1; col++) {
			for (int level = 0; level < WORLD_HEIGHT; level++) {
				GLint value = _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level];
				if (value) {
					if (value == blocks::WATER) {
						// do nothing
					} else if (exposed_block(row, col, level, value != blocks::OAK_LEAVES)) {
						if (value == blocks::IRON_ORE && distribution(generator) < 500) {
							_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = blocks::STONE;
						} else if (value == blocks::GRAVEL && distribution(generator) < 100) {
							_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = blocks::STONE;
						} else if (value == blocks::DIAMOND_ORE && distribution(generator) < 900) {
							_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = blocks::STONE;
						} else if (value == blocks::COAL_ORE && distribution(generator) < 200) {
							_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = blocks::STONE;
						} 
					}
				}
			}
		}
	}

	// loading backup, MUST be done after all randomness to not alter anything
	if (_added.size() || _removed.size()) {
		for (int row = 0; row < CHUNK_SIZE + 2; row++) {
			for (int col = 0; col < CHUNK_SIZE + 2; col++) {
				for (int level = 0; level < WORLD_HEIGHT; level++) {
					std::map<int, int>::iterator search = _added.find((row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level);
					if (search != _added.end()) {
						_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = search->second;
					} else {
						std::set<int>::iterator rmsearch = _removed.find((row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level);
						if (rmsearch != _removed.end()) {
							_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = blocks::AIR;
						}
					}
				}
			}
		}
	}

	// counting displayed blocks and hiding unseen blocks
	for (int row = 1; row < CHUNK_SIZE + 1; row++) {
		for (int col = 1; col < CHUNK_SIZE + 1; col++) {
			for (int level = 0; level < WORLD_HEIGHT; level++) {
				GLint value = _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level];
				if (value && value < blocks::WATER) {
					GLint count = face_count(value, row, col, level, value != blocks::OAK_LEAVES);
					if (count) {
						// std::cout << "count is " << count << std::endl;
						_displayed_faces += count;
					} else {
						_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] -= blocks::NOTVISIBLE;
						if (value != blocks::OAK_LEAVES && exposed_block(row, col, level, true)) {
							std::cerr << "BLOCK EXPOSED BUT COUNT IS 0: " << s_blocks[value].name << std::endl;
						}
					}
				}
			}
		}
	}
}

void Chunk::generate_sky( void )
{
	const siv::PerlinNoise perlin{ perlin_seed };

	for (int row = 0; row < (CHUNK_SIZE + 2); row++) {
		for (int col = 0; col < (CHUNK_SIZE + 2); col++) {
			double sky = perlin.octave2D_01((_startX - 1 + row) / 100.0f, (_startY - 1 + col) / 100.0f, 4);
			_sky[row * (CHUNK_SIZE + 2) + col] = (sky < 0.5);
		}
	}

	for (int row = 1; row < (CHUNK_SIZE + 1); row++) {
		for (int col = 1; col < (CHUNK_SIZE + 1); col++) {
			if (_sky[row * (CHUNK_SIZE + 2) + col]) {
				_sky_count += 2; // top and bottom faces
				_sky_count += !_sky[(row - 1) * (CHUNK_SIZE + 2) + col];
				_sky_count += !_sky[(row + 1) * (CHUNK_SIZE + 2) + col];
				_sky_count += !_sky[row * (CHUNK_SIZE + 2) + col - 1];
				_sky_count += !_sky[row * (CHUNK_SIZE + 2) + col + 1];
			}
		}
	}
// *18 because 2 triangles/face, which is (4 coord * 3 points * 2 triang) int
	_sky_vert = new GLint[_sky_count * 24]; // will be filled in later with a call to sort_sky
}

int Chunk::sand_fall_endz( glm::ivec3 pos )
{
	for (int level = pos.z - 1; level > 0; level--) {
		if (_blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + level] != blocks::AIR) { // and diff water later
			return (level + 1);
		}
	}
	return (1);
}

void Chunk::handle_border_block( glm::ivec3 pos, int type, bool adding )
{
	if (type == blocks::AIR) {
		return ;
	}
	if (!pos.x) {
		for (auto& c : *_vis_chunks) {
			if (c->isInChunk(_startX - CHUNK_SIZE, _startY)) {
				c->update_border(CHUNK_SIZE + 1, pos.y + 1, pos.z, type, adding);
				break ;
			}
		}
	} else if (pos.x == CHUNK_SIZE - 1) {
		for (auto& c : *_vis_chunks) {
			if (c->isInChunk(_startX + CHUNK_SIZE, _startY)) {
				c->update_border(0, pos.y + 1, pos.z, type, adding);
				break ;
			}
		}
	}
	if (!pos.y) {
		for (auto& c : *_vis_chunks) {
			if (c->isInChunk(_startX, _startY - CHUNK_SIZE)) {
				c->update_border(pos.x + 1, CHUNK_SIZE + 1, pos.z, type, adding);
				break ;
			}
		}
	} else if (pos.y == CHUNK_SIZE - 1) {
		for (auto& c : *_vis_chunks) {
			if (c->isInChunk(_startX, _startY + CHUNK_SIZE)) {
				c->update_border(pos.x + 1, 0, pos.z, type, adding);
				break ;
			}
		}
	}
}

void Chunk::remove_block( Inventory *inventory, glm::ivec3 pos )
{
	// std::cout << "in chunk " << _startX << ", " << _startY << ":rm block " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
	// std::cout << "nb displayed blocks before: " << _displayed_blocks << std::endl;
	int value = _blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z];
	if (value >= blocks::CRAFTING_TABLE && value < blocks::BEDROCK) { // oriented blocks
		_orientations.erase(((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z);
	}
	_added.erase(((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z);
	_removed.insert(((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z);
	if (value == blocks::FURNACE) {
		_furnaces.erase(((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z); // TODO drop furnace's items/ put them in inventory
	}
	mtx_inventory.lock();
	if (inventory) {
		inventory->addBlock(value + (value < 0) * blocks::NOTVISIBLE);
	}
	mtx_inventory.unlock();
	int endZ = -1;
	int block_above = _blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z + 1];
	if (pos.z < 255 && isSandOrGravel(block_above)) { // sand falls if block underneath deleted
		_blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z] = blocks::AIR;
		endZ = sand_fall_endz(pos);
		int above_value = (block_above == blocks::SAND || block_above + blocks::NOTVISIBLE == blocks::SAND) ? blocks::SAND : blocks::GRAVEL;
		if (endZ == pos.z) {
			_blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z] = above_value - (value < 0) * blocks::NOTVISIBLE;
			return (remove_block(NULL, glm::ivec3(pos.x, pos.y, pos.z + 1)));
		} else {
			add_block(NULL, glm::ivec3(pos.x, pos.y, endZ), above_value, blocks::AIR);
		}
	}
	handle_border_block(pos, air_flower(value, true, true), false); // if block at border of chunk gets deleted, we update neighbours // TODO might use false, true to update leaves too ?
	_blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z] = blocks::AIR;
	if (value > blocks::AIR && value < blocks::WATER) { // if invisible block gets deleted, same amount of displayed_blocks
		_mtx.lock();
		_displayed_faces -= face_count(value, pos.x + 1, pos.y + 1, pos.z, value != blocks::OAK_LEAVES);
		_mtx.unlock();
	} else if (value == blocks::WATER) { // use bucket on water source
		// std::cout << "bucket on water" << std::endl;
		_fluids.insert(pos.x + 1 + ((pos.y + 1) << 8) + (pos.z << 16));
		return ;
	}
	if (air_flower(value, true, true)) {
		for (int index = 0; index < 6; index++) {
			const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
			if (pos.x + delta[0] < 0 || pos.x + delta[0] >= CHUNK_SIZE || pos.y + delta[1] < 0 || pos.y + delta[1] >= CHUNK_SIZE || pos.z + delta[2] < 0 || pos.z + delta[2] > 255) {

			} else {
				int adj = _blocks[((pos.x + delta[0] + 1) * (CHUNK_SIZE + 2) + pos.y + delta[1] + 1) * WORLD_HEIGHT + pos.z + delta[2]];
				if (adj < blocks::AIR) {
					_blocks[((pos.x + delta[0] + 1) * (CHUNK_SIZE + 2) + pos.y + delta[1] + 1) * WORLD_HEIGHT + pos.z + delta[2]] += blocks::NOTVISIBLE;
					_mtx.lock();
					_displayed_faces++;
					_mtx.unlock();
				} else if (air_flower(adj, true, false)) {
					_mtx.lock();
					_displayed_faces++;
					_mtx.unlock();
				} else if (index != 4 && adj >= blocks::WATER) { // if water under, top was already displayed
					// ++_water_count;
					std::cout << "WATER UPDATE ON " << s_blocks[adj].name << std::endl;
					if (adj != blocks::WATER) { // block added and not water bucket
						_fluids.insert((pos.x + 1 + delta[0]) + ((pos.y + 1 + delta[1]) << 8) + ((pos.z + delta[2]) << 16));
					}
					// std::cout << "++water count" << std::endl;
				}
			}
		}
	}
	// if (value == blocks::WATER) {
	// 	// std::cout << "removed water" << std::endl;
	// 	return ;
	// }
	if (endZ != -1) { // sand fall
		remove_block(NULL, glm::ivec3(pos.x, pos.y, pos.z + 1));
	} else if (pos.z < 255 && ((block_above >= blocks::POPPY && block_above < blocks::WATER) || block_above == blocks::CACTUS)) { // del flower if block underneath deleted
		(block_above == blocks::GRASS)
			? remove_block(NULL, glm::ivec3(pos.x, pos.y, pos.z + 1)) // if grass above breaked block, don't collect it
			: remove_block(inventory, glm::ivec3(pos.x, pos.y, pos.z + 1));
	}
	// std::cout << "nb displayed blocks after: " << _displayed_blocks << std::endl;
}

void Chunk::add_block( Inventory *inventory, glm::ivec3 pos, int type, int previous )
{
	// std::cout << "in chunk " << _startX << ", " << _startY << ":rm block " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
	// std::cout << "nb displayed blocks before: " << _displayed_blocks << std::endl;
	if (type == blocks::WATER) { // we place water
		if (previous < blocks::WATER) {
			_hasWater = true;
		}
	} else if (type >= blocks::POPPY && pos.z > 0) {
		if (previous >= blocks::WATER) {
			return ;
		}
		int block_below = _blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z - 1];
		if (block_below != blocks::GRASS_BLOCK && block_below != blocks::DIRT && block_below != blocks::SAND) {
			return ; // can't place flower on something else than grass/dirt block
		}
	} else if (previous >= blocks::WATER) { // replace water block with something else
		_fluids.insert(pos.x + 1 + ((pos.y + 1) << 8) + (pos.z << 16));
	}
	mtx_inventory.lock();
	if (inventory) {
		inventory->removeBlock();
	}
	mtx_inventory.unlock();
	if (type >= blocks::CRAFTING_TABLE && type < blocks::BEDROCK) { // oriented blocks
		_orientations[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z] = _camera->getOrientation();
	}
	if (type == blocks::SAND || type == blocks::GRAVEL) {
		pos.z = sand_fall_endz(pos);
	}
	handle_border_block(pos, air_flower(type, false, true), true); // if block at border of chunk gets added, we update neighbours
	_blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z] = type;
	_removed.erase(((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z);
	_added[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z] = type;
	if (type == blocks::WATER) {
		_fluids.insert(pos.x + 1 + ((pos.y + 1) << 8) + (pos.z << 16));
		// std::cout << "adding water" << std::endl;
		return ;
	}
	if (type == blocks::FURNACE) {
		_furnaces[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z] = FurnaceInstance();
	}
	_mtx.lock();
	_displayed_faces += face_count(type, pos.x + 1, pos.y + 1, pos.z, type != blocks::OAK_LEAVES);
	_mtx.unlock();
	if (!air_flower(type, true, false)) {
		return ;
	}
	for (int index = 0; index < 6; index++) {
		const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
		if (pos.x + delta[0] < 0 || pos.x + delta[0] >= CHUNK_SIZE || pos.y + delta[1] < 0 || pos.y + delta[1] >= CHUNK_SIZE || pos.z + delta[2] < 0 || pos.z + delta[2] > 255) {

		} else {
			GLint value = _blocks[((pos.x + delta[0] + 1) * (CHUNK_SIZE + 2) + pos.y + delta[1] + 1) * WORLD_HEIGHT + pos.z + delta[2]];
			if (air_flower(value, true, false)) {
				_mtx.lock();
				_displayed_faces--;
				_mtx.unlock();
			}
			if (value > blocks::AIR && !exposed_block(pos.x + delta[0] + 1, pos.y + delta[1] + 1, pos.z + delta[2], value != blocks::OAK_LEAVES)) {
				// was exposed before, but isn't anymore
				_blocks[((pos.x + delta[0] + 1) * (CHUNK_SIZE + 2) + pos.y + delta[1] + 1) * WORLD_HEIGHT + pos.z + delta[2]] -= blocks::NOTVISIBLE;
			}
		}
	}
	// std::cout << "nb displayed blocks after: " << _displayed_blocks << std::endl;
}

void Chunk::fill_vertex_array( void )
{
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
						bool isNotLeaves = (block_type != blocks::OAK_LEAVES);
						if (!air_flower(_blocks[((row + 1 - 1) * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level], true, false)) {
							int spec = blockGridX(block_type, 0) + (blockGridY(block_type) << 4) + (0 << 12) + (84 << 16);
							glm::ivec4 v0 = {spec, p4};
							glm::ivec4 v1 = {spec + 1 + (1 << 8), p0};
							glm::ivec4 v2 = {spec + (1 << 4) + (1 << 12), p6};
							glm::ivec4 v3 = {spec + 1 + (1 << 4) + (1 << 8) + (1 << 12), p2};
							// std::cout << "debug spec v0 " << s_blocks[block_type].name << ": x = " << ((v0.x >> 8) & 0xF) << ", y = " << ((v0.x >> 12) & 0xF) << std::endl;
							// std::cout << "debug spec v1 " << s_blocks[block_type].name << ": x = " << ((v1.x >> 8) & 0xF) << ", y = " << ((v1.x >> 12) & 0xF) << std::endl;
							// std::cout << "debug spec v2 " << s_blocks[block_type].name << ": x = " << ((v2.x >> 8) & 0xF) << ", y = " << ((v2.x >> 12) & 0xF) << std::endl;
							// std::cout << "debug spec v3 " << s_blocks[block_type].name << ": x = " << ((v3.x >> 8) & 0xF) << ", y = " << ((v3.x >> 12) & 0xF) << std::endl;
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (!air_flower(_blocks[((row + 1 + 1) * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level], isNotLeaves, false)) {
							int spec = blockGridX(block_type, 0) + (blockGridY(block_type) << 4) + (0 << 12) + (80 << 16);
							glm::ivec4 v0 = {spec, p1};
							glm::ivec4 v1 = {spec + 1 + (1 << 8), p5};
							glm::ivec4 v2 = {spec + (1 << 4) + (1 << 12), p3};
							glm::ivec4 v3 = {spec + 1 + (1 << 4) + (1 << 8) + (1 << 12), p7};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (!air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col + 1 - 1) * WORLD_HEIGHT + level], true, false)) {
							int spec = blockGridX(block_type, 0) + (blockGridY(block_type) << 4) + (0 << 12) + (92 << 16);
							glm::ivec4 v0 = {spec, p0};
							glm::ivec4 v1 = {spec + 1 + (1 << 8), p1};
							glm::ivec4 v2 = {spec + (1 << 4) + (1 << 12), p2};
							glm::ivec4 v3 = {spec + 1 + (1 << 4) + (1 << 8) + (1 << 12), p3};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (!air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col + 1 + 1) * WORLD_HEIGHT + level], isNotLeaves, false)) {
							int spec = blockGridX(block_type, 0) + (blockGridY(block_type) << 4) + (0 << 12) + (88 << 16);
							glm::ivec4 v0 = {spec, p5};
							glm::ivec4 v1 = {spec + 1 + (1 << 8), p4};
							glm::ivec4 v2 = {spec + (1 << 4) + (1 << 12), p7};
							glm::ivec4 v3 = {spec + 1 + (1 << 4) + (1 << 8) + (1 << 12), p6};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (!air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level + 1], isNotLeaves, false)) {
							int spec = blockGridX(block_type, 1) + (blockGridY(block_type) << 4) + (0 << 12) + (100 << 16);
							glm::ivec4 v0 = {spec, p4};
							glm::ivec4 v1 = {spec + 1 + (1 << 8), p5};
							glm::ivec4 v2 = {spec + (1 << 4) + (1 << 12), p0};
							glm::ivec4 v3 = {spec + 1 + (1 << 4) + (1 << 8) + (1 << 12), p1};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
						if (!air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level - 1], true, false)) {
							int spec = blockGridX(block_type, 1) + (blockGridY(block_type) << 4) + (0 << 12) + (74 << 16);
							glm::ivec4 v0 = {spec, p2};
							glm::ivec4 v1 = {spec + 1 + (1 << 8), p3};
							glm::ivec4 v2 = {spec + (1 << 4) + (1 << 12), p6};
							glm::ivec4 v3 = {spec + 1 + (1 << 4) + (1 << 8) + (1 << 12), p7};
							face_vertices(_vertices, v0, v1, v2, v3, index);
						}
					} else {
						int spec = blockGridX(block_type, 0) + (blockGridY(block_type) << 4) + (0 << 12) + (100 << 16);
							glm::ivec4 v0 = {spec, p0};
							glm::ivec4 v1 = {spec + 1 + (1 << 8), p5};
							glm::ivec4 v2 = {spec + (1 << 4) + (1 << 12), p2};
							glm::ivec4 v3 = {spec + 1 + (1 << 4) + (1 << 8) + (1 << 12), p7};
							face_vertices(_vertices, v0, v1, v2, v3, index);
							v0 = {spec, p1};
							v1 = {spec + 1 + (1 << 8), p4};
							v2 = {spec + (1 << 4) + (1 << 12), p3};
							v3 = {spec + 1 + (1 << 4) + (1 << 8) + (1 << 12), p6};
							face_vertices(_vertices, v0, v1, v2, v3, index);
					}
				}
			}
		}
	}
	if (index != _displayed_faces * 24) {
		std::cout << "index at end is " << index << " vs " << _displayed_faces << " | " << _displayed_faces * 4 * 6 << std::endl;
	}
}

void Chunk::setup_array_buffer( void )
{
	if (_thread.joinable()) {
		_thread.join();
	}

	if (!_vaoSet) {
		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vbo);
		_vaoSet = true;
	}
	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _displayed_faces * 4 * 6 * sizeof(GLint), _vertices, GL_STATIC_DRAW);

	_vaoReset = true;
	_vaoVIP = false;

	glEnableVertexAttribArray(SPECATTRIB);
	glVertexAttribIPointer(SPECATTRIB, 1, GL_INT, 4 * sizeof(GLint), 0);

	// glEnableVertexAttribArray(BLOCKATTRIB);
	// glVertexAttribIPointer(BLOCKATTRIB, 1, GL_INT, 6 * sizeof(GLint), 0);
	
	// glEnableVertexAttribArray(BREAKATTRIB);
	// glVertexAttribIPointer(BREAKATTRIB, 1, GL_INT, 6 * sizeof(GLint), (void *)(sizeof(GLint)));
	
	// glEnableVertexAttribArray(ADJATTRIB);
	// glVertexAttribIPointer(ADJATTRIB, 1, GL_INT, 6 * sizeof(GLint), (void *)(2 * sizeof(GLint)));

	glEnableVertexAttribArray(POSATTRIB);
	glVertexAttribIPointer(POSATTRIB, 3, GL_INT, 4 * sizeof(GLint), (void *)(1 * sizeof(GLint)));

	check_glstate("NO");
}

void Chunk::setup_sky_array_buffer( void )
{
	if (_thread.joinable()) {
		_thread.join();
	}

	if (!_skyVaoSet) {
		glGenVertexArrays(1, &_skyVao);
		glGenBuffers(1, &_skyVbo);
		_skyVaoSet = true;
	}
	glBindVertexArray(_skyVao);

	glBindBuffer(GL_ARRAY_BUFFER, _skyVbo);
	_mtx_sky.lock();
	glBufferData(GL_ARRAY_BUFFER, _sky_count * 24 * sizeof(GLint), _sky_vert, GL_STATIC_DRAW);
	_mtx_sky.unlock();

	_skyVaoReset = false;
	_skyVaoVIP = false;

	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 4, GL_INT, 4 * sizeof(GLint), 0);

	check_glstate("NO");
}

void Chunk::setup_water_array_buffer( void )
{
	if (_thread.joinable()) {
		_thread.join();
	}

	if (!_waterVaoSet) {
		glGenVertexArrays(1, &_waterVao);
		glGenBuffers(1, &_waterVbo);
		_waterVaoSet = true;
	}
	glBindVertexArray(_waterVao);

	glBindBuffer(GL_ARRAY_BUFFER, _waterVbo);
	_mtx_fluid.lock();
	glBufferData(GL_ARRAY_BUFFER, _water_count * 24 * sizeof(GLint), _water_vert, GL_STATIC_DRAW);
	_mtx_fluid.unlock();

	_waterVaoReset = false;
	_waterVaoVIP = false;

	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 4, GL_INT, 4 * sizeof(GLint), 0);

	check_glstate("NO");
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

GLint Chunk::getStartX( void )
{
	return (_startX);	
}

GLint Chunk::getStartY( void )
{
	return (_startY);
}

void Chunk::setBackup( std::map<std::pair<int, int>, s_backup> *backups )
{
	if (_orientations.size() || _added.size() || _removed.size()) {
		mtx_backup.lock();
		(*backups)[std::pair<int, int>(_startX, _startY)] = {_orientations, _added, _removed, _furnaces};
		mtx_backup.unlock();
	}
}

void Chunk::restoreBackup( s_backup backup )
{
	_orientations = backup.orientations;
	_added = backup.added;
	_removed = backup.removed;
	_furnaces = backup.furnaces;
}

FurnaceInstance *Chunk::getFurnaceInstance( glm::ivec3 pos )
{
	int key = ((pos.x - _startX + 1) * (CHUNK_SIZE + 2) + pos.y - _startY + 1) * WORLD_HEIGHT + pos.z;
	std::map<int, FurnaceInstance>::iterator search = _furnaces.find(key);
	if (search != _furnaces.end()) {
		return (&search->second);
	}
	std::cout << "failed to find furnace at " << key << std::endl;
	std::cout << "furnaces values are " << std::endl;
	for (auto& fur: _furnaces) {
		std::cout << fur.first << std::endl;
	}
	return (NULL);
}

static void thread_setup_chunk( std::list<Chunk *> *chunks, Chunk *current )
{
	current->generation();

	mtx.lock();
	chunks->push_back(current);
	mtx.unlock();
}

void Chunk::generation( void )
{
	_blocks = new GLint[(CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * WORLD_HEIGHT];
	generate_blocks();
	_vertices = new GLint[_displayed_faces * 4 * 6]; // specifications, X Y Z
	// _vertices = new GLint[_displayed_blocks * 6]; // blocktype, break frame, adjacents blocks, X Y Z
	fill_vertex_array();
	_sky = new GLboolean[(CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)];
	generate_sky();
}

void Chunk::regeneration( Inventory *inventory, int type, glm::ivec3 pos, bool adding )
{
	if (!adding) {
		int value = _blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z];
		if (value == blocks::AIR || value == blocks::BEDROCK || value + blocks::NOTVISIBLE == blocks::BEDROCK) { // can't rm bedrock
			return ;
		}
		remove_block(inventory, pos);
	} else {
		int value = _blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z];
		if ((value != blocks::AIR && value < blocks::WATER) || type == blocks::AIR) { // can't replace block
			return ;
		}
		add_block(inventory, pos, type, value);
	}
	if (type == blocks::WATER || type == blocks::BUCKET) {
		// std::cout << "modif to water with " << ((type == blocks::WATER) ? "water" : "bucket") << std::endl;
		// std::cout << "water count before " << waterSave << ", after " << _water_count << std::endl;
		return ;
	}
	delete [] _vertices;
	_vertices = new GLint[_displayed_faces * 4 * 6];
	fill_vertex_array();
	_vaoReset = false;
	_mtx.lock();
	_vaoVIP = true;
	_mtx.unlock();
}

void Chunk::generate_chunk( std::list<Chunk *> *chunks )
{
	if (_thread.joinable()) {
		_thread.join();
	}
	_thread = std::thread(thread_setup_chunk, chunks, this);
}

void Chunk::sort_sky( glm::vec3 pos, bool vip )
{
	// std::cout << "in sort sky" << std::endl;
	if (!_sky_count) {
		return ;
	}
	// pos = glm::vec3(pos.x - _startX, pos.y - _startY, pos.z);
	std::vector<std::pair<float, std::array<int, 6>>> order;
	for (int row = 1; row < CHUNK_SIZE + 1; row++) {
		for (int col = 1; col < CHUNK_SIZE + 1; col++) {
			if (_sky[row * (CHUNK_SIZE + 2) + col]) {
				int pX = _startX + row - 1;
				int pY = _startY + col - 1;
				order.push_back(std::pair<float, std::array<int, 6>>(glm::distance(pos, glm::vec3(pX + 0.5f, pY + 0.5f, 156.0f)), {pX, pY + 1, 156, 1, -1, 0, }));
				order.push_back(std::pair<float, std::array<int, 6>>(glm::distance(pos, glm::vec3(pX + 0.5f, pY + 0.5f, 155.0f)), {pX, pY, 155, 1, 1, 0, }));
				if (!_sky[(row - 1) * (CHUNK_SIZE + 2) + col]) {
					order.push_back(std::pair<float, std::array<int, 6>>(glm::distance(pos, glm::vec3(pX, pY + 0.5f, 155.5f)), {pX, pY + 1, 156, 0, -1, -1, }));
				}
				if (!_sky[(row + 1) * (CHUNK_SIZE + 2) + col]) {
					order.push_back(std::pair<float, std::array<int, 6>>(glm::distance(pos, glm::vec3(pX + 1, pY + 0.5f, 155.5f)), {pX + 1, pY, 156, 0, 1, -1, }));
				}
				if (!_sky[row * (CHUNK_SIZE + 2) + col - 1]) {
					order.push_back(std::pair<float, std::array<int, 6>>(glm::distance(pos, glm::vec3(pX + 0.5f, pY, 155.5f)), {pX, pY, 156, 1, 0, -1, }));
				}
				if (!_sky[row * (CHUNK_SIZE + 2) + col + 1]) {
					order.push_back(std::pair<float, std::array<int, 6>>(glm::distance(pos, glm::vec3(pX + 0.5f, pY + 1, 155.5f)), {pX + 1, pY + 1, 156, -1, 0, -1, }));
				}
			}
		}
	}
	// std::cout << "before sort" << std::endl;
	// 	std::cout << "_sky_vert malloc " << _sky_count * 24 << std::endl;
	// 	std::cout << "order size " << order.size() << std::endl;
	
	if (order.size() != _sky_count) {
		std::cerr << "sort_sky, order size " << order.size() << ", but _sky_count " << _sky_count << std::endl;
		return ;
	}

	for (size_t index = 0; index < order.size() - 1; index++) {
		float minDist = order[index].first;
		size_t minIndex = index;
		for (size_t jindex = index + 1; jindex < order.size(); jindex++) {
			if (order[jindex].first > minDist) {
				minIndex = jindex;
				minDist = order[minIndex].first;
			}
		}
		if (minIndex != index) {
			std::pair<float, std::array<int, 6>> tmp = order[minIndex];
			order[minIndex] = order[index];
			order[index] = tmp;
		}
	}

	size_t vindex = 0;
	for (auto& o: order) {
		glm::ivec4 start = {o.second[0], o.second[1], o.second[2], 0}, offset0, offset1, offset2;
		if (!o.second[5]) {
			offset0 = {o.second[3], 0, 0, 0};
			offset1 = {0, o.second[4], 0, 0};
			offset2 = {o.second[3], o.second[4], 0, 0};
		} else {
			offset0 = {o.second[3], o.second[4], 0, 0};
			offset1 = {0, 0, o.second[5], 0};
			offset2 = {o.second[3], o.second[4], o.second[5], 0};
		}
		// std::cout << "vindex " << vindex << std::endl;
		_mtx_sky.lock();
		face_vertices(_sky_vert, start, start + offset0, start + offset1, start + offset2, vindex); // TODO mtx lock sky vert
		_mtx_sky.unlock();
	}
	order.clear();
	_skyVaoReset = true;
	_mtx.lock();
	if (vip) {
		_skyVaoVIP = true;
	}
	_mtx.unlock();
}

bool Chunk::inPerimeter( int posX, int posY, GLint render_dist )
{
	return (_startX >= posX - render_dist && _startX <= posX + render_dist
			&& _startY >= posY - render_dist && _startY <= posY + render_dist);
}

int Chunk::manhattanDist( int posX, int posY )
{
	int distX = (_startX > posX) ? _startX - posX : posX - _startX;
	int distY = (_startY > posY) ? _startY - posY : posY - _startY;
	return (distX + distY);
}

void Chunk::show( void )
{
	_isVisible = true;
}

void Chunk::hide( void )
{
	_isVisible = false;
}

bool Chunk::isInChunk( int posX, int posY )
{
	return (posX == _startX && posY == _startY);
}

int Chunk::isHit( glm::ivec3 pos, bool waterIsBlock )
{
	// std::cout << "current_chunk is " << _startX << ", " << _startY << std::endl;
	glm::ivec3 chunk_pos = glm::ivec3(pos.x - _startX, pos.y - _startY, pos.z);
	if (chunk_pos.z < 0 || chunk_pos.z > 255) {
		return (blocks::AIR);
	}
	if (chunk_pos.x < 0 || chunk_pos.x >= CHUNK_SIZE || chunk_pos.y < 0 || chunk_pos.y >= CHUNK_SIZE) {
		std::cout << "ERROR block out of chunk " << chunk_pos.x << ", " << chunk_pos.y << ", " << chunk_pos.z << std::endl;
		return (blocks::AIR);
	}
	if (_thread.joinable()) {
		_thread.join();
	}
	int value = _blocks[((chunk_pos.x + 1) * (CHUNK_SIZE + 2) + chunk_pos.y + 1) * WORLD_HEIGHT + chunk_pos.z];
	if (waterIsBlock) {
		return (value == blocks::WATER);
	}
	if (value >= blocks::WATER) {
		return (blocks::AIR);
	}
	return (value);
}

static void thread_modif_block( Chunk *current, Inventory *inventory, int type, glm::ivec3 pos, bool adding )
{
	current->regeneration(inventory, type, pos, adding);
}

void Chunk::handleHit( Inventory *inventory, int type, glm::ivec3 pos, bool adding )
{

	glm::ivec3 chunk_pos = glm::ivec3(pos.x - _startX, pos.y - _startY, pos.z);
	// std::cout << "handle hit at pos " << chunk_pos.x << ", " << chunk_pos.y << ", " << chunk_pos.z << std::endl;
	// if (chunk_pos.x < 0 || chunk_pos.x >= CHUNK_SIZE || chunk_pos.y < 0 || chunk_pos.y >= CHUNK_SIZE || chunk_pos.z < 0 || chunk_pos.z > 255) {
	// 	std::cout << "ERROR BLOCK OUT OF CHUNK " << chunk_pos.x << ", " << chunk_pos.y << ", " << chunk_pos.z << std::endl;
	// }
	if (_thread.joinable()) {
		_thread.join();
	}
	_thread = std::thread(thread_modif_block, this, inventory, type, chunk_pos, adding);
}

void Chunk::updateBreak( glm::ivec4 block_hit, int frame )
{
	if (block_hit.w == blocks::AIR) {
		return ;
	}
	glm::ivec3 chunk_pos = glm::ivec3(block_hit.x - _startX, block_hit.y - _startY, block_hit.z);
	if (chunk_pos.x < 0 || chunk_pos.x >= CHUNK_SIZE || chunk_pos.y < 0 || chunk_pos.y >= CHUNK_SIZE || chunk_pos.z < 0 || chunk_pos.z > 255) {
		std::cout << "ERROR block hit out of chunk " << chunk_pos.x << ", " << chunk_pos.y << ", " << chunk_pos.z << std::endl;
		return ;
	}
	if (_thread.joinable()) {
		_thread.join();
	}
	glm::ivec3 p0 = {_startX + chunk_pos.x + 0, _startY + chunk_pos.y + 0, chunk_pos.z + 1};
	glm::ivec3 p1 = {_startX + chunk_pos.x + 1, _startY + chunk_pos.y + 0, chunk_pos.z + 1};
	glm::ivec3 p2 = {_startX + chunk_pos.x + 0, _startY + chunk_pos.y + 0, chunk_pos.z + 0};
	glm::ivec3 p3 = {_startX + chunk_pos.x + 1, _startY + chunk_pos.y + 0, chunk_pos.z + 0};

	glm::ivec3 p4 = {_startX + chunk_pos.x + 0, _startY + chunk_pos.y + 1, chunk_pos.z + 1};
	glm::ivec3 p5 = {_startX + chunk_pos.x + 1, _startY + chunk_pos.y + 1, chunk_pos.z + 1};
	glm::ivec3 p6 = {_startX + chunk_pos.x + 0, _startY + chunk_pos.y + 1, chunk_pos.z + 0};
	glm::ivec3 p7 = {_startX + chunk_pos.x + 1, _startY + chunk_pos.y + 1, chunk_pos.z + 0};
	int value = _blocks[((chunk_pos.x + 1) * (CHUNK_SIZE + 2) + chunk_pos.y + 1) * WORLD_HEIGHT + chunk_pos.z];
	if (value <= 0) {
		return ;
	}
	int count = face_count(value, chunk_pos.x + 1, chunk_pos.y + 1, chunk_pos.z, value != blocks::OAK_LEAVES);
	int cnt = 0;
	for (size_t index = 0; index < _displayed_faces * 4 * 6; index += 24) {
		_mtx.lock();
		if (blockFace(_vertices, p0, p1, p2, p3, p4, p5, p6, p7, index)) {
			_vaoVIP = true;
			_vertices[index] = (_vertices[index] & 0xFF0FFF) + (frame << 12);
			_vertices[index + 4] = (_vertices[index + 4] & 0xFF0FFF) + (frame << 12);
			_vertices[index + 8] = (_vertices[index + 8] & 0xFF0FFF) + ((frame + 1) << 12);
			_vertices[index + 12] = (_vertices[index + 12] & 0xFF0FFF) + (frame << 12);
			_vertices[index + 16] = (_vertices[index + 16] & 0xFF0FFF) + ((frame + 1) << 12);
			_vertices[index + 20] = (_vertices[index + 20] & 0xFF0FFF) + ((frame + 1) << 12);
			_mtx.unlock();
			_vaoReset = false;
			if (++cnt >= count) {
				return ;
			}
			_mtx.lock();
		}
		_mtx.unlock();
	}
	std::cout << "update break not found " << cnt << std::endl;
}

// called by neighbour chunk if block change at border
// posX and posY in [0; CHUNK_SIZE + 1] === _blocks compatible
void Chunk::update_border(int posX, int posY, int level, int type, bool adding)
{
	// std::cout << "got into update border of chunk " << _startX << ", " << _startY << std::endl;
	// std::cout << "args: " << posX << ", " << posY << ", " << level << ": " << adding << std::endl;
	if (!(!posX || !posY || posX == CHUNK_SIZE + 1 || posY == CHUNK_SIZE + 1)) {
		std::cout << "ERROR update_border not border block " << posX << ", " << posY << std::endl;
		return ;
	}
	if (_thread.joinable()) {
		_thread.join();
	}
	glm::ivec2 target;
	if (!posX) {
		target = glm::ivec2(1, posY);
	} else if (posX == CHUNK_SIZE + 1) {
		target = glm::ivec2(CHUNK_SIZE, posY);
	} else if (!posY) {
		target = glm::ivec2(posX, 1);
	} else if (posY == CHUNK_SIZE + 1) {
		target = glm::ivec2(posX, CHUNK_SIZE);
	}
	if (adding) {
		// std::cout << '[' << _startX << ", " << _startY << "] block at border " << posX << ", " << posY << ", " << level << ": " << s_blocks[type].name << std::endl;
		_blocks[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + level] = type;
		_removed.erase((posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + level);
		_added[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + level] = type;
		if (!air_flower(type, true, false) || !air_flower(_blocks[(target.x * (CHUNK_SIZE + 2) + target.y) * WORLD_HEIGHT + level], false, false)) {
			return ;
		}
		_mtx.lock();
		_displayed_faces--;
		_mtx.unlock();
		if (!exposed_block(target.x, target.y, level, true)) { // block no more visible
			_blocks[(target.x * (CHUNK_SIZE + 2) + target.y) * WORLD_HEIGHT + level] -= blocks::NOTVISIBLE;
		}
	} else {
		// std::cout << '[' << _startX << ", " << _startY << "] rm block at border " << posX << ", " << posY << ", " << level << ": " << s_blocks[type].name << std::endl;
		if (!exposed_block(target.x, target.y, level, true)) {
			_blocks[(target.x * (CHUNK_SIZE + 2) + target.y) * WORLD_HEIGHT + level] += blocks::NOTVISIBLE;
		}
		_blocks[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + level] = blocks::AIR;
		_added.erase((posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + level);
		_removed.insert((posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + level);
		if (_blocks[(target.x * (CHUNK_SIZE + 2) + target.y) * WORLD_HEIGHT + level] >= blocks::WATER) {
			_hasWater = true;
			_fluids.insert(target.x + (target.y << 8) + (level << 16));
		}
		_mtx.lock();
		_displayed_faces++;
		_mtx.unlock();
	}
	delete [] _vertices;
	_vertices = new GLint[_displayed_faces * 24];
	fill_vertex_array();
	_vaoReset = false;
	_mtx.lock();
	_vaoVIP = true;
	_mtx.unlock();
}

/* collisionBox takes feet position of object, dimension of its hitbox and returns wether object is inside block or not */
bool Chunk::collisionBox( glm::vec3 pos, float width, float height )
{
	// WATCHOUT if width > 0.5 problemos because we check block left and right but not middle
	glm::ivec3 top0 = glm::ivec3(glm::floor(pos.x - width - _startX), glm::floor(pos.y - width - _startY), glm::floor(pos.z + height));
	if (top0.x < -1 || top0.x > CHUNK_SIZE || top0.y < -1 || top0.y > CHUNK_SIZE || top0.z < 0 || top0.z > 255) {
		std::cout << "ERROR COLLISION BLOCK OUT OF CHUNK top0 " << top0.x << ", " << top0.y << ", " << top0.z << std::endl;
		return (true);
	}
	if (air_flower(_blocks[((top0.x + 1) * (CHUNK_SIZE + 2) + top0.y + 1) * WORLD_HEIGHT + top0.z], false, false)) {
		return (true);
	}
	glm::ivec3 top1 = glm::ivec3(glm::floor(pos.x + width - _startX), glm::floor(pos.y - width - _startY), glm::floor(pos.z + height));
	if (top1 != top0) {
		if (top1.x < -1 || top1.x > CHUNK_SIZE || top1.y < -1 || top1.y > CHUNK_SIZE || top1.z < 0 || top1.z > 255) {
			std::cout << "ERROR COLLISION BLOCK OUT OF CHUNK top1 " << top1.x << ", " << top1.y << ", " << top1.z << std::endl;
			return (true);
		}
		if (air_flower(_blocks[((top1.x + 1) * (CHUNK_SIZE + 2) + top1.y + 1) * WORLD_HEIGHT + top1.z], false, false)) {
			return (true);
		}
	}
	glm::ivec3 top2 = glm::ivec3(glm::floor(pos.x + width - _startX), glm::floor(pos.y + width - _startY), glm::floor(pos.z + height));
	if (top2 != top0) {
		if (top2.x < -1 || top2.x > CHUNK_SIZE || top2.y < -1 || top2.y > CHUNK_SIZE || top2.z < 0 || top2.z > 255) {
			std::cout << "ERROR COLLISION BLOCK OUT OF CHUNK top2 " << top2.x << ", " << top2.y << ", " << top2.z << std::endl;
			return (true);
		}
		if (air_flower(_blocks[((top2.x + 1) * (CHUNK_SIZE + 2) + top2.y + 1) * WORLD_HEIGHT + top2.z], false, false)) {
			return (true);
		}
	}
	glm::ivec3 top3 = glm::ivec3(glm::floor(pos.x - width - _startX), glm::floor(pos.y + width - _startY), glm::floor(pos.z + height));
	if (top3 != top0) {
		if (top3.x < -1 || top3.x > CHUNK_SIZE || top3.y < -1 || top3.y > CHUNK_SIZE || top3.z < 0 || top3.z > 255) {
			std::cout << "ERROR COLLISION BLOCK OUT OF CHUNK top3 " << top3.x << ", " << top3.y << ", " << top3.z << std::endl;
			return (true);
		}
		if (air_flower(_blocks[((top3.x + 1) * (CHUNK_SIZE + 2) + top3.y + 1) * WORLD_HEIGHT + top3.z], false, false)) {
			return (true);
		}
	}
	if (height > 1) {
		return (collisionBox(pos, width, height - 1));
	} else if (top0.z != glm::floor(pos.z)) {
		return (collisionBox(pos, width, 0));
	}
	return (false);
}

/* collisionBoxWater takes feet position of object, dimension of its hitbox and returns wether object is inside water or not */
bool Chunk::collisionBoxWater( glm::vec3 pos, float width, float height )
{
	// WATCHOUT if width > 0.5 problemos because we check block left and right but not middle
	glm::ivec3 top0 = glm::ivec3(glm::floor(pos.x - width - _startX), glm::floor(pos.y - width - _startY), glm::floor(pos.z + height));
	if (top0.x < -1 || top0.x > CHUNK_SIZE || top0.y < -1 || top0.y > CHUNK_SIZE || top0.z < 0 || top0.z > 255) {
		std::cout << "ERROR COLLISION WATER OUT OF CHUNK top0 " << top0.x << ", " << top0.y << ", " << top0.z << std::endl;
		return (true);
	}
	if (_blocks[((top0.x + 1) * (CHUNK_SIZE + 2) + top0.y + 1) * WORLD_HEIGHT + top0.z] >= blocks::WATER) {
		return (true);
	}
	glm::ivec3 top1 = glm::ivec3(glm::floor(pos.x + width - _startX), glm::floor(pos.y - width - _startY), glm::floor(pos.z + height));
	if (top1 != top0) {
		if (top1.x < -1 || top1.x > CHUNK_SIZE || top1.y < -1 || top1.y > CHUNK_SIZE || top1.z < 0 || top1.z > 255) {
			std::cout << "ERROR COLLISION WATER OUT OF CHUNK top1 " << top1.x << ", " << top1.y << ", " << top1.z << std::endl;
			return (true);
		}
		if (_blocks[((top1.x + 1) * (CHUNK_SIZE + 2) + top1.y + 1) * WORLD_HEIGHT + top1.z] >= blocks::WATER) {
			return (true);
		}
	}
	glm::ivec3 top2 = glm::ivec3(glm::floor(pos.x + width - _startX), glm::floor(pos.y + width - _startY), glm::floor(pos.z + height));
	if (top2 != top0) {
		if (top2.x < -1 || top2.x > CHUNK_SIZE || top2.y < -1 || top2.y > CHUNK_SIZE || top2.z < 0 || top2.z > 255) {
			std::cout << "ERROR COLLISION WATER OUT OF CHUNK top2 " << top2.x << ", " << top2.y << ", " << top2.z << std::endl;
			return (true);
		}
		if (_blocks[((top2.x + 1) * (CHUNK_SIZE + 2) + top2.y + 1) * WORLD_HEIGHT + top2.z] >= blocks::WATER) {
			return (true);
		}
	}
	glm::ivec3 top3 = glm::ivec3(glm::floor(pos.x - width - _startX), glm::floor(pos.y + width - _startY), glm::floor(pos.z + height));
	if (top3 != top0) {
		if (top3.x < -1 || top3.x > CHUNK_SIZE || top3.y < -1 || top3.y > CHUNK_SIZE || top3.z < 0 || top3.z > 255) {
			std::cout << "ERROR COLLISION WATER OUT OF CHUNK top3 " << top3.x << ", " << top3.y << ", " << top3.z << std::endl;
			return (true);
		}
		if (_blocks[((top3.x + 1) * (CHUNK_SIZE + 2) + top3.y + 1) * WORLD_HEIGHT + top3.z] >= blocks::WATER) {
			return (true);
		}
	}
	if (height > 1) {
		return (collisionBoxWater(pos, width, height - 1));
	} else if (top0.z != glm::floor(pos.z)) {
		return (collisionBoxWater(pos, width, 0));
	}
	return (false);
}

void Chunk::applyGravity( Camera *camera )
{
	float saved_posZ = camera->getPos().z;
	_camera->applyGravity();
	glm::vec3 pos = _camera->getPos();
	float distZ = saved_posZ - pos.z;
	if (distZ < 0) { // jumping
		camera->_touchGround = false;
		// std::cout << "DEBUG: " << std::to_string(camera->_position.z) << std::endl;
		for (float posZ = saved_posZ; posZ < pos.z; posZ++) {
			// std::cout << "testing with posZ " << posZ << std::endl;
			if (collisionBox(glm::vec3(pos.x, pos.y, posZ), 0.3f, 1.8f)) {
				camera->setPosZ(glm::floor(posZ) + 0.19f);
				// std::cout << "value z spam: " << std::to_string(camera->_position.z) << std::endl;
				camera->_fall_distance -= (camera->getPos().z - posZ);
				camera->_update = true;
				camera->_inJump = false;
				return ;
			}
		}
		if (collisionBox(glm::vec3(pos.x, pos.y, pos.z), 0.3f, 1.8f)) {
			camera->setPosZ(glm::floor(pos.z) + 0.19f);
			// std::cout << "value z spam: " << std::to_string(camera->_position.z) << std::endl;
			camera->_fall_distance -= 1;
			camera->_update = true;
			camera->_inJump = false;
			return ;
		}
	} else { // falling
		for (float posZ = saved_posZ; posZ - 1 > pos.z; posZ--) {
			if (collisionBox(glm::vec3(pos.x, pos.y, posZ), 0.3f, 1.8f)) {
				camera->setPosZ(glm::floor((posZ + 1)));
				camera->_fall_distance -= (camera->getPos().z - posZ);
				camera->touchGround();
				if (saved_posZ != camera->getPos().z) {
					camera->_update = true;
				}
				return ;
			}
		}
		if (collisionBox(glm::vec3(pos), 0.3f, 1.8f)) {
			camera->setPosZ(glm::floor((pos.z + 1)));
			camera->_fall_distance -= (camera->getPos().z - pos.z);
			camera->touchGround();
			if (saved_posZ != camera->getPos().z) {
				camera->_update = true;
			}
			return ;
		}
	}
	camera->_update = true;
	camera->_touchGround = false;
}

int Chunk::isLoaded( GLint &counter )
{
	_mtx.lock();
	if (!_vaoSet) {
		_mtx.unlock();
		++counter;
		if (counter % 50 <= 5) {
			setup_array_buffer();
		}
		return (0);
	}
	_mtx.unlock();
	return (1);
}

void Chunk::drawArray( GLint & counter, GLint &block_counter )
{
	if (!_vaoReset) { // TODO change vosReset logic (swap true and false)
		// std::cout << "chunk reset " << _startX << ", " << _startY << std::endl;
		++counter;
		if (!_vaoVIP && counter % 50 > 5) { // we don't load more than 5 new chunks per 50 new chunks per frame
			return ;
		}
		setup_array_buffer();
	}
    glBindVertexArray(_vao); // this is the costly operation, chunk_size up == fps down
	_mtx.lock();
	glDrawArrays(GL_TRIANGLES, 0, _displayed_faces * 6);
	block_counter += _displayed_faces;
	_mtx.unlock();
}

void Chunk::updateFurnaces( double currentTime )
{
	for (auto& fur: _furnaces) {
		fur.second.updateTimes(currentTime);
	}
}

void Chunk::drawSky( GLint & counter, GLint &triangle_counter )
{
	if (!_sky_count) {
		return ;
	}
	if (_skyVaoReset) {
		// std::cout << "chunk reset " << _startX << ", " << _startY << std::endl;
		++counter;
		if (!_skyVaoVIP && !_skyVaoSet && counter > 5) { // we don't load more than 5 new chunks per 50 new chunks per frame
			return ;
		} else if (_skyVaoVIP || counter < 6) {
			setup_sky_array_buffer();
		}
	}
    glBindVertexArray(_skyVao);
	_mtx.lock();
	glDrawArrays(GL_TRIANGLES, 0, _sky_count * 6); // 6 points/face
	// std::cout << "draw sky" << std::endl;
	triangle_counter += _sky_count * 2; // 2 triang/face
	_mtx.unlock();
}

void Chunk::drawWater( GLint & counter, GLint &triangle_counter )
{
	if (!_water_count) {
		return ;
	}
	if (_waterVaoReset) {
		// std::cout << "chunk reset " << _startX << ", " << _startY << std::endl;
		++counter;
		if (!_waterVaoVIP && !_waterVaoSet && counter > 5) { // we don't load more than 5 new chunks per 50 new chunks per frame
			return ;
		} else if (_waterVaoVIP || counter < 6) {
			setup_water_array_buffer();
		}
	}
    glBindVertexArray(_waterVao);
	_mtx.lock();
	glDrawArrays(GL_TRIANGLES, 0, _water_count * 6); // 6 points/face
	// std::cout << "draw water" << std::endl;
	triangle_counter += _water_count * 2; // 2 triang/face
	_mtx.unlock();
}

std::string Chunk::getAddsRmsString( void )
{
	std::string res = "\nADDED\t> " + std::to_string(_added.size()) + "\n";
	for (auto& add: _added) {
		res += "\t\t\t" + std::to_string(add.first) + ": " + std::to_string(add.second) + '\n';
	}
	res += "\nRMED\t> " + std::to_string(_removed.size()) + "\n";
	for (auto& rm: _removed) {
		res += "\t\t\t" + std::to_string(rm) + '\n';
	}
	return (res);
}
