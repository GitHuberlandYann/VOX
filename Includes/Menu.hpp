#ifndef MENU_HPP
# define MENU_HPP

enum {
	MAIN_MENU,
	LOAD_MENU,
	PAUSE_MENU,
	INVENTORY_MENU,
	CRAFTING_MENU
};

class Menu
{
	private:
	    GLuint _vao, _vbo;
		GLuint _shaderProgram;
        GLint _nb_points, _state, _selection;
		glm::ivec2 _selected_block;
		bool _vaoSet, _esc_released, _e_released, _left_released, _right_released;
		GLFWwindow *_window;
		Inventory &_inventory;
		Text *_text;
		std::list<Chunk *> _chunks;

		int main_menu( void );
		int loading_screen( GLint render_dist );
		int pause_menu( void );
		int inventory_menu( void );
		int crafting_menu( void );
        void setup_array_buffer_main( void );
		void setup_array_buffer_load( int completion );
        void setup_array_buffer_pause( void );
		void display_slot_value( int index );
		void display_backpack_value( int index );
		void display_icraft_value( int index );
		void display_craft_value( int index );
		void add_slot_value( GLint *vertices, int mult, int index, int & vindex );
		void add_backpack_value( GLint *vertices, int mult, int index, int & vindex );
		void add_icraft_value( GLint *vertices, int mult, int index, int & vindex );
		void add_craft_value( GLint *vertices, int mult, int index, int & vindex );
		void add_crafted_value( GLint *vertices, int mult, int & vindex );
        void setup_array_buffer_inventory( void );
        void setup_array_buffer_crafting( void );
		void setup_shader( GLint *vertices );

	public:
		Menu( Inventory & inventory, Text *text );
		~Menu( void );

		void processMouseMovement( float posX, float posY );
		void setWindow( GLFWwindow *window );
		void setShaderProgram( GLuint shaderProgram );
		void setChunks( std::list<Chunk *> chunks );
		void setState( int state );
		int getState( void );
		int run( GLint render_dist );
};

#endif
