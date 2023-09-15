#include "vox.h"

Chunk::Chunk( glm::vec2 start ) : _isVisible(true)
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
	std::cout << "new Chunk at [" << _start.x << ", " << _start.y << ']' << std::endl;
}

Chunk::~Chunk( void )
{
    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void Chunk::fill_vertex_array( GLfloat *vertices, GLfloat z )
{
	const siv::PerlinNoise::seed_type seed = 123456u;
	const siv::PerlinNoise perlin{ seed };

	for (int row = 0; row < 16; row++) {
		for (int col = 0; col < 16; col++) {
			vertices[(row * 16 + col) * 5] = 1.0f;
			vertices[(row * 16 + col) * 5 + 1] = 0.0f;//4.0f * (row != 0) + 8.0f * (row != 15) + 2.0f * (col != 15) + 1.0f * (col != 0);
			vertices[(row * 16 + col) * 5 + 2] = _start.x + row;
			vertices[(row * 16 + col) * 5 + 3] = _start.y + col;
			vertices[(row * 16 + col) * 5 + 4] = glm::floor(z + (perlin.noise2D_01(double(_start.x + row) / 100, double(_start.y + col) / 100) - 0.5) * 100);
		}
	}
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Chunk::setVisibility( bool value )
{
	_isVisible = value;
}

bool Chunk::isInChunk( glm::vec2 pos )
{
	// std::cout << "x: " << pos.x << ", y: " << pos.y << std::endl;
	// std::cout << "checking chunk " << _start.x << ", " << _start.y << std::endl;
	return (pos.x >= _start.x && pos.x < _start.x + 16.0f && pos.y >= _start.y && pos.y < _start.y + 16.0f);
}

void Chunk::setup_array_buffer( GLfloat z )
{
    glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	GLfloat *vertices = new GLfloat[16 * 16 * 5]; // blocktype, adjacents blocks, X Y Z
	// GLfloat points[] = {
	// 	1.0f, 8.0f, 0.0f,  0.0f, z, // blocktype, adjacents blocks, X Y Z
	// 	1.0f, 0.0f, 2.5f,  3.5f, z,
	// 	1.0f, 4.0f, 1.0f, 0.0f, z,
	// 	// 1.0f, -0.45f, -0.45f, 0.0f
	// };
	// std::cout << "total alloc of vertices: " << _number_vertices * 12 << std::endl;
	fill_vertex_array(vertices, z);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, 16 * 16 * 5 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	// glBufferData(GL_ARRAY_BUFFER, _number_vertices * 12 * sizeof(float), vertices, GL_STATIC_DRAW);

	delete [] vertices;
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

	check_glstate("chunk succesfully added");
}

void Chunk::drawArray( void )
{
	if (!_isVisible) {
		return ;
	}
    glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, 16 * 16);
}
