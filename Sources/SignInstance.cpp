#include "SignInstance.hpp"
#include <cassert>

SignInstance::SignInstance( Chunk *chunk, int value, glm::ivec3 pos )
	: _value(value), _pos(pos), _chunk(chunk)
{

}

SignInstance::~SignInstance( void )
{

}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

int SignInstance::getValue( void )
{
	return (_value);
}

std::string SignInstance::getContent( int index )
{
	assert(((index >= 0 && index < 4) && "SignInstance::getContent out of bound"));

	return (_content[index]);
}

void SignInstance::setContent( std::vector<std::string> content )
{
	size_t cSize = content.size();
	for (size_t index = 0; index < cSize && index < 4; ++index) {
		_content[index] = content[index];
	}
}

void SignInstance::setChunk( Chunk *chunk )
{
	_chunk = chunk;
}

void SignInstance::displayText( std::vector<t_shaderInput> &partArr )
{
	if (!_chunk) {
		return ;
	}

	glm::vec3 right, front, up;
	switch ((_value >> 9) & 0x7) {
		case (face_dir::MINUSX):
			right = glm::vec3( 0, 1, 0);
			front = glm::vec3(-1, 0, 0);
			up    = glm::vec3( 0, 0, 1);
			break ;
		case (face_dir::PLUSX):
			right = glm::vec3(0, -1, 0);
			front = glm::vec3(1,  0, 0);
			up    = glm::vec3(0,  0, 1);
			break ;
		case (face_dir::MINUSY):
			right = glm::vec3(-1,  0, 0);
			front = glm::vec3( 0, -1, 0);
			up    = glm::vec3( 0,  0, 1);
			break ;
		case (face_dir::PLUSY):
			right = glm::vec3(1, 0, 0);
			front = glm::vec3(0, 1, 0);
			up    = glm::vec3(0, 0, 1);
			break ;
		default:
			std::cout << "ERROR signInstance::displayText orientation" << std::endl;
			return ;
	}
	glm::vec3 topLeft = glm::vec3(_chunk->getStartX(), _chunk->getStartY(), 0) + _pos + glm::vec3(0.5f, 0.5f, 0.5f) + front * (0.5f - 1.751f * ONE16TH) + up * 0.225f;

	for (size_t index = 0; index < 4; ++index) {
		std::string str = _content[index];

		glm::vec3 startLine = topLeft - right * static_cast<float>(Utils::Text::textWidth(7, str) * 0.5f * 0.01f) - up * static_cast<float>(index) * 0.1f;
		char c;
		for (size_t i = 0, charLine = 0; i < str.size(); i++) {
			c = str[i];
			if (c == ' ') {
				startLine += 7.0f * right * 0.01f;
				++charLine;
			} else {
				int spec = 128 + 8 * (c & 0xF) + ((8 * (c >> 4)) << 8) + (1 << 19);  // + (_chunk->getLightLevel(_pos.x, _pos.y, _pos.z) << 24); // (1 << 19) = atlas index
				partArr.push_back({spec, startLine});
				partArr.push_back({spec + 8 + (1 << 17), startLine + 7.0f * right * 0.01f});
				partArr.push_back({spec + (8 << 8) + (1 << 18), startLine - 7.0f * up * 0.01f});

				partArr.push_back({spec + 8 + (1 << 17), startLine + 7.0f * right * 0.01f});
				partArr.push_back({spec + 8 + (1 << 17) + (8 << 8) + (1 << 18), startLine + 7.0f * 0.01f * right - 7.0f * up * 0.01f});
				partArr.push_back({spec + (8 << 8) + (1 << 18), startLine - 7.0f * up * 0.01f});

				if (c == 'i' || c == '.' || c == ':' || c == '!' || c == '\'' || c == ',' || c == ';' || c == '|' || c == '`') {
					startLine += 7.0f * right * 0.01f * 0.5f;
				} else if (c == 'I' || c == '[' || c == ']' || c == '"' || c == '*') {
					startLine += 7.0f * right * 0.01f * 0.6f;	
				} else if (c == 'l' || c == 't' || c == '(' || c == ')' || c == '<' || c == '>' || c == '{' || c == '}') {
					startLine += 7.0f * right * 0.01f * 0.7f;
				} else {
					startLine += 7.0f * right * 0.01f;
				}
				++charLine;
			}
		}
	}
}