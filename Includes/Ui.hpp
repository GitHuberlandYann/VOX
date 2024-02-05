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

class UI
{
    private:
        GLuint _vao, _vbo, _item_vao, _item_vbo;
		GLuint _shaderProgram, _itemShaderProgram;
		GLuint *_textures;
        GLint _nb_points, _nb_items;
		bool _movement;
		std::vector<int> _items;
		Text *_text;
		Chat *_chat;
		Inventory &_inventory;
        Camera &_camera;
		bool _vaoSet;

		void load_texture( std::string texstr, std::string shname, int index );
        void add_inventory_elem( int index );
        void add_dura_value( GLint *vertices, int mult, int index, int & vindex );
        void add_hearts_holder( GLint *vertices, int mult, int index, int & vindex );
        void add_hearts( GLint *vertices, int mult, int index, int & vindex );
        void add_armor_holder( GLint *vertices, int mult, int index, int & vindex );
        void add_armor( GLint *vertices, int mult, int index, int & vindex );
        void add_food_holder( GLint *vertices, int mult, int index, int & vindex, int saturation );
        void add_food( GLint *vertices, int mult, int index, int & vindex );
        void add_bubbles( GLint *vertices, int mult, int index, int & vindex );
        void setup_array_buffer( void );
        void setup_item_array_buffer( void );
        void display_slot_value( int index );

    public:
        bool _hideUI;

        UI( Inventory & inventory, Camera &camera );
        ~UI( void );

		Text *getTextPtr( void );
		Chat *getChatPtr( void );
		GLuint getShaderProgram( void );
        void setup_shader( void );
		void addFace( glm::ivec3 v0, glm::ivec3 v1, glm::ivec3 v2, glm::ivec3 v3, bool alien, bool movement = false );
        void drawUserInterface( std::string str, bool game_mode, float deltaTime );
        void chatMessage( std::string str );
		void textToScreen( void );
};

#endif
