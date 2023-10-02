#ifndef MENU_HPP
# define MENU_HPP

enum {
	MAIN_MENU,
	LOAD_MENU,
	PAUSE_MENU,
	INVENTORY_MENU,
	CRAFTING_MENU,
	FURNACE_MENU
};

class Menu
{
	private:
	    GLuint _vao, _vbo;
		GLuint _shaderProgram;
        GLint _nb_points, _state, _selection;
		glm::ivec2 _selected_block;
		bool _vaoSet, _esc_released, _e_released, _left_released, _right_released;
		GLint _key_1, _key_2, _key_3, _key_4, _key_5, _key_6, _key_7, _key_8, _key_9;
		GLFWwindow *_window;
		Inventory &_inventory;
		Text *_text;
		std::list<Chunk *> _chunks;

		void reset_values( void );
		int main_menu( void );
		int loading_screen( GLint render_dist );
		int pause_menu( void );
		int ingame_inputs( void );
		int ingame_menu( void );
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
		void add_dura_value( GLint *vertices, int mult, int index, int & vindex );
		void add_crafted_value( GLint *vertices, int mult, int & vindex );
        void setup_array_buffer_inventory( void );
        void setup_array_buffer_crafting( void );
        void setup_array_buffer_furnace( void );
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
