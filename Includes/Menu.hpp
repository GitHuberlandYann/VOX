#ifndef MENU_HPP
# define MENU_HPP

# include "Chunk.hpp"
# include "Ui.hpp"
# include "Text.hpp"
# include "Chat.hpp"
class ChestInstance;
# include <list>

# define ALPHABETA " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"

namespace MENU
{
	enum {
		MAIN,
		MAIN_OPTIONS,
		WORLD_SELECT,
		LOAD,
		DEATH,
		PAUSE,
		OPTIONS,
		INVENTORY,
		CRAFTING,
		CHEST,
		FURNACE,
		CHAT
	};
}

class Menu
{
	private:
	    GLuint _vao, _vbo;
		GLuint _shaderProgram;
        GLint _gui_size, _nb_points, _state, _selection, _selected_world;
		t_item _selected_block;
		bool _vaoSet, _esc_released, _e_released, _left_released, _right_released, _textBar;
		GLint _key_1, _key_2, _key_3, _key_4, _key_5, _key_6, _key_7, _key_8, _key_9, _chat_released;
		std::vector<std::string> _worlds;
		std::vector<int> _selection_list;
		std::vector<std::array<int, 9>> _vertices;
		std::string _world_file;
		GLFWwindow *_window;
		Inventory &_inventory;
		UI *_ui;
		Text *_text;
		Chat *_chat;
		std::list<Chunk *> _chunks;
		ChestInstance *_chest;
		FurnaceInstance *_furnace;

		void reset_values( void );
		int main_menu( void );
		int world_select_menu( void );
		int loading_screen( GLint render_dist );
		int death_menu( void );
		int pause_menu( void );
		int options_menu( void );
		int ingame_inputs( void );
		int ingame_menu( void );
		int chat_menu( bool animUpdate );

        void setup_array_buffer_main( void );
        void setup_array_buffer_select( void );
		void setup_array_buffer_load( int completion );
		void setup_array_buffer_death( void );
        void setup_array_buffer_pause( void );
        void setup_array_buffer_options( void );
        void setup_array_buffer_chat( void );

		void occult_selection( void );
		void display_item_value( int x, int y, int amount );
		void add_dura_value( glm::ivec2 dura, int x, int y );
		void add_item_value( t_item item, int x, int y, bool movement = false );
	
		void add_slot_value( int index );
		void add_backpack_value( int index );
		void add_icraft_value( int index );
		void add_craft_value( int index );
		void add_crafted_value( void );
		void add_chest_value( int index );
		void add_furnace_value( void );

        void setup_array_buffer_inventory( void );
        void setup_array_buffer_crafting( void );
        void setup_array_buffer_chest( void );
        void setup_array_buffer_furnace( void );
		void setup_shader( void );

	public:
		Menu( Inventory & inventory, UI *ui );
		~Menu( void );

		void processMouseMovement( float posX, float posY );
		void setWindow( GLFWwindow *window );
		void setShaderProgram( GLuint shaderProgram );
		void setChunks( std::list<Chunk *> &chunks );
		void setChestInstance( ChestInstance *chest );
		void setFurnaceInstance( FurnaceInstance *furnace );
		void setState( int state );
		int getState( void );
		void changeGuiSize( int offset );
		std::string getWorldFile( void );
		int run( GLint render_dist, bool animUpdate );

		std::string getInfoString( void );
};

#endif
