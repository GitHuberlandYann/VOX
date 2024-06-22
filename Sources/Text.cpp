#include "utils.h"
#include "Text.hpp"
#include "Settings.hpp"
#include "logs.hpp"

Text::Text( void ) : _texture(0)
{
}

Text::~Text( void )
{
	MAINLOG(LOG("Destructor of Text called"));

	if (_texture) {
		glDeleteTextures(1, &_texture);
	}
	_shader.deleteProgram();

	_vabo.deleteBuffers();

	_texts.clear();
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void Text::addQuads( int spec, int posX, int posY, int width, int height, unsigned color, int depth )
{
	_texts.push_back({spec + (0 << 8) + (0 << 9) + (depth << 24), color, posX,         posY});
	_texts.push_back({spec + (1 << 8) + (0 << 9) + (depth << 24), color, posX + width, posY});
	_texts.push_back({spec + (0 << 8) + (1 << 9) + (depth << 24), color, posX,         posY + height});

	_texts.push_back({spec + (1 << 8) + (0 << 9) + (depth << 24), color, posX + width, posY});
	_texts.push_back({spec + (1 << 8) + (1 << 9) + (depth << 24), color, posX + width, posY + height});
	_texts.push_back({spec + (0 << 8) + (1 << 9) + (depth << 24), color, posX,         posY + height});
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Text::setupShader( void )
{
	_shader.createProgram(Settings::Get()->getString(settings::strings::text_vertex_shader), "",
						Settings::Get()->getString(settings::strings::text_fragment_shader));
	
	_shader.bindFragData(settings::consts::shader::outColor, "outColor");
	_shader.bindAttribute(settings::consts::shader::attributes::specifications, "specifications");
	_shader.bindAttribute(settings::consts::shader::attributes::color, "color");
	_shader.bindAttribute(settings::consts::shader::attributes::position, "position");
	_shader.linkProgram();

	check_glstate("text_Shader program successfully created", true);

	glUniform1i(glGetUniformLocation(_shader.getProgram(), "win_width"), WIN_WIDTH);
	glUniform1i(glGetUniformLocation(_shader.getProgram(), "win_height"), WIN_HEIGHT);

	_vabo.genBuffers();
	_vabo.addAttribute(settings::consts::shader::attributes::specifications, 1, GL_INT);
	_vabo.addAttribute(settings::consts::shader::attributes::color, 1, GL_UNSIGNED_INT);
	_vabo.addAttribute(settings::consts::shader::attributes::position, 2, GL_INT);
}

void Text::loadTexture( void )
{
	if (_texture) {
		glDeleteTextures(1, &_texture);
	}
	glGenTextures(1, &_texture);

	_shader.useProgram();
	loadTextureShader(1, _texture, Settings::Get()->getString(settings::strings::ascii_atlas));
	glUniform1i(glGetUniformLocation(_shader.getProgram(), "asciiAtlas"), 1); // sampler2D #index in fragment shader
}

void Text::updateWinSize( void )
{
	_shader.useProgram();
	glUniform1i(glGetUniformLocation(_shader.getProgram(), "win_width"), WIN_WIDTH);
	glUniform1i(glGetUniformLocation(_shader.getProgram(), "win_height"), WIN_HEIGHT);
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

void Text::addText( int posX, int posY, int font_size, unsigned color, int depth, std::string str )
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
			addQuads(c, posX, posY, font_size, font_size, color, depth);
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

void Text::addCenteredText( int left, int top, int width, int height, int font_size, bool shadow, int depth, std::string str )
{
	int text_width = Utils::Text::textWidth(font_size, str);
	if (shadow) {
		int offset = font_size / 8;
		addText(left + offset + (width - text_width) / 2, top + offset + (height - font_size) / 2, font_size, TEXT::BLACK, depth + 1, str);
	}
	addText(left + (width - text_width) / 2, top + (height - font_size) / 2, font_size, TEXT::WHITE, depth, str);
}

void Text::toScreen( void )
{
	size_t tSize = _texts.size();
	if (tSize == 0) {
		return ;
	}

	_vabo.uploadData(tSize, &_texts[0].spec);

	check_glstate("Text::toScreen", false);

	_shader.useProgram();
	glDrawArrays(GL_TRIANGLES, 0, tSize);

	_texts.clear();
	_texts.reserve(tSize);
}
