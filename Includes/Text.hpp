#ifndef TEXT_HPP
# define TEXT_HPP

enum {
    TEXT_POSATTRIB,
    TEXT_SIZEATTRIB,
    TEXT_CHARATTRIB
};

class Text
{
	private:
        GLuint _vao, _vbo;
		GLuint _shaderProgram, _vertexShader, _geometryShader, _fragmentShader;
		GLint _uniColor;
		GLuint *_textures;

        void setup_array_buffer( void );

	public:
		Text( void );
		~Text( void );

        void setup_shader( void );
		void load_texture( void );
        void displayText( int posX, int posY, int font_size, glm::vec3 color, std::string str );
};

#endif
