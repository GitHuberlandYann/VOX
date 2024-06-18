#ifndef TEXT_HPP
# define TEXT_HPP

# include "Shader.hpp"
# include "Buffer.hpp"

namespace TEXT
{
	const unsigned WHITE = 0xFFFFFFFF;
	const unsigned BLACK = 0xFF000000;
	const unsigned GREEN = 0xFF00FF00;
	const unsigned RED = 0xFFFF0000;
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

        void addText( int posX, int posY, int font_size, unsigned color, int depth, std::string str );
		void addCenteredText( int left, int top, int width, int height, int font_size, bool shadow, int depth, std::string str );
		void toScreen( void );

	private:
		Shader _shader;
		Buffer _vabo;
		GLuint _texture;
		std::vector<t_text_shader_input> _texts;

		void addQuads( int spec, int posX, int posY, int width, int height, unsigned color, int depth );
};

#endif
