#include "vox.h"

Chunk::Chunk( glm::vec2 start ) : _start(start)
{
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
	for (int row = 0; row < 16; row++) {
		for (int col = 0; col < 16; col++) {
			vertices[(row * 16 + col) * 5] = 1.0f;
			vertices[(row * 16 + col) * 5 + 1] = 4.0f * (row != 0) + 8.0f * (row != 15) + 2.0f * (col != 15) + 1.0f * (col != 0);
			vertices[(row * 16 + col) * 5 + 2] = _start.x + row;
			vertices[(row * 16 + col) * 5 + 3] = _start.y + col;
			vertices[(row * 16 + col) * 5 + 4] = z;
		}
	}
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

bool Chunk::isInChunk( glm::vec3 pos )
{
	// std::cout << "x: " << x << ", y: " << y << std::endl;
	return (pos.x >= _start.x - 0.5f && pos.x < _start.x + 15.5f && pos.y >= _start.y - 0.5f && pos.y < _start.y + 15.5f);
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
	check_glstate("Vertex buffer successfully created");

	glEnableVertexAttribArray(BLOCKATTRIB);
	glVertexAttribPointer(BLOCKATTRIB, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	check_glstate("blockAttrib successfully set");
	
	glEnableVertexAttribArray(ADJATTRIB);
	glVertexAttribPointer(ADJATTRIB, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(sizeof(GLfloat)));
	check_glstate("adjAttrib successfully set");

	glEnableVertexAttribArray(POSATTRIB);
	glVertexAttribPointer(POSATTRIB, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
	check_glstate("posAttrib successfully set");
}

void Chunk::drawArray( void )
{
    glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, 16 * 16);
}
