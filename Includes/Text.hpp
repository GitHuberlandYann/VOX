#ifndef TEXT_HPP
# define TEXT_HPP

namespace TEXT
{
	const int WHITE = 0xFFFFFFFF;
	const int BLACK = 0xFF000000;

	enum {
		SPECATTRIB,
		COLATTRIB,
		POSATTRIB
	};
}

class Text
{
	private:
        GLuint _vao, _vbo;
		GLuint _shaderProgram;
		GLuint *_textures;
		std::vector<std::array<int, 4>> _texts;

		void addQuads( int spec, int posX, int posY, int width, int height, int color );

	public:
		Text( void );
		~Text( void );

        void setup_shader( void );
		void load_texture( void );
		int textWidth( int font_size, std::string str, int limit = -1 );
        void addText( int posX, int posY, int font_size, int color, std::string str );
		void addCenteredText( int left, int top, int width, int height, int font_size, bool shadow, std::string str );
		void toScreen( void );
};

#endif
