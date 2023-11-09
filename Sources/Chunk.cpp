#include "vox.h"

Chunk::Chunk( Camera *camera, int posX, int posY, std::list<Chunk *> *chunks )
	: _isVisible(true), _vaoSet(false),
	_waterVaoSet(false), _waterVaoVIP(false),
	_skyVaoSet(false), _skyVaoVIP(false), _genDone(false), _light_update(false),
	_vaoReset(false), _vaoVIP(false), _waterVaoReset(false), _skyVaoReset(false), _sortedOnce(false),
	_startX(posX), _startY(posY), _nb_neighbours(0),
	_blocks(NULL), _water_vert(NULL), _sky_vert(NULL), _vertices(NULL), _lights(NULL),
	_sky(NULL), _hasWater(true), _displayed_faces(0), _water_count(0), _sky_count(0),
	_neighbours({NULL, NULL, NULL, NULL}), _camera(camera)
{
	int cnt = 0;
// std::cout << "new chunk at " << posX << ", " << posY << std::endl;
	for (auto c : *chunks) { // TODO load backups if needed. for trees
		if (c->isInChunk(_startX - CHUNK_SIZE, _startY)) {
			_neighbours[face_dir::MINUSX] = c;
			c->setNeighbour(this, face_dir::PLUSX);
			++cnt;
		} else if (c->isInChunk(_startX + CHUNK_SIZE, _startY)) {
			_neighbours[face_dir::PLUSX] = c;
			c->setNeighbour(this, face_dir::MINUSX);
			++cnt;
		} else if (c->isInChunk(_startX, _startY - CHUNK_SIZE)) {
			_neighbours[face_dir::MINUSY] = c;
			c->setNeighbour(this, face_dir::PLUSY);
			++cnt;
		} else if (c->isInChunk(_startX, _startY + CHUNK_SIZE)) {
			_neighbours[face_dir::PLUSY] = c;
			c->setNeighbour(this, face_dir::MINUSY);
			++cnt;
		}

		if (cnt == 4) { // will never happen
			return ;
		}
	}
}

Chunk::~Chunk( void )
{
	if (_thread.joinable()) {
		_thread.join();
	}

	if (_neighbours[face_dir::MINUSX]) {
		_neighbours[face_dir::MINUSX]->setNeighbour(NULL, face_dir::PLUSX);
		_neighbours[face_dir::MINUSX] = NULL;
	}
	if (_neighbours[face_dir::PLUSX]) {
		_neighbours[face_dir::PLUSX]->setNeighbour(NULL, face_dir::MINUSX);
		_neighbours[face_dir::PLUSX] = NULL;
	}
	if (_neighbours[face_dir::MINUSY]) {
		_neighbours[face_dir::MINUSY]->setNeighbour(NULL, face_dir::PLUSY);
		_neighbours[face_dir::MINUSY] = NULL;
	}
	if (_neighbours[face_dir::PLUSY]) {
		_neighbours[face_dir::PLUSY]->setNeighbour(NULL, face_dir::MINUSY);
		_neighbours[face_dir::PLUSY] = NULL;
	}

	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_skyVbo);
	glDeleteVertexArrays(1, &_skyVao);

	delete [] _blocks;
	delete [] _lights;
	delete [] static_cast<GLint*>(_vertices);
	delete [] _water_vert;
	delete [] _sky;
	delete [] _sky_vert;

	// std::cout << "chunk deleted " << _startX << ", " << _startY << std::endl;
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void Chunk::gen_ore_blob( int ore_type, int row, int col, int level, int & blob_size, int dir)
{
	if (row == -1 || col == -1 || !level || !blob_size
		|| row == CHUNK_SIZE || col == CHUNK_SIZE || level == WORLD_HEIGHT
		|| _blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] != blocks::STONE) {
		return ;
	}
	_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = ore_type;
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
	if (!type || type >= blocks::WATER) {
		std::cerr << "face_count ERROR counting " << s_blocks[type].name << std::endl;
		return (0);
	}
	if (type >= blocks::POPPY) {
		return (2 << (type == blocks::TORCH));
	}
	GLint res = !air_flower(getBlockAt(row - 1, col, level, true), true, false)
				+ !air_flower(getBlockAt(row + 1, col, level, true), isNotLeaves, false)
				+ !air_flower(getBlockAt(row, col - 1, level, true), true, false)
				+ !air_flower(getBlockAt(row, col + 1, level, true), isNotLeaves, false);
	switch (level) {
		case 0:
			res += !air_flower(_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level + 1], isNotLeaves, false);
			break ;
		case 255:
			res += !air_flower(_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level - 1], true, false);
			res += 1;
			break ;
		default:
			res += !air_flower(_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level - 1], true, false);
			res += !air_flower(_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level + 1], isNotLeaves, false);
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
			above = !air_flower(_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level + 1], isNotLeaves, false);
			break ;
		case 255:
			below = !air_flower(_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level - 1], isNotLeaves, false);
			above = true;
			break ;
		default:
			below = !air_flower(_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level - 1], isNotLeaves, false);
			above = !air_flower(_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level + 1], isNotLeaves, false);
	}
	return (below || above
		|| !air_flower(getBlockAt(row - 1, col, level, false), isNotLeaves, false)
		|| !air_flower(getBlockAt(row + 1, col, level, false), isNotLeaves, false)
		|| !air_flower(getBlockAt(row, col - 1, level, false), isNotLeaves, false)
		|| !air_flower(getBlockAt(row, col + 1, level, false), isNotLeaves, false));
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
			double undergound = perlin.noise3D_01((_startX + row) / 100.0f, (_startY + col) / 100.0f, level / 100.0f);
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
			trees.push_back({row, col, level});
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
	double continentalness = (perlin.octave2D_01((_startX + row) / 1000.0f, (_startY + col) / 1000.0f, 2) - 0.5) * 2 * 3.80;
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
		// double weirdness = (perlin.octave2D_01((_startX + row) / 200.0f, (_startY + col) / 200.0f, 4) - 0.5) * 2 * 2.97;
		// if (weirdness > -1.0 && weirdness < 1.0) {
			// double peaks_valleys = 1 - glm::abs(3 * glm::abs(weirdness) - 2);
			double peaks_valleys = (perlin.octave2D_01((_startX + row) / 100.0f, (_startY + col) / 100.0f, 4) - 0.5) * 2 * 2.97;
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
	// Bench b;
	const siv::PerlinNoise perlin{ perlin_seed };
	std::minstd_rand0  generator((_startX * 19516 + _startY * 56849) * perlin_seed);
	// std::minstd_rand0  generator(perlin.noise2D(_startX * _startX, _startY * _startY) * perlin_seed);
	std::uniform_int_distribution<int> distribution(0, 1000);

	std::vector<glm::ivec3> trees;

	// generating base terrain
	for (int row = 0; row < CHUNK_SIZE; row++) {
		for (int col = 0; col < CHUNK_SIZE; col++) {
			// int surface_level = glm::floor(SEA_LEVEL + (perlin.octave2D_01((_startX - 1 + row) / 100.0f, (_startY - 1 + col) / 100.0f, 4) - 0.5) * 100);
			int surface_level = surfaceLevel(row, col, perlin);
			int gen = distribution(generator);
			bool tree_gen = (gen <= 2 && row > 1 && row < CHUNK_SIZE - 2 && col > 1 && col < CHUNK_SIZE - 2);
			bool poppy = (gen <= 5);
			bool dandelion = (gen <= 8);
			bool blue_orchid = (gen <= 11);
			bool allium = (gen <= 14);
			bool cornflower = (gen <= 17);
			bool pink_tulip = (gen <= 20);
			bool grass = (gen <= 121);
			double pillar = perlin.noise2D_01((_startX + row) / 20.0f, (_startY + col) / 20.0f);
			int ground_cave = 7 + perlin.octave2D_01((_startX + row) / 100.0f, (_startY + col) / 100.0f, 4) * 14;
			for (int level = 0; level < WORLD_HEIGHT; level++) {
				// // double cheese = perlin.octave3D_01((_startX + row) / 100.0f, (_startY + col) / 100.0f, (level) / 100.0f, 4);
				// // double bridge = perlin.noise2D_01((_startX + row) / 60.0f, (level) / 15.0f);
				// // double obridge = perlin.noise2D_01((_startY + col) / 60.0f, (level) / 15.0f);
				// // (level < surface_level - 5 && ((cave >= 0.459 && cave <= 0.551f) && !(pillar < 0.3f - 0.7f * (surface_level / 2 - glm::abs(level - surface_level / 2)) / (5.0f * surface_level)) && !(bridge < 0.2f) && !(obridge < 0.2f)) && level > ground_cave)
				// // (level < surface_level - 5 && cave <= 0.2f && level > 0) //  * (1 - (0.5 + glm::abs(level - surface_level / 2) / surface_level))
				double cave = -1;
				if (level < surface_level - 5 && level > ground_cave) {
					#if 1
					cave = perlin.octave3D_01((_startX + row) / 100.0f, (_startY + col) / 100.0f, (level) / 100.0f, 4);
					#else
					cave = perlin.octave3D_01((_startX + row) / 750.0f, (_startY + col) / 750.0f, (level) / 750.0f, 4); // big holes
					#endif
				}
				(((cave >= 0.459 && cave <= 0.551f) && !(pillar < 0.3f - 0.7f * (surface_level / 2 - glm::abs(level - surface_level / 2)) / (5.0f * surface_level))))
					? _blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = blocks::AIR//get_block_type_cave(row, col, level, ground_cave, poppy, dandelion, blue_orchid, allium, cornflower, pink_tulip, grass, tree_gen, trees)
					: _blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = get_block_type(perlin, row, col, level, surface_level, poppy, dandelion, blue_orchid, allium, cornflower, pink_tulip, grass, tree_gen, trees);
				// _blocks[(row * CHUNK_SIZE + col) * WORLD_HEIGHT + level] = get_block_type(perlin, row, col, level, surface_level, poppy, dandelion, blue_orchid, allium, cornflower, pink_tulip, grass, tree_gen, trees);
				// GLfloat squashing_factor;
				// (level < 64)
				// 	? squashing_factor = (64 - level) / 64.0f
				// 	: squashing_factor = -(level - 64) / 192.0f;
				// // // std::cout << "squash at " << level << ": " << squashing_factor << std::endl;
				// _blocks[(row * CHUNK_SIZE + col) * WORLD_HEIGHT + level] = ((perlin.octave3D_01((_startX - 1 + row) / 100.0f, (_startY - 1 + col) / 100.0f, level / 20.0f, 4) + squashing_factor) > 0.5f);
				// if (row == 9 && col == 9 && level == surface_level + 5) {
				// 	_blocks[(row * CHUNK_SIZE + col) * WORLD_HEIGHT + level] = 1;
				// }
			}
		}
	}
	// b.stamp("basic");
	// adding leaves to trees
	for (auto& tree: trees) { // TODO put trees on chunk border and communicate with border chunks
		for (int index = 0; index < 61; index++) {
			const GLint delta[3] = {oak_normal[index][0], oak_normal[index][1], oak_normal[index][2]};
			if (_blocks[((((tree.x + delta[0]) << CHUNK_SHIFT) + tree.y + delta[1]) << WORLD_SHIFT) + tree.z + delta[2]] == blocks::AIR) {
				_blocks[((((tree.x + delta[0]) << CHUNK_SHIFT) + tree.y + delta[1]) << WORLD_SHIFT) + tree.z + delta[2]] = blocks::OAK_LEAVES;
			}
		}
	}
	trees.clear();

	// generating ores
	bool gravel_blob = false;
	int coal_blobs = 0, iron_blobs = 0, diamond_blobs = 0;
	for (int row = 0; row < CHUNK_SIZE; row++) {
		for (int col = 0; col < CHUNK_SIZE; col++) {
			for (int level = 0; level < WORLD_HEIGHT; level++) {
				GLint value = _blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level];
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
					int block_above = _blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level + 1];
					if (block_above && block_above < blocks::POPPY) {
						_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = blocks::DIRT;
					}
				}
			}
		}
	}

	// reverting ores back to stone if they are exposed
	for (int row = 0; row < CHUNK_SIZE; row++) {
		for (int col = 0; col < CHUNK_SIZE; col++) {
			for (int level = 0; level < WORLD_HEIGHT; level++) {
				GLint value = _blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level];
				if (value) {
					if (value == blocks::WATER) {
						// do nothing
					} else if (exposed_block(row, col, level, value != blocks::OAK_LEAVES)) {
						if (value == blocks::IRON_ORE && distribution(generator) < 500) {
							_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = blocks::STONE;
						} else if (value == blocks::GRAVEL && distribution(generator) < 100) {
							_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = blocks::STONE;
						} else if (value == blocks::DIAMOND_ORE && distribution(generator) < 900) {
							_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = blocks::STONE;
						} else if (value == blocks::COAL_ORE && distribution(generator) < 200) {
							_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = blocks::STONE;
						} 
					}
				}
			}
		}
	}

	// loading backup, MUST be done after all randomness to not alter anything
	for (auto r: _removed) {
		_blocks[r] = blocks::AIR;
	}
	for (auto a: _added) {
		_blocks[a.first] = a.second;
	}
	// b.stamp("rest");
	// b.stop("blocks");
}

void Chunk::resetDisplayedFaces( void )
{
	_displayed_faces = 0;
	// counting displayed blocks and hiding unseen blocks
	for (int row = 0; row < CHUNK_SIZE; row++) {
		for (int col = 0; col < CHUNK_SIZE; col++) {
			for (int level = 0; level < WORLD_HEIGHT; level++) {
				int offset = (((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level;
				GLint value = _blocks[offset];
				bool restore = false;
				if (value < blocks::AIR && (!row || !col || row == CHUNK_SIZE - 1 || col == CHUNK_SIZE - 1)) {
					value += blocks::NOTVISIBLE;
					restore = true;
				}
				if (value > blocks::AIR && value < blocks::WATER) {
					GLint below = ((level) ? _blocks[offset - 1] : 0);
					if (!air_flower(value, false, true) && value != blocks::TORCH && below != blocks::GRASS_BLOCK && below != blocks::DIRT && below != blocks::SAND) {
						_blocks[offset] = blocks::AIR;
					} else {
						GLint count = face_count(value, row, col, level, value != blocks::OAK_LEAVES);
						if (count) {
							// std::cout << "count is " << count << std::endl;
							_displayed_faces += count;
							if (restore) {
								_blocks[offset] = value;
							}
						} else if (value > blocks::AIR) { // hide block
							_blocks[offset] = value - blocks::NOTVISIBLE;
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
}

int Chunk::sand_fall_endz( glm::ivec3 pos )
{
	for (int level = pos.z - 1; level > 0; level--) {
		int value = _blocks[(((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + level];
		if (value != blocks::AIR && value < blocks::WATER) {
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
	if (!pos.x && _neighbours[face_dir::MINUSX]) {
		_neighbours[face_dir::MINUSX]->update_border(CHUNK_SIZE - 1, pos.y, pos.z, type, adding);
	} else if (pos.x == CHUNK_SIZE - 1 && _neighbours[face_dir::PLUSX]) {
		_neighbours[face_dir::PLUSX]->update_border(0, pos.y, pos.z, type, adding);
	}
	if (!pos.y && _neighbours[face_dir::MINUSY]) {
		_neighbours[face_dir::MINUSY]->update_border(pos.x, CHUNK_SIZE - 1, pos.z, type, adding);
	} else if (pos.y == CHUNK_SIZE - 1 && _neighbours[face_dir::PLUSY]) {
		_neighbours[face_dir::PLUSY]->update_border(pos.x, 0, pos.z, type, adding);
	}
}

void Chunk::remove_block( Inventory *inventory, glm::ivec3 pos )
{
	// std::cout << "in chunk " << _startX << ", " << _startY << ":rm block " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
	// std::cout << "nb displayed blocks before: " << _displayed_blocks << std::endl;
	int offset = (((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z;
	int value = _blocks[offset]; // TODO better handling of invis block gets deleted
	if (value >= blocks::CRAFTING_TABLE && value < blocks::BEDROCK) { // oriented blocks
		_orientations.erase(offset);
	}
	_added.erase(offset);
	_removed.insert(offset);
	if (value == blocks::FURNACE) {
		auto search = _furnaces.find(offset); // drop furnace's items
		if (search != _furnaces.end()) {
			glm::ivec2 items = search->second.getComposant();
			if (items.y) {
				_entities.push_back(Entity(this, inventory, {pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f}, glm::normalize(glm::vec2(-1, -1)), false, items.x, items.y));
			}
			items = search->second.getFuel();
			if (items.y) {
				_entities.push_back(Entity(this, inventory, {pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f}, glm::normalize(glm::vec2(1, 9)), false, items.x, items.y));
			}
			items = search->second.getProduction();
			if (items.y) {
				_entities.push_back(Entity(this, inventory, {pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f}, glm::normalize(glm::vec2(-4, 5)), false, items.x, items.y));
			}
		}
		_furnaces.erase(offset);
	}
	mtx_inventory.lock();
	if (inventory) {
		(value == blocks::WATER)
			? inventory->addBlock(value)
			: _entities.push_back(Entity(this, inventory, {pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f}, glm::normalize(glm::vec2(pos.x - 8, pos.y - 8)), false, s_blocks[value + (value < 0) * blocks::NOTVISIBLE].mined));
	}
	mtx_inventory.unlock();
	int endZ = -1;
	int block_above = _blocks[offset + 1];
	if (pos.z < 255 && isSandOrGravel(block_above)) { // sand falls if block underneath deleted
		_blocks[offset] = blocks::AIR;
		endZ = sand_fall_endz(pos);
		int above_value = (block_above == blocks::SAND || block_above + blocks::NOTVISIBLE == blocks::SAND) ? blocks::SAND : blocks::GRAVEL;
		if (endZ == pos.z) {
			_blocks[offset] = above_value - (value < 0) * blocks::NOTVISIBLE;
			return (remove_block(NULL, {pos.x, pos.y, pos.z + 1}));
		}
		_blocks[offset] = value; // putting it back temporarily to avoid segfault in neighbour on addblock
		add_block(NULL, {pos.x, pos.y, endZ}, above_value, blocks::AIR);
		_blocks[offset] = blocks::AIR;
	}
	_blocks[offset] = blocks::AIR;
	if (value > blocks::AIR && value < blocks::WATER) { // if invisible block gets deleted, same amount of displayed_blocks
		_displayed_faces -= face_count(value, pos.x, pos.y, pos.z, value != blocks::OAK_LEAVES);
		if (value == blocks::TORCH) {
			std::cout << "rm light" << std::endl;
			_lights[offset] &= 0xFF00;
			light_spread(pos.x, pos.y, pos.z, false); // spread block light
			std::cout << "over" << std::endl;
		}
	} else if (value == blocks::WATER) { // use bucket on water source
		// std::cout << "bucket on water" << std::endl;
		_fluids.insert(offset);
		return ;
	}
	if (air_flower(value, false, true)) {
		light_spread(pos.x, pos.y, pos.z, true); // spread sky light
		light_spread(pos.x, pos.y, pos.z, false); // spread block light
		for (int index = 0; index < 6; index++) {
			const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
			if (pos.x + delta[0] < 0 || pos.x + delta[0] >= CHUNK_SIZE || pos.y + delta[1] < 0 || pos.y + delta[1] >= CHUNK_SIZE || pos.z + delta[2] < 0 || pos.z + delta[2] > 255) {

			} else {
				int adj_offset = ((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2];
				int adj = _blocks[adj_offset];
				if (adj < blocks::AIR) {
					_blocks[adj_offset] += blocks::NOTVISIBLE;
					_displayed_faces++;
				} else if (air_flower(adj, false, false)) {
					_displayed_faces++;
				} else if (index != face_dir::MINUSZ && adj >= blocks::WATER) { // if water under, it is not updated
					_fluids.insert(adj_offset);
				}
			}
		}
	}
	handle_border_block(pos, air_flower(value, true, true), false); // if block at border of chunk gets deleted, we update neighbours. doing it after light spread
	if (endZ != -1) { // sand fall
		remove_block(NULL, {pos.x, pos.y, pos.z + 1});
	} else if (pos.z < 255 && ((block_above >= blocks::POPPY && block_above < blocks::WATER) || block_above == blocks::CACTUS)) { // del flower if block underneath deleted
		(block_above == blocks::GRASS)
			? remove_block(NULL, {pos.x, pos.y, pos.z + 1}) // if grass above breaked block, don't collect it
			: remove_block(inventory, {pos.x, pos.y, pos.z + 1});
	}
	// std::cout << "nb displayed blocks after: " << _displayed_blocks << std::endl;
}

void Chunk::add_block( Inventory *inventory, glm::ivec3 pos, int type, int previous )
{
	// std::cout << "in chunk " << _startX << ", " << _startY << ":add block " << _startX + pos.x << ", " << _startY + pos.y << ", " << pos.z << std::endl;
	// std::cout << "nb displayed blocks before: " << _displayed_blocks << std::endl;
	if (type == blocks::SAND || type == blocks::GRAVEL) {
		pos.z = sand_fall_endz(pos);
		previous = _blocks[(((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z];
		// if (previous == blocks::TORCH) { // TODO implement sand/gravel breaking into entity
		// 	return ;
		// }
	}
	int offset = (((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z;
	if (type == blocks::WATER) { // we place water
		if (previous < blocks::WATER) {
			_hasWater = true;
		}
	} else if (type >= blocks::POPPY && pos.z > 0) {
		if (previous >= blocks::WATER) {
			return ;
		}
		int block_below = _blocks[offset - 1];
		if (type != blocks::TORCH && block_below != blocks::GRASS_BLOCK && block_below != blocks::DIRT && block_below != blocks::SAND) {
			return ; // can't place flower on something else than grass/dirt block
		} else if (!(block_below > blocks::AIR && block_below < blocks::POPPY)) {
			return ;
		}
	} else if (previous >= blocks::WATER) { // replace water block with something else
		_fluids.insert(offset);
	}
	mtx_inventory.lock();
	if (inventory) {
		inventory->removeBlock(false);
	}
	mtx_inventory.unlock();
	if (type >= blocks::CRAFTING_TABLE && type < blocks::BEDROCK) { // oriented blocks
		_orientations[offset] = _camera->getOrientation() + (furnace_state::OFF << 4);
	}
	_blocks[offset] = type;
	_removed.erase(offset);
	_added[offset] = type;
	if (type == blocks::WATER) {
		_fluids.insert(offset);
		// std::cout << "adding water" << std::endl;
		return ;
	}
	if (type == blocks::FURNACE) {
		_furnaces[offset] = FurnaceInstance();
	} else if (type == blocks::TORCH) {
		std::cout << "add light" << std::endl;
		_lights[offset] &= 0xFF00;
		_lights[offset] += s_blocks[type].light_level + (s_blocks[type].light_level << 4);
		light_spread(pos.x, pos.y, pos.z, false);
		handle_border_block(pos, type, true); // torch is special case flower
		_light_update = false;
		std::cout << "over" << std::endl;
	}
	_displayed_faces += face_count(type, pos.x, pos.y, pos.z, type != blocks::OAK_LEAVES);
	if (!air_flower(type, true, false)) {
		return ;
	}
	_lights[offset] = 0; // rm light if solid block added
	for (int index = 0; index < 6; index++) {
		const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
		if (pos.x + delta[0] < 0 || pos.x + delta[0] >= CHUNK_SIZE || pos.y + delta[1] < 0 || pos.y + delta[1] >= CHUNK_SIZE || pos.z + delta[2] < 0 || pos.z + delta[2] > 255) {

		} else {
			GLint value = _blocks[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]];
			if (air_flower(value, true, false)) {
				_displayed_faces--;
			} else {
				if (index != face_dir::PLUSZ) {
					light_try_spread(pos.x + delta[0], pos.y + delta[1], pos.z + delta[2], 0, true); // spread sky light, but not upwards duh
				}
				light_try_spread(pos.x + delta[0], pos.y + delta[1], pos.z + delta[2], 0, false);
			}
			if (value > blocks::AIR && value < blocks::WATER && !face_count(value, pos.x + delta[0], pos.y + delta[1], pos.z + delta[2], value != blocks::OAK_LEAVES)) {
				// was exposed before, but isn't anymore
				_blocks[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] -= blocks::NOTVISIBLE;
			}
		}
	}
	handle_border_block(pos, air_flower(type, false, true), true); // if block at border of chunk gets added, we update neighbours, after light spread
		// std::cout << "nb displayed blocks after: " << _displayed_blocks << std::endl;
}

// works as long as sizeof(int) == sizeof(float)
// we output 1 int and 3 floats to shader
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
	_mtx.lock();
	glBufferData(GL_ARRAY_BUFFER, _displayed_faces * 4 * 6 * sizeof(GLint), _vertices, GL_STATIC_DRAW);
	_mtx.unlock();

	_vaoReset = true;
	_vaoVIP = false;

	glEnableVertexAttribArray(SPECATTRIB);
	glVertexAttribIPointer(SPECATTRIB, 1, GL_INT, 4 * sizeof(GLint), 0);

	glEnableVertexAttribArray(POSATTRIB);
	glVertexAttribPointer(POSATTRIB, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(1 * sizeof(GLint)));

	check_glstate("Chunk::setup_array_buffer", false);
}

void Chunk::setup_sky_array_buffer( void )
{
	if (_thread.joinable()) {
		_thread.join();
	}

	// check_glstate("BEFORE Chunk::setup_sky_array_buffer", false);
	if (!_skyVaoSet) {
		glGenVertexArrays(1, &_skyVao);
		glGenBuffers(1, &_skyVbo);
		_skyVaoSet = true;
		// check_glstate("GEN Chunk::setup_sky_array_buffer", false);
	// glBindVertexArray(_skyVao);

	// glBindBuffer(GL_ARRAY_BUFFER, _skyVbo);
	// check_glstate("BIND GEN Chunk::setup_sky_array_buffer", false);
	}//else{
	glBindVertexArray(_skyVao);

	glBindBuffer(GL_ARRAY_BUFFER, _skyVbo);
	// check_glstate("BIND Chunk::setup_sky_array_buffer", false);}
	_mtx_sky.lock();
	glBufferData(GL_ARRAY_BUFFER, _sky_count * 24 * sizeof(GLint), _sky_vert, GL_STATIC_DRAW);
	_mtx_sky.unlock();

	// check_glstate("BUFFERDATA Chunk::setup_sky_array_buffer", false);
	_skyVaoReset = false;
	_skyVaoVIP = false;

	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 4, GL_INT, 4 * sizeof(GLint), 0);

	check_glstate("Chunk::setup_sky_array_buffer", false);
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

	check_glstate("Chunk::setup_water_array_buffer", false);
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

bool Chunk::getSortedOnce( void )
{
	return (_sortedOnce);
}

void Chunk::waitGenDone( void )
{
	while (!_genDone);
}

void Chunk::setNeighbour( Chunk *neighbour, face_dir index )
{
	if (_neighbours[index] && neighbour) {
		std::cerr << "setNeighbour ERROR DUPLICATE " << neighbour->getStartX() << ", " << neighbour->getStartY() << " vs " << _neighbours[index]->getStartX() << ", " << _neighbours[index]->getStartY() << std::endl;
		if (_neighbours[index] == neighbour) {
			std::cerr << "but they are the same.." << std::endl;
		}
	}
	_neighbours[index] = neighbour;
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
	int key = ((((pos.x - _startX) << CHUNK_SHIFT) + pos.y - _startY) << WORLD_SHIFT) + pos.z;
	std::map<int, FurnaceInstance>::iterator search = _furnaces.find(key);
	if (search != _furnaces.end()) {
		return (&search->second);
	}
	std::cout << "failed to find furnace at " << key << " from " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
	std::cout << "furnaces values are " << std::endl;
	for (auto& fur: _furnaces) {
		std::cout << fur.first << std::endl;
	}
	return (NULL);
}

GLint Chunk::getBlockAt( int posX, int posY, int posZ, bool askNeighbours )
{
	if (!_blocks || posZ < 0) {
		return (blocks::BEDROCK);
	} else if (posZ >= WORLD_HEIGHT) {
		return (blocks::AIR);
	}
	int res = blocks::BEDROCK;
	if (posX < 0) {
		if (askNeighbours && _neighbours[face_dir::MINUSX]) {
			res = _neighbours[face_dir::MINUSX]->getBlockAt(posX + CHUNK_SIZE, posY, posZ, askNeighbours);
		}
	} else if (posX >= CHUNK_SIZE) {
		if (askNeighbours && _neighbours[face_dir::PLUSX]) {
			res = _neighbours[face_dir::PLUSX]->getBlockAt(posX - CHUNK_SIZE, posY, posZ, askNeighbours);
		}
	} else if (posY < 0) {
		if (askNeighbours && _neighbours[face_dir::MINUSY]) {
			res = _neighbours[face_dir::MINUSY]->getBlockAt(posX, posY + CHUNK_SIZE, posZ, askNeighbours);
		}
	} else if (posY >= CHUNK_SIZE) {
		if (askNeighbours && _neighbours[face_dir::PLUSY]) {
			res = _neighbours[face_dir::PLUSY]->getBlockAt(posX, posY - CHUNK_SIZE, posZ, askNeighbours);
		}
	} else {
		res = _blocks[(((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + posZ];
	}
	return (res);
}

static void thread_setup_chunk( Chunk *current )
{
	current->generation();
}

void Chunk::generation( void )
{
	// Bench b;
	_blocks = new GLint[ALLOC_SIZE];
	generate_blocks();
	// b.stamp("blocks");
	_lights = new short[ALLOC_SIZE];
	generate_lights();
	// b.stamp("lights");
	// _mtx.lock();
	// _vertices = new GLint[_displayed_faces * 4 * 6]; // specifications, X Y Z
	// _mtx.unlock();
	_genDone = true;
	// b.stamp("vertcices");
	_mtx_sky.lock();
	_sky = new GLboolean[(CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)];
	generate_sky();
	if (_sky_count) {
		_sky_vert = new GLint[_sky_count * 24]; // will be filled in later with a call to sort_sky
	}
	_mtx_sky.unlock();
	// b.stamp("sky");
	// b.stop("generation");
}

// fill_vertices uses light emited by neighbours, so had to make sure they are loaded before we fill our own vertices in
void Chunk::checkFillVertices( void )
{
	int cnt = 0;
	for (int i = 0; i < 4; i++) {
		if (_neighbours[i]) {
			_neighbours[i]->waitGenDone();
			++cnt;
		}
	}
	if (cnt > _nb_neighbours) {
		_nb_neighbours = cnt;
		if (cnt == 4) {
			for (auto a: _added) { // update torches == block_light spreading, we do it here because before neighbours might not be ready to accept spread
				if (a.second == blocks::TORCH) {
					int level = a.first & (WORLD_HEIGHT - 1);
					int col = ((a.first >> WORLD_SHIFT) & (CHUNK_SIZE - 1));
					int row = ((a.first >> WORLD_SHIFT) >> CHUNK_SHIFT);
					_lights[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] &= 0xFF00; // discard previous light value TODO change this if different light source level implemented
					_lights[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] += 14 + (14 << 4); // 0xF0 = light source. & 0xF = light level
					light_spread(row, col, level, false);
				}
			}
			// this time spread sky_light underground
			for (int row = 0; row < CHUNK_SIZE; row++) {
				for (int col = 0; col < CHUNK_SIZE; col++) {
					for (int level = WORLD_HEIGHT - 1; level > 0; level--) {
						if (!(_lights[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] & 0xFF00)) {
							int value = _blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level];
							if (!air_flower(value, true, false)) { // underground hole
								// spread_light TODO watch out for leaves and water light damping..
								light_spread(row, col, level, true);
							}
						}
					}
				}
			}
		}
		resetDisplayedFaces();
		// std::cout << "chunk " << _startX << ", " << _startY << ": " << _displayed_faces << std::endl;
		_mtx.lock();
		delete [] static_cast<GLint*>(_vertices);
		_vertices = new GLint[_displayed_faces * 4 * 6];
		_mtx.unlock();
		fill_vertex_array();
		_vaoVIP = false;
	} else if (cnt < _nb_neighbours) {
		_nb_neighbours = cnt;
	} else if (!cnt) {
		std::cerr << "ERROR chunk has no neighbours" << std::endl;
	}
}

void Chunk::regeneration( Inventory *inventory, int type, glm::ivec3 pos, bool adding )
{
	if (!adding) {
		int value = _blocks[(((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z];
		if (value == blocks::AIR || value == blocks::BEDROCK || value + blocks::NOTVISIBLE == blocks::BEDROCK) { // can't rm bedrock
			return ;
		}
		remove_block(inventory, pos);
	} else {
		int value = _blocks[(((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z];
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
	delete [] static_cast<GLint*>(_vertices);
	_vertices = new GLint[_displayed_faces * 4 * 6];
	if (!_light_update) {
		fill_vertex_array();
	}
}

void Chunk::generate_chunk( void )
{
	#if 1
	if (_thread.joinable()) {
		_thread.join();
	}
	_thread = std::thread(thread_setup_chunk, this);
	#else
	thread_setup_chunk(this);
	#endif
}

void Chunk::addEntity( Inventory *inventory, glm::vec3 dir, int value, int amount, int dura )
{
	glm::vec3 camPos = _camera->getPos();
	camPos.z += 1;
	camPos += dir;
	_entities.push_back(Entity(this, inventory, camPos, dir, true, value, amount, dura));
}

void Chunk::sort_sky( glm::vec3 pos, bool vip )
{
	// std::cout << "in sort sky" << std::endl;
	if (!_sky_count) {
		return ;
	}
	// pos = glm::vec3(pos.x - _startX, pos.y - _startY, pos.z);
	_mtx_sky.lock();
	std::vector<std::pair<float, std::array<int, 6>>> order;
	order.reserve(_sky_count);
	for (int row = 1; row < CHUNK_SIZE + 1; row++) {
		for (int col = 1; col < CHUNK_SIZE + 1; col++) {
			if (_sky[row * (CHUNK_SIZE + 2) + col]) {
				int pX = _startX + row - 1;
				int pY = _startY + col - 1;
				order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY + 0.5f, 156.0f)), {pX, pY + 1, 156, 1, -1, 0, }});
				order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY + 0.5f, 155.0f)), {pX, pY, 155, 1, 1, 0, }});
				if (!_sky[(row - 1) * (CHUNK_SIZE + 2) + col]) {
					order.push_back({dist2(pos, glm::vec3(pX, pY + 0.5f, 155.5f)), {pX, pY + 1, 156, 0, -1, -1, }});
				}
				if (!_sky[(row + 1) * (CHUNK_SIZE + 2) + col]) {
					order.push_back({dist2(pos, glm::vec3(pX + 1, pY + 0.5f, 155.5f)), {pX + 1, pY, 156, 0, 1, -1, }});
				}
				if (!_sky[row * (CHUNK_SIZE + 2) + col - 1]) {
					order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY, 155.5f)), {pX, pY, 156, 1, 0, -1, }});
				}
				if (!_sky[row * (CHUNK_SIZE + 2) + col + 1]) {
					order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY + 1, 155.5f)), {pX + 1, pY + 1, 156, -1, 0, -1, }});
				}
			}
		}
	}
	_mtx_sky.unlock();
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
		face_water_vertices(_sky_vert, start, start + offset0, start + offset1, start + offset2, vindex);
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
	while (!_genDone); // wait for generation taking place in another thread to be done
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
	int value = _blocks[(((chunk_pos.x << CHUNK_SHIFT) + chunk_pos.y) << WORLD_SHIFT) + chunk_pos.z];
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
	int value = _blocks[(((chunk_pos.x << CHUNK_SHIFT) + chunk_pos.y) << WORLD_SHIFT) + chunk_pos.z];
	if (value <= 0) {
		return ;
	}
	glm::ivec3 p0 = {_startX + chunk_pos.x + 0, _startY + chunk_pos.y + 0, chunk_pos.z + 1};
	glm::ivec3 p1 = {_startX + chunk_pos.x + 1, _startY + chunk_pos.y + 0, chunk_pos.z + 1};
	glm::ivec3 p2 = {_startX + chunk_pos.x + 0, _startY + chunk_pos.y + 0, chunk_pos.z + 0};
	glm::ivec3 p3 = {_startX + chunk_pos.x + 1, _startY + chunk_pos.y + 0, chunk_pos.z + 0};

	glm::ivec3 p4 = {_startX + chunk_pos.x + 0, _startY + chunk_pos.y + 1, chunk_pos.z + 1};
	glm::ivec3 p5 = {_startX + chunk_pos.x + 1, _startY + chunk_pos.y + 1, chunk_pos.z + 1};
	glm::ivec3 p6 = {_startX + chunk_pos.x + 0, _startY + chunk_pos.y + 1, chunk_pos.z + 0};
	glm::ivec3 p7 = {_startX + chunk_pos.x + 1, _startY + chunk_pos.y + 1, chunk_pos.z + 0};
	int count = face_count(value, chunk_pos.x, chunk_pos.y, chunk_pos.z, value != blocks::OAK_LEAVES);
	int cnt = 0;
	GLfloat *vertFloat = static_cast<GLfloat*>(_vertices);
	GLint *vertInt = static_cast<GLint*>(_vertices);
	if (value == blocks::TORCH) {
		for (size_t index = 0; index < _displayed_faces * 4 * 6; index += 24) {
			_mtx.lock();
			if (torchFace(vertFloat, p0, p1, p2, p3, p4, p6, index)) { // TODO add top(/bottom ?) face too
				vertInt[index] = (vertInt[index] & 0xFFFF0FFF) + (frame << 12);
				vertInt[index + 4] = (vertInt[index + 4] & 0xFFFF0FFF) + (frame << 12);
				vertInt[index + 8] = (vertInt[index + 8] & 0xFFFF0FFF) + ((frame + 1) << 12);
				vertInt[index + 12] = (vertInt[index + 12] & 0xFFFF0FFF) + (frame << 12);
				vertInt[index + 16] = (vertInt[index + 16] & 0xFFFF0FFF) + ((frame + 1) << 12);
				vertInt[index + 20] = (vertInt[index + 20] & 0xFFFF0FFF) + ((frame + 1) << 12);
				_mtx.unlock();
				_vaoVIP = true;
				_vaoReset = false;
				if (++cnt >= count) {
					return ;
				}
				_mtx.lock();
			}
			_mtx.unlock();
		}
		return ;
	} else if (!air_flower(value, false, true)) { // cross image
		for (size_t index = 0; index < _displayed_faces * 4 * 6; index += 24) {
			_mtx.lock();
			if (crossFace(vertFloat, p0, p1, p2, p3, p4, p5, index)) {
				vertInt[index] = (vertInt[index] & 0xFFFF0FFF) + (frame << 12);
				vertInt[index + 4] = (vertInt[index + 4] & 0xFFFF0FFF) + (frame << 12);
				vertInt[index + 8] = (vertInt[index + 8] & 0xFFFF0FFF) + ((frame + 1) << 12);
				vertInt[index + 12] = (vertInt[index + 12] & 0xFFFF0FFF) + (frame << 12);
				vertInt[index + 16] = (vertInt[index + 16] & 0xFFFF0FFF) + ((frame + 1) << 12);
				vertInt[index + 20] = (vertInt[index + 20] & 0xFFFF0FFF) + ((frame + 1) << 12);
				_mtx.unlock();
				_vaoVIP = true;
				_vaoReset = false;
				if (++cnt >= count) {
					return ;
				}
				_mtx.lock();
			}
			_mtx.unlock();
		}
		return ;
	}
	for (size_t index = 0; index < _displayed_faces * 4 * 6; index += 24) {
		_mtx.lock();
		if (blockFace(vertFloat, p0, p1, p2, p3, p4, p5, p6, p7, index)) {
			vertInt[index] = (vertInt[index] & 0xFFFF0FFF) + (frame << 12);
			vertInt[index + 4] = (vertInt[index + 4] & 0xFFFF0FFF) + (frame << 12);
			vertInt[index + 8] = (vertInt[index + 8] & 0xFFFF0FFF) + ((frame + 1) << 12);
			vertInt[index + 12] = (vertInt[index + 12] & 0xFFFF0FFF) + (frame << 12);
			vertInt[index + 16] = (vertInt[index + 16] & 0xFFFF0FFF) + ((frame + 1) << 12);
			vertInt[index + 20] = (vertInt[index + 20] & 0xFFFF0FFF) + ((frame + 1) << 12);
			_mtx.unlock();
			_vaoVIP = true;
			_vaoReset = false;
			if (++cnt >= count) {
				return ;
			}
			_mtx.lock();
		}
		_mtx.unlock();
	}
	// std::cout << "update break not found " << cnt << std::endl; // TODO find out conditions for this to be printed
}

// called by neighbour chunk if block change at border
// posX and posY in [0; CHUNK_SIZE + 1] === _blocks compatible
void Chunk::update_border( int posX, int posY, int level, int type, bool adding )
{
	// std::cout << "got into update border of chunk " << _startX << ", " << _startY << std::endl;
	// std::cout << "args: " << posX << ", " << posY << ", " << level << ": " << ((adding) ? "add" : "rm") << " " << s_blocks[type].name << " | real pos is " << _startX + posX << ", " << _startY + posY << std::endl;
	if (!(!posX || !posY || posX == CHUNK_SIZE - 1 || posY == CHUNK_SIZE - 1)) {
		std::cout << "ERROR update_border not border block " << posX << ", " << posY << std::endl;
		return ;
	}
	if (_thread.joinable()) {
		_thread.join();
	}
	int offset = (((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + level;
	if (adding) {
		// if (_blocks[offset] && _blocks[offset] < blocks::WATER) {
		// 	std::cout << "\033[31mADDING IS REMOVING BLOCK\033[0m " << s_blocks[_blocks[offset]].name << std::endl;
		// }
		if (type >= blocks::WATER) {
			return ;
		}
		int value = _blocks[offset];
		if (!air_flower(type, true, false) || !air_flower(value, false, false)) {
			// std::cout << "took jump" << std::endl;
			goto FILL; // this is only to update face shading TODO only update concerned faces
		}
		_displayed_faces--;
		if (!face_count(value, posX, posY, level, value != blocks::OAK_LEAVES)) { // block no more visible
			_blocks[offset] -= blocks::NOTVISIBLE;
		}
	} else {
		if (_blocks[offset] < blocks::AIR) {
			_blocks[offset] += blocks::NOTVISIBLE;
		}
		if (_blocks[offset] >= blocks::WATER) {
			_hasWater = true;
			_fluids.insert(offset);
		}
		if (type >= blocks::WATER) {
			return ;
		}
		if (!air_flower(_blocks[offset], false, false)) {
			goto FILL;
		}
		_displayed_faces++;
	}
	// std::cout << "got here" << std::endl;
	delete [] static_cast<GLint*>(_vertices);
	_vertices = new GLint[_displayed_faces * 24];
	FILL:
	fill_vertex_array();
}

int Chunk::computePosLight( glm::vec3 pos )
{
	return (computeLight(glm::floor(pos.x - _startX), glm::floor(pos.y - _startY), glm::floor(pos.z)));
}

/* collisionBox takes feet position of object, dimension of its hitbox and returns wether object is inside block or not */
bool Chunk::collisionBox( glm::vec3 pos, float width, float height )
{
	// WATCHOUT if width > 0.5 problemos because we check block left and right but not middle
	glm::ivec3 top0 = glm::ivec3(glm::floor(pos.x - width - _startX), glm::floor(pos.y - width - _startY), glm::floor(pos.z + height));
	if (air_flower(getBlockAt(top0.x, top0.y, top0.z, true), false, false)) {
		return (true);
	}
	glm::ivec3 top1 = glm::ivec3(glm::floor(pos.x + width - _startX), glm::floor(pos.y - width - _startY), glm::floor(pos.z + height));
	if (top1 != top0) {
		if (air_flower(getBlockAt(top1.x, top1.y, top1.z, true), false, false)) {
			return (true);
		}
	}
	glm::ivec3 top2 = glm::ivec3(glm::floor(pos.x + width - _startX), glm::floor(pos.y + width - _startY), glm::floor(pos.z + height));
	if (top2 != top0) {
		if (air_flower(getBlockAt(top2.x, top2.y, top2.z, true), false, false)) {
			return (true);
		}
	}
	glm::ivec3 top3 = glm::ivec3(glm::floor(pos.x - width - _startX), glm::floor(pos.y + width - _startY), glm::floor(pos.z + height));
	if (top3 != top0) {
		if (air_flower(getBlockAt(top3.x, top3.y, top3.z, true), false, false)) {
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
	if (getBlockAt(top0.x, top0.y, top0.z, true) >= blocks::WATER) {
		return (true);
	}
	glm::ivec3 top1 = glm::ivec3(glm::floor(pos.x + width - _startX), glm::floor(pos.y - width - _startY), glm::floor(pos.z + height));
	if (top1 != top0) {
		if (getBlockAt(top1.x, top1.y, top1.z, true) >= blocks::WATER) {
			return (true);
		}
	}
	glm::ivec3 top2 = glm::ivec3(glm::floor(pos.x + width - _startX), glm::floor(pos.y + width - _startY), glm::floor(pos.z + height));
	if (top2 != top0) {
		if (getBlockAt(top2.x, top2.y, top2.z, true) >= blocks::WATER) {
			return (true);
		}
	}
	glm::ivec3 top3 = glm::ivec3(glm::floor(pos.x - width - _startX), glm::floor(pos.y + width - _startY), glm::floor(pos.z + height));
	if (top3 != top0) {
		if (getBlockAt(top3.x, top3.y, top3.z, true) >= blocks::WATER) {
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

void Chunk::applyGravity( void )
{
	float saved_posZ = _camera->getPos().z;
	_camera->applyGravity();
	glm::vec3 pos = _camera->getPos();
	float distZ = saved_posZ - pos.z;
	if (distZ < 0) { // jumping
		_camera->_touchGround = false;
		// std::cout << "DEBUG: " << std::to_string(_camera->_position.z) << std::endl;
		for (float posZ = saved_posZ; posZ < pos.z; posZ++) {
			// std::cout << "testing with posZ " << posZ << std::endl;
			if (collisionBox(glm::vec3(pos.x, pos.y, posZ), 0.3f, 1.8f)) {
				_camera->setPosZ(glm::floor(posZ) + 0.19f);
				// std::cout << "value z spam: " << std::to_string(_camera->_position.z) << std::endl;
				_camera->_fall_distance -= (_camera->getPos().z - posZ);
				_camera->_update = true;
				_camera->_inJump = false;
				return ;
			}
		}
		if (collisionBox(glm::vec3(pos.x, pos.y, pos.z), 0.3f, 1.8f)) {
			_camera->setPosZ(glm::floor(pos.z) + 0.19f);
			// std::cout << "value z spam: " << std::to_string(_camera->_position.z) << std::endl;
			_camera->_fall_distance -= 1;
			_camera->_update = true;
			_camera->_inJump = false;
			return ;
		}
	} else { // falling
		for (float posZ = saved_posZ; posZ - 1 > pos.z; posZ--) {
			if (collisionBox(glm::vec3(pos.x, pos.y, posZ), 0.3f, 1.8f)) {
				_camera->setPosZ(glm::floor((posZ + 1)));
				_camera->_fall_distance -= (_camera->getPos().z - posZ);
				_camera->touchGround();
				if (saved_posZ != _camera->getPos().z) {
					_camera->_update = true;
				}
				return ;
			}
		}
		if (collisionBox(glm::vec3(pos), 0.3f, 1.8f)) {
			_camera->setPosZ(glm::floor((pos.z + 1)));
			_camera->_fall_distance -= (_camera->getPos().z - pos.z);
			_camera->touchGround();
			if (saved_posZ != _camera->getPos().z) {
				_camera->_update = true;
			}
			return ;
		}
	}
	_camera->_update = true;
	_camera->_touchGround = false;
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

void Chunk::drawArray( GLint & counter, GLint &face_counter )
{
	if (_light_update && _nb_neighbours == 4) {
		std::cout << _startX << ", " << _startY << " light update" << std::endl;
		fill_vertex_array();
	}
	if (!_vaoReset) { // TODO change vaoReset logic (swap true and false)
		// std::cout << "chunk reset " << _startX << ", " << _startY << std::endl;
		++counter;
		if (!_vaoVIP && (counter & 63) > 6) { // we don't load more than 5 new chunks per 50 new chunks per frame
		// std::cout << "skip one" << std::endl;
			return ;
		}
		setup_array_buffer();
	}
    glBindVertexArray(_vao); // this is the costly operation, chunk_size up == fps down
	glDrawArrays(GL_TRIANGLES, 0, _displayed_faces * 6);
	face_counter += _displayed_faces;
}

void Chunk::updateFurnaces( double currentTime )
{
	for (auto& fur: _furnaces) {
		int state = fur.second.updateTimes(currentTime);
		if (state != furnace_state::NOCHANGE) {
			// std::cout << "FURNACE STATE CHANGE TO " << state << std::endl;
			auto o = _orientations.find(fur.first);
			if (o != _orientations.end()) {
				// std::cout << "ORIENTATION FOUND" << std::endl;
				o->second = (o->second & 0xF) + (state << 4);
				// set/unset furnace position as light source of 13 using state
				int posZ = o->first & (WORLD_HEIGHT - 1);
				int posY = ((o->first >> WORLD_SHIFT) & (CHUNK_SIZE - 1));
				int posX = ((o->first >> WORLD_SHIFT) >> CHUNK_SHIFT);
				// std::cout << "furnace at " << _startX + posX << ", " << _startY + posY << ", " << posZ << std::endl;
				if (state == furnace_state::ON) {
					_lights[o->first] = 13 + (13 << 4);
					light_spread(posX, posY, posZ, false); // spread block light
				} else {
					_lights[o->first] = 0;
					for (int index = 0; index < 6; index++) {
						const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
						light_try_spread(posX + delta[0], posY + delta[1], posZ + delta[2], 0, false);
					}
				}
				_light_update = true;
			}
		}
	}
}

void Chunk::updateEntities( std::vector<std::pair<int, glm::vec3>> &arr, double deltaTime )
{
	// TODO merge identical close(3/4 of a block) stackable items together
	// on merge, item timer set to longest of 2
	// double for loop, use bool given as parameter to do this once per second at most

	for (auto e = _entities.begin(); e != _entities.end();) {
		if (e->update(arr, _camera->getPos(), deltaTime)) {
			e = _entities.erase(e);
		} else {
			++e;
		}
	}
}

void Chunk::drawSky( GLint & counter, GLint &face_counter )
{
	if (!_sky_count) {
		return ;
	}
	if (_skyVaoReset) {
		// std::cout << "chunk reset " << _startX << ", " << _startY << std::endl;
		++counter;
		if (!_skyVaoVIP && !_skyVaoSet && counter > 5) { // we don't load more than 5 new chunks per frame
			return ;
		} else if (_skyVaoVIP || counter < 6) {
			setup_sky_array_buffer();
		}
	}
    glBindVertexArray(_skyVao);
	glDrawArrays(GL_TRIANGLES, 0, _sky_count * 6); // 6 points/face
	// std::cout << "draw sky" << std::endl;
	face_counter += _sky_count;
}

void Chunk::drawWater( GLint & counter, GLint &face_counter )
{
	if (!_water_count) {
		return ;
	}
	if (_waterVaoReset) {
		// std::cout << "chunk reset " << _startX << ", " << _startY << std::endl;
		++counter;
		if (!_waterVaoVIP && !_waterVaoSet && counter > 5) { // we don't load more than 5 new chunks per frame
			return ;
		} else if (_waterVaoVIP || counter < 6) {
			setup_water_array_buffer();
		}
	}
    glBindVertexArray(_waterVao);
	glDrawArrays(GL_TRIANGLES, 0, _water_count * 6); // 6 points/face
	// std::cout << "draw water" << std::endl;
	face_counter += _water_count;
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
