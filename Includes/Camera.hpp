#ifndef CAMERA_H
# define CAMERA_H

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
	UP,
	DOWN
};
	// _cam_angles = glm::vec2(225.0f, -37.0f);
# define YAW          90.0f
# define PITCH        0.0f
# define SPEED        20
# define WALK_SPEED   4.317
// # define RUN_SPEED    5.612
# define EYE_LEVEL    1.62f
# define INITIAL_JUMP -3.317f
# define INITIAL_FALL 6.605f
# define STANDARD_GRAVITY 9.81f
# define FALL_SPEED   77.71f //77.71f
# define FOV          40.0f // fov must be 80

// void cursor_position_callback( GLFWwindow* window, double xpos, double ypos );
// void scroll_callback( GLFWwindow* window, double xoffset, double yoffset );

class Camera
{
	private:
		glm::vec3 _front;
		glm::vec3 _up;
		glm::vec3 _right;
		glm::vec3 _world_up;
		float _yaw;
		float _pitch;
		// double _mouse_sensitivity;
		float _deltaTime, _fall_time;
		float _fov;
		float _fall_speed;
    	void updateCameraVectors( void );
	
	public:
		glm::vec3 _position;
		int _movement_speed;
		bool _update, _inJump, _touchGround;

		Camera( glm::vec3 position );
		~Camera( void );

		glm::mat4 getViewMatrix( void );
		glm::mat4 getPerspectiveMatrix( void );
		void setDelta( float deltaTime );
		void update_movement_speed( GLint key_cam_speed );
		void processKeyboard( Camera_Movement direction, bool game_mode );
		void fall( bool real_fall );
		void touchGround( void );
		void processPitch( GLint offset );
		void processYaw( GLint offset );
		std::vector<glm::ivec3> get_ray_casting( GLfloat radius );
		void processMouseMovement( float x_offset, float y_offset );
		// void processMouseScroll( float offset );
		std::string getCamString( void );
};

#endif
