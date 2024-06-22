#include "Skybox.hpp"
#include "DayCycle.hpp"
#include "Settings.hpp"
#include "logs.hpp"

Skybox::Skybox( void )
{

}

Skybox::~Skybox( void )
{
	MAINLOG(LOG("Destructor of Skybox called"));
}

void Skybox::deleteBuffers( void )
{
	_shader.deleteProgram();

	_vabo.deleteBuffers();
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Skybox::createShader( void )
{
	_shader.createProgram(Settings::Get()->getString(settings::strings::skybox_vertex_shader), "",
						Settings::Get()->getString(settings::strings::skybox_fragment_shader));

	_shader.bindFragData(settings::consts::shader::outColor, "outColor");
	_shader.bindAttribute(settings::consts::shader::attributes::position, "position");
	_shader.linkProgram();

	check_glstate("Skybox shader program successfully created", true);

	_vabo.genBuffers();
	_vabo.addAttribute(settings::consts::shader::attributes::position, 3, GL_FLOAT);
}

void Skybox::setupCommunicationShaders( void )
{
	_shader.setUniformLocation(settings::consts::shader::uniform::view, "view");
	_shader.setUniformLocation(settings::consts::shader::uniform::proj, "proj");
	_shader.setUniformLocation(settings::consts::shader::uniform::camPos, "camPos");
	_shader.setUniformLocation(settings::consts::shader::uniform::ticks, "ticks");

	const glm::vec3 p0 = {-1.0f, -1.0f,  1.0f};
	const glm::vec3 p1 = { 1.0f, -1.0f,  1.0f};
	const glm::vec3 p2 = {-1.0f, -1.0f, -1.0f};
	const glm::vec3 p3 = { 1.0f, -1.0f, -1.0f};

	const glm::vec3 p4 = {-1.0f,  1.0f,  1.0f};
	const glm::vec3 p5 = { 1.0f,  1.0f,  1.0f};
	const glm::vec3 p6 = {-1.0f,  1.0f, -1.0f};
	const glm::vec3 p7 = { 1.0f,  1.0f, -1.0f};

	const std::array<glm::vec3, 36> vertices = {
		p0, p4, p2,   p4, p6, p2, // minux
		p5, p1, p7,   p1, p3, p7, // plux
		p1, p0, p3,   p0, p2, p3, // minusy
		p4, p5, p6,   p5, p7, p6, // plusy
		p3, p2, p7,   p2, p6, p7, // minusz
		p5, p4, p1,   p4, p0, p1, // plusz
	};

	_shader.useProgram();
	_vabo.uploadData(vertices.size(), &vertices[0]);

	check_glstate("Skybox::setup_communication_shaders", false);
}

void Skybox::updateCamView( glm::mat4 view )
{
	glUniformMatrix4fv(_shader.getUniform(settings::consts::shader::uniform::view), 1, GL_FALSE, glm::value_ptr(view));
}

void Skybox::updateCamPerspective( glm::mat4 proj )
{
	glUniformMatrix4fv(_shader.getUniform(settings::consts::shader::uniform::proj), 1, GL_FALSE, glm::value_ptr(proj));
}

void Skybox::render( glm::vec3 camPos )
{
    GLint OldDepthFuncMode;
    glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);
    glDepthFunc(GL_LEQUAL);

	glUniform3fv(_shader.getUniform(settings::consts::shader::uniform::camPos), 1, &camPos.x);
	glUniform1i(_shader.getUniform(settings::consts::shader::uniform::ticks), DayCycle::Get()->getTicks());

	_vabo.bindVertexArray();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthFunc(OldDepthFuncMode);
}
