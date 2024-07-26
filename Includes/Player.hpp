#ifndef PLAYER_HPP
# define PLAYER_HPP

# include "Chunk.hpp"
class OpenGL_Manager;

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

		namespace permission {
			const int none = 0;
			const int admin = 5;
		};
	};
};

typedef struct s_hit {
	glm::ivec3 pos, prev_pos, water_pos;
	int value, type;
	bool water_value;
}				t_hit;

class Player : public AMob
{
    public:
        Player( void );
        ~Player( void );

        bool chunkInFront( int camPlacement, glm::ivec2 current_chunk, int posX, int posY );
        std::vector<glm::ivec3> computeRayCasting( GLfloat radius );
		void drawHeldItem( std::vector<t_shaderInputModel>& arr, std::vector<t_shaderInput>& partArr );
		void drawPlayer( std::vector<t_shaderInputModel>& arr, std::vector<t_shaderInput>& partArr );

		/** @category setters */
		void setDelta( float deltaTime );
       	void setCamUpdate( bool state );
       	void setUIUpdate( bool state );
		void setGameMode( int gamemode, bool ingame = true );
		void setPtrs( OpenGL_Manager* oglMan, Menu* menu, UI* ui );
		void setPermissionLevel( int permission );

		/** @category getters */
       	glm::vec3 getEyePos( void ) override;
		glm::ivec2 getChunkPos( void );
		Chunk* getChunkPtr( void );
       	float getHitbox( void ) override;
        int getOrientation( void );
		int getOrientation6( void );
        int getWaterStatus( void );
       	float getFovOffset( void ) override;
        int getFoodLevel( void );
        float getFoodSaturationLevel( void );
        bool getCamUpdate( void );
        bool getResetFovUpdate( void );
        bool getResetUIUpdate( void );
		Inventory* getInventory( void );
		Chunk* getChunkHit( void );
		t_hit& getBlockHit( void );
		int getGameMode( void );
		int getPermissionLevel( void );
		void outputGameMode( void );

        void tickUpdate( void );

		void addBreakingAnim( std::vector<t_shaderInput>& entities );
        std::string getString( void );
        std::string saveString( void );
		void loadWorld( std::ofstream& ofs, std::ifstream& indata );

        void setPos( glm::vec3 pos );
		void setSpawnpoint( glm::vec3 spawnpoint );
		glm::vec3 getSpawnpoint( void );
		glm::vec3 getLastTp( void );
		void respawn( void );

		/** @category multiplayer */
		int getId( void );
		void setId( int id );
		std::string getName( void );
		void setName( std::string name );
		bool isLoaded( glm::ivec2 chunkPos );
		bool unloadChunk( glm::ivec2 chunkPos );
		void moveClient( void );
		size_t packetPos( t_packet_data& packet );
		bool handlePacketPos( t_packet_data& packet, size_t& cursor, bool client, Chunk* chunk = NULL );
		void appendPacketInfo( t_pending_packet& packet );

		/** @category physics */
       	void applyGravity( void ) override;
        void applyGravityUnderwater( void );
		void touchGround( float value ) override;
		void touchCeiling( float value ) override;
        void resetFall( void );

		/** @category inputs */
		bool update( std::vector<t_shaderInputModel>& modArr, float deltaTime ) override;
		void resetInputsPtrs( void );
		void inputToggle( void );
		void blockInputUpdate( void );
        void inputUpdate( bool rayCast );
		void setItemFrame( bool visible, bool lock );
        void clientInputUpdate( void );
        void processMouseMovement( float x_offset, float y_offset );
    
    private:
		int _id, _permission; // multiplayer id
        int _flySpeed;
        int _foodLevel, _foodTickTimer;
		int _gameMode, _breakFrame, _handContent;
        float _yaw, _pitch;
        float _smoothCamZ, _fovOffset, _armAnimTime;
		float _breakTime, _eatTime, _bowTime, _breathTime;
        float _foodSaturationLevel, _foodExhaustionLevel;
        glm::vec3 _spawnpoint, _lastTp;
        glm::vec2 _front2, _right2;
		t_hit _blockHit;
		glm::ivec2 _currentChunk;
		std::string name;
		std::set<std::pair<int, int>> _loadedChunks;
		std::shared_ptr<Chunk> _chunkHit;
		std::unique_ptr<Inventory> _inventory;
		OpenGL_Manager* _oglMan;
		Menu* _menu;
		UI* _ui;
		std::mutex _mtx;
        bool _smoothCam, _armAnimation, _fallImmunity;
        bool _sprinting, _sneaking;
        bool _waterHead, _waterFeet;
        bool _updateCam, _updateFov, _updateUI;

        void updateVectors( void );
        void processPitch( GLint offset );
		void processYaw( GLint offset );

        void setRun( bool state );
        void setSneak( bool state );
        void setJump( bool state );
        void setArmAnimation( bool state );
        void setWaterStatus( bool head, bool underwater );
        void updateExhaustion( float level );
        bool canEatFood( int type );
		glm::vec3 getSmoothPos( void );

       	bool updateCurrentBlock( void ) override;
		bool updateCurrentChunk( void );
        void moveFly( GLint v, GLint h, GLint z );
        void moveUnderwater( int direction, GLint v, GLint h, GLint z );
		float getSpeed( void ) override;
        void move( int direction, GLint v, GLint h, GLint z );
       	void restorePos( glm::vec3 position ) override;
       	bool customObstacle( int dir, float maxZ ) override;
        void updateFlySpeed( GLint key_cam_speed );

		float getBreakTime( bool canHarvest );
		t_hit computeBlockHit( void );
		void handleBlockModif( bool adding, bool collect );
};

#endif
