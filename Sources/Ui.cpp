#include "vox.h"

UI::UI( void ) : _textures(NULL), _vaoSet(false)
{
	_text = new Text();
}

UI::~UI( void )
{
    glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
	
	if (_textures) {
		glDeleteTextures(1, _textures);
		delete [] _textures;
	}
	
	glDeleteProgram(_shaderProgram);

	delete _text;
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void UI::load_texture( void )
{
	_textures = new GLuint[1];
	glGenTextures(1, _textures);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _textures[0]);

	// load image
	t_tex *texture = new t_tex;
	texture->content = SOIL_load_image("Resources/uiAtlas.png", &texture->width, &texture->height, 0, SOIL_LOAD_RGBA);
	if (!texture->content) {
		std::cerr << "failed to load image " << "Resources/uiAtlas.png" << " because:" << std::endl << SOIL_last_result() << std::endl;
		exit(1);
	}

	// load image as texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, texture->content);

	glUniform1i(glGetUniformLocation(_shaderProgram, "blockAtlas"), 0); // we reuse texture from main shader
	glUniform1i(glGetUniformLocation(_shaderProgram, "uiAtlas"), 2); // sampler2D #index in fragment shader
			
	// set settings for texture wraping and size modif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST because pixel art, otherwise GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (texture) {
		SOIL_free_image_data(texture->content);
	}
	delete texture;

	check_glstate("Succesfully loaded Resources/uiAtlas.png to shader\n");

}

void UI::setup_array_buffer( int slot )
{
    _nb_points = 3 + 10 + 8 + 10 + 4 + 10 + 2;
	int mult = 4;
    GLint vertices[] = { // pos: x y width height textcoord: x y width height
        WIN_WIDTH / 2 - 16, WIN_HEIGHT / 2 - 16, 32, 32, 0, 0, 16, 16, // crosshair
		(WIN_WIDTH - (182 * mult)) / 2, WIN_HEIGHT - (22 * mult) * 2, 182 * mult, 22 * mult, 0, 25, 182, 22,  // hot bar
		(WIN_WIDTH - (182 * mult)) / 2 + (20 * slot * mult) - mult, WIN_HEIGHT - (22 * mult) * 2 - mult, 24 * mult, 24 * mult, 0, 47, 24, 24,  // slot select
		(WIN_WIDTH - (182 * mult)) / 2 + mult, WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 0, 16, 9, 9,  // hearts
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (1 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 0, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (2 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 0, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (3 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 0, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (4 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 0, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (5 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 0, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (6 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 0, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (7 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 0, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (8 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 0, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (9 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 0, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult, WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 9, 16, 9, 9,  // filling hearts
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (1 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 9, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (2 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 9, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (3 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 9, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (4 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 9, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (5 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 9, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (6 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 9, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (7 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 18, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult, WIN_HEIGHT - (22 * mult) * 2 - (2 * 8 * mult) - (mult * 3), 8 * mult, 8 * mult, 27, 16, 9, 9,  // armor
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (1 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (2 * 8 * mult) - (mult * 3), 8 * mult, 8 * mult, 27, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (2 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (2 * 8 * mult) - (mult * 3), 8 * mult, 8 * mult, 27, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (3 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (2 * 8 * mult) - (mult * 3), 8 * mult, 8 * mult, 27, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (4 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (2 * 8 * mult) - (mult * 3), 8 * mult, 8 * mult, 27, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (5 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (2 * 8 * mult) - (mult * 3), 8 * mult, 8 * mult, 27, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (6 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (2 * 8 * mult) - (mult * 3), 8 * mult, 8 * mult, 27, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (7 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (2 * 8 * mult) - (mult * 3), 8 * mult, 8 * mult, 27, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (8 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (2 * 8 * mult) - (mult * 3), 8 * mult, 8 * mult, 27, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (9 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (2 * 8 * mult) - (mult * 3), 8 * mult, 8 * mult, 27, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult, WIN_HEIGHT - (22 * mult) * 2 - (2 * 8 * mult) - (mult * 3), 8 * mult, 8 * mult, 36, 16, 9, 9,  // filling armor
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (1 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (2 * 8 * mult) - (mult * 3), 8 * mult, 8 * mult, 36, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (2 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (2 * 8 * mult) - (mult * 3), 8 * mult, 8 * mult, 36, 16, 9, 9,
		(WIN_WIDTH - (182 * mult)) / 2 + mult + (3 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (2 * 8 * mult) - (mult * 3), 8 * mult, 8 * mult, 45, 16, 9, 9,
		(WIN_WIDTH + (182 * mult)) / 2 - mult - (1 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 54, 16, 9, 9,  // food
		(WIN_WIDTH + (182 * mult)) / 2 - mult - (2 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 54, 16, 9, 9,
		(WIN_WIDTH + (182 * mult)) / 2 - mult - (3 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 54, 16, 9, 9,
		(WIN_WIDTH + (182 * mult)) / 2 - mult - (4 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 54, 16, 9, 9,
		(WIN_WIDTH + (182 * mult)) / 2 - mult - (5 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 54, 16, 9, 9,
		(WIN_WIDTH + (182 * mult)) / 2 - mult - (6 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 54, 16, 9, 9,
		(WIN_WIDTH + (182 * mult)) / 2 - mult - (7 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 54, 16, 9, 9,
		(WIN_WIDTH + (182 * mult)) / 2 - mult - (8 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 54, 16, 9, 9,
		(WIN_WIDTH + (182 * mult)) / 2 - mult - (9 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 54, 16, 9, 9,
		(WIN_WIDTH + (182 * mult)) / 2 - mult - (10 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 54, 16, 9, 9,
		(WIN_WIDTH + (182 * mult)) / 2 - mult - (1 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 63, 16, 9, 9,  // filling food
		(WIN_WIDTH + (182 * mult)) / 2 - mult - (2 * 8 * mult), WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult), 8 * mult, 8 * mult, 72, 16, 9, 9,

    };

	glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
	_vaoSet = true;

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _nb_points * 8 * sizeof(GLint), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(UI_POSATTRIB);
	glVertexAttribIPointer(UI_POSATTRIB, 4, GL_INT, 8 * sizeof(GLint), 0);
	// check_glstate("blockAttrib successfully set");
	
	glEnableVertexAttribArray(UI_TEXATTRIB);
	glVertexAttribIPointer(UI_TEXATTRIB, 4, GL_INT, 8 * sizeof(GLint), (void *)(4 * sizeof(GLint)));
	// check_glstate("adjAttrib successfully set");

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

	glBindAttribLocation(_shaderProgram, UI_POSATTRIB, "pos");
	glBindAttribLocation(_shaderProgram, UI_TEXATTRIB, "textcoord");

	glLinkProgram(_shaderProgram);
	glUseProgram(_shaderProgram);

	glDeleteShader(_fragmentShader);
	glDeleteShader(_geometryShader);
    glDeleteShader(_vertexShader);

	check_glstate("UI_Shader program successfully created\n");

	glUniform1i(glGetUniformLocation(_shaderProgram, "window_width"), WIN_WIDTH);
	glUniform1i(glGetUniformLocation(_shaderProgram, "window_height"), WIN_HEIGHT);
	load_texture();
}

void UI::drawUserInterface( std::string str, bool game_mode, int slot )
{
	if (!_vaoSet) {
		setup_array_buffer(slot);
	}
	glUseProgram(_shaderProgram);
    glBindVertexArray(_vao);
	(game_mode == SURVIVAL)
		? glDrawArrays(GL_POINTS, 0, _nb_points)
		: glDrawArrays(GL_POINTS, 0, 3);

	_text->displayText(12, 24, 12, str);
}
