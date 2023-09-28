#ifndef MENU_HPP
# define MENU_HPP

class Menu
{
	private:
	    GLuint _vao, _vbo;
		// GLuint *_textures;
        GLint _nb_points;
		bool _vaoSet, _esc_released;
		Text *_text;

        void setup_array_buffer( void );

	public:
		Menu( Text *text );
		~Menu( void );

		int pause_menu( GLFWwindow* window );
};

#endif
