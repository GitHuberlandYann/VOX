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
		MAIN_VIDEO_SETTINGS,
		MAIN_RESOURCE_PACKS,
		WORLD_SELECT,
		LOAD,
		DEATH,
		PAUSE,
		OPTIONS,
		VIDEO_SETTINGS,
		RESOURCE_PACKS,
		INVENTORY,
		CRAFTING,
		CHEST,
		FURNACE,
		CHAT,
		COMMAND,
	};

	enum RET {
		QUIT = -1,
		NO_CHANGE,
		BACK_TO_GAME,
		WORLD_SELECTED,
		SAVE_AND_QUIT,
		PLAY_DEFAULT,
		RESPAWN_PLAYER,
		RESPAWN_SAVE_QUIT,
		FOV_UPDATE,
		RENDER_DIST_UPDATE,
		BRIGHTNESS_UPDATE,
	};
}

class Menu
{
	private:
	    GLuint _vao, _vbo;
		GLuint _shaderProgram;
        GLint _gui_size, _nb_points, _state, _selection, _selected_world;
		t_item _selected_block;
		bool _vaoSet, _textBar, _moving_slider;
		float _fov_gradient, _render_gradient, _brightness_gradient;
		std::vector<std::string> _worlds, _resource_packs, _active_resource_packs;
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
		MENU::RET main_menu( void );
		MENU::RET world_select_menu( void );
		MENU::RET loading_screen( void );
		MENU::RET death_menu( void );
		MENU::RET pause_menu( void );
		MENU::RET options_menu( void );
		MENU::RET video_menu( void );
		MENU::RET resource_packs_menu( void );
		MENU::RET ingame_inputs( void );
		MENU::RET ingame_menu( void );
		MENU::RET chat_menu( bool animUpdate );

        void setup_array_buffer_main( void );
        void setup_array_buffer_select( void );
		void setup_array_buffer_load( int completion );
		void setup_array_buffer_death( void );
        void setup_array_buffer_pause( void );
        void setup_array_buffer_options( void );
        void setup_array_buffer_video( void );
        void setup_array_buffer_resource_packs( void );
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

		void loadSettings( void );
		void saveSettings( void );

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
		void changeGuiSize( void );
		std::string getWorldFile( void );
		MENU::RET run( bool animUpdate );

		std::string getInfoString( void );
};

#endif
