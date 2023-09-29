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
        GLint _nb_points, _state;
		bool _vaoSet, _esc_released;
		Text *_text;

		int main_menu( GLFWwindow* window );
		int loading_screen( std::list<Chunk *> chunks, GLint render_dist );
		int pause_menu( GLFWwindow* window );
		void setup_array_buffer_load( int completion );
        void setup_array_buffer( void );

	public:
		Menu( Text *text );
		~Menu( void );

		void processMouseMovement( float x_offset, float y_offset );
		void setShaderProgram( GLuint shaderProgram );
		void setState( int state );
		int getState( void );
		int run( GLFWwindow* window, std::list<Chunk *> chunks, GLint render_dist );
};

#endif
