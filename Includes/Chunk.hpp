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
# include "SignInstance.hpp"
class Player;
class Menu;
# include "Inventory.hpp"
# include "Entity.hpp"
# include "Particle.hpp"
# include "Zombie.hpp"
# include "Skeleton.hpp"
# include "Buffer.hpp"
# include "redstoneSchedule.hpp"

enum cont {
	CONT_MUSHROOM_FIELDS,
	CONT_DEEP_OCEAN,
	CONT_OCEAN,
	CONT_COAST,
	CONT_NEAR_INLAND,
	CONT_MID_INLAND,
	CONT_FAR_INLAND
};

enum Modif {
	rm,
	add,
	replace,
	litnt,
	use,
	popItem,
};

const std::array<glm::ivec3, 6> adj_blocks = {{
	{0, -1, 0}, {0, 1, 0}, {-1, 0, 0}, {1, 0, 0}, {0, 0, -1}, {0, 0, 1}
}};

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

enum COLLISION {
	NONE,
	TOTAL,
	PARTIAL
};

typedef struct s_sign_info t_sign_info;
typedef struct s_hit t_hit;

typedef struct s_collision {
	int type;
	float minZ = 0.0f;
	float maxZ = 0.0f;
}				t_collision;

typedef struct s_shaderInput {
	int texture;
	int light;
	glm::vec3 pos;
}				t_shaderInput;

typedef struct s_shaderInputModel {
	int spec;
	glm::vec3 pos;
}				t_shaderInputModel;

typedef struct s_shaderInputPart {
	int spec;
	glm::vec3 pos;
}				t_shaderInputPart;

typedef struct s_pathfinding_node {
	s_pathfinding_node( int dist, glm::ivec3 position )
		: f_dist(dist), pos(position), parent(NULL) {}
	int f_dist = 0;
	glm::ivec3 pos = {0, 0, 0};
	std::shared_ptr<s_pathfinding_node> parent = NULL;
}				t_pathfinding_node;

struct s_backup { // TODO add fluids and entities and mobs to backups
	std::map<int, int> added;
	std::set<int> removed;
	std::map<int, std::shared_ptr<ChestInstance>> chests;
	std::map<int, std::shared_ptr<FurnaceInstance>> furnaces;
	std::map<int, std::shared_ptr<SignInstance>> signs;
	std::vector<std::shared_ptr<Entity>> entities;
};

class Chunk
{
    public:
        Chunk( Player* player, Inventory* inventory, int posX, int posY, std::list<std::shared_ptr<Chunk>>& chunks );
        ~Chunk( void );

		GLint getStartX( void );
		GLint getStartY( void );
		Chunk* getChunkAt( int startX, int startY );
		Player* getPlayer( void );
		unsigned& getSeed( void );
		bool getSortedOnce( void );
		GLint getCamLightLevel( glm::ivec3 location );
		int computePosLight( glm::vec3 pos );
		short getLightLevel( int posX, int posY, int posZ );
		void setLightLevelAbsolute( short level, glm::ivec3 pos, bool askNeighbours );
		void setLightLevel( short level, int posX, int posY, int posZ, bool askNeighbours );
		void startLightSpread( int posX, int posY, int posZ, bool skySpread );
		void waitGenDone( void );

		int computeLight( int row, int col, int level );
		int computeSmoothLight( int basefaceLight, int row, int col, int level, std::array<int, 9> offsets );
		int computeShade( int row, int col, int level, std::array<int, 9> offsets );

		void setNeighbour( Chunk* neighbour, int index );
		void setBackup( std::map<std::pair<int, int>, s_backup>& backups );
		void restoreBackup( s_backup& backup);
		void openChest( glm::ivec3 pos );
		ChestInstance* getChestInstance( glm::ivec3 pos );
		FurnaceInstance* getFurnaceInstance( glm::ivec3 pos );
		void setSignContent( t_sign_info sign );

		GLint getBlockAtAbsolute( glm::ivec3 pos );
		GLint getBlockAtAbsolute( int posX, int posY, int posZ );
		GLint getBlockAt( glm::ivec3 pos, bool askNeighbours = true );
		GLint getBlockAt( int posX, int posY, int posZ, bool askNeighbours = true );
		void setBlockAtAbsolute( int value, glm::ivec3 pos, bool update );
		void setBlockAtAbsolute( int value, int posX, int posY, int posZ, bool update );
		void setBlockAt( int value, glm::ivec3 pos, bool update, bool observer = true );
		void setBlockAt( int value, int posX, int posY, int posZ, bool update, bool observer = true );
		std::pair<std::vector<glm::ivec3>, int> computePathfinding( glm::ivec3 start, glm::ivec3 end );
		void update_adj_block( glm::ivec3 pos, int dir, int source );
		void turnDirtToGrass( int posX, int posY, int posZ );

		void generation( void );
		void checkFillVertices( void );
		void generate_chunk( void );
		void dropEntities( std::vector<t_item> drops );
		void dropEntity( glm::vec3 dir, t_item item );
		void addParticle( glm::vec3 pos, int type, float shade = 1, int block = blocks::air );
		void sort_sky( glm::vec3& pos, bool vip );
		void sort_water( glm::vec3 pos, bool vip );

		bool lineOfSight( const glm::vec3 src, const glm::vec3 dst );
		bool inPerimeter( int posX, int posY, GLint render_dist );
		int manhattanDist( int posX, int posY );
        bool isInChunk( int posX, int posY );

		int isHit( glm::ivec3 pos );
		bool bookedLectern( Menu* menu, glm::ivec3 pos, bool turnPage );
		AMob* mobHit( const t_hit blockHit );
		void handleHit( bool useInventory, int type, glm::ivec3 pos, Modif modif );
		void handleBlast( glm::vec3 pos, int blast_radius );
		void explosion( glm::vec3 pos, int power );
		void addArrow( ArrowEntity& arrow );
		void shootArrow( const glm::vec3 start, const glm::vec3 dir, float timer );
		void updateBreak( glm::ivec4 block_hit );
		void light_try_spread( int posX, int posY, int posZ, short level, bool skySpread, int recurse );
		bool try_addFlow( std::set<int>* newFluids, int posX, int posY, int posZ, int level );
		void insertFluidAt( std::set<int>* newFluids, int posX, int posY, int posZ );
		t_collision collisionBox( glm::vec3 pos, float width, float height, float originalHeight );
		bool collisionBoxWater( glm::vec3 pos, float width, float height );
		void applyGravity( AMob* mob );

		int isLoaded( GLint& counter );
        void drawArray( GLint& counter, GLint& face_counter );
		void drawSky( GLint& counter, GLint& face_counter );
		void drawWater( GLint& counter, GLint& face_counter );

		void updateRedstone( int priority, schedule::t_redstoneTick red );
		void updatePiston( glm::ivec3 pos, int value );
		void scheduleRedstoneTick( schedule::t_redstoneTick red );
		void abortComparatorScheduleTick( glm::ivec3 pos );
		void updateFurnaces( double currentTime );
		void updateFluids( void );
		void tickUpdate( void );
		void updateScheduledBlocks( void );
		void addMob( const AMob& mob, int mobType );
		void updateMobs( std::vector<t_shaderInputModel>& modArr, double deltaTime );
		void updateEntities( std::vector<t_shaderInput>& arr, std::vector<t_shaderInputPart>& partArr, double deltaTime );
		size_t clearEntities( void );
		void updateParticles( std::vector<t_shaderInput>& entityArr, std::vector<t_shaderInputPart>& partArr, double deltaTime );
		size_t clearParticles( void );

		std::string getAddsRmsString( void );

    private:
		Buffer _vabo, _vaboWater, _vaboSky;
        bool _vaoSet, _waterVaoSet, _waterVaoVIP, _skyVaoSet, _skyVaoVIP;
		GLboolean _hasWater;
		std::atomic_bool _genDone, _light_update, _vertex_update, _vaoReset, _vaoVIP, _waterVaoReset, _skyVaoReset, _sortedOnce;
        GLint _startX, _startY, _nb_neighbours;
		unsigned _seed;
		GLint _continent;
		std::array<GLint, settings::consts::alloc_size> _blocks;
		std::vector<glm::ivec4> _water_vert, _sky_vert;
		std::vector<t_shaderInput> _vertices;
		std::array<short, settings::consts::alloc_size> _lights;  // 0xFF00 sky_light(0xF000 is source value and 0xF00 is actual value), 0xFF block_light(0xF0 source value and 0xF actual value)
		std::array<GLboolean, (settings::consts::chunk_size + 2) * (settings::consts::chunk_size + 2)> _sky;
		std::atomic_size_t _displayed_faces, _water_count, _sky_count;
		std::array<Chunk*, 4> _neighbours;
		Player* _player;
		Inventory* _inventory;
		std::map<int,int> _added;
		std::set<int> _removed, _fluids;
		std::vector<int> _scheduled_to_fall;
		std::map<int, std::shared_ptr<ChestInstance>> _chests;
		std::map<int, std::shared_ptr<FurnaceInstance>> _furnaces;
		std::map<int, std::shared_ptr<SignInstance>> _signs;
		std::vector<std::shared_ptr<Entity>> _entities;
		std::vector<std::shared_ptr<AMob>> _mobs;
		std::vector<std::shared_ptr<Particle>> _particles;
		std::map<int, std::shared_ptr<Particle>> _flames;
		std::thread _thread;
		std::mutex _mtx, _mtx_fluid, _mtx_sky;

		// utils
		void gen_ore_blob( int ore_type, int row, int col, int level, int & blob_size, int dir);
		void collisionWHitbox( t_collision& res, const Block* target, int value, glm::vec3 pos, float width,
			float height, int bX, int bY, int bZ );
		GLint face_count( int type, glm::ivec3 pos );
		GLint face_count( int type, int row, int col, int level );
		bool exposed_block( int row, int col, int level, bool isNotLeaves, bool isNotGlass );

		// fluids
		int exposed_water_faces( int row, int col, int level );
		std::array<int, 4> water_heights( int value, int above, int row, int col, int level );
		bool endFlow( std::set<int>* newFluids, int &value, int posX, int posY, int posZ );
		bool addFlow( std::set<int>* newFluids, int posX, int posY, int posZ, int srcWLevel );

		// world gen
		int get_block_type( siv::PerlinNoise perlin, int row, int col, int level, int surface_level,
			bool poppy, bool dandelion, bool blue_orchid, bool allium, bool cornflower, bool pink_tulip,
			bool grass, bool tree_gen, std::vector<glm::ivec3> & trees );
		int surfaceLevel( int row, int col, siv::PerlinNoise perlin );
		void generate_flat_world( void );
		void generate_blocks( void );
		void resetDisplayedFaces( void );
		void generate_sky( void );

		// player-related block update
		void regeneration( bool useInventory, int type, glm::ivec3 pos, Modif modif );
		void entity_block( int posX, int posY, int posZ, int type );
		void remove_block( bool useInventory, glm::ivec3 pos );
		void handle_stair_corners( glm::ivec3 pos, int& type );
		void handle_door_placement( glm::ivec3 pos, int& type );
		void handle_fence_placement( glm::ivec3 pos, int& type );
		void addFlame( int offset, glm::vec3 pos, int source, int orientation );
		void add_block( bool useInventory, glm::ivec3 pos, int block_value, int previous );
		void replace_block( bool useInventory, glm::ivec3 pos, int type );
		void use_block( bool useInventory, glm::ivec3 pos, int type );
		void update_block( glm::ivec3 pos, int previous, int value );

		// lights
		void light_spread( int posX, int posY, int posZ, bool skySpread, int recurse = settings::consts::light_recurse );
		void generate_lights( void );

		// redstone
		glm::ivec3 getAttachedDir( int value );
		int getWeakdyState( glm::ivec3 pos, glm::ivec3 except );
		int getRedstoneSignalItemFrame( glm::ivec3 pos, glm::ivec3 except );
		int getRedstoneSignalLectern( glm::ivec3 pos );
		int getRedstoneSignalTarget( glm::ivec3 pos, glm::ivec3 target, bool side, bool repeater );
		int getRedstoneStrength( glm::ivec3 pos, glm::ivec3 except, int state, bool weak );
		int getDustStrength( glm::ivec3 pos );
		void stronglyPower( glm::ivec3 pos, glm::ivec3 except, int state );
		void weaklyPowerTarget( glm::ivec3 pos, glm::ivec3 source, bool state, bool weakdy );
		void weaklyPower( glm::ivec3 pos, glm::ivec3 except, bool state, bool weakdy );
		void flickLever( glm::ivec3 pos, int value, bool state );
		void updateRedstoneTorch( glm::ivec3 pos, int value );
		void updateRedstoneDust( glm::ivec3 pos );
		void initRepeater( glm::ivec3 pos, int& value, bool init );
		void updateComparator( glm::ivec3 pos, int value, bool scheduledUpdate );
		void updateItemFrame( glm::ivec3 pos );
		void updateLectern( glm::ivec3 pos );
		int pistonExtendCount( glm::ivec3 pos, int value );
		void extendPiston( glm::ivec3 pos, int value, int count );
		void retractPiston( glm::ivec3 pos, int value );
		void connectRedstoneDust( glm::ivec3 pos, int &value, bool placed );

		// pathfinding
		void checkNodePathfinding( std::list<std::shared_ptr<t_pathfinding_node>>& open_nodes,
			std::list<std::shared_ptr<t_pathfinding_node>>& visited_nodes, glm::ivec3 pos, bool* trav,
			std::shared_ptr<t_pathfinding_node> current, glm::ivec3 start, glm::ivec3 end );

		// block update (20/sec)
		void updateCrop( int value, int offset );
		bool watered_farmland( int posX, int posY, int posZ );
		void updateFarmland( int value, int offset );
		void spreadGrassblock( int offset );
		bool findLog( int posX, int posY, int posZ, int level );
		void decayLeaves( int offset );
		bool spaceForTree( int posX, int posY, int posZ );
		void growTree( int value, int offset );

		// array buffer
        void fill_vertex_array( void );
        void setup_array_buffer( void );
		void setup_sky_array_buffer( void );
		void setup_water_array_buffer( void );
};

#endif
