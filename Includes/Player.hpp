#ifndef PLAYER_HPP
# define PLAYER_HPP

# include "Chunk.hpp"

class Player : public AMob
{
    public:
        Player( void );
        virtual ~Player( void );

        bool chunkInFront( glm::ivec2 current_chunk, int posX, int posY );
        std::vector<glm::ivec3> computeRayCasting( GLfloat radius );
		void drawPlayer( std::vector<t_shaderInput> &arr, int item, int gameMode );

        virtual glm::vec3 getEyePos( void );
        virtual glm::vec3 getCamPos( bool update );
        virtual bool getHitBox( void );
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
        virtual void setCamUpdate( bool state );

        virtual float getFovOffset( void );
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

        virtual void applyGravity( void );
        void applyGravityUnderwater( void );
		virtual void touchGround( float value );
		virtual void touchCeiling( float value );
        void resetFall( void );
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

        virtual bool updateCurrentBlock( void );
        void moveFly( GLint v, GLint h, GLint z );
        void moveUnderwater( int direction, GLint v, GLint h, GLint z );
        void move( int direction, GLint v, GLint h, GLint z );
        virtual void restorePos( glm::vec3 position );
        virtual bool customObstacle( float minZ, float maxZ );
        void updateFlySpeed( GLint key_cam_speed );

		void drawHeldItem( std::vector<t_shaderInput> &arr, int item, int gameMode );
};

#endif
