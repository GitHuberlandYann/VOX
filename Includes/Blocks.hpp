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
# include <array>
# include <memory>

class Chunk;
class UI;
typedef struct s_shaderInput t_shaderInput;

const float one16th = 0.0625f;

# define S_BLOCKS_SIZE 168

namespace face_dir {
	enum {
		minus_y,
		plus_y,
		minus_x,
		plus_x,
		minus_z,
		plus_z
	};
};

namespace corners {
	const int mm = 0b0001;  // -x-y
	const int mp = 0b0010;  // -x+y
	const int pm = 0b0100;  // +x-y
	const int pp = 0b1000;  // +x+y
};

namespace fence {
	const int my = 0b0001;  // -y
	const int py = 0b0010;  // +y
	const int mx = 0b0100;  // -x
	const int px = 0b1000;  // +x
	enum {
		base,
		arm,
		arm_end,
	};
};

namespace door {
	const int upper_half  = 0b001;
	const int right_hinge = 0b010;
	const int open        = 0b100;
};

namespace placement {
	enum {
		wall,
		floor,
		ceiling
	};
};

namespace redstone {
	const bool on = true;
	const bool off = false;
	const int tick = 2;

	namespace dust {
		const int none = 0;
		const int side = 1;
		const int up   = 2;
	};

	namespace comparator {
		const int compare = 0;
		const int substract = 1;
	};

	namespace piston {
		const int force_retraction = 13; // used by Chunk::pistonExtendCount
		const int cancel_retraction = 42; // random nbr above 12, used by Chunk::pistonExtendCount
	};
};

namespace offset {

	namespace blocks {
		const int orientation = 13;
		const int bitfield = 24;
	};

	namespace redstone {
		const int powered = 16;
		const int weakdy_powered = 17;
		const int activated = 18;
		const int strength = 20;

		namespace dust {
			const int my = 24;
			const int py = 26;
			const int mx = 28;
			const int px = 30;
		};

		namespace repeater {
			const int ticks = 24;
		};

		namespace piston {
			const int sticky = 24;
		};
	};

	const int adventure_block = 19;

	namespace farmland {
		const int moisture = 25;
	};
};

namespace mask {
	const int all_bits = -1;

	namespace blocks {
		const int type = 0xFFF;
		const int notVisible = (1 << 12);
		const int orientation = (0x7 << offset::blocks::orientation);
	};

	namespace redstone {
		const int powered = (1 << offset::redstone::powered);
		const int weakdy_powered = (1 << offset::redstone::weakdy_powered);
		const int activated = (1 << offset::redstone::activated);
		const int strength = (0xF << offset::redstone::strength);
		
		namespace dust {
			const int connect = 0x3;
		};

		namespace repeater {
			const int lock = (1 << 27);
		};

		namespace comparator {
			const int mode = (1 << 24);
		};

		namespace piston {
			const int sticky = (1 << offset::redstone::piston::sticky);
			const int moving = (1 << 24);
			const int retracting = (1 << 28); // flag given to blocks::MOVING_PISTON to set their transparency -> transparent when retracting, solid when extending
		};
	};

	const int adventure_block = (1 << offset::adventure_block);

	namespace sapling {
		const int growth = (1 << 24);
	};

	namespace leaves {
		const int natural = (1 << 24);
	};

	namespace farmland {
		const int wet = (1 << 24);
		const int moisture = (0x7 << offset::farmland::moisture);
	};

	namespace frame {
		const int locked = (1 << 26);
		const int notVisible = (1 << 27);
	};
};

enum class geometry {
	none,          // air, water
	cube,          // default blocks
	cross,         // flowers and such
	farmland,      // cube with z = 15 * one16th
	torch,
	slab_bottom,
	slab_top,
	stairs_bottom,
	stairs_top,
	fence,
	glass,
	glass_pane,
	door,
	trapdoor,
	crop,
	lever,
	dust,
	repeater,
	button,
	piston,
};

namespace blocks {
	enum {
		air,
		grass_block,
		oak_log,
		cactus,
		farmland,
		dirt_path,
		tnt,
		target,
		crafting_table = 8,
		furnace,
		oak_stairs_bottom,
		oak_stairs_top,
		oak_door,
		oak_trapdoor,
		stone_stairs_bottom,
		stone_stairs_top,
		smooth_stone_stairs_bottom = 16,
		smooth_stone_stairs_top,
		cobblestone_stairs_bottom,
		cobblestone_stairs_top,
		stone_bricks_stairs_bottom,
		stone_bricks_stairs_top,
		lever,
		oak_sign,
		bedrock = 24,
		dirt,
		smooth_stone,
		stone,
		cobblestone,
		stone_bricks,
		cracked_stone_bricks,
		sand,
		gravel = 32,
		oak_leaves,
		oak_planks,
		glass,
		glass_pane,
		redstone_lamp,
		stone_button,
		oak_button,
		coal_ore = 40,
		iron_ore,
		diamond_ore,
		coal_block,
		iron_block,
		diamond_block,
		redstone_ore,
		redstone_block,
		oak_slab_bottom = 48,
		oak_slab_top,
		oak_fence,
		stone_slab_bottom,
		stone_slab_top,
		smooth_stone_slab_bottom,
		smooth_stone_slab_top,
		cobblestone_slab_bottom,
		cobblestone_slab_top = 56,
		stone_bricks_slab_bottom,
		stone_bricks_slab_top,
		piston,
		sticky_piston,
		piston_head,
		moving_piston,
		observer,
		poppy = 64,
		dandelion,
		blue_orchid,
		allium,
		cornflower,
		pink_tulip,
		grass,
		sugar_cane,
		dead_bush = 72,
		oak_sapling,
		torch,
		redstone_torch,
		redstone_dust,
		repeater,
		comparator,
		chest,
		wheat_crop = 80,
		item_frame,
		birch_planks,
		sand_stone,
		water = 88,
		stick = 96,
		wooden_shovel,
		stone_shovel,
		iron_shovel,
		diamond_shovel,
		wooden_axe,
		stone_axe,
		iron_axe,
		diamond_axe = 104,
		wooden_pickaxe,
		stone_pickaxe,
		iron_pickaxe,
		diamond_pickaxe,
		bow,
		arrow,
		worldedit_wand,
		coal = 112,
		charcoal,
		iron_ingot,
		diamond,
		bucket,
		water_bucket,
		wooden_hoe,
		stone_hoe,
		iron_hoe = 120,
		diamond_hoe,
		wheat_seeds,
		wheat,
		bread,
		apple,
		flint,
		flint_and_steel,
		zombie_egg = 128,
		skeleton_egg,
		white_wool = 136,
		pink_wool,
		magenta_wool,
		purple_wool,
		blue_wool,
		light_blue_wool,
		cyan_wool,
		green_wool,
		lime_wool = 144,
		yellow_wool,
		orange_wool,
		red_wool,
		brown_wool,
		black_wool,
		gray_wool,
		light_gray_wool,
		white_carpet = 152,
		pink_carpet,
		magenta_carpet,
		purple_carpet,
		blue_carpet,
		light_blue_carpet,
		cyan_carpet,
		green_carpet,
		lime_carpet = 160,
		yellow_carpet,
		orange_carpet,
		red_carpet,
		brown_carpet,
		black_carpet,
		gray_carpet,
		light_gray_carpet,
	};
	const int item = 1942;
}

struct Block {
	public:
		std::string name = "DEFAULT";
		int mined = blocks::air;
		int adventure_block = blocks::bedrock;
		int getProduction = blocks::air;
		int hunger_restauration = 0;
		int needed_tool = blocks::item;
		int needed_material_level = 0;
		int durability = 0;
		int textureX = 1;
		int textureY = 15;
		float fuel_time = 0;
		float blast_resistance = FLT_MAX;
		float saturation_restauration = 0;
		float hardness = -1;
		glm::vec3 hitboxCenter = {0, 0, 0};
		glm::vec3 hitboxHalfSize = {0, 0, 0};
		geometry geometry = geometry::cube;
		char light_level = 0;
		bool stackable = true;
		bool isFuel = false;
		bool isComposant = false;
		bool hasHitbox = false;
		bool hasOrientedHitbox = false;
		bool collisionHitbox_1x1x1 = true;
		bool collisionHitbox = false;
		bool oriented = false;
		bool orientedCollisionHitbox = false;
		bool isFood = false;
		bool byHand = false;
		bool transparent = false;
		bool redstoneComponant = false;
		bool isSolidForFluid = true;
		bool isItemOnly = false;
		bool item3D = true;

		bool canCollect( int tool ) const {
			if (byHand) {
				return (true);
			}
			return (tool >= needed_tool + needed_material_level && tool < needed_tool + 4);
		}
		virtual int texX( int dir = face_dir::minus_y, int value = 0 ) const {
			(void)dir;(void)value;
			return (textureX);
		}
		virtual int texY( int dir = face_dir::minus_y, int value = 0 ) const {
			(void)dir;(void)value;
			return (textureY);
		}
		virtual void getSecondaryHitbox( glm::vec3* hitbox, int orientation, int corners ) const {
			(void)hitbox;(void)orientation;(void)corners;
		}
		virtual void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const {
			(void)chunk;
			(void)vertices;
			(void)start;
			(void)pos;
			(void)value;
		}
		virtual void addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const;
		virtual void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const;
		virtual bool isTransparent( int value ) const {
			(void)value;
			return (transparent);
		}
		virtual ~Block() {}
};

struct Cube : Block {
	public:
		Cube() {
			geometry = geometry::cube;
		}
		virtual void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
		virtual void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const;
};

struct Cross : Block {
	public:
		Cross() {
			blast_resistance = 0.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			hitboxCenter = {0.5f, 0.5f, 0.3f};
			hitboxHalfSize = {0.2f, 0.2f, 0.3f};
			geometry = geometry::cross;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			isSolidForFluid = false;
			item3D = false;
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
};

struct SlabBottom : Block {
	public:
		SlabBottom() {
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, 0.25f};
			hitboxHalfSize = {0.5f, 0.5f, 0.25f};
			geometry = geometry::slab_bottom;
			transparent = true;
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
		void addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const override;
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
};

struct SlabTop : Block {
	public:
		SlabTop() {
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, 0.75f};
			hitboxHalfSize = {0.5f, 0.5f, 0.25f};
			geometry = geometry::slab_top;
			transparent = true;
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
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
			geometry = geometry::stairs_bottom;
			transparent = true;
		}
		void getSecondaryHitbox( glm::vec3* hitbox, int orientation, int corners ) const override {
			switch (corners) {
				case corners::mm | corners::mp:
					hitbox[0] = {0.25f, 0.5f, 0.75f}; // hitboxCenter
					hitbox[1] = {0.25f, 0.5f, 0.25f}; // hitboxHalfSize
					break ;
				case corners::pm | corners::pp:
					hitbox[0] = {0.75f, 0.5f, 0.75f};
					hitbox[1] = {0.25f, 0.5f, 0.25f};
					break ;
				case corners::mm | corners::pm:
					hitbox[0] = {0.5f, 0.25f, 0.75f};
					hitbox[1] = {0.5f, 0.25f, 0.25f};
					break ;
				case corners::mp | corners::pp:
					hitbox[0] = {0.5f, 0.75f, 0.75f};
					hitbox[1] = {0.5f, 0.25f, 0.25f};
					break ;
				case corners::mm:
					hitbox[0] = {0.25f, 0.25f, 0.75f};
					hitbox[1] = {0.25f, 0.25f, 0.25f};
					break ;
				case corners::mp:
					hitbox[0] = {0.25f, 0.75f, 0.75f};
					hitbox[1] = {0.25f, 0.25f, 0.25f};
					break ;
				case corners::pm:
					hitbox[0] = {0.75f, 0.25f, 0.75f};
					hitbox[1] = {0.25f, 0.25f, 0.25f};
					break ;
				case corners::pp:
					hitbox[0] = {0.75f, 0.75f, 0.75f};
					hitbox[1] = {0.25f, 0.25f, 0.25f};
					break ;
				case corners::mm | corners::mp | corners::pm:
					if (orientation == face_dir::plus_x) {
						hitbox[0] = {0.25f, 0.5f, 0.75f};
						hitbox[1] = {0.25f, 0.5f, 0.25f};
					} else {
						hitbox[0] = {0.5f, 0.25f, 0.75f};
						hitbox[1] = {0.5f, 0.25f, 0.25f};
					}
					break ;
				case corners::mm | corners::mp | corners::pp:
					if (orientation == face_dir::plus_x) {
						hitbox[0] = {0.25f, 0.5f, 0.75f};
						hitbox[1] = {0.25f, 0.5f, 0.25f};
					} else {
						hitbox[0] = {0.5f, 0.75f, 0.75f};
						hitbox[1] = {0.5f, 0.25f, 0.25f};
					}
					break ;
				case corners::pm | corners::pp | corners::mm:
					if (orientation == face_dir::minus_x) {
						hitbox[0] = {0.75f, 0.5f, 0.75f};
						hitbox[1] = {0.25f, 0.5f, 0.25f};
					} else {
						hitbox[0] = {0.5f, 0.25f, 0.75f};
						hitbox[1] = {0.5f, 0.25f, 0.25f};
					}
					break ;
				case corners::pm | corners::pp | corners::mp:
					if (orientation == face_dir::minus_x) {
						hitbox[0] = {0.75f, 0.5f, 0.75f};
						hitbox[1] = {0.25f, 0.5f, 0.25f};
					} else {
						hitbox[0] = {0.5f, 0.75f, 0.75f};
						hitbox[1] = {0.5f, 0.25f, 0.25f};
					}
					break ;
			}
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
		void addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const override;
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
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
			geometry = geometry::stairs_top;
			transparent = true;
		}
		void getSecondaryHitbox( glm::vec3* hitbox, int orientation, int corners ) const override {
			switch (corners) {
				case corners::mm | corners::mp:
					hitbox[0] = {0.25f, 0.5f, 0.25f}; // hitboxCenter
					hitbox[1] = {0.25f, 0.5f, 0.25f}; // hitboxHalfSize
					break ;
				case corners::pm | corners::pp:
					hitbox[0] = {0.75f, 0.5f, 0.25f};
					hitbox[1] = {0.25f, 0.5f, 0.25f};
					break ;
				case corners::mm | corners::pm:
					hitbox[0] = {0.5f, 0.25f, 0.25f};
					hitbox[1] = {0.5f, 0.25f, 0.25f};
					break ;
				case corners::mp | corners::pp:
					hitbox[0] = {0.5f, 0.75f, 0.25f};
					hitbox[1] = {0.5f, 0.25f, 0.25f};
					break ;
				case corners::mm:
					hitbox[0] = {0.25f, 0.25f, 0.25f};
					hitbox[1] = {0.25f, 0.25f, 0.25f};
					break ;
				case corners::mp:
					hitbox[0] = {0.25f, 0.75f, 0.25f};
					hitbox[1] = {0.25f, 0.25f, 0.25f};
					break ;
				case corners::pm:
					hitbox[0] = {0.75f, 0.25f, 0.25f};
					hitbox[1] = {0.25f, 0.25f, 0.25f};
					break ;
				case corners::pp:
					hitbox[0] = {0.75f, 0.75f, 0.25f};
					hitbox[1] = {0.25f, 0.25f, 0.25f};
					break ;
				case corners::mm | corners::mp | corners::pm:
					if (orientation == face_dir::plus_x) {
						hitbox[0] = {0.25f, 0.5f, 0.25f};
						hitbox[1] = {0.25f, 0.5f, 0.25f};
					} else {
						hitbox[0] = {0.5f, 0.25f, 0.25f};
						hitbox[1] = {0.5f, 0.25f, 0.25f};
					}
					break ;
				case corners::mm | corners::mp | corners::pp:
					if (orientation == face_dir::plus_x) {
						hitbox[0] = {0.25f, 0.5f, 0.25f};
						hitbox[1] = {0.25f, 0.5f, 0.25f};
					} else {
						hitbox[0] = {0.5f, 0.75f, 0.25f};
						hitbox[1] = {0.5f, 0.25f, 0.25f};
					}
					break ;
				case corners::pm | corners::pp | corners::mm:
					if (orientation == face_dir::minus_x) {
						hitbox[0] = {0.75f, 0.5f, 0.25f};
						hitbox[1] = {0.25f, 0.5f, 0.25f};
					} else {
						hitbox[0] = {0.5f, 0.25f, 0.25f};
						hitbox[1] = {0.5f, 0.25f, 0.25f};
					}
					break ;
				case corners::pm | corners::pp | corners::mp:
					if (orientation == face_dir::minus_x) {
						hitbox[0] = {0.75f, 0.5f, 0.25f};
						hitbox[1] = {0.25f, 0.5f, 0.25f};
					} else {
						hitbox[0] = {0.5f, 0.75f, 0.25f};
						hitbox[1] = {0.5f, 0.25f, 0.25f};
					}
					break ;
			}
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
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
			geometry = geometry::fence;
			transparent = true;
		}
		void getSecondaryHitbox( glm::vec3* hitbox, int orientation, int bitfield ) const override {
			(void)orientation;
			hitbox[0] = {0.5f, 0.5f, 0.5f};
			hitbox[1] = {0.125f, 0.125f, 0.5f};
			switch (bitfield & (fence::mx | fence::px)) {
				case fence::mx:
					hitbox[0].x = 0.375f;
					hitbox[1].x = 0.375f;
					break ;
				case fence::px:
					hitbox[0].x = 1 - 0.375f;
					hitbox[1].x = 0.375f;
					break ;
				case fence::mx | fence::px:
					hitbox[1].x = 0.5f;
					break ;
			}
			switch (bitfield & (fence::my | fence::py)) {
				case fence::my:
					hitbox[0].y = 0.375f;
					hitbox[1].y = 0.375f;
					break ;
				case fence::py:
					hitbox[0].y = 1 - 0.375f;
					hitbox[1].y = 0.375f;
					break ;
				case fence::my | fence::py:
					hitbox[1].y = 0.5f;
					break ;
			}
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
		void addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const override;
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
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
			geometry = geometry::door;
			transparent = true;
			redstoneComponant = true;
			item3D = false;
		}
		void getSecondaryHitbox( glm::vec3* hitbox, int orientation, int bitfield ) const override {
			bool open = !!(bitfield & door::open) ^ !!((bitfield << 12) & mask::redstone::powered);
			switch (orientation) {
				case face_dir::minus_x:
					if (!open) {
						hitbox[0] = {0.90625f, 0.5f, 0.5f};
						hitbox[1] = {0.09375f, 0.5f, 0.5f};
					} else {
						hitbox[0] = {0.5f, (bitfield & door::right_hinge) ? 0.09375f : 0.90625f, 0.5f};
						hitbox[1] = {0.5f, 0.09375f, 0.5f};
					}
					break ;
				case face_dir::plus_x:
					if (!open) {
						hitbox[0] = {0.09375f, 0.5f, 0.5f};
						hitbox[1] = {0.09375f, 0.5f, 0.5f};
					} else {
						hitbox[0] = {0.5f, (bitfield & door::right_hinge) ? 0.90625f : 0.09375f, 0.5f};
						hitbox[1] = {0.5f, 0.09375f, 0.5f};
					}
					break ;
				case face_dir::minus_y:
					if (!open) {
						hitbox[0] = {0.5f, 0.90625f, 0.5f};
						hitbox[1] = {0.5f, 0.09375f, 0.5f};
					} else {
						hitbox[0] = {(bitfield & door::right_hinge) ? 0.90625f : 0.09375f, 0.5f, 0.5f};
						hitbox[1] = {0.09375f, 0.5f, 0.5f};
					}
					break ;
				case face_dir::plus_y:
					if (!open) {
						hitbox[0] = {0.5f, 0.09375f, 0.5f};
						hitbox[1] = {0.5f, 0.09375f, 0.5f};
					} else {
						hitbox[0] = {(bitfield & door::right_hinge) ? 0.09375f : 0.90625f, 0.5f, 0.5f};
						hitbox[1] = {0.09375f, 0.5f, 0.5f};
					}
					break ;
			}
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
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
			geometry = geometry::trapdoor;
			transparent = true;
			redstoneComponant = true;
		}
		void getSecondaryHitbox( glm::vec3* hitbox, int orientation, int bitfield ) const override {
			bool open = !!(bitfield & door::open) ^ !!((bitfield << 12) & mask::redstone::powered);
			if (!open) {
				hitbox[0] = {0.5f, 0.5f, (bitfield & door::upper_half) ? 0.90625f : 0.09375f};
				hitbox[1] = {0.5f, 0.5f, 0.09375f};
			} else {
				switch (orientation) {
					case face_dir::minus_x:
						hitbox[0] = {0.90625f, 0.5f, 0.5f};
						hitbox[1] = {0.09375f, 0.5f, 0.5f};
						break ;
					case face_dir::plus_x:
						hitbox[0] = {0.09375f, 0.5f, 0.5f};
						hitbox[1] = {0.09375f, 0.5f, 0.5f};
						break ;
					case face_dir::minus_y:
						hitbox[0] = {0.5f, 0.90625f, 0.5f};
						hitbox[1] = {0.5f, 0.09375f, 0.5f};
						break ;
					case face_dir::plus_y:
						hitbox[0] = {0.5f, 0.09375f, 0.5f};
						hitbox[1] = {0.5f, 0.09375f, 0.5f};
						break ;
				}
			}
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
		void addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const override;
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
};

struct Crop : Block {
	public:
		Crop() {
			blast_resistance = 0.0f;
			hasHitbox = true;
			hasOrientedHitbox = true;
			collisionHitbox_1x1x1 = false;
			hitboxCenter = {0, 0, 100000}; // we discard normal hitbox
			geometry = geometry::crop;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			isSolidForFluid = false;
			item3D = false;
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
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
			geometry = geometry::button;
			transparent = true;
			isSolidForFluid = false;
			item3D = true;
		}
		void getSecondaryHitbox( glm::vec3* hitbox, int orientation, int bitfield ) const override {
			hitbox[0] = {0.5f, 0.5f, 1.5f * one16th};
			hitbox[1] = {0,    0,    1.5f * one16th};
			switch (bitfield & 0x3) {
				case placement::wall:
					switch (orientation) {
						case face_dir::minus_x:
							hitbox[0] = {14.5f * one16th, 0.5f,           0.5f};
							hitbox[1] = { 1.5f * one16th, 3.0f * one16th, 4.0f * one16th};
							break ;
						case face_dir::plus_x:
							hitbox[0] = { 1.5f * one16th, 0.5f,           0.5f};
							hitbox[1] = { 1.5f * one16th, 3.0f * one16th, 4.0f * one16th};
							break ;
						case face_dir::minus_y:
							hitbox[0] = {0.5f,           14.5f * one16th, 0.5f};
							hitbox[1] = {3.0f * one16th,  1.5f * one16th, 4.0f * one16th};
							break ;
						case face_dir::plus_y:
							hitbox[0] = {0.5f,           1.5f * one16th, 0.5f};
							hitbox[1] = {3.0f * one16th, 1.5f * one16th, 4.0f * one16th};
							break ;
					}
					break ;
				case placement::floor:
				case placement::ceiling:
					hitbox[0].z = ((bitfield & 0x3) == placement::floor) ? 1.5f * one16th : 14.5f * one16th;
					switch (orientation) {
						case face_dir::minus_x:
						case face_dir::plus_x:
							hitbox[1].x = 4.0f * one16th;
							hitbox[1].y = 3.0F * one16th;
							break ;
						case face_dir::minus_y:
						case face_dir::plus_y:
							hitbox[1].x = 3.0f * one16th;
							hitbox[1].y = 4.0f * one16th;
							break ;
					}
					break ;
			}
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
		void addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const override;
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
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
			geometry = geometry::none;
			transparent = true;
			isSolidForFluid = false;
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
			mined = blocks::dirt;
			blast_resistance = 0.6f;
			byHand = true;
			needed_tool = blocks::wooden_shovel;
			hardness = 0.6f;
			textureY = 1;
		}
		int texX( int dir, int value ) const override {
			(void)value;
			if (dir == face_dir::plus_z) {
				return (1);
			} else if (dir == face_dir::minus_z) {
				return (4);
			}
			return (0);
		}
};

struct OakLog : Cube {
	public:
		OakLog() {
			name = "OAK_LOG";
			mined = blocks::oak_log;
			isFuel = true;
			fuel_time = 15;
			isComposant = true;
			getProduction = blocks::charcoal;
			blast_resistance = 2.0f;
			byHand = true;
			needed_tool = blocks::wooden_axe;
			hardness = 2.0f;
			textureY = 2;
		}
		int texX( int dir, int value ) const override {
			int axis = (value >> offset::blocks::orientation) & 0x7;
			switch (dir) {
				case face_dir::minus_z:
				case face_dir::plus_z:
					return ((axis == face_dir::plus_z) + 2 * (axis == face_dir::plus_x));
				case face_dir::minus_x:
				case face_dir::plus_x:
					return ((axis == face_dir::plus_x) + 2 * (axis == face_dir::plus_y));
				case face_dir::minus_y:
				case face_dir::plus_y:
					return ((axis == face_dir::plus_y) + 2 * (axis == face_dir::plus_x));
			}
			return (0);
		}
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
};

struct Cactus : Cube {
	public:
		Cactus() {
			name = "CACTUS";
			mined = blocks::cactus;
			blast_resistance = 0.4f;
			collisionHitbox_1x1x1 = false;
			byHand = true;
			hardness = 0.4f;
			transparent = true;
			textureY = 3;
		}
		int texX( int dir, int value ) const override {
			(void)value;
			if (dir == face_dir::plus_z) {
				return (1);
			} else if (dir == face_dir::minus_z) {
				return (2);
			}
			return (0);
		}
};

struct Farmland : Block {
	public:
		Farmland() {
			name = "FARMLAND";
			mined = blocks::dirt;
			blast_resistance = 0.6f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, 7.5f / 16.0f};
			hitboxHalfSize = {0.5f, 0.5f, 7.5f / 16.0f};
			geometry = geometry::farmland;
			byHand = true;
			needed_tool = blocks::wooden_shovel;
			hardness = 0.6f;
			textureY = 1;
		}
		virtual int texX( int dir, int value ) const {
			if (dir == face_dir::plus_z) {
				return (2 + !(value & mask::farmland::wet));
			}
			return (4);
		}
		virtual void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
};

struct DirtPath : Farmland {
	public:
		DirtPath() {
			name = "DIRT PATH";
			mined = blocks::dirt;
			blast_resistance = 0.65f;
			hardness = 0.65f;
		}
		int texX( int dir, int value ) const override {
			(void)value;
			if (dir == face_dir::plus_z) {
				return (1);
			} else if (dir == face_dir::minus_z) {
				return (4);
			}
			return (0);
		}
		int texY( int dir, int value ) const override {
			(void)value;
			if (dir == face_dir::minus_z) {
				return (1);
			}
			return (5);
		}
};

struct TNT : Cube {
	public:
		TNT() {
			name = "TNT";
			mined = blocks::tnt;
			blast_resistance = 0.0f;
			byHand = true;
			hardness = 0.0f;
			redstoneComponant = true;
			textureY = 6;
		}
		int texX( int dir, int value ) const override {
			(void)value;
			if (dir == face_dir::plus_z) {
				return (1);
			} else if (dir == face_dir::minus_z) {
				return (2);
			}
			return (0);
		}
};

struct Target : Cube {
	public:
		Target() {
			name = "TARGET";
			mined = blocks::target;
			blast_resistance = 0.5f;
			byHand = true;
			needed_tool = blocks::wooden_hoe;
			hardness = 0.5f;
			textureY = 7;
		}
		int texX( int dir, int value ) const override {
			(void)value;
			return (dir == face_dir::plus_z || dir == face_dir::minus_z);
		}
};

struct CraftingTable : Cube {
	public:
		CraftingTable() {
			name = "CRAFTING_TABLE";
			mined = blocks::crafting_table;
			adventure_block = blocks::coal_ore;
			isFuel = true;
			fuel_time = 15;
			blast_resistance = 2.5f;
			oriented = true;
			byHand = true;
			needed_tool = blocks::wooden_axe;
			hardness = 2.5f;
			textureY = 8;
		}
		int texX( int dir, int value ) const override {
			if (dir == face_dir::plus_z || dir == face_dir::minus_z) {
				return (1);
			} else if (((value >> offset::blocks::orientation) & 0x7) == dir) {
				return (2);
			}
			return (0);
		}
};

struct Furnace : Cube {
	public:
		Furnace() {
			name = "FURNACE";
			mined = blocks::furnace;
			adventure_block = blocks::coal_ore;
			blast_resistance = 3.5f;
			oriented = true;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 3.5f;
			textureY = 9;
		}
		int texX( int dir, int value ) const override {
			if (dir == face_dir::plus_z || dir == face_dir::minus_z) {
				return (1);
			} else if (((value >> offset::blocks::orientation) & 0x7) == dir) {
				return (2 + ((value >> offset::redstone::activated) & 0x1));
			}
			return (0);
		}
};

struct OakStairsBottom : StairsBottom {
	public:
		OakStairsBottom() {
			name = "OAK_STAIRS_BOTTOM";
			mined = blocks::oak_stairs_bottom;
			blast_resistance = 3.0f;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::wooden_axe;
			hardness = 2.0f;
			textureX = 4;
			textureY = 10;
		}
};

struct OakStairsTop : StairsTop {
	public:
		OakStairsTop() {
			name = "OAK_STAIRS_TOP";
			mined = blocks::oak_stairs_bottom;
			blast_resistance = 3.0f;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::wooden_axe;
			hardness = 2.0f;
			textureX = 4;
			textureY = 10;
		}
};

struct OakDoor : Door {
	public:
		OakDoor() {
			name = "OAK_DOOR";
			mined = blocks::oak_door;
			blast_resistance = 3.0f;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::wooden_axe;
			hardness = 3.0f;
		}
		// offset is bool half or 2 if item
		int texX( int dir, int value ) const override {
			(void)dir;
			return ((value == blocks::item) ? 9 : 0);
		}
		int texY( int dir, int value ) const override {
			switch (dir) {
				case face_dir::plus_z:
					return (10);
				case face_dir::minus_z:
					return (11);
				default:
					return ((value == blocks::item) ? 14 : 11 - ((value >> offset::blocks::bitfield) & door::upper_half));
			}
		}
};

struct OakTrapdoor : Trapdoor {
	public:
		OakTrapdoor() {
			name = "OAK_TRAPDOOR";
			mined = blocks::oak_trapdoor;
			blast_resistance = 3.0f;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::wooden_axe;
			hardness = 3.0f;
			textureX = 2;
			textureY = 11;
		}
};

struct StoneStairsBottom : StairsBottom {
	public:
		StoneStairsBottom() {
			name = "STONE_STAIRS_BOTTOM";
			mined = blocks::stone_stairs_bottom;
			isComposant = true;
			getProduction = blocks::smooth_stone_stairs_bottom;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			textureX = 4;
			textureY = 3;
		}
};

struct StoneStairsTop : StairsTop {
	public:
		StoneStairsTop() {
			name = "STONE_STAIRS_TOP";
			mined = blocks::stone_stairs_bottom;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			textureX = 4;
			textureY = 3;
		}
};

struct SmoothStoneStairsBottom : StairsBottom {
	public:
		SmoothStoneStairsBottom() {
			name = "SMOOTH_STONE_STAIRS_BOTTOM";
			mined = blocks::smooth_stone_stairs_bottom;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			textureX = 4;
			textureY = 2;
		}
};

struct SmoothStoneStairsTop : StairsTop {
	public:
		SmoothStoneStairsTop() {
			name = "SMOOTH_STONE_STAIRS_TOP";
			mined = blocks::smooth_stone_stairs_bottom;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			textureX = 4;
			textureY = 2;
		}
};

struct CobbleStoneStairsBottom : StairsBottom {
	public:
		CobbleStoneStairsBottom() {
			name = "COBBLESTONE_STAIRS_BOTTOM";
			mined = blocks::cobblestone_stairs_bottom;
			isComposant = true;
			getProduction = blocks::stone_stairs_bottom;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			textureX = 4;
			textureY = 4;
		}
};

struct CobbleStoneStairsTop : StairsTop {
	public:
		CobbleStoneStairsTop() {
			name = "COBBLESTONE_STAIRS_TOP";
			mined = blocks::cobblestone_stairs_bottom;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			textureX = 4;
			textureY = 4;
		}
};

struct StoneBricksStairsBottom : StairsBottom {
	public:
		StoneBricksStairsBottom() {
			name = "STONE_BRICKS_STAIRS_BOTTOM";
			mined = blocks::stone_bricks_stairs_bottom;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			textureX = 4;
			textureY = 5;
		}
};

struct StoneBricksStairsTop : StairsTop {
	public:
		StoneBricksStairsTop() {
			name = "STONE_BRICKS_STAIRS_TOP";
			mined = blocks::stone_bricks_stairs_bottom;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			textureX = 4;
			textureY = 5;
		}
};

struct Lever : Block {
	public:
		Lever() {
			name = "LEVER";
			mined = blocks::lever;
			adventure_block = blocks::redstone_ore;
			blast_resistance = 0.5f;
			collisionHitbox_1x1x1 = false;
			hasHitbox = true;
			hasOrientedHitbox = true;
			collisionHitbox = true;
			oriented = true;
			orientedCollisionHitbox = true;
			hitboxCenter = {0, 0, 100000}; // we discard normal hitbox
			geometry = geometry::lever;
			byHand = true;
			hardness = 0.5f;
			transparent = true;
			isSolidForFluid = false;
			item3D = false;
			textureX = 7;
			textureY = 10;
		}
		void getSecondaryHitbox( glm::vec3* hitbox, int orientation, int bitfield ) const override {
			hitbox[0] = {0.5f, 0.5f, 1.5f * one16th};
			hitbox[1] = {0,    0,    1.5f * one16th};
			switch (bitfield & 0x3) {
				case placement::wall:
					switch (orientation) {
						case face_dir::minus_x:
							hitbox[0] = {14.5f * one16th, 0.5f,           0.5f};
							hitbox[1] = { 1.5f * one16th, 3.0f * one16th, 4.0f * one16th};
							break ;
						case face_dir::plus_x:
							hitbox[0] = { 1.5f * one16th, 0.5f,           0.5f};
							hitbox[1] = { 1.5f * one16th, 3.0f * one16th, 4.0f * one16th};
							break ;
						case face_dir::minus_y:
							hitbox[0] = {0.5f,           14.5f * one16th, 0.5f};
							hitbox[1] = {3.0f * one16th,  1.5f * one16th, 4.0f * one16th};
							break ;
						case face_dir::plus_y:
							hitbox[0] = {0.5f,           1.5f * one16th, 0.5f};
							hitbox[1] = {3.0f * one16th, 1.5f * one16th, 4.0f * one16th};
							break ;
					}
					break ;
				case placement::floor:
				case placement::ceiling:
					hitbox[0].z = ((bitfield & 0x3) == placement::floor) ? 1.5f * one16th : 14.5f * one16th;
					switch (orientation) {
						case face_dir::minus_x:
						case face_dir::plus_x:
							hitbox[1].x = 4.0f * one16th;
							hitbox[1].y = 3.0F * one16th;
							break ;
						case face_dir::minus_y:
						case face_dir::plus_y:
							hitbox[1].x = 3.0f * one16th;
							hitbox[1].y = 4.0f * one16th;
							break ;
					}
					break ;
			}
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
};

struct OakSign : Block {
	public:
		OakSign() {
			name = "OAK_SIGN";
			mined = blocks::oak_sign;
			blast_resistance = 1.0f;
			collisionHitbox_1x1x1 = false;
			hasHitbox = true;
			hasOrientedHitbox = true;
			hitboxCenter = {0, 0, 100000}; // we discard normal hitbox
			geometry = geometry::none;
			byHand = true;
			needed_tool = blocks::wooden_axe;
			hardness = 1.0f;
			transparent = true;
			item3D = false;
			textureX = 1;
			textureY = 11;
		}
		void getSecondaryHitbox( glm::vec3* hitbox, int orientation, int bitfield ) const override {
			(void)bitfield;
			switch (orientation) {
				case face_dir::minus_x:
					hitbox[0] = {1.75f * one16th, 0.5f, 0.5f};
					hitbox[1] = {1.75f * one16th, 0.5f, 0.25f};
					break ;
				case face_dir::plus_x:
					hitbox[0] = {14.25f * one16th, 0.5f, 0.5f};
					hitbox[1] = {1.75f * one16th, 0.5f, 0.25f};
					break ;
				case face_dir::minus_y:
					hitbox[0] = {0.5f, 1.75f * one16th, 0.5f};
					hitbox[1] = {0.5f, 1.75f * one16th, 0.25f};
					break ;
				case face_dir::plus_y:
					hitbox[0] = {0.5f, 14.25f * one16th, 0.5f};
					hitbox[1] = {0.5f, 1.75f * one16th, 0.25f};
					break ;
			}
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
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
			mined = blocks::dirt;
			blast_resistance = 0.5f;
			byHand = true;
			needed_tool = blocks::wooden_shovel;
			hardness = 0.5f;
			textureX = 4;
			textureY = 1;
		}
};

struct SmoothStone : Cube {
	public:
		SmoothStone() {
			name = "SMOOTH_STONE";
			mined = blocks::smooth_stone;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 2.0f;
			textureX = 4;
			textureY = 2;
		}
};

struct Stone : Cube {
	public:
		Stone() {
			name = "STONE";
			mined = blocks::cobblestone;
			isComposant = true;
			getProduction = blocks::smooth_stone;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			textureX = 4;
			textureY = 3;
		}
};

struct Cobblestone : Cube {
	public:
		Cobblestone() {
			name = "COBBLESTONE";
			mined = blocks::cobblestone;
			isComposant = true;
			getProduction = blocks::stone;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 2.0f;
			textureX = 4;
			textureY = 4;
		}
};

struct StoneBrick : Cube {
	public:
		StoneBrick() {
			name = "STONE_BRICK";
			mined = blocks::stone_bricks;
			isComposant = true;
			getProduction = blocks::cracked_stone_bricks;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			textureX = 4;
			textureY = 5;
		}
};

struct CrackedStoneBrick : Cube {
	public:
		CrackedStoneBrick() {
			name = "CRACKED_STONE_BRICK";
			mined = blocks::cracked_stone_bricks;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			textureX = 4;
			textureY = 6;
		}
};

struct Sand : Cube {
	public:
		Sand() {
			name = "SAND";
			mined = blocks::sand;
			isComposant = true;
			getProduction = blocks::glass;
			blast_resistance = 0.5f;
			byHand = true;
			needed_tool = blocks::wooden_shovel;
			hardness = 0.5f;
			textureX = 4;
			textureY = 7;
		}
};

struct Gravel : Cube {
	public:
		Gravel() {
			name = "GRAVEL";
			mined = blocks::gravel;
			blast_resistance = 0.6f;
			byHand = true;
			needed_tool = blocks::wooden_shovel;
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
			mined = blocks::oak_planks;
			isFuel = true;
			fuel_time = 15;
			blast_resistance = 3.0f;
			byHand = true;
			needed_tool = blocks::wooden_axe;
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
			geometry = geometry::glass;
			transparent = true;
			textureX = 4;
			textureY = 11;
		}
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
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
			geometry = geometry::glass_pane;
			hardness = 0.3f;
			transparent = true;
			item3D = false;
			textureY = 11;
		}
		int texX( int dir, int offset ) const override {
			(void)dir;
			switch (offset) {
				case fence::base:
					return (4);
				case fence::arm:
					return (5);
				case fence::arm_end:
					return (3);
			}
			return (4);
		}
		void getSecondaryHitbox( glm::vec3* hitbox, int orientation, int bitfield ) const override {
			(void)orientation;
			hitbox[0] = {0.5f, 0.5f, 0.5f};
			hitbox[1] = {0.0625, 0.0625, 0.5f};
			switch (bitfield & (fence::mx | fence::px)) {
				case fence::mx:
					hitbox[0].x = 0.28125f;
					hitbox[1].x = 0.28125f;
					break ;
				case fence::px:
					hitbox[0].x = 1 - 0.28125f;
					hitbox[1].x = 0.28125f;
					break ;
				case fence::mx | fence::px:
					hitbox[1].x = 0.5f;
					break ;
			}
			switch (bitfield & (fence::my | fence::py)) {
				case fence::my:
					hitbox[0].y = 0.28125f;
					hitbox[1].y = 0.28125f;
					break ;
				case fence::py:
					hitbox[0].y = 1 - 0.28125f;
					hitbox[1].y = 0.28125f;
					break ;
				case fence::my | fence::py:
					hitbox[1].y = 0.5f;
					break ;
			}
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
};

struct RedstoneLamp : Cube {
	public:
		RedstoneLamp() {
			name = "REDSTONE_LAMP";
			mined = blocks::redstone_lamp;
			light_level = 15;
			isFuel = true;
			fuel_time = 15;
			blast_resistance = 0.3f;
			byHand = true;
			hardness = 0.3f;
			redstoneComponant = true;
			textureX = 5;
		}
		int texY( int dir, int value ) const override {
			(void)dir;
			return ((value & mask::redstone::activated) ? 9 : 8);
		}
};

struct StoneButton : Button {
	public:
		StoneButton() {
			name = "STONE_BUTTON";
			mined = blocks::stone_button;
			adventure_block = blocks::redstone_ore;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			textureX = 4;
			textureY = 3;
		}
};

struct OakButton : Button {
	public:
		OakButton() {
			name = "OAK_BUTTON";
			mined = blocks::oak_button;
			adventure_block = blocks::redstone_ore;
			blast_resistance = 0.5f;
			byHand = true;
			needed_tool = blocks::wooden_axe;
			hardness = 0.5f;
			textureX = 4;
			textureY = 10;
		}
};

struct CoalOre : Cube {
	public:
		CoalOre() {
			name = "COAL_ORE";
			mined = blocks::coal;
			isComposant = true;
			getProduction = blocks::coal;
			blast_resistance = 3.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 3.0f;
			textureX = 5;
			textureY = 0;
		}
};

struct IronOre : Cube {
	public:
		IronOre() {
			name = "IRON_ORE";
			mined = blocks::iron_ore;
			isComposant = true;
			getProduction = blocks::iron_ingot;
			blast_resistance = 3.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
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
			mined = blocks::diamond;
			isComposant = true;
			getProduction = blocks::diamond;
			blast_resistance = 3.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
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
			mined = blocks::coal_block;
			isFuel = true;
			fuel_time = 800;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 5.0f;
			textureX = 5;
			textureY = 3;
		}
};

struct IronBlock : Cube {
	public:
		IronBlock() {
			name = "IRON_BLOCK";
			mined = blocks::iron_block;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
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
			mined = blocks::diamond_block;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
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
			mined = blocks::redstone_dust; // TODO 4 or 5
			isComposant = true;
			getProduction = blocks::redstone_dust;
			blast_resistance = 3.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
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
			mined = blocks::redstone_block;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 5.0f;
			textureX = 5;
			textureY = 7;
		}
};

struct OakSlabBottom : SlabBottom {
	public:
		OakSlabBottom() {
			name = "OAK_SLAB_BOTTOM";
			mined = blocks::oak_slab_bottom;
			blast_resistance = 3.0f;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::wooden_axe;
			hardness = 2.0f;
			textureX = 4;
			textureY = 10;
		}
};

struct OakSlabTop : SlabTop {
	public:
		OakSlabTop() {
			name = "OAK_SLAB_TOP";
			mined = blocks::oak_slab_bottom;
			blast_resistance = 3.0f;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::wooden_axe;
			hardness = 2.0f;
			textureX = 4;
			textureY = 10;
		}
};

struct OakFence : Fence {
	public:
		OakFence() {
			name = "OAK_FENCE";
			mined = blocks::oak_fence;
			blast_resistance = 3.0f;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::wooden_axe;
			hardness = 2.0f;
			textureX = 4;
			textureY = 10;
		}
};

struct StoneSlabBottom : SlabBottom {
	public:
		StoneSlabBottom() {
			name = "STONE_SLAB_BOTTOM";
			mined = blocks::stone_slab_bottom;
			isComposant = true;
			getProduction = blocks::smooth_stone_slab_bottom;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			textureX = 4;
			textureY = 3;
		}
};

struct StoneSlabTop : SlabTop {
	public:
		StoneSlabTop() {
			name = "STONE_SLAB_TOP";
			mined = blocks::stone_slab_bottom;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 2.0f;
			textureX = 4;
			textureY = 3;
		}
};

struct SmoothStoneSlabBottom : SlabBottom {
	public:
		SmoothStoneSlabBottom() {
			name = "SMOOTH_STONE_SLAB_BOTTOM";
			mined = blocks::smooth_stone_slab_bottom;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			textureX = 4;
			textureY = 2;
		}
};

struct SmoothStoneSlabTop : SlabTop {
	public:
		SmoothStoneSlabTop() {
			name = "SMOOTH_STONE_SLAB_TOP";
			mined = blocks::smooth_stone_slab_bottom;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 2.0f;
			textureX = 4;
			textureY = 2;
		}
};

struct CobbleStoneSlabBottom : SlabBottom {
	public:
		CobbleStoneSlabBottom() {
			name = "COBBLESTONE_SLAB_BOTTOM";
			mined = blocks::cobblestone_slab_bottom;
			isComposant = true;
			getProduction = blocks::stone_slab_bottom;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			textureX = 4;
			textureY = 4;
		}
};

struct CobbleStoneSlabTop : SlabTop {
	public:
		CobbleStoneSlabTop() {
			name = "COBBLESTONE_SLAB_TOP";
			mined = blocks::cobblestone_slab_bottom;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 2.0f;
			textureX = 4;
			textureY = 4;
		}
};

struct StoneBricksSlabBottom : SlabBottom {
	public:
		StoneBricksSlabBottom() {
			name = "STONE_BRICKS_SLAB_BOTTOM";
			mined = blocks::stone_bricks_slab_bottom;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			textureX = 4;
			textureY = 5;
		}
};

struct StoneBricksSlabTop : SlabTop {
	public:
		StoneBricksSlabTop() {
			name = "STONE_BRICKS_SLAB_TOP";
			mined = blocks::stone_bricks_slab_bottom;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 2.0f;
			textureX = 4;
			textureY = 5;
		}
};
int opposite_dir( int dir );
struct Piston : Block {
	public:
		Piston() {
			name = "PISTON";
			mined = blocks::piston;
			blast_resistance = 0.5f;
			geometry = geometry::piston;
			byHand = true;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			transparent = true;
			oriented = true;
			textureY = 13;
		}
		int texX( int dir = face_dir::minus_y, int value = 0 ) const override {
			int orientation = (value >> offset::blocks::orientation) & 0x7;
			if (dir == orientation) {
				return ((value & mask::redstone::piston::moving) ? 2 : 3 + (mined == blocks::sticky_piston));
			}
			if (dir == opposite_dir(orientation)) {
				return (1);
			}
			return (0);
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
		void addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const override;
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
};

struct StickyPiston : Piston {
	public:
		StickyPiston() {
			name = "STICKY_PISTON";
			mined = blocks::sticky_piston;
		}
};

struct PistonHead : Block {
	public:
		PistonHead() {
			name = "PISTON_HEAD";
			blast_resistance = 0.5f;
			geometry = geometry::cross;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			transparent = true;
			oriented = true;
			textureX = 4;
			textureY = 5;
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
};

struct MovingPiston : Block {
	public:
		MovingPiston() {
			name = "MOVING_PISTON";
			geometry = geometry::piston;
		}
		bool isTransparent( int value ) const override {
			return (value & mask::redstone::piston::retracting);
		}
};

struct Observer : Block {
	public:
		Observer() {
			name = "OBSERVER";
			mined = blocks::observer;
			blast_resistance = 3.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 3.0f;
			transparent = true;
			oriented = true;
			textureY = 12;
		}
		int texX( int dir = face_dir::minus_y, int value = 0 ) const override {
			int orientation = (value >> offset::blocks::orientation) & 0x7;
			if (dir == orientation) {
				return (2);
			}
			if (dir == opposite_dir(orientation)) {
				return ((value & mask::redstone::activated) ? 4 : 3);
			}
			if (orientation & 0x4) {
				return (!(dir & 0x2));
			}
			return (!!(dir & 0x4));
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
};

struct Poppy : Cross {
	public:
		Poppy() {
			name = "POPPY";
			mined = blocks::poppy;
			textureX = 6;
			textureY = 0;
		}
};

struct Dandelion : Cross {
	public:
		Dandelion() {
			name = "DANDELION";
			mined = blocks::dandelion;
			textureX = 6;
			textureY = 1;
		}
};

struct BlueOrchid : Cross {
	public:
		BlueOrchid() {
			name = "BLUE_ORCHID";
			mined = blocks::blue_orchid;
			textureX = 6;
			textureY = 2;
		}
};

struct Allium : Cross {
	public:
		Allium() {
			name = "ALLIUM";
			mined = blocks::allium;
			textureX = 6;
			textureY = 3;
		}
};

struct CornFlower : Cross {
	public:
		CornFlower() {
			name = "CORNFLOWER";
			mined = blocks::cornflower;
			textureX = 6;
			textureY = 4;
		}
};

struct PinkTulip : Cross {
	public:
		PinkTulip() {
			name = "PINK_TULIP";
			mined = blocks::pink_tulip;
			textureX = 6;
			textureY = 5;
		}
};

struct Grass : Cross {
	public:
		Grass() {
			name = "GRASS";
			mined = blocks::wheat_seeds;
			hasHitbox = false;
			textureX = 6;
			textureY = 6;
		}
};

struct SugarCane : Cross {
	public:
		SugarCane() {
			name = "SUGAR_CANE";
			mined = blocks::sugar_cane;
			hasHitbox = false;
			textureX = 6;
			textureY = 7;
		}
};

struct DeadBush : Cross {
	public:
		DeadBush() {
			name = "DEAD_BUSH";
			mined = blocks::stick;
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
			mined = blocks::oak_sapling;
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
			mined = blocks::torch;
			blast_resistance = 0.0f;
			hasHitbox = true;
			hasOrientedHitbox = true;
			collisionHitbox_1x1x1 = false;
			geometry = geometry::torch;
			light_level = 14;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			isSolidForFluid = false;
			item3D = false;
			textureX = 6;
			textureY = 10;
		}
		void getSecondaryHitbox( glm::vec3* hitbox, int orientation, int bitfield ) const override {
			(void)bitfield;
			switch (orientation) {
				case face_dir::minus_x:
					hitbox[0] = {2 * one16th, 0.5f, 8 * one16th};
					hitbox[1] = {2 * one16th, one16th, 5 * one16th};
					break ;
				case face_dir::plus_x:
					hitbox[0] = {14 * one16th, 0.5f, 8 * one16th};
					hitbox[1] = {2 * one16th, one16th, 5 * one16th};
					break ;
				case face_dir::minus_y:
					hitbox[0] = {0.5f, 2 * one16th, 8 * one16th};
					hitbox[1] = {one16th, 2 * one16th, 5 * one16th};
					break ;
				case face_dir::plus_y:
					hitbox[0] = {0.5f, 14 * one16th, 8 * one16th};
					hitbox[1] = {one16th, 2 * one16th, 5 * one16th};
					break ;
				default:
					hitbox[0] = {0.5f, 0.5f, 5 * one16th};
					hitbox[1] = {one16th, one16th, 5 * one16th};
					break ;
			}
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
};

struct RedstoneTorch : Torch {
	public:
		RedstoneTorch() {
			name = "REDSTONE_TORCH";
			mined = blocks::redstone_torch;
			light_level = 7;
			textureX = 7;
			textureY = 11;
		}
		int texY( int dir = face_dir::minus_y, int offset = 0 ) const override {
			(void)dir;
			return ((offset == blocks::item) ? 11 : 11 + offset);
		}
};

struct RedstoneDust : Block {
	public:
		RedstoneDust() {
			name = "REDSTONE_DUST";
			mined = blocks::redstone_dust;
			blast_resistance = 0.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			hitboxCenter = {0.5f, 0.5f, one16th};
			hitboxHalfSize = {0.2f, 0.2f, one16th};
			geometry = geometry::dust;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			isSolidForFluid = false;
			item3D = false;
		}
		int texX( int dir = face_dir::minus_y, int value = 0 ) const override {
			(void)dir;
			return ((value == blocks::item) ? 7 : 6);
		}
		int texY( int dir = face_dir::minus_y, int value = 0 ) const override {
			(void)dir;
			return ((value == blocks::item) ? 8 : 12);
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
};

struct Repeater : Block {
	public:
		Repeater() {
			name = "REPEATER";
			mined = blocks::repeater;
			adventure_block = blocks::iron_ore;
			blast_resistance = 0.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, one16th};
			hitboxHalfSize = {0.5f, 0.5f, one16th};
			oriented = true;
			geometry = geometry::repeater;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			isSolidForFluid = false;
			item3D = false;
			textureY = 13;
		}
		int texX( int dir = face_dir::minus_y, int value = 0 ) const override {
			(void)dir;
			return ((value == blocks::item) ? 7 : 6 - value);
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
};

struct Comparator : Block {
	public:
		Comparator() {
			name = "COMPARATOR";
			mined = blocks::comparator;
			adventure_block = blocks::iron_ore;
			blast_resistance = 0.0f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, one16th};
			hitboxHalfSize = {0.5f, 0.5f, one16th};
			oriented = true;
			geometry = geometry::repeater;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			isSolidForFluid = false;
			item3D = false;
			textureY = 14;
		}
		int texX( int dir = face_dir::minus_y, int value = 0 ) const override {
			(void)dir;
			return ((value == blocks::item) ? 7 : 6 - value);
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
};

struct Chest : Block {
	public:
		Chest() {
			name = "CHEST";
			mined = blocks::chest;
			blast_resistance = 2.5f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, 7.5 / 16.0f};
			hitboxHalfSize = {7 / 16.0f, 7 / 16.0f, 7.5 / 16.0f};
			geometry = geometry::none;
			byHand = true;
			needed_tool = blocks::wooden_axe;
			hardness = 2.5f;
			transparent = true;
			textureX = 0;
			textureY = 15;
		}
		void addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const override;
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
};

struct WheatCrop : Crop {
	public:
		WheatCrop() {
			name = "WHEAT_CROP";
			mined = blocks::wheat_seeds;
			textureX = 7;
		}
		int texY( int dir = face_dir::minus_y, int value = 0 ) const override {
			(void)dir;
			return (value >> offset::blocks::bitfield);
		}
		void getSecondaryHitbox( glm::vec3* hitbox, int orientation, int bitfield ) const override {
			(void)orientation;
			hitbox[0] = {0.5f, 0.5f, (1 + bitfield * 2) / 32.0f};
			hitbox[1] = {0.4f, 0.4f, (1 + bitfield * 2) / 32.0f};
		}
};

struct ItemFrame : Block {
	public:
		ItemFrame() {
			name = "ITEM_FRAME";
			mined = blocks::item_frame;
			blast_resistance = 0.0f;
			hasHitbox = true;
			hasOrientedHitbox = true;
			collisionHitbox_1x1x1 = false;
			oriented = true;
			hitboxCenter = {0, 0, 100000}; // we discard normal hitbox
			geometry = geometry::none;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			isSolidForFluid = false;
			item3D = false;
			textureY = 14;
		}
		int texX( int dir = face_dir::minus_y, int value = 0 ) const override {
			(void)dir;
			return ((value == blocks::item) ? 10 : 4);
		}
		void getSecondaryHitbox( glm::vec3* hitbox, int orientation, int bitfield ) const override {
			switch (bitfield & 0x3) {
				case placement::wall:
					switch (orientation) {
						case face_dir::minus_x:
							hitbox[0] = {15.5f * one16th, .4f,           .4f};
							hitbox[1] = {  .5f * one16th, .4f, 4.0f * one16th};
							break ;
						case face_dir::plus_x:
							hitbox[0] = { .5f * one16th, .4f,           .4f};
							hitbox[1] = { .5f * one16th, .4f, 4.0f * one16th};
							break ;
						case face_dir::minus_y:
							hitbox[0] = {.4f, 15.5f * one16th, .4f};
							hitbox[1] = {.4f,   .5f * one16th, 4.0f * one16th};
							break ;
						case face_dir::plus_y:
							hitbox[0] = {.4f, .5f * one16th, .4f};
							hitbox[1] = {.4f, .5f * one16th, 4.0f * one16th};
							break ;
					}
					break ;
				case placement::floor:
				case placement::ceiling:
					hitbox[0] = {.4f, .4f, ((bitfield & 0x3) == placement::floor) ? .5f * one16th : 15.5f * one16th};
					hitbox[1] = {.4f, .4f, one16th};
					break ;
			}
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
		void addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const override;
};

struct BirchPlanks : Cube {
	public:
		BirchPlanks() {
			name = "BIRCH_PLANKS";
			mined = blocks::birch_planks;
			isFuel = true;
			fuel_time = 15;
			blast_resistance = 3.0f;
			byHand = true;
			needed_tool = blocks::wooden_axe;
			hardness = 2.0f;
			textureX = 5;
			textureY = 10;
		}
};

struct SandStone : Cube {
	public:
		SandStone() {
			name = "SAND_STONE";
			mined = blocks::sand_stone;
			blast_resistance = .8f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = .8f;
			textureX = 3;
		}
		int texY( int dir, int value ) const override {
			(void)value;
			return ((dir == face_dir::plus_z) ? 6 : (dir == face_dir::minus_z) ? 8 : 7);
		}
};

struct Water : Block {
	public:
		Water() {
			name = "WATER";
			mined = blocks::water_bucket;
			blast_resistance = 100.0f;
			collisionHitbox_1x1x1 = false;
			geometry = geometry::none;
			transparent = true;
		}
};

struct Stick : Block {
	public:
		Stick() {
			name = "STICK";
			isFuel = true;
			fuel_time = 5;
			isItemOnly = true;
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
			isItemOnly = true;
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
			isItemOnly = true;
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
			isItemOnly = true;
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
			isItemOnly = true;
			item3D = false;
			textureX = 8;
			textureY = 4;
		}
};

struct WoodenAxe : Block {
	public:
		WoodenAxe() {
			name = "wooden_axe";
			stackable = false;
			isFuel = true;
			fuel_time = 10;
			durability = 59;
			isItemOnly = true;
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
			isItemOnly = true;
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
			isItemOnly = true;
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
			isItemOnly = true;
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
			isItemOnly = true;
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
			isItemOnly = true;
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
			isItemOnly = true;
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
			isItemOnly = true;
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
			isItemOnly = true;
			item3D = false;
			textureX = 8;
			textureY = 13;
		}
};

struct Arrow : Block {
	public:
		Arrow() {
			name = "ARROW";
			isItemOnly = true;
			item3D = false;
			textureX = 8;
			textureY = 14;
		}
};

struct WorldEditWand : Block {
	public:
		WorldEditWand() {
			name = "WORLDEDIT_WAND";
			isItemOnly = true;
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
			isItemOnly = true;
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
			isItemOnly = true;
			item3D = false;
			textureX = 9;
			textureY = 1;
		}
};

struct IronIngot : Block {
	public:
		IronIngot() {
			name = "IRON_INGOT";
			isItemOnly = true;
			item3D = false;
			textureX = 9;
			textureY = 2;
		}
};

struct Diamond : Block {
	public:
		Diamond() {
			name = "DIAMOND";
			isItemOnly = true;
			item3D = false;
			textureX = 9;
			textureY = 3;
		}
};

struct Bucket : Block {
	public:
		Bucket() {
			name = "BUCKET";
			adventure_block = blocks::diamond_ore;
			isItemOnly = true;
			item3D = false;
			textureX = 9;
			textureY = 4;
		}
};

struct WaterBucket : Block {
	public:
		WaterBucket() {
			name = "WATER_BUCKET";
			adventure_block = blocks::diamond_ore;
			stackable = false;
			isItemOnly = true;
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
			isItemOnly = true;
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
			isItemOnly = true;
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
			isItemOnly = true;
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
			isItemOnly = true;
			item3D = false;
			textureX = 9;
			textureY = 9;
		}
};

struct WheatSeeds : Block {
	public:
		WheatSeeds() {
			name = "WHEAT_SEEDS";
			adventure_block = blocks::farmland;
			isItemOnly = true;
			item3D = false;
			textureX = 9;
			textureY = 10;
		}
};

struct Wheat : Block {
	public:
		Wheat() {
			name = "WHEAT";
			isItemOnly = true;
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
			isItemOnly = true;
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
			isItemOnly = true;
			item3D = false;
			textureX = 9;
			textureY = 13;
		}
};

struct Flint : Block {
	public:
		Flint() {
			name = "FLINT";
			isItemOnly = true;
			item3D = false;
			textureX = 10;
			textureY = 0;
		}
};

struct FlintAndSteel : Block {
	public:
		FlintAndSteel() {
			name = "FLINT AND STEEL";
			isItemOnly = true;
			item3D = false;
			textureX = 10;
			textureY = 1;
		}
};

struct ZombieEgg : Block {
	public:
		ZombieEgg() {
			name = "ZOMBIE_EGG";
			isItemOnly = true;
			item3D = false;
			textureX = 10;
			textureY = 2;
		}
};

struct SkeletonEgg : Block {
	public:
		SkeletonEgg() {
			name = "SKELETON_EGG";
			isItemOnly = true;
			item3D = false;
			textureX = 10;
			textureY = 3;
		}
};

struct Wool : Cube {
	public:
		Wool() {
			blast_resistance = .8f;
			byHand = true;
			// needed_tool = blocks::shear;
			hardness = .8f;
			textureX = 11;
		}
};

struct WhiteWool : Wool {
	public:
		WhiteWool() {
			name = "WHITE_WOOL";
			mined = blocks::white_wool;
			textureY = 0;
		}
};

struct PinkWool : Wool {
	public:
		PinkWool() {
			name = "PINK_WOOL";
			mined = blocks::pink_wool;
			textureY = 1;
		}
};

struct MagentaWool : Wool {
	public:
		MagentaWool() {
			name = "MAGENTA_WOOL";
			mined = blocks::magenta_wool;
			textureY = 2;
		}
};

struct PurpleWool : Wool {
	public:
		PurpleWool() {
			name = "PURPLE_WOOL";
			mined = blocks::purple_wool;
			textureY = 3;
		}
};

struct BlueWool : Wool {
	public:
		BlueWool() {
			name = "BLUE_WOOL";
			mined = blocks::blue_wool;
			textureY = 4;
		}
};

struct LightBlueWool : Wool {
	public:
		LightBlueWool() {
			name = "LIGHT_BLUE_WOOL";
			mined = blocks::light_blue_wool;
			textureY = 5;
		}
};

struct CyanWool : Wool {
	public:
		CyanWool() {
			name = "CYAN_WOOL";
			mined = blocks::cyan_wool;
			textureY = 6;
		}
};

struct GreenWool : Wool {
	public:
		GreenWool() {
			name = "GREEN_WOOL";
			mined = blocks::green_wool;
			textureY = 7;
		}
};

struct LimeWool : Wool {
	public:
		LimeWool() {
			name = "LIME_WOOL";
			mined = blocks::lime_wool;
			textureY = 8;
		}
};

struct YellowWool : Wool {
	public:
		YellowWool() {
			name = "YELLOW_WOOL";
			mined = blocks::yellow_wool;
			textureY = 9;
		}
};

struct OrangeWool : Wool {
	public:
		OrangeWool() {
			name = "ORANGE_WOOL";
			mined = blocks::orange_wool;
			textureY = 10;
		}
};

struct RedWool : Wool {
	public:
		RedWool() {
			name = "RED_WOOL";
			mined = blocks::red_wool;
			textureY = 11;
		}
};

struct BrownWool : Wool {
	public:
		BrownWool() {
			name = "BROWN_WOOL";
			mined = blocks::brown_wool;
			textureY = 12;
		}
};

struct BlackWool : Wool {
	public:
		BlackWool() {
			name = "BLACK_WOOL";
			mined = blocks::black_wool;
			textureY = 13;
		}
};

struct GrayWool : Wool {
	public:
		GrayWool() {
			name = "GRAY_WOOL";
			mined = blocks::gray_wool;
			textureY = 14;
		}
};

struct LightGrayWool : Wool {
	public:
		LightGrayWool() {
			name = "LIGHT_GRAY_WOOL";
			mined = blocks::light_gray_wool;
			textureY = 15;
		}
};

struct Carpet : Block {
	public:
		Carpet() {
			blast_resistance = .1f;
			hasHitbox = true;
			collisionHitbox_1x1x1 = false;
			collisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, .5f * one16th};
			hitboxHalfSize = {0.5f, 0.5f, .5f * one16th};
			geometry = geometry::repeater;
			byHand = true;
			hardness = .1f;
			transparent = true;
			isSolidForFluid = false;
			textureX = 11;
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
		void addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const override;
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
};

struct WhiteCarpet : Carpet {
	public:
		WhiteCarpet() {
			name = "WHITE_CARPET";
			mined = blocks::white_carpet;
			textureY = 0;
		}
};

struct PinkCarpet : Carpet {
	public:
		PinkCarpet() {
			name = "PINK_CARPET";
			mined = blocks::pink_carpet;
			textureY = 1;
		}
};

struct MagentaCarpet : Carpet {
	public:
		MagentaCarpet() {
			name = "MAGENTA_CARPET";
			mined = blocks::magenta_carpet;
			textureY = 2;
		}
};

struct PurpleCarpet : Carpet {
	public:
		PurpleCarpet() {
			name = "PURPLE_CARPET";
			mined = blocks::purple_carpet;
			textureY = 3;
		}
};

struct BlueCarpet : Carpet {
	public:
		BlueCarpet() {
			name = "BLUE_CARPET";
			mined = blocks::blue_carpet;
			textureY = 4;
		}
};

struct LightBlueCarpet : Carpet {
	public:
		LightBlueCarpet() {
			name = "LIGHT_BLUE_CARPET";
			mined = blocks::light_blue_carpet;
			textureY = 5;
		}
};

struct CyanCarpet : Carpet {
	public:
		CyanCarpet() {
			name = "CYAN_CARPET";
			mined = blocks::cyan_carpet;
			textureY = 6;
		}
};

struct GreenCarpet : Carpet {
	public:
		GreenCarpet() {
			name = "GREEN_CARPET";
			mined = blocks::green_carpet;
			textureY = 7;
		}
};

struct LimeCarpet : Carpet {
	public:
		LimeCarpet() {
			name = "LIME_CARPET";
			mined = blocks::lime_carpet;
			textureY = 8;
		}
};

struct YellowCarpet : Carpet {
	public:
		YellowCarpet() {
			name = "YELLOW_CARPET";
			mined = blocks::yellow_carpet;
			textureY = 9;
		}
};

struct OrangeCarpet : Carpet {
	public:
		OrangeCarpet() {
			name = "ORANGE_CARPET";
			mined = blocks::orange_carpet;
			textureY = 10;
		}
};

struct RedCarpet : Carpet {
	public:
		RedCarpet() {
			name = "RED_CARPET";
			mined = blocks::red_carpet;
			textureY = 11;
		}
};

struct BrownCarpet : Carpet {
	public:
		BrownCarpet() {
			name = "BROWN_CARPET";
			mined = blocks::brown_carpet;
			textureY = 12;
		}
};

struct BlackCarpet : Carpet {
	public:
		BlackCarpet() {
			name = "BLACK_CARPET";
			mined = blocks::black_carpet;
			textureY = 13;
		}
};

struct GrayCarpet : Carpet {
	public:
		GrayCarpet() {
			name = "GRAY_CARPET";
			mined = blocks::gray_carpet;
			textureY = 14;
		}
};

struct LightGrayCarpet : Carpet {
	public:
		LightGrayCarpet() {
			name = "LIGHT_GRAY_CARPET";
			mined = blocks::light_gray_carpet;
			textureY = 15;
		}
};

extern const std::array<std::unique_ptr<Block>, S_BLOCKS_SIZE> s_blocks;

#endif
