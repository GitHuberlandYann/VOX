#include "vox.h"

Chunk::Chunk( int posX, int posY ) : _isVisible(true), _vaoSet(false), _vaoReset(false), _startX(posX), _startY(posY),
	_blocks(NULL), _vertices(NULL), _displayed_blocks(0)
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
	if (value >= blocks::POPPY || (air_leaves && value == blocks::OAK_LEAVES)) {
		return (0);
	}
	return (value);
}

GLint Chunk::get_empty_faces( int row, int col, int level )
{
	GLint res = !!air_flower(_blocks[((row - 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], true) * (1 << 2)
				+ !!air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], true) * (1 << 3)
				+ !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col - 1) * WORLD_HEIGHT + level], true) * (1 << 0)
				+ !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level], true) * (1 << 1);
	switch (level) {
		case 0:
			res += (1 << 5);
			res += !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1], true) * (1 << 4);
			break ;
		case 255:
			res += !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], true) * (1 << 5);
			res += (1 << 4);
			break ;
		default:
			res += !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], true) * (1 << 5);
			res += !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1], true) * (1 << 4);
	}
	return (res);
}

// takes a block and check if player can see it at some point
bool Chunk::exposed_block( int row, int col, int level )
{
	bool below = false;
	bool above = false;
	switch (level) {
		case 0:
			above = !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1], true);
			break ;
		case 255:
			below = !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], true);
			break ;
		default:
			below = !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], true);
			above = !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1], true);
	}
	return (!air_flower(_blocks[((row - 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], true)
		|| !air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], true)
		|| !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col - 1) * WORLD_HEIGHT + level], true)
		|| !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level], true)
		|| below || above);
}

int Chunk::get_block_type(siv::PerlinNoise perlin, int row, int col, int level, int surface_level,
	bool poppy, bool dandelion, bool blue_orchid, bool allium, bool cornflower, bool pink_tulip,
	bool grass, bool tree_gen, std::vector<glm::ivec3> & trees)
{
	if (level == 0) {
		return (blocks::BEDROCK);
	}
	int value = (level > surface_level) ? blocks::AIR : (level < surface_level - 2 || level <= 64) ? blocks::STONE : blocks::GRASS_BLOCK;
	if (value) {
		if (value == blocks::STONE) {
			double undergound = perlin.noise3D_01((_startX - 1 + row) / 100.0f, (_startY - 1 + col) / 100.0f, level / 100.0f);
			(undergound < 0.55) ? : value = blocks::SAND;
		}
	} else if (tree_gen && surface_level > 65 && level <= surface_level + 5 && surface_level < 220) {
		value = blocks::OAK_TRUNK;
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

void Chunk::generate_blocks( void )
{
	const siv::PerlinNoise perlin{ perlin_seed };
	// std::minstd_rand0  generator(_startX * 19516 + _startY * 56849);
	std::minstd_rand0  generator((_startX * 19511 + _startY * 56844) * perlin_seed);
	std::uniform_int_distribution<int> distribution(0, 1000);

	std::vector<glm::ivec3> trees;

	// generating base terrain
	for (int row = 0; row < (CHUNK_SIZE + 2); row++) {
		for (int col = 0; col < (CHUNK_SIZE + 2); col++) {
			// int surface_level = glm::floor(SEA_LEVEL + (perlin.noise2D_01((_startX - 1 + row) / 100.0f, (_startY - 1 + col) / 100.0f) - 0.5) * 100); 
			int surface_level = glm::floor(SEA_LEVEL + (perlin.octave2D_01((_startX - 1 + row) / 100.0f, (_startY - 1 + col) / 100.0f, 4) - 0.5) * 100);
			// int surface_level = glm::floor(SEA_LEVEL + (perlin.octave2D_01((_startX - 1 + row) / 100.0f, (_startY - 1 + col) / 100.0f, 4) - 0.5) * 50
			// 			+ (perlin.noise2D_01((_startX - 1000 + row) / 1000.0f, (_startY - 1000 + col) / 1000.0f) - 0.5) * 200);
			bool tree_gen = (distribution(generator) <= 2 && row > 2 && row < CHUNK_SIZE - 1 && col > 2 && col < CHUNK_SIZE - 1);
			bool poppy = (distribution(generator) <= 2 && row > 1 && row < CHUNK_SIZE && col > 1 && col < CHUNK_SIZE);
			bool dandelion = (distribution(generator) <= 2 && row > 1 && row < CHUNK_SIZE && col > 1 && col < CHUNK_SIZE);
			bool blue_orchid = (distribution(generator) <= 2 && row > 1 && row < CHUNK_SIZE && col > 1 && col < CHUNK_SIZE);
			bool allium = (distribution(generator) <= 2 && row > 1 && row < CHUNK_SIZE && col > 1 && col < CHUNK_SIZE);
			bool cornflower = (distribution(generator) <= 2 && row > 1 && row < CHUNK_SIZE && col > 1 && col < CHUNK_SIZE);
			bool pink_tulip = (distribution(generator) <= 2 && row > 1 && row < CHUNK_SIZE && col > 1 && col < CHUNK_SIZE);
			bool grass = (distribution(generator) <= 100 && row > 1 && row < CHUNK_SIZE && col > 1 && col < CHUNK_SIZE);
			for (int level = 0; level < WORLD_HEIGHT; level++) {
				// double cave = perlin.octave3D_01((_startX - 1000 + row) / 100.0f, (_startY - 1000 + col) / 100.0f, (level) / 20.0f, 4);
				// (level < surface_level - 5 && cave <= 0.2f && level > 0)
				// 	? _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = blocks::AIR
				// 	: _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = get_block_type(perlin, row, col, level, surface_level, poppy, dandelion, blue_orchid, allium, cornflower, pink_tulip, tree_gen, trees);
				_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = get_block_type(perlin, row, col, level, surface_level, poppy, dandelion, blue_orchid, allium, cornflower, pink_tulip, grass, tree_gen, trees);
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
					if (coal_blobs < 20 && distribution(generator) <= 80) {
						int size = (distribution(generator) + 3) / 30;
						gen_ore_blob(blocks::COAL_ORE, row, col, level, size, 0);
						coal_blobs++;
					} else if (iron_blobs < 10 && (level <= 56 || level > 80) && distribution(generator) <= 80) {
						int size = (distribution(generator) + 3) / 60;
						gen_ore_blob(blocks::IRON_ORE, row, col, level, size, 0);
						iron_blobs++;
					} else if (!gravel_blob && distribution(generator) <= 10) {
						int size = 60;
						gen_ore_blob(blocks::GRAVEL, row, col, level, size, 0);
						gravel_blob = true;
					} else if (diamond_blobs < 7 && level < 15 && distribution(generator) <= 60) {
						int size = (distribution(generator) + 5) / 200;
						gen_ore_blob(blocks::DIAMOND_ORE, row, col, level, size, 0);
						diamond_blobs++;
					}
				}
				// turning grass into dirt if under other block
				if (value == blocks::GRASS_BLOCK && level != 255) {
					int block_above = _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1];
					if (block_above && block_above < blocks::POPPY) {
						_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level] = blocks::DIRT_BLOCK;
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
					if (exposed_block(row, col, level)) {
						_displayed_blocks++;
						// reverting ores back to stone if they are exposed
						if (value == blocks::IRON_ORE && distribution(generator) < 500) {
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

void Chunk::remove_block( Inventory *inventory, glm::ivec3 pos )
{
	// std::cout << "in chunk " << _startX << ", " << _startY << ":rm block " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
	// std::cout << "nb displayed blocks before: " << _displayed_blocks << std::endl;
	int value = _blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z];
	if (value > blocks::AIR) { // if invisible block gets deleted, same amount of displayed_blocks
		_displayed_blocks--;
		inventory->addBlock(value);
	}
	_blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z] = blocks::AIR;
	for (int index = 0; index < 6; index++) {
		const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
		if (pos.x + delta[0] < 0 || pos.x + delta[0] >= CHUNK_SIZE || pos.y + delta[1] < 0 || pos.y + delta[1] >= CHUNK_SIZE || pos.z + delta[2] < 0 || pos.z + delta[2] > 255) {

		} else {
			if (_blocks[((pos.x + delta[0] + 1) * (CHUNK_SIZE + 2) + pos.y + delta[1] + 1) * WORLD_HEIGHT + pos.z + delta[2]] < blocks::AIR) {
				_blocks[((pos.x + delta[0] + 1) * (CHUNK_SIZE + 2) + pos.y + delta[1] + 1) * WORLD_HEIGHT + pos.z + delta[2]] += blocks::NOTVISIBLE;
				_displayed_blocks++;
			}
		}
	}
	// std::cout << "nb displayed blocks after: " << _displayed_blocks << std::endl;
}

void Chunk::add_block( glm::ivec3 pos, int type )
{
	// std::cout << "in chunk " << _startX << ", " << _startY << ":rm block " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
	// std::cout << "nb displayed blocks before: " << _displayed_blocks << std::endl;
	_blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z] = type;
	_displayed_blocks++;
	for (int index = 0; index < 6; index++) {
		const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
		if (pos.x + delta[0] < 0 || pos.x + delta[0] >= CHUNK_SIZE || pos.y + delta[1] < 0 || pos.y + delta[1] >= CHUNK_SIZE || pos.z + delta[2] < 0 || pos.z + delta[2] > 255) {

		} else {
			GLint value = _blocks[((pos.x + delta[0] + 1) * (CHUNK_SIZE + 2) + pos.y + delta[1] + 1) * WORLD_HEIGHT + pos.z + delta[2]];
			if (value > blocks::AIR && !exposed_block(pos.x + delta[0] + 1, pos.y + delta[1] + 1, pos.z + delta[2] + 1)) {
				// was exposed before, but isn't anymore
				_blocks[((pos.x + delta[0] + 1) * (CHUNK_SIZE + 2) + pos.y + delta[1] + 1) * WORLD_HEIGHT + pos.z + delta[2]] -= blocks::NOTVISIBLE;
				_displayed_blocks--;
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
				if (block_type > blocks::AIR) {
					// if (index + 5 > _displayed_blocks * 5) {
					// 	std::cout << "ERROR index is " << index / 5 << std::endl;
					// }
					_vertices[index] = block_type;
					_vertices[index + 1] = get_empty_faces(row + 1, col + 1, level);
					_vertices[index + 2] = _startX + row;
					_vertices[index + 3] = _startY + col;
					_vertices[index + 4] = level;
					index += 5;
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
	glBufferData(GL_ARRAY_BUFFER, _displayed_blocks * 5 * sizeof(GLint), _vertices, GL_STATIC_DRAW);

	delete [] _vertices;
	_vertices = NULL;
	_vaoReset = true;
	// check_glstate("Vertex buffer successfully created");

	glEnableVertexAttribArray(BLOCKATTRIB);
	glVertexAttribIPointer(BLOCKATTRIB, 1, GL_INT, 5 * sizeof(GLint), 0);
	// check_glstate("blockAttrib successfully set");
	
	glEnableVertexAttribArray(ADJATTRIB);
	glVertexAttribIPointer(ADJATTRIB, 1, GL_INT, 5 * sizeof(GLint), (void *)(sizeof(GLint)));
	// check_glstate("adjAttrib successfully set");

	glEnableVertexAttribArray(POSATTRIB);
	glVertexAttribIPointer(POSATTRIB, 3, GL_INT, 5 * sizeof(GLint), (void *)(2 * sizeof(GLint)));
	// check_glstate("posAttrib successfully set");

	check_glstate("NO");
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

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
	_vertices = new GLint[_displayed_blocks * 5]; // blocktype, adjacents blocks, X Y Z
	fill_vertex_array();
}

void Chunk::regeneration( Inventory *inventory, glm::ivec3 pos, bool adding )
{
	if (!adding) {
		if (_blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z] == blocks::BEDROCK) { // can't rm bedrock
			return ;
		}
		remove_block(inventory, pos);
	} else {
		int type = inventory->getCurrentSlot();
		if (_blocks[((pos.x + 1) * (CHUNK_SIZE + 2) + pos.y + 1) * WORLD_HEIGHT + pos.z] != blocks::AIR || type == blocks::AIR) { // can't replace block
			return ;
		}
		add_block(pos, type);
		inventory->removeBlock();
	}
	delete [] _vertices;
	_vertices = new GLint[_displayed_blocks * 5]; // blocktype, adjacents blocks, X Y Z
	fill_vertex_array();
	_mtx.lock();
	_vaoReset = false;
	_mtx.unlock();
}

void Chunk::generate_chunk( std::list<Chunk *> *chunks )
{
	_thread = std::thread(thread_setup_chunk, chunks, this);
}

void Chunk::setVisibility( std::list<Chunk *> *visible_chunks, int posX, int posY, GLint render_dist )
{
	_isVisible = (_startX >= posX - render_dist && _startX <= posX + render_dist
			&& _startY >= posY - render_dist && _startY <= posY + render_dist);
	if (_isVisible) {
		visible_chunks->push_back(this);
	}
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

bool Chunk::isHit( glm::ivec3 pos )
{
	// std::cout << "current_chunk is " << _startX << ", " << _startY << std::endl;
	glm::ivec3 chunk_pos = glm::ivec3(pos.x - _startX, pos.y - _startY, pos.z);
	if (chunk_pos.x < 0 || chunk_pos.x >= CHUNK_SIZE || chunk_pos.y < 0 || chunk_pos.y >= CHUNK_SIZE || chunk_pos.z < 0 || chunk_pos.z > 255) {
		std::cout << "ERROR block out of chunk " << chunk_pos.x << ", " << chunk_pos.y << ", " << chunk_pos.z << std::endl;
		return (false);
	}
	if (_thread.joinable()) {
		_thread.join();
	}
	return (_blocks[((chunk_pos.x + 1) * (CHUNK_SIZE + 2) + chunk_pos.y + 1) * WORLD_HEIGHT + chunk_pos.z]);
}

static void thread_modif_block( Chunk *current, Inventory *inventory, glm::ivec3 pos, bool adding )
{
	current->regeneration(inventory, pos, adding);
}

void Chunk::handleHit( Inventory *inventory, glm::ivec3 pos, bool adding )
{

	glm::ivec3 chunk_pos = glm::ivec3(pos.x - _startX, pos.y - _startY, pos.z);
	// if (chunk_pos.x < 0 || chunk_pos.x >= CHUNK_SIZE || chunk_pos.y < 0 || chunk_pos.y >= CHUNK_SIZE || chunk_pos.z < 0 || chunk_pos.z > 255) {
	// 	std::cout << "ERROR BLOCK OUT OF CHUNK " << chunk_pos.x << ", " << chunk_pos.y << ", " << chunk_pos.z << std::endl;
	// }
	if (_thread.joinable()) {
		_thread.join();
	}
	_thread = std::thread(thread_modif_block, this, inventory, chunk_pos, adding);
}

bool Chunk::collision( glm::vec3 & pos, Camera &cam )
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
		for (int index = save_pos.z - 2; index >= ((pos.z > 2) ? pos.z : 2); index--) {
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
	if (save_pos.z != pos.z) {
		cam._update = true;
	}
	return (false);
}

void Chunk::drawArray( GLint & counter )
{
	_mtx.lock();
	if (!_vaoReset) {
		_mtx.unlock();
		++counter;
		// if (counter > 5) { // we don't load more than 5 new chunks per frame
		// if (counter > 5 && counter < 50) { // we don't load more than 5 new chunks per frame, but if there's more than 50 to load, we take the hit
		if (counter % 50 > 5) { // we don't load more than 5 new chunks per 50 new chunks per frame
			return ;
		}
		setup_array_buffer();
		// return ;// TODO decide if this changes something
		_mtx.lock();
	}
	_mtx.unlock();
    glBindVertexArray(_vao); // this is the costly operation, chunk_size up == fps down
	glDrawArrays(GL_POINTS, 0, _displayed_blocks);
}
