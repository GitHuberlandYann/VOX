#ifndef CHUNK_HPP
# define CHUNK_HPP

class Chunk
{
    private:
        GLuint _vao, _vbo;
        glm::vec2 _start;

        void fill_vertex_array( GLfloat *vertices, GLfloat z );

    public:
        Chunk( glm::vec2 start );
        ~Chunk( void );

        void setup_array_buffer( GLfloat z );
        void drawArray( void );
};

#endif
