#include "vox.h"

UI::UI( void ) : _vertices(NULL)
{

}

UI::~UI( void )
{
    glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void UI::setup_array_buffer( void )
{
    _nb_points = 2;
    (void)_vertices;
    // delete [] _vertices;
    // _vertices = new GLfloat[_nb_points * 7];
    GLfloat vertices[] = { // pos x y size x y color r g b
        -0.01f, 0.002f, 0.02f, 0.004f, 1.0f, 0.0f, 0.0f,
        -0.001f, 0.02f, 0.002f, 0.04f, 1.0f, 0.0f, 0.0f
    };

    glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _nb_points * 7 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(UI_POSATTRIB);
	glVertexAttribPointer(UI_POSATTRIB, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);
	// check_glstate("blockAttrib successfully set");
	
	glEnableVertexAttribArray(UI_SIZEATTRIB);
	glVertexAttribPointer(UI_SIZEATTRIB, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
	// check_glstate("adjAttrib successfully set");

	glEnableVertexAttribArray(UI_COLORATTRIB);
	glVertexAttribPointer(UI_COLORATTRIB, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void *)(4 * sizeof(GLfloat)));
	// check_glstate("posAttrib successfully set");

	check_glstate("NO");
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void UI::setup( void )
{
    glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
}

void UI::drawUserInterface( void )
{
    setup_array_buffer();
    glBindVertexArray(_vao); // this is the costly operation, chunk_size up == fps down
	glDrawArrays(GL_POINTS, 0, _nb_points);
}
