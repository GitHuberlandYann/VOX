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

extern std::mutex mtx;
extern std::mutex mtx_perimeter;
extern std::mutex mtx_deleted_chunks;
extern std::mutex mtx_backup;

enum {
	FILL,
	LINE,
	F_LAST
};

typedef struct s_hit {
	glm::ivec3 pos, prev_pos, water_pos;
	int value, type;
	bool water_value;
}				t_hit;

class OpenGL_Manager
{
	public:
		std::list<Chunk*> _chunks;
		std::vector<Chunk*> _perimeter_chunks, _deleted_chunks;
		std::map<std::pair<int, int>, s_backup> _backups;
		std::unique_ptr<Player> _player;
		std::unique_ptr<Inventory> _inventory;

		OpenGL_Manager( void );
		~OpenGL_Manager( void );

		Chunk *getCurrentChunkPtr( void );

		void setupWindow( void );
		void initWorld( void );
		void createShaders( void );
		void setupCommunicationShaders( void );
		void loadTextures( void );
		void resetInputsPtrs( void );
		void setGamemode( int gamemode );
		void getGamemode( void );
		void setItemFrame( bool visible, bool lock );
		size_t clearEntities( void );
		size_t clearParticles( void );
		void main_loop( void );

		glm::ivec2 getCurrentChunk( void );
		void setThreadUpdate( bool state );
		bool getThreadUpdate( void );
		bool getThreadStop( void );

	private:
		GLFWwindow* _window;
		Shader _shader, _skyShader, _particleShader, _modelShader;
		Buffer _vaboEntities, _vaboParticles, _vaboModels;
		glm::ivec2 _current_chunk;
		std::array<GLuint, 5> _textures;
		GLint _fill;
		bool _debug_mode, _outline, _paused, _threadUpdate, _threadStop;
		float _break_time, _eat_timer, _bow_timer;
		int _game_mode, _break_frame, _hand_content;
		std::string _world_name;
		std::vector<Chunk*> _visible_chunks;
		std::vector<t_shaderInput> _entities;
		std::vector<t_shaderInput> _particles;
		std::vector<t_shaderInput> _models;
		std::thread _thread;
		std::mutex _mtx;
		t_hit _block_hit;
		Chunk *_current_chunk_ptr = NULL, *_chunk_hit = NULL;
		std::unique_ptr<Camera> _camera;
		std::unique_ptr<UI> _ui;
		std::unique_ptr<Menu> _menu;
		std::unique_ptr<Skybox> _skybox;


		void updateVisibleChunks( void );
		void chunkUpdate( void );
		float getBreakTime( bool canHarvest );
		void userInputs( float deltaTime, bool rayCast );
		t_hit getBlockHit( void );
		void handleBlockModif( bool adding, bool collect );
		void updateCamView( void );
		void updateCamPerspective( void );
		void updateAnimFrame( void );
		void screenshot( void );
		void saveWorld( void );
		std::string saveBackupString( void );
		void loadWorld( std::string file );
		void loadBackups( std::ofstream & ofs, std::ifstream & indata );

		void addBreakingAnim( void );
		void addLine( glm::vec3 a, glm::vec3 b );
		void drawEntities( void );
		void drawParticles( void );
		void drawModels( void );

		void startThread( void );
		void stopThread( void );
};

#endif
