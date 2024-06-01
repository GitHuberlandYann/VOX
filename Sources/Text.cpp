#include "utils.h"
#include "Text.hpp"
#include "Settings.hpp"

Text::Text( void ) : _shaderProgram(0), _textures(NULL)
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

void Text::addQuads( int spec, int posX, int posY, int width, int height, uint color )
{
	_texts.push_back({spec + (0 << 8) + (0 << 9), color, posX,         posY});
	_texts.push_back({spec + (1 << 8) + (0 << 9), color, posX + width, posY});
	_texts.push_back({spec + (0 << 8) + (1 << 9), color, posX,         posY + height});

	_texts.push_back({spec + (1 << 8) + (0 << 9), color, posX + width, posY});
	_texts.push_back({spec + (1 << 8) + (1 << 9), color, posX + width, posY + height});
	_texts.push_back({spec + (0 << 8) + (1 << 9), color, posX,         posY + height});
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Text::setup_shader( void )
{
	if (_shaderProgram) {
		glDeleteProgram(_shaderProgram);
	}
	_shaderProgram = createShaderProgram(Settings::Get()->getString(SETTINGS::STRING::TEXT_VERTEX_SHADER), "",
										Settings::Get()->getString(SETTINGS::STRING::TEXT_FRAGMENT_SHADER));
	glBindFragDataLocation(_shaderProgram, 0, "outColor");

	glBindAttribLocation(_shaderProgram, TEXT::SPECATTRIB, "specifications");
	glBindAttribLocation(_shaderProgram, TEXT::COLATTRIB, "color");
	glBindAttribLocation(_shaderProgram, TEXT::POSATTRIB, "position");

	glLinkProgram(_shaderProgram);
	glUseProgram(_shaderProgram);

	check_glstate("text_Shader program successfully created", true);

	glUniform1i(glGetUniformLocation(_shaderProgram, "win_width"), WIN_WIDTH);
	glUniform1i(glGetUniformLocation(_shaderProgram, "win_height"), WIN_HEIGHT);

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
}

void Text::load_texture( void )
{
	if (_textures) {
		glDeleteTextures(1, _textures);
		delete [] _textures;
	}
	_textures = new GLuint[1];
	glGenTextures(1, _textures);

	glUseProgram(_shaderProgram);
	loadTextureShader(1, _textures[0], Settings::Get()->getString(SETTINGS::STRING::ASCII_ATLAS));
	glUniform1i(glGetUniformLocation(_shaderProgram, "asciiAtlas"), 1); // sampler2D #index in fragment shader
}

void Text::updateWinSize( void )
{
	glUseProgram(_shaderProgram);
	glUniform1i(glGetUniformLocation(_shaderProgram, "win_width"), WIN_WIDTH);
	glUniform1i(glGetUniformLocation(_shaderProgram, "win_height"), WIN_HEIGHT);
}

int Utils::Text::textWidth( int font_size, std::string str, int limit )
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

void Text::addText( int posX, int posY, int font_size, uint color, std::string str )
{
	int startX = posX;
	char c;
	for (size_t i = 0, charLine = 0; i < str.size(); i++) {
		c = str[i];
		if (c == '\n') {
			posY += 1.2f * font_size;
			posX = startX;
			charLine = 0;
		} else if (c == ' ') {
			posX += font_size;
			++charLine;
		} else if (c == '\t') {
			charLine += 4 - (charLine & 3);
			posX = startX + charLine * font_size;
		} else {
			addQuads(c, posX, posY, font_size, font_size, color);
			if (c == 'i' || c == '.' || c == ':' || c == '!' || c == '\'' || c == ',' || c == ';' || c == '|' || c == '`') {
				posX += font_size * 0.5f;
			} else if (c == 'I' || c == '[' || c == ']' || c == '"' || c == '*') {
				posX += font_size * 0.6f;	
			} else if (c == 'l' || c == 't' || c == '(' || c == ')' || c == '<' || c == '>' || c == '{' || c == '}') {
				posX += font_size * 0.7f;
			} else {
				posX += font_size;
			}
			++charLine;
		}
	}
}

void Text::addCenteredText( int left, int top, int width, int height, int font_size, bool shadow, std::string str )
{
	int text_width = Utils::Text::textWidth(font_size, str);
	if (shadow) {
		int offset = font_size / 8;
		addText(left + offset + (width - text_width) / 2, top + offset + (height - font_size) / 2, font_size, TEXT::BLACK, str);
	}
	addText(left + (width - text_width) / 2, top + (height - font_size) / 2, font_size, TEXT::WHITE, str);
}

void Text::toScreen( void )
{
	size_t tSize = _texts.size();
	if (tSize == 0) {
		return ;
	}

    glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, tSize * 4 * sizeof(GLint), &_texts[0].spec, GL_STATIC_DRAW);

	glEnableVertexAttribArray(TEXT::SPECATTRIB);
	glVertexAttribIPointer(TEXT::SPECATTRIB, 1, GL_INT, 4 * sizeof(GLint), 0);
	glEnableVertexAttribArray(TEXT::COLATTRIB);
	glVertexAttribIPointer(TEXT::COLATTRIB, 1, GL_UNSIGNED_INT, 4 * sizeof(GLint), (void *)(1 * sizeof(GLint)));
	glEnableVertexAttribArray(TEXT::POSATTRIB);
	glVertexAttribIPointer(TEXT::POSATTRIB, 2, GL_INT, 4 * sizeof(GLint), (void *)(2 * sizeof(GLint)));

	check_glstate("Text::toScreen", false);

	glUseProgram(_shaderProgram);
	glDrawArrays(GL_TRIANGLES, 0, tSize);

	_texts.clear();
	_texts.reserve(tSize);
}
