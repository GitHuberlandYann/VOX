#ifndef CHUNK_HPP
# define CHUNK_HPP

# define SEA_LEVEL 64

class Chunk
{
    private:
        GLuint _vao, _vbo;
        bool _isVisible, _vaoSet;
        GLint _startX, _startY;
		GLint *_blocks;
		GLfloat *_vertices;
		GLint _displayed_blocks;
		std::thread _thread;

		GLfloat get_empty_faces( int row, int col, int level );
		bool exposed_block( int row, int col, int level );
		void generate_blocks( void );
        void fill_vertex_array( void );
        void setup_array_buffer( void );

    public:
        Chunk( int posX, int posY );
        ~Chunk( void );

		void generation( void );
		void generate_chunk( std::list<Chunk *> *chunks );
        void setVisibility( std::list<Chunk *> *visible_chunks, int posX, int posY, GLint render_dist );
        bool shouldDelete( glm::vec3 pos, GLfloat dist );
        bool isInChunk( int posX, int posY );
        void drawArray( void );
};

#endif
