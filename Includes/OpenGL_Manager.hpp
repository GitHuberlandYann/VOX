#ifndef OPENGL_MANAGER_HPP
# define OPENGL_MANAGER_HPP

# include "Chunk.hpp"
# include "DayCycle.hpp"
# include "Camera.hpp"
# include "Text.hpp"
# include "Ui.hpp"
# include "Menu.hpp"
# include "Skybox.hpp"

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
	private:
		GLFWwindow* _window;
		GLuint _vaoEntities, _vboEntities, _vaoParticles, _vboParticles, _shaderProgram, _skyShaderProgram, _particleShaderProgram;
		GLint _uniFog, _uniView, _uniProj, _uniBrightness, _skyUniFog, _skyUniView, _skyUniProj, _skyUniColor, _skyUniAnim, _partUniView, _partUniProj, _partUniBrightness;
		glm::ivec2 _current_chunk;
		std::array<GLuint, 4> _textures;
		GLint _fill;
		bool _debug_mode, _outline, _paused, _threadUpdate, _threadStop;
		float _break_time, _eat_timer, _bow_timer;
		int _game_mode, _break_frame, _hand_content;
		std::string _world_name;
		std::vector<Chunk *> _visible_chunks;
		std::vector<t_shaderInput> _entities;
		std::vector<t_shaderInput> _particles;
		std::thread _thread;
		std::mutex _mtx;
		t_hit _block_hit;
		Chunk *_current_chunk_ptr = NULL, *_chunk_hit = NULL;
		UI *_ui;
		Menu *_menu;
		Skybox *_skybox;


		void update_visible_chunks( void );
		void chunk_update( void );
		float getBreakTime( bool canHarvest );
		void user_inputs( float deltaTime, bool rayCast );
		t_hit get_block_hit( void );
		void handle_add_rm_block( bool adding, bool collect );
		void update_cam_view( void );
		void update_cam_perspective( void );
		void update_anim_frame( void );
		void screenshot( void );
		void saveWorld( void );
		std::string saveBackupString( void );
		void loadWorld( std::string file );
		void loadBackups( std::ofstream & ofs, std::ifstream & indata );

		void addBreakingAnim( void );
		void addLine( glm::vec3 a, glm::vec3 b );
		void drawEntities( void );
		void drawParticles( void );

		void startThread( void );
		void stopThread( void );

	public:
		std::list<Chunk *> _chunks;
		std::vector<Chunk *> _perimeter_chunks, _deleted_chunks;
		std::map<std::pair<int, int>, s_backup> _backups;
		Camera *_camera;
		Inventory *_inventory;

		OpenGL_Manager( void );
		~OpenGL_Manager( void );

		Chunk *getCurrentChunkPtr( void );

		void setup_window( void );
		void initWorld( void );
		void create_shaders( void );
		void setup_communication_shaders( void );
		void load_texture( void );
		void resetInputsPtrs( void );
		void setGamemode( int gamemode );
		void getGamemode( void );
		size_t clearEntities( void );
		size_t clearParticles( void );
		void main_loop( void );

		glm::ivec2 getCurrentChunk( void );
		void setThreadUpdate( bool state );
		bool getThreadUpdate( void );
		bool getThreadStop( void );
};

#endif
