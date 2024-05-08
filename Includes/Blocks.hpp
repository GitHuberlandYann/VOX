#ifndef BLOCKS_HPP
# define BLOCKS_HPP

# define GLEW_STATIC
# include <GL/glew.h> // must be before glfw
# include "GLFW/glfw3.h"
# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>
# include <string>

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

enum AXIS {
	Z,
	X,
	Y
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
		COAL_ORE = 40,
		IRON_ORE,
		DIAMOND_ORE,
		COAL_BLOCK,
		IRON_BLOCK,
		DIAMOND_BLOCK,
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
		CHEST = 79,
		WHEAT_CROP = 80,
		WHEAT_CROP1,
		WHEAT_CROP2,
		WHEAT_CROP3,
		WHEAT_CROP4,
		WHEAT_CROP5,
		WHEAT_CROP6,
		WHEAT_CROP7,
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
		NOTVISIBLE = 1 << 8,
		WET_FARMLAND = 1 << 9,
		NATURAL = 1 << 9
	};
}

struct Block {
	public:
		std::string name = "DEFAULT";
		int mined = blocks::AIR;
		bool stackable = true;
		char light_level = 0;
		bool isFuel = false;
		float fuel_time = 0;
		bool isComposant = false;
		int getProduction = blocks::AIR;
		float blast_resistance = FLT_MAX;
		bool hasHitbox = false;
		bool collisionHitbox_1x1x1 = true;
		bool collisionHitbox = false;
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
		bool item3D = true;
		int textureX = 1;
		int textureY = 15;

		bool canCollect( int tool ) const {
			if (byHand) {
				return (true);
			}
			return (tool >= needed_tool + needed_material_level && tool < needed_tool + 4);
		}
		virtual int texX( face_dir dir = face_dir::MINUSY, int offset = 0 ) const {
			(void)dir;(void)offset;
			return (textureX);
		}
		virtual int texY( face_dir dir = face_dir::MINUSY, int offset = 0 ) const {
			(void)dir;(void)offset;
			return (textureY);
		}
		virtual void getSecondaryHitbox( glm::vec3 *hitbox, int orientation, int corners ) const {
			(void)hitbox;(void)orientation;(void)corners;
		}
		virtual ~Block() {}
};

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

struct TBD : Block {
	public:
		TBD() {
			name = "TBD";
		}
};

struct GrassBlock : Block {
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
		virtual int texX( face_dir dir, int offset ) const {
			(void)offset;
			if (dir == face_dir::PLUSZ) {
				return (1);
			} else if (dir == face_dir::MINUSZ) {
				return (4);
			}
			return (0);
		}
};

struct OakLog : Block {
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
		virtual int texX( face_dir dir, int offset ) const {
			switch (dir) {
				case face_dir::MINUSZ:
				case face_dir::PLUSZ:
					return ((offset == AXIS::Z) + 2 * (offset == AXIS::X));
				case face_dir::MINUSX:
				case face_dir::PLUSX:
					return ((offset == AXIS::X) + 2 * (offset == AXIS::Y));
				case face_dir::MINUSY:
				case face_dir::PLUSY:
					return ((offset == AXIS::Y) + 2 * (offset == AXIS::X));
			}
			return (0);
		}
};

struct Cactus : Block {
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
		virtual int texX( face_dir dir, int offset ) const {
			(void)offset;
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
		virtual int texX( face_dir dir, int offset ) const {
			if (dir == face_dir::PLUSZ) {
				return (2 + !offset);
			}
			return (4);
		}
};

struct DirtPath : Block {
	public:
		DirtPath() {
			name = "DIRT PATH";
			mined = blocks::DIRT;
			blast_resistance = 0.65f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, 7.5f / 16.0f};
			hitboxHalfSize = {0.5f, 0.5f, 7.5f / 16.0f};
			geometry = GEOMETRY::FARMLAND;
			byHand = true;
			needed_tool = blocks::WOODEN_SHOVEL;
			hardness = 0.65f;
		}
		virtual int texX( face_dir dir, int offset ) const {
			(void)offset;
			if (dir == face_dir::PLUSZ) {
				return (1);
			} else if (dir == face_dir::MINUSZ) {
				return (4);
			}
			return (0);
		}
		virtual int texY(  face_dir dir, int offset ) const {
			(void)offset;
			if (dir == face_dir::MINUSZ) {
				return (1);
			}
			return (5);
		}
};

struct TNT : Block {
	public:
		TNT() {
			name = "TNT";
			mined = blocks::TNT;
			blast_resistance = 0.0f;
			byHand = true;
			hardness = 0.0f;
			textureY = 6;
		}
		virtual int texX( face_dir dir, int offset ) const {
			(void)offset;
			if (dir == face_dir::PLUSZ) {
				return (1);
			} else if (dir == face_dir::MINUSZ) {
				return (2);
			}
			return (0);
		}
};

struct CraftingTable : Block {
	public:
		CraftingTable() {
			name = "CRAFTING_TABLE";
			mined = blocks::CRAFTING_TABLE;
			isFuel = true;
			fuel_time = 15;
			blast_resistance = 2.5f;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 2.5f;
			textureY = 8;
		}
		virtual int texX( face_dir dir, int offset ) const {
			if (dir == face_dir::PLUSZ || dir == face_dir::MINUSZ) {
				return (1);
			} else if ((offset & 0x7) == dir) {
				return (2);
			}
			return (0);
		}
};

struct Furnace : Block {
	public:
		Furnace() {
			name = "FURNACE";
			mined = blocks::FURNACE;
			blast_resistance = 3.5f;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			hardness = 3.5f;
			textureY = 9;
		}
		virtual int texX( face_dir dir, int offset ) const {
			if (dir == face_dir::PLUSZ || dir == face_dir::MINUSZ) {
				return (1);
			} else if ((offset & 0x7) == dir) {
				return (2 + ((offset >> 4) & 0x1));
			}
			return (0);
		}
};

struct OakStairsBottom : Block {
	public:
		OakStairsBottom() {
			name = "OAK_STAIRS_BOTTOM";
			mined = blocks::OAK_STAIRS_BOTTOM;
			blast_resistance = 3.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			orientedCollisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, 0.25f};
			hitboxHalfSize = {0.5f, 0.5f, 0.25f};
			geometry = GEOMETRY::STAIRS_BOTTOM;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 2.0f;
			transparent = true;
			textureX = 4;
			textureY = 10;
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
};

struct OakStairsTop : Block {
	public:
		OakStairsTop() {
			name = "OAK_STAIRS_TOP";
			mined = blocks::OAK_STAIRS_BOTTOM;
			blast_resistance = 3.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			orientedCollisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, 0.75f};
			hitboxHalfSize = {0.5f, 0.5f, 0.25f};
			geometry = GEOMETRY::STAIRS_TOP;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 2.0f;
			transparent = true;
			textureX = 4;
			textureY = 10;
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
};

struct OakDoor : Block {
	public:
		OakDoor() {
			name = "OAK_DOOR";
			mined = blocks::OAK_DOOR;
			blast_resistance = 3.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			orientedCollisionHitbox = true;
			hitboxCenter = {0, 0, 100000}; // we discard normal hitbox
			geometry = GEOMETRY::DOOR;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 3.0f;
			transparent = true;
			item3D = false;
		}
		// offset is bool half or 2 if item
		virtual int texX( face_dir dir, int offset ) const {
			(void)dir;
			return ((offset == 2) ? 9 : 0);
		}
		virtual int texY( face_dir dir, int offset ) const {
			switch (dir) {
				case face_dir::PLUSZ:
					return (10);
				case face_dir::MINUSZ:
					return (11);
				default:
					return ((offset == 2) ? 14 : 11 - (offset & DOOR::UPPER_HALF));
			}
		}
		virtual void getSecondaryHitbox( glm::vec3 *hitbox, int orientation, int bitfield ) const {
			switch (orientation) {
				case face_dir::MINUSX:
					if (!(bitfield & DOOR::OPEN)) {
						hitbox[0] = {0.90625f, 0.5f, 0.5f};
						hitbox[1] = {0.09375f, 0.5f, 0.5f};
					} else {
						hitbox[0] = {0.5f, (bitfield & DOOR::RIGHT_HINGE) ? 0.09375f : 0.90625f, 0.5f};
						hitbox[1] = {0.5f, 0.09375f, 0.5f};
					}
					break ;
				case face_dir::PLUSX:
					if (!(bitfield & DOOR::OPEN)) {
						hitbox[0] = {0.09375f, 0.5f, 0.5f};
						hitbox[1] = {0.09375f, 0.5f, 0.5f};
					} else {
						hitbox[0] = {0.5f, (bitfield & DOOR::RIGHT_HINGE) ? 0.90625f : 0.09375f, 0.5f};
						hitbox[1] = {0.5f, 0.09375f, 0.5f};
					}
					break ;
				case face_dir::MINUSY:
					if (!(bitfield & DOOR::OPEN)) {
						hitbox[0] = {0.5f, 0.90625f, 0.5f};
						hitbox[1] = {0.5f, 0.09375f, 0.5f};
					} else {
						hitbox[0] = {(bitfield & DOOR::RIGHT_HINGE) ? 0.90625f : 0.09375f, 0.5f, 0.5f};
						hitbox[1] = {0.09375f, 0.5f, 0.5f};
					}
					break ;
				case face_dir::PLUSY:
					if (!(bitfield & DOOR::OPEN)) {
						hitbox[0] = {0.5f, 0.09375f, 0.5f};
						hitbox[1] = {0.5f, 0.09375f, 0.5f};
					} else {
						hitbox[0] = {(bitfield & DOOR::RIGHT_HINGE) ? 0.09375f : 0.90625f, 0.5f, 0.5f};
						hitbox[1] = {0.09375f, 0.5f, 0.5f};
					}
					break ;
			}
		}
};

struct OakTrapdoor : Block {
	public:
		OakTrapdoor() {
			name = "OAK_TRAPDOOR";
			mined = blocks::OAK_TRAPDOOR;
			blast_resistance = 3.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			orientedCollisionHitbox = true;
			hitboxCenter = {0, 0, 100000}; // we discard normal hitbox
			geometry = GEOMETRY::TRAPDOOR;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 3.0f;
			transparent = true;
			textureX = 2;
			textureY = 11;
		}
		virtual void getSecondaryHitbox( glm::vec3 *hitbox, int orientation, int bitfield ) const {
			if (!(bitfield & DOOR::OPEN)) {
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
};

struct StoneStairsBottom : OakStairsBottom {
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

struct StoneStairsTop : OakStairsTop {
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

struct SmoothStoneStairsBottom : OakStairsBottom {
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

struct SmoothStoneStairsTop : OakStairsTop {
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

struct CobbleStoneStairsBottom : OakStairsBottom {
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

struct CobbleStoneStairsTop : OakStairsTop {
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

struct StoneBricksStairsBottom : OakStairsBottom {
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

struct StoneBricksStairsTop : OakStairsTop {
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

struct Bedrock : Block {
	public:
		Bedrock() {
			name = "BEDROCK";
			byHand = false;
			textureX = 4;
			textureY = 0;
		}
};

struct Dirt : Block {
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

struct SmoothStone : Block {
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

struct Stone : Block {
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

struct Cobblestone : Block {
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

struct StoneBrick : Block {
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

struct CrackedStoneBrick : Block {
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

struct Sand : Block {
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

struct Gravel : Block {
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

struct OakLeaves : Block {
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

struct OakPlanks : Block {
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
};

struct GlassPane : Block {
	public:
		GlassPane() {
			name = "GLASS_PANE";
			blast_resistance = 0.3f;
			hasHitbox = true;
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
};

struct CoalOre : Block {
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

struct IronOre : Block {
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

struct DiamondOre : Block {
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

struct CoalBlock : Block {
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

struct IronBlock : Block {
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

struct DiamondBlock : Block {
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

struct OakSlabBottom : Block {
	public:
		OakSlabBottom() {
			name = "OAK_SLAB_BOTTOM";
			mined = blocks::OAK_SLAB_BOTTOM;
			blast_resistance = 3.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, 0.25f};
			hitboxHalfSize = {0.5f, 0.5f, 0.25f};
			geometry = GEOMETRY::SLAB_BOTTOM;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 2.0f;
			transparent = true;
			textureX = 4;
			textureY = 10;
		}
};

struct OakSlabTop : Block {
	public:
		OakSlabTop() {
			name = "OAK_SLAB_TOP";
			mined = blocks::OAK_SLAB_BOTTOM;
			blast_resistance = 3.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, 0.75f};
			hitboxHalfSize = {0.5f, 0.5f, 0.25f};
			geometry = GEOMETRY::SLAB_TOP;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 2.0f;
			transparent = true;
			textureX = 4;
			textureY = 10;
		}
};

struct OakFence : Block {
	public:
		OakFence() {
			name = "OAK_FENCE";
			mined = blocks::OAK_FENCE;
			blast_resistance = 3.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			orientedCollisionHitbox = true;
			hitboxCenter = {0, 0, 100000}; // we discard normal hitbox
			geometry = GEOMETRY::FENCE;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			hardness = 2.0f;
			transparent = true;
			textureX = 4;
			textureY = 10;
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
};

struct StoneSlabBottom : OakSlabBottom {
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

struct StoneSlabTop : OakSlabTop {
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

struct SmoothStoneSlabBottom : OakSlabBottom {
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

struct SmoothStoneSlabTop : OakSlabTop {
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

struct CobbleStoneSlabBottom : OakSlabBottom {
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

struct CobbleStoneSlabTop : OakSlabTop {
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

struct StoneBricksSlabBottom : OakSlabBottom {
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

struct StoneBricksSlabTop : OakSlabTop {
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

struct Flower : Block {
	public:
		Flower() {
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
};

struct Poppy : Flower {
	public:
		Poppy() {
			name = "POPPY";
			mined = blocks::POPPY;
			textureX = 6;
			textureY = 0;
		}
};

struct Dandelion : Flower {
	public:
		Dandelion() {
			name = "DANDELION";
			mined = blocks::DANDELION;
			textureX = 6;
			textureY = 1;
		}
};

struct BlueOrchid : Flower {
	public:
		BlueOrchid() {
			name = "BLUE_ORCHID";
			mined = blocks::BLUE_ORCHID;
			textureX = 6;
			textureY = 2;
		}
};

struct Allium : Flower {
	public:
		Allium() {
			name = "ALLIUM";
			mined = blocks::ALLIUM;
			textureX = 6;
			textureY = 3;
		}
};

struct CornFlower : Flower {
	public:
		CornFlower() {
			name = "CORNFLOWER";
			mined = blocks::CORNFLOWER;
			textureX = 6;
			textureY = 4;
		}
};

struct PinkTulip : Flower {
	public:
		PinkTulip() {
			name = "PINK_TULIP";
			mined = blocks::PINK_TULIP;
			textureX = 6;
			textureY = 5;
		}
};

struct Grass : Flower {
	public:
		Grass() {
			name = "GRASS";
			mined = blocks::WHEAT_SEEDS;
			hasHitbox = false;
			textureX = 6;
			textureY = 6;
		}
};

struct SugarCane : Flower {
	public:
		SugarCane() {
			name = "SUGAR_CANE";
			mined = blocks::SUGAR_CANE;
			hasHitbox = false;
			textureX = 6;
			textureY = 7;
		}
};

struct DeadBush : Flower {
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

struct OakSapling : Flower {
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
			collisionHitbox_1x1x1 = false;
			hitboxCenter = {0.5f, 0.5f, 5 / 16.0f};
			hitboxHalfSize = {1 / 16.0f, 1 / 16.0f, 5 / 16.0f};
			geometry = GEOMETRY::TORCH;
			light_level = 14;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			item3D = false;
			textureX = 6;
			textureY = 10;
		}
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
};

struct WheatCrop : Block {
	public:
		WheatCrop() {
			name = "WHEAT_CROP";
			mined = blocks::WHEAT_SEEDS;
			blast_resistance = 0.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			hitboxCenter = {0.5f, 0.5f, 1 / 32.0f};
			hitboxHalfSize = {0.4f, 0.4f, 1 / 32.0f};
			geometry = GEOMETRY::CROP;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			item3D = false;
			textureX = 7;
		}
		virtual int texY(  face_dir dir, int offset = 0 ) const {
			(void)dir;
			return (offset);
		}
};

struct WheatCrop1 : Block {
	public:
		WheatCrop1() {
			name = "WHEAT_CROP 1";
			mined = blocks::WHEAT_SEEDS;
			blast_resistance = 0.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			hitboxCenter = {0.5f, 0.5f, 3 / 32.0f};
			hitboxHalfSize = {0.4f, 0.4f, 3 / 32.0f};
			geometry = GEOMETRY::CROP;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
		}
};

struct WheatCrop2 : Block {
	public:
		WheatCrop2() {
			name = "WHEAT_CROP 2";
			mined = blocks::WHEAT_SEEDS;
			blast_resistance = 0.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			hitboxCenter = {0.5f, 0.5f, 5 / 32.0f};
			hitboxHalfSize = {0.4f, 0.4f, 5 / 32.0f};
			geometry = GEOMETRY::CROP;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
		}
};

struct WheatCrop3 : Block {
	public:
		WheatCrop3() {
			name = "WHEAT_CROP 3";
			mined = blocks::WHEAT_SEEDS;
			blast_resistance = 0.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			hitboxCenter = {0.5f, 0.5f, 7 / 32.0f};
			hitboxHalfSize = {0.4f, 0.4f, 7 / 32.0f};
			geometry = GEOMETRY::CROP;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
		}
};

struct WheatCrop4 : Block {
	public:
		WheatCrop4() {
			name = "WHEAT_CROP 4";
			mined = blocks::WHEAT_SEEDS;
			blast_resistance = 0.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			hitboxCenter = {0.5f, 0.5f, 9 / 32.0f};
			hitboxHalfSize = {0.4f, 0.4f, 9 / 32.0f};
			geometry = GEOMETRY::CROP;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
		}
};

struct WheatCrop5 : Block {
	public:
		WheatCrop5() {
			name = "WHEAT_CROP 5";
			mined = blocks::WHEAT_SEEDS;
			blast_resistance = 0.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			hitboxCenter = {0.5f, 0.5f, 11 / 32.0f};
			hitboxHalfSize = {0.4f, 0.4f, 11 / 32.0f};
			geometry = GEOMETRY::CROP;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
		}
};

struct WheatCrop6 : Block {
	public:
		WheatCrop6() {
			name = "WHEAT_CROP 6";
			mined = blocks::WHEAT_SEEDS;
			blast_resistance = 0.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			hitboxCenter = {0.5f, 0.5f, 13 / 32.0f};
			hitboxHalfSize = {0.4f, 0.4f, 13 / 32.0f};
			geometry = GEOMETRY::CROP;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
		}
};

struct WheatCrop7 : Block {
	public:
		WheatCrop7() {
			name = "WHEAT_CROP 7";
			mined = blocks::WHEAT;
			blast_resistance = 0.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			hitboxCenter = {0.5f, 0.5f, 15 / 32.0f};
			hitboxHalfSize = {0.4f, 0.4f, 15 / 32.0f};
			geometry = GEOMETRY::CROP;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
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
			item3D = false;
			textureX = 9;
			textureY = 4;
		}
};

struct WaterBucket : Block {
	public:
		WaterBucket() {
			name = "WATER_BUCKET";
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
