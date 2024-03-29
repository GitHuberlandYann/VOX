#ifndef CHUNK_HPP
# define CHUNK_HPP

# include <set>
# include <map>
# include <atomic>
# include <thread>
# include <mutex>
# include "utils.h"
# include "PerlinNoise.hpp"
# include "ChestInstance.hpp"
# include "FurnaceInstance.hpp"
class Camera;
# include "Inventory.hpp"
# include "Entity.hpp"
# include "Particle.hpp"

# define SEA_LEVEL 64
// # define CHUNK_SIZE 16 in utils
# define LIGHT_RECURSE 32
# define CHUNK_SHIFT 4
# define WORLD_HEIGHT 256
# define WORLD_SHIFT 8
# define ALLOC_SIZE 1 << 16 // represents CHUNK_SIZE * CHUNK_SIZE * WORLD_HEIGHT

# define XTEX 1 + (1 << 17) + (1 << 16)
# define YTEX (1 << 4) + (1 << 18) + (1 << 12)

enum cont {
	CONT_MUSHROOM_FIELDS,
	CONT_DEEP_OCEAN,
	CONT_OCEAN,
	CONT_COAST,
	CONT_NEAR_INLAND,
	CONT_MID_INLAND,
	CONT_FAR_INLAND
};

enum {
	SPECATTRIB,
	POSATTRIB
};

enum Modif {
	REMOVE,
	ADD,
	REPLACE,
	LITNT
};

const GLint adj_blocks[6][3] = {
	{0, -1, 0}, {0, 1, 0}, {-1, 0, 0}, {1, 0, 0}, {0, 0, -1}, {0, 0, 1}
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

struct s_backup { // TODO add fluids and entities to backups
	std::map<int, int> added;
	std::set<int> removed;
	std::map<int, ChestInstance*> chests;
	std::map<int, FurnaceInstance*> furnaces;
};

class Chunk
{
    private:
        GLuint _vao, _vbo, _waterVao, _waterVbo, _skyVao, _skyVbo;
        bool _isVisible, _vaoSet, _waterVaoSet, _waterVaoVIP, _skyVaoSet, _skyVaoVIP;
		std::atomic_bool _genDone, _light_update, _vertex_update, _vaoReset, _vaoVIP, _waterVaoReset, _skyVaoReset, _sortedOnce;
        GLint _startX, _startY, _nb_neighbours;
		unsigned _seed;
		GLint _continent;
		GLint *_blocks, *_water_vert, *_sky_vert;
		void *_vertices; // int, vec3
		short *_lights; // 0xFF00 sky_light(0xF000 is source value and 0xF00 is actual value), 0xFF block_light(0xF0 source value and 0xF actual value)
		GLboolean *_sky, _hasWater;
		std::atomic_size_t _displayed_faces, _displayed_alloc, _water_count, _sky_count;
		std::array<Chunk *, 4> _neighbours;
		Camera *_camera;
		Inventory *_inventory;
		std::map<int,int> _added;
		std::set<int> _removed, _fluids;
		std::vector<int> _scheduled_to_fall;
		std::map<int, ChestInstance*> _chests;
		std::map<int, FurnaceInstance*> _furnaces;
		std::vector<Entity*> _entities;
		std::vector<Particle*> _particles;
		std::map<int, Particle*> _flames;
		std::thread _thread;
		std::mutex _mtx, _mtx_fluid, _mtx_sky;

		void gen_ore_blob( int ore_type, int row, int col, int level, int & blob_size, int dir);
		GLint face_count( int type, int row, int col, int level );
		bool exposed_block( int row, int col, int level, bool isNotLeaves, bool isNotGlass );
	
		int exposed_water_faces( int row, int col, int level );
		std::array<int, 4> water_heights( int value, int above, int row, int col, int level );
		bool endFlow( std::set<int> *newFluids, int &value, int posX, int posY, int posZ );
		bool addFlow( std::set<int> *newFluids, int posX, int posY, int posZ, int level );
	
		// int get_block_type_cave( int row, int col, int level, int ground_level,
		// 	bool poppy, bool dandelion, bool blue_orchid, bool allium, bool cornflower, bool pink_tulip,
		// 	bool grass, bool tree_gen, std::vector<glm::ivec3> & trees );
		int get_block_type( siv::PerlinNoise perlin, int row, int col, int level, int surface_level,
			bool poppy, bool dandelion, bool blue_orchid, bool allium, bool cornflower, bool pink_tulip,
			bool grass, bool tree_gen, std::vector<glm::ivec3> & trees );
		int surfaceLevel( int row, int col, siv::PerlinNoise perlin );
		void generate_blocks( void );
		void resetDisplayedFaces( void );
		void generate_sky( void );
	
		void handle_border_block( glm::ivec3 pos, int type, bool adding );
		void entity_block( int posX, int posY, int posZ, int type );
		void remove_block( bool useInventory, glm::ivec3 pos );
		void addFlame( int offset, glm::vec3 pos, int source, int orientation );
		void add_block( bool useInventory, glm::ivec3 pos, int type, int previous );
		void replace_block( bool useInventory, glm::ivec3 pos, int type, int previous );

		void light_spread( int posX, int posY, int level, bool skySpread, int recurse = LIGHT_RECURSE );
		void generate_lights( void );
		int computeLight( int row, int col, int level );
		int computeSmoothLight( int basefaceLight, int row, int col, int level, std::array<int, 9> offsets );
		int computeShade( int row, int col, int level, std::array<int, 9> offsets );

		void updateCrop( int value, int offset );
		bool watered_farmland( int posX, int posY, int posZ );
		void updateFarmland( int value, int offset );
		void spreadGrassblock( int offset );
		bool findLog( int posX, int posY, int posZ, int level );
		void decayLeaves( int offset );
		bool spaceForTree( int posX, int posY, int posZ );
		void growTree( int value, int offset );
	
        void fill_vertex_array( void );
        void setup_array_buffer( void );
		void setup_sky_array_buffer( void );
		void setup_water_array_buffer( void );

    public:
        Chunk( Camera *camera, Inventory *inventory, int posX, int posY, std::list<Chunk *> *chunks );
        ~Chunk( void );

		GLint getStartX( void );
		GLint getStartY( void );
		unsigned getSeed( void );
		bool getSortedOnce( void );
		GLint getCamLightLevel( glm::ivec3 location );
		int computePosLight( glm::vec3 pos );
		short getLightLevel( int posX, int posY, int posZ );
		void waitGenDone( void );

		void setNeighbour( Chunk *neighbour, face_dir index );
		void setBackup( std::map<std::pair<int, int>, s_backup> &backups );
		void restoreBackup( s_backup &backup);
		void openChest( glm::ivec3 pos );
		ChestInstance *getChestInstance( glm::ivec3 pos );
		FurnaceInstance *getFurnaceInstance( glm::ivec3 pos );
		GLint getBlockAt( int posX, int posY, int posZ, bool askNeighbours );
		void turnDirtToGrass( int posX, int posY, int posZ );

		void generation( void );
		void checkFillVertices( void );
		void regeneration( bool useInventory, int type, glm::ivec3 pos, Modif modif );
		void generate_chunk( void );
		void dropEntity( glm::vec3 dir, t_item item );
		void addParticle( Particle *particle );
		void sort_sky( glm::vec3 &pos, bool vip );
		void sort_water( glm::vec3 pos, bool vip );
	
		bool inPerimeter( int posX, int posY, GLint render_dist );
		int manhattanDist( int posX, int posY );
        void show( void );
		void hide( void );
        bool isInChunk( int posX, int posY );

		int isHit( glm::ivec3 pos );
		void handleHit( bool useInventory, int type, glm::ivec3 pos, Modif modif );
		void handleBlast( glm::vec3 pos, int blast_radius );
		void explosion( glm::vec3 pos, int power );
		void shootArrow( float timer );
		void updateBreak( glm::ivec4 block_hit, int frame );
		void light_try_spread( int posX, int posY, int posZ, short level, bool skySpread, int recurse );
		bool try_addFlow( std::set<int> *newFluids, int posX, int posY, int posZ, int level );
		void insertFluidAt( std::set<int> *newFluids, int posX, int posY, int posZ );
		void update_border( int posX, int posY, int level, int type, bool adding, int origin );
		bool collisionBox( glm::vec3 pos, float width, float height );
		bool collisionBoxWater( glm::vec3 pos, float width, float height );
		void applyGravity( void );

		int isLoaded( GLint &counter );
        void drawArray( GLint & counter, GLint &face_counter );
		void updateFurnaces( double currentTime );
		void updateFluids( void );
		void tickUpdate( void );
		void updateScheduledBlocks( void );
		void updateEntities( std::vector<std::pair<int, glm::vec3>> &arr, std::vector<std::pair<int, glm::vec3>> &partArr, double deltaTime );
		size_t clearEntities( void );
		void updateParticles( std::vector<std::pair<int, glm::vec3>> &arr, double deltaTime );
		size_t clearParticles( void );
		void drawSky( GLint & counter, GLint &face_counter );
		void drawWater( GLint & counter, GLint &face_counter );
		std::string getAddsRmsString( void );
};

#endif
