#include "vox.h"

Chunk::Chunk( Camera *camera, int posX, int posY ) : _isVisible(true), _vaoSet(false), _vaoReset(false), _startX(posX), _startY(posY),
	_blocks(NULL), _vertices(NULL), _displayed_blocks(0), _camera(camera)
{
}

Chunk::~Chunk( void )
{
	if (_thread.joinable()) {
		_thread.join();
	}

	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);

	delete [] _blocks;
	delete [] _vertices;

	_orientations.clear();
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

/* * we simulate that flowers are air block in order to display adjacent blocks properly
   * also use this for all 'see-through' blocks like leaves, water..*/
static int air_flower( int value, bool air_leaves )
{
	if (value >= blocks::POPPY || value == blocks::CACTUS || (air_leaves && value == blocks::OAK_LEAVES)) {
		return (0);
	}
	return (value);
}

GLint Chunk::get_empty_faces( int type, int row, int col, int level, bool isNotLeaves )
{
	GLint res = !!air_flower(_blocks[((row - 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], isNotLeaves) * (1 << 2)
				+ !!air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], isNotLeaves) * (1 << 3)
				+ !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col - 1) * WORLD_HEIGHT + level], isNotLeaves) * (1 << 0)
				+ !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level], isNotLeaves) * (1 << 1);
	switch (level) {
		case 0:
			res += (1 << 5);
			res += !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1], isNotLeaves) * (1 << 4);
			break ;
		case 255:
			res += !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], isNotLeaves) * (1 << 5);
			// res += (1 << 4); // don't wan't to hide face from above, because we can be at pos 257 and looking at block 255
			break ;
		default:
			res += !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], isNotLeaves) * (1 << 5);
			res += !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1], isNotLeaves) * (1 << 4);
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
			above = !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1], isNotLeaves);
			break ;
		case 255:
			below = !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], isNotLeaves);
			above = true;
			break ;
		default:
			below = !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], isNotLeaves);
			above = !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1], isNotLeaves);
	}
	return (!air_flower(_blocks[((row - 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], isNotLeaves)
		|| !air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], isNotLeaves)
		|| !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col - 1) * WORLD_HEIGHT + level], isNotLeaves)
		|| !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level], isNotLeaves)
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
			return (DIAMOND_ORE);
		}
		if (value == blocks::STONE) {
			double undergound = perlin.noise3D_01((_startX - 1 + row) / 100.0f, (_startY - 1 + col) / 100.0f, level / 100.0f);
			(undergound < 0.55) ? : value = blocks::GRAVEL;//blocks::SAND;
		} else if (value == blocks::GRASS_BLOCK && _continent <= cont::CONT_COAST) {
			value = blocks::SAND;
		}
	} else if (level <= SEA_LEVEL) {
		// value = blocks::WATER;
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
	std::vector<glm::ivec3>::iterator it = trees.begin();
	for (; it != trees.end(); it++) {
		for (int index = 0; index < 61; index++) {
			const GLint delta[3] = {oak_normal[index][0], oak_normal[index][1], oak_normal[index][2]};
			if (_blocks[(((*it).x + delta[0]) * (CHUNK_SIZE + 2) + (*it).y + delta[1]) * WORLD_HEIGHT + (*it).z + delta[2]] == blocks::AIR) {
				_blocks[(((*it).x + delta[0]) * (CHUNK_SIZE + 2) + (*it).y + delta[1]) * WORLD_HEIGHT + (*it).z + delta[2]] = blocks::OAK_LEAVES;
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


	// hiding unseen blocks
	for (int row = 1; row < CHUNK_SIZE + 1; row++) {
		for (int col = 1; col < CHUNK_SIZE + 1; col++) {
			for (int level = 0; level < WORLD_HEIGHT; level++) {
				GLint value = _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level];
				if (value) {
					if (value == blocks::WATER) {
						// do nothing
					} else if (exposed_block(row, col, level, value != blocks::OAK_LEAVES)) {
						_displayed_blocks++;
						// reverting ores back to stone if they are exposed
						if (value == blocks::IRON_ORE && distribution(generator) < 500) {
							_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = blocks::STONE;
						} else if (value == blocks::GRAVEL && distribution(generator) < 100) {
							_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = blocks::STONE;
						} else if (value == blocks::DIAMOND_ORE && distribution(generator) < 900) {
							_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = blocks::STONE;
						} else if (value == blocks::COAL_ORE && distribution(generator) < 200) {
							_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = blocks::STONE;
						} 
					} else {
						_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] -= blocks::NOTVISIBLE;
					}
				}
			}
		}
	}
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

static bool isSandOrGravel( int type )
{
	type += blocks::NOTVISIBLE * (type < blocks::AIR);
	return (type == blocks::SAND || type == blocks::GRAVEL);
}

void Chunk::handle_border_block( glm::ivec3 pos, int type, bool adding )
{
	if (type == blocks::AIR) {
		return ;
	}
	if (!pos.x) {
		// std::cout << std::endl << _startX << ", " << _startY << ": at " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
		// std::cout << "pos.x is 0!! gogo neighbour" << std::endl;
		for (auto& c : _vis_chunks) {
			if (c->isInChunk(_startX - CHUNK_SIZE, _startY)) {
				c->update_border(CHUNK_SIZE + 1, pos.y + 1, pos.z, adding);
				break ;
			}
		}
	} else if (pos.x == CHUNK_SIZE - 1) {
		// std::cout << std::endl << _startX << ", " << _startY << ": at " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
		// std::cout << "pos.x is 15 gogo neighbour" << std::endl;
		for (auto& c : _vis_chunks) {
			if (c->isInChunk(_startX + CHUNK_SIZE, _startY)) {
				c->update_border(0, pos.y + 1, pos.z, adding);
				break ;
			}
		}
	}
	if (!pos.y) {
		// std::cout << std::endl << _startX << ", " << _startY << ": at " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
		// std::cout << "pos.y is 0!! gogo neighbour" << std::endl;
		for (auto& c : _vis_chunks) {
			if (c->isInChunk(_startX, _startY - CHUNK_SIZE)) {
				c->update_border(pos.x + 1, CHUNK_SIZE + 1, pos.z, adding);
				break ;
			}
		}
	} else if (pos.y == CHUNK_SIZE - 1) {
		// std::cout << std::endl << _startX << ", " << _startY << ": at " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
		// std::cout << "pos.y is 15 gogo neighbour" << std::endl;
		for (auto& c : _vis_chunks) {
			if (c->isInChunk(_startX, _startY + CHUNK_SIZE)) {
				c->update_border(pos.x + 1, 0, pos.z, adding);
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
			if (block_above < 0 && value > 0) {
				_mtx.lock();
				++_displayed_blocks;
				_mtx.unlock();
			}
			return (remove_block(NULL, glm::ivec3(pos.x, pos.y, pos.z + 1)));
		} else {
			add_block(NULL, glm::ivec3(pos.x, pos.y, endZ), above_value);
		}
	}
	if (value > blocks::AIR) { // if invisible block gets deleted, same amount of displayed_blocks
		_mtx.lock();
		_displayed_blocks--;
		_mtx.unlock();
	}
	handle_border_block(pos, air_flower(value, true), false); // if block at border of chunk gets deleted, we update neighbours
	_blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z] = blocks::AIR;
	for (int index = 0; index < 6; index++) {
		const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
		if (pos.x + delta[0] < 0 || pos.x + delta[0] >= CHUNK_SIZE || pos.y + delta[1] < 0 || pos.y + delta[1] >= CHUNK_SIZE || pos.z + delta[2] < 0 || pos.z + delta[2] > 255) {

		} else {
			int adj = _blocks[((pos.x + delta[0] + 1) * (CHUNK_SIZE + 2) + pos.y + delta[1] + 1) * WORLD_HEIGHT + pos.z + delta[2]];
			if (adj < blocks::AIR) {
				_blocks[((pos.x + delta[0] + 1) * (CHUNK_SIZE + 2) + pos.y + delta[1] + 1) * WORLD_HEIGHT + pos.z + delta[2]] += blocks::NOTVISIBLE;
				_mtx.lock();
				_displayed_blocks++;
				_mtx.unlock();
			}
		}
	}
	if (endZ != -1) { // sand fall
		remove_block(NULL, glm::ivec3(pos.x, pos.y, pos.z + 1));
	} else if (pos.z < 255 && block_above >= blocks::POPPY && block_above < blocks::WATER) { // del flower if block underneath deleted
		(block_above == blocks::GRASS)
			? remove_block(NULL, glm::ivec3(pos.x, pos.y, pos.z + 1)) // if grass above breaked block, don't collect it
			: remove_block(inventory, glm::ivec3(pos.x, pos.y, pos.z + 1));
	}
	// std::cout << "nb displayed blocks after: " << _displayed_blocks << std::endl;
}

void Chunk::add_block( Inventory *inventory, glm::ivec3 pos, int type )
{
	// std::cout << "in chunk " << _startX << ", " << _startY << ":rm block " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
	// std::cout << "nb displayed blocks before: " << _displayed_blocks << std::endl;
	if (type >= blocks::POPPY && pos.z - 1 > 0) {
		int block_below = _blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z - 1];
		if (block_below != blocks::GRASS_BLOCK && block_below != blocks::DIRT && block_below != blocks::SAND) {
			return ; // can't place flower on something else than grass/dirt block
		}
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
	_blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z] = type;
	_mtx.lock();
	_displayed_blocks++;
	_mtx.unlock();
	for (int index = 0; index < 6; index++) {
		const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
		if (pos.x + delta[0] < 0 || pos.x + delta[0] >= CHUNK_SIZE || pos.y + delta[1] < 0 || pos.y + delta[1] >= CHUNK_SIZE || pos.z + delta[2] < 0 || pos.z + delta[2] > 255) {

		} else {
			GLint value = _blocks[((pos.x + delta[0] + 1) * (CHUNK_SIZE + 2) + pos.y + delta[1] + 1) * WORLD_HEIGHT + pos.z + delta[2]];
			if (value > blocks::AIR && !exposed_block(pos.x + delta[0] + 1, pos.y + delta[1] + 1, pos.z + delta[2], _blocks[((pos.x + delta[0] + 1) * (CHUNK_SIZE + 2) + pos.y + delta[1] + 1) * WORLD_HEIGHT + pos.z + delta[2] + 1] != blocks::OAK_LEAVES)) {
				// was exposed before, but isn't anymore
				_blocks[((pos.x + delta[0] + 1) * (CHUNK_SIZE + 2) + pos.y + delta[1] + 1) * WORLD_HEIGHT + pos.z + delta[2]] -= blocks::NOTVISIBLE;
				_mtx.lock();
				_displayed_blocks--;
				_mtx.unlock();
			}
		}
	}
	// std::cout << "nb displayed blocks after: " << _displayed_blocks << std::endl;
}

void Chunk::fill_vertex_array( void )
{
	int index = 0;
	for (int row = 0; row < CHUNK_SIZE; row++) {
		for (int col = 0; col < CHUNK_SIZE; col++) {
			for (int level = 0; level < WORLD_HEIGHT; level++) {
				GLint block_type = _blocks[((row + 1) * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level];
				if (block_type > blocks::AIR && block_type != blocks::WATER) {
					// if (index + 5 > _displayed_blocks * 5) {
					// 	std::cout << "ERROR index is " << index / 5 << std::endl;
					// }
					_vertices[index] = block_type;
					_vertices[index + 1] = 0;
					_vertices[index + 2] = get_empty_faces(block_type, row + 1, col + 1, level, block_type != blocks::OAK_LEAVES);
					_vertices[index + 3] = _startX + row;
					_vertices[index + 4] = _startY + col;
					_vertices[index + 5] = level;
					index += 6;
				}
			}
		}
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
	glBufferData(GL_ARRAY_BUFFER, _displayed_blocks * 6 * sizeof(GLint), _vertices, GL_STATIC_DRAW);

	// delete [] _vertices;
	// _vertices = NULL;
	_vaoReset = true;

	glEnableVertexAttribArray(BLOCKATTRIB);
	glVertexAttribIPointer(BLOCKATTRIB, 1, GL_INT, 6 * sizeof(GLint), 0);
	
	glEnableVertexAttribArray(BREAKATTRIB);
	glVertexAttribIPointer(BREAKATTRIB, 1, GL_INT, 6 * sizeof(GLint), (void *)(sizeof(GLint)));
	
	glEnableVertexAttribArray(ADJATTRIB);
	glVertexAttribIPointer(ADJATTRIB, 1, GL_INT, 6 * sizeof(GLint), (void *)(2 * sizeof(GLint)));

	glEnableVertexAttribArray(POSATTRIB);
	glVertexAttribIPointer(POSATTRIB, 3, GL_INT, 6 * sizeof(GLint), (void *)(3 * sizeof(GLint)));

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
	_vertices = new GLint[_displayed_blocks * 6]; // blocktype, adjacents blocks, X Y Z
	fill_vertex_array();
}

void Chunk::regeneration( Inventory *inventory, int type, glm::ivec3 pos, bool adding )
{
	if (!adding) {
		if (_blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z] == blocks::BEDROCK) { // can't rm bedrock
			return ;
		}
		remove_block(inventory, pos);
	} else {
		int value = _blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z];
		if ((value != blocks::AIR && value != blocks::WATER) || type == blocks::AIR) { // can't replace block
			return ;
		}
		add_block(inventory, pos, type);
	}
	delete [] _vertices;
	_vertices = new GLint[_displayed_blocks * 6]; // blocktype, adjacents blocks, X Y Z
	fill_vertex_array();
	_mtx.lock();
	_vaoReset = false;
	_mtx.unlock();
}

void Chunk::generate_chunk( std::list<Chunk *> *chunks )
{
	if (_thread.joinable()) {
		_thread.join();
	}
	_thread = std::thread(thread_setup_chunk, chunks, this);
}

bool Chunk::inPerimeter( int posX, int posY, GLint render_dist )
{
	return (_startX >= posX - render_dist && _startX <= posX + render_dist
			&& _startY >= posY - render_dist && _startY <= posY + render_dist);
}

void Chunk::setVisibility( std::list<Chunk *> *visible_chunks, int posX, int posY, GLint render_dist )
{
	_isVisible = inPerimeter(posX, posY, render_dist);
	if (_isVisible) {
		visible_chunks->push_back(this);
	}
}

void Chunk::hide( void )
{
	_isVisible = false;
}

bool Chunk::shouldDelete( glm::vec3 pos, GLfloat dist )
{
	if (_isVisible) {
		return (false);
	}
	if (dist < CHUNK_SIZE * 5) {
		dist = CHUNK_SIZE * 5;
	}
	return (glm::distance(glm::vec2(pos.x, pos.y), glm::vec2(_startX, _startY)) > dist);
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
	if (_thread.joinable()) {
		_thread.join();
	}
	int value = _blocks[((chunk_pos.x + 1) * (CHUNK_SIZE + 2) + chunk_pos.y + 1) * WORLD_HEIGHT + chunk_pos.z];
	if (value == blocks::WATER) {
		return (blocks::AIR);
	}
	return (value);
}

static void thread_modif_block( Chunk *current, Inventory *inventory, int type, glm::ivec3 pos, bool adding )
{
	current->regeneration(inventory, type, pos, adding);
}

void Chunk::handleHit( Inventory *inventory, int type, glm::ivec3 pos, bool adding, std::list<Chunk *> vis_chunks )
{

	glm::ivec3 chunk_pos = glm::ivec3(pos.x - _startX, pos.y - _startY, pos.z);
	// std::cout << "handle hit at pos " << chunk_pos.x << ", " << chunk_pos.y << ", " << chunk_pos.z << std::endl;
	// if (chunk_pos.x < 0 || chunk_pos.x >= CHUNK_SIZE || chunk_pos.y < 0 || chunk_pos.y >= CHUNK_SIZE || chunk_pos.z < 0 || chunk_pos.z > 255) {
	// 	std::cout << "ERROR BLOCK OUT OF CHUNK " << chunk_pos.x << ", " << chunk_pos.y << ", " << chunk_pos.z << std::endl;
	// }
	if (_thread.joinable()) {
		_thread.join();
	}
	_vis_chunks = vis_chunks;
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
	for (int index = 0; index < _displayed_blocks * 6; index += 6) {
		_mtx.lock();
		if (_vertices[index + 5] == block_hit.z && _vertices[index + 3] == block_hit.x && _vertices[index + 4] == block_hit.y) {
			_vaoReset = false;
			_vertices[index + 1] = frame;
			_mtx.unlock();
			return ;
		}
		_mtx.unlock();
	}
}

// called by neighbour chunk if block change at border
// posX and posY in [0; CHUNK_SIZE + 1] === _blocks compatible
void Chunk::update_border(int posX, int posY, int level, bool adding)
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
	if (adding) {
		_blocks[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + level] = blocks::STONE;
		// TODO hide block if no longer visible
	} else {
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
		if (exposed_block(target.x, target.y, level, true)) { // block was already exposed, no change in displayed_blocks, only in visible faces
			// std::cout << "displayed blocks same" << std::endl;
			_blocks[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + level] = blocks::AIR;
			for (int index = 0; index < _displayed_blocks * 6; index += 6) {
				_mtx.lock();
				if (_vertices[index + 3] == _startX + target.x - 1 && _vertices[index + 4] == _startY + target.y - 1 && _vertices[index + 5] == level) {
					_vertices[index + 2] = get_empty_faces(_blocks[(target.x * (CHUNK_SIZE + 2) + target.y) * WORLD_HEIGHT + level], target.x, target.y, level, true);
					_vaoReset = false;
					_mtx.unlock();
					return ;
				}
				_mtx.unlock();
			}
		} else { // we need to redo it all because one new exposed block
			// std::cout << "redo it all" << std::endl;
			_blocks[(target.x * (CHUNK_SIZE + 2) + target.y) * WORLD_HEIGHT + level] += blocks::NOTVISIBLE;
			_blocks[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + level] = blocks::AIR;
			delete [] _vertices;
			_displayed_blocks++;
			_vertices = new GLint[_displayed_blocks * 6]; // blocktype, adjacents blocks, X Y Z
			fill_vertex_array();
			_mtx.lock();
			_vaoReset = false;
			_mtx.unlock();
		}
	}
}

bool Chunk::collision( glm::vec3 &pos, Camera &cam )
{
	glm::ivec3 ipos = glm::ivec3(pos.x - _startX, pos.y - _startY, pos.z);
	if (ipos.x < 0 || ipos.x >= CHUNK_SIZE || ipos.y < 0 || ipos.y >= CHUNK_SIZE || ipos.z < 2 || ipos.z > 255) {
		std::cout << "ERROR COLLISION BLOCK OUT OF CHUNK " << ipos.x << ", " << ipos.y << ", " << ipos.z << std::endl;
		// std::cout << "\treal pos " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
		// std::cout << "\tchunk pos " << _startX << ", " << _startY << std::endl;
		return (true);
	}
	if (air_flower(_blocks[((ipos.x + 1) * (CHUNK_SIZE + 2) + ipos.y + 1) * WORLD_HEIGHT + ipos.z], false)
		|| air_flower(_blocks[((ipos.x + 1) * (CHUNK_SIZE + 2) + ipos.y + 1) * WORLD_HEIGHT + ipos.z - 1], false)) { // body in block
		return (true);
	}
	glm::vec3 save_pos = pos;
	if (!air_flower(_blocks[((ipos.x + 1) * (CHUNK_SIZE + 2) + ipos.y + 1) * WORLD_HEIGHT + ipos.z - 2], false)) { // falling
		// std::cout << "before fall: " << pos.z << " vs " << save_pos.z << std::endl;
		cam.fall(true);
		// std::cout << "after fall: " << pos.z << " vs " << save_pos.z << std::endl;
		for (int index = save_pos.z; index >= ((pos.z > 2) ? pos.z : 2); index--) {
			if (air_flower(_blocks[((ipos.x + 1) * (CHUNK_SIZE + 2) + ipos.y + 1) * WORLD_HEIGHT + index], false)) {
				pos.z = index + 2.62f;
				cam.touchGround();
				cam._update = true;
				return (false);
			}
		}
	} else {
		cam.fall(false);
	}
	// cam.fall(true);
	// for (int index = save_pos.z - 1; index >= ((pos.z > 2) ? pos.z : 2); index--) {
	// 	if (air_flower(_blocks[((ipos.x + 1) * (CHUNK_SIZE + 2) + ipos.y + 1) * WORLD_HEIGHT + index], false)) {
	// 		pos.z = index + 2.62f;
	// 		cam.touchGround();
	// 		cam._update = true;
	// 		return (false);
	// 	}
	// }
	if (save_pos.z != pos.z) {
		cam._update = true;
	}
	return (false);
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
	_mtx.lock();
	if (!_vaoReset) {
		// std::cout << "chunk reset " << _startX << ", " << _startY << std::endl;
		_mtx.unlock();
		++counter;
		// if (counter > 5) { // we don't load more than 5 new chunks per frame
		// if (counter > 5 && counter < 50) { // we don't load more than 5 new chunks per frame, but if there's more than 50 to load, we take the hit
		if (counter % 50 > 5) { // we don't load more than 5 new chunks per 50 new chunks per frame
			return ;
		}
		setup_array_buffer();
		_mtx.lock();
	}
    glBindVertexArray(_vao); // this is the costly operation, chunk_size up == fps down
	glDrawArrays(GL_POINTS, 0, _displayed_blocks);
	block_counter += _displayed_blocks;
	_mtx.unlock();
}
