#ifndef CHUNK_HPP
# define CHUNK_HPP

# define SEA_LEVEL 64
# define CHUNK_SIZE 16
# define WORLD_HEIGHT 256

// enum {
// 	ADD_BLOCK,
// 	RM_BLOCK
// };

enum blocks {
	AIR,
	GRASS_BLOCK,
	OAK_TRUNK,
	BEDROCK = 16,
	DIRT_BLOCK,
	STONE,
	SAND,
	GRAVEL,
	OAK_LEAVES,
	COAL_ORE = 32,
	IRON_ORE,
	DIAMOND_ORE,
	POPPY = 48,
	DANDELION,
	BLUE_ORCHID,
	ALLIUM,
	CORNFLOWER,
	PINK_TULIP,
	GRASS,
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
        bool _isVisible, _vaoSet, _vaoReset;
        GLint _startX, _startY;
		GLint *_blocks;
		GLint *_vertices;
		GLint _displayed_blocks;
		std::thread _thread;
		std::mutex _mtx;

		void gen_ore_blob( int ore_type, int row, int col, int level, int & blob_size, int dir);
		GLint get_empty_faces( int row, int col, int level, bool isNotLeaves );
		bool exposed_block( int row, int col, int level, bool isNotLeaves );
		int get_block_type(siv::PerlinNoise perlin, int row, int col, int level, int surface_level,
			bool poppy, bool dandelion, bool blue_orchid, bool allium, bool cornflower, bool pink_tulip,
			bool grass, bool tree_gen, std::vector<glm::ivec3> & trees);
		void generate_blocks( void );
		int sand_fall_endz( glm::ivec3 pos );
		void remove_block( Inventory *inventory, glm::ivec3 pos );
		void add_block( Inventory *inventory, glm::ivec3 pos, int type );
        void fill_vertex_array( void );
        void setup_array_buffer( void );

    public:
        Chunk( int posX, int posY );
        ~Chunk( void );

		GLint getStartX( void );
		GLint getStartY( void );
		void generation( void );
		void regeneration( Inventory *inventory, glm::ivec3 pos, bool adding );
		void generate_chunk( std::list<Chunk *> *chunks );
        void setVisibility( std::list<Chunk *> *visible_chunks, int posX, int posY, GLint render_dist );
        bool shouldDelete( glm::vec3 pos, GLfloat dist );
        bool isInChunk( int posX, int posY );
		bool isHit( glm::ivec3 pos );
		void handleHit( Inventory *inventory, glm::ivec3 pos, bool adding );
		bool collision( glm::vec3 & pos, Camera &cam );
        void drawArray( GLint & counter );
};

#endif
