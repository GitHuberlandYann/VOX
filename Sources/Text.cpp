#include "utils.h"
#include "Text.hpp"

Text::Text( void ) : _textures(NULL)
{
}

Text::~Text( void )
{
	std::cout << "Destructor of Text called" << std::endl;
	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);

	if (_textures) {
		glDeleteTextures(1, _textures);
		delete [] _textures;
	}
	glDeleteProgram(_shaderProgram);

	_texts.clear();
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Text::setup_shader( void )
{
	_shaderProgram = createShaderProgram("text_vertex", "text_geometry", "text_fragment");

	glBindFragDataLocation(_shaderProgram, 0, "outColor");

	glBindAttribLocation(_shaderProgram, TEXT_POSATTRIB, "position");
	glBindAttribLocation(_shaderProgram, TEXT_SIZEATTRIB, "size");
	glBindAttribLocation(_shaderProgram, TEXT_CHARATTRIB, "character");
	glBindAttribLocation(_shaderProgram, TEXT_COLORATTRIB, "color");

	glLinkProgram(_shaderProgram);
	glUseProgram(_shaderProgram);

	check_glstate("text_Shader program successfully created", true);

	glUniform1i(glGetUniformLocation(_shaderProgram, "window_width"), WIN_WIDTH);
	glUniform1i(glGetUniformLocation(_shaderProgram, "window_height"), WIN_HEIGHT);

	// _uniColor = glGetUniformLocation(_shaderProgram, "color");
}

void Text::load_texture( void )
{
	_textures = new GLuint[1];
	glGenTextures(1, _textures);

	loadTextureShader(1, _textures[0], "Resources/asciiAtlas.png");
	glUniform1i(glGetUniformLocation(_shaderProgram, "asciiAtlas"), 1); // sampler2D #index in fragment shader
}

int Text::textWidth( int font_size, std::string str, int limit )
{
	int res = 0;
	for (size_t i = 0, charLine = 0; str[i]; ++i, ++charLine) {
		if (limit != -1 && static_cast<int>(i) >= limit) return (res);
		switch (str[i]) {
			case '\n':
				break ;
			case '\t':
				charLine += 4 - (charLine & 3);
				res = charLine * font_size;
				break ;
			case 'i':
			case '.':
			case ':':
			case '!':
			case '\'':
			case ',':
			case ';':
			case '|':
			case '`':
				res += font_size * 0.5f;
				break ;
			case 'I':
			case '[':
			case ']':
			case '"':
			case '*':
				res += font_size * 0.6f;
				break ;
			case 'l':
			case 't':
			case '(':
			case ')':
			case '<':
			case '>':
			case '{':
			case '}':
				res += font_size * 0.7f;
				break ;
			default:
				res += font_size;
				break ;
		}
	}
	return (res);
}

void Text::addText( int posX, int posY, int font_size, bool white, std::string str )
{
	int startX = posX;
	for (size_t i = 0, charLine = 0; i < str.size(); i++) {
		if (str[i] == '\n') {
			posY += 1.2f * font_size;
			posX = startX;
			charLine = 0;
		} else if (str[i] == ' ') {
			posX += font_size;
			++charLine;
		} else if (str[i] == '\t') {
			charLine += 4 - (charLine & 3);
			posX = startX + charLine * font_size;
		} else {
			_texts.push_back(posX);
			_texts.push_back(posY);
			_texts.push_back(font_size);
			_texts.push_back(str[i]);
			_texts.push_back(white);
			char c = str[i];
			if (c == 'i' || c == '.' || c == ':' || c == '!' || c == '\'' || c == ',' || c == ';' || c == '|' || c == '`') { // TODO might want to use strchr here
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
}

void Text::addCenteredText( int left, int top, int width, int height, int font_size, bool shadow, std::string str )
{
	int text_width = textWidth(font_size, str);
	if (shadow) {
		int offset = font_size / 8;
		addText(left + offset + (width - text_width) / 2, top + offset + (height - font_size) / 2, font_size, false, str);
	}
	addText(left + (width - text_width) / 2, top + (height - font_size) / 2, font_size, true, str);
}


void Text::toScreen( void )
{
	size_t tSize = _texts.size();
	if (tSize == 0) {
		return ;
	}

	glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, tSize * sizeof(GLint), &_texts[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(TEXT_POSATTRIB);
	glVertexAttribIPointer(TEXT_POSATTRIB, 2, GL_INT, 5 * sizeof(GLint), 0);
	// check_glstate("text_posattrib successfully set");
	
	glEnableVertexAttribArray(TEXT_SIZEATTRIB);
	glVertexAttribIPointer(TEXT_SIZEATTRIB, 1, GL_INT, 5 * sizeof(GLint), (void *)(2 * sizeof(GLint)));
	// check_glstate("text_sizeattrib successfully set");

	glEnableVertexAttribArray(TEXT_CHARATTRIB);
	glVertexAttribIPointer(TEXT_CHARATTRIB, 1, GL_INT, 5 * sizeof(GLint), (void *)(3 * sizeof(GLint)));
	// check_glstate("text_charattrib successfully set");

	glEnableVertexAttribArray(TEXT_COLORATTRIB);
	glVertexAttribIPointer(TEXT_COLORATTRIB, 1, GL_INT, 5 * sizeof(GLint), (void *)(4 * sizeof(GLint)));

	check_glstate("Text::toScreen", false);

	glUseProgram(_shaderProgram);
	glDrawArrays(GL_POINTS, 0, tSize / 5);

	_texts.clear();
}
