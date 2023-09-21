#ifndef OPENGL_MANAGER_HPP
# define OPENGL_MANAGER_HPP

# if __linux__
#  define WIN_WIDTH 1800
#  define WIN_HEIGHT 900
# else
#  define WIN_WIDTH 2560
#  define WIN_HEIGHT 1400
# endif

# define RENDER_DISTANCE 20

enum {
	BLOCKATTRIB,
	ADJATTRIB,
	POSATTRIB
};

enum {
	FILL,
	LINE,
	F_LAST
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
		GLint _key_rdist, _render_distance, _key_fill, _fill, _key_add_block, _key_rm_block;
		std::list<Chunk *> _chunks;
		std::list<Chunk *> _visible_chunks;
		std::list<Chunk *> _delete_chunks;
		std::thread _thread;//, _thread_block;
		UI *_ui;

		void chunk_update( void );
		void user_inputs( float deltaTime );
		// void update_cam_matrix( void );
		void handle_add_rm_block( bool adding );
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
