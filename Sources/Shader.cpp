#include "utils.h"
#include "Shader.hpp"

Shader::Shader( void )
    : _program(0)
{

}

Shader::~Shader( void )
{

}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

/**
 * @brief sets uniform at given index to result of glGetUniformLocation(_program, location)
 * if index > _uniforms.size, expend _uniform vector
 */
void Shader::setUniformLocation( size_t index, std::string location )
{
    GLint uniform = glGetUniformLocation(_program, location.c_str());

    // std::cout << "uniform for loc " << location << " at index " << index << " is " << uniform << ", uni size was " << _uniforms.size() << std::endl;
    while (index >= _uniforms.size()) {
        _uniforms.push_back(0);
    }

    _uniforms[index] = uniform;
}

/**
 * @brief return uniform stored at given index in _uniforms after calling glUseProgram
 */
GLint Shader::getUniform( size_t index )
{
    assert((index < _uniforms.size() && "Shader::getUniform out of bound"));
    glUseProgram(_program);
    return (_uniforms[index]);
}

GLuint Shader::getProgram( void )
{
    return (_program);
}

void Shader::useProgram( void )
{
    glUseProgram(_program);
}

/**
 * @brief create or recreate shader program and return program created
 */
GLuint Shader::createProgram( std::string vertex, std::string geometry, std::string fragment )
{
    if (_program) {
		glDeleteProgram(_program);
	}
	_program = utils::shader::createShaderProgram(vertex, geometry, fragment);
    return (_program);
}

void Shader::bindFragData( int index, const char* location )
{
	glBindFragDataLocation(_program, index, location);
}

void Shader::bindAttribute( int index, const char* location )
{
	glBindAttribLocation(_program, index, location);
}

void Shader::linkProgram( void )
{
	glLinkProgram(_program);
	glUseProgram(_program);
}

void Shader::deleteProgram( void )
{
    if (_program) {
        glDeleteProgram(_program);
        _program = 0;
    }
}
