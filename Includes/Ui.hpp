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
		bool _vaoSet;

		void load_texture( void );
        void setup_array_buffer( void );

    public:
        UI( Inventory & inventory );
        ~UI( void );

        void setup_shader( void );
        void drawUserInterface( std::string str, bool game_mode );
};

#endif
