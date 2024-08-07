#include "utils.h"
#include "Buffer.hpp"
#include "logs.hpp"

Buffer::Buffer( void )
    : _vao(0), _vbo(0), _size(0)
{

}

Buffer::~Buffer( void )
{

}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Buffer::genBuffers( void )
{
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);

	_attributes.clear();
	_size = 0;
}

void Buffer::addAttribute( int index, size_t size, GLenum type )
{
	_attributes.push_back({index, size, type});
	_size += size * sizeof(GLint);
}

void Buffer::uploadData( size_t size, const void* data )
{
	if (!size) {
		return ;
	}
	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, size * _size, data, GL_STATIC_DRAW);

	size_t offset = 0;
	for (auto attrib : _attributes) {
		glEnableVertexAttribArray(attrib.index);
		switch (attrib.type) {
			case GL_INT:
			case GL_UNSIGNED_INT:
				glVertexAttribIPointer(attrib.index, attrib.size, attrib.type, _size, (void*)(offset));
				break ;
			case GL_FLOAT:
				glVertexAttribPointer(attrib.index, attrib.size, GL_FLOAT, GL_FALSE, _size, (void *)(offset));
				break ;
			default:
				LOGERROR("Buffer::updloadData unrecognized type: " << attrib.type);
				break ;
		}
		offset += attrib.size * sizeof(GLint);
	}
}

void Buffer::bindVertexArray( void )
{
	glBindVertexArray(_vao);
}

void Buffer::deleteBuffers( void )
{
	if (_vao) {
		glDeleteBuffers(1, &_vbo);
		glDeleteVertexArrays(1, &_vao);
	}
}
