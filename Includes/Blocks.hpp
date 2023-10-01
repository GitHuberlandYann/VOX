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
	STONE,
	COBBLESTONE,
	SAND,
	GRAVEL,
	OAK_LEAVES,
	OAK_PLANKS,
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
	SUGAR_CANE,
	DEAD_BUSH,
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
	IRON_INGOT,
	DIAMOND,
	NOTVISIBLE = 1000
};

struct Block {
	public:
		std::string name = "DEFAULT";
		bool byHand = false;
		int needed_tool = blocks::NOTVISIBLE;
		int needed_material_level = 0;
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
			return (tool >= needed_tool + needed_material_level && tool < needed_tool + 4 - needed_material_level);
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
			byHand = true;
			break_time_hand = 0.6f;
		}
};

struct CraftingTable : Block {
	public:
		CraftingTable() {
			name = "CRAFTING_TABLE";
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
			byHand = true;
			needed_tool = blocks::WOODEN_SHOVEL;
			break_time_hand = 0.75f;
			break_time_wooden = 0.4f;
			break_time_stone = 0.2f;
			break_time_iron = 0.15f;
			break_time_diamond = 0.1f;
		}
};

struct Stone : Block {
	public:
		Stone() {
			name = "STONE";
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
			byHand = false;
			needed_tool = WOODEN_PICKAXE;
			break_time_hand = 10.0f;
			break_time_wooden = 1.5f;
			break_time_stone = 0.74f;
			break_time_iron = 0.5f;
			break_time_diamond = 0.4f;
		}
};

struct Sand : Block {
	public:
		Sand() {
			name = "SAND";
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

struct Poppy : Block {
	public:
		Poppy() {
			name = "POPPY";
			byHand = true;
			break_time_hand = 0.05f;
		}
};

struct Dandelion : Block {
	public:
		Dandelion() {
			name = "DANDELION";
			byHand = true;
			break_time_hand = 0.05f;
		}
};

struct BlueOrchid : Block {
	public:
		BlueOrchid() {
			name = "BLUE_ORCHID";
			byHand = true;
			break_time_hand = 0.05f;
		}
};

struct Allium : Block {
	public:
		Allium() {
			name = "ALLIUM";
			byHand = true;
			break_time_hand = 0.05f;
		}
};

struct CornFlower : Block {
	public:
		CornFlower() {
			name = "CORNFLOWER";
			byHand = true;
			break_time_hand = 0.05f;
		}
};

struct PinkTulip : Block {
	public:
		PinkTulip() {
			name = "PINK_TULIP";
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
			byHand = true;
			break_time_hand = 0.2f;
		}
};

struct DeadBush : Block {
	public:
		DeadBush() {
			name = "DEAD_BUSH";
			byHand = true;
			break_time_hand = 0.05f;
		}
};

const Block s_blocks[64] = {
	Air(), GrassBlock(), OakLog(), Cactus(), TBD(), TBD(), TBD(), TBD(), CraftingTable(), Furnace(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(),
	Bedrock(), Dirt(), Stone(), Cobblestone(), Sand(), Gravel(), OakLeaves(), OakPlanks(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(),
	CoalOre(), IronOre(), DiamondOre(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(),
	Poppy(), Dandelion(), BlueOrchid(), Allium(), CornFlower(), PinkTulip(), Grass(), SugarCane(), DeadBush(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD()
};

#endif
