#ifndef OPENGL_MANAGER_HPP
# define OPENGL_MANAGER_HPP

# if __linux__
#  define WIN_WIDTH 1680//1540//1920
#  define WIN_HEIGHT 1050//845//1080
#  define IS_LINUX true
# else
#  define WIN_WIDTH 2560
#  define WIN_HEIGHT 1440
#  define IS_LINUX false
# endif

# define RENDER_DISTANCE 10//20

enum {
	BLOCKATTRIB,
	BREAKATTRIB,
	ADJATTRIB,
	POSATTRIB
};

enum {
	FILL,
	LINE,
	F_LAST
};

enum {
	CREATIVE = false,
	SURVIVAL = true
};

class OpenGL_Manager
{
	private:
		GLFWwindow* _window;
		GLuint _shaderProgram, _vertexShader, _geometryShader, _fragmentShader;
		// GLint _uniPV;
		GLint _uniView, _uniProj;
		glm::ivec2 _current_chunk;
		GLuint *_textures;
		glm::vec3 _background_color;
		GLint _key_rdist, _render_distance, _key_fill, _fill, _key_add_block, _key_rm_block, _key_pick_block, _key_h, _key_g, _key_j, _key_o, _key_1, _key_2, _key_3, _key_4, _key_5, _key_6, _key_7, _key_8, _key_9;
		bool _debug_mode, _game_mode, _f5_mode, _outline, _paused, _esc_released, _e_released;
		float _break_time;
		int _break_frame;
		std::list<Chunk *> _chunks;
		std::list<Chunk *> _visible_chunks;
		// std::list<Chunk *> _delete_chunks;
		std::list<Chunk *> _perimeter_chunks;
		std::thread _thread;
		glm::ivec4 _block_hit;
		Camera *_camera;
		Inventory *_inventory;
		UI *_ui;
		Menu *_menu;

		void update_visible_chunks( void );
		void chunk_update( void );
		void user_inputs( float deltaTime, bool rayCast );
		// void update_cam_matrix( void );
		glm::ivec4 get_block_hit( void );
		void handle_add_rm_block( bool adding, bool collect );
		void update_cam_view( void );
		void update_cam_perspective( void );

	public:
		OpenGL_Manager( void );
		~OpenGL_Manager( void );

		void setup_window( void );
		void initWorld( void );
		void create_shaders( void );
		void setup_communication_shaders( void );
		void load_texture( std::string texture_file );
		void main_loop( void );
};

#endif
