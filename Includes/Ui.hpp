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
		GLfloat *_vertices;
        GLint _nb_points;

        void setup_array_buffer( void );

    public:
        UI( void );
        ~UI( void );

        void setup( void );
        void drawUserInterface( void );
};

#endif
