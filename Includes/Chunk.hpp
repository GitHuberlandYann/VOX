#ifndef CHUNK_HPP
# define CHUNK_HPP

# define SEA_LEVEL 64
# define CHUNK_SIZE 16

// enum {
// 	ADD_BLOCK,
// 	RM_BLOCK
// };

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
		// void action_block( glm::vec3 pos, glm::vec3 front, int action);
        void drawArray( void );
};

#endif
