#ifndef TEXT_HPP
# define TEXT_HPP

namespace TEXT
{
	const uint WHITE = 0xFFFFFFFF;
	const uint BLACK = 0xFF000000;
	const uint GREEN = 0xFF00FF00;
	const uint RED = 0xFFFF0000;

	enum {
		SPECATTRIB,
		COLATTRIB,
		POSATTRIB
	};
}

typedef struct s_text_shader_input {
	int spec;
	uint color;
	int posX;
	int posY;
}				t_text_shader_input;

class Text
{
	private:
        GLuint _vao, _vbo;
		GLuint _shaderProgram;
		GLuint *_textures;
		std::vector<t_text_shader_input> _texts;

		void addQuads( int spec, int posX, int posY, int width, int height, uint color );

	public:
		Text( void );
		~Text( void );

        void setup_shader( void );
		void load_texture( void );
		void updateWinSize( void );

        void addText( int posX, int posY, int font_size, uint color, std::string str );
		void addCenteredText( int left, int top, int width, int height, int font_size, bool shadow, std::string str );
		void toScreen( void );
};

#endif
