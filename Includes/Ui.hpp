#ifndef UI_HPP
# define UI_HPP

# include "Text.hpp"
# include "Chat.hpp"

enum {
    UI_ATLASATTRIB,
    UI_POSATTRIB,
    UI_TEXATTRIB
};

class UI
{
    private:
        GLuint _vao, _vbo;
		GLuint _shaderProgram, _vertexShader, _geometryShader, _fragmentShader;
		GLuint *_textures;
        GLint _nb_points;
		Text *_text;
		Chat *_chat;
		Inventory &_inventory;
        Camera &_camera;
		bool _vaoSet;

		void load_texture( std::string texstr, std::string shname, int index );
        void add_inventory_elem( GLint *vertices, int mult, int index, int & vindex );
        void add_dura_value( GLint *vertices, int mult, int index, int & vindex );
        void add_hearts_holder( GLint *vertices, int mult, int index, int & vindex );
        void add_hearts( GLint *vertices, int mult, int index, int & vindex );
        void add_armor_holder( GLint *vertices, int mult, int index, int & vindex );
        void add_armor( GLint *vertices, int mult, int index, int & vindex );
        void add_food_holder( GLint *vertices, int mult, int index, int & vindex, int saturation );
        void add_food( GLint *vertices, int mult, int index, int & vindex );
        void add_bubbles( GLint *vertices, int mult, int index, int & vindex );
        void setup_array_buffer( void );
        void display_slot_value( int index );

    public:
        bool _hideUI;

        UI( Inventory & inventory, Camera &camera );
        ~UI( void );

		Text *getTextPtr( void );
		Chat *getChatPtr( void );
		GLuint getShaderProgram( void );
        void setup_shader( void );
        void drawUserInterface( std::string str, bool game_mode, float deltaTime );
        void chatMessage( std::string str );
		void textToScreen( void );
};

#endif
