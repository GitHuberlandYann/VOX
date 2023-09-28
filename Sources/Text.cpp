#include "vox.h"

Text::Text( void ) : _textures(NULL)
{

}

Text::~Text( void )
{
	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);

	if (_textures) {
		glDeleteTextures(1, _textures);
		delete [] _textures;
	}
	glDeleteProgram(_shaderProgram);
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Text::setup_shader( void )
{
	std::string vertex_shader_data = get_file_content("Sources/Shaders/text_vertex.glsl");
	char *vertexSource = &vertex_shader_data[0];

	_vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(_vertexShader, 1, &vertexSource, NULL);
	compile_shader(_vertexShader, "text_vertex");

	std::string geometry_shader_data = get_file_content("Sources/Shaders/text_geometry.glsl");
	char *geometrySource = &geometry_shader_data[0];

	_geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(_geometryShader, 1, &geometrySource, NULL);
	compile_shader(_geometryShader, "text_geometry");

	std::string fragment_shader_data = get_file_content("Sources/Shaders/text_fragment.glsl");
	char *fragmentSource = &fragment_shader_data[0];

	_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(_fragmentShader, 1, &fragmentSource, NULL);
	compile_shader(_fragmentShader, "text_fragment");

	// Combining shaders into a program
	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, _vertexShader);
	glAttachShader(_shaderProgram, _geometryShader);
	glAttachShader(_shaderProgram, _fragmentShader);

	glBindFragDataLocation(_shaderProgram, 0, "outColor");

	glBindAttribLocation(_shaderProgram, TEXT_POSATTRIB, "position");
	glBindAttribLocation(_shaderProgram, TEXT_SIZEATTRIB, "size");
	glBindAttribLocation(_shaderProgram, TEXT_CHARATTRIB, "character");

	glLinkProgram(_shaderProgram);
	glUseProgram(_shaderProgram);

	glDeleteShader(_fragmentShader);
	glDeleteShader(_geometryShader);
    glDeleteShader(_vertexShader);

	check_glstate("text_Shader program successfully created");

	glUniform1i(glGetUniformLocation(_shaderProgram, "window_width"), WIN_WIDTH);
	glUniform1i(glGetUniformLocation(_shaderProgram, "window_height"), WIN_HEIGHT);

	_uniColor = glGetUniformLocation(_shaderProgram, "color");
}

void Text::load_texture( void )
{
	_textures = new GLuint[1];
	glGenTextures(1, _textures);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _textures[0]);

	// load image
	t_tex *texture = new t_tex;
	texture->content = SOIL_load_image("Resources/asciiAtlas.png", &texture->width, &texture->height, 0, SOIL_LOAD_RGBA);
	if (!texture->content) {
		std::cerr << "failed to load image " << "Resources/asciiAtlas.png" << " because:" << std::endl << SOIL_last_result() << std::endl;
		exit(1);
	}

	// load image as texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, texture->content);

	glUniform1i(glGetUniformLocation(_shaderProgram, "asciiAtlas"), 1); // sampler2D #index in fragment shader
			
	// set settings for texture wraping and size modif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST because pixel art, otherwise GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (texture) {
		SOIL_free_image_data(texture->content);
	}
	delete texture;

	check_glstate("Succesfully loaded Resources/asciiAtlas.png to shader\n");
}

void Text::displayText( int posX, int posY, int font_size, glm::vec3 color ,std::string str )
{
	int nb_points = str.size();
	for (size_t index = 0; index < str.size(); index++) {
		if (str[index] == '\n' || str[index] == ' ' || str[index] == '\t') {
			nb_points--;
		}
	}
	if (!nb_points) {
		return ;
	}

	int startX = posX;
	GLint *vertices = new GLint[nb_points * 4];
	for (size_t i = 0, charLine = 0, index = 0; i < str.size(); i++) {
		if (str[i] == '\n') {
			posY += 1.2f * font_size;
			posX = startX;
			charLine = 0;
		} else if (str[i] == ' ') {
			posX += font_size;
			++charLine;
		} else if (str[i] == '\t') {
			int currentTab = 1 + charLine / 4;
			posX = startX + (currentTab * 4) * font_size;
			charLine += 4 - charLine % 4;
		} else {
			vertices[index] = posX;
			vertices[index + 1] = posY;
			vertices[index + 2] = font_size;
			vertices[index + 3] = str[i];
			index += 4;
			char c = str[i];
			if (c == 'i' || c == '.' || c == ':' || c == '!' || c == '\'' || c == ',' || c == ';' || c == '|' || c == '`') {
				posX += font_size * 0.5;
			} else if (c == 'I' || c == '[' || c == ']' || c == '"' || c == '*') {
				posX += font_size * 0.6;	
			} else if (c == 'l' || c == 't' || c == '(' || c == ')' || c == '<' || c == '>' || c == '{' || c == '}') {
				posX += font_size * 0.7;
			} else {
				posX += font_size;
			}
			++charLine;
		}
	}

	glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, nb_points * 4 * sizeof(GLint), vertices, GL_STATIC_DRAW);
	delete [] vertices;
    
    glEnableVertexAttribArray(TEXT_POSATTRIB);
	glVertexAttribIPointer(TEXT_POSATTRIB, 2, GL_INT, 4 * sizeof(GLint), 0);
	// check_glstate("text_posattrib successfully set");
	
	glEnableVertexAttribArray(TEXT_SIZEATTRIB);
	glVertexAttribIPointer(TEXT_SIZEATTRIB, 1, GL_INT, 4 * sizeof(GLint), (void *)(2 * sizeof(GLint)));
	// check_glstate("text_sizeattrib successfully set");

	glEnableVertexAttribArray(TEXT_CHARATTRIB);
	glVertexAttribIPointer(TEXT_CHARATTRIB, 1, GL_INT, 4 * sizeof(GLint), (void *)(3 * sizeof(GLint)));
	// check_glstate("text_charattrib successfully set");

	check_glstate("NO");

	glUseProgram(_shaderProgram);
	GLfloat vecptr[3] = {color.x, color.y, color.z};
	glUniform3fv(_uniColor, 1, vecptr);
	glDrawArrays(GL_POINTS, 0, nb_points);
}
