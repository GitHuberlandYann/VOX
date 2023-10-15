#ifndef TEXT_HPP
# define TEXT_HPP

enum {
    TEXT_POSATTRIB,
    TEXT_SIZEATTRIB,
    TEXT_CHARATTRIB,
	TEXT_COLORATTRIB
};

class Text
{
	private:
        GLuint _vao, _vbo;
		GLuint _shaderProgram, _vertexShader, _geometryShader, _fragmentShader;
		GLuint *_textures;
		std::vector<int> _texts;

        void setup_array_buffer( void );

	public:
		Text( void );
		~Text( void );

        void setup_shader( void );
		void load_texture( void );
        void addText( int posX, int posY, int font_size, bool white, std::string str );
		void toScreen( void );
};

#endif
