#ifndef TEXT_HPP
# define TEXT_HPP

namespace TEXT
{
	const unsigned WHITE = 0xFFFFFFFF;
	const unsigned BLACK = 0xFF000000;
	const unsigned GREEN = 0xFF00FF00;
	const unsigned RED = 0xFFFF0000;

	enum {
		SPECATTRIB,
		COLATTRIB,
		POSATTRIB
	};
}

typedef struct s_text_shader_input {
	int spec;
	unsigned color;
	int posX;
	int posY;
}				t_text_shader_input;

class Text
{
	public:
		Text( void );
		~Text( void );

        void setupShader( void );
		void loadTexture( void );
		void updateWinSize( void );

        void addText( int posX, int posY, int font_size, unsigned color, std::string str );
		void addCenteredText( int left, int top, int width, int height, int font_size, bool shadow, std::string str );
		void toScreen( void );

	private:
        GLuint _vao, _vbo;
		GLuint _shaderProgram;
		GLuint _texture;
		std::vector<t_text_shader_input> _texts;

		void addQuads( int spec, int posX, int posY, int width, int height, unsigned color );
};

#endif
