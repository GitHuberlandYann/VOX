#include "Camera.hpp"
#include "random.hpp"

extern std::mutex mtx_backup;
extern siv::PerlinNoise::seed_type perlin_seed;

Chunk::Chunk( Camera *camera, Inventory *inventory, int posX, int posY, std::list<Chunk *> *chunks )
	: _isVisible(true), _vaoSet(false),
	_waterVaoSet(false), _waterVaoVIP(false),
	_skyVaoSet(false), _skyVaoVIP(false), _genDone(false), _light_update(false), _vertex_update(false),
	_vaoReset(false), _vaoVIP(false), _waterVaoReset(false), _skyVaoReset(false), _sortedOnce(false),
	_startX(posX), _startY(posY), _nb_neighbours(0),
	_blocks(NULL), _water_vert(NULL), _sky_vert(NULL), _lights(NULL),
	_sky(NULL), _hasWater(true), _displayed_faces(0), _water_count(0), _sky_count(0),
	_neighbours({NULL, NULL, NULL, NULL}), _camera(camera), _inventory(inventory)
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
	delete [] _water_vert;
	delete [] _sky;
	delete [] _sky_vert;

	// chests and furnaces are deleted in OpenGl_Manager when deleting backups
	for (auto e : _entities) {
		delete e;
	}
	for (auto p : _particles) {
		delete p;
	}
	for (auto &f : _flames) {
		delete f.second;
	}

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

GLint Chunk::face_count( int type, int row, int col, int level )
{
	type &= 0xFF;
	if (!type || type >= blocks::WATER) {
		std::cerr << "face_count ERROR counting " << s_blocks[type]->name << std::endl;
		return (0);
	}
	if (type == blocks::GLASS || type == blocks::CHEST) {
		return (0);
	}
	if (type >= blocks::POPPY) {
		return (2 << (type >= blocks::TORCH));
	}
	GLint res = visible_face(type, getBlockAt(row - 1, col, level, true), face_dir::MINUSX)
				+ visible_face(type, getBlockAt(row + 1, col, level, true), face_dir::PLUSX)
				+ visible_face(type, getBlockAt(row, col - 1, level, true), face_dir::MINUSY)
				+ visible_face(type, getBlockAt(row, col + 1, level, true), face_dir::PLUSY);
	switch (level) {
		case 0:
			res += visible_face(type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ);
			break ;
		case 255:
			res += visible_face(type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ);
			res += 1;
			break ;
		default:
			res += visible_face(type, getBlockAt(row, col, level - 1, true), face_dir::MINUSZ);
			res += visible_face(type, getBlockAt(row, col, level + 1, true), face_dir::PLUSZ);
	}
	if (type == blocks::OAK_STAIRS_BOTTOM || type == blocks::OAK_STAIRS_TOP) {
		res += 2; // +3 if corner stair, but np so far
	}
	return (res);
}

// takes a block and check if player can see it at some point
bool Chunk::exposed_block( int row, int col, int level, bool isNotLeaves, bool isNotGlass )
{
	// isNotLeaves = true;
	if (!isNotLeaves) {
		return (true);
	}
	if (!isNotGlass) {
		return (false);
	}
	bool below, above;
	switch (level) {
		case 0:
			below = false;
			above = !air_flower(_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level + 1], isNotLeaves, true, false);
			break ;
		case 255:
			below = !air_flower(_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level - 1], isNotLeaves, true, false);
			above = true;
			break ;
		default:
			below = !air_flower(_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level - 1], isNotLeaves, true, false);
			above = !air_flower(_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level + 1], isNotLeaves, true, false);
	}
	return (below || above
		|| !air_flower(getBlockAt(row - 1, col, level, false), isNotLeaves, true, false)
		|| !air_flower(getBlockAt(row + 1, col, level, false), isNotLeaves, true, false)
		|| !air_flower(getBlockAt(row, col - 1, level, false), isNotLeaves, true, false)
		|| !air_flower(getBlockAt(row, col + 1, level, false), isNotLeaves, true, false));
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
			return (blocks::GRASS_BLOCK);
			// return (blocks::DIAMOND_ORE);
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
	_seed = (_startX * 19516 + _startY * 56849) * perlin_seed;

	std::vector<glm::ivec3> trees;

	// generating base terrain
	for (int row = 0; row < CHUNK_SIZE; row++) {
		for (int col = 0; col < CHUNK_SIZE; col++) {
			// int surface_level = glm::floor(SEA_LEVEL + (perlin.octave2D_01((_startX - 1 + row) / 100.0f, (_startY - 1 + col) / 100.0f, 4) - 0.5) * 100);
			int surface_level = surfaceLevel(row, col, perlin);
			int gen = Random::rangedNumber(_seed, 0, 1000);
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
				}//cave = -1;
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
				_blocks[((((tree.x + delta[0]) << CHUNK_SHIFT) + tree.y + delta[1]) << WORLD_SHIFT) + tree.z + delta[2]] = blocks::OAK_LEAVES + blocks::NATURAL;
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
					if (coal_blobs < 20 && Random::rangedNumber(_seed, 0, 1000) <= 1) {
						int size = (Random::rangedNumber(_seed, 0, 1000) + 3) / 30;
						gen_ore_blob(blocks::COAL_ORE, row, col, level, size, 0);
						coal_blobs++;
					} else if (iron_blobs < 10 && (level <= 56 || level > 80) && Random::rangedNumber(_seed, 0, 1000) <= 1) {
						int size = (Random::rangedNumber(_seed, 0, 1000) + 3) / 60;
						gen_ore_blob(blocks::IRON_ORE, row, col, level, size, 0);
						iron_blobs++;
					} else if (!gravel_blob && Random::rangedNumber(_seed, 0, 1000) <= 1) {
						int size = 60;
						gen_ore_blob(blocks::GRAVEL, row, col, level, size, 0);
						gravel_blob = true;
					} else if (diamond_blobs < 7 && level < 15 && Random::rangedNumber(_seed, 0, 1000) <= 1) {
						int size = (Random::rangedNumber(_seed, 0, 1000) + 5) / 200;
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
					if (value >= blocks::WATER) {
						// do nothing
					} else if (exposed_block(row, col, level, value != blocks::OAK_LEAVES, value != blocks::GLASS)) {
						if (value == blocks::IRON_ORE && Random::rangedNumber(_seed, 0, 1000) < 500) {
							_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = blocks::STONE;
						} else if (value == blocks::GRAVEL && Random::rangedNumber(_seed, 0, 1000) < 100) {
							_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = blocks::STONE;
						} else if (value == blocks::DIAMOND_ORE && Random::rangedNumber(_seed, 0, 1000) < 900) {
							_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = blocks::STONE;
						} else if (value == blocks::COAL_ORE && Random::rangedNumber(_seed, 0, 1000) < 200) {
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
		if ((a.second & 0xFF) == blocks::TORCH) {
			addFlame(a.first, {((a.first >> WORLD_SHIFT) >> CHUNK_SHIFT), ((a.first >> WORLD_SHIFT) & (CHUNK_SIZE - 1)), (a.first & (WORLD_HEIGHT - 1))}, blocks::TORCH, (a.second >> 9) & 0x7);
			// _flames.emplace(a.first, new Particle(this, {((a.first >> WORLD_SHIFT) >> CHUNK_SHIFT) + _startX + 0.5f, ((a.first >> WORLD_SHIFT) & (CHUNK_SIZE - 1)) + _startY + 0.5f, (a.first & (WORLD_HEIGHT - 1)) + 10.0f / 16.0f + 0.1f}, PARTICLES::FLAME));
		}
	}
	// b.stamp("rest");
	// b.stop("blocks");
}

void Chunk::resetDisplayedFaces( void )
{
	// hiding unseen blocks
	for (int row = 0; row < CHUNK_SIZE; row++) {
		for (int col = 0; col < CHUNK_SIZE; col++) {
			for (int level = 0; level < WORLD_HEIGHT; level++) {
				int offset = (((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level;
				GLint value = _blocks[offset], type = value & 0xFF;
				bool restore = false;
				if (value & blocks::NOTVISIBLE && (!row || !col || row == CHUNK_SIZE - 1 || col == CHUNK_SIZE - 1)) {
					value -= blocks::NOTVISIBLE;
					restore = true;
				}
				if (type == blocks::CHEST) {}
				else if (type > blocks::AIR && type < blocks::WATER) {
					GLint below = ((level) ? _blocks[offset - 1] : 0) & 0xFF;
					if (!air_flower(type, false, false, true) && type < blocks::TORCH && below != blocks::GRASS_BLOCK && below != blocks::DIRT && below != blocks::SAND) {
						_blocks[offset] = blocks::AIR;
					} else {
						GLint count = face_count(type, row, col, level);
						if (count) {
							// std::cout << "count is " << count << std::endl;
							if (restore) {
								_blocks[offset] = value;
							}
						} else if (type > blocks::AIR) { // hide block
							_blocks[offset] = value + blocks::NOTVISIBLE;
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

void Chunk::handle_border_block( glm::ivec3 pos, int type, bool adding )
{
	if (type == blocks::AIR) {
		return ;
	}
	if (!pos.x && _neighbours[face_dir::MINUSX]) {
		_neighbours[face_dir::MINUSX]->update_border(CHUNK_SIZE - 1, pos.y, pos.z, type, adding, face_dir::PLUSX);
	} else if (pos.x == CHUNK_SIZE - 1 && _neighbours[face_dir::PLUSX]) {
		_neighbours[face_dir::PLUSX]->update_border(0, pos.y, pos.z, type, adding, face_dir::MINUSX);
	}
	if (!pos.y && _neighbours[face_dir::MINUSY]) {
		_neighbours[face_dir::MINUSY]->update_border(pos.x, CHUNK_SIZE - 1, pos.z, type, adding, face_dir::PLUSY);
	} else if (pos.y == CHUNK_SIZE - 1 && _neighbours[face_dir::PLUSY]) {
		_neighbours[face_dir::PLUSY]->update_border(pos.x, 0, pos.z, type, adding, face_dir::MINUSY);
	}
}

/* generate one or many entities depending on block broken */
void Chunk::entity_block( int posX, int posY, int posZ, int type )
{
	// std::cout << "breaking " << s_blocks[type]->name << std::endl;
	float random;
	switch (type) {
	case blocks::GRASS:
		random = Random::randomFloat(_seed);
		if (random <= 0.125f) {
			_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, false, {blocks::WHEAT_SEEDS, 1, {0, 0}}));
		}
		break ;
	case blocks::OAK_LEAVES:
		random = Random::randomFloat(_seed);
		if (random <= 0.05f) {
			_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, false, {blocks::OAK_SAPLING, 1, {0, 0}}));
		}
		random = Random::randomFloat(_seed);
		if (random <= 0.02f) {
			_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, false, {blocks::STICK, 2, {0, 0}}));
		}
		random = Random::randomFloat(_seed);
		if (random <= 0.005f) {
			_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, false, {blocks::APPLE, 1, {0, 0}}));
		}
		break ;
	case blocks::WHEAT_CROP7:
		_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, false, {blocks::WHEAT, 1, {0, 0}}));
		random = Random::randomFloat(_seed);
		if (random <= 0.0787f) {
			_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, false, {blocks::WHEAT_SEEDS, 1, {0, 0}}));
		} else if (random <= 0.0787f + 0.3149f) {
			_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, false, {blocks::WHEAT_SEEDS, 2, {0, 0}}));
		} else if (random <= 0.0787f + 0.3149f + 0.4198f) {
			_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, false, {blocks::WHEAT_SEEDS, 3, {0, 0}}));
		} else {
			_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, false, {blocks::WHEAT_SEEDS, 4, {0, 0}}));
		}
		break ;
	case blocks::GRAVEL:
		random = Random::randomFloat(_seed);
		if (random < 0.1f) {
			_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, false, {blocks::FLINT, 1, {0, 0}}));
			return ;
		} // no break on gravel so it drops a gravel block
	default:
		_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, false, {s_blocks[type]->mined, 1, {0, 0}}));
	}
}

void Chunk::remove_block( bool useInventory, glm::ivec3 pos )
{
	// std::cout << "in chunk " << _startX << ", " << _startY << ":rm block " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
	// std::cout << "nb displayed blocks before: " << _displayed_blocks << std::endl;
	int offset = (((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z;
	int value = _blocks[offset], type = value & 0xFF; // TODO better handling of invis block gets deleted
	_added.erase(offset);
	_removed.insert(offset);
	if (type == blocks::CHEST) {
		auto search = _chests.find(offset);
		if (search != _chests.end()) {
			t_item *item;
			for (int index = 0; index < 27; ++index) {
				item = search->second->getItem(index);
				if (item->amount) {
					_entities.push_back(new Entity(this, _inventory, {pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, false, *item));
				}
			}
		}
		delete _chests[offset]; // TODO what happens if chest explodes while we use it ? prob heap use after free in Menu.cpp
		_chests.erase(offset);
	} else if (type == blocks::FURNACE) {
		auto search = _furnaces.find(offset); // drop furnace's items
		if (search != _furnaces.end()) {
			t_item items = search->second->getComposant();
			if (items.amount) {
				_entities.push_back(new Entity(this, _inventory, {pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f}, {glm::normalize(glm::vec2(-1, -1)), 1.0f}, false, false, items));
			}
			items = search->second->getFuel();
			if (items.amount) {
				_entities.push_back(new Entity(this, _inventory, {pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f}, {glm::normalize(glm::vec2(1, 9)), 1.0f}, false, false, items));
			}
			items = search->second->getProduction();
			if (items.amount) {
				_entities.push_back(new Entity(this, _inventory, {pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f}, {glm::normalize(glm::vec2(-4, 5)), 1.0f}, false, false, items));
			}
		}
		delete _furnaces[offset];
		_furnaces.erase(offset);
	} else if (type == blocks::OAK_DOOR) {
		// break other part of door
		_blocks[offset + (((value >> 12) & DOOR::UPPER_HALF) ? -1 : 1)] = blocks::AIR;
		_added.erase(offset + (((value >> 12) & DOOR::UPPER_HALF) ? -1 : 1));
		_removed.insert(offset + (((value >> 12) & DOOR::UPPER_HALF) ? -1 : 1));
	}
	if (useInventory && type == blocks::WATER) {
		_inventory->addBlock(type); // water bucket
	} else if (useInventory) {
		entity_block(pos.x, pos.y, pos.z, type);
	}
	int type_above = blocks::AIR;
	if (pos.z < 255) {
		type_above = _blocks[offset + 1] & 0xFF;
		if (type_above == blocks::SAND || type_above == blocks::GRAVEL) { // sand falls if block underneath deleted
			// std::cout << "new scheduled_to_fall" << std::endl;
			_scheduled_to_fall.push_back(offset + 1);
		}
	}
	_blocks[offset] = blocks::AIR;
	if (type == blocks::GLASS) { // TODO check neighbours and spread water
		return ;
	} else if (type > blocks::AIR && type < blocks::WATER) { // if invisible block gets deleted, same amount of displayed_blocks
		if (s_blocks[type]->light_level) {
			std::cout << "rm light" << std::endl;
			if (type == blocks::TORCH) {
				delete _flames[offset];
				_flames.erase(offset);
			}
			_lights[offset] &= 0xFF00;
			light_spread(pos.x, pos.y, pos.z, false); // spread block light
			_light_update = false;
			std::cout << "over" << std::endl;
		} else if (type == blocks::OAK_LEAVES) {
			_lights[offset] &= 0xFF;
		} else if (type == blocks::LEVER) {
			flickLever(pos, blocks::AIR, false);
		}
	} else if (type == blocks::WATER) { // use bucket on water source
		// std::cout << "bucket on water" << std::endl;
		_fluids.insert(offset);
		return ;
	}
	for (int i = 0; i < 15; ++i) {
		_particles.push_back(new Particle(this, {pos.x + _startX + Random::randomFloat(_seed), pos.y + _startY + Random::randomFloat(_seed), pos.z + Random::randomFloat(_seed)}, PARTICLES::BREAKING, 0, type));
	}
	if (air_flower(type, false, false, true)) {
		light_spread(pos.x, pos.y, pos.z, true); // spread sky light
		light_spread(pos.x, pos.y, pos.z, false); // spread block light
		for (int index = 0; index < 6; index++) {
			const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
			if (pos.x + delta[0] < 0 || pos.x + delta[0] >= CHUNK_SIZE || pos.y + delta[1] < 0 || pos.y + delta[1] >= CHUNK_SIZE || pos.z + delta[2] < 0 || pos.z + delta[2] > 255) {
			} else {
				int adj_offset = ((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2];
				int adj = _blocks[adj_offset];
				if (adj & blocks::NOTVISIBLE) {
					_blocks[adj_offset] = adj - blocks::NOTVISIBLE;
				} else if (air_flower(adj, false, false, false)) {
				} else if (index != face_dir::MINUSZ && (adj & 0xFF) >= blocks::WATER) { // if water under, it is not updated
					_fluids.insert(adj_offset);
				} else if ((adj & 0xFF) == blocks::TORCH && ((adj >> 9) & 0x7) == opposite_dir(index)) {
					std::cout << "rm adj torch" << std::endl;
					remove_block(useInventory, {pos.x + delta[0], pos.y + delta[1], pos.z + delta[2]});
				}
				if ((adj & 0xFF) == blocks::OAK_FENCE || (adj & 0xFF) == blocks::GLASS_PANE) {
					switch (index) {
						case face_dir::MINUSX:
							_blocks[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] ^= (FENCE::PX << 12);
							_added[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] ^= (FENCE::PX << 12);
							break ;
						case face_dir::PLUSX:
							_blocks[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] ^= (FENCE::MX << 12);
							_added[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] ^= (FENCE::MX << 12);
							break ;
						case face_dir::MINUSY:
							_blocks[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] ^= (FENCE::PY << 12);
							_added[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] ^= (FENCE::PY << 12);
							break ;
						case face_dir::PLUSY:
							_blocks[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] ^= (FENCE::MY << 12);
							_added[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] ^= (FENCE::MY << 12);
							break ;
					}
				}
			}
		}
	}
	handle_border_block(pos, air_flower(value, true, false, true), false); // if block at border of chunk gets deleted, we update neighbours. doing it after light spread
	if (type_above != blocks::TORCH && type_above != blocks::CHEST && ((type_above >= blocks::POPPY && type_above < blocks::WATER) || type_above == blocks::CACTUS
		|| (type_above == blocks::OAK_DOOR && type != blocks::OAK_DOOR))) { // del flower if block underneath deleted
		(type_above == blocks::GRASS)
			? remove_block(false, {pos.x, pos.y, pos.z + 1}) // if grass above breaked block, don't collect it
			: remove_block(useInventory, {pos.x, pos.y, pos.z + 1});
	}
	// std::cout << "nb displayed blocks after: " << _displayed_blocks << std::endl;
}

// takes stair_bottom at given pos and check if it should be corner stair
void Chunk::handle_stair_corners( glm::ivec3 pos, int &type )
{
	int behind, front, left, right;
	switch ((type >> 9) & 0x7) {
		case face_dir::MINUSX:
			type |= ((CORNERS::PM | CORNERS::PP) << 12);
			behind = getBlockAt(pos.x + 1, pos.y, pos.z, true);
			if ((behind & 0xFF) == (type & 0xFF)) {
				if (((behind >> 9) & 0x7) == face_dir::MINUSY) {
					type ^= (CORNERS::PM << 12); // outside corner
					goto SKIP_FRONT_MINUSX;
				} else if (((behind >> 9) & 0x7) == face_dir::PLUSY) {
					type ^= (CORNERS::PP << 12);
					goto SKIP_FRONT_MINUSX;
				}
			}
			front = getBlockAt(pos.x - 1, pos.y, pos.z, true);
			if ((front & 0xFF) == (type & 0xFF)) {
				if (((front >> 9) & 0x7) == face_dir::MINUSY) {
					type |= (CORNERS::MP << 12); // inside corner
				} else if (((front >> 9) & 0x7) == face_dir::PLUSY) {
					type |= (CORNERS::MM << 12);
				}
			}
			SKIP_FRONT_MINUSX:
			left = getBlockAt(pos.x, pos.y + 1, pos.z, true);
			if ((left & 0xFF) == (type & 0xFF)) {
				if (((left >> 9) & 0x7) == face_dir::MINUSY) {
					if (((left >> 12) & 0xF) == (CORNERS::MP | CORNERS::PP)) {
						left |= (CORNERS::PM << 12); // transform left stair into inside corner stair
						setBlockAt(left, pos.x, pos.y + 1, pos.z, true);
					}
				} else if (((left >> 9) & 0x7) == face_dir::PLUSY) {
					if (((left >> 12) & 0xF) == (CORNERS::MM | CORNERS::PM)) {
						left ^= (CORNERS::MM << 12); // transform left stair into outside corner stair
						setBlockAt(left, pos.x, pos.y + 1, pos.z, true);
					}
				}
			}
			right = getBlockAt(pos.x, pos.y - 1, pos.z, true);
			if ((right & 0xFF) == (type & 0xFF)) {
				if (((right >> 9) & 0x7) == face_dir::MINUSY) {
					if (((right >> 12) & 0xF) == (CORNERS::MP | CORNERS::PP)) {
						right ^= (CORNERS::MP << 12); // transform left stair into outside corner stair
						setBlockAt(right, pos.x, pos.y - 1, pos.z, true);
					}
				} else if (((right >> 9) & 0x7) == face_dir::PLUSY) {
					if (((right >> 12) & 0xF) == (CORNERS::MM | CORNERS::PM)) {
						right |= (CORNERS::PP << 12); // transform right stair into inside corner stair
						setBlockAt(right, pos.x, pos.y - 1, pos.z, true);
					}
				}
			}
			break ;
		case face_dir::PLUSX:
			type |= ((CORNERS::MM | CORNERS::MP) << 12);
			behind = getBlockAt(pos.x - 1, pos.y, pos.z, true);
			if ((behind & 0xFF) == (type & 0xFF)) {
				if (((behind >> 9) & 0x7) == face_dir::MINUSY) {
					type ^= (CORNERS::MM << 12); // outside corner
					goto SKIP_FRONT_PLUSX;
				} else if (((behind >> 9) & 0x7) == face_dir::PLUSY) {
					type ^= (CORNERS::MP << 12);
					goto SKIP_FRONT_PLUSX;
				}
			}
			front = getBlockAt(pos.x + 1, pos.y, pos.z, true);
			if ((front & 0xFF) == (type & 0xFF)) {
				if (((front >> 9) & 0x7) == face_dir::MINUSY) {
					type |= (CORNERS::PP << 12); // inside corner
				} else if (((front >> 9) & 0x7) == face_dir::PLUSY) {
					type |= (CORNERS::PM << 12);
				}
			}
			SKIP_FRONT_PLUSX:
			left = getBlockAt(pos.x, pos.y - 1, pos.z, true);
			if ((left & 0xFF) == (type & 0xFF)) {
				if (((left >> 9) & 0x7) == face_dir::MINUSY) {
					if (((left >> 12) & 0xF) == (CORNERS::MP | CORNERS::PP)) {
						left ^= (CORNERS::PP << 12); // transform left stair into outside corner stair
						setBlockAt(left, pos.x, pos.y - 1, pos.z, true);
					}
				} else if (((left >> 9) & 0x7) == face_dir::PLUSY) {
					if (((left >> 12) & 0xF) == (CORNERS::MM | CORNERS::PM)) {
						left |= (CORNERS::MP << 12); // transform left stair into inside corner stair
						setBlockAt(left, pos.x, pos.y - 1, pos.z, true);
					}
				}
			}
			right = getBlockAt(pos.x, pos.y + 1, pos.z, true);
			if ((right & 0xFF) == (type & 0xFF)) {
				if (((right >> 9) & 0x7) == face_dir::MINUSY) {
					if (((right >> 12) & 0xF) == (CORNERS::MP | CORNERS::PP)) {
						right |= (CORNERS::MM << 12); // transform right stair into inside corner stair
						setBlockAt(right, pos.x, pos.y + 1, pos.z, true);
					}
				} else if (((right >> 9) & 0x7) == face_dir::PLUSY) {
					if (((right >> 12) & 0xF) == (CORNERS::MM | CORNERS::PM)) {
						right ^= (CORNERS::PM << 12); // transform left stair into outside corner stair
						setBlockAt(right, pos.x, pos.y + 1, pos.z, true);
					}
				}
			}
			break ;
		case face_dir::MINUSY:
			type |= ((CORNERS::MP | CORNERS::PP) << 12);
			behind = getBlockAt(pos.x, pos.y + 1, pos.z, true);
			if ((behind & 0xFF) == (type & 0xFF)) {
				if (((behind >> 9) & 0x7) == face_dir::MINUSX) {
					type ^= (CORNERS::MP << 12); // outside corner
					goto SKIP_FRONT_MINUSY;
				} else if (((behind >> 9) & 0x7) == face_dir::PLUSX) {
					type ^= (CORNERS::PP << 12);
					goto SKIP_FRONT_MINUSY;
				}
			}
			front = getBlockAt(pos.x, pos.y - 1, pos.z, true);
			if ((front & 0xFF) == (type & 0xFF)) {
				if (((front >> 9) & 0x7) == face_dir::MINUSX) {
					type |= (CORNERS::PM << 12); // inside corner
				} else if (((front >> 9) & 0x7) == face_dir::PLUSX) {
					type |= (CORNERS::MM << 12);
				}
			}
			SKIP_FRONT_MINUSY:
			left = getBlockAt(pos.x - 1, pos.y, pos.z, true);
			if ((left & 0xFF) == (type & 0xFF)) {
				if (((left >> 9) & 0x7) == face_dir::MINUSX) {
					if (((left >> 12) & 0xF) == (CORNERS::PM | CORNERS::PP)) {
						left ^= (CORNERS::PM << 12); // transform left stair into outside corner stair
						setBlockAt(left, pos.x - 1, pos.y, pos.z, true);
					}
				} else if (((left >> 9) & 0x7) == face_dir::PLUSX) {
					if (((left >> 12) & 0xF) == (CORNERS::MM | CORNERS::MP)) {
						left |= (CORNERS::PP << 12); // transform left stair into inside corner stair
						setBlockAt(left, pos.x - 1, pos.y, pos.z, true);
					}
				}
			}
			right = getBlockAt(pos.x + 1, pos.y, pos.z, true);
			if ((right & 0xFF) == (type & 0xFF)) {
				if (((right >> 9) & 0x7) == face_dir::MINUSX) {
					if (((right >> 12) & 0xF) == (CORNERS::PM | CORNERS::PP)) {
						right |= (CORNERS::MP << 12); // transform right stair into inside corner stair
						setBlockAt(right, pos.x + 1, pos.y, pos.z, true);
					}
				} else if (((right >> 9) & 0x7) == face_dir::PLUSX) {
					if (((right >> 12) & 0xF) == (CORNERS::MM | CORNERS::MP)) {
						right ^= (CORNERS::MM << 12); // transform left stair into outside corner stair
						setBlockAt(right, pos.x + 1, pos.y, pos.z, true);
					}
				}
			}
			break ;
		case face_dir::PLUSY:
			type |= ((CORNERS::MM | CORNERS::PM) << 12);
			behind = getBlockAt(pos.x, pos.y - 1, pos.z, true);
			if ((behind & 0xFF) == (type & 0xFF)) {
				if (((behind >> 9) & 0x7) == face_dir::MINUSX) {
					type ^= (CORNERS::MM << 12); // outside corner
					goto SKIP_FRONT_PLUSY;
				} else if (((behind >> 9) & 0x7) == face_dir::PLUSX) {
					type ^= (CORNERS::PM << 12);
					goto SKIP_FRONT_PLUSY;
				}
			}
			front = getBlockAt(pos.x, pos.y + 1, pos.z, true);
			if ((front & 0xFF) == (type & 0xFF)) {
				if (((front >> 9) & 0x7) == face_dir::MINUSX) {
					type |= (CORNERS::PP << 12); // inside corner
				} else if (((front >> 9) & 0x7) == face_dir::PLUSX) {
					type |= (CORNERS::MP << 12);
				}
			}
			SKIP_FRONT_PLUSY:
			left = getBlockAt(pos.x + 1, pos.y, pos.z, true);
			if ((left & 0xFF) == (type & 0xFF)) {
				if (((left >> 9) & 0x7) == face_dir::MINUSX) {
					if (((left >> 12) & 0xF) == (CORNERS::PM | CORNERS::PP)) {
						left |= (CORNERS::MM << 12); // transform left stair into inside corner stair
						setBlockAt(left, pos.x + 1, pos.y, pos.z, true);
					}
				} else if (((left >> 9) & 0x7) == face_dir::PLUSX) {
					if (((left >> 12) & 0xF) == (CORNERS::MM | CORNERS::MP)) {
						left ^= (CORNERS::MP << 12); // transform left stair into outside corner stair
						setBlockAt(left, pos.x + 1, pos.y, pos.z, true);
					}
				}
			}
			right = getBlockAt(pos.x - 1, pos.y, pos.z, true);
			if ((right & 0xFF) == (type & 0xFF)) {
				if (((right >> 9) & 0x7) == face_dir::MINUSX) {
					if (((right >> 12) & 0xF) == (CORNERS::PM | CORNERS::PP)) {
						right ^= (CORNERS::PP << 12); // transform left stair into outside corner stair
						setBlockAt(right, pos.x - 1, pos.y, pos.z, true);
					}
				} else if (((right >> 9) & 0x7) == face_dir::PLUSX) {
					if (((right >> 12) & 0xF) == (CORNERS::MM | CORNERS::MP)) {
						right |= (CORNERS::PM << 12); // transform right stair into inside corner stair
						setBlockAt(right, pos.x - 1, pos.y, pos.z, true);
					}
				}
			}
			break ;
	}
}

void Chunk::handle_door_placement( glm::ivec3 pos, int &type )
{
	int below = getBlockAt(pos.x, pos.y, pos.z - 1, false);
	if (!s_blocks[below & 0xFF]->collisionHitbox_1x1x1) {
		type = blocks::AIR;
		return ;
	}
	int above = getBlockAt(pos.x, pos.y, pos.z + 1, false);
	if (above != blocks::AIR) {
		type = blocks::AIR;
		return ;
	}
	// TODO DOOR::RIGHT_HINGE
}

// used by glass pane too
void Chunk::handle_fence_placement( glm::ivec3 pos, int &type )
{
	int next = getBlockAt(pos.x - 1, pos.y, pos.z, true);
	if (s_blocks[next & 0xFF]->collisionHitbox_1x1x1 || (next & 0xFF) == (type & 0xFF)) {
		type |= (FENCE::MX << 12);
	}
	next = getBlockAt(pos.x + 1, pos.y, pos.z, true);
	if (s_blocks[next & 0xFF]->collisionHitbox_1x1x1 || (next & 0xFF) == (type & 0xFF)) {
		type |= (FENCE::PX << 12);
	}
	next = getBlockAt(pos.x, pos.y - 1, pos.z, true);
	if (s_blocks[next & 0xFF]->collisionHitbox_1x1x1 || (next & 0xFF) == (type & 0xFF)) {
		type |= (FENCE::MY << 12);
	}
	next = getBlockAt(pos.x, pos.y + 1, pos.z, true);
	if (s_blocks[next & 0xFF]->collisionHitbox_1x1x1 || (next & 0xFF) == (type & 0xFF)) {
		type |= (FENCE::PY << 12);
	}
}

void Chunk::addFlame( int offset, glm::vec3 pos, int source, int orientation )
{
	if (source == blocks::TORCH) {
		switch (orientation) {
		case face_dir::MINUSX:
			_flames.emplace(offset, new Particle(this, {pos.x + _startX + 0.25f, pos.y + _startY + 0.5f, pos.z + 15.0f / 16.0f}, PARTICLES::FLAME));
			break ;
		case face_dir::PLUSX:
			_flames.emplace(offset, new Particle(this, {pos.x + _startX + 0.75f, pos.y + _startY + 0.5f, pos.z + 15.0f / 16.0f}, PARTICLES::FLAME));
			break ;
		case face_dir::MINUSY:
			_flames.emplace(offset, new Particle(this, {pos.x + _startX + 0.5f, pos.y + _startY + 0.25f, pos.z + 15.0f / 16.0f}, PARTICLES::FLAME));
			break ;
		case face_dir::PLUSY:
			_flames.emplace(offset, new Particle(this, {pos.x + _startX + 0.5f, pos.y + _startY + 0.75f, pos.z + 15.0f / 16.0f}, PARTICLES::FLAME));
			break ;
		case face_dir::MINUSZ:
			_flames.emplace(offset, new Particle(this, {pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 11.0f / 16.0f + 0.05f}, PARTICLES::FLAME));
			break ;
		}
	}
}

void Chunk::add_block( bool useInventory, glm::ivec3 pos, int block_value, int previous )
{
	// std::cout << "in chunk " << _startX << ", " << _startY << ":add block " << _startX + pos.x << ", " << _startY + pos.y << ", " << pos.z << std::endl;
	// std::cout << "nb displayed blocks before: " << _displayed_blocks << std::endl;
	int offset = (((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z;
	int type = block_value & 0xFF;
	if (type == blocks::SAND || type == blocks::GRAVEL) {
		int type_under = (_blocks[offset - 1] & 0xFF);
		if (type_under == blocks::AIR) {
			if (useInventory) {
				_inventory->removeBlock(false);
			}
			_entities.push_back(new Entity(this, _inventory, {pos.x + _startX, pos.y + _startY, pos.z}, {0, 0, 0}, true, false, {type, 1, {0, 0}}));
			return ;
		}
	}
	if (type == blocks::CHEST) { // chests treated like flowers, but can be placed anywhere
	} else if (type == blocks::WATER) { // we place water
		if (previous < blocks::WATER) {
			_hasWater = true;
		}
	} else if (type == blocks::TORCH) {
		// check if orientation possible (wall available to hang from)
		// if not, check if block underneath and change orientation
		// else abort mission
		int neighbourShape = GEOMETRY::NONE;
		switch ((block_value >> 9) & 0x7) {
		case face_dir::PLUSX:
			neighbourShape = s_blocks[getBlockAt(pos.x + 1, pos.y, pos.z, true) & 0xFF]->geometry;
			break;
		case face_dir::MINUSX:
			neighbourShape = s_blocks[getBlockAt(pos.x - 1, pos.y, pos.z, true) & 0xFF]->geometry;
			break;
		case face_dir::PLUSY:
			neighbourShape = s_blocks[getBlockAt(pos.x, pos.y + 1, pos.z, true) & 0xFF]->geometry;
			break;
		case face_dir::MINUSY:
			neighbourShape = s_blocks[getBlockAt(pos.x, pos.y - 1, pos.z, true) & 0xFF]->geometry;
			break;
		}
		if (neighbourShape != GEOMETRY::CUBE) {
			block_value = blocks::TORCH + (face_dir::MINUSZ << 9);
			neighbourShape = s_blocks[getBlockAt(pos.x, pos.y, pos.z - 1, false) & 0xFF]->geometry;
			if (!(neighbourShape == GEOMETRY::CUBE || neighbourShape == GEOMETRY::SLAB_TOP
				|| neighbourShape == GEOMETRY::STAIRS_TOP || neighbourShape == GEOMETRY::FENCE)) {
				return ;
			}
		}
		addFlame(offset, pos, blocks::TORCH, (block_value >> 9) & 0x7);
	} else if (s_blocks[type]->oriented) {
		if (type != blocks::LEVER || ((block_value >> 12) & 0x3) != PLACEMENT::WALL) {
			block_value += (_camera->getOrientation() << 9);
		}
		switch (s_blocks[type]->geometry) {
			case GEOMETRY::STAIRS_BOTTOM:
			case GEOMETRY::STAIRS_TOP:
				handle_stair_corners(pos, block_value);
				break ;
			case GEOMETRY::DOOR:
				handle_door_placement(pos, block_value);
				if (block_value == blocks::AIR) return ; // door couldn't be placed
				else if (!((block_value >> 12) & DOOR::UPPER_HALF)) { // place upper_half
					_blocks[offset + 1] = block_value | (DOOR::UPPER_HALF << 12);
					_removed.erase(offset + 1);
					_added[offset + 1] = block_value | (DOOR::UPPER_HALF << 12);
				}
				break ;
		}
	} else if (type == blocks::OAK_FENCE || type == blocks::GLASS_PANE) {
		handle_fence_placement(pos, block_value);
	} else if (type >= blocks::POPPY && pos.z > 0) {
		if (previous >= blocks::WATER) {
			return ;
		}
		int type_below = _blocks[offset - 1] & 0xFF;
		if (type == blocks::WHEAT_CROP) {
		} else if (type_below != blocks::GRASS_BLOCK && type_below != blocks::DIRT && type_below != blocks::SAND) {
			return ; // can't place flower on something else than grass/dirt block
		} else if (!(type_below > blocks::AIR && type_below < blocks::POPPY) || s_blocks[type_below]->hasHitbox) {
			return ;
		}
	} else if (previous >= blocks::WATER) { // replace water block with something else
		_fluids.insert(offset);
	}
	if (useInventory) {
		_inventory->removeBlock(false);
	}

	_blocks[offset] = block_value;
	_removed.erase(offset);
	_added[offset] = block_value;
	if (type == blocks::WATER) {
		_fluids.insert(offset);
		// std::cout << "adding water" << std::endl;
		return ;
	}
	if (type == blocks::CHEST) {
		_chests.emplace(offset, new ChestInstance(this, {pos.x + _startX, pos.y + _startY, pos.z}, _camera->getOrientation()));
	} else if (type == blocks::FURNACE) {
		_furnaces.emplace(offset, new FurnaceInstance());
	} else if (type == blocks::TORCH) {
		std::cout << "add light" << std::endl;
		_lights[offset] &= 0xFF00;
		_lights[offset] += s_blocks[type]->light_level + (s_blocks[type]->light_level << 4);
		light_spread(pos.x, pos.y, pos.z, false);
		handle_border_block(pos, type, true); // torch is special case flower
		_light_update = false;
		std::cout << "over" << std::endl;
	} else if (type == blocks::GLASS) {
		_hasWater = true; // glass considered as invis block
	}
	if (!air_flower(type, true, true, false)) {
		return ;
	}
	if (!s_blocks[type]->transparent) {
		_lights[offset] = 0; // rm light if solid block added
	}
	for (int index = 0; index < 6; index++) {
		const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
		if (pos.x + delta[0] < 0 || pos.x + delta[0] >= CHUNK_SIZE || pos.y + delta[1] < 0 || pos.y + delta[1] >= CHUNK_SIZE || pos.z + delta[2] < 0 || pos.z + delta[2] > 255) {

		} else {
			GLint adj = _blocks[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] & 0xFF;
			if (air_flower(adj, false, true, false)) {
				_displayed_faces -= !visible_face(adj, type,  opposite_dir(index));
			} else {
				if (index != face_dir::PLUSZ) {
					light_try_spread(pos.x + delta[0], pos.y + delta[1], pos.z + delta[2], 1, true, LIGHT_RECURSE); // spread sky light, but not upwards duh
				}
				light_try_spread(pos.x + delta[0], pos.y + delta[1], pos.z + delta[2], 1, false, LIGHT_RECURSE);
			}
			if (adj > blocks::AIR && adj < blocks::WATER && !face_count(adj, pos.x + delta[0], pos.y + delta[1], pos.z + delta[2])) {
				// was exposed before, but isn't anymore
				_blocks[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] += blocks::NOTVISIBLE;
			}
			if ((adj & 0xFF) == blocks::OAK_FENCE || (adj & 0xFF) == blocks::GLASS_PANE) {
				switch (index) {
					case face_dir::MINUSX:
						_blocks[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] |= (FENCE::PX << 12);
						_added[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] |= (FENCE::PX << 12);
						break ;
					case face_dir::PLUSX:
						_blocks[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] |= (FENCE::MX << 12);
						_added[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] |= (FENCE::MX << 12);
						break ;
					case face_dir::MINUSY:
						_blocks[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] |= (FENCE::PY << 12);
						_added[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] |= (FENCE::PY << 12);
						break ;
					case face_dir::PLUSY:
						_blocks[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] |= (FENCE::MY << 12);
						_added[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] |= (FENCE::MY << 12);
						break ;
				}
			}
		}
	}
	handle_border_block(pos, air_flower(type, false, false, true), true); // if block at border of chunk gets added, we update neighbours, after light spread
		// std::cout << "nb displayed blocks after: " << _displayed_blocks << std::endl;
}

void Chunk::replace_block( bool useInventory, glm::ivec3 pos, int type, int previous )
{
	// std::cout << "replace_block " << useInventory << std::endl;
	if (useInventory) {
		_inventory->decrementDurabitilty();
	}
	int offset = (((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z;
	_blocks[offset] = blocks::AIR;
	handle_border_block(pos, previous, false);
	// std::cout << "\t_displayed_faces after rm " << _displayed_faces << std::endl;
	_blocks[offset] = type;
	_added[offset] = type;
	light_spread(pos.x, pos.y, pos.z, false);
	light_spread(pos.x, pos.y, pos.z, true);
	_light_update = false;
	for (int index = 0; index < 6; index++) {
		const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
		if (pos.x + delta[0] < 0 || pos.x + delta[0] >= CHUNK_SIZE || pos.y + delta[1] < 0 || pos.y + delta[1] >= CHUNK_SIZE || pos.z + delta[2] < 0 || pos.z + delta[2] > 255) {
		} else {
			GLint adj = _blocks[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]];
			if (visible_face(adj, type, opposite_dir(index))) {
				if (adj & blocks::NOTVISIBLE) {
					_blocks[((((pos.x + delta[0]) << CHUNK_SHIFT) + pos.y + delta[1]) << WORLD_SHIFT) + pos.z + delta[2]] = adj - blocks::NOTVISIBLE;
				}
			}
		}
	}
	// std::cout << "\t_displayed_faces after add neighbours " << _displayed_faces << std::endl;
	handle_border_block(pos, type, true);
}

// works as long as sizeof(int) == sizeof(float)
// we output 1 int and 3 floats to shader
void Chunk::setup_array_buffer( void )
{
	// if (_thread.joinable()) {
	// 	_thread.join();
	// }

	if (!_vaoSet) {
		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vbo);
		_vaoSet = true;
	}
	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	_displayed_faces = _vertices.size() / 6;
	_mtx.lock();
	glBufferData(GL_ARRAY_BUFFER, _displayed_faces * 4 * 6 * sizeof(GLint), &_vertices[0].spec, GL_STATIC_DRAW);
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
	// if (_thread.joinable()) {
	// 	_thread.join();
	// }

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
	// if (_thread.joinable()) {
	// 	_thread.join();
	// }

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

unsigned Chunk::getSeed( void )
{
	return (_seed);
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

void Chunk::setBackup( std::map<std::pair<int, int>, s_backup> &backups )
{
	if (_added.size() || _removed.size()) {
		mtx_backup.lock();
		backups[std::pair<int, int>(_startX, _startY)] = {_added, _removed, _chests, _furnaces};
		mtx_backup.unlock();
	}
}

void Chunk::restoreBackup( s_backup &backup )
{
	_added = backup.added;
	_removed = backup.removed;
	_chests = backup.chests;
	for (auto &ch : _chests) {
		// std::cout << "chest in backup of " << _startX << " " << _startY << " at " << ch.first << std::endl;
		int posZ = ch.first & (WORLD_HEIGHT - 1);
		int posY = ((ch.first >> WORLD_SHIFT) & (CHUNK_SIZE - 1));
		int posX = ((ch.first >> WORLD_SHIFT) >> CHUNK_SHIFT);
		ch.second->setChunk(this, {posX + _startX, posY + _startY, posZ});
	}
	_furnaces = backup.furnaces;
}

void Chunk::openChest( glm::ivec3 pos )
{
	int key = ((((pos.x - _startX) << CHUNK_SHIFT) + pos.y - _startY) << WORLD_SHIFT) + pos.z;
	auto search = _chests.find(key);
	if (search != _chests.end()) {
		search->second->setState(chest_state::OPENING);
	}
}

ChestInstance *Chunk::getChestInstance( glm::ivec3 pos )
{
	int key = ((((pos.x - _startX) << CHUNK_SHIFT) + pos.y - _startY) << WORLD_SHIFT) + pos.z;
	std::map<int, ChestInstance*>::iterator search = _chests.find(key);
	if (search != _chests.end()) {
		return (search->second);
	}
	std::cout << _startX << ", " << _startY << " failed to find chest at " << key << " from " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
	std::cout << "chests values are " << std::endl;
	for (auto& ch: _chests) {
		std::cout << ch.first << std::endl;
	}
	return (NULL);
}

FurnaceInstance *Chunk::getFurnaceInstance( glm::ivec3 pos )
{
	int key = ((((pos.x - _startX) << CHUNK_SHIFT) + pos.y - _startY) << WORLD_SHIFT) + pos.z;
	std::map<int, FurnaceInstance*>::iterator search = _furnaces.find(key);
	if (search != _furnaces.end()) {
		return (search->second);
	}
	std::cout << _startX << ", " << _startY << " failed to find furnace at " << key << " from " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
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

void Chunk::setBlockAt( int value, int posX, int posY, int posZ, bool askNeighbours )
{
	if (!_blocks || posZ < 0 || posZ >= WORLD_HEIGHT || !(value & 0xFF)) {
		return ;
	}
	if (posX < 0) {
		if (askNeighbours && _neighbours[face_dir::MINUSX]) {
			_neighbours[face_dir::MINUSX]->setBlockAt(value, posX + CHUNK_SIZE, posY, posZ, askNeighbours);
		}
	} else if (posX >= CHUNK_SIZE) {
		if (askNeighbours && _neighbours[face_dir::PLUSX]) {
			_neighbours[face_dir::PLUSX]->setBlockAt(value, posX - CHUNK_SIZE, posY, posZ, askNeighbours);
		}
	} else if (posY < 0) {
		if (askNeighbours && _neighbours[face_dir::MINUSY]) {
			_neighbours[face_dir::MINUSY]->setBlockAt(value, posX, posY + CHUNK_SIZE, posZ, askNeighbours);
		}
	} else if (posY >= CHUNK_SIZE) {
		if (askNeighbours && _neighbours[face_dir::PLUSY]) {
			_neighbours[face_dir::PLUSY]->setBlockAt(value, posX, posY - CHUNK_SIZE, posZ, askNeighbours);
		}
	} else {
		int offset = (((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + posZ;
		_blocks[offset] = value;
		_added[offset] = value;
		_removed.erase(offset);
		_vertex_update = true;
	}
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
		if (cnt == 4) {
			for (auto a: _added) { // update torches == block_light spreading, we do it here because before neighbours might not be ready to accept spread
				if ((a.second & 0xFF) == blocks::TORCH) {
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
						if (!(_lights[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] & 0xF000)) {
							int value = _blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level];
							if (s_blocks[value & 0xFF]->transparent) { // underground hole
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
		fill_vertex_array();
		_vaoVIP = false;
		_nb_neighbours = cnt; // do this at end to make sure resetDisplayedFaces is called before any light_update shinanigans
	} else if (cnt < _nb_neighbours) {
		_nb_neighbours = cnt;
	} else if (!cnt) {
		std::cerr << "ERROR chunk has no neighbours" << std::endl;
	}
}

void Chunk::regeneration( bool useInventory, int type, glm::ivec3 pos, Modif modif )
{
	int previous_type = _blocks[(((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z] & 0xFF;
	switch (modif) {
		case Modif::REMOVE:
			if (previous_type == blocks::AIR || previous_type == blocks::BEDROCK) { // can't rm bedrock
				return ;
			}
			// std::cout << "remove_block. displayed before " << _displayed_faces << std::endl;
			remove_block(useInventory, pos);
		break ;
		case Modif::ADD:
			if (type == blocks::WHEAT_CROP && (_blocks[(((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z - 1] & 0xFF) != blocks::FARMLAND) { // can't place crop on something other than farmland
				return ;
			} else if ((previous_type != blocks::AIR && previous_type < blocks::WATER) || type == blocks::AIR) { // can't replace block
				return ;
			}
			// std::cout << "ADD BLOCK " << s_blocks[previous_type]->name << " -> " << s_blocks[type]->name << std::endl;
			add_block(useInventory, pos, type, previous_type);
			break ;
		case Modif::REPLACE:
			if (type == blocks::DIRT_PATH && pos.z < 254 && (_blocks[(((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z + 1] & 0xFF) != blocks::AIR) { // can't turn dirt to dirt path if anything above it
				return ;
			} else if ((type == blocks::DIRT_PATH || type == blocks::FARMLAND) && previous_type != blocks::DIRT && previous_type != blocks::GRASS_BLOCK) {
				return ;
			}
			replace_block(useInventory, pos, type, previous_type);
			break ;
		case Modif::LITNT:
			remove_block(false, pos);
			_entities.push_back(new Entity(this, _inventory, {pos.x + _startX, pos.y + _startY, pos.z}, {0.2f, 0.2f, 0.4f}, true, false, {type, 1, {0, 0}}));
			break ;
		case Modif::USE:
			int offset = (((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z;
			int value = _blocks[offset];
			switch (type) {
			case blocks::OAK_DOOR:
				value ^= (DOOR::OPEN << 12);
				_blocks[offset] = value;
				_added[offset] = value;
				offset += ((value >> 12) & DOOR::UPPER_HALF) ? -1 : 1;
				value ^= (DOOR::UPPER_HALF << 12);
				break ;
			case blocks::OAK_TRAPDOOR:
				value ^= (DOOR::OPEN << 12);
				break ;
			case blocks::LEVER:
				value ^= REDSTONE::POWERED;
				flickLever(pos, value, (value >> REDSTONE::POWERED_OFFSET) & 0x1);
				break ;
			default:
				std::cerr << "Chunk::regeneration case Modif::USE defaulted on: " << s_blocks[value & 0xFF]->name << std::endl;
				return ;
			}
			_blocks[offset] = value;
			_added[offset] = value;
			break ;
	}
	if (type == blocks::WATER || type == blocks::BUCKET) {
		// std::cout << "modif to water with " << ((type == blocks::WATER) ? "water" : "bucket") << std::endl;
		// std::cout << "water count before " << waterSave << ", after " << _water_count << std::endl;
		return ;
	} else if (type == blocks::GLASS || previous_type == blocks::GLASS) {
		sort_water(_camera->getPos(), true);
		return ;
	}
	// std::cout << "_displayed_faces after modif " << _displayed_faces << " at " << _startX << ", " << _startY << std::endl;
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

void Chunk::dropEntity( glm::vec3 dir, t_item item )
{
	glm::vec3 camPos = _camera->getPos();
	camPos.z += 1;
	camPos += dir;
	_entities.push_back(new Entity(this, _inventory, camPos, dir, false, true, item));
}

void Chunk::addParticle( Particle *particle )
{
	_particles.push_back(particle);
}

void Chunk::sort_sky( glm::vec3 &pos, bool vip )
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
				order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY + 0.5f, 196.0f)), {pX, pY + 1, 196, 1, -1, 0, }});
				order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY + 0.5f, 195.0f)), {pX, pY, 195, 1, 1, 0, }});
				if (!_sky[(row - 1) * (CHUNK_SIZE + 2) + col]) {
					order.push_back({dist2(pos, glm::vec3(pX, pY + 0.5f, 195.5f)), {pX, pY + 1, 196, 0, -1, -1, }});
				}
				if (!_sky[(row + 1) * (CHUNK_SIZE + 2) + col]) {
					order.push_back({dist2(pos, glm::vec3(pX + 1, pY + 0.5f, 195.5f)), {pX + 1, pY, 196, 0, 1, -1, }});
				}
				if (!_sky[row * (CHUNK_SIZE + 2) + col - 1]) {
					order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY, 195.5f)), {pX, pY, 196, 1, 0, -1, }});
				}
				if (!_sky[row * (CHUNK_SIZE + 2) + col + 1]) {
					order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY + 1, 195.5f)), {pX + 1, pY + 1, 196, -1, 0, -1, }});
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
			offset0 = start + glm::ivec4(o.second[3], 0, 0, 0);
			offset1 = start + glm::ivec4(0, o.second[4], 0, 0);
			offset2 = start + glm::ivec4(o.second[3], o.second[4], 0, 0);
		} else {
			offset0 = start + glm::ivec4(o.second[3], o.second[4], 0, 0);
			offset1 = start + glm::ivec4(0, 0, o.second[5], 0);
			offset2 = start + glm::ivec4(o.second[3], o.second[4], o.second[5], 0);
		}
		// std::cout << "vindex " << vindex << std::endl;
		_mtx_sky.lock();
		face_water_vertices(_sky_vert, start, offset0, offset1, offset2, vindex);
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

int Chunk::isHit( glm::ivec3 pos )
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
	// if (_thread.joinable()) {
	// 	_thread.join();
	// }
	int type = _blocks[(((chunk_pos.x << CHUNK_SHIFT) + chunk_pos.y) << WORLD_SHIFT) + chunk_pos.z]; // TODO RESTORE & 0xFF;
	if ((type & 0xFF) > blocks::WATER) { // ... TODO AND RM & 0xFF from here
		return (blocks::AIR);
	}
	return (type);
}

void Chunk::handleHit( bool useInventory, int type, glm::ivec3 pos, Modif modif )
{
	glm::ivec3 chunk_pos = {pos.x - _startX, pos.y - _startY, pos.z};
	if (chunk_pos.x < 0) {
		if (_neighbours[face_dir::MINUSX]) {
			return (_neighbours[face_dir::MINUSX]->handleHit(useInventory, type, pos, modif));
		}
	} else if (chunk_pos.x >= CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSX]) {
			return (_neighbours[face_dir::PLUSX]->handleHit(useInventory, type, pos, modif));
		}
	} else if (chunk_pos.y < 0) {
		if (_neighbours[face_dir::MINUSY]) {
			return (_neighbours[face_dir::MINUSY]->handleHit(useInventory, type, pos, modif));
		}
	} else if (chunk_pos.y >= CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSY]) {
			return (_neighbours[face_dir::PLUSY]->handleHit(useInventory, type, pos, modif));
		}
	} else {
		// std::cout << _startX << " " << _startY << ": handle hit at pos " << chunk_pos.x << ", " << chunk_pos.y << ", " << chunk_pos.z << std::endl;
		regeneration(useInventory, type, chunk_pos, modif);
		return ;
	}
	
	std::cout << _startX << ", " << _startY << " ERROR BLOCK OUT OF CHUNK " << chunk_pos.x << ", " << chunk_pos.y << ", " << chunk_pos.z << std::endl;
}

void Chunk::handleBlast( glm::vec3 pos, int blast_radius )
{
	// update current entities
	// items get destroyed if in range
	// falling blocks and tnts get propelled if in ranges
	for (auto e : _entities) {
		e->getBlasted(pos, blast_radius);
	}

	glm::vec3 relative = {pos.x - _startX, pos.y - _startY, pos.z};
	if (relative.x > 0 && relative.x - blast_radius < 0 && _neighbours[face_dir::MINUSX]) {
		_neighbours[face_dir::MINUSX]->handleBlast(pos, blast_radius);
	}
	if (relative.x <= CHUNK_SIZE && relative.x + blast_radius >= CHUNK_SIZE && _neighbours[face_dir::PLUSX]) {
		_neighbours[face_dir::PLUSX]->handleBlast(pos, blast_radius);
	}

	if (relative.y > 0 && relative.y - blast_radius < 0 && _neighbours[face_dir::MINUSY]) {
		_neighbours[face_dir::MINUSY]->handleBlast(pos, blast_radius);
	}
	if (relative.y <= CHUNK_SIZE && relative.y + blast_radius >= CHUNK_SIZE && _neighbours[face_dir::PLUSY]) {
		_neighbours[face_dir::PLUSY]->handleBlast(pos, blast_radius);
	}
}

// explosions from TNT cause a 100% drop rate, otherwise 1/power drop rate
void Chunk::explosion( glm::vec3 pos, int power )
{
	handleBlast(pos, 2 * power);

	// std::cout << "EXPLOSION POWER " << power << std::endl;
	// we launch a ray in each OUTER intersection of a 16x16x16 grid == 1352 rays
	for (int row = 0; row < 16; ++row) {
		for (int col = 0; col < 16; ++col) {
			for (int level = 0; level < 16; ++level) {
				if (!row || !col || !level || row == 15 || col == 15 || level == 15) {
					float intensity = (0.7f + 0.6f * Random::randomFloat(_seed)) * power; // [0.7:1.3] * power
					glm::vec3 end = pos + glm::normalize(glm::vec3(-7.5f + row, -7.5f + col, -7.5f + level)) * intensity;
					std::vector<glm::ivec3> vt = voxel_traversal(pos, end);
					intensity += 0.75f;
					for (auto &p : vt) {
						int type = getBlockAt(p.x - _startX, p.y - _startY, p.z, true) & 0xFF;
						intensity -= 0.75f + s_blocks[type]->blast_resistance;
						if (intensity < 0) {
							break ;
						} else if (type == blocks::TNT) {
							handleHit(false, type, p, Modif::LITNT);
							_entities.back()->setLifetime(3.5 - Random::randomFloat(_seed)); // tnt lit by explosion has lifetime in [0.5;1.5] sec
							// TODO better particles spawning
							_particles.push_back(new Particle(this, {p.x + Random::randomFloat(_seed), p.y + Random::randomFloat(_seed), p.z + Random::randomFloat(_seed)}, PARTICLES::EXPLOSION, Random::randomFloat(_seed)));
						} else if (type != blocks::AIR) {
							// std::cout << "block " << s_blocks[type]->name << " removed" << std::endl;
							handleHit(true, type, p, Modif::REMOVE);
							// handleHit(false, type, p, Modif::REMOVE);
							_particles.push_back(new Particle(this, {p.x + Random::randomFloat(_seed), p.y + Random::randomFloat(_seed), p.z + Random::randomFloat(_seed)}, PARTICLES::EXPLOSION, Random::randomFloat(_seed)));
						}
					}
				}
			}
		}
	}
	// std::cout << "EXPLOSION OVER" << std::endl;
}

void Chunk::shootArrow( float timer )
{
	timer = (timer > 1.0f) ? 20.0f : timer * 20.0f;
	glm::vec3 camPos = _camera->getEyePos(), camDir = _camera->getDir();
	camPos += camDir;
	_entities.push_back(new Entity(this, _inventory, camPos, camDir * timer, true, false, {blocks::ARROW, 1, {0, 0}}));
}

void Chunk::updateBreak( glm::ivec4 block_hit )
{
	if (block_hit.w == blocks::AIR || !_vaoReset) {
		return ;
	}
	glm::ivec3 chunk_pos = {block_hit.x - _startX, block_hit.y - _startY, block_hit.z};
	if (chunk_pos.x < 0 || chunk_pos.x >= CHUNK_SIZE || chunk_pos.y < 0 || chunk_pos.y >= CHUNK_SIZE || chunk_pos.z < 0 || chunk_pos.z > 255) {
		std::cout << "ERROR block hit out of chunk " << chunk_pos.x << ", " << chunk_pos.y << ", " << chunk_pos.z << std::endl;
		return ;
	}
	for (int i = 0; i < 6; ++i) {
		_particles.push_back(new Particle(this, {block_hit.x + 0.5f + (Random::randomFloat(_seed) - 0.5f) * !adj_blocks[i][0] + 0.55f * adj_blocks[i][0], block_hit.y + 0.5f + (Random::randomFloat(_seed) - 0.5f) * !adj_blocks[i][1] + 0.55f * adj_blocks[i][1], block_hit.z + 0.5f + (Random::randomFloat(_seed) - 0.5f) * !adj_blocks[i][2] + 0.55f * adj_blocks[i][2]}, PARTICLES::BREAKING, 0, block_hit.w));
	}
}

// called by neighbour chunk if block change at border
// posX and posY in [0; CHUNK_SIZE + 1] === _blocks compatible
void Chunk::update_border( int posX, int posY, int level, int type, bool adding, int origin )
{
	// std::cout << "got into update border of chunk " << _startX << ", " << _startY << ": " << _displayed_faces << std::endl;
	// std::cout << "args: " << posX << ", " << posY << ", " << level << ": " << ((adding) ? "add" : "rm") << " " << s_blocks[type]->name << " | real pos is " << _startX + posX << ", " << _startY + posY << std::endl;
	if (!(!posX || !posY || posX == CHUNK_SIZE - 1 || posY == CHUNK_SIZE - 1)) {
		std::cout << "ERROR update_border not border block " << posX << ", " << posY << std::endl;
		return ;
	}
	// if (_thread.joinable()) {
	// 	_thread.join();
	// }
	int offset = (((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + level;
	int value = _blocks[offset];
	if (adding) {
		// if (_blocks[offset] && _blocks[offset] < blocks::WATER) {
		// 	std::cout << "\033[31mADDING IS REMOVING BLOCK\033[0m " << s_blocks[_blocks[offset]].name << std::endl;
		// }
		if (type >= blocks::WATER) {
			return ;
		}
		if ((value & 0xFF) == blocks::OAK_FENCE || (value & 0xFF) == blocks::GLASS_PANE) {
			switch (origin) {
				case face_dir::MINUSX:
					_blocks[offset] |= (FENCE::MX << 12);
					_added[offset] |= (FENCE::MX << 12);
					break ;
				case face_dir::PLUSX:
					_blocks[offset] |= (FENCE::PX << 12);
					_added[offset] |= (FENCE::PX << 12);
					break ;
				case face_dir::MINUSY:
					_blocks[offset] |= (FENCE::MY << 12);
					_added[offset] |= (FENCE::MY << 12);
					break ;
				case face_dir::PLUSY:
					_blocks[offset] |= (FENCE::PY << 12);
					_added[offset] |= (FENCE::PY << 12);
					break ;
			}
		}
		if (!air_flower(type, true, true, false) || !air_flower(value, false, false, false)) {
			// std::cout << "took jump" << std::endl;
			goto FILL; // this is only to update face shading TODO only update concerned faces
		}
		if (!face_count(value, posX, posY, level)) { // block no more visible
			_blocks[offset] += blocks::NOTVISIBLE;
		}
	} else {
		// std::cout << "update border, value is " << s_blocks[value & 0xFF]->name << ", origin " << origin << ", orientation " << ((value >> 9) & 0x7) << std::endl;
		if ((value & 0xFF) == blocks::TORCH && ((value >> 9) & 0x7) == origin) {
			remove_block(true, {posX, posY, level}); // TODO handle useInventory instead of setting it to true by default
		}
		if (value & blocks::NOTVISIBLE) {
			_blocks[offset] -= blocks::NOTVISIBLE;
		}
		if ((value & 0xFF) >= blocks::WATER) {
			_hasWater = true;
			_fluids.insert(offset);
		}
		if ((value & 0xFF) == blocks::OAK_FENCE || (value & 0xFF) == blocks::GLASS_PANE) {
			switch (origin) {
				case face_dir::MINUSX:
					_blocks[offset] ^= (FENCE::MX << 12);
					_added[offset] ^= (FENCE::MX << 12);
					break ;
				case face_dir::PLUSX:
					_blocks[offset] ^= (FENCE::PX << 12);
					_added[offset] ^= (FENCE::PX << 12);
					break ;
				case face_dir::MINUSY:
					_blocks[offset] ^= (FENCE::MY << 12);
					_added[offset] ^= (FENCE::MY << 12);
					break ;
				case face_dir::PLUSY:
					_blocks[offset] ^= (FENCE::PY << 12);
					_added[offset] ^= (FENCE::PY << 12);
					break ;
			}
		}
		if (type >= blocks::WATER) {
			return ;
		}
		if (!air_flower(value, false, false, false)) {
			goto FILL;
		}
		// std::cout << s_blocks[_blocks[offset] & 0xFF]->name << " next " << s_blocks[type]->name << std::endl;
	}
	// std::cout << "got here" << std::endl;
	// std::cout << "update border displayed " << _displayed_faces << " at " << _startX << ", " << _startY << std::endl;
	FILL:
	// fill_vertex_array();
	_vertex_update = true;
}

// called by Chunk::collisionBox
// checks if given box collides with given custom hitboxed block
void Chunk::collisionWHitbox( t_collision &res, const Block *target, int value, glm::vec3 pos, float width, float height, int bX, int bY, int bZ )
{
	if (cube_cube_intersection(pos, {width, width, height},
		{bX + target->hitboxCenter.x + _startX, bY + target->hitboxCenter.y + _startY, bZ + target->hitboxCenter.z},
		target->hitboxHalfSize)) {
		if (res.type == COLLISION::NONE) {
			res = {COLLISION::PARTIAL, bZ + target->hitboxCenter.z - target->hitboxHalfSize.z, bZ + target->hitboxCenter.z + target->hitboxHalfSize.z};
		} else {
			res.minZ = glm::min(res.minZ, bZ + target->hitboxCenter.z - target->hitboxHalfSize.z);
			res.maxZ = glm::max(res.maxZ, bZ + target->hitboxCenter.z + target->hitboxHalfSize.z);
		}
	}
	if (target->orientedCollisionHitbox) {
		glm::vec3 hitbox[2];
		target->getSecondaryHitbox(hitbox, (value >> 9) & 0x7, value >> 12);
		if (cube_cube_intersection(pos, {width, width, height},
			{bX + hitbox[0].x + _startX, bY + hitbox[0].y + _startY, bZ + hitbox[0].z},
			hitbox[1])) {
			if (res.type == COLLISION::NONE) {
				res = {COLLISION::PARTIAL, bZ + hitbox[0].z - hitbox[1].z, bZ + hitbox[0].z + hitbox[1].z};
			} else {
				res.minZ = glm::min(res.minZ, bZ + hitbox[0].z - hitbox[1].z);
				res.maxZ = glm::max(res.maxZ, bZ + hitbox[0].z + hitbox[1].z);
			}
		}
	}
}

/* collisionBox takes feet position of object, dimension of its hitbox and returns wether object is inside block or not
 * WATCHOUT if width > 0.5 problemos because we check block left and right but not middle */
t_collision Chunk::collisionBox( glm::vec3 pos, float width, float height, float originalHeight )
{
	t_collision res = {COLLISION::NONE}, recurse_res = {COLLISION::NONE};
	int minX = glm::floor(pos.x - width - _startX);
	int maxX = glm::floor(pos.x + width - _startX);
	int minY = glm::floor(pos.y - width - _startY);
	int maxY = glm::floor(pos.y + width - _startY);
	int top  = glm::floor(pos.z + height);
	int value = getBlockAt(minX, minY, top, true);
	const Block *target = s_blocks[value & 0xFF];
	if (target->collisionHitbox_1x1x1) {
		return {COLLISION::TOTAL, 0, static_cast<float>(top + 1)};
	} else if (target->collisionHitbox) {
		collisionWHitbox(res, target, value, pos, width, originalHeight, minX, minY, top);
	}
	if (minX != maxX) {
		value = getBlockAt(maxX, minY, top, true);
		target = s_blocks[value & 0xFF];
		if (target->collisionHitbox_1x1x1) {
			return {COLLISION::TOTAL, 0, static_cast<float>(top + 1)};
		} else if (target->collisionHitbox) {
			collisionWHitbox(res, target, value, pos, width, originalHeight, maxX, minY, top);
		}
	}
	if (minY != maxY) {
		value = getBlockAt(minX, maxY, top, true);
		target = s_blocks[value & 0xFF];
		if (target->collisionHitbox_1x1x1) {
			return {COLLISION::TOTAL, 0, static_cast<float>(top + 1)};
		} else if (target->collisionHitbox) {
			collisionWHitbox(res, target, value, pos, width, originalHeight, minX, maxY, top);
		}
	}
	if (minX != maxX && minY != maxY) {
		value = getBlockAt(maxX, maxY, top, true);
		target = s_blocks[value & 0xFF];
		if (target->collisionHitbox_1x1x1) {
			return {COLLISION::TOTAL, 0, static_cast<float>(top + 1)};
		} else if (target->collisionHitbox) {
			collisionWHitbox(res, target, value, pos, width, originalHeight, maxX, maxY, top);
		}
	}
	if (height > 1) {
		recurse_res = collisionBox(pos, width, height - 1, originalHeight);
	} else if (top != glm::floor(pos.z)) {
		recurse_res = collisionBox(pos, width, 0, originalHeight);
	}
	if (recurse_res.type == COLLISION::TOTAL) {
		recurse_res.maxZ = glm::max(res.maxZ, recurse_res.maxZ);
		return (recurse_res);
	} else if (recurse_res.type == COLLISION::PARTIAL) {
		if (res.type == COLLISION::NONE) {
			return (recurse_res);
		} else {
			res.minZ = glm::min(res.minZ, recurse_res.minZ);
			res.maxZ = glm::max(res.maxZ, recurse_res.maxZ);
		}
	}
	return (res);
}

/* collisionBoxWater takes feet position of object, dimension of its hitbox and returns wether object is inside water or not
 * WATCHOUT if width > 0.5 problemos because we check block left and right but not middle
 * WATCHOUT if _block with water has bit shifting to represent anything, need to change this function */
bool Chunk::collisionBoxWater( glm::vec3 pos, float width, float height )
{
	glm::ivec3 top0 = {glm::floor(pos.x - width - _startX), glm::floor(pos.y - width - _startY), glm::floor(pos.z + height)};
	if ((getBlockAt(top0.x, top0.y, top0.z, true) & 0xFF) >= blocks::WATER) {
		return (true);
	}
	glm::ivec3 top1 = {glm::floor(pos.x + width - _startX), glm::floor(pos.y - width - _startY), glm::floor(pos.z + height)};
	if (top1 != top0) {
		if ((getBlockAt(top1.x, top1.y, top1.z, true) & 0xFF) >= blocks::WATER) {
			return (true);
		}
	}
	glm::ivec3 top2 = {glm::floor(pos.x + width - _startX), glm::floor(pos.y + width - _startY), glm::floor(pos.z + height)};
	if (top2 != top0) {
		if ((getBlockAt(top2.x, top2.y, top2.z, true) & 0xFF) >= blocks::WATER) {
			return (true);
		}
	}
	glm::ivec3 top3 = {glm::floor(pos.x - width - _startX), glm::floor(pos.y + width - _startY), glm::floor(pos.z + height)};
	if (top3 != top0) {
		if ((getBlockAt(top3.x, top3.y, top3.z, true) & 0xFF) >= blocks::WATER) {
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
	t_collision coll;
	if (distZ < 0) { // jumping
		_camera->_touchGround = false;
		float hitboxHeight = _camera->getHitBox();
		coll = collisionBox({pos.x, pos.y, pos.z + hitboxHeight}, 0.3f, -distZ, -distZ);
		if (coll.type == COLLISION::TOTAL) {
			_camera->touchCeiling(glm::floor(pos.z) + 0.19f);
			// std::cout << "hit roof, " << pos.z << " -> " << _camera->getPos().z << std::endl;
			return ;
		} else if (coll.type == COLLISION::PARTIAL) {
			_camera->touchCeiling(coll.minZ - hitboxHeight);
			return ;
		}
	} else { // falling
		coll = collisionBox(pos, 0.3f, distZ, distZ);
		if (coll.type != COLLISION::NONE) {
			_camera->touchGround(coll.maxZ);
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
		// std::cout << "over." << std::endl;
	} else if (_vertex_update && _nb_neighbours == 4) {
		// std::cout << _startX << ", " << _startY << " vertex update (prob crop)" << std::endl;
		fill_vertex_array();
	}
	if (!_vaoReset) { // TODO change vaoReset logic (swap true and false)
		++counter;
		if (!_vaoVIP && (counter & 63) > 6) { // we don't load more than 5 new chunks per 50 new chunks per frame
		// std::cout << "skip one" << std::endl;
			return ;
		}
		// std::cout << "chunk reset " << _startX << ", " << _startY << std::endl;
		setup_array_buffer();
	}
    glBindVertexArray(_vao); // this is the costly operation, chunk_size up == fps down
	glDrawArrays(GL_TRIANGLES, 0, _displayed_faces * 6);
	face_counter += _displayed_faces;
}

void Chunk::updateFurnaces( double currentTime )
{
	for (auto& fur: _furnaces) {
		int state = fur.second->updateTimes(currentTime);
		if (state != furnace_state::NOCHANGE) {
			// std::cout << "FURNACE STATE CHANGE TO " << state << std::endl;
			int value = _blocks[fur.first];
			_blocks[fur.first] = (value & 0xFFFFEFFF) + ((state == furnace_state::ON) << 12);
			// set/unset furnace position as light source of 13 using state
			int posZ = fur.first & (WORLD_HEIGHT - 1);
			int posY = ((fur.first >> WORLD_SHIFT) & (CHUNK_SIZE - 1));
			int posX = ((fur.first >> WORLD_SHIFT) >> CHUNK_SHIFT);
			// std::cout << "furnace at " << _startX + posX << ", " << _startY + posY << ", " << posZ << std::endl;
			if (state == furnace_state::ON) {
				_lights[fur.first] = 13 + (13 << 4);
				light_spread(posX, posY, posZ, false); // spread block light
			} else {
				_lights[fur.first] = 0;
				for (int index = 0; index < 6; index++) {
					const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
					light_try_spread(posX + delta[0], posY + delta[1], posZ + delta[2], 0, false, LIGHT_RECURSE);
				}
			}
			_light_update = true;
		}
	}
}

void Chunk::updateEntities( std::vector<t_shaderInput> &arr,  std::vector<t_shaderInput> &partArr, double deltaTime )
{
	// TODO merge identical close(3/4 of a block) stackable items together
	// 			on merge, item timer set to longest of 2
	// 			double for loop, use bool given as parameter to do this once per second at most

	// following code not working with tnt explosion, because reallocation on _entities.push_back and iterators are invalidated -> erase(e) causes segfault
	// for (auto e = _entities.begin(); e != _entities.end();) {
	// 	std::cout << "debug 0, enti size " << _entities.size() << std::endl;
	// 	if (e->update(arr, _camera->getPos(), deltaTime)) {
	// 	std::cout << "debug 1, enti size " << _entities.size() << std::endl;
	// 		e = _entities.erase(e);
	// 	} else {
	// 		++e;
	// 	}
	// }

	glm::vec3 camPos;
	size_t size = _entities.size();
	if (!size) {
		goto CHESTS;
	}

	camPos = _camera->getPos();
	for (int index = size - 1; index >= 0; --index) {
		if (_entities[index]->update(arr, partArr, camPos, deltaTime)) {
			delete _entities[index];
			_entities.erase(_entities.begin() + index);
		}
	}

	CHESTS:
	for (auto &c : _chests) {
		c.second->update(arr, deltaTime);
	}
}

size_t Chunk::clearEntities( void )
{
	size_t res = _entities.size();
	for (auto e : _entities) {
		delete e;
	}
	_entities.clear();
	return (res);
}

void Chunk::updateParticles( std::vector<t_shaderInput> &arr, double deltaTime )
{
	glm::vec3 camPos = _camera->getPos(), camDir = _camera->getDir();
	size_t size = _particles.size();
	if (!size) {
		goto FLAMES;
	}

	for (int index = size - 1; index >= 0; --index) {
		if (_particles[index]->update(arr, camPos, camDir, deltaTime)) {
			delete _particles[index];
			_particles.erase(_particles.begin() + index);
		}
	}

	FLAMES:
	for (auto &f : _flames) {
		f.second->update(arr, camPos, camDir, deltaTime);
	}
}

size_t Chunk::clearParticles( void )
{
	size_t res = _particles.size();
	for (auto p : _particles) {
		delete p;
	}
	_particles.clear();
	return (res);
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
