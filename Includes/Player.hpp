#ifndef PLAYER_HPP
# define PLAYER_HPP

# include "Chunk.hpp"

namespace settings {
	namespace consts {
		namespace speed {
			const float fly = 20.0f;
			const float walk = 4.317f;
			const float sneak = 1.295f;
			const float sneak_sprint = 1.684f;
			const float sprint = 5.612f;
			const float sprint_jump = 7.127f;
			const float swim = 1.97f;
			const float swim_up = 0.39f;
			const float swim_down = 1.81f;

			const float initial_jump = 9.317f;
			const float initial_fall = -6.605f;
			const float zombie = 2.28378822f;
			const float skeleton = 3.1642862f;
			const float smooth_cam = 5.0f;
		};

		namespace exhaustion {
			const float swim = 0.01f;
			const float breaking_block = 0.005f;
			const float sprint = 0.1f;
			const float jump = 0.05f;
			const float sprint_jump = 0.2f;
			const float regen = 6.0f;
		};
		
		namespace reach {
			const float survival = 4.5f;
			const float creative = chunk_size;
			const float brush = 64;
			const float attack = 3.0f;
		};
	};
};

class Player : public AMob
{
    public:
        Player( void );
        ~Player( void );

        bool chunkInFront( int camPlacement, glm::ivec2 current_chunk, int posX, int posY );
        std::vector<glm::ivec3> computeRayCasting( GLfloat radius );
		void drawHeldItem( std::vector<t_shaderInputModel>& arr, std::vector<t_shaderInput>& partArr, int item, int gameMode );
		void drawPlayer( std::vector<t_shaderInputModel>& arr, std::vector<t_shaderInput>& partArr, int item );

       	glm::vec3 getEyePos( void ) override;
       	float getHitbox( void ) override;
        int getOrientation( void );
		int getOrientation6( void );

        void setWaterStatus( bool head, bool underwater );
        int getWaterStatus( void );
        bool isUnderwater( void );
        bool canEatFood( int type );

        void setRun( bool state );
        void setSneak( bool state );
        void setJump( bool state );
		void setDelta( float deltaTime );
        void setArmAnimation( bool state );
       	void setCamUpdate( bool state );
       	void setUIUpdate( bool state );

       	float getFovOffset( void ) override;
        int getFoodLevel( void );
        float getFoodSaturationLevel( void );
        bool getCamUpdate( void );
        bool getResetFovUpdate( void );
        bool getResetUIUpdate( void );

        void updateExhaustion( float level );
        void tickUpdate( void );
        void processMouseMovement( float x_offset, float y_offset );

        void setChunkPtr( Chunk* chunk );
        std::string getString( int game_mode );
        std::string saveString( void );
		void loadWorld( std::ofstream & ofs, std::ifstream & indata );

        void setPos( glm::vec3 pos );
		void setSpawnpoint( glm::vec3 spawnpoint );
		glm::vec3 getSpawnpoint( void );
		glm::vec3 getLastTp( void );
		void respawn( void );

       	void applyGravity( void ) override;
        void applyGravityUnderwater( void );
		void touchGround( float value ) override;
		void touchCeiling( float value ) override;
        void resetFall( void );

		bool update( std::vector<t_shaderInputModel>& modArr, float deltaTime ) override;
        void inputUpdate( bool rayCast, int gameMode );
    
    private:
        int _flySpeed;
        int _foodLevel, _foodTickTimer;
        float _yaw, _pitch;
        float _smoothCamZ, _fovOffset, _armAnimTime;
        float _breathTime;
        float _foodSaturationLevel, _foodExhaustionLevel;
        glm::vec3 _spawnpoint, _lastTp;
        glm::vec2 _front2, _right2;
        bool _smoothCam, _armAnimation, _fallImmunity;
        bool _sprinting, _sneaking;
        bool _waterHead, _waterFeet;
        bool _updateCam, _updateFov, _updateUI;

        void updateVectors( void );
        void processPitch( GLint offset );
		void processYaw( GLint offset );

       	bool updateCurrentBlock( void ) override;
        void moveFly( GLint v, GLint h, GLint z );
        void moveUnderwater( int direction, GLint v, GLint h, GLint z );
		float getSpeed( void ) override;
        void move( int direction, GLint v, GLint h, GLint z );
       	void restorePos( glm::vec3 position ) override;
       	bool customObstacle( int dir, float maxZ ) override;
        void updateFlySpeed( GLint key_cam_speed );
};

#endif
