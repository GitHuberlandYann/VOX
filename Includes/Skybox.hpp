#ifndef SKYBOX_HPP
# define SKYBOX_HPP

# include "utils.h"

class Skybox {
	private:
		GLuint _vao, _vbo, _shaderProgram;
		GLint _uniView, _uniProj, _uniCamPos, _uniTicks;

	public:
		Skybox( void );
		~Skybox( void );

		void create_shader( void );
		void setup_communication_shaders( void );
		void update_cam_view( glm::mat4 view );
		void update_cam_perspective( glm::mat4 proj );
		void render( glm::vec3 camPos );
};

#endif
