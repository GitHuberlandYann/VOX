#ifndef SKYBOX_HPP
# define SKYBOX_HPP

# include "utils.h"

class Skybox {
	public:
		Skybox( void );
		~Skybox( void );

		void createShader( void );
		void setupCommunicationShaders( void );
		void updateCamView( glm::mat4 view );
		void updateCamPerspective( glm::mat4 proj );
		void render( glm::vec3 camPos );

	private:
		GLuint _vao, _vbo, _shaderProgram;
		GLint _uniView, _uniProj, _uniCamPos, _uniTicks;
};

#endif
