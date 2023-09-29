#ifndef MENU_HPP
# define MENU_HPP

enum {
	MAIN_MENU,
	LOAD_MENU,
	PAUSE_MENU
};

class Menu
{
	private:
	    GLuint _vao, _vbo;
		GLuint _shaderProgram;
		// GLuint *_textures;
        GLint _nb_points, _state, _selection;
		bool _vaoSet, _esc_released;
		Text *_text;

		int main_menu( GLFWwindow* window );
		int loading_screen( std::list<Chunk *> chunks, GLint render_dist );
		int pause_menu( GLFWwindow* window );
        void setup_array_buffer_main( void );
		void setup_array_buffer_load( int completion );
        void setup_array_buffer_pause( void );
		void setup_shader( GLint *vertices );

	public:
		Menu( Text *text );
		~Menu( void );

		void processMouseMovement( float posX, float posY );
		void setShaderProgram( GLuint shaderProgram );
		void setState( int state );
		int getState( void );
		int run( GLFWwindow* window, std::list<Chunk *> chunks, GLint render_dist );
};

#endif
