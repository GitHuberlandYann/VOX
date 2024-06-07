#ifndef BLOCKS_HPP
# define BLOCKS_HPP

# define GLEW_STATIC
# include <GL/glew.h> // must be before glfw
# include "GLFW/glfw3.h"
# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>
# include <string>
# include <vector>

class Chunk;
class UI;
typedef struct s_shaderInput t_shaderInput;

const float ONE16TH = 0.0625f;

# define S_BLOCKS_SIZE 128

enum face_dir {
	MINUSY,
	PLUSY,
	MINUSX,
	PLUSX,
	MINUSZ,
	PLUSZ
};

enum CORNERS {
	MM = 0b0001, // -x-y
	MP = 0b0010, // -x+y
	PM = 0b0100, // +x-y
	PP = 0b1000, // +x+y
};

enum FENCE {
	MY = 0b0001, // -y
	PY = 0b0010, // +y
	MX = 0b0100, // -x
	PX = 0b1000, // +x
	BASE,
	ARM,
	ARM_END,
};

enum DOOR {
	UPPER_HALF  = 0b001,
	RIGHT_HINGE = 0b010,
	OPEN        = 0b100
};

enum PLACEMENT {
	WALL,
	FLOOR,
	CEILING
};

namespace SAPLING {
	const int GROWTH = (1 << 24);
};

namespace LEAVES {
	const int NATURAL = (1 << 24);
};

namespace FARMLAND {
	const int WET = (1 << 24);
	const int MOISTURE_OFFSET = 25;
	const int MOISTURE = (0x7 << MOISTURE_OFFSET);
};

namespace REDSTONE {
	const bool ON = true;
	const bool OFF = false;
	const int TICK = 2;

	// activated means it receives signal, powered means it also transmits signal
	const int POWERED_OFFSET = 16;
	const int POWERED = (1 << POWERED_OFFSET);
	const int WEAKDY_POWERED_OFFSET = 17;
	const int WEAKDY_POWERED = (1 << WEAKDY_POWERED_OFFSET);
	const int ACTIVATED_OFFSET = 18;
	const int ACTIVATED = (1 << ACTIVATED_OFFSET);
	const int STRENGTH_OFFSET = 20;
	const int STRENGTH = (0xF << STRENGTH_OFFSET);

	namespace DUST {
		const int CONNECT = 0x3;
		const int NONE = 0;
		const int SIDE = 1;
		const int UP   = 2;
		const int MY = 24;
		const int PY = 26;
		const int MX = 28;
		const int PX = 30;
	};

	namespace REPEATER {
		const int TICKS_OFFSET = 24;
		const int LOCK = (1 << 27);
	}

	namespace COMPARATOR {
		const int MODE = (1 << 24);
		const int COMPARE = 0;
		const int SUBSTRACT = 1;
	};

	namespace PISTON {
		const int STICKY_OFFSET = 24;
		const int STICKY = (1 << STICKY_OFFSET);
		const int MOVING = (1 << 24);
		const int RETRACTING = (1 << 28); // flag given to blocks::MOVING_PISTON to set their transparency -> transparent when retracting, solid when extending
		const int FORCE_RETRACTION = 13; // used by Chunk::pistonExtendCount
		const int CANCEL_RETRACTION = 42; // random nbr above 12, used by Chunk::pistonExtendCount
	};

	const int ALL_BITS = -1;
};

namespace GAMEMODE {
	const int ADVENTURE_BLOCK = (1 << 19);
};

namespace GEOMETRY {
	enum {
		NONE,          // air, water
		CUBE,          // default blocks
		CROSS,         // flowers and such
		FARMLAND,      // cube with z = 15 * ONE16TH
		TORCH,
		SLAB_BOTTOM,
		SLAB_TOP,
		STAIRS_BOTTOM,
		STAIRS_TOP,
		FENCE,
		GLASS,
		GLASS_PANE,
		DOOR,
		TRAPDOOR,
		CROP,
		LEVER,
		DUST,
		REPEATER,
		BUTTON,
		PISTON,
	};
};

namespace blocks {
	enum {
		AIR,
		GRASS_BLOCK,
		OAK_LOG,
		CACTUS,
		FARMLAND,
		DIRT_PATH,
		TNT,
		TARGET,
		CRAFTING_TABLE = 8,
		FURNACE,
		OAK_STAIRS_BOTTOM,
		OAK_STAIRS_TOP,
		OAK_DOOR,
		OAK_TRAPDOOR,
		STONE_STAIRS_BOTTOM,
		STONE_STAIRS_TOP,
		SMOOTH_STONE_STAIRS_BOTTOM,
		SMOOTH_STONE_STAIRS_TOP,
		COBBLESTONE_STAIRS_BOTTOM,
		COBBLESTONE_STAIRS_TOP,
		STONE_BRICKS_STAIRS_BOTTOM,
		STONE_BRICKS_STAIRS_TOP,
		LEVER,
		OAK_SIGN,
		BEDROCK = 24,
		DIRT,
		SMOOTH_STONE,
		STONE,
		COBBLESTONE,
		STONE_BRICKS,
		CRACKED_STONE_BRICKS,
		SAND,
		GRAVEL,
		OAK_LEAVES,
		OAK_PLANKS,
		GLASS,
		GLASS_PANE,
		REDSTONE_LAMP,
		STONE_BUTTON,
		OAK_BUTTON,
		COAL_ORE = 40,
		IRON_ORE,
		DIAMOND_ORE,
		COAL_BLOCK,
		IRON_BLOCK,
		DIAMOND_BLOCK,
		REDSTONE_ORE,
		REDSTONE_BLOCK,
		OAK_SLAB_BOTTOM = 48,
		OAK_SLAB_TOP,
		OAK_FENCE,
		STONE_SLAB_BOTTOM,
		STONE_SLAB_TOP,
		SMOOTH_STONE_SLAB_BOTTOM,
		SMOOTH_STONE_SLAB_TOP,
		COBBLESTONE_SLAB_BOTTOM,
		COBBLESTONE_SLAB_TOP,
		STONE_BRICKS_SLAB_BOTTOM,
		STONE_BRICKS_SLAB_TOP,
		PISTON,
		STICKY_PISTON,
		PISTON_HEAD,
		MOVING_PISTON,
		OBSERVER,
		POPPY = 64,
		DANDELION,
		BLUE_ORCHID,
		ALLIUM,
		CORNFLOWER,
		PINK_TULIP,
		GRASS,
		SUGAR_CANE,
		DEAD_BUSH,
		OAK_SAPLING,
		TORCH,
		REDSTONE_TORCH,
		REDSTONE_DUST,
		REPEATER,
		COMPARATOR,
		CHEST,
		WHEAT_CROP = 80,
		WATER = 88,
		WATER1,
		WATER2,
		WATER3,
		WATER4,
		WATER5,
		WATER6,
		WATER7,
		STICK = 96,
		WOODEN_SHOVEL,
		STONE_SHOVEL,
		IRON_SHOVEL,
		DIAMOND_SHOVEL,
		WOODEN_AXE,
		STONE_AXE,
		IRON_AXE,
		DIAMOND_AXE,
		WOODEN_PICKAXE,
		STONE_PICKAXE,
		IRON_PICKAXE,
		DIAMOND_PICKAXE,
		BOW,
		ARROW,
		WORLDEDIT_WAND,
		COAL = 112,
		CHARCOAL,
		IRON_INGOT,
		DIAMOND,
		BUCKET,
		WATER_BUCKET,
		WOODEN_HOE,
		STONE_HOE,
		IRON_HOE,
		DIAMOND_HOE,
		WHEAT_SEEDS,
		WHEAT,
		BREAD,
		APPLE,
		FLINT,
		FLINT_AND_STEEL,
	};
	const int TYPE = 0xFFF;
	const int NOTVISIBLE = (1 << 12);
	const int ORIENTATION_OFFSET = 13;
	const int ORIENTATION_MASK = 0x7;
	const int ORIENTATION = (ORIENTATION_MASK << ORIENTATION_OFFSET);
	const int BITFIELD_OFFSET = 24;
	const int ITEM = 1942;
}

struct Block {
	public:
		std::string name = "DEFAULT";
		int mined = blocks::AIR;
		int adventure_block = blocks::BEDROCK;
		bool stackable = true;
		char light_level = 0;
		bool isFuel = false;
		float fuel_time = 0;
		bool isComposant = false;
		int getProduction = blocks::AIR;
		float blast_resistance = FLT_MAX;
		bool hasHitbox = false;
		bool hasOrientedHitbox = false;
		bool collisionHitbox_1x1x1 = true;
		bool collisionHitbox = false;
		bool oriented = false;
		bool orientedCollisionHitbox = false;
		glm::vec3 hitboxCenter = {0, 0, 0};
		glm::vec3 hitboxHalfSize = {0, 0, 0};
		int geometry = GEOMETRY::CUBE;
		bool isFood = false;
		int hunger_restauration = 0;
		float saturation_restauration = 0;
		bool byHand = false;
		int needed_tool = blocks::NOTVISIBLE;
		int needed_material_level = 0;
		int durability = 0;
		float hardness = -1;
		bool transparent = false;
		bool redstoneComponant = false;
		bool item3D = true;
		int textureX = 1;
		int textureY = 15;

		bool canCollect( int tool ) const {
			if (byHand) {
				return (true);
			}
			return (tool >= needed_tool + needed_material_level && tool < needed_tool + 4);
		}
		virtual int texX( face_dir dir = face_dir::MINUSY, int value = 0 ) const {
			(void)dir;(void)value;
			return (textureX);
		}
		virtual int texY( face_dir dir = face_dir::MINUSY, int value = 0 ) const {
			(void)dir;(void)value;
			return (textureY);
		}
		virtual void getSecondaryHitbox( glm::vec3 *hitbox, int orientation, int corners ) const {
			(void)hitbox;(void)orientation;(void)corners;
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const {
			(void)chunk;
			(void)vertices;
			(void)start;
			(void)pos;
			(void)value;
		}
		virtual void addItem( UI *ui, int x, int y, int gui_size, int width, bool alien, bool movement ) const;
		virtual bool isTransparent( int value ) const {
			(void)value;
			return (transparent);
		}
		virtual ~Block() {}
};

struct Cube : Block {
	public:
		Cube() {
			geometry = GEOMETRY::CUBE;
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
		virtual void addItem( UI *ui, int x, int y, int gui_size, int width, bool alien, bool movement ) const;
};

struct Cross : Block {
	public:
		Cross() {
			blast_resistance = 0.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			hitboxCenter = {0.5f, 0.5f, 0.3f};
			hitboxHalfSize = {0.2f, 0.2f, 0.3f};
			geometry = GEOMETRY::CROSS;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			item3D = false;
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
};

struct SlabBottom : Block {
	public:
		SlabBottom() {
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, 0.25f};
			hitboxHalfSize = {0.5f, 0.5f, 0.25f};
			geometry = GEOMETRY::SLAB_BOTTOM;
			transparent = true;
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
		virtual void addItem( UI *ui, int x, int y, int gui_size, int width, bool alien, bool movement ) const;
};

struct SlabTop : Block {
	public:
		SlabTop() {
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, 0.75f};
			hitboxHalfSize = {0.5f, 0.5f, 0.25f};
			geometry = GEOMETRY::SLAB_TOP;
			transparent = true;
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
};

struct StairsBottom : Block {
	public:
		StairsBottom() {
			hasHitbox = true;
			hasOrientedHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			oriented = true;
			orientedCollisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, 0.25f};
			hitboxHalfSize = {0.5f, 0.5f, 0.25f};
			geometry = GEOMETRY::STAIRS_BOTTOM;
			transparent = true;
		}
		virtual void getSecondaryHitbox( glm::vec3 *hitbox, int orientation, int corners ) const {
			switch (corners) {
				case CORNERS::MM | CORNERS::MP:
					hitbox[0] = {0.25f, 0.5f, 0.75f}; // hitboxCenter
					hitbox[1] = {0.25f, 0.5f, 0.25f}; // hitboxHalfSize
					break ;
				case CORNERS::PM | CORNERS::PP:
					hitbox[0] = {0.75f, 0.5f, 0.75f};
					hitbox[1] = {0.25f, 0.5f, 0.25f};
					break ;
				case CORNERS::MM | CORNERS::PM:
					hitbox[0] = {0.5f, 0.25f, 0.75f};
					hitbox[1] = {0.5f, 0.25f, 0.25f};
					break ;
				case CORNERS::MP | CORNERS::PP:
					hitbox[0] = {0.5f, 0.75f, 0.75f};
					hitbox[1] = {0.5f, 0.25f, 0.25f};
					break ;
				case CORNERS::MM:
					hitbox[0] = {0.25f, 0.25f, 0.75f};
					hitbox[1] = {0.25f, 0.25f, 0.25f};
					break ;
				case CORNERS::MP:
					hitbox[0] = {0.25f, 0.75f, 0.75f};
					hitbox[1] = {0.25f, 0.25f, 0.25f};
					break ;
				case CORNERS::PM:
					hitbox[0] = {0.75f, 0.25f, 0.75f};
					hitbox[1] = {0.25f, 0.25f, 0.25f};
					break ;
				case CORNERS::PP:
					hitbox[0] = {0.75f, 0.75f, 0.75f};
					hitbox[1] = {0.25f, 0.25f, 0.25f};
					break ;
				case CORNERS::MM | CORNERS::MP | CORNERS::PM:
					if (orientation == face_dir::PLUSX) {
						hitbox[0] = {0.25f, 0.5f, 0.75f};
						hitbox[1] = {0.25f, 0.5f, 0.25f};
					} else {
						hitbox[0] = {0.5f, 0.25f, 0.75f};
						hitbox[1] = {0.5f, 0.25f, 0.25f};
					}
					break ;
				case CORNERS::MM | CORNERS::MP | CORNERS::PP:
					if (orientation == face_dir::PLUSX) {
						hitbox[0] = {0.25f, 0.5f, 0.75f};
						hitbox[1] = {0.25f, 0.5f, 0.25f};
					} else {
						hitbox[0] = {0.5f, 0.75f, 0.75f};
						hitbox[1] = {0.5f, 0.25f, 0.25f};
					}
					break ;
				case CORNERS::PM | CORNERS::PP | CORNERS::MM:
					if (orientation == face_dir::MINUSX) {
						hitbox[0] = {0.75f, 0.5f, 0.75f};
						hitbox[1] = {0.25f, 0.5f, 0.25f};
					} else {
						hitbox[0] = {0.5f, 0.25f, 0.75f};
						hitbox[1] = {0.5f, 0.25f, 0.25f};
					}
					break ;
				case CORNERS::PM | CORNERS::PP | CORNERS::MP:
					if (orientation == face_dir::MINUSX) {
						hitbox[0] = {0.75f, 0.5f, 0.75f};
						hitbox[1] = {0.25f, 0.5f, 0.25f};
					} else {
						hitbox[0] = {0.5f, 0.75f, 0.75f};
						hitbox[1] = {0.5f, 0.25f, 0.25f};
					}
					break ;
			}
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
		virtual void addItem( UI *ui, int x, int y, int gui_size, int width, bool alien, bool movement ) const;
};

struct StairsTop : Block {
	public:
		StairsTop() {
			hasHitbox = true;
			hasOrientedHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			oriented = true;
			orientedCollisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, 0.75f};
			hitboxHalfSize = {0.5f, 0.5f, 0.25f};
			geometry = GEOMETRY::STAIRS_TOP;
			transparent = true;
		}
		virtual void getSecondaryHitbox( glm::vec3 *hitbox, int orientation, int corners ) const {
			switch (corners) {
				case CORNERS::MM | CORNERS::MP:
					hitbox[0] = {0.25f, 0.5f, 0.25f}; // hitboxCenter
					hitbox[1] = {0.25f, 0.5f, 0.25f}; // hitboxHalfSize
					break ;
				case CORNERS::PM | CORNERS::PP:
					hitbox[0] = {0.75f, 0.5f, 0.25f};
					hitbox[1] = {0.25f, 0.5f, 0.25f};
					break ;
				case CORNERS::MM | CORNERS::PM:
					hitbox[0] = {0.5f, 0.25f, 0.25f};
					hitbox[1] = {0.5f, 0.25f, 0.25f};
					break ;
				case CORNERS::MP | CORNERS::PP:
					hitbox[0] = {0.5f, 0.75f, 0.25f};
					hitbox[1] = {0.5f, 0.25f, 0.25f};
					break ;
				case CORNERS::MM:
					hitbox[0] = {0.25f, 0.25f, 0.25f};
					hitbox[1] = {0.25f, 0.25f, 0.25f};
					break ;
				case CORNERS::MP:
					hitbox[0] = {0.25f, 0.75f, 0.25f};
					hitbox[1] = {0.25f, 0.25f, 0.25f};
					break ;
				case CORNERS::PM:
					hitbox[0] = {0.75f, 0.25f, 0.25f};
					hitbox[1] = {0.25f, 0.25f, 0.25f};
					break ;
				case CORNERS::PP:
					hitbox[0] = {0.75f, 0.75f, 0.25f};
					hitbox[1] = {0.25f, 0.25f, 0.25f};
					break ;
				case CORNERS::MM | CORNERS::MP | CORNERS::PM:
					if (orientation == face_dir::PLUSX) {
						hitbox[0] = {0.25f, 0.5f, 0.25f};
						hitbox[1] = {0.25f, 0.5f, 0.25f};
					} else {
						hitbox[0] = {0.5f, 0.25f, 0.25f};
						hitbox[1] = {0.5f, 0.25f, 0.25f};
					}
					break ;
				case CORNERS::MM | CORNERS::MP | CORNERS::PP:
					if (orientation == face_dir::PLUSX) {
						hitbox[0] = {0.25f, 0.5f, 0.25f};
						hitbox[1] = {0.25f, 0.5f, 0.25f};
					} else {
						hitbox[0] = {0.5f, 0.75f, 0.25f};
						hitbox[1] = {0.5f, 0.25f, 0.25f};
					}
					break ;
				case CORNERS::PM | CORNERS::PP | CORNERS::MM:
					if (orientation == face_dir::MINUSX) {
						hitbox[0] = {0.75f, 0.5f, 0.25f};
						hitbox[1] = {0.25f, 0.5f, 0.25f};
					} else {
						hitbox[0] = {0.5f, 0.25f, 0.25f};
						hitbox[1] = {0.5f, 0.25f, 0.25f};
					}
					break ;
				case CORNERS::PM | CORNERS::PP | CORNERS::MP:
					if (orientation == face_dir::MINUSX) {
						hitbox[0] = {0.75f, 0.5f, 0.25f};
						hitbox[1] = {0.25f, 0.5f, 0.25f};
					} else {
						hitbox[0] = {0.5f, 0.75f, 0.25f};
						hitbox[1] = {0.5f, 0.25f, 0.25f};
					}
					break ;
			}
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
};

struct Fence : Block {
	public:
		Fence() {
			hasHitbox = true;
			hasOrientedHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			orientedCollisionHitbox = true;
			hitboxCenter = {0, 0, 100000}; // we discard normal hitbox
			geometry = GEOMETRY::FENCE;
			transparent = true;
		}
		virtual void getSecondaryHitbox( glm::vec3 *hitbox, int orientation, int bitfield ) const {
			(void)orientation;
			hitbox[0] = {0.5f, 0.5f, 0.5f};
			hitbox[1] = {0.125f, 0.125f, 0.5f};
			switch (bitfield & (FENCE::MX | FENCE::PX)) {
				case FENCE::MX:
					hitbox[0].x = 0.375f;
					hitbox[1].x = 0.375f;
					break ;
				case FENCE::PX:
					hitbox[0].x = 1 - 0.375f;
					hitbox[1].x = 0.375f;
					break ;
				case FENCE::MX | FENCE::PX:
					hitbox[1].x = 0.5f;
					break ;
			}
			switch (bitfield & (FENCE::MY | FENCE::PY)) {
				case FENCE::MY:
					hitbox[0].y = 0.375f;
					hitbox[1].y = 0.375f;
					break ;
				case FENCE::PY:
					hitbox[0].y = 1 - 0.375f;
					hitbox[1].y = 0.375f;
					break ;
				case FENCE::MY | FENCE::PY:
					hitbox[1].y = 0.5f;
					break ;
			}
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
		virtual void addItem( UI *ui, int x, int y, int gui_size, int width, bool alien, bool movement ) const;
};

struct Door : Block {
	public:
		Door() {
			hasHitbox = true;
			hasOrientedHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			oriented = true;
			orientedCollisionHitbox = true;
			hitboxCenter = {0, 0, 100000}; // we discard normal hitbox
			geometry = GEOMETRY::DOOR;
			transparent = true;
			redstoneComponant = true;
			item3D = false;
		}
		virtual void getSecondaryHitbox( glm::vec3 *hitbox, int orientation, int bitfield ) const {
			bool open = !!(bitfield & DOOR::OPEN) ^ !!((bitfield << 12) & REDSTONE::POWERED);
			switch (orientation) {
				case face_dir::MINUSX:
					if (!open) {
						hitbox[0] = {0.90625f, 0.5f, 0.5f};
						hitbox[1] = {0.09375f, 0.5f, 0.5f};
					} else {
						hitbox[0] = {0.5f, (bitfield & DOOR::RIGHT_HINGE) ? 0.09375f : 0.90625f, 0.5f};
						hitbox[1] = {0.5f, 0.09375f, 0.5f};
					}
					break ;
				case face_dir::PLUSX:
					if (!open) {
						hitbox[0] = {0.09375f, 0.5f, 0.5f};
						hitbox[1] = {0.09375f, 0.5f, 0.5f};
					} else {
						hitbox[0] = {0.5f, (bitfield & DOOR::RIGHT_HINGE) ? 0.90625f : 0.09375f, 0.5f};
						hitbox[1] = {0.5f, 0.09375f, 0.5f};
					}
					break ;
				case face_dir::MINUSY:
					if (!open) {
						hitbox[0] = {0.5f, 0.90625f, 0.5f};
						hitbox[1] = {0.5f, 0.09375f, 0.5f};
					} else {
						hitbox[0] = {(bitfield & DOOR::RIGHT_HINGE) ? 0.90625f : 0.09375f, 0.5f, 0.5f};
						hitbox[1] = {0.09375f, 0.5f, 0.5f};
					}
					break ;
				case face_dir::PLUSY:
					if (!open) {
						hitbox[0] = {0.5f, 0.09375f, 0.5f};
						hitbox[1] = {0.5f, 0.09375f, 0.5f};
					} else {
						hitbox[0] = {(bitfield & DOOR::RIGHT_HINGE) ? 0.09375f : 0.90625f, 0.5f, 0.5f};
						hitbox[1] = {0.09375f, 0.5f, 0.5f};
					}
					break ;
			}
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
};

struct Trapdoor : Block {
	public:
		Trapdoor() {
			hasHitbox = true;
			hasOrientedHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			oriented = true;
			orientedCollisionHitbox = true;
			hitboxCenter = {0, 0, 100000}; // we discard normal hitbox
			geometry = GEOMETRY::TRAPDOOR;
			transparent = true;
			redstoneComponant = true;
		}
		virtual void getSecondaryHitbox( glm::vec3 *hitbox, int orientation, int bitfield ) const {
			bool open = !!(bitfield & DOOR::OPEN) ^ !!((bitfield << 12) & REDSTONE::POWERED);
			if (!open) {
				hitbox[0] = {0.5f, 0.5f, (bitfield & DOOR::UPPER_HALF) ? 0.90625f : 0.09375f};
				hitbox[1] = {0.5f, 0.5f, 0.09375f};
			} else {
				switch (orientation) {
					case face_dir::MINUSX:
						hitbox[0] = {0.90625f, 0.5f, 0.5f};
						hitbox[1] = {0.09375f, 0.5f, 0.5f};
						break ;
					case face_dir::PLUSX:
						hitbox[0] = {0.09375f, 0.5f, 0.5f};
						hitbox[1] = {0.09375f, 0.5f, 0.5f};
						break ;
					case face_dir::MINUSY:
						hitbox[0] = {0.5f, 0.90625f, 0.5f};
						hitbox[1] = {0.5f, 0.09375f, 0.5f};
						break ;
					case face_dir::PLUSY:
						hitbox[0] = {0.5f, 0.09375f, 0.5f};
						hitbox[1] = {0.5f, 0.09375f, 0.5f};
						break ;
				}
			}
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
		virtual void addItem( UI *ui, int x, int y, int gui_size, int width, bool alien, bool movement ) const;
};

struct Crop : Block {
	public:
		Crop() {
			blast_resistance = 0.0f;
			hasHitbox = true;
			hasOrientedHitbox = true;
			collisionHitbox_1x1x1 = false;
			hitboxCenter = {0, 0, 100000}; // we discard normal hitbox
			geometry = GEOMETRY::CROP;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			item3D = false;
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
};

struct Button : Block {
	public:
		Button() {
			collisionHitbox_1x1x1 = false;
			hasHitbox = true;
			hasOrientedHitbox = true;
			collisionHitbox = true;
			oriented = true;
			orientedCollisionHitbox = true;
			hitboxCenter = {0, 0, 100000}; // we discard normal hitbox
			geometry = GEOMETRY::BUTTON;
			transparent = true;
			item3D = true;
		}
		virtual void getSecondaryHitbox( glm::vec3 *hitbox, int orientation, int bitfield ) const {
			hitbox[0] = {0.5f, 0.5f, 1.5f * ONE16TH};
			hitbox[1] = {0,    0,    1.5f * ONE16TH};
			switch (bitfield & 0x3) {
				case PLACEMENT::WALL:
					switch (orientation) {
						case face_dir::MINUSX:
							hitbox[0] = {14.5f * ONE16TH, 0.5f,           0.5f};
							hitbox[1] = { 1.5f * ONE16TH, 3.0f * ONE16TH, 4.0f * ONE16TH};
							break ;
						case face_dir::PLUSX:
							hitbox[0] = { 1.5f * ONE16TH, 0.5f,           0.5f};
							hitbox[1] = { 1.5f * ONE16TH, 3.0f * ONE16TH, 4.0f * ONE16TH};
							break ;
						case face_dir::MINUSY:
							hitbox[0] = {0.5f,           14.5f * ONE16TH, 0.5f};
							hitbox[1] = {3.0f * ONE16TH,  1.5f * ONE16TH, 4.0f * ONE16TH};
							break ;
						case face_dir::PLUSY:
							hitbox[0] = {0.5f,           1.5f * ONE16TH, 0.5f};
							hitbox[1] = {3.0f * ONE16TH, 1.5f * ONE16TH, 4.0f * ONE16TH};
							break ;
					}
					break ;
				case PLACEMENT::FLOOR:
				case PLACEMENT::CEILING:
					hitbox[0].z = ((bitfield & 0x3) == PLACEMENT::FLOOR) ? 1.5f * ONE16TH : 14.5f * ONE16TH;
					switch (orientation) {
						case face_dir::MINUSX:
						case face_dir::PLUSX:
							hitbox[1].x = 4.0f * ONE16TH;
							hitbox[1].y = 3.0F * ONE16TH;
							break ;
						case face_dir::MINUSY:
						case face_dir::PLUSY:
							hitbox[1].x = 3.0f * ONE16TH;
							hitbox[1].y = 4.0f * ONE16TH;
							break ;
					}
					break ;
			}
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
		virtual void addItem( UI *ui, int x, int y, int gui_size, int width, bool alien, bool movement ) const;
};


// ************************************************************************** //
//                          actual structures                                 //
// ************************************************************************** //

struct Air : Block {
	public:
		Air() {
			name = "AIR";
			blast_resistance = 0.0f;
			collisionHitbox_1x1x1 = false;
			geometry = GEOMETRY::NONE;
			transparent = true;
		}
};

struct TBD : Cube {
	public:
		TBD() {
			name = "TBD";
			textureX = 15;
			textureY = 15;
		}
};

struct GrassBlock : Cube {
	public:
		GrassBlock() {
			name = "GRASS_BLOCK";
			mined = blocks::DIRT;
			blast_resistance = 0.6f;
			byHand = true;
			needed_tool = blocks::WOODEN_SHOVEL;
			hardness = 0.6f;
			textureY = 1;
		}
		virtual int texX( face_dir dir, int value ) const {
			(void)value;
			if (dir == face_dir::PLUSZ) {
				return (1);
			} else if (dir == face_dir::MINUSZ) {
				return (4);
			}
			return (0);
		}
};

struct OakLog : Cube {
	public:
		OakLog() {
			name = "OAK_LOG";
			mined = blocks::OAK_LOG;
			isFuel = true;
			fuel_time = 15;
			isComposant = true;
			getProduction = blocks::CHARCOAL;
			blast_resistance = 2.0f;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 2.0f;
			textureY = 2;
		}
		virtual int texX( face_dir dir, int value ) const {
			int axis = (value >> blocks::ORIENTATION_OFFSET) & 0x7;
			switch (dir) {
				case face_dir::MINUSZ:
				case face_dir::PLUSZ:
					return ((axis == face_dir::PLUSZ) + 2 * (axis == face_dir::PLUSX));
				case face_dir::MINUSX:
				case face_dir::PLUSX:
					return ((axis == face_dir::PLUSX) + 2 * (axis == face_dir::PLUSY));
				case face_dir::MINUSY:
				case face_dir::PLUSY:
					return ((axis == face_dir::PLUSY) + 2 * (axis == face_dir::PLUSX));
			}
			return (0);
		}
		virtual void addItem( UI *ui, int x, int y, int gui_size, int width, bool alien, bool movement ) const;
};

struct Cactus : Cube {
	public:
		Cactus() {
			name = "CACTUS";
			mined = blocks::CACTUS;
			blast_resistance = 0.4f;
			collisionHitbox_1x1x1 = false;
			byHand = true;
			hardness = 0.4f;
			transparent = true;
			textureY = 3;
		}
		virtual int texX( face_dir dir, int value ) const {
			(void)value;
			if (dir == face_dir::PLUSZ) {
				return (1);
			} else if (dir == face_dir::MINUSZ) {
				return (2);
			}
			return (0);
		}
};

struct Farmland : Block {
	public:
		Farmland() {
			name = "FARMLAND";
			mined = blocks::DIRT;
			blast_resistance = 0.6f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, 7.5f / 16.0f};
			hitboxHalfSize = {0.5f, 0.5f, 7.5f / 16.0f};
			geometry = GEOMETRY::FARMLAND;
			byHand = true;
			needed_tool = blocks::WOODEN_SHOVEL;
			hardness = 0.6f;
			textureY = 1;
		}
		virtual int texX( face_dir dir, int value ) const {
			if (dir == face_dir::PLUSZ) {
				return (2 + !(value & FARMLAND::WET));
			}
			return (4);
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
};

struct DirtPath : Farmland {
	public:
		DirtPath() {
			name = "DIRT PATH";
			mined = blocks::DIRT;
			blast_resistance = 0.65f;
			hardness = 0.65f;
		}
		virtual int texX( face_dir dir, int value ) const {
			(void)value;
			if (dir == face_dir::PLUSZ) {
				return (1);
			} else if (dir == face_dir::MINUSZ) {
				return (4);
			}
			return (0);
		}
		virtual int texY(  face_dir dir, int value ) const {
			(void)value;
			if (dir == face_dir::MINUSZ) {
				return (1);
			}
			return (5);
		}
};

struct TNT : Cube {
	public:
		TNT() {
			name = "TNT";
			mined = blocks::TNT;
			blast_resistance = 0.0f;
			byHand = true;
			hardness = 0.0f;
			redstoneComponant = true;
			textureY = 6;
		}
		virtual int texX( face_dir dir, int value ) const {
			(void)value;
			if (dir == face_dir::PLUSZ) {
				return (1);
			} else if (dir == face_dir::MINUSZ) {
				return (2);
			}
			return (0);
		}
};

struct Target : Cube {
	public:
		Target() {
			name = "TARGET";
			mined = blocks::TARGET;
			blast_resistance = 0.5f;
			byHand = true;
			needed_tool = blocks::WOODEN_HOE;
			hardness = 0.5f;
			textureY = 7;
		}
		virtual int texX( face_dir dir, int value ) const {
			(void)value;
			return (dir == face_dir::PLUSZ || dir == face_dir::MINUSZ);
		}
};

struct CraftingTable : Cube {
	public:
		CraftingTable() {
			name = "CRAFTING_TABLE";
			mined = blocks::CRAFTING_TABLE;
			adventure_block = blocks::COAL_ORE;
			isFuel = true;
			fuel_time = 15;
			blast_resistance = 2.5f;
			oriented = true;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 2.5f;
			textureY = 8;
		}
		virtual int texX( face_dir dir, int value ) const {
			if (dir == face_dir::PLUSZ || dir == face_dir::MINUSZ) {
				return (1);
			} else if (((value >> blocks::ORIENTATION_OFFSET) & 0x7) == dir) {
				return (2);
			}
			return (0);
		}
};

struct Furnace : Cube {
	public:
		Furnace() {
			name = "FURNACE";
			mined = blocks::FURNACE;
			adventure_block = blocks::COAL_ORE;
			blast_resistance = 3.5f;
			oriented = true;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 3.5f;
			textureY = 9;
		}
		virtual int texX( face_dir dir, int value ) const {
			if (dir == face_dir::PLUSZ || dir == face_dir::MINUSZ) {
				return (1);
			} else if (((value >> blocks::ORIENTATION_OFFSET) & 0x7) == dir) {
				return (2 + ((value >> REDSTONE::ACTIVATED_OFFSET) & 0x1));
			}
			return (0);
		}
};

struct OakStairsBottom : StairsBottom {
	public:
		OakStairsBottom() {
			name = "OAK_STAIRS_BOTTOM";
			mined = blocks::OAK_STAIRS_BOTTOM;
			blast_resistance = 3.0f;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 2.0f;
			textureX = 4;
			textureY = 10;
		}
};

struct OakStairsTop : StairsTop {
	public:
		OakStairsTop() {
			name = "OAK_STAIRS_TOP";
			mined = blocks::OAK_STAIRS_BOTTOM;
			blast_resistance = 3.0f;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 2.0f;
			textureX = 4;
			textureY = 10;
		}
};

struct OakDoor : Door {
	public:
		OakDoor() {
			name = "OAK_DOOR";
			mined = blocks::OAK_DOOR;
			blast_resistance = 3.0f;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 3.0f;
		}
		// offset is bool half or 2 if item
		virtual int texX( face_dir dir, int value ) const {
			(void)dir;
			return ((value == blocks::ITEM) ? 9 : 0);
		}
		virtual int texY( face_dir dir, int value ) const {
			switch (dir) {
				case face_dir::PLUSZ:
					return (10);
				case face_dir::MINUSZ:
					return (11);
				default:
					return ((value == blocks::ITEM) ? 14 : 11 - ((value >> blocks::BITFIELD_OFFSET) & DOOR::UPPER_HALF));
			}
		}
};

struct OakTrapdoor : Trapdoor {
	public:
		OakTrapdoor() {
			name = "OAK_TRAPDOOR";
			mined = blocks::OAK_TRAPDOOR;
			blast_resistance = 3.0f;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 3.0f;
			textureX = 2;
			textureY = 11;
		}
};

struct StoneStairsBottom : StairsBottom {
	public:
		StoneStairsBottom() {
			name = "STONE_STAIRS_BOTTOM";
			mined = blocks::STONE_STAIRS_BOTTOM;
			isComposant = true;
			getProduction = blocks::SMOOTH_STONE_STAIRS_BOTTOM;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 1.5f;
			textureX = 4;
			textureY = 3;
		}
};

struct StoneStairsTop : StairsTop {
	public:
		StoneStairsTop() {
			name = "STONE_STAIRS_TOP";
			mined = blocks::STONE_STAIRS_BOTTOM;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 1.5f;
			textureX = 4;
			textureY = 3;
		}
};

struct SmoothStoneStairsBottom : StairsBottom {
	public:
		SmoothStoneStairsBottom() {
			name = "SMOOTH_STONE_STAIRS_BOTTOM";
			mined = blocks::SMOOTH_STONE_STAIRS_BOTTOM;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 1.5f;
			textureX = 4;
			textureY = 2;
		}
};

struct SmoothStoneStairsTop : StairsTop {
	public:
		SmoothStoneStairsTop() {
			name = "SMOOTH_STONE_STAIRS_TOP";
			mined = blocks::SMOOTH_STONE_STAIRS_BOTTOM;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 1.5f;
			textureX = 4;
			textureY = 2;
		}
};

struct CobbleStoneStairsBottom : StairsBottom {
	public:
		CobbleStoneStairsBottom() {
			name = "COBBLESTONE_STAIRS_BOTTOM";
			mined = blocks::COBBLESTONE_STAIRS_BOTTOM;
			isComposant = true;
			getProduction = blocks::STONE_STAIRS_BOTTOM;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 1.5f;
			textureX = 4;
			textureY = 4;
		}
};

struct CobbleStoneStairsTop : StairsTop {
	public:
		CobbleStoneStairsTop() {
			name = "COBBLESTONE_STAIRS_TOP";
			mined = blocks::COBBLESTONE_STAIRS_BOTTOM;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 1.5f;
			textureX = 4;
			textureY = 4;
		}
};

struct StoneBricksStairsBottom : StairsBottom {
	public:
		StoneBricksStairsBottom() {
			name = "STONE_BRICKS_STAIRS_BOTTOM";
			mined = blocks::STONE_BRICKS_STAIRS_BOTTOM;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 1.5f;
			textureX = 4;
			textureY = 5;
		}
};

struct StoneBricksStairsTop : StairsTop {
	public:
		StoneBricksStairsTop() {
			name = "STONE_BRICKS_STAIRS_TOP";
			mined = blocks::STONE_BRICKS_STAIRS_BOTTOM;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 1.5f;
			textureX = 4;
			textureY = 5;
		}
};

struct Lever : Block {
	public:
		Lever() {
			name = "LEVER";
			mined = blocks::LEVER;
			adventure_block = blocks::REDSTONE_ORE;
			blast_resistance = 0.5f;
			collisionHitbox_1x1x1 = false;
			hasHitbox = true;
			hasOrientedHitbox = true;
			collisionHitbox = true;
			oriented = true;
			orientedCollisionHitbox = true;
			hitboxCenter = {0, 0, 100000}; // we discard normal hitbox
			geometry = GEOMETRY::LEVER;
			byHand = true;
			hardness = 0.5f;
			transparent = true;
			item3D = false;
			textureX = 7;
			textureY = 10;
		}
		virtual void getSecondaryHitbox( glm::vec3 *hitbox, int orientation, int bitfield ) const {
			hitbox[0] = {0.5f, 0.5f, 1.5f * ONE16TH};
			hitbox[1] = {0,    0,    1.5f * ONE16TH};
			switch (bitfield & 0x3) {
				case PLACEMENT::WALL:
					switch (orientation) {
						case face_dir::MINUSX:
							hitbox[0] = {14.5f * ONE16TH, 0.5f,           0.5f};
							hitbox[1] = { 1.5f * ONE16TH, 3.0f * ONE16TH, 4.0f * ONE16TH};
							break ;
						case face_dir::PLUSX:
							hitbox[0] = { 1.5f * ONE16TH, 0.5f,           0.5f};
							hitbox[1] = { 1.5f * ONE16TH, 3.0f * ONE16TH, 4.0f * ONE16TH};
							break ;
						case face_dir::MINUSY:
							hitbox[0] = {0.5f,           14.5f * ONE16TH, 0.5f};
							hitbox[1] = {3.0f * ONE16TH,  1.5f * ONE16TH, 4.0f * ONE16TH};
							break ;
						case face_dir::PLUSY:
							hitbox[0] = {0.5f,           1.5f * ONE16TH, 0.5f};
							hitbox[1] = {3.0f * ONE16TH, 1.5f * ONE16TH, 4.0f * ONE16TH};
							break ;
					}
					break ;
				case PLACEMENT::FLOOR:
				case PLACEMENT::CEILING:
					hitbox[0].z = ((bitfield & 0x3) == PLACEMENT::FLOOR) ? 1.5f * ONE16TH : 14.5f * ONE16TH;
					switch (orientation) {
						case face_dir::MINUSX:
						case face_dir::PLUSX:
							hitbox[1].x = 4.0f * ONE16TH;
							hitbox[1].y = 3.0F * ONE16TH;
							break ;
						case face_dir::MINUSY:
						case face_dir::PLUSY:
							hitbox[1].x = 3.0f * ONE16TH;
							hitbox[1].y = 4.0f * ONE16TH;
							break ;
					}
					break ;
			}
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
};

struct OakSign : Block {
	public:
		OakSign() {
			name = "OAK_SIGN";
			mined = blocks::OAK_SIGN;
			blast_resistance = 1.0f;
			collisionHitbox_1x1x1 = false;
			hasHitbox = true;
			hasOrientedHitbox = true;
			hitboxCenter = {0, 0, 100000}; // we discard normal hitbox
			geometry = GEOMETRY::NONE;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 1.0f;
			transparent = true;
			item3D = false;
			textureX = 1;
			textureY = 11;
		}
		virtual void getSecondaryHitbox( glm::vec3 *hitbox, int orientation, int bitfield ) const {
			(void)bitfield;
			switch (orientation) {
				case face_dir::MINUSX:
					hitbox[0] = {1.75f * ONE16TH, 0.5f, 0.5f};
					hitbox[1] = {1.75f * ONE16TH, 0.5f, 0.25f};
					break ;
				case face_dir::PLUSX:
					hitbox[0] = {14.25f * ONE16TH, 0.5f, 0.5f};
					hitbox[1] = {1.75f * ONE16TH, 0.5f, 0.25f};
					break ;
				case face_dir::MINUSY:
					hitbox[0] = {0.5f, 1.75f * ONE16TH, 0.5f};
					hitbox[1] = {0.5f, 1.75f * ONE16TH, 0.25f};
					break ;
				case face_dir::PLUSY:
					hitbox[0] = {0.5f, 14.25f * ONE16TH, 0.5f};
					hitbox[1] = {0.5f, 1.75f * ONE16TH, 0.25f};
					break ;
			}
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
};

struct Bedrock : Cube {
	public:
		Bedrock() {
			name = "BEDROCK";
			byHand = false;
			textureX = 4;
			textureY = 0;
		}
};

struct Dirt : Cube {
	public:
		Dirt() {
			name = "DIRT";
			mined = blocks::DIRT;
			blast_resistance = 0.5f;
			byHand = true;
			needed_tool = blocks::WOODEN_SHOVEL;
			hardness = 0.5f;
			textureX = 4;
			textureY = 1;
		}
};

struct SmoothStone : Cube {
	public:
		SmoothStone() {
			name = "SMOOTH_STONE";
			mined = blocks::SMOOTH_STONE;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 2.0f;
			textureX = 4;
			textureY = 2;
		}
};

struct Stone : Cube {
	public:
		Stone() {
			name = "STONE";
			mined = blocks::COBBLESTONE;
			isComposant = true;
			getProduction = blocks::SMOOTH_STONE;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 1.5f;
			textureX = 4;
			textureY = 3;
		}
};

struct Cobblestone : Cube {
	public:
		Cobblestone() {
			name = "COBBLESTONE";
			mined = blocks::COBBLESTONE;
			isComposant = true;
			getProduction = blocks::STONE;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 2.0f;
			textureX = 4;
			textureY = 4;
		}
};

struct StoneBrick : Cube {
	public:
		StoneBrick() {
			name = "STONE_BRICK";
			mined = blocks::STONE_BRICKS;
			isComposant = true;
			getProduction = blocks::CRACKED_STONE_BRICKS;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 1.5f;
			textureX = 4;
			textureY = 5;
		}
};

struct CrackedStoneBrick : Cube {
	public:
		CrackedStoneBrick() {
			name = "CRACKED_STONE_BRICK";
			mined = blocks::CRACKED_STONE_BRICKS;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 1.5f;
			textureX = 4;
			textureY = 6;
		}
};

struct Sand : Cube {
	public:
		Sand() {
			name = "SAND";
			mined = blocks::SAND;
			isComposant = true;
			getProduction = blocks::GLASS;
			blast_resistance = 0.5f;
			byHand = true;
			needed_tool = blocks::WOODEN_SHOVEL;
			hardness = 0.5f;
			textureX = 4;
			textureY = 7;
		}
};

struct Gravel : Cube {
	public:
		Gravel() {
			name = "GRAVEL";
			mined = blocks::GRAVEL;
			blast_resistance = 0.6f;
			byHand = true;
			needed_tool = blocks::WOODEN_SHOVEL;
			hardness = 0.6f;
			textureX = 4;
			textureY = 8;
		}
};

struct OakLeaves : Cube {
	public:
		OakLeaves() {
			name = "OAK_LEAVES";
			blast_resistance = 0.2f;
			byHand = true;
			hardness = 0.2f;
			transparent = true;
			textureX = 4;
			textureY = 9;
		}
};

struct OakPlanks : Cube {
	public:
		OakPlanks() {
			name = "OAK_PLANKS";
			mined = blocks::OAK_PLANKS;
			isFuel = true;
			fuel_time = 15;
			blast_resistance = 3.0f;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 2.0f;
			textureX = 4;
			textureY = 10;
		}
};

struct Glass : Block {
	public:
		Glass() {
			name = "GLASS";
			blast_resistance = 0.3f;
			hardness = 0.3f;
			geometry = GEOMETRY::GLASS;
			transparent = true;
			textureX = 4;
			textureY = 11;
		}
		virtual void addItem( UI *ui, int x, int y, int gui_size, int width, bool alien, bool movement ) const;
};

struct GlassPane : Block {
	public:
		GlassPane() {
			name = "GLASS_PANE";
			blast_resistance = 0.3f;
			hasHitbox = true;
			hasOrientedHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			orientedCollisionHitbox = true;
			hitboxCenter = {0, 0, 100000}; // we discard normal hitbox
			geometry = GEOMETRY::GLASS_PANE;
			hardness = 0.3f;
			transparent = true;
			item3D = false;
			textureY = 11;
		}
		virtual int texX( face_dir dir, int offset ) const {
			(void)dir;
			switch (offset) {
				case FENCE::BASE:
					return (4);
				case FENCE::ARM:
					return (5);
				case FENCE::ARM_END:
					return (3);
			}
			return (4);
		}
		virtual void getSecondaryHitbox( glm::vec3 *hitbox, int orientation, int bitfield ) const {
			(void)orientation;
			hitbox[0] = {0.5f, 0.5f, 0.5f};
			hitbox[1] = {0.0625, 0.0625, 0.5f};
			switch (bitfield & (FENCE::MX | FENCE::PX)) {
				case FENCE::MX:
					hitbox[0].x = 0.28125f;
					hitbox[1].x = 0.28125f;
					break ;
				case FENCE::PX:
					hitbox[0].x = 1 - 0.28125f;
					hitbox[1].x = 0.28125f;
					break ;
				case FENCE::MX | FENCE::PX:
					hitbox[1].x = 0.5f;
					break ;
			}
			switch (bitfield & (FENCE::MY | FENCE::PY)) {
				case FENCE::MY:
					hitbox[0].y = 0.28125f;
					hitbox[1].y = 0.28125f;
					break ;
				case FENCE::PY:
					hitbox[0].y = 1 - 0.28125f;
					hitbox[1].y = 0.28125f;
					break ;
				case FENCE::MY | FENCE::PY:
					hitbox[1].y = 0.5f;
					break ;
			}
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
};

struct RedstoneLamp : Cube {
	public:
		RedstoneLamp() {
			name = "REDSTONE_LAMP";
			mined = blocks::REDSTONE_LAMP;
			light_level = 15;
			isFuel = true;
			fuel_time = 15;
			blast_resistance = 0.3f;
			byHand = true;
			hardness = 0.3f;
			redstoneComponant = true;
			textureX = 5;
		}
		virtual int texY( face_dir dir, int value ) const {
			(void)dir;
			return ((value & REDSTONE::ACTIVATED) ? 9 : 8);
		}
};

struct StoneButton : Button {
	public:
		StoneButton() {
			name = "STONE_BUTTON";
			adventure_block = blocks::REDSTONE_ORE;
			mined = blocks::STONE_BUTTON;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 1.5f;
			textureX = 4;
			textureY = 3;
		}
};

struct OakButton : Button {
	public:
		OakButton() {
			name = "OAK_BUTTON";
			mined = blocks::OAK_BUTTON;
			adventure_block = blocks::REDSTONE_ORE;
			blast_resistance = 0.5f;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 0.5f;
			textureX = 4;
			textureY = 10;
		}
};

struct CoalOre : Cube {
	public:
		CoalOre() {
			name = "COAL_ORE";
			mined = blocks::COAL;
			isComposant = true;
			getProduction = blocks::COAL;
			blast_resistance = 3.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 3.0f;
			textureX = 5;
			textureY = 0;
		}
};

struct IronOre : Cube {
	public:
		IronOre() {
			name = "IRON_ORE";
			mined = blocks::IRON_ORE;
			isComposant = true;
			getProduction = blocks::IRON_INGOT;
			blast_resistance = 3.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			needed_material_level = 1; // min stone to collect
			hardness = 3.0f;
			textureX = 5;
			textureY = 1;
		}
};

struct DiamondOre : Cube {
	public:
		DiamondOre() {
			name = "DIAMOND_ORE";
			mined = blocks::DIAMOND;
			isComposant = true;
			getProduction = blocks::DIAMOND;
			blast_resistance = 3.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			needed_material_level = 2; // min iron to collect
			hardness = 3.0f;
			textureX = 5;
			textureY = 2;
		}
};

struct CoalBlock : Cube {
	public:
		CoalBlock() {
			name = "COAL_BLOCK";
			mined = blocks::COAL_BLOCK;
			isFuel = true;
			fuel_time = 800;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 5.0f;
			textureX = 5;
			textureY = 3;
		}
};

struct IronBlock : Cube {
	public:
		IronBlock() {
			name = "IRON_BLOCK";
			mined = blocks::IRON_BLOCK;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			needed_material_level = 1; // min stone to collect
			hardness = 5.0f;
			textureX = 5;
			textureY = 4;
		}
};

struct DiamondBlock : Cube {
	public:
		DiamondBlock() {
			name = "DIAMOND_BLOCK";
			mined = blocks::DIAMOND_BLOCK;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			needed_material_level = 2; // min iron to collect
			hardness = 5.0f;
			textureX = 5;
			textureY = 5;
		}
};

struct RedstoneOre : Cube {
	public:
		RedstoneOre() {
			name = "REDSTONE_ORE";
			mined = blocks::REDSTONE_DUST; // TODO 4 or 5
			isComposant = true;
			getProduction = blocks::REDSTONE_DUST;
			blast_resistance = 3.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			needed_material_level = 2; // min iron to collect
			hardness = 3.0f;
			textureX = 5;
			textureY = 6;
		}
};

struct RedstoneBlock : Cube {
	public:
		RedstoneBlock() {
			name = "REDSTONE_BLOCK";
			mined = blocks::REDSTONE_BLOCK;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 5.0f;
			textureX = 5;
			textureY = 7;
		}
};

struct OakSlabBottom : SlabBottom {
	public:
		OakSlabBottom() {
			name = "OAK_SLAB_BOTTOM";
			mined = blocks::OAK_SLAB_BOTTOM;
			blast_resistance = 3.0f;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 2.0f;
			textureX = 4;
			textureY = 10;
		}
};

struct OakSlabTop : SlabTop {
	public:
		OakSlabTop() {
			name = "OAK_SLAB_TOP";
			mined = blocks::OAK_SLAB_BOTTOM;
			blast_resistance = 3.0f;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 2.0f;
			textureX = 4;
			textureY = 10;
		}
};

struct OakFence : Fence {
	public:
		OakFence() {
			name = "OAK_FENCE";
			mined = blocks::OAK_FENCE;
			blast_resistance = 3.0f;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 2.0f;
			textureX = 4;
			textureY = 10;
		}
};

struct StoneSlabBottom : SlabBottom {
	public:
		StoneSlabBottom() {
			name = "STONE_SLAB_BOTTOM";
			mined = blocks::STONE_SLAB_BOTTOM;
			isComposant = true;
			getProduction = blocks::SMOOTH_STONE_SLAB_BOTTOM;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 1.5f;
			textureX = 4;
			textureY = 3;
		}
};

struct StoneSlabTop : SlabTop {
	public:
		StoneSlabTop() {
			name = "STONE_SLAB_TOP";
			mined = blocks::STONE_SLAB_BOTTOM;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 2.0f;
			textureX = 4;
			textureY = 3;
		}
};

struct SmoothStoneSlabBottom : SlabBottom {
	public:
		SmoothStoneSlabBottom() {
			name = "SMOOTH_STONE_SLAB_BOTTOM";
			mined = blocks::SMOOTH_STONE_SLAB_BOTTOM;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 1.5f;
			textureX = 4;
			textureY = 2;
		}
};

struct SmoothStoneSlabTop : SlabTop {
	public:
		SmoothStoneSlabTop() {
			name = "SMOOTH_STONE_SLAB_TOP";
			mined = blocks::SMOOTH_STONE_SLAB_BOTTOM;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 2.0f;
			textureX = 4;
			textureY = 2;
		}
};

struct CobbleStoneSlabBottom : SlabBottom {
	public:
		CobbleStoneSlabBottom() {
			name = "COBBLESTONE_SLAB_BOTTOM";
			mined = blocks::COBBLESTONE_SLAB_BOTTOM;
			isComposant = true;
			getProduction = blocks::STONE_SLAB_BOTTOM;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 1.5f;
			textureX = 4;
			textureY = 4;
		}
};

struct CobbleStoneSlabTop : SlabTop {
	public:
		CobbleStoneSlabTop() {
			name = "COBBLESTONE_SLAB_TOP";
			mined = blocks::COBBLESTONE_SLAB_BOTTOM;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 2.0f;
			textureX = 4;
			textureY = 4;
		}
};

struct StoneBricksSlabBottom : SlabBottom {
	public:
		StoneBricksSlabBottom() {
			name = "STONE_BRICKS_SLAB_BOTTOM";
			mined = blocks::STONE_BRICKS_SLAB_BOTTOM;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 1.5f;
			textureX = 4;
			textureY = 5;
		}
};

struct StoneBricksSlabTop : SlabTop {
	public:
		StoneBricksSlabTop() {
			name = "STONE_BRICKS_SLAB_TOP";
			mined = blocks::STONE_BRICKS_SLAB_BOTTOM;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 2.0f;
			textureX = 4;
			textureY = 5;
		}
};
face_dir opposite_dir( int dir );
struct Piston : Block {
	public:
		Piston() {
			name = "PISTON";
			mined = blocks::PISTON;
			blast_resistance = 0.5f;
			geometry = GEOMETRY::PISTON;
			byHand = true;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 1.5f;
			transparent = true;
			oriented = true;
			textureY = 13;
		}
		virtual int texX( face_dir dir = face_dir::MINUSY, int value = 0 ) const {
			int orientation = (value >> blocks::ORIENTATION_OFFSET) & 0x7;
			if (dir == orientation) {
				return ((value & REDSTONE::PISTON::MOVING) ? 2 : 3 + (mined == blocks::STICKY_PISTON));
			}
			if (dir == opposite_dir(orientation)) {
				return (1);
			}
			return (0);
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
		virtual void addItem( UI *ui, int x, int y, int gui_size, int width, bool alien, bool movement ) const;
};

struct StickyPiston : Piston {
	public:
		StickyPiston() {
			name = "STICKY_PISTON";
			mined = blocks::STICKY_PISTON;
		}
};

struct PistonHead : Block {
	public:
		PistonHead() {
			name = "PISTON_HEAD";
			blast_resistance = 0.5f;
			geometry = GEOMETRY::CROSS;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 1.5f;
			transparent = true;
			oriented = true;
			textureX = 4;
			textureY = 5;
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
};

struct MovingPiston : Block {
	public:
		MovingPiston() {
			name = "MOVING_PISTON";
			geometry = GEOMETRY::PISTON;
		}
		virtual bool isTransparent( int value ) const {
			return (value & REDSTONE::PISTON::RETRACTING);
		}
};

struct Observer : Block {
	public:
		Observer() {
			name = "OBSERVER";
			mined = blocks::OBSERVER;
			blast_resistance = 3.0f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 3.0f;
			transparent = true;
			oriented = true;
			textureY = 12;
		}
		virtual int texX( face_dir dir = face_dir::MINUSY, int value = 0 ) const {
			int orientation = (value >> blocks::ORIENTATION_OFFSET) & 0x7;
			if (dir == orientation) {
				return (2);
			}
			if (dir == opposite_dir(orientation)) {
				return ((value & REDSTONE::ACTIVATED) ? 4 : 3);
			}
			if (orientation & 0x4) {
				return (!(dir & 0x2));
			}
			return (!!(dir & 0x4));
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
		virtual void addItem( UI *ui, int x, int y, int gui_size, int width, bool alien, bool movement ) const;
};

struct Poppy : Cross {
	public:
		Poppy() {
			name = "POPPY";
			mined = blocks::POPPY;
			textureX = 6;
			textureY = 0;
		}
};

struct Dandelion : Cross {
	public:
		Dandelion() {
			name = "DANDELION";
			mined = blocks::DANDELION;
			textureX = 6;
			textureY = 1;
		}
};

struct BlueOrchid : Cross {
	public:
		BlueOrchid() {
			name = "BLUE_ORCHID";
			mined = blocks::BLUE_ORCHID;
			textureX = 6;
			textureY = 2;
		}
};

struct Allium : Cross {
	public:
		Allium() {
			name = "ALLIUM";
			mined = blocks::ALLIUM;
			textureX = 6;
			textureY = 3;
		}
};

struct CornFlower : Cross {
	public:
		CornFlower() {
			name = "CORNFLOWER";
			mined = blocks::CORNFLOWER;
			textureX = 6;
			textureY = 4;
		}
};

struct PinkTulip : Cross {
	public:
		PinkTulip() {
			name = "PINK_TULIP";
			mined = blocks::PINK_TULIP;
			textureX = 6;
			textureY = 5;
		}
};

struct Grass : Cross {
	public:
		Grass() {
			name = "GRASS";
			mined = blocks::WHEAT_SEEDS;
			hasHitbox = false;
			textureX = 6;
			textureY = 6;
		}
};

struct SugarCane : Cross {
	public:
		SugarCane() {
			name = "SUGAR_CANE";
			mined = blocks::SUGAR_CANE;
			hasHitbox = false;
			textureX = 6;
			textureY = 7;
		}
};

struct DeadBush : Cross {
	public:
		DeadBush() {
			name = "DEAD_BUSH";
			mined = blocks::STICK;
			isFuel = true;
			fuel_time = 5;
			textureX = 6;
			textureY = 8;
		}
};

struct OakSapling : Cross {
	public:
		OakSapling() {
			name = "OAK_SAPLING";
			mined = blocks::OAK_SAPLING;
			isFuel = true;
			fuel_time = 5;
			textureX = 6;
			textureY = 9;
		}
};

struct Torch : Block {
	public:
		Torch() {
			name = "TORCH";
			mined = blocks::TORCH;
			blast_resistance = 0.0f;
			hasHitbox = true;
			hasOrientedHitbox = true;
			collisionHitbox_1x1x1 = false;
			geometry = GEOMETRY::TORCH;
			light_level = 14;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			item3D = false;
			textureX = 6;
			textureY = 10;
		}
		virtual void getSecondaryHitbox( glm::vec3 *hitbox, int orientation, int bitfield ) const {
			(void)bitfield;
			switch (orientation) {
				case face_dir::MINUSX:
					hitbox[0] = {2 * ONE16TH, 0.5f, 8 * ONE16TH};
					hitbox[1] = {2 * ONE16TH, ONE16TH, 5 * ONE16TH};
					break ;
				case face_dir::PLUSX:
					hitbox[0] = {14 * ONE16TH, 0.5f, 8 * ONE16TH};
					hitbox[1] = {2 * ONE16TH, ONE16TH, 5 * ONE16TH};
					break ;
				case face_dir::MINUSY:
					hitbox[0] = {0.5f, 2 * ONE16TH, 8 * ONE16TH};
					hitbox[1] = {ONE16TH, 2 * ONE16TH, 5 * ONE16TH};
					break ;
				case face_dir::PLUSY:
					hitbox[0] = {0.5f, 14 * ONE16TH, 8 * ONE16TH};
					hitbox[1] = {ONE16TH, 2 * ONE16TH, 5 * ONE16TH};
					break ;
				default:
					hitbox[0] = {0.5f, 0.5f, 5 * ONE16TH};
					hitbox[1] = {ONE16TH, ONE16TH, 5 * ONE16TH};
					break ;
			}
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
};

struct RedstoneTorch : Torch {
	public:
		RedstoneTorch() {
			name = "REDSTONE_TORCH";
			mined = blocks::REDSTONE_TORCH;
			light_level = 7;
			textureX = 7;
			textureY = 11;
		}
		virtual int texY( face_dir dir = face_dir::MINUSY, int offset = 0 ) const {
			(void)dir;
			return ((offset == blocks::ITEM) ? 11 : 11 + offset);
		}
};

struct RedstoneDust : Block {
	public:
		RedstoneDust() {
			name = "REDSTONE_DUST";
			mined = blocks::REDSTONE_DUST;
			blast_resistance = 0.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			hitboxCenter = {0.5f, 0.5f, ONE16TH};
			hitboxHalfSize = {0.2f, 0.2f, ONE16TH};
			geometry = GEOMETRY::DUST;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			item3D = false;
		}
		virtual int texX( face_dir dir = face_dir::MINUSY, int value = 0 ) const {
			(void)dir;
			return ((value == blocks::ITEM) ? 7 : 6);
		}
		virtual int texY( face_dir dir = face_dir::MINUSY, int value = 0 ) const {
			(void)dir;
			return ((value == blocks::ITEM) ? 8 : 12);
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
};

struct Repeater : Block {
	public:
		Repeater() {
			name = "REPEATER";
			mined = blocks::REPEATER;
			adventure_block = blocks::IRON_ORE;
			blast_resistance = 0.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, ONE16TH};
			hitboxHalfSize = {0.5f, 0.5f, ONE16TH};
			oriented = true;
			geometry = GEOMETRY::REPEATER;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			item3D = false;
			textureY = 13;
		}
		virtual int texX( face_dir dir = face_dir::MINUSY, int value = 0 ) const {
			(void)dir;
			return ((value == blocks::ITEM) ? 7 : 6 - value);
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
};

struct Comparator : Block {
	public:
		Comparator() {
			name = "COMPARATOR";
			mined = blocks::COMPARATOR;
			adventure_block = blocks::IRON_ORE;
			blast_resistance = 0.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, ONE16TH};
			hitboxHalfSize = {0.5f, 0.5f, ONE16TH};
			oriented = true;
			geometry = GEOMETRY::REPEATER;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			item3D = false;
			textureY = 14;
		}
		virtual int texX( face_dir dir = face_dir::MINUSY, int value = 0 ) const {
			(void)dir;
			return ((value == blocks::ITEM) ? 7 : 6 - value);
		}
		virtual void addMesh( Chunk *chunk, std::vector<t_shaderInput> &vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
};

struct Chest : Block {
	public:
		Chest() {
			name = "CHEST";
			mined = blocks::CHEST;
			blast_resistance = 2.5f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, 7.5 / 16.0f};
			hitboxHalfSize = {7 / 16.0f, 7 / 16.0f, 7.5 / 16.0f};
			geometry = GEOMETRY::NONE;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 2.5f;
			transparent = true;
			textureX = 0;
			textureY = 15;
		}
		virtual void addItem( UI *ui, int x, int y, int gui_size, int width, bool alien, bool movement ) const;
};

struct WheatCrop : Crop {
	public:
		WheatCrop() {
			name = "WHEAT_CROP";
			mined = blocks::WHEAT_SEEDS;
			textureX = 7;
		}
		virtual int texY( face_dir dir = face_dir::MINUSY, int value = 0 ) const {
			(void)dir;
			return (value >> blocks::BITFIELD_OFFSET);
		}
		virtual void getSecondaryHitbox( glm::vec3 *hitbox, int orientation, int bitfield ) const {
			(void)orientation;
			hitbox[0] = {0.5f, 0.5f, (1 + bitfield * 2) / 32.0f};
			hitbox[1] = {0.4f, 0.4f, (1 + bitfield * 2) / 32.0f};
		}
};

struct Water : Block {
	public:
		Water() {
			name = "WATER";
			mined = blocks::WATER_BUCKET;
			blast_resistance = 100.0f;
			collisionHitbox_1x1x1 = false;
			geometry = GEOMETRY::NONE;
			transparent = true;
		}
};

struct Water1 : Block {
	public:
		Water1() {
			name = "WATER 1";
			blast_resistance = 100.0f;
			collisionHitbox_1x1x1 = false;
			geometry = GEOMETRY::NONE;
			transparent = true;
		}
};

struct Water2 : Block {
	public:
		Water2() {
			name = "WATER 2";
			blast_resistance = 100.0f;
			collisionHitbox_1x1x1 = false;
			geometry = GEOMETRY::NONE;
			transparent = true;
		}
};

struct Water3 : Block {
	public:
		Water3() {
			name = "WATER 3";
			blast_resistance = 100.0f;
			collisionHitbox_1x1x1 = false;
			geometry = GEOMETRY::NONE;
			transparent = true;
		}
};

struct Water4 : Block {
	public:
		Water4() {
			name = "WATER 4";
			blast_resistance = 100.0f;
			collisionHitbox_1x1x1 = false;
			geometry = GEOMETRY::NONE;
			transparent = true;
		}
};

struct Water5 : Block {
	public:
		Water5() {
			name = "WATER 5";
			blast_resistance = 100.0f;
			collisionHitbox_1x1x1 = false;
			geometry = GEOMETRY::NONE;
			transparent = true;
		}
};

struct Water6 : Block {
	public:
		Water6() {
			name = "WATER 6";
			blast_resistance = 100.0f;
			collisionHitbox_1x1x1 = false;
			geometry = GEOMETRY::NONE;
			transparent = true;
		}
};

struct Water7 : Block {
	public:
		Water7() {
			name = "WATER 7";
			blast_resistance = 100.0f;
			collisionHitbox_1x1x1 = false;
			geometry = GEOMETRY::NONE;
			transparent = true;
		}
};

struct Stick : Block {
	public:
		Stick() {
			name = "STICK";
			isFuel = true;
			fuel_time = 5;
			item3D = false;
			textureX = 8;
			textureY = 0;
		}
};

struct WoodenShovel : Block {
	public:
		WoodenShovel() {
			name = "WOODEN_SHOVEL";
			stackable = false;
			isFuel = true;
			fuel_time = 10;
			durability = 59;
			item3D = false;
			textureX = 8;
			textureY = 1;
		}
};

struct StoneShovel : Block {
	public:
		StoneShovel() {
			name = "STONE_SHOVEL";
			stackable = false;
			durability = 131;
			item3D = false;
			textureX = 8;
			textureY = 2;
		}
};

struct IronShovel : Block {
	public:
		IronShovel() {
			name = "IRON_SHOVEL";
			stackable = false;
			durability = 250;
			item3D = false;
			textureX = 8;
			textureY = 3;
		}
};

struct DiamondShovel : Block {
	public:
		DiamondShovel() {
			name = "DIAMOND_SHOVEL";
			stackable = false;
			durability = 1561;
			item3D = false;
			textureX = 8;
			textureY = 4;
		}
};

struct WoodenAxe : Block {
	public:
		WoodenAxe() {
			name = "WOODEN_AXE";
			stackable = false;
			isFuel = true;
			fuel_time = 10;
			durability = 59;
			item3D = false;
			textureX = 8;
			textureY = 5;
		}
};

struct StoneAxe : Block {
	public:
		StoneAxe() {
			name = "STONE_AXE";
			stackable = false;
			durability = 131;
			item3D = false;
			textureX = 8;
			textureY = 6;
		}
};

struct IronAxe : Block {
	public:
		IronAxe() {
			name = "IRON_AXE";
			stackable = false;
			durability = 250;
			item3D = false;
			textureX = 8;
			textureY = 7;
		}
};

struct DiamondAxe : Block {
	public:
		DiamondAxe() {
			name = "DIAMOND_AXE";
			stackable = false;
			durability = 1561;
			item3D = false;
			textureX = 8;
			textureY = 8;
		}
};

struct WoodenPickaxe : Block {
	public:
		WoodenPickaxe() {
			name = "WOODEN_PICKAXE";
			stackable = false;
			isFuel = true;
			fuel_time = 10;
			durability = 59;
			item3D = false;
			textureX = 8;
			textureY = 9;
		}
};

struct StonePickaxe : Block {
	public:
		StonePickaxe() {
			name = "STONE_PICKAXE";
			stackable = false;
			durability = 131;
			item3D = false;
			textureX = 8;
			textureY = 10;
		}
};

struct IronPickaxe : Block {
	public:
		IronPickaxe() {
			name = "IRON_PICKAXE";
			stackable = false;
			durability = 250;
			item3D = false;
			textureX = 8;
			textureY = 11;
		}
};

struct DiamondPickaxe : Block {
	public:
		DiamondPickaxe() {
			name = "DIAMOND_PICKAXE";
			stackable = false;
			durability = 1561;
			item3D = false;
			textureX = 8;
			textureY = 12;
		}
};

struct Bow : Block {
	public:
		Bow() {
			name = "BOW";
			stackable = false;
			durability = 384;
			item3D = false;
			textureX = 8;
			textureY = 13;
		}
};

struct Arrow : Block {
	public:
		Arrow() {
			name = "ARROW";
			item3D = false;
			textureX = 8;
			textureY = 14;
		}
};

struct WorldEditWand : Block {
	public:
		WorldEditWand() {
			name = "WORLDEDIT_WAND";
			item3D = false;
			textureX = 8;
			textureY = 5;
		}
};

struct Coal : Block {
	public:
		Coal() {
			name = "COAL";
			isFuel = true;
			fuel_time = 80;
			item3D = false;
			textureX = 9;
			textureY = 0;
		}
};

struct Charcoal : Block {
	public:
		Charcoal() {
			name = "CHARCOAL";
			isFuel = true;
			fuel_time = 80;
			item3D = false;
			textureX = 9;
			textureY = 1;
		}
};

struct IronIngot : Block {
	public:
		IronIngot() {
			name = "IRON_INGOT";
			item3D = false;
			textureX = 9;
			textureY = 2;
		}
};

struct Diamond : Block {
	public:
		Diamond() {
			name = "DIAMOND";
			item3D = false;
			textureX = 9;
			textureY = 3;
		}
};

struct Bucket : Block {
	public:
		Bucket() {
			name = "BUCKET";
			adventure_block = blocks::DIAMOND_ORE;
			item3D = false;
			textureX = 9;
			textureY = 4;
		}
};

struct WaterBucket : Block {
	public:
		WaterBucket() {
			name = "WATER_BUCKET";
			adventure_block = blocks::DIAMOND_ORE;
			stackable = false;
			item3D = false;
			textureX = 9;
			textureY = 5;
		}
};

struct WoodenHoe : Block {
	public:
		WoodenHoe() {
			name = "WOODEN_HOE";
			stackable = false;
			isFuel = true;
			fuel_time = 10;
			durability = 59;
			item3D = false;
			textureX = 9;
			textureY = 6;
		}
};

struct StoneHoe : Block {
	public:
		StoneHoe() {
			name = "STONE_HOE";
			stackable = false;
			durability = 131;
			item3D = false;
			textureX = 9;
			textureY = 7;
		}
};

struct IronHoe : Block {
	public:
		IronHoe() {
			name = "IRON_HOE";
			stackable = false;
			durability = 250;
			item3D = false;
			textureX = 9;
			textureY = 8;
		}
};

struct DiamondHoe : Block {
	public:
		DiamondHoe() {
			name = "DIAMOND_HOE";
			stackable = false;
			durability = 1561;
			item3D = false;
			textureX = 9;
			textureY = 9;
		}
};

struct WheatSeeds : Block {
	public:
		WheatSeeds() {
			name = "WHEAT_SEEDS";
			adventure_block = blocks::FARMLAND;
			item3D = false;
			textureX = 9;
			textureY = 10;
		}
};

struct Wheat : Block {
	public:
		Wheat() {
			name = "WHEAT";
			item3D = false;
			textureX = 9;
			textureY = 11;
		}
};

struct Bread : Block {
	public:
		Bread() {
			name = "BREAD";
			isFood = true;
			hunger_restauration = 5;
			saturation_restauration = 6;
			item3D = false;
			textureX = 9;
			textureY = 12;
		}
};

struct Apple : Block {
	public:
		Apple() {
			name = "APPLE";
			isFood = true;
			hunger_restauration = 4;
			saturation_restauration = 2.4f;
			item3D = false;
			textureX = 9;
			textureY = 13;
		}
};

struct Flint : Block {
	public:
		Flint() {
			name = "FLINT";
			item3D = false;
			textureX = 10;
			textureY = 0;
		}
};

struct FlintAndSteel : Block {
	public:
		FlintAndSteel() {
			name = "FLINT AND STEEL";
			item3D = false;
			textureX = 10;
			textureY = 1;
		}
};

extern const Block *s_blocks[S_BLOCKS_SIZE];

#endif
