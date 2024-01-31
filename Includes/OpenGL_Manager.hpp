#ifndef OPENGL_MANAGER_HPP
# define OPENGL_MANAGER_HPP

# include "Chunk.hpp"
# include "DayCycle.hpp"
# include "Camera.hpp"
# include "Text.hpp"
# include "Ui.hpp"
# include "Menu.hpp"
# include "callbacks.hpp"

extern std::mutex mtx;
extern std::mutex mtx_perimeter;
extern std::mutex mtx_deleted_chunks;
extern std::mutex mtx_inventory;
extern std::mutex mtx_backup;

# define RENDER_DISTANCE 10//20

enum {
	FILL,
	LINE,
	F_LAST
};

typedef struct s_hit {
	glm::ivec3 pos, prev_pos, water_pos;
	int value;
	bool prev_value, water_value;
}				t_hit;

class OpenGL_Manager
{
	private:
		GLFWwindow* _window;
		GLuint _vaoEntities, _vboEntities, _shaderProgram, _skyShaderProgram;
		GLint _uniFog, _uniView, _uniProj, _skyUniFog, _skyUniView, _skyUniProj, _skyUniColor, _skyUniAnim;
		glm::ivec2 _current_chunk;
		GLuint *_textures;
		GLint _key_rdist, _render_distance, _key_fill, _fill, _key_add_block, _key_rm_block, _key_pick_block, _key_screenshot, _key_h, _key_g, _key_j, _key_o, _key_time_mul, _key_jump, _key_1, _key_2, _key_3, _key_4, _key_5, _key_6, _key_7, _key_8, _key_9;
		bool _debug_mode, _game_mode, _outline, _paused, _esc_released, _e_released;
		float _break_time, _eat_timer;
		int _break_frame;
		std::string _world_name;
		std::vector<Chunk *> _visible_chunks;
		std::vector<std::pair<int, glm::vec3>> _entities;
		std::thread _thread;
		t_hit _block_hit;
		UI *_ui;
		Menu *_menu;

		void resetInputsPtrs( void );

		void update_visible_chunks( void );
		void chunk_update( void );
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

		void drawEntities( int size );

	public:
		std::list<Chunk *> _chunks;
		std::vector<Chunk *> _perimeter_chunks, _deleted_chunks;
		std::map<std::pair<int, int>, s_backup> _backups;
		Camera *_camera;
		Inventory *_inventory;

		OpenGL_Manager( void );
		~OpenGL_Manager( void );

		void setup_window( void );
		void initWorld( void );
		void create_shaders( void );
		void setup_communication_shaders( void );
		void load_texture( std::string texture_file );
		void setGamemode( bool gamemode );
		void main_loop( void );
};

#endif
