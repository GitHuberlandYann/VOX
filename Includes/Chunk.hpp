#ifndef CHUNK_HPP
# define CHUNK_HPP

# define SEA_LEVEL 64

class Chunk
{
    private:
        GLuint _vao, _vbo;
        bool _isVisible, _vaoSet;
        glm::vec2 _start;
		GLint *_blocks;
		GLfloat *_vertices;
		GLint _displayed_blocks;
		std::thread _thread;

		GLfloat get_empty_faces( int row, int col, int level );
		bool exposed_block( int row, int col, int level );
		void generate_blocks( void );
        void fill_vertex_array( void );

    public:
        Chunk( glm::vec2 start );
        ~Chunk( void );

		void generation( void );
		void generate_chunk( std::list<Chunk *> *chunks );
        void setVisibility( bool value );
        bool shouldDelete( glm::vec3 pos, GLfloat dist );
        bool isInChunk( glm::vec2 pos );
        void setup_array_buffer( void );
        void drawArray( void );
};

#endif
