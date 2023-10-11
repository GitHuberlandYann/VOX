#ifndef CHUNK_HPP
# define CHUNK_HPP

# define SEA_LEVEL 64
# define CHUNK_SIZE 16
# define WORLD_HEIGHT 256

enum cont {
	CONT_MUSHROOM_FIELDS,
	CONT_DEEP_OCEAN,
	CONT_OCEAN,
	CONT_COAST,
	CONT_NEAR_INLAND,
	CONT_MID_INLAND,
	CONT_FAR_INLAND
};

enum face_dir {
	MINUSZ,
	PLUSZ,
	MINUSX,
	PLUSX,
	MINUSY,
	PLUSY
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

struct s_backup {
	std::map<int, int> orientations;
	std::map<int, int> added;
	std::set<int> removed;
	std::map<int, FurnaceInstance> furnaces;
};

class Chunk
{
    private:
        GLuint _vao, _vbo, _waterVao, _waterVbo, _skyVao, _skyVbo;
        bool _isVisible, _vaoSet, _vaoReset, _vaoVIP, _waterVaoSet, _waterVaoReset, _waterVaoVIP, _skyVaoSet, _skyVaoReset, _skyVaoVIP;
        GLint _startX, _startY;
		GLint _continent;
		GLint *_blocks, *_vertices, *_water_vert, *_sky_vert;
		GLboolean *_sky;
		size_t _displayed_blocks, _water_count, _sky_count;
		std::list<Chunk *> _vis_chunks;
		Camera *_camera;
		std::map<int,int> _orientations, _added;
		std::set<int> _removed, _fluids; // TODO add fluids to backups
		std::map<int, FurnaceInstance> _furnaces;
		std::thread _thread;
		std::mutex _mtx;

		void gen_ore_blob( int ore_type, int row, int col, int level, int & blob_size, int dir);
		GLint get_empty_faces( int type, int row, int col, int level, bool isNotLeaves );
		bool exposed_block( int row, int col, int level, bool isNotLeaves );
		int exposed_water_faces( int row, int col, int level );
		int get_block_type_cave( int row, int col, int level, int ground_level,
			bool poppy, bool dandelion, bool blue_orchid, bool allium, bool cornflower, bool pink_tulip,
			bool grass, bool tree_gen, std::vector<glm::ivec3> & trees );
		int get_block_type( siv::PerlinNoise perlin, int row, int col, int level, int surface_level,
			bool poppy, bool dandelion, bool blue_orchid, bool allium, bool cornflower, bool pink_tulip,
			bool grass, bool tree_gen, std::vector<glm::ivec3> & trees );
		int surfaceLevel( int row, int col, siv::PerlinNoise perlin );
		void generate_blocks( void );
		void generate_sky( void );
		int sand_fall_endz( glm::ivec3 pos );
		void handle_border_block( glm::ivec3 pos, int type, bool adding );
		void remove_block( Inventory *inventory, glm::ivec3 pos );
		void add_block( Inventory *inventory, glm::ivec3 pos, int type, int previous );
        void fill_vertex_array( void );
        void setup_array_buffer( void );
		void setup_sky_array_buffer( void );
		void setup_water_array_buffer( void );

    public:
        Chunk( Camera *camera, int posX, int posY );
        ~Chunk( void );

		GLint getStartX( void );
		GLint getStartY( void );
		void setBackup( std::map<std::pair<int, int>, s_backup> *backups );
		void restoreBackup( s_backup backup);
		FurnaceInstance *getFurnaceInstance( glm::ivec3 pos );

		void generation( void );
		void regeneration( Inventory *inventory, int type, glm::ivec3 pos, bool adding );
		void generate_chunk( std::list<Chunk *> *chunks );
		void sort_sky( glm::vec3 pos, bool vip );
		void sort_water( glm::vec3 pos, bool vip );
	
		bool inPerimeter( int posX, int posY, GLint render_dist );
		int manhattanDist( int posX, int posY );
        void setVisibility( std::list<Chunk *> *visible_chunks, int posX, int posY, GLint render_dist );
		void hide( void );
        bool shouldDelete( glm::vec3 pos, GLfloat dist );
        bool isInChunk( int posX, int posY );

		int isHit( glm::ivec3 pos, bool waterIsBlock );
		void handleHit( Inventory *inventory, int type, glm::ivec3 pos, bool adding, std::list<Chunk *> vis_chunks );
		void updateBreak( glm::ivec4 block_hit, int frame );
		void update_border(int posX, int posY, int level, int type, bool adding);
		bool collisionBox( glm::vec3 pos, float width, float height );
		void applyGravity( Camera *camera );

		int isLoaded( GLint &counter );
        void drawArray( GLint & counter, GLint &block_counter );
		void updateFurnaces( double currentTime );
		bool addFlow( std::set<int> &newFluids, int posX, int posY, int posZ, int level );
		void updateFluids( void );
		void drawSky( GLint & counter, GLint &triangle_counter );
		void drawWater( GLint & counter, GLint &triangle_counter );
		std::string getAddsRmsString( void );
};

#endif
