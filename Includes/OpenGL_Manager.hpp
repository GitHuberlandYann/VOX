#ifndef OPENGL_MANAGER_HPP
# define OPENGL_MANAGER_HPP

# include "Chunk.hpp"
# include "DayCycle.hpp"
# include "Player.hpp"
# include "Camera.hpp"
# include "Text.hpp"
# include "Ui.hpp"
# include "Menu.hpp"
# include "Skybox.hpp"
# include "Shader.hpp"
# include "Buffer.hpp"
# include "Socket.hpp"

extern std::mutex mtx;
extern std::mutex mtx_perimeter;
extern std::mutex mtx_deleted_chunks;
extern std::mutex mtx_backup;

typedef struct s_time {
	s_time( void )
		: deltaTime(0.), currentTime(0.), lastSecondRecorded(0.), lastGameTick(0.), previousFrame(0.),
		nbFrames(0), nbFramesLastSecond(0), nbTicks(0), nbTicksLastSecond(0),
		fluidUpdate(false), animUpdate(false), tickUpdate(false), redTickUpdate(false) {}
	double deltaTime;
	double currentTime;
	double lastSecondRecorded;
	double lastGameTick;
	double previousFrame;
	int nbFrames;
	int nbFramesLastSecond;
	int nbTicks;
	int nbTicksLastSecond;
	bool fluidUpdate;
	bool animUpdate;
	bool tickUpdate;
	bool redTickUpdate;
}				t_time;

typedef struct s_counter {
	s_counter( void )
		: newVaos(0), meshFaces(0), waterFaces(0), skyFaces(0) {}
	GLint newVaos;
	GLint meshFaces;
	GLint waterFaces;
	GLint skyFaces;
}				t_counter;

class Server;

class OpenGL_Manager
{
	public:
		std::list<std::shared_ptr<Chunk>> _chunks;
		std::vector<std::shared_ptr<Chunk>> _perimeter_chunks, _deleted_chunks;
		std::map<std::pair<int, int>, s_backup> _backups;
		std::unique_ptr<Player> _player;

		OpenGL_Manager( void );
		~OpenGL_Manager( void );

		void setCurrentChunkPtr( std::shared_ptr<Chunk>& chunk );
		Chunk* getCurrentChunkPtr( void );
		std::shared_ptr<Chunk> getChunkAt( int posX, int posY );

		void resetInputsPtrs( void );
		void pauseGame( void );
		void setItemFrame( bool visible, bool lock );
		size_t clearEntities( void );
		size_t clearParticles( void );

		void createServer( std::unique_ptr<Server>& server );
		bool run( void );

		glm::ivec2 getCurrentChunkPos( void );
		std::vector<t_packet_data> getPacketDatas( void );
		void setThreadUpdate( bool state );
		bool getThreadUpdate( void );
		bool getThreadStop( void );

	private:
		GLFWwindow* _window;
		Shader _shader, _skyShader, _particleShader, _modelShader;
		Buffer _vaboEntities, _vaboParticles, _vaboModels;
		glm::ivec2 _current_chunk;
		std::array<GLuint, 6> _textures;
		bool _fill, _debugMode, _paused, _threadUpdate, _threadStop;
		std::string _world_name;
		std::vector<std::shared_ptr<Chunk>> _visible_chunks;
		std::vector<t_shaderInput> _entities;
		std::vector<t_shaderInputPart> _particles;
		std::vector<t_shaderInputModel> _models;
		std::vector<t_packet_data> _chunkPackets;
		std::thread _thread;
		std::mutex _mtx, _mtx_packets;
		t_time _time, _serverTime;
		t_packet_data _packet;
		t_counter _counter;
		std::map<int, std::unique_ptr<Player>> _players;
		std::shared_ptr<Chunk> _current_chunk_ptr = NULL;
		std::unique_ptr<Camera> _camera;
		std::unique_ptr<UI> _ui;
		std::unique_ptr<Menu> _menu;
		std::unique_ptr<Skybox> _skybox;
		std::shared_ptr<Socket> _socket;


		void updateVisibleChunks( void );
		void chunkUpdate( void );
		void userInputs( bool rayCast );
		void updateCamView( void );
		void updateCamPerspective( void );
		void updateAnimFrame( void );
		void screenshot( void );
		void saveWorld( void );
		std::string saveBackupString( void );
		void loadWorld( std::string file );
		void loadBackups( std::ofstream& ofs, std::ifstream& indata );
		void clearChunks( void );

		void addLine( glm::vec3 a, glm::vec3 b );
		void drawEntities( void );
		void drawParticles( void );
		void drawModels( void );
		void initWorld( void );

		/** @category setup */
		void setupWindow( void );
		void createShaders( void );
		void setupCommunicationShaders( void );
		void loadTextures( void );

		/** @category main loop */
		void handleEndSetup( void );
		void initTime( void );
		void handleTime( bool gamePaused );
		void handleUserInputs( int& backFromMenu );
		void handleDraw( bool gamePaused );
		void handleUI( void );
		void handleBackToGame( void );
		void hostServer( void );
		void joinServer( void );
		void handleMenu( bool animUpdate );
		void handleChunkDeletion( void );

		/** @category client packets send */
		void sendPacket( size_t size );
		void sendPlayerPos( void );
		void sendMessageServer( void );

		/** @category client packets receive */
		void handlePacketLogin( char* data );
		void handlePacketPing( char* data );
		void handlePacketPingList( void );
		void handlePacketKick( std::string msg );
		void handlePacketChunk( t_packet_data& packet );
		void handlePacketChunkUnload( t_packet_data& packet );
		void handlePacketPlayerJoin( void );
		void handlePacketPlayerLeave( void );
		void handlePacketPlayersInfo( t_packet_data& packet, size_t size );
		void handlePacketChatMsg( char* data );

		/** @category client loop */
		void handlePackets( void );
		void handleClientInputs( void );
		void handleClientDraw( void );
		void handleClientUI( void );
		void runClient( void );

		/** @category threading */
		void startThread( void );
		void startClientThread( void );
		void stopThread( void );
};

#endif
