#include "vox.h"

UI::UI( void ) : _vertices(NULL), _vaoSet(false)
{
	_text = new Text();
}

UI::~UI( void )
{
    glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
	glDeleteProgram(_shaderProgram);

	delete _text;
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
        -0.01f, -0.002f, 0.02f, 0.004f, 1.0f, 0.0f, 0.0f,
        -0.001f, -0.02f, 0.002f, 0.04f, 1.0f, 0.0f, 0.0f
    };

	glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
	_vaoSet = true;

	glGenBuffers(1, &_vbo);
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

void UI::setup_shader( void )
{
	_text->setup_shader();
	_text->load_texture();

	std::string vertex_shader_data = get_file_content("Sources/Shaders/ui_vertex.glsl");
	char *vertexSource = &vertex_shader_data[0];

	_vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(_vertexShader, 1, &vertexSource, NULL);
	compile_shader(_vertexShader, "ui_vertex");

	std::string geometry_shader_data = get_file_content("Sources/Shaders/ui_geometry.glsl");
	char *geometrySource = &geometry_shader_data[0];

	_geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(_geometryShader, 1, &geometrySource, NULL);
	compile_shader(_geometryShader, "ui_geometry");

	std::string fragment_shader_data = get_file_content("Sources/Shaders/ui_fragment.glsl");
	char *fragmentSource = &fragment_shader_data[0];

	_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(_fragmentShader, 1, &fragmentSource, NULL);
	compile_shader(_fragmentShader, "ui_fragment");

	// Combining shaders into a program
	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, _vertexShader);
	glAttachShader(_shaderProgram, _geometryShader);
	glAttachShader(_shaderProgram, _fragmentShader);

	glBindFragDataLocation(_shaderProgram, 0, "outColor");

	glBindAttribLocation(_shaderProgram, UI_POSATTRIB, "position");
	glBindAttribLocation(_shaderProgram, UI_SIZEATTRIB, "size");
	glBindAttribLocation(_shaderProgram, UI_COLORATTRIB, "color");

	glLinkProgram(_shaderProgram);
	glUseProgram(_shaderProgram);

	glDeleteShader(_fragmentShader);
	glDeleteShader(_geometryShader);
    glDeleteShader(_vertexShader);

	check_glstate("UI_Shader program successfully created\n");
}

void UI::drawUserInterface( std::string str )
{
	if (!_vaoSet) {
		setup_array_buffer();
	}
	glUseProgram(_shaderProgram);
    glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	_text->displayText(12, 24, 12, str);
}
