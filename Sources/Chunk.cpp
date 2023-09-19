#include "vox.h"

Chunk::Chunk( int posX, int posY ) : _isVisible(true), _vaoSet(false), _startX(posX), _startY(posY), _blocks(NULL), _vertices(NULL), _displayed_blocks(0)
{
	// int posX = static_cast<int>(glm::floor(start.x));
	// int posY = static_cast<int>(glm::floor(start.y));

	// // std::cout << "start: " << start.x << ", " << start.y << ", posX: " << posX << ", posY: " << posY << std::endl;
	// (posX >= 0)
	// 	? posX -= posX % CHUNK_SIZE
	// 	: posX -= CHUNK_SIZE + posX % CHUNK_SIZE;
	// (posY >= 0)
	// 	? posY -= posY % CHUNK_SIZE
	// 	: posY -= CHUNK_SIZE + posY % CHUNK_SIZE;
	// // std::cout << "posX: " << posX << ", posY: " << posY << std::endl;
	// _start = glm::vec2(static_cast<float>(posX), static_cast<float>(posY));
	// std::cout << "new Chunk at [" << _start.x << ", " << _start.y << ']' << std::endl;
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

// we simulate that flowers are air block in order to display adjacent blocks properly
// also use this for all 'see-through' blocks like leaves, ..
static int air_flower( int value )
{
	if (value > 16 || value == blocks::OAK_LEAVES) {
		return (0);
	}
	return (value);
}

GLint Chunk::get_empty_faces( int row, int col, int level )
{
	GLint res = 0;
	res += !!air_flower(_blocks[((row - 1) * (CHUNK_SIZE + 2) + col) * 256 + level]) * (1 << 2);
	res += !!air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col) * 256 + level]) * (1 << 3);
	res += !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col - 1) * 256 + level]) * (1 << 0);
	res += !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col + 1) * 256 + level]) * (1 << 1);
	switch (level) {
		case 0:
			res += (1 << 5);
			res += !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level + 1]) * (1 << 4);
			break ;
		case 255:
			res += !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level - 1]) * (1 << 5);
			res += (1 << 4);
			break ;
		default:
			res += !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level - 1]) * (1 << 5);
			res += !!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level + 1]) * (1 << 4);
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
			above = !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level + 1]);
			break ;
		case 255:
			below = !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level - 1]);
			break ;
		default:
			below = !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level - 1]);
			above = !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level + 1]);
	}
	return (!air_flower(_blocks[((row - 1) * (CHUNK_SIZE + 2) + col) * 256 + level])
		|| !air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col) * 256 + level])
		|| !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col - 1) * 256 + level])
		|| !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col + 1) * 256 + level])
		|| below || above);
}

int Chunk::get_block_type(siv::PerlinNoise perlin, int row, int col, int level, int surface_level,
	bool poppy, bool dandelion, bool blue_orchid, bool allium, bool cornflower, bool pink_tulip,
	bool tree_gen, std::vector<glm::ivec3> & trees)
{
	int value = (level <= surface_level);
	if (value) {
		if (level <= 64) {
			(perlin.noise2D_01((_startX - 1 + row) / 100.0f, (_startY - 1 + col) / 100.0f) < 0.55)
				? value = blocks::STONE
				: value = blocks::SAND;
		}
	} else if (tree_gen && surface_level > 65 && surface_level < 220 && level <= surface_level + 5) {
		value = blocks::OAK_TRUNK;
		if (level == surface_level + 5) {
			trees.push_back(glm::ivec3(row, col, level));
		}
	} else if (surface_level > 65 && surface_level < 220 && level == surface_level + 1) {
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
		}
	}
	if (level == 0) {
		value = blocks::BEDROCK;
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
			for (int level = 0; level < 256; level++) {
				// double cave = perlin.octave3D_01((_startX - 1000 + row) / 100.0f, (_startY - 1000 + col) / 100.0f, (level) / 20.0f, 4);
				// (level < surface_level - 5 && cave <= 0.2f)
				// 	? _blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level] = 0
				// 	: _blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level] = (level <= surface_level);
				_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level] = get_block_type(perlin, row, col, level, surface_level, poppy, dandelion, blue_orchid, allium, cornflower, pink_tulip, tree_gen, trees);;
				// GLfloat squashing_factor;
				// (level < 64)
				// 	? squashing_factor = (64 - level) / 64.0f
				// 	: squashing_factor = -(level - 64) / 192.0f;
				// // // std::cout << "squash at " << level << ": " << squashing_factor << std::endl;
				// _blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level] = ((perlin.octave3D_01((_startX - 1 + row) / 100.0f, (_startY - 1 + col) / 100.0f, level / 20.0f, 4) + squashing_factor) > 0.5f);
				// if (row == 9 && col == 9 && level == surface_level + 5) {
				// 	_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level] = 1;
				// }
			}
		}
	}

	// adding trees
	std::vector<glm::ivec3>::iterator it = trees.begin();
	for (; it != trees.end(); it++) {
		for (int index = 0; index < 61; index++) {
			const GLint delta[3] = {oak_normal[index][0], oak_normal[index][1], oak_normal[index][2]};
			if (_blocks[(((*it).x + delta[0]) * (CHUNK_SIZE + 2) + (*it).y + delta[1]) * 256 + (*it).z + delta[2]] == blocks::AIR) {
				_blocks[(((*it).x + delta[0]) * (CHUNK_SIZE + 2) + (*it).y + delta[1]) * 256 + (*it).z + delta[2]] = blocks::OAK_LEAVES;
			}
		}
	}
	trees.clear();

	// hiding unseen block
	for (int row = 1; row < CHUNK_SIZE + 1; row++) {
		for (int col = 1; col < CHUNK_SIZE + 1; col++) {
			for (int level = 0; level < 256; level++) {
				GLint value = _blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level];
				if (value) {
					if (exposed_block(row, col, level)) {
						_displayed_blocks++;
						if (value == blocks::GRASS_BLOCK && level != 255) {
							int block_above = _blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level + 1];
							if (block_above && block_above < 17) {
								_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level] = blocks::GRASS_BLOCK_UNDER;
							}
						}
					} else {
						_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level] = blocks::NOTVISIBLE;
					}
				}
			}
		}
	}
}

void Chunk::fill_vertex_array( void )
{
	int index = 0;
	for (int row = 0; row < CHUNK_SIZE; row++) {
		for (int col = 0; col < CHUNK_SIZE; col++) {
			for (int level = 0; level < 256; level++) {
				GLint block_type = _blocks[((row + 1) * (CHUNK_SIZE + 2) + col + 1) * 256 + level];
				if (block_type && block_type != blocks::NOTVISIBLE) {
					_vertices[index] = block_type;
					_vertices[index + 1] = get_empty_faces(row + 1, col + 1, level);//4.0f * (row != 0) + 8.0f * (row != 15) + 2.0f * (col != 15) + 1.0f * (col != 0);
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
	if (_vaoSet) {
		return ;
	}
	if (_thread.joinable()) {
		_thread.join();
	}

    glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _displayed_blocks * 5 * sizeof(GLint), _vertices, GL_STATIC_DRAW);
	// glBufferData(GL_ARRAY_BUFFER, _number_vertices * 12 * sizeof(float), vertices, GL_STATIC_DRAW);

	delete [] _vertices;
	_vertices = NULL;
	_vaoSet = true;
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
	// std::cout << "hello from thread" << std::endl;
	current->generation();

	mtx.lock();
	chunks->push_back(current);
	mtx.unlock();
}

void Chunk::generation( void )
{
	_blocks = new GLint[(CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * 256];
	generate_blocks();
	_vertices = new GLint[_displayed_blocks * 5]; // blocktype, adjacents blocks, X Y Z
	fill_vertex_array();
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
	// std::cout << "x: " << pos.x << ", y: " << pos.y << std::endl;
	// std::cout << "checking chunk " << _start.x << ", " << _start.y << std::endl;
	// return (pos.x >= _start.x && pos.x < _start.x + CHUNK_SIZE.0f && pos.y >= _start.y && pos.y < _start.y + CHUNK_SIZE.0f);
	return (posX == _startX && posY == _startY);
}

// void Chunk::action_block( glm::vec3 pos, glm::vec3 front, int action)
// {
// 	(action == ADD_BLOCK)
// 		? std::cout << "add block from " << pos.x << ", " << pos.y << ", " << pos.z << " in chunk " << _startX << ", " << _startY << std::endl
// 		: std::cout << "rm block from " << pos.x << ", " << pos.y << ", " << pos.z << " in chunk " << _startX << ", " << _startY << std::endl;
// 	float sideX = (front.x > 0) - glm::mod(pos.x, 1.0f);
// 	float sideY = (front.y > 0) - glm::mod(pos.y, 1.0f);
// 	float sideZ = (front.z > 0) - glm::mod(pos.z, 1.0f);
// 	std::cout << "computed sides: " << sideX << ", " << sideY << ", " << sideZ << std::endl;
// }

void Chunk::drawArray( GLint & counter )
{
	if (!_vaoSet) {
		++counter;
		// if (counter > 5) { // we don't load more than 5 new chunks per frame
		// if (counter > 5 && counter < 50) { // we don't load more than 5 new chunks per frame, but if there's more than 50 to load, we take the hit
		if (counter % 50 > 5) { // we don't load more than 5 new chunks per 50 new chunks per frame
			return ;
		}
		setup_array_buffer();
		// return ;// TODO decide if this changes something
	}
    glBindVertexArray(_vao); // this is the costly operation, chunk_size up == fps down
	glDrawArrays(GL_POINTS, 0, _displayed_blocks);
}
