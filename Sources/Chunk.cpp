#include "vox.h"

Chunk::Chunk( void )
{

}

Chunk::~Chunk( void )
{
    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Chunk::setup_array_buffer( GLfloat z )
{
    glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// _number_vertices = parser->get_number_vertices();

	// GLfloat *vertices = new GLfloat[_number_vertices * 12]; // num, X Y Z, R G B, U V, nX nY nZ
	GLfloat points[] = {
		1.0f, 8.0f, 0.0f,  0.0f, z, // blocktype, adjacents blocks, X Y Z
		1.0f, 0.0f, 2.5f,  3.5f, z,
		1.0f, 4.0f, 1.0f, 0.0f, z,
		// 1.0f, -0.45f, -0.45f, 0.0f
	};
	// std::cout << "total alloc of vertices: " << _number_vertices * 12 << std::endl;
	// parser->fill_vertex_array(vertices);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	// glBufferData(GL_ARRAY_BUFFER, _number_vertices * 12 * sizeof(float), vertices, GL_STATIC_DRAW);

	// delete [] vertices;
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
	glDrawArrays(GL_POINTS, 0, 3);
}
