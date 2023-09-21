#ifndef UI_HPP
# define UI_HPP

enum {
    UI_POSATTRIB,
    UI_SIZEATTRIB,
    UI_COLORATTRIB
};

class UI
{
    private:
        GLuint _vao, _vbo;
		GLuint _shaderProgram, _vertexShader, _geometryShader, _fragmentShader;
		GLfloat *_vertices;
        GLint _nb_points;
		bool _vaoSet;
		Text *_text;

        void setup_array_buffer( void );

    public:
        UI( void );
        ~UI( void );

        void setup_shader( void );
        void drawUserInterface( int fps );
};

#endif
