#ifndef UI_HPP
# define UI_HPP

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
		Inventory &_inventory;
        Camera &_camera;
		bool _vaoSet;

		void load_texture( void );
        void setup_array_buffer( void );
        void display_slot_value( int index );

    public:
        UI( Inventory & inventory, Camera &camera );
        ~UI( void );

        void setup_shader( void );
        void drawUserInterface( std::string str, bool game_mode, bool f5_mode );
};

#endif
