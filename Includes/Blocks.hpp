#ifndef BLOCKS_HPP
# define BLOCKS_HPP

struct Block {
	public:
		std::string name = "BLOCK";
		bool byHand = false;
		float break_time_hand = FLT_MAX;
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
			break_time_hand = 0.9f;
		}
};

struct OakTrunk : Block {
	public:
		OakTrunk() {
			name = "OAK_TRUNK";
			byHand = true;
			break_time_hand = 3.0f;
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
			break_time_hand = 0.75f;
		}
};

struct Stone : Block {
	public:
		Stone() {
			name = "STONE";
			byHand = false;
			break_time_hand = 7.5f;
		}
};

struct Sand : Block {
	public:
		Sand() {
			name = "SAND";
			byHand = true;
			break_time_hand = 0.75f;
		}
};

struct Gravel : Block {
	public:
		Gravel() {
			name = "GRAVEL";
			byHand = true;
			break_time_hand = 0.9f;
		}
};

struct OakLeaves : Block {
	public:
		OakLeaves() {
			name = "OAK_LEAVES";
			byHand = true;
			break_time_hand = 0.3f;
		}
};

struct CoalOre : Block {
	public:
		CoalOre() {
			name = "COAL_ORE";
			byHand = false;
			break_time_hand = 15.0f;
		}
};

struct IronOre : Block {
	public:
		IronOre() {
			name = "IRON_ORE";
			byHand = false;
			break_time_hand = 15.0f;
		}
};

struct DiamondOre : Block {
	public:
		DiamondOre() {
			name = "DIAMOND_ORE";
			byHand = false;
			break_time_hand = 15.0f;
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

const Block s_blocks[64] = {
	Air(), GrassBlock(), OakTrunk(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(),
	Bedrock(), Dirt(), Stone(), Sand(), Gravel(), OakLeaves(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(),
	CoalOre(), IronOre(), DiamondOre(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(),
	Poppy(), Dandelion(), BlueOrchid(), Allium(), CornFlower(), PinkTulip(), Grass(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD(), TBD()
};

#endif
