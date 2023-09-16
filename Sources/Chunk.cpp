#include "vox.h"

Chunk::Chunk( glm::vec2 start ) : _isVisible(true), _vaoSet(false), _blocks(NULL), _vertices(NULL), _displayed_blocks(0)
{
	int posX = static_cast<int>(glm::floor(start.x));
	int posY = static_cast<int>(glm::floor(start.y));

	// std::cout << "start: " << start.x << ", " << start.y << ", posX: " << posX << ", posY: " << posY << std::endl;
	(posX >= 0)
		? posX -= posX % 16
		: posX -= 16 + posX % 16;
	(posY >= 0)
		? posY -= posY % 16
		: posY -= 16 + posY % 16;
	// std::cout << "posX: " << posX << ", posY: " << posY << std::endl;
	_start = glm::vec2(static_cast<float>(posX), static_cast<float>(posY));
	// std::cout << "new Chunk at [" << _start.x << ", " << _start.y << ']' << std::endl;
}

Chunk::~Chunk( void )
{
    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);

	delete [] _blocks;
	delete [] _vertices;

	if (_thread.joinable()) {
		_thread.join();
	}
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// takes a block and check if player can see it at some point
bool Chunk::exposed_block( int row, int col, int level )
{
	bool below = false;
	bool above = false;
	switch (level) {
		case 0:
			above = !_blocks[(row * 18 + col) * 256 + level + 1];
			break ;
		case 255:
			below = !_blocks[(row * 18 + col) * 256 + level - 1];
			break ;
		default:
			below = !_blocks[(row * 18 + col) * 256 + level - 1];
			above = !_blocks[(row * 18 + col) * 256 + level + 1];
	}
	return (!_blocks[((row - 1) * 18 + col) * 256 + level]
		|| !_blocks[((row + 1) * 18 + col) * 256 + level]
		|| !_blocks[(row * 18 + col - 1) * 256 + level]
		|| !_blocks[(row * 18 + col + 1) * 256 + level]
		|| below || above);
}

void Chunk::generate_blocks( void )
{
	const siv::PerlinNoise::seed_type seed = 123456u;
	const siv::PerlinNoise perlin{ seed };

	for (int row = 0; row < 18; row++) {
		for (int col = 0; col < 18; col++) {
			int surface_level = glm::floor(SEA_LEVEL + (perlin.noise2D_01(double(_start.x - 1 + row) / 100, double(_start.y - 1 + col) / 100) - 0.5) * 100); 
			for (int level = 0; level < 256; level++) {
				_blocks[(row * 18 + col) * 256 + level] = (level < surface_level) * 2 + (level == surface_level);//+ (level > surface_level && level <= 64) * 3;
			}
		}
	}

	for (int row = 1; row < 17; row++) {
		for (int col = 1; col < 17; col++) {
			for (int level = 0; level < 256; level++) {
				if (_blocks[(row * 18 + col) * 256 + level]) {
					(exposed_block(row, col, level))
						? _displayed_blocks++
						:_blocks[(row * 18 + col) * 256 + level] = 8;
				}
			}
		}
	}
}

void Chunk::fill_vertex_array( void )
{
	int index = 0;
	for (int row = 0; row < 16; row++) {
		for (int col = 0; col < 16; col++) {
			for (int level = 0; level < 256; level++) {
				GLint block_type = _blocks[((row + 1) * 18 + col + 1) * 256 + level];
				if (block_type && block_type != 8) {
					_vertices[index] = block_type;
					_vertices[index + 1] = 0.0f;//4.0f * (row != 0) + 8.0f * (row != 15) + 2.0f * (col != 15) + 1.0f * (col != 0);
					_vertices[index + 2] = _start.x + row;
					_vertices[index + 3] = _start.y + col;
					_vertices[index + 4] = static_cast<GLfloat>(level);
					index += 5;
				}
			}
		}
	}
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void thread_setup_chunk( std::list<Chunk *> *chunks, Chunk *current )
{
	// std::cout << "hello from thread" << std::endl;
	current->generation();

	mtx.lock();
	chunks->push_back(current);
	mtx.unlock();
}

void Chunk::generation( void )
{
	_blocks = new GLint[18 * 18 * 256];
	generate_blocks();
	_vertices = new GLfloat[_displayed_blocks * 5]; // blocktype, adjacents blocks, X Y Z
	fill_vertex_array();
}

void Chunk::generate_chunk( std::list<Chunk *> *chunks )
{
	_thread = std::thread(thread_setup_chunk, chunks, this);
}

void Chunk::setVisibility( bool value )
{
	_isVisible = value;
}


bool Chunk::shouldDelete( glm::vec3 pos, GLfloat dist )
{
	if (glm::distance(glm::vec2(pos.x, pos.y), _start) > dist) {
		// std::cout << "deleting chunk at " << _start.x << ", " << _start.y << std::endl;
		return (true);
	}
	return (false);
}

bool Chunk::isInChunk( glm::vec2 pos )
{
	// std::cout << "x: " << pos.x << ", y: " << pos.y << std::endl;
	// std::cout << "checking chunk " << _start.x << ", " << _start.y << std::endl;
	return (pos.x >= _start.x && pos.x < _start.x + 16.0f && pos.y >= _start.y && pos.y < _start.y + 16.0f);
}

void Chunk::setup_array_buffer( void )
{
	if (_vaoSet) {
		return ;
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

void Chunk::drawArray( void )
{
	if (!_isVisible || !_vaoSet) {
		return ;
	}
    glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _displayed_blocks);
}
