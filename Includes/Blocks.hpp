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

# define S_BLOCKS_SIZE 184

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

namespace settings {
	namespace consts {
		namespace shader {
			namespace block {
				enum {
					tex0, tex1, tex2, tex3, tex4, tex5, tex6, tex7, tex8, tex9, tex10, tex11, tex12, tex13, tex14, tex15,
					allium,
					arrow,
					bedrock,
					birch_planks,
					black_wool,
					blue_orchid,
					blue_wool,
					bookshelf,
					brown_wool,
					cactus_bottom,
					cactus_side,
					cactus_top,
					chest_front,
					chest_inner_bottom,
					chest_inner_top,
					chest_side,
					chest_top,
					chiseled_quartz_block,
					chiseled_quartz_block_top,
					coal_block,
					coal_ore,
					cobblestone,
					comparator,
					comparator_on,
					cornflower,
					cracked_stone_bricks,
					crafting_table_front,
					crafting_table_side,
					crafting_table_top,
					cyan_wool,
					dandelion,
					dead_bush,
					debug,
					destroy_stage,
					destroy_stage_0,
					destroy_stage_1,
					destroy_stage_2,
					destroy_stage_3,
					destroy_stage_4,
					destroy_stage_5,
					destroy_stage_6,
					destroy_stage_7,
					destroy_stage_8,
					destroy_stage_9,
					diamond_block,
					diamond_ore,
					dirt,
					dirt_path_side,
					dirt_path_top,
					dust_dot,
					dust_line,
					farmland,
					farmland_moist,
					furnace_front,
					furnace_front_on,
					furnace_side,
					furnace_top,
					glass,
					glass_pane_top,
					glowstone,
					grass,
					grass_block_side,
					grass_block_top,
					gravel,
					gray_wool,
					green_wool,
					iron_block,
					iron_ore,
					item_frame,
					lectern_base,
					lectern_front,
					lectern_sides,
					lectern_top,
					lever,
					light_blue_wool,
					light_gray_wool,
					lime_wool,
					magenta_wool,
					netherrack,
					oak_door_bottom,
					oak_door_top,
					oak_leaves,
					oak_log,
					oak_log_spin,
					oak_log_top,
					oak_planks,
					oak_sapling,
					oak_sign,
					oak_trapdoor,
					observer_back,
					observer_back_on,
					observer_front,
					observer_side,
					observer_top,
					obsidian,
					orange_wool,
					pink_tulip,
					pink_wool,
					piston_bottom,
					piston_inner,
					piston_side,
					piston_top,
					piston_top_sticky,
					poppy,
					purple_wool,
					quartz_block,
					quartz_bricks,
					quartz_ore,
					quartz_pillar,
					quartz_pillar_top,
					red_wool,
					redstone_block,
					redstone_lamp,
					redstone_lamp_on,
					redstone_ore,
					redstone_torch,
					redstone_torch_off,
					repeater,
					repeater_on,
					sand,
					sandstone,
					sandstone_bottom,
					sandstone_top,
					smooth_stone,
					smooth_quartz_block,
					stone,
					stone_bricks,
					sugar_cane,
					target_side,
					target_top,
					tnt_bottom,
					tnt_side,
					tnt_top,
					torch,
					wheat_stage0,
					wheat_stage1,
					wheat_stage2,
					wheat_stage3,
					wheat_stage4,
					wheat_stage5,
					wheat_stage6,
					wheat_stage7,
					white_tulip,
					white_wool,
					yellow_wool,
					last
				};
			};

			namespace item {
				enum {
					apple = block::last,
					arrow,
					book,
					book_and_quill,
					bow,
					bow_pulling_0,
					bow_pulling_1,
					bow_pulling_2,
					bread,
					bucket,
					charcoal,
					coal,
					comparator,
					diamond,
					diamond_axe,
					diamond_hoe,
					diamond_pickaxe,
					diamond_shovel,
					feather,
					flint,
					flint_and_steel,
					glowstone_dust,
					ink_sac,
					iron_axe,
					iron_hoe,
					iron_ingot,
					iron_pickaxe,
					iron_shovel,
					item_frame,
					leather,
					oak_door,
					oak_sign,
					paper,
					quartz,
					redstone,
					repeater,
					skeleton_egg,
					stick,
					stone_axe,
					stone_hoe,
					stone_pickaxe,
					stone_shovel,
					string,
					sugar_cane,
					water_bucket,
					wheat,
					wheat_seeds,
					wooden_axe,
					wooden_hoe,
					wooden_pickaxe,
					wooden_shovel,
					written_book,
					zombie_egg,
					last
				};
			};
			const int blocks_size = item::last;
		};
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

	namespace slab {
		const int top = (1 << 24);
	};

	namespace stairs {
		const int top = (1 << 28);
	};
};

enum class geometry {
	none,          // air, water
	cube,          // default blocks
	cross,         // flowers and such
	farmland,      // cube with z = 15 * one16th
	torch,
	slab,
	stairs,
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
		oak_stairs,
		quartz_stairs,
		oak_door,
		oak_trapdoor,
		stone_stairs,
		empty15,
		smooth_stone_stairs = 16,
		empty17,
		cobblestone_stairs,
		empty19,
		stone_bricks_stairs,
		empty21,
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
		oak_slab = 48,
		quartz_slab,
		oak_fence,
		stone_slab,
		empty52,
		smooth_stone_slab,
		empty54,
		cobblestone_slab,
		empty56 = 56,
		stone_bricks_slab,
		empty58,
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
		glowstone,
		bookshelf,
		lectern,
		water = 88,
		empty89,
		obsidian,
		feather = 93,
		leather,
		ink_sac,
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
		string,
		glowstone_dust,
		paper,
		book,
		book_and_quill,
		written_book,
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
		quartz = 168,
		netherrack,
		quartz_ore,
		quartz_block,
		smooth_quartz_block,
		chiseled_quartz_block,
		quartz_pillar,
		quartz_bricks,
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
		int texture = settings::consts::shader::block::debug;
		geometry geometry = geometry::cube;
		float fuel_time = 0;
		float blast_resistance = FLT_MAX;
		float saturation_restauration = 0;
		float hardness = -1;
		glm::vec3 hitboxCenter = {0, 0, 0};
		glm::vec3 hitboxHalfSize = {0, 0, 0};
		char light_level = 0;
		char stackSize = 64;
		bool isFuel = false;
		bool isComposant = false;
		bool hasHitbox = false;
		bool hasSecondaryHitbox = false;
		bool hasCollisionHitbox_1x1x1 = true;
		bool hasCollisionHitbox = false;
		bool oriented = false;
		bool hasSecondaryCollisionHitbox = false;
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
		virtual int getTex( int dir = face_dir::minus_y, int value = 0 ) const {
			(void)dir;(void)value;
			return (texture);
		}
		virtual void getHitbox( glm::vec3* hitbox, int value ) const;
		virtual void getSecondaryHitbox( glm::vec3* hitbox, int value ) const;
		virtual void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const;
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
		virtual void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
		virtual void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
};

struct Cross : Block {
	public:
		Cross() {
			blast_resistance = 0.0f;
			hasHitbox = true;
			hasCollisionHitbox_1x1x1 = false;
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

struct Slab : Block {
	public:
		Slab() {
			hasHitbox = true;
			hasCollisionHitbox_1x1x1 = false;
			hasCollisionHitbox = true;
			geometry = geometry::slab;
			transparent = true;
		}
		void getHitbox( glm::vec3* hitbox, int value ) const override;
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
		void addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const override;
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
};

struct Stairs : Block {
	public:
		Stairs() {
			hasHitbox = true;
			hasSecondaryHitbox = true;
			hasCollisionHitbox_1x1x1 = false;
			hasCollisionHitbox = true;
			oriented = true;
			hasSecondaryCollisionHitbox = true;
			geometry = geometry::stairs;
			transparent = true;
		}
		void getHitbox( glm::vec3* hitbox, int value ) const override;
		void getSecondaryHitbox( glm::vec3* hitbox, int value ) const override;
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
		void addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const override;
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
};

struct Fence : Block {
	public:
		Fence() {
			hasHitbox = true;
			hasCollisionHitbox_1x1x1 = false;
			hasCollisionHitbox = true;
			geometry = geometry::fence;
			transparent = true;
		}
		void getHitbox( glm::vec3* hitbox, int value ) const override;
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
		void addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const override;
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
};

struct Door : Block {
	public:
		Door() {
			hasHitbox = true;
			hasCollisionHitbox_1x1x1 = false;
			hasCollisionHitbox = true;
			oriented = true;
			geometry = geometry::door;
			transparent = true;
			redstoneComponant = true;
			item3D = false;
		}
		void getHitbox( glm::vec3* hitbox, int value ) const override;
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
};

struct Trapdoor : Block {
	public:
		Trapdoor() {
			hasHitbox = true;
			hasCollisionHitbox_1x1x1 = false;
			hasCollisionHitbox = true;
			oriented = true;
			geometry = geometry::trapdoor;
			transparent = true;
			redstoneComponant = true;
		}
		void getHitbox( glm::vec3* hitbox, int value ) const override;
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
		void addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const override;
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
};

struct Crop : Block {
	public:
		Crop() {
			blast_resistance = 0.0f;
			hasHitbox = true;
			hasCollisionHitbox_1x1x1 = false;
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
			hasHitbox = true;
			hasCollisionHitbox_1x1x1 = false;
			hasCollisionHitbox = true;
			oriented = true;
			geometry = geometry::button;
			transparent = true;
			isSolidForFluid = false;
			item3D = true;
		}
		void getHitbox( glm::vec3* hitbox, int value ) const override;
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
			hasCollisionHitbox_1x1x1 = false;
			geometry = geometry::none;
			transparent = true;
			isSolidForFluid = false;
		}
};

struct TBD : Cube {
	public:
		TBD() {
			name = "TBD";
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
		}
		int getTex( int dir, int value ) const override {
			(void)value;
			if (dir == face_dir::plus_z) {
				return (settings::consts::shader::block::grass_block_top);
			} else if (dir == face_dir::minus_z) {
				return (settings::consts::shader::block::dirt);
			}
			return (settings::consts::shader::block::grass_block_side);
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
		}
		int getTex( int dir, int value ) const override {
			int axis = (value >> offset::blocks::orientation) & 0x7;
			switch (dir) {
				case face_dir::minus_z:
				case face_dir::plus_z:
					return ((axis == face_dir::plus_z) ? settings::consts::shader::block::oak_log_top
						: (axis == face_dir::plus_x) ? settings::consts::shader::block::oak_log_spin : settings::consts::shader::block::oak_log);
				case face_dir::minus_x:
				case face_dir::plus_x:
					return ((axis == face_dir::plus_x) ? settings::consts::shader::block::oak_log_top
						: (axis == face_dir::plus_y) ? settings::consts::shader::block::oak_log_spin : settings::consts::shader::block::oak_log);
				case face_dir::minus_y:
				case face_dir::plus_y:
					return ((axis == face_dir::plus_y) ? settings::consts::shader::block::oak_log_top
						: (axis == face_dir::plus_x) ? settings::consts::shader::block::oak_log_spin : settings::consts::shader::block::oak_log);
			}
			return (settings::consts::shader::block::oak_log);
		}
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
};

struct Cactus : Cube {
	public:
		Cactus() {
			name = "CACTUS";
			mined = blocks::cactus;
			blast_resistance = 0.4f;
			hasCollisionHitbox_1x1x1 = false;
			geometry = geometry::crop;
			byHand = true;
			hardness = 0.4f;
			transparent = true;
		}
		int getTex( int dir, int value ) const override {
			(void)value;
			if (dir == face_dir::plus_z) {
				return (settings::consts::shader::block::cactus_top);
			} else if (dir == face_dir::minus_z) {
				return (settings::consts::shader::block::cactus_bottom);
			}
			return (settings::consts::shader::block::cactus_side);
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
		void addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const override;
};

struct Farmland : Block {
	public:
		Farmland() {
			name = "FARMLAND";
			mined = blocks::dirt;
			blast_resistance = 0.6f;
			hasHitbox = true;
			hasCollisionHitbox_1x1x1 = false;
			hasCollisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, 7.5f / 16.0f};
			hitboxHalfSize = {0.5f, 0.5f, 7.5f / 16.0f};
			geometry = geometry::farmland;
			byHand = true;
			needed_tool = blocks::wooden_shovel;
			hardness = 0.6f;
		}
		virtual int getTex( int dir, int value ) const override {
			if (value == blocks::item) {
				return (settings::consts::shader::block::debug);
			} else if (dir == face_dir::plus_z) {
				return ((value & mask::farmland::wet) ? settings::consts::shader::block::farmland_moist
					: settings::consts::shader::block::farmland);
			}
			return (settings::consts::shader::block::dirt);
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
};

struct DirtPath : Farmland {
	public:
		DirtPath() {
			name = "DIRT PATH";
			mined = blocks::dirt;
			blast_resistance = 0.65f;
			hardness = 0.65f;
		}
		int getTex( int dir, int value ) const override {
			if (value == blocks::item) {
				return (settings::consts::shader::block::debug);
			} else if (dir == face_dir::plus_z) {
				return (settings::consts::shader::block::dirt_path_top);
			} else if (dir == face_dir::minus_z) {
				return (settings::consts::shader::block::dirt);
			}
			return (settings::consts::shader::block::dirt_path_side);
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
		}
		int getTex( int dir, int value ) const override {
			(void)value;
			if (dir == face_dir::plus_z) {
				return (settings::consts::shader::block::tnt_top);
			} else if (dir == face_dir::minus_z) {
				return (settings::consts::shader::block::tnt_bottom);
			}
			return (settings::consts::shader::block::tnt_side);
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
		}
		int getTex( int dir, int value ) const override {
			(void)value;
			return ((dir == face_dir::plus_z || dir == face_dir::minus_z)
				? settings::consts::shader::block::target_top : settings::consts::shader::block::target_side);
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
		}
		int getTex( int dir, int value ) const override {
			if (dir == face_dir::plus_z || dir == face_dir::minus_z) {
				return (settings::consts::shader::block::crafting_table_top);
			} else if (((value >> offset::blocks::orientation) & 0x7) == dir) {
				return (settings::consts::shader::block::crafting_table_front);
			}
			return (settings::consts::shader::block::crafting_table_side);
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
		}
		int getTex( int dir, int value ) const override {
			if (dir == face_dir::plus_z || dir == face_dir::minus_z) {
				return (settings::consts::shader::block::furnace_top);
			} else if (((value >> offset::blocks::orientation) & 0x7) == dir) {
				return (((value >> offset::redstone::activated) & 0x1)
					? settings::consts::shader::block::furnace_front_on : settings::consts::shader::block::furnace_front);
			}
			return (settings::consts::shader::block::furnace_side);
		}
};

struct OakStairs : Stairs {
	public:
		OakStairs() {
			name = "OAK_STAIRS";
			mined = blocks::oak_stairs;
			blast_resistance = 3.0f;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::wooden_axe;
			hardness = 2.0f;
			texture = settings::consts::shader::block::oak_planks;
		}
};

struct QuartzStairs : Stairs {
	public:
		QuartzStairs() {
			name = "QUARTZ_STAIRS";
			mined = blocks::quartz_stairs;
			blast_resistance = .8f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = .8f;
			texture = settings::consts::shader::block::quartz_block;
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
		int getTex( int dir, int value ) const override {
			(void)dir;
			if (value == blocks::item) {
				return (settings::consts::shader::item::oak_door);
			}
			switch (dir) {
				case face_dir::plus_z:
					return (settings::consts::shader::block::oak_door_top);
				case face_dir::minus_z:
					return (settings::consts::shader::block::oak_door_bottom);
				default:
					return (((value >> offset::blocks::bitfield) & door::upper_half)
						? settings::consts::shader::block::oak_door_top
						: settings::consts::shader::block::oak_door_bottom);
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
			texture = settings::consts::shader::block::oak_trapdoor;
		}
};

struct StoneStairs : Stairs {
	public:
		StoneStairs() {
			name = "STONE_STAIRS";
			mined = blocks::stone_stairs;
			isComposant = true;
			getProduction = blocks::smooth_stone_stairs;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			texture = settings::consts::shader::block::stone;
		}
};

struct SmoothStoneStairs : Stairs {
	public:
		SmoothStoneStairs() {
			name = "SMOOTH_STONE_STAIRS";
			mined = blocks::smooth_stone_stairs;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			texture = settings::consts::shader::block::smooth_stone;
		}
};

struct CobbleStoneStairs : Stairs {
	public:
		CobbleStoneStairs() {
			name = "COBBLESTONE_STAIRS";
			mined = blocks::cobblestone_stairs;
			isComposant = true;
			getProduction = blocks::stone_stairs;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			texture = settings::consts::shader::block::cobblestone;
		}
};

struct StoneBricksStairs : Stairs {
	public:
		StoneBricksStairs() {
			name = "STONE_BRICKS_STAIRS";
			mined = blocks::stone_bricks_stairs;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			texture = settings::consts::shader::block::stone_bricks;
		}
};

struct Lever : Block {
	public:
		Lever() {
			name = "LEVER";
			mined = blocks::lever;
			adventure_block = blocks::redstone_ore;
			blast_resistance = 0.5f;
			hasHitbox = true;
			hasCollisionHitbox_1x1x1 = false;
			hasCollisionHitbox = true;
			oriented = true;
			geometry = geometry::lever;
			byHand = true;
			hardness = 0.5f;
			transparent = true;
			isSolidForFluid = false;
			item3D = false;
			texture = settings::consts::shader::block::lever;
		}
		void getHitbox( glm::vec3* hitbox, int value ) const override;
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
};

struct OakSign : Block {
	public:
		OakSign() {
			name = "OAK_SIGN";
			mined = blocks::oak_sign;
			blast_resistance = 1.0f;
			hasCollisionHitbox_1x1x1 = false;
			hasHitbox = true;
			geometry = geometry::none;
			byHand = true;
			needed_tool = blocks::wooden_axe;
			hardness = 1.0f;
			transparent = true;
			stackSize = 16;
			item3D = false;
			texture = settings::consts::shader::item::oak_sign;
		}
		void getHitbox( glm::vec3* hitbox, int value ) const override;
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
};

struct Bedrock : Cube {
	public:
		Bedrock() {
			name = "BEDROCK";
			byHand = false;
			texture = settings::consts::shader::block::bedrock;
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
			texture = settings::consts::shader::block::dirt;
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
			texture = settings::consts::shader::block::smooth_stone;
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
			texture = settings::consts::shader::block::stone;
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
			texture = settings::consts::shader::block::cobblestone;
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
			texture = settings::consts::shader::block::stone_bricks;
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
			texture = settings::consts::shader::block::cracked_stone_bricks;
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
			texture = settings::consts::shader::block::sand;
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
			texture = settings::consts::shader::block::gravel;
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
			texture = settings::consts::shader::block::oak_leaves;
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
			texture = settings::consts::shader::block::oak_planks;
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
			texture = settings::consts::shader::block::glass;
		}
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
};

struct GlassPane : Block {
	public:
		GlassPane() {
			name = "GLASS_PANE";
			blast_resistance = 0.3f;
			hasHitbox = true;
			hasCollisionHitbox_1x1x1 = false;
			hasCollisionHitbox = true;
			geometry = geometry::glass_pane;
			hardness = 0.3f;
			transparent = true;
			item3D = false;
			texture = settings::consts::shader::block::glass;
		}
		void getHitbox( glm::vec3* hitbox, int value ) const override;
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
		}
		int getTex( int dir, int value ) const override {
			(void)dir;
			return ((value & mask::redstone::activated) ? settings::consts::shader::block::redstone_lamp_on
				: settings::consts::shader::block::redstone_lamp);
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
			texture = settings::consts::shader::block::stone;
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
			texture = settings::consts::shader::block::oak_planks;
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
			texture = settings::consts::shader::block::coal_ore;
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
			texture = settings::consts::shader::block::iron_ore;
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
			texture = settings::consts::shader::block::diamond_ore;
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
			texture = settings::consts::shader::block::coal_block;
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
			texture = settings::consts::shader::block::iron_block;
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
			texture = settings::consts::shader::block::diamond_block;
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
			texture = settings::consts::shader::block::redstone_ore;
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
			texture = settings::consts::shader::block::redstone_block;
		}
};

struct OakSlab : Slab {
	public:
		OakSlab() {
			name = "OAK_SLAB";
			mined = blocks::oak_slab;
			blast_resistance = 3.0f;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::wooden_axe;
			hardness = 2.0f;
			texture = settings::consts::shader::block::oak_planks;
		}
};

struct QuartzSlab : Slab {
	public:
		QuartzSlab() {
			name = "QUARTZ_SLAB";
			mined = blocks::quartz_slab;
			blast_resistance = .8f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = .8f;
			texture = settings::consts::shader::block::quartz_block;
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
			texture = settings::consts::shader::block::oak_planks;
		}
};

struct StoneSlab : Slab {
	public:
		StoneSlab() {
			name = "STONE_SLAB";
			mined = blocks::stone_slab;
			isComposant = true;
			getProduction = blocks::smooth_stone_slab;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			texture = settings::consts::shader::block::stone;
		}
};

struct SmoothStoneSlab : Slab {
	public:
		SmoothStoneSlab() {
			name = "SMOOTH_STONE_SLAB";
			mined = blocks::smooth_stone_slab;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			texture = settings::consts::shader::block::smooth_stone;
		}
};

struct CobbleStoneSlab : Slab {
	public:
		CobbleStoneSlab() {
			name = "COBBLESTONE_SLAB";
			mined = blocks::cobblestone_slab;
			isComposant = true;
			getProduction = blocks::stone_slab;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			texture = settings::consts::shader::block::cobblestone;
		}
};

struct StoneBricksSlab : Slab {
	public:
		StoneBricksSlab() {
			name = "STONE_BRICKS_SLAB";
			mined = blocks::stone_bricks_slab;
			blast_resistance = 6.0f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 1.5f;
			texture = settings::consts::shader::block::stone_bricks;
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
		}
		int getTex( int dir = face_dir::minus_y, int value = 0 ) const override {
			int orientation = (value >> offset::blocks::orientation) & 0x7;
			if (dir == orientation) {
				return ((value & mask::redstone::piston::moving) ? settings::consts::shader::block::piston_inner
					: (mined == blocks::sticky_piston) ? settings::consts::shader::block::piston_top_sticky
					: settings::consts::shader::block::piston_top);
			}
			if (dir == opposite_dir(orientation)) {
				return (settings::consts::shader::block::piston_bottom);
			}
			return (settings::consts::shader::block::piston_side);
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
		}
		int getTex( int dir = face_dir::minus_y, int value = 0 ) const override {
			int orientation = (value >> offset::blocks::orientation) & 0x7;
			if (dir == orientation) {
				return (settings::consts::shader::block::observer_front);
			}
			if (dir == opposite_dir(orientation)) {
				return ((value & mask::redstone::activated) ? settings::consts::shader::block::observer_back_on
					: settings::consts::shader::block::observer_back);
			}
			if (orientation & 0x4) {
				return ((dir & 0x2) ? settings::consts::shader::block::observer_side
					: settings::consts::shader::block::observer_top);
			}
			return ((dir & 0x4) ? settings::consts::shader::block::observer_top
				: settings::consts::shader::block::observer_side);
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
};

struct Poppy : Cross {
	public:
		Poppy() {
			name = "POPPY";
			mined = blocks::poppy;
			texture = settings::consts::shader::block::poppy;
		}
};

struct Dandelion : Cross {
	public:
		Dandelion() {
			name = "DANDELION";
			mined = blocks::dandelion;
			texture = settings::consts::shader::block::dandelion;
		}
};

struct BlueOrchid : Cross {
	public:
		BlueOrchid() {
			name = "BLUE_ORCHID";
			mined = blocks::blue_orchid;
			texture = settings::consts::shader::block::blue_orchid;
		}
};

struct Allium : Cross {
	public:
		Allium() {
			name = "ALLIUM";
			mined = blocks::allium;
			texture = settings::consts::shader::block::allium;
		}
};

struct CornFlower : Cross {
	public:
		CornFlower() {
			name = "CORNFLOWER";
			mined = blocks::cornflower;
			texture = settings::consts::shader::block::cornflower;
		}
};

struct PinkTulip : Cross {
	public:
		PinkTulip() {
			name = "PINK_TULIP";
			mined = blocks::pink_tulip;
			texture = settings::consts::shader::block::pink_tulip;
		}
};

struct Grass : Cross {
	public:
		Grass() {
			name = "GRASS";
			mined = blocks::wheat_seeds;
			hasHitbox = false;
			texture = settings::consts::shader::block::grass;
		}
};

struct SugarCane : Cross {
	public:
		SugarCane() {
			name = "SUGAR_CANE";
			mined = blocks::sugar_cane;
			hasHitbox = false;
			texture = settings::consts::shader::block::sugar_cane;
		}
};

struct DeadBush : Cross {
	public:
		DeadBush() {
			name = "DEAD_BUSH";
			mined = blocks::stick;
			isFuel = true;
			fuel_time = 5;
			texture = settings::consts::shader::block::dead_bush;
		}
};

struct OakSapling : Cross {
	public:
		OakSapling() {
			name = "OAK_SAPLING";
			mined = blocks::oak_sapling;
			isFuel = true;
			fuel_time = 5;
			texture = settings::consts::shader::block::oak_sapling;
		}
};

struct Torch : Block {
	public:
		Torch() {
			name = "TORCH";
			mined = blocks::torch;
			blast_resistance = 0.0f;
			hasHitbox = true;
			hasCollisionHitbox_1x1x1 = false;
			geometry = geometry::torch;
			light_level = 14;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			isSolidForFluid = false;
			item3D = false;
			texture = settings::consts::shader::block::torch;
		}
		void getHitbox( glm::vec3* hitbox, int value ) const override;
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
};

struct RedstoneTorch : Torch {
	public:
		RedstoneTorch() {
			name = "REDSTONE_TORCH";
			mined = blocks::redstone_torch;
			light_level = 7;
		}
		int getTex( int dir = face_dir::minus_y, int value = 0 ) const override {
			(void)dir;
			if (value == blocks::item) {
				return (settings::consts::shader::block::redstone_torch);
			}
			return ((value & mask::redstone::powered) ? settings::consts::shader::block::redstone_torch_off
				: settings::consts::shader::block::redstone_torch);
		}
};

struct RedstoneDust : Block {
	public:
		RedstoneDust() {
			name = "REDSTONE_DUST";
			mined = blocks::redstone_dust;
			blast_resistance = 0.0f;
			hasHitbox = true;
			hasCollisionHitbox_1x1x1 = false;
			hitboxCenter = {0.5f, 0.5f, one16th};
			hitboxHalfSize = {0.2f, 0.2f, one16th};
			geometry = geometry::dust;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			isSolidForFluid = false;
			item3D = false;
		}
		int getTex( int dir, int value ) const override {
			(void)dir;
			return ((value == blocks::item) ? settings::consts::shader::item::redstone
				: settings::consts::shader::block::dust_dot);
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
			hasCollisionHitbox_1x1x1 = false;
			hasCollisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, one16th};
			hitboxHalfSize = {0.5f, 0.5f, one16th};
			oriented = true;
			geometry = geometry::repeater;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			isSolidForFluid = false;
			item3D = false;
		}
		int getTex( int dir = face_dir::minus_y, int value = 0 ) const override {
			(void)dir;
			return ((value == blocks::item) ? settings::consts::shader::item::repeater
				: (value & mask::redstone::powered) ? settings::consts::shader::block::repeater_on
				: settings::consts::shader::block::repeater);
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
			hasCollisionHitbox_1x1x1 = false;
			hasCollisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, one16th};
			hitboxHalfSize = {0.5f, 0.5f, one16th};
			oriented = true;
			geometry = geometry::repeater;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			isSolidForFluid = false;
			item3D = false;
		}
		int getTex( int dir = face_dir::minus_y, int value = 0 ) const override {
			(void)dir;
			return ((value == blocks::item) ? settings::consts::shader::item::comparator
				: (value & mask::redstone::powered) ? settings::consts::shader::block::comparator_on
				: settings::consts::shader::block::comparator);
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
			hasCollisionHitbox_1x1x1 = false;
			hasCollisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, 7.5 / 16.0f};
			hitboxHalfSize = {7 / 16.0f, 7 / 16.0f, 7.5 / 16.0f};
			geometry = geometry::none;
			byHand = true;
			needed_tool = blocks::wooden_axe;
			hardness = 2.5f;
			transparent = true;
			texture = settings::consts::shader::block::chest_front;
		}
		void addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const override;
		void addItem( UI* ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const override;
};

struct WheatCrop : Crop {
	public:
		WheatCrop() {
			name = "WHEAT_CROP";
			mined = blocks::wheat_seeds;
		}
		int getTex( int dir = face_dir::minus_y, int value = 0 ) const override {
			(void)dir;
			return (settings::consts::shader::block::wheat_stage0 + (value >> offset::blocks::bitfield));
		}
		void getHitbox( glm::vec3* hitbox, int value ) const override;
};

struct ItemFrame : Block {
	public:
		ItemFrame() {
			name = "ITEM_FRAME";
			mined = blocks::item_frame;
			blast_resistance = 0.0f;
			hasHitbox = true;
			hasCollisionHitbox_1x1x1 = false;
			oriented = true;
			geometry = geometry::none;
			byHand = true;
			hardness = 0.0f;
			transparent = true;
			isSolidForFluid = false;
			item3D = false;
			texture = settings::consts::shader::block::item_frame;
		}
		int getTex( int dir = face_dir::minus_y, int value = 0 ) const override {
			(void)dir;(void)value;
			return (settings::consts::shader::item::item_frame);
		}
		void getHitbox( glm::vec3* hitbox, int value ) const override;
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
			texture = settings::consts::shader::block::birch_planks;
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
		}
		int getTex( int dir, int value ) const override {
			(void)value;
			return ((dir == face_dir::plus_z) ? settings::consts::shader::block::sandstone_top
				: (dir == face_dir::minus_z) ? settings::consts::shader::block::sandstone_bottom
				: settings::consts::shader::block::sandstone);
		}
};

struct Glowstone : Cube {
	public:
		Glowstone() {
			name = "GLOWSTONE";
			mined = blocks::glowstone;
			light_level = 15;
			blast_resistance = 0.3f;
			byHand = true;
			hardness = 0.3f;
			texture = settings::consts::shader::block::glowstone;
		}
};

struct Bookshelf : Cube {
	public:
		Bookshelf() {
			name = "BOOKSHELF";
			mined = blocks::book;
			blast_resistance = 1.5f;
			needed_tool = blocks::wooden_axe;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			hardness = 1.5f;
		}
		int getTex( int dir, int value ) const override {
			(void)value;
			switch (dir) {
				case face_dir::plus_z:
				case face_dir::minus_z:
					return (settings::consts::shader::block::oak_planks);
			}
			return (settings::consts::shader::block::bookshelf);
		}
};

struct Lectern : Block {
	public:
		Lectern() {
			name = "LECTERN";
			mined = blocks::lectern;
			blast_resistance = 2.5f;
			oriented = true;
			needed_tool = blocks::wooden_axe;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			hardness = 2.5f;
		}
		void addMesh( Chunk* chunk, std::vector<t_shaderInput>& vertices, glm::ivec2 start, glm::vec3 pos, int value ) const override;
		void addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const override;
};

struct Water : Block {
	public:
		Water() {
			name = "WATER";
			mined = blocks::water_bucket;
			blast_resistance = 100.0f;
			hasCollisionHitbox_1x1x1 = false;
			geometry = geometry::none;
			transparent = true;
		}
};

struct Obsidian : Cube {
	public:
		Obsidian() {
			name = "OBSIDIAN";
			mined = blocks::obsidian;
			blast_resistance = 1200.f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			needed_material_level = 3; // min diamond to collect
			hardness = 50.f;
			texture = settings::consts::shader::block::obsidian;
		}
};

struct Feather : Block {
	public:
		Feather() {
			name = "FEATHER";
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::feather;
		}
};

struct Leather : Block {
	public:
		Leather() {
			name = "LEATHER";
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::leather;
		}
};

struct InkSac : Block {
	public:
		InkSac() {
			name = "INK_SAC";
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::ink_sac;
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
			texture = settings::consts::shader::item::stick;
		}
};

struct WoodenShovel : Block {
	public:
		WoodenShovel() {
			name = "WOODEN_SHOVEL";
			stackSize = 1;
			isFuel = true;
			fuel_time = 10;
			durability = 59;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::wooden_shovel;
		}
};

struct StoneShovel : Block {
	public:
		StoneShovel() {
			name = "STONE_SHOVEL";
			stackSize = 1;
			durability = 131;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::stone_shovel;
		}
};

struct IronShovel : Block {
	public:
		IronShovel() {
			name = "IRON_SHOVEL";
			stackSize = 1;
			durability = 250;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::iron_shovel;
		}
};

struct DiamondShovel : Block {
	public:
		DiamondShovel() {
			name = "DIAMOND_SHOVEL";
			stackSize = 1;
			durability = 1561;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::diamond_shovel;
		}
};

struct WoodenAxe : Block {
	public:
		WoodenAxe() {
			name = "wooden_axe";
			stackSize = 1;
			isFuel = true;
			fuel_time = 10;
			durability = 59;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::wooden_axe;
		}
};

struct StoneAxe : Block {
	public:
		StoneAxe() {
			name = "STONE_AXE";
			stackSize = 1;
			durability = 131;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::stone_axe;
		}
};

struct IronAxe : Block {
	public:
		IronAxe() {
			name = "IRON_AXE";
			stackSize = 1;
			durability = 250;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::iron_axe;
		}
};

struct DiamondAxe : Block {
	public:
		DiamondAxe() {
			name = "DIAMOND_AXE";
			stackSize = 1;
			durability = 1561;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::diamond_axe;
		}
};

struct WoodenPickaxe : Block {
	public:
		WoodenPickaxe() {
			name = "WOODEN_PICKAXE";
			stackSize = 1;
			isFuel = true;
			fuel_time = 10;
			durability = 59;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::wooden_pickaxe;
		}
};

struct StonePickaxe : Block {
	public:
		StonePickaxe() {
			name = "STONE_PICKAXE";
			stackSize = 1;
			durability = 131;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::stone_pickaxe;
		}
};

struct IronPickaxe : Block {
	public:
		IronPickaxe() {
			name = "IRON_PICKAXE";
			stackSize = 1;
			durability = 250;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::iron_pickaxe;
		}
};

struct DiamondPickaxe : Block {
	public:
		DiamondPickaxe() {
			name = "DIAMOND_PICKAXE";
			stackSize = 1;
			durability = 1561;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::diamond_pickaxe;
		}
};

struct Bow : Block {
	public:
		Bow() {
			name = "BOW";
			stackSize = 1;
			durability = 384;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::bow;
		}
};

struct Arrow : Block {
	public:
		Arrow() {
			name = "ARROW";
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::arrow;
		}
};

struct WorldEditWand : Block {
	public:
		WorldEditWand() {
			name = "WORLDEDIT_WAND";
			stackSize = 1;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::wooden_axe;
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
			texture = settings::consts::shader::item::coal;
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
			texture = settings::consts::shader::item::charcoal;
		}
};

struct IronIngot : Block {
	public:
		IronIngot() {
			name = "IRON_INGOT";
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::iron_ingot;
		}
};

struct Diamond : Block {
	public:
		Diamond() {
			name = "DIAMOND";
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::diamond;
		}
};

struct Bucket : Block {
	public:
		Bucket() {
			name = "BUCKET";
			adventure_block = blocks::diamond_ore;
			stackSize = 16;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::bucket;
		}
};

struct WaterBucket : Block {
	public:
		WaterBucket() {
			name = "WATER_BUCKET";
			adventure_block = blocks::diamond_ore;
			stackSize = 1;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::water_bucket;
		}
};

struct WoodenHoe : Block {
	public:
		WoodenHoe() {
			name = "WOODEN_HOE";
			stackSize = 1;
			isFuel = true;
			fuel_time = 10;
			durability = 59;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::wooden_hoe;
		}
};

struct StoneHoe : Block {
	public:
		StoneHoe() {
			name = "STONE_HOE";
			stackSize = 1;
			durability = 131;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::stone_hoe;
		}
};

struct IronHoe : Block {
	public:
		IronHoe() {
			name = "IRON_HOE";
			stackSize = 1;
			durability = 250;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::iron_hoe;
		}
};

struct DiamondHoe : Block {
	public:
		DiamondHoe() {
			name = "DIAMOND_HOE";
			stackSize = 1;
			durability = 1561;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::diamond_hoe;
		}
};

struct WheatSeeds : Block {
	public:
		WheatSeeds() {
			name = "WHEAT_SEEDS";
			adventure_block = blocks::farmland;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::wheat_seeds;
		}
};

struct Wheat : Block {
	public:
		Wheat() {
			name = "WHEAT";
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::wheat;
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
			texture = settings::consts::shader::item::bread;
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
			texture = settings::consts::shader::item::apple;
		}
};

struct Flint : Block {
	public:
		Flint() {
			name = "FLINT";
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::flint;
		}
};

struct FlintAndSteel : Block {
	public:
		FlintAndSteel() {
			name = "FLINT AND STEEL";
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::flint_and_steel;
		}
};

struct ZombieEgg : Block {
	public:
		ZombieEgg() {
			name = "ZOMBIE_EGG";
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::zombie_egg;
		}
};

struct SkeletonEgg : Block {
	public:
		SkeletonEgg() {
			name = "SKELETON_EGG";
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::skeleton_egg;
		}
};

struct String : Block {
	public:
		String() {
			name = "STRING";
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::string;
		}
};

struct GlowstoneDust : Block {
	public:
		GlowstoneDust() {
			name = "GLOWSTONE_DUST";
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::glowstone_dust;
		}
};

struct Paper : Block {
	public:
		Paper() {
			name = "PAPER";
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::paper;
		}
};

struct Book : Block {
	public:
		Book() {
			name = "BOOK";
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::book;
		}
};

struct BookAndQuill : Block {
	public:
		BookAndQuill() {
			name = "BOOK_AND_QUILL";
			stackSize = 1;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::book_and_quill;
		}
};

struct WrittenBook : Block {
	public:
		WrittenBook() {
			name = "WRITTEN_BOOK";
			stackSize = 16;
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::written_book;
		}
};

struct Wool : Cube {
	public:
		Wool() {
			blast_resistance = .8f;
			byHand = true;
			// needed_tool = blocks::shear;
			hardness = .8f;
		}
};

struct WhiteWool : Wool {
	public:
		WhiteWool() {
			name = "WHITE_WOOL";
			mined = blocks::white_wool;
			texture = settings::consts::shader::block::white_wool;
		}
};

struct PinkWool : Wool {
	public:
		PinkWool() {
			name = "PINK_WOOL";
			mined = blocks::pink_wool;
			texture = settings::consts::shader::block::pink_wool;
		}
};

struct MagentaWool : Wool {
	public:
		MagentaWool() {
			name = "MAGENTA_WOOL";
			mined = blocks::magenta_wool;
			texture = settings::consts::shader::block::magenta_wool;
		}
};

struct PurpleWool : Wool {
	public:
		PurpleWool() {
			name = "PURPLE_WOOL";
			mined = blocks::purple_wool;
			texture = settings::consts::shader::block::purple_wool;
		}
};

struct BlueWool : Wool {
	public:
		BlueWool() {
			name = "BLUE_WOOL";
			mined = blocks::blue_wool;
			texture = settings::consts::shader::block::blue_wool;
		}
};

struct LightBlueWool : Wool {
	public:
		LightBlueWool() {
			name = "LIGHT_BLUE_WOOL";
			mined = blocks::light_blue_wool;
			texture = settings::consts::shader::block::light_blue_wool;
		}
};

struct CyanWool : Wool {
	public:
		CyanWool() {
			name = "CYAN_WOOL";
			mined = blocks::cyan_wool;
			texture = settings::consts::shader::block::cyan_wool;
		}
};

struct GreenWool : Wool {
	public:
		GreenWool() {
			name = "GREEN_WOOL";
			mined = blocks::green_wool;
			texture = settings::consts::shader::block::green_wool;
		}
};

struct LimeWool : Wool {
	public:
		LimeWool() {
			name = "LIME_WOOL";
			mined = blocks::lime_wool;
			texture = settings::consts::shader::block::lime_wool;
		}
};

struct YellowWool : Wool {
	public:
		YellowWool() {
			name = "YELLOW_WOOL";
			mined = blocks::yellow_wool;
			texture = settings::consts::shader::block::yellow_wool;
		}
};

struct OrangeWool : Wool {
	public:
		OrangeWool() {
			name = "ORANGE_WOOL";
			mined = blocks::orange_wool;
			texture = settings::consts::shader::block::orange_wool;
		}
};

struct RedWool : Wool {
	public:
		RedWool() {
			name = "RED_WOOL";
			mined = blocks::red_wool;
			texture = settings::consts::shader::block::red_wool;
		}
};

struct BrownWool : Wool {
	public:
		BrownWool() {
			name = "BROWN_WOOL";
			mined = blocks::brown_wool;
			texture = settings::consts::shader::block::brown_wool;
		}
};

struct BlackWool : Wool {
	public:
		BlackWool() {
			name = "BLACK_WOOL";
			mined = blocks::black_wool;
			texture = settings::consts::shader::block::black_wool;
		}
};

struct GrayWool : Wool {
	public:
		GrayWool() {
			name = "GRAY_WOOL";
			mined = blocks::gray_wool;
			texture = settings::consts::shader::block::gray_wool;
		}
};

struct LightGrayWool : Wool {
	public:
		LightGrayWool() {
			name = "LIGHT_GRAY_WOOL";
			mined = blocks::light_gray_wool;
			texture = settings::consts::shader::block::light_gray_wool;
		}
};

struct Carpet : Block {
	public:
		Carpet() {
			blast_resistance = .1f;
			hasHitbox = true;
			hasCollisionHitbox_1x1x1 = false;
			hasCollisionHitbox = true;
			hitboxCenter = {0.5f, 0.5f, .5f * one16th};
			hitboxHalfSize = {0.5f, 0.5f, .5f * one16th};
			geometry = geometry::repeater;
			byHand = true;
			hardness = .1f;
			transparent = true;
			isSolidForFluid = false;
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
			texture = settings::consts::shader::block::white_wool;
		}
};

struct PinkCarpet : Carpet {
	public:
		PinkCarpet() {
			name = "PINK_CARPET";
			mined = blocks::pink_carpet;
			texture = settings::consts::shader::block::pink_wool;
		}
};

struct MagentaCarpet : Carpet {
	public:
		MagentaCarpet() {
			name = "MAGENTA_CARPET";
			mined = blocks::magenta_carpet;
			texture = settings::consts::shader::block::magenta_wool;
		}
};

struct PurpleCarpet : Carpet {
	public:
		PurpleCarpet() {
			name = "PURPLE_CARPET";
			mined = blocks::purple_carpet;
			texture = settings::consts::shader::block::purple_wool;
		}
};

struct BlueCarpet : Carpet {
	public:
		BlueCarpet() {
			name = "BLUE_CARPET";
			mined = blocks::blue_carpet;
			texture = settings::consts::shader::block::blue_wool;
		}
};

struct LightBlueCarpet : Carpet {
	public:
		LightBlueCarpet() {
			name = "LIGHT_BLUE_CARPET";
			mined = blocks::light_blue_carpet;
			texture = settings::consts::shader::block::light_blue_wool;
		}
};

struct CyanCarpet : Carpet {
	public:
		CyanCarpet() {
			name = "CYAN_CARPET";
			mined = blocks::cyan_carpet;
			texture = settings::consts::shader::block::cyan_wool;
		}
};

struct GreenCarpet : Carpet {
	public:
		GreenCarpet() {
			name = "GREEN_CARPET";
			mined = blocks::green_carpet;
			texture = settings::consts::shader::block::green_wool;
		}
};

struct LimeCarpet : Carpet {
	public:
		LimeCarpet() {
			name = "LIME_CARPET";
			mined = blocks::lime_carpet;
			texture = settings::consts::shader::block::lime_wool;
		}
};

struct YellowCarpet : Carpet {
	public:
		YellowCarpet() {
			name = "YELLOW_CARPET";
			mined = blocks::yellow_carpet;
			texture = settings::consts::shader::block::yellow_wool;
		}
};

struct OrangeCarpet : Carpet {
	public:
		OrangeCarpet() {
			name = "ORANGE_CARPET";
			mined = blocks::orange_carpet;
			texture = settings::consts::shader::block::orange_wool;
		}
};

struct RedCarpet : Carpet {
	public:
		RedCarpet() {
			name = "RED_CARPET";
			mined = blocks::red_carpet;
			texture = settings::consts::shader::block::red_wool;
		}
};

struct BrownCarpet : Carpet {
	public:
		BrownCarpet() {
			name = "BROWN_CARPET";
			mined = blocks::brown_carpet;
			texture = settings::consts::shader::block::brown_wool;
		}
};

struct BlackCarpet : Carpet {
	public:
		BlackCarpet() {
			name = "BLACK_CARPET";
			mined = blocks::black_carpet;
			texture = settings::consts::shader::block::black_wool;
		}
};

struct GrayCarpet : Carpet {
	public:
		GrayCarpet() {
			name = "GRAY_CARPET";
			mined = blocks::gray_carpet;
			texture = settings::consts::shader::block::gray_wool;
		}
};

struct LightGrayCarpet : Carpet {
	public:
		LightGrayCarpet() {
			name = "LIGHT_GRAY_CARPET";
			mined = blocks::light_gray_carpet;
			texture = settings::consts::shader::block::light_gray_wool;
		}
};

struct Quartz : Block {
	public:
		Quartz() {
			name = "NETHER_QUARTZ";
			isItemOnly = true;
			item3D = false;
			texture = settings::consts::shader::item::quartz;
		}
};

struct Netherrack : Cube {
	public:
		Netherrack() {
			name = "NETHERRACK";
			mined = blocks::netherrack;
			blast_resistance = 0.4f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 0.4f;
			texture = settings::consts::shader::block::netherrack;
		}
};

struct QuartzOre : Cube {
	public:
		QuartzOre() {
			name = "QUARTZ_ORE";
			mined = blocks::quartz;
			blast_resistance = 3.f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = 3.f;
			texture = settings::consts::shader::block::quartz_ore;
		}
};

struct QuartzBlock : Cube {
	public:
		QuartzBlock() {
			name = "QUARTZ_BLOCK";
			mined = blocks::quartz_block;
			isComposant = true;
			getProduction = blocks::quartz_block;
			blast_resistance = .8f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = .8f;
			texture = settings::consts::shader::block::quartz_block;
		}
};

struct SmoothQuartzBlock : Cube {
	public:
		SmoothQuartzBlock() {
			name = "SMOOTH_QUARTZ_BLOCK";
			mined = blocks::smooth_quartz_block;
			blast_resistance = .8f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = .8f;
			texture = settings::consts::shader::block::smooth_quartz_block;
		}
};

struct ChiseledQuartzBlock : Cube {
	public:
		ChiseledQuartzBlock() {
			name = "CHISELED_QUARTZ_BLOCK";
			mined = blocks::chiseled_quartz_block;
			blast_resistance = .8f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = .8f;
			texture = settings::consts::shader::block::chiseled_quartz_block;
		}
		int getTex( int dir, int value ) const override {
			int axis = (value >> offset::blocks::orientation) & 0x7;
			switch (dir) {
				case face_dir::minus_z:
				case face_dir::plus_z:
					return ((axis == face_dir::plus_z) ? settings::consts::shader::block::chiseled_quartz_block_top
						: (axis == face_dir::plus_x) ? settings::consts::shader::block::oak_log_spin : settings::consts::shader::block::chiseled_quartz_block);
				case face_dir::minus_x:
				case face_dir::plus_x:
					return ((axis == face_dir::plus_x) ? settings::consts::shader::block::chiseled_quartz_block_top
						: (axis == face_dir::plus_y) ? settings::consts::shader::block::oak_log_spin : settings::consts::shader::block::chiseled_quartz_block);
				case face_dir::minus_y:
				case face_dir::plus_y:
					return ((axis == face_dir::plus_y) ? settings::consts::shader::block::chiseled_quartz_block_top
						: (axis == face_dir::plus_x) ? settings::consts::shader::block::oak_log_spin : settings::consts::shader::block::chiseled_quartz_block);
			}
			return (settings::consts::shader::block::chiseled_quartz_block);
		}
};

struct QuartzPillar : Cube {
	public:
		QuartzPillar() {
			name = "QUARTZ_PILLAR";
			mined = blocks::quartz_pillar;
			oriented = true;
			blast_resistance = .8f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = .8f;
		}
		int getTex( int dir, int value ) const override {
			int axis = (value >> offset::blocks::orientation) & 0x7;
			switch (dir) {
				case face_dir::minus_z:
				case face_dir::plus_z:
					return ((axis == face_dir::plus_z) ? settings::consts::shader::block::quartz_pillar_top
						: (axis == face_dir::plus_x) ? settings::consts::shader::block::oak_log_spin : settings::consts::shader::block::quartz_pillar);
				case face_dir::minus_x:
				case face_dir::plus_x:
					return ((axis == face_dir::plus_x) ? settings::consts::shader::block::quartz_pillar_top
						: (axis == face_dir::plus_y) ? settings::consts::shader::block::oak_log_spin : settings::consts::shader::block::quartz_pillar);
				case face_dir::minus_y:
				case face_dir::plus_y:
					return ((axis == face_dir::plus_y) ? settings::consts::shader::block::quartz_pillar_top
						: (axis == face_dir::plus_x) ? settings::consts::shader::block::oak_log_spin : settings::consts::shader::block::quartz_pillar);
			}
			return (settings::consts::shader::block::quartz_pillar);
		}
};

struct QuartzBricks : Cube {
	public:
		QuartzBricks() {
			name = "QUARTZ_BRICKS";
			mined = blocks::quartz_bricks;
			blast_resistance = .8f;
			byHand = false;
			needed_tool = blocks::wooden_pickaxe;
			hardness = .8f;
			texture = settings::consts::shader::block::quartz_bricks;
		}
};

extern const std::array<std::unique_ptr<Block>, S_BLOCKS_SIZE> s_blocks;

#endif
