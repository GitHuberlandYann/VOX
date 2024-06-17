#ifndef CAMERA_H
# define CAMERA_H

# include <mutex>
# include "Chunk.hpp"

enum Camera_Movement {
	X_AXIS,
	Y_AXIS,
	Z_AXIS
};

namespace CAMPLACEMENT {
	enum {
		DEFAULT,
		BEHIND,
		FRONT
	};
}

class Camera
{
	public:
		Camera( void );
		~Camera( void );


		glm::mat4 getViewMatrix( void );
		glm::mat4 getPerspectiveMatrix( void );

		void setTarget( AMob* mob );

		glm::vec3 getCamPos( void );
		void changeCamPlacement( void );

	private:
		int _camPlacement;
		AMob* _target;
};

#endif
