#ifndef BLOCKS_HPP
# define BLOCKS_HPP

enum blocks {
	AIR,
	GRASS_BLOCK,
	OAK_LOG,
	CACTUS,
	CRAFTING_TABLE = 8,
	FURNACE,
	BEDROCK = 16,
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
	COAL_ORE = 32,
	IRON_ORE,
	DIAMOND_ORE,
	COAL_BLOCK,
	IRON_BLOCK,
	DIAMOND_BLOCK,
	POPPY = 48,
	DANDELION,
	BLUE_ORCHID,
	ALLIUM,
	CORNFLOWER,
	PINK_TULIP,
	GRASS,
	SUGAR_CANE,
	DEAD_BUSH,
	TORCH,
	WATER = 72,
	WATER1,
	WATER2,
	WATER3,
	WATER4,
	WATER5,
	WATER6,
	WATER7,
	STICK = 80,
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
	COAL = 96,
	CHARCOAL,
	IRON_INGOT,
	DIAMOND,
	BUCKET,
	WATER_BUCKET,
	NOTVISIBLE = 1000
};

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
		bool byHand = false;
		int needed_tool = blocks::NOTVISIBLE;
		int needed_material_level = 0;
		int durability = 0;
		float break_time_hand = FLT_MAX;
		float break_time_wooden = FLT_MAX;
		float break_time_stone = FLT_MAX;
		float break_time_iron = FLT_MAX;
		float break_time_diamond = FLT_MAX;
		float getBreakTime( int tool ) const {
			if (tool == needed_tool) {
				return (break_time_wooden);
			} else if (tool == needed_tool + 1) {
				return (break_time_stone);
			} else if (tool == needed_tool + 2) {
				return (break_time_iron);
			} else if (tool == needed_tool + 3) {
				return (break_time_diamond);
			}
			return (break_time_hand);
		}
		bool canCollect( int tool ) const {
			if (byHand) {
				return (true);
			}
			return (tool >= needed_tool + needed_material_level && tool < needed_tool + 4);
		}
};

struct Air : Block {
	public:
		Air() {
			name = "AIR";
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
			byHand = true;
			needed_tool = blocks::WOODEN_SHOVEL;
			break_time_hand = 0.9f;
			break_time_wooden = 0.45f;
			break_time_stone = 0.25f;
			break_time_iron = 0.15f;
			break_time_diamond = 0.15f;
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
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			break_time_hand = 3.0f;
			break_time_wooden = 1.5f;
			break_time_stone = 0.75f;
			break_time_iron = 0.5f;
			break_time_diamond = 0.4f;
		}
};

struct Cactus : Block {
	public:
		Cactus() {
			name = "CACTUS";
			mined = blocks::CACTUS;
			byHand = true;
			break_time_hand = 0.6f;
		}
};

struct CraftingTable : Block {
	public:
		CraftingTable() {
			name = "CRAFTING_TABLE";
			mined = blocks::CRAFTING_TABLE;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = blocks::WOODEN_AXE;
			break_time_hand = 3.75f;
			break_time_wooden = 1.9f;
			break_time_stone = 0.95f;
			break_time_iron = 0.65f;
			break_time_diamond = 0.5f;
		}
};

struct Furnace : Block {
	public:
		Furnace() {
			name = "FURNACE";
			mined = blocks::FURNACE;
			byHand = false;
			needed_tool = blocks::WOODEN_PICKAXE;
			break_time_hand = 17.5f;
			break_time_wooden = 2.65f;
			break_time_stone = 1.35f;
			break_time_iron = 0.9f;
			break_time_diamond = 0.7f;
		}
};

struct Bedrock : Block {
	public:
		Bedrock() {
			name = "BEDROCK";
			byHand = false;
		}
};

struct Dirt : Block {
	public:
		Dirt() {
			name = "DIRT";
			mined = blocks::DIRT;
			byHand = true;
			needed_tool = blocks::WOODEN_SHOVEL;
			break_time_hand = 0.75f;
			break_time_wooden = 0.4f;
			break_time_stone = 0.2f;
			break_time_iron = 0.15f;
			break_time_diamond = 0.1f;
		}
};

struct SmoothStone : Block {
	public:
		SmoothStone() {
			name = "SMOOTH_STONE";
			mined = blocks::SMOOTH_STONE;
			byHand = false;
			needed_tool = WOODEN_PICKAXE;
			break_time_hand = 10.0f;
			break_time_wooden = 1.5f;
			break_time_stone = 0.75f;
			break_time_iron = 0.5f;
			break_time_diamond = 0.4f;
		}
};

struct Stone : Block {
	public:
		Stone() {
			name = "STONE";
			mined = blocks::COBBLESTONE;
			isComposant = true;
			getProduction = blocks::SMOOTH_STONE;
			byHand = false;
			needed_tool = WOODEN_PICKAXE;
			break_time_hand = 7.5f;
			break_time_wooden = 1.15f;
			break_time_stone = 0.6f;
			break_time_iron = 0.4f;
			break_time_diamond = 0.3f;
		}
};

struct Cobblestone : Block {
	public:
		Cobblestone() {
			name = "COBBLESTONE";
			mined = blocks::COBBLESTONE;
			isComposant = true;
			getProduction = blocks::STONE;
			byHand = false;
			needed_tool = WOODEN_PICKAXE;
			break_time_hand = 10.0f;
			break_time_wooden = 1.5f;
			break_time_stone = 0.74f;
			break_time_iron = 0.5f;
			break_time_diamond = 0.4f;
		}
};

struct StoneBrick : Block {
	public:
		StoneBrick() {
			name = "STONE_BRICK";
			mined = blocks::STONE_BRICKS;
			isComposant = true;
			getProduction = blocks::CRACKED_STONE_BRICKS;
			byHand = false;
			needed_tool = WOODEN_PICKAXE;
			break_time_hand = 7.5f;
			break_time_wooden = 1.15f;
			break_time_stone = 0.6f;
			break_time_iron = 0.4f;
			break_time_diamond = 0.3f;
		}
};

struct CrackedStoneBrick : Block {
	public:
		CrackedStoneBrick() {
			name = "CRACKED_STONE_BRICK";
			mined = blocks::CRACKED_STONE_BRICKS;
			byHand = false;
			needed_tool = WOODEN_PICKAXE;
			break_time_hand = 7.5f;
			break_time_wooden = 1.15f;
			break_time_stone = 0.6f;
			break_time_iron = 0.4f;
			break_time_diamond = 0.3f;
		}
};

struct Sand : Block {
	public:
		Sand() {
			name = "SAND";
			mined = blocks::SAND;
			byHand = true;
			needed_tool = WOODEN_SHOVEL;
			break_time_hand = 0.75f;
			break_time_wooden = 0.4f;
			break_time_stone = 0.2f;
			break_time_iron = 0.15f;
			break_time_diamond = 0.1f;
		}
};

struct Gravel : Block {
	public:
		Gravel() {
			name = "GRAVEL";
			mined = blocks::GRAVEL;
			byHand = true;
			needed_tool = WOODEN_SHOVEL;
			break_time_hand = 0.9f;
			break_time_wooden = 0.45f;
			break_time_stone = 0.25f;
			break_time_iron = 0.15f;
			break_time_diamond = 0.15f;
		}
};

struct OakLeaves : Block {
	public:
		OakLeaves() {
			name = "OAK_LEAVES";
			byHand = false;
			break_time_hand = 0.3f;
		}
};

struct OakPlanks : Block {
	public:
		OakPlanks() {
			name = "OAK_PLANKS";
			mined = blocks::OAK_PLANKS;
			isFuel = true;
			fuel_time = 15;
			byHand = true;
			needed_tool = WOODEN_AXE;
			break_time_hand = 3.0f;
			break_time_wooden = 1.5f;
			break_time_stone = 0.75f;
			break_time_iron = 0.5f;
			break_time_diamond = 0.4f;
		}
};

struct CoalOre : Block {
	public:
		CoalOre() {
			name = "COAL_ORE";
			mined = blocks::COAL;
			isComposant = true;
			getProduction = blocks::COAL;
			byHand = false;
			needed_tool = WOODEN_PICKAXE;
			break_time_hand = 15.0f;
			break_time_wooden = 2.25f;
			break_time_stone = 1.15f;
			break_time_iron = 0.75f;
			break_time_diamond = 0.6f;
		}
};

struct IronOre : Block {
	public:
		IronOre() {
			name = "IRON_ORE";
			mined = blocks::IRON_ORE;
			isComposant = true;
			getProduction = blocks::IRON_INGOT;
			byHand = false;
			needed_tool = WOODEN_PICKAXE;
			needed_material_level = 1; // min stone to collect
			break_time_hand = 15.0f;
			break_time_wooden = 7.5f;
			break_time_stone = 1.15f;
			break_time_iron = 0.75f;
			break_time_diamond = 0.6f;
		}
};

struct DiamondOre : Block {
	public:
		DiamondOre() {
			name = "DIAMOND_ORE";
			mined = blocks::DIAMOND;
			isComposant = true;
			getProduction = blocks::DIAMOND;
			byHand = false;
			needed_tool = WOODEN_PICKAXE;
			needed_material_level = 2; // min iron to collect
			break_time_hand = 15.0f;
			break_time_wooden = 7.5f;
			break_time_stone = 3.75f;
			break_time_iron = 0.75f;
			break_time_diamond = 0.6f;
		}
};

struct CoalBlock : Block {
	public:
		CoalBlock() {
			name = "COAL_BLOCK";
			mined = blocks::COAL_BLOCK;
			isFuel = true;
			fuel_time = 800;
			byHand = false;
			needed_tool = WOODEN_PICKAXE;
			break_time_hand = 25.0f;
			break_time_wooden = 3.75f;
			break_time_stone = 1.9f;
			break_time_iron = 1.25f;
			break_time_diamond = 0.95f;
		}
};

struct IronBlock : Block {
	public:
		IronBlock() {
			name = "IRON_BLOCK";
			mined = blocks::IRON_BLOCK;
			byHand = false;
			needed_tool = WOODEN_PICKAXE;
			needed_material_level = 1; // min stone to collect
			break_time_hand = 25.0f;
			break_time_wooden = 12.5f;
			break_time_stone = 1.9f;
			break_time_iron = 1.25f;
			break_time_diamond = 0.95f;
		}
};

struct DiamondBlock : Block {
	public:
		DiamondBlock() {
			name = "DIAMOND_BLOCK";
			mined = blocks::DIAMOND_BLOCK;
			byHand = false;
			needed_tool = WOODEN_PICKAXE;
			needed_material_level = 2; // min iron to collect
			break_time_hand = 25.0f;
			break_time_wooden = 12.5f;
			break_time_stone = 6.25f;
			break_time_iron = 1.25f;
			break_time_diamond = 0.95f;
		}
};

struct Poppy : Block {
	public:
		Poppy() {
			name = "POPPY";
			mined = blocks::POPPY;
			byHand = true;
			break_time_hand = 0.05f;
		}
};

struct Dandelion : Block {
	public:
		Dandelion() {
			name = "DANDELION";
			mined = blocks::DANDELION;
			byHand = true;
			break_time_hand = 0.05f;
		}
};

struct BlueOrchid : Block {
	public:
		BlueOrchid() {
			name = "BLUE_ORCHID";
			mined = blocks::BLUE_ORCHID;
			byHand = true;
			break_time_hand = 0.05f;
		}
};

struct Allium : Block {
	public:
		Allium() {
			name = "ALLIUM";
			mined = blocks::ALLIUM;
			byHand = true;
			break_time_hand = 0.05f;
		}
};

struct CornFlower : Block {
	public:
		CornFlower() {
			name = "CORNFLOWER";
			mined = blocks::CORNFLOWER;
			byHand = true;
			break_time_hand = 0.05f;
		}
};

struct PinkTulip : Block {
	public:
		PinkTulip() {
			name = "PINK_TULIP";
			mined = blocks::PINK_TULIP;
			byHand = true;
			break_time_hand = 0.05f;
		}
};

struct Grass : Block {
	public:
		Grass() {
			name = "GRASS";
			byHand = false;
			break_time_hand = 0.05f;
		}
};

struct SugarCane : Block {
	public:
		SugarCane() {
			name = "SUGAR_CANE";
			mined = blocks::SUGAR_CANE;
			byHand = true;
			break_time_hand = 0.2f;
		}
};

struct DeadBush : Block {
	public:
		DeadBush() {
			name = "DEAD_BUSH";
			mined = blocks::STICK;
			isFuel = true;
			fuel_time = 5;
			byHand = true;
			break_time_hand = 0.05f;
		}
};

struct Torch : Block {
	public:
		Torch() {
			name = "TORCH";
			mined = blocks::TORCH;
			light_level = 14;
			byHand = true;
			break_time_hand = 0.05f;
		}
};

struct Water : Block {
	public:
		Water() {
			name = "WATER";
			mined = blocks::WATER_BUCKET;
		}
};

struct Water1 : Block {
	public:
		Water1() {
			name = "WATER 1";
		}
};

struct Water2 : Block {
	public:
		Water2() {
			name = "WATER 2";
		}
};

struct Water3 : Block {
	public:
		Water3() {
			name = "WATER 3";
		}
};

struct Water4 : Block {
	public:
		Water4() {
			name = "WATER 4";
		}
};

struct Water5 : Block {
	public:
		Water5() {
			name = "WATER 5";
		}
};

struct Water6 : Block {
	public:
		Water6() {
			name = "WATER 6";
		}
};

struct Water7 : Block {
	public:
		Water7() {
			name = "WATER 7";
		}
};

struct Stick : Block {
	public:
		Stick() {
			name = "STICK";
			isFuel = true;
			fuel_time = 5;
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
		}
};

struct StoneShovel : Block {
	public:
		StoneShovel() {
			name = "STONE_SHOVEL";
			stackable = false;
			durability = 131;
		}
};

struct IronShovel : Block {
	public:
		IronShovel() {
			name = "IRON_SHOVEL";
			stackable = false;
			durability = 250;
		}
};

struct DiamondShovel : Block {
	public:
		DiamondShovel() {
			name = "DIAMOND_SHOVEL";
			stackable = false;
			durability = 1561;
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
		}
};

struct StoneAxe : Block {
	public:
		StoneAxe() {
			name = "STONE_AXE";
			stackable = false;
			durability = 131;
		}
};

struct IronAxe : Block {
	public:
		IronAxe() {
			name = "IRON_AXE";
			stackable = false;
			durability = 250;
		}
};

struct DiamondAxe : Block {
	public:
		DiamondAxe() {
			name = "DIAMOND_AXE";
			stackable = false;
			durability = 1561;
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
		}
};

struct StonePickaxe : Block {
	public:
		StonePickaxe() {
			name = "STONE_PICKAXE";
			stackable = false;
			durability = 131;
		}
};

struct IronPickaxe : Block {
	public:
		IronPickaxe() {
			name = "IRON_PICKAXE";
			stackable = false;
			durability = 250;
		}
};

struct DiamondPickaxe : Block {
	public:
		DiamondPickaxe() {
			name = "DIAMOND_PICKAXE";
			stackable = false;
			durability = 1561;
		}
};

struct Coal : Block {
	public:
		Coal() {
			name = "COAL";
			isFuel = true;
			fuel_time = 80;
		}
};

struct Charcoal : Block {
	public:
		Charcoal() {
			name = "CHARCOAL";
			isFuel = true;
			fuel_time = 80;
		}
};

struct IronIngot : Block {
	public:
		IronIngot() {
			name = "IRON_INGOT";
		}
};

struct Diamond : Block {
	public:
		Diamond() {
			name = "DIAMOND";
		}
};

struct Bucket : Block {
	public:
		Bucket() {
			name = "BUCKET";
		}
};

struct WaterBucket : Block {
	public:
		WaterBucket() {
			name = "WATER_BUCKET";
			stackable = false;
		}
};

const Block s_blocks[112] = {
	Air(), GrassBlock(), OakLog(), Cactus(), TBD(), TBD(), TBD(), TBD(), CraftingTable(), Furnace(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(),
	Bedrock(), Dirt(), SmoothStone(), Stone(), Cobblestone(), StoneBrick(), CrackedStoneBrick(), Sand(), Gravel(), OakLeaves(), OakPlanks(), TBD(), TBD(), TBD(), TBD(), TBD(),
	CoalOre(), IronOre(), DiamondOre(), CoalBlock(), IronBlock(), DiamondBlock(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(),
	Poppy(), Dandelion(), BlueOrchid(), Allium(), CornFlower(), PinkTulip(), Grass(), SugarCane(), DeadBush(), Torch(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(),
	TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), Water(), Water1(), Water2(), Water3(), Water4(), Water5(), Water6(), Water7(),
	Stick(), WoodenShovel(), StoneShovel(), IronShovel(), DiamondShovel(), WoodenAxe(), StoneAxe(), IronAxe(), DiamondAxe(), WoodenPickaxe(), StonePickaxe(), IronPickaxe(), DiamondPickaxe(), TBD(), TBD(), TBD(),
	Coal(), Charcoal(), IronIngot(), Diamond(), Bucket(), WaterBucket(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(),
};
#endif
