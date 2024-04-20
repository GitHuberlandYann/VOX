#include "Skybox.hpp"
#include "DayCycle.hpp"

namespace SHADERS {
	const int POSATTRIB = 0;
};

Skybox::Skybox( void )
{

}

Skybox::~Skybox( void )
{
	std::cout << "Destructor of Skybox called" << std::endl;

	glDeleteProgram(_shaderProgram);

	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Skybox::create_shader( void )
{
	_shaderProgram = createShaderProgram("skybox_vertex", "", "skybox_fragment");

	glBindFragDataLocation(_shaderProgram, 0, "outColor");

	glBindAttribLocation(_shaderProgram, SHADERS::POSATTRIB, "position");

	glLinkProgram(_shaderProgram);
	glUseProgram(_shaderProgram);

	check_glstate("Skybox shader program successfully created", true);
}

void Skybox::setup_communication_shaders( void )
{
	_uniView = glGetUniformLocation(_shaderProgram, "view");
	_uniProj = glGetUniformLocation(_shaderProgram, "proj");
	_uniCamPos = glGetUniformLocation(_shaderProgram, "camPos");
	_uniTicks = glGetUniformLocation(_shaderProgram, "ticks");

	const glm::vec3 p0 = {-1.0f, -1.0f,  1.0f};
	const glm::vec3 p1 = { 1.0f, -1.0f,  1.0f};
	const glm::vec3 p2 = {-1.0f, -1.0f, -1.0f};
	const glm::vec3 p3 = { 1.0f, -1.0f, -1.0f};

	const glm::vec3 p4 = {-1.0f,  1.0f,  1.0f};
	const glm::vec3 p5 = { 1.0f,  1.0f,  1.0f};
	const glm::vec3 p6 = {-1.0f,  1.0f, -1.0f};
	const glm::vec3 p7 = { 1.0f,  1.0f, -1.0f};

	const std::array<glm::vec3, 36> vertices = {
		p4, p0, p6,   p0, p2, p6, // minux
		p1, p5, p3,   p5, p7, p3, // plux
		p0, p1, p2,   p1, p3, p2, // minusy
		p5, p4, p7,   p4, p6, p7, // plusy
		p2, p3, p6,   p3, p7, p6, // minusz
		p4, p5, p0,   p5, p1, p0, // plusz
	};

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);

	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(SHADERS::POSATTRIB);
	glVertexAttribPointer(SHADERS::POSATTRIB, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)(0 * sizeof(GLfloat)));

	check_glstate("Skybox::setup_communication_shaders", false);
}

void Skybox::update_cam_view( glm::mat4 view )
{
	glUseProgram(_shaderProgram);
	glUniformMatrix4fv(_uniView, 1, GL_FALSE, glm::value_ptr(view));
}

void Skybox::update_cam_perspective( glm::mat4 proj )
{
	glUseProgram(_shaderProgram);
	glUniformMatrix4fv(_uniProj, 1, GL_FALSE, glm::value_ptr(proj));
}

void Skybox::render( glm::vec3 camPos )
{
    GLint OldDepthFuncMode;
    glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);
    glDepthFunc(GL_LEQUAL);

	glUseProgram(_shaderProgram);

	glUniform3fv(_uniCamPos, 1, &camPos.x);
	glUniform1i(_uniTicks, DayCycle::Get()->getTicks());

	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthFunc(OldDepthFuncMode);
}
