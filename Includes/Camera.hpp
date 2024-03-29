#ifndef CAMERA_H
# define CAMERA_H

# include <mutex>
# include "Chunk.hpp"

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

# define YAW          		90.0f
# define PITCH        		0.0f

# define REACH		  		4.5f
# define EYE_LEVEL    		0.62f
# define SNEAK_EYE_LEVEL	0.32f
# define FOV          		70.0f // if fov = -fov, world is upside down

# define FLY_SPEED    		20
# define WALK_SPEED   		4.317f
# define SNEAK_SPEED		1.295f
# define SNEAK_SPRINT_SPEED	1.684f
# define SPRINT_SPEED    	5.612f
# define SPRINT_JUMP_SPEED 	7.127f
# define SWIM_SPEED			1.97f
# define SWIM_UP_SPEED		0.39f
# define SWIM_DOWN_SPEED	1.81f

# define INITIAL_JUMP 		9.317f
# define INITIAL_FALL 		-6.605f
# define STANDARD_GRAVITY 	-9.81f
// # define PLAYER_MASS 10
// # define FALL_SPEED   77.71f

# define EXHAUSTION_SWIMMING 		0.01f
# define EXHAUSTION_BREAKING_BLOCK 	0.005f
# define EXHAUSTION_SPRINTING 		0.1f
# define EXHAUSTION_JUMP 			0.05f
# define EXHAUSTION_SPRINT_JUMP 	0.2f
# define EXHAUSTION_REGEN 			6.0f

namespace CAMPLACEMENT {
	enum {
		DEFAULT,
		BEHIND,
		FRONT
	};
}

class Camera
{
	private:
		glm::vec3 _position, _spawnpoint, _lastTp, _front, _bodyFront, _up, _right, _world_up;
		glm::vec2 _front2, _right2;
		float _yaw, _pitch;
		float _deltaTime, _fall_time, _walk_time, _breathTime, _armAnimTime;
		float _fov, _fov_offset;
		float _fall_distance;
		int _foodTickTimer, _camPlacement;
		float _foodExhaustionLevel;
		float _z0, _fall_immunity;
		bool _walking, _sprinting, _sneaking, _healthUpdate, _waterHead, _waterFeet, _armAnimation, _hideUI;
		std::mutex _mtx;
		Chunk *_current_chunk_ptr;

    	void updateCameraVectors( void );
		void moveHumanUnderwater( Camera_Movement direction, GLint v, GLint h, GLint z );
		void applyGravityUnderwater( void );
	
	public:
		glm::ivec3 _current_block;
		int _movement_speed, _health_points, _foodLevel;
		bool _update, _fovUpdate, _inJump, _touchGround;
		float _foodSaturationLevel;

		Camera( glm::vec3 position );
		~Camera( void );

		void drawHeldItem( std::vector<std::pair<int, glm::vec3>> &arr, int item, bool game_mode );
		void drawPlayer( std::vector<std::pair<int, glm::vec3>> &arr, int item, bool game_mode );

		glm::mat4 getViewMatrix( void );
		glm::mat4 getPerspectiveMatrix( void );
		bool chunkInFront( glm::ivec2 current_chunk, int posX, int posY );

		int getOrientation( void );
		glm::vec3 getPos( void );
		glm::vec3 getEyePos( void );
		glm::vec3 getDir( void );
		float getHitBox( void );
		void setPosZ( float value );
		void setHideUI( bool value );
		void setWaterStatus( bool head, bool underwater );
		int getWaterStatus( void );
		bool isUnderwater( void );

		void setCurrentChunkPtr( Chunk *ptr );
		void changeCamPlacement( void );
		void setRun( bool value );
		void setSneak( bool value );
		void setArmAnimation( bool state );
		void setDelta( float deltaTime );
		void update_movement_speed( GLint key_cam_speed );
		void moveFly( GLint v, GLint h, GLint z );
		void moveHuman( Camera_Movement direction, GLint v, GLint h, GLint z );
		void resetFall( void );
		void applyGravity( void );
		void touchGround( float value );
		void touchCeiling( float value );

		void tickUpdate( void );
		void updateExhaustion( float level );
		bool canEatFood( int type );
		bool getModif( void );

		void processPitch( GLint offset );
		void processYaw( GLint offset );
		std::vector<glm::ivec3> get_ray_casting( GLfloat radius );
		void processMouseMovement( float x_offset, float y_offset );

		std::string getCamString( bool game_mode );
		std::string saveString( void );
		void loadWorld( std::ofstream & ofs, std::ifstream & indata );
		void setPos( glm::vec3 pos );
		void setSpawnpoint( glm::vec3 spawnpoint );
		glm::vec3 getSpawnpoint( void );
		glm::vec3 getLastTp( void );
		void respawn( void );
};

#endif
