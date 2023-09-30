#ifndef MENU_HPP
# define MENU_HPP

enum {
	MAIN_MENU,
	LOAD_MENU,
	PAUSE_MENU,
	INVENTORY_MENU
};

class Menu
{
	private:
	    GLuint _vao, _vbo;
		GLuint _shaderProgram;
        GLint _nb_points, _state, _selection;
		glm::ivec2 _selected_block;
		float _mouseX, _mouseY;
		bool _vaoSet, _esc_released, _e_released, _left_released, _right_released;
		Inventory &_inventory;
		Text *_text;

		int main_menu( GLFWwindow* window );
		int loading_screen( std::list<Chunk *> chunks, GLint render_dist );
		int pause_menu( GLFWwindow* window );
		int inventory_menu( GLFWwindow* window );
        void setup_array_buffer_main( void );
		void setup_array_buffer_load( int completion );
        void setup_array_buffer_pause( void );
		void display_slot_value( int index );
		void display_backpack_value( int index );
		void display_icraft_value( int index );
		void add_slot_value( GLint *vertices, int mult, int index, int & vindex );
		void add_backpack_value( GLint *vertices, int mult, int index, int & vindex );
		void add_icraft_value( GLint *vertices, int mult, int index, int & vindex );
		void add_crafted_value( GLint *vertices, int mult, int & vindex );
        void setup_array_buffer_inventory( void );
		void setup_shader( GLint *vertices );

	public:
		Menu( Inventory & inventory, Text *text );
		~Menu( void );

		void processMouseMovement( float posX, float posY );
		void setShaderProgram( GLuint shaderProgram );
		void setState( int state );
		int getState( void );
		int run( GLFWwindow* window, std::list<Chunk *> chunks, GLint render_dist );
};

#endif
