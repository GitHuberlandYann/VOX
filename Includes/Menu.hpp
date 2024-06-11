#ifndef MENU_HPP
# define MENU_HPP

# include "Chunk.hpp"
# include "Ui.hpp"
# include "Text.hpp"
# include "Chat.hpp"
class ChestInstance;
# include <list>

# define ALPHABETA " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"

namespace menu
{
	enum {
		main,
		main_options,
		main_video_settings,
		main_controls,
		main_resource_packs,
		world_select,
		world_create,
		load,
		death,
		pause,
		options,
		video_settings,
		controls,
		resource_packs,
		inventory,
		crafting,
		chest,
		furnace,
		chat,
		command,
		sign,
	};

	enum ret {
		quit = -1,
		no_change,
		back_to_game,
		world_selected,
		world_created,
		save_and_quit,
		respawn_player,
		respawn_save_quit,
		fov_update,
		render_dist_update,
		brightness_update,
		apply_resource_packs,
		sign_done,
	};

	const int resolutions_size = 7;
	const std::array<std::array<int, 2>, resolutions_size> resolutions = {{
		{960,  540},  // 540p  / qHD
		{1280, 720},  // 720p  / HD
		{1920, 1080}, // 1080p / Full HD
		{2048, 1048}, // 2K
		{2560, 1440}, // 1440p / QuadHD
		{3840, 2160}, // 2160p / UltraHD
		{4096, 2160}, // 4K
	}};
}

typedef struct s_sign_info {
	glm::ivec3 pos = {0, 0, 0};
	std::vector<std::string> content;
}				t_sign_info;

class Menu
{
	public:
		Menu( void );
		~Menu( void );

		void processMouseMovement( float posX, float posY );
		void setWindow( GLFWwindow *window );
		void setShaderProgram( GLuint shaderProgram );
		void setPtrs( Inventory* inventory, UI* ui );
		void setChunks( std::list<Chunk *>& chunks );
		void setChestInstance( ChestInstance* chest );
		void setFurnaceInstance( FurnaceInstance* furnace );
		void setSignPos( glm::ivec3 pos );
		void setState( int state );
		int getState( void );
		void changeGuiSize( void );
		std::string getWorldFile( void );
		t_sign_info getSignContent( void );
		menu::ret run( bool animUpdate );

		std::string getInfoString( void );

	private:
	    GLuint _vao, _vbo;
		GLuint _shaderProgram;
        GLint _gui_size, _nb_points, _state, _selection, _selected_world;
		t_item _selected_block;
		bool _vaoSet, _textBar, _input_world, _input_seed, _moving_slider, _change_to_apply, _drop_down_menu;
		float _fov_gradient, _render_gradient, _brightness_gradient;
		std::vector<std::string> _worlds, _resource_packs, _active_resource_packs, _sign_content;
		glm::ivec3 _sign_pos;
		std::vector<int> _selection_list;
		std::vector<std::array<int, 3>> _vertices;
		std::string _world_file;
		GLFWwindow* _window;
		Inventory* _inventory;
		UI* _ui;
		std::shared_ptr<Chat> _chat;
		std::shared_ptr<Text> _text;
		std::list<Chunk*> _chunks;
		ChestInstance* _chest;
		FurnaceInstance* _furnace;

		void reset_values( void );
		menu::ret main_menu( void );
		menu::ret enter_world_select_menu( void );
		menu::ret world_select_menu( void );
		menu::ret world_create_menu( bool animUpdate );
		menu::ret loading_screen_menu( void );
		menu::ret death_menu( void );
		menu::ret pause_menu( void );
		menu::ret options_menu( void );
		menu::ret video_menu( void );
		menu::ret controls_menu( void );
		menu::ret resource_packs_menu( void );
		menu::ret ingame_inputs( void );
		menu::ret ingame_menu( void );
		menu::ret chat_menu( bool animUpdate );
		menu::ret sign_menu( bool animUpdate );

		void addQuads( int atlas, int posX, int posY, int width, int height, int texX, int texY, int texWidth, int texHeight );
        void setup_array_buffer_main( void );
        void setup_array_buffer_select( void );
        void setup_array_buffer_create( void );
		void setup_array_buffer_load( int completion );
		void setup_array_buffer_death( void );
        void setup_array_buffer_pause( void );
        void setup_array_buffer_options( void );
        void setup_array_buffer_video( void );
        void setup_array_buffer_controls( void );
        void setup_array_buffer_resource_packs( void );
        void setup_array_buffer_chat( void );
        void setup_array_buffer_sign( void );

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
};

#endif
