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

GLfloat Chunk::get_empty_faces( int row, int col, int level )
{
	GLfloat res = 0.0f;
	res += !!_blocks[((row - 1) * (CHUNK_SIZE + 2) + col) * 256 + level] * (1 << 2);
	res += !!_blocks[((row + 1) * (CHUNK_SIZE + 2) + col) * 256 + level] * (1 << 3);
	res += !!_blocks[(row * (CHUNK_SIZE + 2) + col - 1) * 256 + level] * (1 << 0);
	res += !!_blocks[(row * (CHUNK_SIZE + 2) + col + 1) * 256 + level] * (1 << 1);
	switch (level) {
		case 0:
			res += !!_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level + 1] * (1 << 4);
			break ;
		case 255:
			res += !!_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level - 1] * (1 << 5);
			break ;
		default:
			res += !!_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level - 1] * (1 << 5);
			res += !!_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level + 1] * (1 << 4);
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
			above = !_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level + 1];
			break ;
		case 255:
			below = !_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level - 1];
			break ;
		default:
			below = !_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level - 1];
			above = !_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level + 1];
	}
	return (!_blocks[((row - 1) * (CHUNK_SIZE + 2) + col) * 256 + level]
		|| !_blocks[((row + 1) * (CHUNK_SIZE + 2) + col) * 256 + level]
		|| !_blocks[(row * (CHUNK_SIZE + 2) + col - 1) * 256 + level]
		|| !_blocks[(row * (CHUNK_SIZE + 2) + col + 1) * 256 + level]
		|| below || above);
}

void Chunk::generate_blocks( void )
{
	const siv::PerlinNoise::seed_type seed = 123456u;
	const siv::PerlinNoise perlin{ seed };

	for (int row = 0; row < (CHUNK_SIZE + 2); row++) {
		for (int col = 0; col < (CHUNK_SIZE + 2); col++) {
			// int surface_level = glm::floor(SEA_LEVEL + (perlin.noise2D_01(double(_startX - 1 + row) / 100, double(_startY - 1 + col) / 100) - 0.5) * 100); 
			int surface_level = glm::floor(SEA_LEVEL + (perlin.octave2D_01((_startX - 1 + row) / 100.0f, (_startY - 1 + col) / 100.0f, 4) - 0.5) * 100); 
			// int surface_level = glm::floor(SEA_LEVEL + (perlin.octave2D_01(double(_startX - 1 + row) / 100, double(_startY - 1 + col) / 100, 4) - 0.5) * 50
			// 			+ (perlin.noise2D_01(double(_startX - 1000 + row) / 1000, double(_startY - 1000 + col) / 1000) - 0.5) * 200);
			for (int level = 0; level < 256; level++) {
				// double cave = perlin.octave3D_01((_startX - 1000 + row) / 100.0f, (_startY - 1000 + col) / 100.0f, (level) / 20.0f, 4);
				// (level < surface_level - 5 && cave <= 0.2f)
				// 	? _blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level] = 0
				// 	: _blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level] = (level <= surface_level);
				int value = (level <= surface_level);
				if (value) {
					value += 2 * (level <= 45);
					// value += 3 * (level >= 100);
				}
				_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level] = value;
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

	for (int row = 1; row < CHUNK_SIZE + 1; row++) {
		for (int col = 1; col < CHUNK_SIZE + 1; col++) {
			for (int level = 0; level < 256; level++) {
				GLint value = _blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level];
				if (value) {
					if (exposed_block(row, col, level)) {
						_displayed_blocks++;
						if (value == 1 && level != 255 && _blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level + 1]) {
							_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level] = 2;
						}
					} else {
						_blocks[(row * (CHUNK_SIZE + 2) + col) * 256 + level] = 8;
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
				if (block_type && block_type != 8) {
					_vertices[index] = block_type;
					_vertices[index + 1] = get_empty_faces(row + 1, col + 1, level);//4.0f * (row != 0) + 8.0f * (row != 15) + 2.0f * (col != 15) + 1.0f * (col != 0);
					_vertices[index + 2] = _startX + row;
					_vertices[index + 3] = _startY + col;
					_vertices[index + 4] = static_cast<GLfloat>(level);
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

	// GLfloat *vertices = new GLfloat[_displayed_blocks * 5]; // blocktype, adjacents blocks, X Y Z
	// GLfloat points[] = {
	// 	1.0f, 8.0f, 0.0f,  0.0f, z, // blocktype, adjacents blocks, X Y Z
	// 	1.0f, 0.0f, 2.5f,  3.5f, z,
	// 	1.0f, 4.0f, 1.0f, 0.0f, z,
	// 	// 1.0f, -0.45f, -0.45f, 0.0f
	// };
	// std::cout << "total alloc of vertices: " << _displayed_blocks << std::endl;
	// fill_vertex_array(vertices);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _displayed_blocks * 5 * sizeof(GLfloat), _vertices, GL_STATIC_DRAW);
	// glBufferData(GL_ARRAY_BUFFER, _number_vertices * 12 * sizeof(float), vertices, GL_STATIC_DRAW);

	delete [] _vertices;
	_vertices = NULL;
	_vaoSet = true;
	// check_glstate("Vertex buffer successfully created");

	glEnableVertexAttribArray(BLOCKATTRIB);
	glVertexAttribPointer(BLOCKATTRIB, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	// check_glstate("blockAttrib successfully set");
	
	glEnableVertexAttribArray(ADJATTRIB);
	glVertexAttribPointer(ADJATTRIB, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(sizeof(GLfloat)));
	// check_glstate("adjAttrib successfully set");

	glEnableVertexAttribArray(POSATTRIB);
	glVertexAttribPointer(POSATTRIB, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
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
	_vertices = new GLfloat[_displayed_blocks * 5]; // blocktype, adjacents blocks, X Y Z
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
		mtx.lock();
		visible_chunks->push_back(this);
		mtx.unlock();
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
