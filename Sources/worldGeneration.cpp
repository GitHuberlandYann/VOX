#include "Chunk.hpp"
#include "random.hpp"
#include "Settings.hpp"

extern siv::PerlinNoise::seed_type perlin_seed;

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

/**
 * @brief recursively generate ore blob of given type and size
 * @param ore_type type contained by ore_blob
 * @param row, col, level coordinates of current block in blob
 * @param blob_size nbr of ores in blob
 * @param dir used to randomize generation, alternates in all directions
*/
void Chunk::gen_ore_blob( int ore_type, int row, int col, int level, int &blob_size, int dir )
{
	if (row == -1 || col == -1 || !level || !blob_size
		|| row == CHUNK_SIZE || col == CHUNK_SIZE || level == WORLD_HEIGHT
		|| _blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] != blocks::stone) {
		return ;
	}
	_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = ore_type;
	--blob_size;
	for (int i = 0; i < 6; i++) {
		if (dir == 6) {
			dir = 0;
		}
		const glm::ivec3 delta = adj_blocks[dir];
		++dir;
		gen_ore_blob(ore_type, row + delta.x, col + delta.y, level + delta.z, blob_size, dir);
	}
}

/**
 * @brief check if given block is exposed to air, used to hide unseen blocks
 * @param row, col, level coordinates of block being checked
 * @param isNotLeaves, isNotGlass literaly whether block being checked is leaves/glass
 * @return block should not be hidden
*/
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

/**
 * @brief determines block proceduraly generated at given pos from several inputs
 * @param perlin used to generate variation in underground blocks
 * @param row, col, level coordinates of block being generated
 * @param surface_level computed level at which blocks become air
 * @param poppy, dandelion, blue_orchid, allium, cornflower, pink_tulip, grass, tree_gen inputs to gen block
 * @param trees vector of tree pos later used to generate leaves
 * @return block type to generate
*/
int Chunk::get_block_type( siv::PerlinNoise perlin, int row, int col, int level, int surface_level,
	bool poppy, bool dandelion, bool blue_orchid, bool allium, bool cornflower, bool pink_tulip,
	bool grass, bool tree_gen, std::vector<glm::ivec3> &trees )
{
	if (level == 0) {
		return (blocks::bedrock);
	}
	int value = (level > surface_level) ? blocks::air : (level < surface_level - 2 || level < SEA_LEVEL) ? blocks::stone : blocks::grass_block;
	if (value) {
		if (_continent == cont::CONT_MUSHROOM_FIELDS) {
			return (blocks::grass_block);
			// return (blocks::diamond_ore);
		}
		if (value == blocks::stone) {
			double undergound = perlin.noise3D_01((_startX + row) / 100.0f, (_startY + col) / 100.0f, level / 100.0f);
			(undergound < 0.55) ? : value = blocks::gravel;//blocks::sand;
		} else if (value == blocks::grass_block && _continent <= cont::CONT_COAST) {
			value = blocks::sand;
		}
	} else if (level <= SEA_LEVEL) {
		value = blocks::water;
	} else if (_continent <= cont::CONT_COAST) {
		if (surface_level >= SEA_LEVEL && level <= surface_level + 3) {
			if (tree_gen) {
				value = blocks::cactus;
			} else if (dandelion && surface_level == SEA_LEVEL) {
				value = blocks::sugar_cane;
			} else if (level == surface_level + 1) {
				if (poppy) {
					value = blocks::dead_bush;
				}
			}
		}
	} else if (tree_gen && surface_level > 65 && level <= surface_level + 5 && surface_level < 220) {
		value = blocks::oak_log + (face_dir::plus_z << offset::blocks::orientation);
		if (level == surface_level + 5) {
			trees.push_back({row, col, level});
		}
	} else if (level == surface_level + 1 && surface_level > 65 && surface_level < 220) {
		if (poppy) {
			value = blocks::poppy;
		} else if (dandelion) {
			value = blocks::dandelion;
		} else if (blue_orchid) {
			value = blocks::blue_orchid;
		} else if (allium) {
			value = blocks::allium;
		} else if (cornflower) {
			value = blocks::cornflower;
		} else if (pink_tulip) {
			value = blocks::pink_tulip;
		} else if (grass) {
			value = blocks::grass;
		}
	}
	return (value);
}

/**
 * @brief computes level from which blocks become air from given x, y position.
 * This is done by using perlin noise
 * @param row, col x and y coordinates
 * @param perlin library instance used to gen perlin noise
 * @return level that block->air
*/
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

/**
 * @brief fills _blocks array with Setttings->getInt(flat_world_block) up to z level 10, rest is air
*/
void Chunk::generate_flat_world( void )
{
	int value = Settings::Get()->getInt(settings::ints::flat_world_block);
	int surface_level = 10;
	for (int row = 0; row < CHUNK_SIZE; row++) {
		for (int col = 0; col < CHUNK_SIZE; col++) {
			for (int level = 0; level < WORLD_HEIGHT; level++) {
				_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = (level > surface_level) ? blocks::air
						: (level == 0) ? blocks::bedrock : value;
			}
		}
	}

	for (auto it = _removed.begin(); it != _removed.end();) {
		if (_blocks[*it] == blocks::air) {
			it = _removed.erase(it);
		} else {
			_blocks[*it] = blocks::air;
			++it;
		}
	}
	for (auto it = _added.begin(); it != _added.end();) {
		if (_blocks[it->first] == it->second) {
			it = _added.erase(it);
		} else {
			// if (it->second & mask::adventure_block) {
			// 	std::cout << "ADVENTURE BLOCK FOUND IN MAP: " << s_blocks[it->second & mask::blocks::type]->name << " at " << _startX + ((it->first >> WORLD_SHIFT) >> CHUNK_SHIFT) << ", " << _startY + ((it->first >> WORLD_SHIFT) & (CHUNK_SIZE - 1)) << ", " << (it->first & (WORLD_HEIGHT - 1)) << std::endl;
			// 	// it->second &= (mask::all_bits - mask::adventure_block);
			// 	// it->second |= mask::redstone::piston::moving;
			// }
			_blocks[it->first] = it->second;
			if ((it->second & mask::blocks::type) == blocks::torch) {
				addFlame(it->first, {((it->first >> WORLD_SHIFT) >> CHUNK_SHIFT), ((it->first >> WORLD_SHIFT) & (CHUNK_SIZE - 1)), (it->first & (WORLD_HEIGHT - 1))}, blocks::torch, (it->second >> offset::blocks::orientation) & 0x7);
			}
			++it;
		}
	}
}

/**
 * @brief fills _blocks with proceduraly generated terrain of 'normal' world settings
*/
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
					? _blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = blocks::air//get_block_type_cave(row, col, level, ground_cave, poppy, dandelion, blue_orchid, allium, cornflower, pink_tulip, grass, tree_gen, trees)
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
			if (_blocks[((((tree.x + delta[0]) << CHUNK_SHIFT) + tree.y + delta[1]) << WORLD_SHIFT) + tree.z + delta[2]] == blocks::air) {
				_blocks[((((tree.x + delta[0]) << CHUNK_SHIFT) + tree.y + delta[1]) << WORLD_SHIFT) + tree.z + delta[2]] = blocks::oak_leaves + mask::leaves::natural;
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
				if (value == blocks::stone) {
					if (coal_blobs < 20 && Random::rangedNumber(_seed, 0, 1000) <= 1) {
						int size = (Random::rangedNumber(_seed, 0, 1000) + 3) / 30;
						gen_ore_blob(blocks::coal_ore, row, col, level, size, 0);
						coal_blobs++;
					} else if (iron_blobs < 10 && (level <= 56 || level > 80) && Random::rangedNumber(_seed, 0, 1000) <= 1) {
						int size = (Random::rangedNumber(_seed, 0, 1000) + 3) / 60;
						gen_ore_blob(blocks::iron_ore, row, col, level, size, 0);
						iron_blobs++;
					} else if (!gravel_blob && Random::rangedNumber(_seed, 0, 1000) <= 1) {
						int size = 60;
						gen_ore_blob(blocks::gravel, row, col, level, size, 0);
						gravel_blob = true;
					} else if (diamond_blobs < 7 && level < 15 && Random::rangedNumber(_seed, 0, 1000) <= 1) {
						int size = (Random::rangedNumber(_seed, 0, 1000) + 5) / 200;
						gen_ore_blob(blocks::diamond_ore, row, col, level, size, 0);
						diamond_blobs++;
					}
				}
				// turning grass into dirt if under other block
				if (value == blocks::grass_block && level != 255) {
					int block_above = _blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level + 1];
					if (block_above && block_above < blocks::poppy) {
						_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = blocks::dirt;
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
					if (value >= blocks::water) {
						// do nothing
					} else if (exposed_block(row, col, level, value != blocks::oak_leaves, value != blocks::glass)) {
						if (value == blocks::iron_ore && Random::rangedNumber(_seed, 0, 1000) < 500) {
							_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = blocks::stone;
						} else if (value == blocks::gravel && Random::rangedNumber(_seed, 0, 1000) < 100) {
							_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = blocks::stone;
						} else if (value == blocks::diamond_ore && Random::rangedNumber(_seed, 0, 1000) < 900) {
							_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = blocks::stone;
						} else if (value == blocks::coal_ore && Random::rangedNumber(_seed, 0, 1000) < 200) {
							_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] = blocks::stone;
						}
					}
				}
			}
		}
	}

	// loading backup, MUST be done after all randomness to not alter anything
	for (auto r: _removed) {
		_blocks[r] = blocks::air;
	}
	for (auto a: _added) {
		_blocks[a.first] = a.second;
		if ((a.second & mask::blocks::type) == blocks::torch) {
			addFlame(a.first, {((a.first >> WORLD_SHIFT) >> CHUNK_SHIFT), ((a.first >> WORLD_SHIFT) & (CHUNK_SIZE - 1)), (a.first & (WORLD_HEIGHT - 1))}, blocks::torch, (a.second >> offset::blocks::orientation) & 0x7);
			// _flames.emplace(a.first, new Particle(this, {((a.first >> WORLD_SHIFT) >> CHUNK_SHIFT) + _startX + 0.5f, ((a.first >> WORLD_SHIFT) & (CHUNK_SIZE - 1)) + _startY + 0.5f, (a.first & (WORLD_HEIGHT - 1)) + 10.0f / 16.0f + 0.1f}, PARTICLES::FLAME));
		}
	}
	// b.stamp("rest");
	// b.stop("blocks");
}

/**
 * @brief generate 18x18 sky to avoid call to neighbours 
*/
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

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

/**
 * @brief std::thread can't take a class' method as argument, so we call this function instead
*/
static void thread_setup_chunk( Chunk *current )
{
	current->generation();
}

/**
 * @brief allocates and fills in _blocks, _lights, and _sky arrays with proceduraly generated world
*/
void Chunk::generation( void )
{
	// Bench b;
	if (Settings::Get()->getBool(settings::bools::flat_world)) {
		generate_flat_world();
	} else {
		generate_blocks();
	}
	// b.stamp("blocks");
	generate_lights();
	// b.stamp("lights");
	_genDone = true;
	_mtx_sky.lock();
	generate_sky();
	_mtx_sky.unlock();
	// b.stamp("sky");
	// b.stop("generation");
}

/**
 * @brief launches thread to generate world
*/
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
