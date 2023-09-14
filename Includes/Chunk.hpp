#ifndef CHUNK_HPP
# define CHUNK_HPP

class Chunk
{
    private:
        GLuint _vao, _vbo;

    public:
        Chunk( void );
        ~Chunk( void );

        void setup_array_buffer( GLfloat z );
        void drawArray( void );
};

#endif
