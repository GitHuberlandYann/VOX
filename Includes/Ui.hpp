#ifndef UI_HPP
# define UI_HPP

# include "Text.hpp"
# include "Chat.hpp"

enum {
	ITEM_SPECATTRIB,
	ITEM_POSATTRIB
};

enum {
    UI_ATLASATTRIB,
    UI_POSATTRIB,
    UI_TEXATTRIB
};

class Inventory;
class Camera;

typedef struct s_inv_mess {
	std::string str = "";
	float timer = 2;
}				t_inv_mess;

class UI
{
    private:
        GLuint _vao, _vbo, _item_vao, _item_vbo;
		GLuint _shaderProgram, _itemShaderProgram;
		GLuint *_textures;
        GLint _gui_size, _nb_points, _nb_points_crea, _nb_items;
		bool _movement;
		std::vector<int> _items;
		t_inv_mess _inventoryMessage;
		Text *_text;
		Chat *_chat;
		Inventory &_inventory;
        Camera &_camera;
		bool _vaoSet;

		void load_texture( std::string texstr, std::string shname, int index );
        void add_inventory_elem( int index );
        void add_dura_value( std::vector<std::array<int, 9>> &vertices, int index );
        void add_hearts_holder( std::vector<std::array<int, 9>> &vertices, int index );
        void add_hearts( std::vector<std::array<int, 9>> &vertices, int index );
        void add_armor_holder( std::vector<std::array<int, 9>> &vertices, int index );
        void add_armor( std::vector<std::array<int, 9>> &vertices, int index );
        void add_food_holder( std::vector<std::array<int, 9>> &vertices, int index, int saturation );
        void add_food( std::vector<std::array<int, 9>> &vertices, int index );
        void add_bubbles( std::vector<std::array<int, 9>> &vertices, int index );
		void fill_vertices( std::vector<std::array<int, 9>> &vertices, std::array<int, 9> values );
        void setup_array_buffer( void );
        void setup_item_array_buffer( void );
		void display_item_value( int x, int y, int amount );
        void display_slot_value( int index );

    public:
        bool _hideUI;

        UI( Inventory & inventory, Camera &camera );
        ~UI( void );

		Text *getTextPtr( void );
		Chat *getChatPtr( void );
		void changeGuiSize( int offset );
        void setFov( float fov );
		GLuint getShaderProgram( void );
        void setup_shader( void );
		void addFace( glm::ivec3 v0, glm::ivec3 v1, glm::ivec3 v2, glm::ivec3 v3, bool alien, bool movement = false );
        void drawUserInterface( std::string str, bool game_mode, float deltaTime );
        void chatMessage( std::string str );
		void inventoryMessage( std::string str );
		void textToScreen( void );
};

#endif
