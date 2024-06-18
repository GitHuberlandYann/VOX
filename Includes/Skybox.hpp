#ifndef SKYBOX_HPP
# define SKYBOX_HPP

# include "utils.h"
# include "Shader.hpp"
# include "Buffer.hpp"

class Skybox {
	public:
		Skybox( void );
		~Skybox( void );
		void deleteBuffers( void );

		void createShader( void );
		void setupCommunicationShaders( void );
		void updateCamView( glm::mat4 view );
		void updateCamPerspective( glm::mat4 proj );
		void render( glm::vec3 camPos );

	private:
		Shader _shader;
		Buffer _vabo;
};

#endif
