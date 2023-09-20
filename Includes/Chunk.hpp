#ifndef CHUNK_HPP
# define CHUNK_HPP

# define SEA_LEVEL 64
# define CHUNK_SIZE 16

// enum {
// 	ADD_BLOCK,
// 	RM_BLOCK
// };

enum blocks {
	AIR,
	GRASS_BLOCK,
	GRASS_BLOCK_UNDER,
	STONE,
	SAND,
	OAK_TRUNK,
	OAK_LEAVES,
	TBD7,
	TBD8,
	TBD9,
	TBD10,
	TBD11,
	TBD12,
	TBD13,
	TBD14,
	TBD15,
	BEDROCK,
	POPPY,
	DANDELION,
	BLUE_ORCHID,
	ALLIUM,
	CORNFLOWER,
	PINK_TULIP,
	NOTVISIBLE = 1000
};

const GLint adj_blocks[6][3] = {
	{-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1}
};

const GLint oak_normal[61][3] = {
	{-2, 2, -2}, {-1, 2, -2}, {0, 2, -2}, {1, 2, -2}, {2, 2, -2},
	{-2, 1, -2}, {-1, 1, -2}, {0, 1, -2}, {1, 1, -2}, {2, 1, -2},
	{-2, 0, -2}, {-1, 0, -2}, {1, 0, -2}, {2, 0, -2},
	{-2, -1, -2}, {-1, -1, -2}, {0, -1, -2}, {1, -1, -2}, {2, -1, -2},
	{-2, -2, -2}, {-1, -2, -2}, {0, -2, -2}, {1, -2, -2}, {2, -2, -2},
	
	{-2, 2, -1}, {-1, 2, -1}, {0, 2, -1}, {1, 2, -1}, {2, 2, -1},
	{-2, 1, -1}, {-1, 1, -1}, {0, 1, -1}, {1, 1, -1}, {2, 1, -1},
	{-2, 0, -1}, {-1, 0, -1}, {1, 0, -1}, {2, 0, -1},
	{-2, -1, -1}, {-1, -1, -1}, {0, -1, -1}, {1, -1, -1}, {2, -1, -1},
	{-2, -2, -1}, {-1, -2, -1}, {0, -2, -1}, {1, -2, -1}, {2, -2, -1},

	{-1, 1, 0}, {0, 1, 0}, {1, 1, 0},
	{-1, 0, 0}, {1, 0, 0},
	{-1, -1, 0}, {0, -1, 0}, {1, -1, 0},

	{0, -1, 1},
	{-1, 0, 1}, {0, 0, 1}, {1, 0, 1},
	{0, 1, 1}
};

class Chunk
{
    private:
        GLuint _vao, _vbo;
        bool _isVisible, _vaoSet;
        GLint _startX, _startY;
		GLint *_blocks;
		GLint *_vertices;
		GLint _displayed_blocks;
		std::thread _thread;

		GLint get_empty_faces( int row, int col, int level );
		bool exposed_block( int row, int col, int level );
		int get_block_type(siv::PerlinNoise perlin, int row, int col, int level, int surface_level,
			bool poppy, bool dandelion, bool blue_orchid, bool allium, bool cornflower, bool pink_tulip,
			bool tree_gen, std::vector<glm::ivec3> & trees);
		void generate_blocks( void );
		void remove_block( glm::ivec3 pos );
        void fill_vertex_array( void );
        void setup_array_buffer( void );

    public:
        Chunk( int posX, int posY );
        ~Chunk( void );

		void generation( void );
		void regeneration( glm::ivec3 pos );
		void generate_chunk( std::list<Chunk *> *chunks );
        void setVisibility( std::list<Chunk *> *visible_chunks, int posX, int posY, GLint render_dist );
        bool shouldDelete( glm::vec3 pos, GLfloat dist );
        bool isInChunk( int posX, int posY );
		bool isHit( glm::ivec3 pos );
		void handleHit( glm::ivec3 pos );
		// void action_block( glm::vec3 pos, glm::vec3 front, int action);
        void drawArray( GLint & counter );
};

#endif
