#ifndef CAMERA_H
# define CAMERA_H

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
	UP,
	DOWN,
	X_AXIS,
	Y_AXIS
};
	// _cam_angles = glm::vec2(225.0f, -37.0f);
# define YAW          90.0f
# define PITCH        0.0f
# define REACH		  4.5f
# define SPEED        20
# define WALK_SPEED   4.317
# define RUN_SPEED    5.612
# define RUN_JUMP_SPEED 7.127
# define EYE_LEVEL    0.62f
# define INITIAL_JUMP -9.317f
# define INITIAL_FALL 6.605f
# define STANDARD_GRAVITY 9.81f
# define PLAYER_MASS 10
# define FALL_SPEED   77.71f //77.71f
# define FOV          70.0f // fov must be 80, but this is vertical fov/2

// void cursor_position_callback( GLFWwindow* window, double xpos, double ypos );
// void scroll_callback( GLFWwindow* window, double xoffset, double yoffset );

class Camera
{
	private:
		glm::vec3 _position, _front, _up, _right, _world_up;
		glm::vec2 _front2, _right2;
		float _yaw, _pitch;
		// double _mouse_sensitivity;
		float _deltaTime, _fall_time, _breathTime;
		float _fov;
		float _fall_speed;
		bool _isRunning, _healthUpdate, _waterHead, _waterFeet;
		std::mutex _mtx;
		Chunk *_current_chunk_ptr;

    	void updateCameraVectors( void );
	
	public:
		glm::ivec3 _current_block;
		int _movement_speed, _health_points;
		bool _update, _fovUpdate, _inJump, _touchGround, _fall_immunity;
		float _fall_distance;

		Camera( glm::vec3 position );
		~Camera( void );

		glm::mat4 getViewMatrix( void );
		glm::mat4 getPerspectiveMatrix( void );
		bool chunkInFront( glm::ivec2 current_chunk, int posX, int posY );

		int getOrientation( void );
		glm::vec3 getPos( void );
		glm::vec3 getDir( void );
		void setPosZ( float value );
		void setWaterStatus( bool head, bool underwater );
		int getWaterStatus( void );

		void setCurrentChunkPtr( Chunk *ptr );
		void setRun( bool value );
		void setDelta( float deltaTime );
		void update_movement_speed( GLint key_cam_speed );
		void moveFly( GLint v, GLint h, GLint z );
		void moveHuman( Camera_Movement direction, GLint v, GLint h );
		void applyGravity( void );
		void touchGround( void );
		bool getModif( void );

		void processPitch( GLint offset );
		void processYaw( GLint offset );
		std::vector<glm::ivec3> get_ray_casting( GLfloat radius );
		void processMouseMovement( float x_offset, float y_offset );

		std::string getCamString( bool game_mode );
		std::string saveString( void );
		void loadWorld( std::ofstream & ofs, std::ifstream & indata );
		void respawn( void );
};

#endif
