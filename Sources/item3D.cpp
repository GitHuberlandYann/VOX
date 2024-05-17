#include "Chunk.hpp"
#include <map>

namespace EXTRUSION {

	typedef struct s_extrusion {
		std::vector<glm::ivec3> up;
		std::vector<glm::ivec3> down;
		std::vector<glm::ivec3> left;
		std::vector<glm::ivec3> right;
	}		t_extrusion;

	const std::map<int, t_extrusion> exList = {
		{blocks::GLASS_PANE, {
			{{0, 0, 16}, {4, 2, 1}, {3, 3, 1}, {2, 4, 1}, {13, 12, 1}, {12, 13, 1}, {1, 15, 14}, },
			{{1, 0, 14}, {4, 2, 1}, {3, 3, 1}, {2, 4, 1}, {13, 12, 1}, {12, 13, 1}, {0, 15, 16}, },
			{{0, 0, 16}, {2, 4, 1}, {3, 3, 1}, {4, 2, 1}, {12, 13, 1}, {13, 12, 1}, {15, 1, 14}, },
			{{0, 1, 14}, {2, 4, 1}, {3, 3, 1}, {4, 2, 1}, {12, 13, 1}, {13, 12, 1}, {15, 0, 16}, }
		}},
		{blocks::POPPY, {
			{{8, 5, 2}, {6, 6, 2}, {10, 6, 1}, {5, 7, 1}, {5, 13, 2}, {10, 13, 2}, {9, 14, 1}, {7, 15, 1}},
			{{10, 7, 1}, {5, 8, 1}, {9, 8, 1}, {6, 9, 2}, {5, 13, 1}, {10, 13, 2}, {6, 14, 1}, {7, 15, 3}},
			{{5, 7, 2}, {5, 13, 1}, {6, 6, 1}, {6, 9, 1}, {6, 14, 1}, {7, 15, 1}, {8, 5, 1}, {8, 10, 5}, {10, 13, 1}},
			{{6, 13, 2}, {8, 9, 5}, {9, 5, 1}, {9, 8, 1}, {9, 14, 2}, {10, 6, 2}, {11, 13, 1}}
		}},
		{blocks::DANDELION, {
			{{7, 7, 2}, {6, 8, 1}, {9, 8, 1}, {5, 9, 1}, {10, 9, 1}, {5, 12, 2}, {9, 13, 2}, },
                {{5, 9, 1}, {10, 9, 1}, {6, 10, 1}, {9, 10, 1}, {5, 12, 1}, {6, 13, 1}, {10, 13, 1}, {9, 14, 1}, {7, 15, 2}, },
                {{5, 9, 1}, {5, 12, 1}, {6, 8, 1}, {6, 10, 1}, {6, 13, 1}, {7, 7, 1}, {7, 11, 1}, {7, 14, 2}, },
                {{8, 7, 1}, {8, 11, 2}, {8, 15, 1}, {9, 8, 1}, {9, 10, 1}, {9, 14, 1}, {10, 9, 1}, {10, 13, 1}, }
		}},
		{blocks::BLUE_ORCHID, {
			{{6, 4, 2}, {5, 5, 1}, {8, 5, 1}, {13, 5, 1}, {12, 6, 1}, {14, 6, 1}, {8, 8, 1}, {11, 8, 1}, {3, 9, 2}, {10, 9, 1}, {2, 10, 1}, {5, 10, 1}, {7, 10, 1}, {9, 10, 1}, {6, 11, 1}, {7, 12, 1}, {9, 14, 1}, },
                {{5, 6, 1}, {8, 6, 1}, {14, 6, 1}, {6, 7, 2}, {12, 7, 2}, {11, 8, 1}, {2, 10, 1}, {4, 10, 2}, {7, 10, 1}, {10, 10, 1}, {3, 11, 1}, {9, 11, 1}, {6, 12, 1}, {7, 13, 1}, {9, 14, 1}, {8, 15, 1}, },
                {{2, 10, 1}, {3, 9, 1}, {3, 11, 1}, {5, 5, 2}, {6, 4, 1}, {6, 7, 1}, {6, 11, 2}, {7, 10, 1}, {7, 13, 1}, {8, 8, 2}, {8, 11, 1}, {8, 14, 2}, {10, 9, 1}, {11, 8, 1}, {12, 6, 2}, {13, 5, 1}, },
                {{3, 11, 1}, {4, 9, 1}, {5, 10, 1}, {6, 11, 1}, {7, 4, 1}, {7, 7, 1}, {8, 5, 2}, {8, 8, 2}, {8, 12, 2}, {8, 15, 1}, {9, 11, 1}, {9, 14, 1}, {10, 9, 2}, {11, 8, 1}, {13, 5, 1}, {13, 7, 1}, {14, 6, 1}, }
		}},
		{blocks::ALLIUM, {
			{{6, 2, 3}, {5, 3, 1}, {9, 3, 1}, {4, 4, 1}, {10, 4, 1}, },
                {{4, 5, 1}, {10, 5, 1}, {5, 6, 1}, {9, 6, 1}, {6, 7, 1}, {8, 7, 1}, {7, 15, 1}, },
                {{4, 4, 2}, {5, 3, 1}, {5, 6, 1}, {6, 2, 1}, {6, 7, 1}, {7, 8, 8}, },
                {{7, 8, 8}, {8, 2, 1}, {8, 7, 1}, {9, 3, 1}, {9, 6, 1}, {10, 4, 2}, }
		}},
		{blocks::CORNFLOWER, {
			{{5, 4, 1}, {7, 4, 3}, {4, 5, 1}, {6, 5, 1}, {10, 5, 1}, {4, 10, 1}, {5, 11, 1}, {6, 12, 1}, {7, 13, 1}, },
                {{4, 5, 1}, {5, 6, 2}, {9, 6, 2}, {7, 8, 1}, {4, 10, 1}, {5, 12, 1}, {6, 13, 1}, {7, 14, 1}, {8, 15, 1}, },
                {{4, 5, 1}, {4, 10, 1}, {5, 4, 1}, {5, 6, 1}, {5, 11, 2}, {6, 13, 1}, {7, 4, 1}, {7, 7, 2}, {7, 14, 1}, {8, 9, 4}, {8, 15, 1}, },
                {{4, 10, 1}, {5, 4, 1}, {5, 11, 1}, {6, 12, 1}, {8, 7, 9}, {9, 4, 1}, {10, 5, 2}, }
		}},
		{blocks::PINK_TULIP, {
			{{6, 3, 1}, {8, 3, 1}, {7, 4, 1}, {5, 7, 1}, {6, 8, 1}, {9, 8, 1}, {8, 10, 1}, },
                {{6, 5, 1}, {8, 5, 1}, {5, 12, 1}, {9, 12, 1}, {6, 14, 1}, {8, 14, 1}, {7, 15, 1}, },
                {{5, 7, 6}, {6, 3, 3}, {6, 13, 2}, {7, 6, 2}, {7, 15, 1}, {8, 3, 1}, {9, 8, 2}, },
                {{5, 7, 1}, {6, 3, 1}, {7, 6, 4}, {7, 15, 1}, {8, 3, 3}, {8, 13, 2}, {9, 8, 5}, }
		}},
		{blocks::GRASS, {
			{{4, 2, 1}, {7, 2, 1}, {10, 2, 1}, {13, 2, 1}, {5, 3, 1}, {12, 3, 1}, {3, 4, 1}, {9, 4, 1}, {1, 5, 1}, {4, 5, 1}, {11, 5, 1}, {14, 5, 1}, {2, 6, 1}, {8, 6, 1}, {13, 6, 1}, {6, 7, 1}, {3, 8, 1}, {7, 8, 1}, {10, 8, 1}, {0, 9, 1}, {12, 9, 1}, {14, 9, 1}, {6, 10, 1}, {9, 10, 1}, {15, 10, 1}, {1, 11, 1}, {4, 11, 1}, {13, 11, 1}, {2, 12, 1}, {7, 12, 1}, {11, 12, 1}, {14, 12, 1}, {1, 13, 1}, {15, 14, 1}, {0, 15, 1}, },
                {{4, 2, 1}, {13, 2, 1}, {3, 4, 1}, {10, 4, 1}, {12, 4, 1}, {1, 5, 1}, {14, 5, 1}, {4, 6, 1}, {7, 6, 1}, {6, 8, 1}, {9, 8, 1}, {13, 8, 1}, {7, 9, 1}, {11, 9, 1}, {2, 10, 1}, {14, 10, 1}, {1, 11, 1}, {15, 12, 1}, {0, 13, 1}, {0, 15, 16}, },
                {{0, 9, 5}, {0, 15, 1}, {1, 5, 1}, {1, 14, 1}, {2, 6, 5}, {2, 12, 1}, {3, 4, 1}, {3, 11, 1}, {4, 2, 1}, {4, 5, 2}, {5, 3, 2}, {5, 7, 4}, {7, 2, 5}, {7, 9, 1}, {8, 7, 1}, {8, 10, 2}, {9, 4, 2}, {10, 2, 2}, {10, 9, 1}, {11, 5, 3}, {12, 3, 2}, {12, 10, 2}, {13, 2, 1}, {13, 6, 3}, {14, 5, 1}, {14, 9, 2}, {15, 11, 1}, },
                {{0, 9, 2}, {0, 12, 1}, {1, 5, 1}, {1, 11, 1}, {2, 6, 2}, {3, 4, 1}, {3, 8, 3}, {4, 2, 1}, {5, 3, 4}, {5, 9, 1}, {6, 7, 1}, {6, 10, 2}, {7, 2, 4}, {8, 9, 1}, {9, 5, 3}, {10, 2, 3}, {10, 10, 2}, {11, 5, 4}, {12, 3, 2}, {12, 9, 2}, {13, 2, 1}, {13, 6, 3}, {13, 11, 1}, {14, 5, 1}, {14, 9, 1}, {14, 13, 1}, {15, 10, 3}, {15, 14, 2}, }
		}},
		{blocks::SUGAR_CANE, {
			{{1, 0, 2}, {5, 0, 2}, {9, 0, 2}, {13, 0, 2}, {3, 2, 1}, {11, 2, 1}, {7, 3, 1}, {15, 6, 1}, {0, 8, 1}, {4, 10, 1}, {8, 10, 1}, {12, 13, 1}, },
                {{11, 2, 1}, {3, 3, 1}, {7, 4, 1}, {15, 7, 1}, {0, 9, 1}, {4, 10, 1}, {8, 11, 1}, {12, 13, 1}, {1, 15, 2}, {5, 15, 2}, {9, 15, 2}, {13, 15, 2}, },
                {{0, 8, 2}, {1, 0, 8}, {1, 10, 6}, {4, 10, 1}, {5, 0, 10}, {5, 11, 5}, {8, 10, 2}, {9, 0, 10}, {9, 12, 4}, {12, 13, 1}, {13, 0, 13}, {13, 14, 2}, },
                {{2, 0, 2}, {2, 4, 12}, {3, 2, 2}, {6, 0, 3}, {6, 5, 11}, {7, 3, 2}, {10, 0, 2}, {10, 3, 13}, {11, 2, 1}, {14, 0, 6}, {14, 8, 8}, {15, 6, 2}, }
		}},
		{blocks::DEAD_BUSH, {
			{{11, 0, 1}, {4, 1, 1}, {10, 1, 1}, {8, 2, 1}, {5, 3, 1}, {9, 3, 1}, {6, 4, 1}, {11, 4, 1}, {7, 5, 1}, {2, 6, 3}, {9, 6, 1}, {13, 6, 2}, {5, 7, 1}, {11, 7, 2}, {6, 8, 1}, {10, 8, 1}, {4, 11, 2}, {10, 11, 1}, {6, 12, 2}, {11, 13, 2}, {10, 14, 1}, },
                {{11, 1, 1}, {4, 3, 1}, {10, 3, 1}, {9, 4, 1}, {11, 4, 1}, {5, 5, 1}, {2, 6, 1}, {6, 6, 1}, {14, 6, 1}, {3, 7, 2}, {12, 7, 2}, {5, 8, 1}, {11, 8, 1}, {6, 9, 1}, {10, 9, 1}, {7, 10, 1}, {4, 11, 1}, {10, 11, 1}, {5, 12, 1}, {6, 13, 1}, {12, 13, 1}, {10, 14, 2}, {7, 15, 3}, },
                {{2, 6, 1}, {3, 7, 1}, {4, 1, 3}, {4, 11, 1}, {5, 4, 2}, {5, 8, 1}, {5, 12, 1}, {6, 6, 1}, {6, 9, 1}, {6, 13, 1}, {7, 7, 1}, {7, 10, 1}, {7, 14, 2}, {8, 2, 3}, {8, 11, 1}, {10, 1, 2}, {11, 0, 1}, {11, 4, 1}, {11, 7, 1}, {11, 13, 1}, {13, 6, 1}, },
                {{4, 1, 2}, {4, 6, 1}, {5, 3, 1}, {5, 7, 1}, {5, 11, 1}, {6, 4, 1}, {8, 2, 1}, {8, 5, 1}, {9, 4, 1}, {9, 6, 2}, {9, 10, 1}, {9, 12, 2}, {9, 15, 1}, {10, 2, 2}, {10, 9, 1}, {10, 11, 1}, {11, 0, 2}, {11, 4, 1}, {11, 8, 1}, {11, 14, 1}, {12, 13, 1}, {13, 7, 1}, {14, 6, 1}, }
		}},
		{blocks::OAK_SAPLING, {
			{{11, 1, 1}, {7, 2, 1}, {10, 2, 1}, {4, 3, 1}, {6, 3, 1}, {12, 3, 2}, {5, 4, 1}, {8, 4, 2}, {2, 5, 2}, {7, 6, 1}, {14, 6, 1}, {8, 7, 1}, {5, 8, 1}, {11, 8, 2}, {3, 9, 2}, {10, 9, 1}, {13, 9, 1}, {2, 10, 1}, {14, 10, 1}, {1, 11, 1}, },
                {{7, 4, 1}, {2, 5, 1}, {8, 5, 1}, {11, 5, 1}, {3, 6, 3}, {10, 6, 1}, {12, 6, 1}, {13, 7, 2}, {4, 10, 1}, {12, 10, 1}, {1, 11, 3}, {11, 11, 1}, {13, 11, 1}, {14, 12, 1}, {5, 13, 2}, {10, 14, 1}, {7, 15, 3}, },
                {{1, 11, 1}, {2, 5, 1}, {2, 10, 1}, {3, 6, 1}, {3, 9, 1}, {4, 3, 2}, {5, 8, 1}, {5, 11, 3}, {6, 3, 1}, {6, 7, 1}, {7, 2, 1}, {7, 14, 2}, {8, 5, 1}, {9, 6, 1}, {10, 2, 2}, {11, 1, 1}, {11, 8, 1}, {12, 6, 1}, {13, 7, 1}, {13, 11, 1}, {14, 12, 1}, },
                {{3, 11, 1}, {4, 3, 1}, {6, 5, 1}, {7, 2, 2}, {7, 6, 1}, {9, 7, 2}, {9, 15, 1}, {10, 6, 1}, {10, 12, 3}, {11, 1, 2}, {11, 11, 1}, {12, 8, 1}, {13, 3, 3}, {13, 9, 1}, {14, 6, 2}, {14, 10, 3}, }
		}},
		{blocks::TORCH, {
			{{7, 6, 2}, },
                {{7, 15, 2}, },
                {{7, 6, 10}, },
                {{8, 6, 10}, }
		}},
		{blocks::REDSTONE_TORCH, {
			{{7, 5, 2}, {6, 6, 1}, {9, 6, 1}, },
                {{6, 7, 1}, {9, 7, 1}, {7, 15, 2}, },
                {{6, 6, 2}, {7, 5, 1}, {7, 8, 8}, },
                {{8, 5, 1}, {8, 8, 8}, {9, 6, 2}, }
		}},
		{blocks::REDSTONE_DUST, {
			{{7, 3, 2}, {6, 4, 1}, {9, 4, 1}, {5, 5, 1}, {10, 5, 1}, {4, 6, 1}, {11, 6, 1}, {3, 7, 1}, {12, 7, 1}, {2, 8, 1}, {13, 8, 1}, },
                {{2, 10, 1}, {13, 10, 1}, {3, 11, 1}, {12, 11, 1}, {4, 12, 2}, {10, 12, 2}, {6, 13, 4}, },
                {{2, 8, 3}, {3, 7, 1}, {3, 11, 1}, {4, 6, 1}, {4, 12, 1}, {5, 5, 1}, {6, 4, 1}, {6, 13, 1}, {7, 3, 1}, },
                {{8, 3, 1}, {9, 4, 1}, {9, 13, 1}, {10, 5, 1}, {11, 6, 1}, {11, 12, 1}, {12, 7, 1}, {12, 11, 1}, {13, 8, 3}, }
		}},
		{blocks::REPEATER, {
			{{12, 2, 2}, {2, 7, 2}, {8, 7, 3}, {6, 8, 2}, {4, 9, 2}, {1, 11, 1}, },
                {{12, 7, 2}, {9, 10, 2}, {7, 11, 2}, {5, 12, 2}, {1, 13, 4}, },
                {{1, 11, 3}, {2, 7, 4}, {6, 8, 1}, {8, 7, 1}, {12, 2, 6}, },
                {{3, 7, 2}, {4, 13, 1}, {6, 12, 1}, {8, 11, 1}, {10, 7, 4}, {13, 2, 6}, }
		}},
		{blocks::LEVER, {
			{{7, 6, 2}, },
                {{7, 15, 2}, },
                {{7, 6, 10}, },
                {{8, 6, 10}, }
		}},
		{blocks::STICK, {
			{{13, 2, 2}, {12, 3, 1}, {11, 4, 1}, {10, 5, 1}, {9, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{14, 3, 1}, {13, 4, 1}, {12, 5, 1}, {11, 6, 1}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 9, 1}, {7, 8, 1}, {8, 7, 1}, {9, 6, 1}, {10, 5, 1}, {11, 4, 1}, {12, 3, 1}, {13, 2, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 7, 1}, {11, 6, 1}, {12, 5, 1}, {13, 4, 1}, {14, 2, 2}, }
		}}, 
		{blocks::WOODEN_SHOVEL, {
			{{11, 2, 3}, {10, 3, 1}, {14, 3, 1}, {9, 4, 1}, {8, 5, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {2, 12, 2}, },
                {{8, 5, 1}, {14, 5, 1}, {13, 6, 1}, {10, 7, 1}, {12, 7, 1}, {9, 8, 1}, {11, 8, 1}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {2, 13, 1}, {3, 14, 2}, },
                {{2, 12, 2}, {3, 14, 1}, {4, 11, 1}, {5, 10, 1}, {6, 9, 1}, {7, 8, 1}, {8, 5, 1}, {8, 7, 1}, {9, 4, 1}, {9, 6, 1}, {10, 3, 1}, {11, 2, 1}, {11, 8, 1}, },
                {{4, 13, 2}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {11, 8, 1}, {12, 7, 1}, {13, 2, 1}, {13, 6, 1}, {14, 3, 3}, }
		}}, 
		{blocks::STONE_SHOVEL, {
			{{11, 2, 3}, {10, 3, 1}, {14, 3, 1}, {9, 4, 1}, {8, 5, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {2, 12, 2}, },
                {{8, 5, 1}, {14, 5, 1}, {13, 6, 1}, {10, 7, 1}, {12, 7, 1}, {9, 8, 1}, {11, 8, 1}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {2, 13, 1}, {3, 14, 2}, },
                {{2, 12, 2}, {3, 14, 1}, {4, 11, 1}, {5, 10, 1}, {6, 9, 1}, {7, 8, 1}, {8, 5, 1}, {8, 7, 1}, {9, 4, 1}, {9, 6, 1}, {10, 3, 1}, {11, 2, 1}, {11, 8, 1}, },
                {{4, 13, 2}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {11, 8, 1}, {12, 7, 1}, {13, 2, 1}, {13, 6, 1}, {14, 3, 3}, }
		}}, 
		{blocks::IRON_SHOVEL, {
			{{11, 2, 3}, {10, 3, 1}, {14, 3, 1}, {9, 4, 1}, {8, 5, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {2, 12, 2}, },
                {{8, 5, 1}, {14, 5, 1}, {13, 6, 1}, {10, 7, 1}, {12, 7, 1}, {9, 8, 1}, {11, 8, 1}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {2, 13, 1}, {3, 14, 2}, },
                {{2, 12, 2}, {3, 14, 1}, {4, 11, 1}, {5, 10, 1}, {6, 9, 1}, {7, 8, 1}, {8, 5, 1}, {8, 7, 1}, {9, 4, 1}, {9, 6, 1}, {10, 3, 1}, {11, 2, 1}, {11, 8, 1}, },
                {{4, 13, 2}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {11, 8, 1}, {12, 7, 1}, {13, 2, 1}, {13, 6, 1}, {14, 3, 3}, }
		}}, 
		{blocks::DIAMOND_SHOVEL, {
			{{11, 2, 3}, {10, 3, 1}, {14, 3, 1}, {9, 4, 1}, {8, 5, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {2, 12, 2}, },
                {{8, 5, 1}, {14, 5, 1}, {13, 6, 1}, {10, 7, 1}, {12, 7, 1}, {9, 8, 1}, {11, 8, 1}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {2, 13, 1}, {3, 14, 2}, },
                {{2, 12, 2}, {3, 14, 1}, {4, 11, 1}, {5, 10, 1}, {6, 9, 1}, {7, 8, 1}, {8, 5, 1}, {8, 7, 1}, {9, 4, 1}, {9, 6, 1}, {10, 3, 1}, {11, 2, 1}, {11, 8, 1}, },
                {{4, 13, 2}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {11, 8, 1}, {12, 7, 1}, {13, 2, 1}, {13, 6, 1}, {14, 3, 3}, }
		}}, 
		{blocks::WOODEN_AXE, {
			{{9, 1, 2}, {8, 2, 1}, {11, 2, 1}, {7, 3, 1}, {6, 4, 1}, {12, 4, 1}, {13, 6, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{6, 5, 1}, {7, 6, 1}, {10, 7, 1}, {13, 7, 1}, {9, 8, 1}, {11, 8, 2}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 4, 2}, {6, 9, 1}, {7, 3, 1}, {7, 6, 1}, {7, 8, 1}, {8, 2, 1}, {8, 7, 1}, {9, 1, 1}, {11, 8, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 1, 1}, {11, 2, 2}, {12, 4, 2}, {12, 8, 1}, {13, 6, 2}, }
		}}, 
		{blocks::WORLDEDIT_WAND, {
			{{9, 1, 2}, {8, 2, 1}, {11, 2, 1}, {7, 3, 1}, {6, 4, 1}, {12, 4, 1}, {13, 6, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{6, 5, 1}, {7, 6, 1}, {10, 7, 1}, {13, 7, 1}, {9, 8, 1}, {11, 8, 2}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 4, 2}, {6, 9, 1}, {7, 3, 1}, {7, 6, 1}, {7, 8, 1}, {8, 2, 1}, {8, 7, 1}, {9, 1, 1}, {11, 8, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 1, 1}, {11, 2, 2}, {12, 4, 2}, {12, 8, 1}, {13, 6, 2}, }
		}}, 
		{blocks::STONE_AXE, {
			{{9, 1, 2}, {8, 2, 1}, {11, 2, 1}, {7, 3, 1}, {6, 4, 1}, {12, 4, 1}, {13, 6, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{6, 5, 1}, {7, 6, 1}, {10, 7, 1}, {13, 7, 1}, {9, 8, 1}, {11, 8, 2}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 4, 2}, {6, 9, 1}, {7, 3, 1}, {7, 6, 1}, {7, 8, 1}, {8, 2, 1}, {8, 7, 1}, {9, 1, 1}, {11, 8, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 1, 1}, {11, 2, 2}, {12, 4, 2}, {12, 8, 1}, {13, 6, 2}, }
		}}, 
		{blocks::IRON_AXE, {
			{{9, 1, 2}, {8, 2, 1}, {11, 2, 1}, {7, 3, 1}, {6, 4, 1}, {12, 4, 1}, {13, 6, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{6, 5, 1}, {7, 6, 1}, {10, 7, 1}, {13, 7, 1}, {9, 8, 1}, {11, 8, 2}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 4, 2}, {6, 9, 1}, {7, 3, 1}, {7, 6, 1}, {7, 8, 1}, {8, 2, 1}, {8, 7, 1}, {9, 1, 1}, {11, 8, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 1, 1}, {11, 2, 2}, {12, 4, 2}, {12, 8, 1}, {13, 6, 2}, }
		}}, 
		{blocks::DIAMOND_AXE, {
			{{9, 1, 2}, {8, 2, 1}, {11, 2, 1}, {7, 3, 1}, {6, 4, 1}, {12, 4, 1}, {13, 6, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{6, 5, 1}, {7, 6, 1}, {10, 7, 1}, {13, 7, 1}, {9, 8, 1}, {11, 8, 2}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 4, 2}, {6, 9, 1}, {7, 3, 1}, {7, 6, 1}, {7, 8, 1}, {8, 2, 1}, {8, 7, 1}, {9, 1, 1}, {11, 8, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 1, 1}, {11, 2, 2}, {12, 4, 2}, {12, 8, 1}, {13, 6, 2}, }
		}}, 
		{blocks::WOODEN_PICKAXE, {
			{{6, 2, 5}, {5, 3, 1}, {11, 3, 3}, {9, 6, 1}, {14, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{5, 3, 1}, {6, 4, 4}, {11, 6, 1}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {12, 10, 1}, {14, 10, 1}, {6, 11, 1}, {13, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 3, 1}, {5, 10, 1}, {6, 2, 1}, {6, 4, 1}, {6, 9, 1}, {7, 8, 1}, {8, 7, 1}, {9, 6, 1}, {10, 5, 1}, {12, 7, 4}, {13, 11, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 2, 1}, {10, 7, 1}, {13, 3, 3}, {13, 11, 1}, {14, 6, 5}, }
		}}, 
		{blocks::STONE_PICKAXE, {
			{{6, 2, 5}, {5, 3, 1}, {11, 3, 3}, {9, 6, 1}, {14, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{5, 3, 1}, {6, 4, 4}, {11, 6, 1}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {12, 10, 1}, {14, 10, 1}, {6, 11, 1}, {13, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 3, 1}, {5, 10, 1}, {6, 2, 1}, {6, 4, 1}, {6, 9, 1}, {7, 8, 1}, {8, 7, 1}, {9, 6, 1}, {10, 5, 1}, {12, 7, 4}, {13, 11, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 2, 1}, {10, 7, 1}, {13, 3, 3}, {13, 11, 1}, {14, 6, 5}, }
		}}, 
		{blocks::IRON_PICKAXE, {
			{{6, 2, 5}, {5, 3, 1}, {11, 3, 3}, {9, 6, 1}, {14, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{5, 3, 1}, {6, 4, 4}, {11, 6, 1}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {12, 10, 1}, {14, 10, 1}, {6, 11, 1}, {13, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 3, 1}, {5, 10, 1}, {6, 2, 1}, {6, 4, 1}, {6, 9, 1}, {7, 8, 1}, {8, 7, 1}, {9, 6, 1}, {10, 5, 1}, {12, 7, 4}, {13, 11, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 2, 1}, {10, 7, 1}, {13, 3, 3}, {13, 11, 1}, {14, 6, 5}, }
		}}, 
		{blocks::DIAMOND_PICKAXE, {
			{{6, 2, 5}, {5, 3, 1}, {11, 3, 3}, {9, 6, 1}, {14, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{5, 3, 1}, {6, 4, 4}, {11, 6, 1}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {12, 10, 1}, {14, 10, 1}, {6, 11, 1}, {13, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 3, 1}, {5, 10, 1}, {6, 2, 1}, {6, 4, 1}, {6, 9, 1}, {7, 8, 1}, {8, 7, 1}, {9, 6, 1}, {10, 5, 1}, {12, 7, 4}, {13, 11, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 2, 1}, {10, 7, 1}, {13, 3, 3}, {13, 11, 1}, {14, 6, 5}, }
		}}, 
		{blocks::BOW, {
			{{11, 1, 4}, {8, 2, 3}, {15, 2, 1}, {6, 3, 2}, {5, 4, 1}, {4, 5, 1}, {12, 5, 1}, {3, 6, 1}, {11, 6, 1}, {10, 7, 1}, {2, 8, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {1, 11, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, },
                {{15, 2, 1}, {11, 3, 2}, {14, 3, 1}, {8, 4, 3}, {13, 4, 1}, {7, 5, 1}, {12, 5, 1}, {6, 6, 1}, {11, 6, 1}, {5, 7, 1}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {4, 10, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {1, 14, 1}, {3, 14, 1}, {2, 15, 1}, },
                {{1, 11, 4}, {2, 8, 3}, {2, 15, 1}, {3, 6, 2}, {4, 5, 1}, {5, 4, 1}, {5, 12, 1}, {6, 3, 1}, {6, 11, 1}, {7, 10, 1}, {8, 2, 1}, {8, 9, 1}, {9, 8, 1}, {10, 7, 1}, {11, 1, 1}, {11, 6, 1}, {12, 5, 1}, {13, 4, 1}, },
                {{2, 15, 1}, {3, 11, 2}, {3, 14, 1}, {4, 8, 3}, {4, 13, 1}, {5, 7, 1}, {5, 12, 1}, {6, 6, 1}, {6, 11, 1}, {7, 5, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 4, 1}, {10, 7, 1}, {11, 6, 1}, {12, 5, 1}, {13, 4, 1}, {14, 1, 1}, {14, 3, 1}, {15, 2, 1}, }
		}}, 
		{blocks::ARROW, {
			{{12, 2, 3}, {10, 3, 2}, {9, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {3, 11, 2}, {2, 12, 1}, },
                {{14, 3, 1}, {11, 5, 1}, {13, 5, 1}, {10, 6, 1}, {12, 6, 1}, {9, 7, 1}, {8, 8, 1}, {7, 9, 1}, {6, 10, 1}, {5, 12, 1}, {2, 13, 1}, {4, 13, 1}, {3, 14, 1}, },
                {{2, 12, 2}, {3, 11, 1}, {3, 14, 1}, {5, 10, 1}, {6, 9, 1}, {7, 8, 1}, {8, 7, 1}, {9, 6, 1}, {10, 3, 3}, {12, 2, 1}, {12, 6, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 11, 2}, {6, 10, 1}, {7, 9, 1}, {8, 8, 1}, {9, 7, 1}, {10, 6, 1}, {12, 6, 1}, {13, 4, 2}, {14, 2, 2}, }
		}}, 
		{blocks::COAL, {
			{{5, 2, 4}, {4, 3, 1}, {9, 3, 1}, {3, 4, 1}, {10, 4, 1}, {11, 5, 1}, {12, 6, 1}, {2, 7, 1}, {13, 7, 1}, {14, 9, 1}, },
                {{2, 11, 1}, {14, 11, 1}, {3, 12, 1}, {13, 12, 1}, {4, 13, 1}, {9, 13, 4}, {5, 14, 4}, },
                {{2, 7, 5}, {3, 4, 3}, {3, 12, 1}, {4, 3, 1}, {4, 13, 1}, {5, 2, 1}, {5, 14, 1}, },
                {{8, 2, 1}, {8, 14, 1}, {9, 3, 1}, {10, 4, 1}, {11, 5, 1}, {12, 6, 1}, {12, 13, 1}, {13, 7, 2}, {13, 12, 1}, {14, 9, 3}, }
		}}, 
		{blocks::CHARCOAL, {
			{{7, 1, 3}, {5, 2, 2}, {10, 2, 1}, {4, 3, 1}, {11, 3, 1}, {3, 4, 1}, {12, 4, 1}, {13, 5, 1}, {14, 6, 1}, {2, 7, 1}, },
                {{2, 11, 1}, {14, 11, 1}, {3, 12, 1}, {13, 12, 1}, {4, 13, 1}, {9, 13, 4}, {5, 14, 4}, },
                {{2, 7, 5}, {3, 4, 3}, {3, 12, 1}, {4, 3, 1}, {4, 13, 1}, {5, 2, 1}, {5, 14, 1}, {7, 1, 1}, },
                {{8, 14, 1}, {9, 1, 1}, {10, 2, 1}, {11, 3, 1}, {12, 4, 1}, {12, 13, 1}, {13, 5, 1}, {13, 12, 1}, {14, 6, 6}, }
		}}, 
		{blocks::IRON_INGOT, {
			{{10, 2, 2}, {7, 3, 3}, {12, 3, 1}, {4, 4, 3}, {13, 4, 1}, {1, 5, 3}, {14, 5, 1}, {0, 6, 1}, {15, 6, 1}, },
                {{15, 9, 1}, {0, 10, 1}, {13, 10, 2}, {1, 11, 1}, {10, 11, 3}, {2, 12, 1}, {7, 12, 3}, {3, 13, 4}, },
                {{0, 6, 5}, {1, 5, 1}, {1, 11, 1}, {2, 12, 1}, {3, 13, 1}, {4, 4, 1}, {7, 3, 1}, {10, 2, 1}, },
                {{6, 13, 1}, {9, 12, 1}, {11, 2, 1}, {12, 3, 1}, {12, 11, 1}, {13, 4, 1}, {14, 5, 1}, {14, 10, 1}, {15, 6, 4}, }
		}}, 
		{blocks::DIAMOND, {
			{{6, 2, 4}, {5, 3, 1}, {10, 3, 1}, {4, 4, 1}, {11, 4, 1}, {3, 5, 1}, {12, 5, 1}, {2, 7, 1}, {13, 7, 1}, },
                {{2, 10, 1}, {13, 10, 1}, {3, 12, 1}, {12, 12, 1}, {4, 13, 1}, {11, 13, 1}, {5, 14, 6}, },
                {{2, 7, 4}, {3, 5, 2}, {3, 11, 2}, {4, 4, 1}, {4, 13, 1}, {5, 3, 1}, {5, 14, 1}, {6, 2, 1}, },
                {{9, 2, 1}, {10, 3, 1}, {10, 14, 1}, {11, 4, 1}, {11, 13, 1}, {12, 5, 2}, {12, 11, 2}, {13, 7, 4}, }
		}}, 
		{blocks::BUCKET, {
			{{5, 1, 6}, {3, 2, 2}, {11, 2, 2}, {2, 3, 1}, {13, 3, 1}, },
                {{2, 10, 1}, {13, 10, 1}, {3, 12, 1}, {12, 12, 1}, {4, 13, 1}, {11, 13, 1}, {5, 14, 6}, },
                {{2, 3, 8}, {3, 2, 1}, {3, 11, 2}, {4, 13, 1}, {5, 1, 1}, {5, 14, 1}, },
                {{10, 1, 1}, {10, 14, 1}, {11, 13, 1}, {12, 2, 1}, {12, 11, 2}, {13, 3, 8}, }
		}}, 
		{blocks::WATER_BUCKET, {
			{{5, 1, 6}, {3, 2, 2}, {11, 2, 2}, {2, 3, 1}, {13, 3, 1}, },
                {{2, 10, 1}, {13, 10, 1}, {3, 12, 1}, {12, 12, 1}, {4, 13, 1}, {11, 13, 1}, {5, 14, 6}, },
                {{2, 3, 8}, {3, 2, 1}, {3, 11, 2}, {4, 13, 1}, {5, 1, 1}, {5, 14, 1}, },
                {{10, 1, 1}, {10, 14, 1}, {11, 13, 1}, {12, 2, 1}, {12, 11, 2}, {13, 3, 8}, }
		}}, 
		{blocks::WOODEN_HOE, {
			{{7, 1, 3}, {6, 2, 1}, {10, 2, 1}, {11, 3, 3}, {9, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{6, 2, 1}, {7, 3, 2}, {9, 4, 1}, {13, 5, 1}, {11, 6, 2}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 2, 1}, {6, 9, 1}, {7, 1, 1}, {7, 3, 1}, {7, 8, 1}, {8, 7, 1}, {9, 4, 1}, {9, 6, 1}, {10, 5, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 1, 1}, {9, 8, 1}, {10, 2, 1}, {10, 7, 1}, {12, 6, 1}, {13, 3, 3}, }
		}}, 
		{blocks::STONE_HOE, {
			{{7, 1, 3}, {6, 2, 1}, {10, 2, 1}, {11, 3, 3}, {9, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{6, 2, 1}, {7, 3, 2}, {9, 4, 1}, {13, 5, 1}, {11, 6, 2}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 2, 1}, {6, 9, 1}, {7, 1, 1}, {7, 3, 1}, {7, 8, 1}, {8, 7, 1}, {9, 4, 1}, {9, 6, 1}, {10, 5, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 1, 1}, {9, 8, 1}, {10, 2, 1}, {10, 7, 1}, {12, 6, 1}, {13, 3, 3}, }
		}}, 
		{blocks::IRON_HOE, {
			{{7, 1, 3}, {6, 2, 1}, {10, 2, 1}, {11, 3, 3}, {9, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{6, 2, 1}, {7, 3, 2}, {9, 4, 1}, {13, 5, 1}, {11, 6, 2}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 2, 1}, {6, 9, 1}, {7, 1, 1}, {7, 3, 1}, {7, 8, 1}, {8, 7, 1}, {9, 4, 1}, {9, 6, 1}, {10, 5, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 1, 1}, {9, 8, 1}, {10, 2, 1}, {10, 7, 1}, {12, 6, 1}, {13, 3, 3}, }
		}}, 
		{blocks::DIAMOND_HOE, {
			{{7, 1, 3}, {6, 2, 1}, {10, 2, 1}, {11, 3, 3}, {9, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{6, 2, 1}, {7, 3, 2}, {9, 4, 1}, {13, 5, 1}, {11, 6, 2}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 2, 1}, {6, 9, 1}, {7, 1, 1}, {7, 3, 1}, {7, 8, 1}, {8, 7, 1}, {9, 4, 1}, {9, 6, 1}, {10, 5, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 1, 1}, {9, 8, 1}, {10, 2, 1}, {10, 7, 1}, {12, 6, 1}, {13, 3, 3}, }
		}}, 
		{blocks::WHEAT_SEEDS, {
			{{5, 4, 1}, {11, 4, 1}, {12, 5, 1}, {13, 6, 1}, {4, 7, 1}, {8, 7, 2}, {6, 8, 1}, {3, 9, 1}, {8, 10, 1}, {11, 10, 2}, {7, 11, 1}, },
                {{11, 4, 1}, {5, 6, 1}, {12, 7, 2}, {8, 8, 2}, {6, 9, 1}, {3, 10, 2}, {11, 11, 2}, {7, 12, 2}, },
                {{3, 9, 2}, {4, 7, 2}, {5, 4, 3}, {6, 8, 2}, {7, 11, 2}, {8, 7, 2}, {8, 10, 1}, {11, 4, 1}, {11, 10, 2}, {12, 5, 3}, },
                {{4, 7, 4}, {5, 4, 3}, {6, 8, 2}, {8, 10, 3}, {9, 7, 2}, {11, 4, 1}, {12, 5, 1}, {12, 10, 2}, {13, 6, 2}, }
		}}, 
		{blocks::WHEAT, {
			{{10, 0, 1}, {8, 1, 2}, {12, 1, 1}, {6, 2, 1}, {11, 2, 1}, {15, 2, 1}, {7, 3, 1}, {12, 3, 3}, {14, 5, 2}, {5, 7, 1}, {15, 7, 1}, {4, 9, 1}, {3, 10, 1}, {1, 11, 2}, {13, 11, 1}, },
                {{12, 1, 1}, {15, 2, 1}, {14, 3, 1}, {15, 5, 1}, {15, 7, 1}, {13, 8, 2}, {9, 10, 2}, {7, 11, 2}, {11, 11, 3}, {6, 12, 1}, {2, 14, 2}, {5, 14, 1}, {1, 15, 1}, {4, 15, 1}, },
                {{1, 11, 5}, {3, 10, 1}, {4, 9, 1}, {4, 15, 1}, {5, 7, 2}, {6, 2, 5}, {8, 1, 2}, {10, 0, 1}, {11, 11, 1}, {12, 1, 1}, {15, 2, 1}, },
                {{1, 15, 1}, {4, 15, 1}, {5, 13, 2}, {6, 2, 1}, {6, 12, 1}, {8, 11, 1}, {10, 0, 2}, {11, 2, 1}, {12, 1, 1}, {12, 9, 2}, {13, 4, 1}, {13, 11, 1}, {14, 3, 1}, {14, 6, 1}, {14, 8, 1}, {15, 2, 1}, {15, 5, 1}, {15, 7, 1}, }
		}}, 
		{blocks::BREAD, {
			{{9, 1, 4}, {7, 2, 2}, {13, 2, 1}, {6, 3, 1}, {14, 3, 1}, {5, 4, 1}, {4, 5, 1}, {3, 6, 1}, {2, 7, 1}, {1, 9, 1}, },
                {{14, 7, 1}, {13, 8, 1}, {12, 9, 1}, {11, 10, 1}, {10, 11, 1}, {9, 12, 1}, {1, 13, 1}, {8, 13, 1}, {2, 14, 1}, {7, 14, 1}, {3, 15, 4}, },
                {{1, 9, 5}, {2, 7, 2}, {2, 14, 1}, {3, 6, 1}, {3, 15, 1}, {4, 5, 1}, {5, 4, 1}, {6, 3, 1}, {7, 2, 1}, {9, 1, 1}, },
                {{6, 15, 1}, {7, 14, 1}, {8, 13, 1}, {9, 12, 1}, {10, 11, 1}, {11, 10, 1}, {12, 1, 1}, {12, 9, 1}, {13, 2, 1}, {13, 8, 1}, {14, 3, 5}, }
		}}, 
		{blocks::APPLE, {
			{{9, 1, 1}, {8, 2, 1}, {5, 4, 3}, {9, 4, 2}, {3, 5, 2}, {11, 5, 2}, {2, 6, 1}, {13, 6, 1}, },
                {{9, 2, 1}, {2, 10, 1}, {13, 10, 1}, {3, 12, 1}, {12, 12, 1}, {4, 13, 1}, {11, 13, 1}, {5, 14, 6}, },
                {{2, 6, 5}, {3, 5, 1}, {3, 11, 2}, {4, 13, 1}, {5, 4, 1}, {5, 14, 1}, {8, 2, 2}, {9, 1, 1}, },
                {{8, 3, 1}, {9, 1, 2}, {10, 4, 1}, {10, 14, 1}, {11, 13, 1}, {12, 5, 1}, {12, 11, 2}, {13, 6, 5}, }
		}}, 
		{blocks::OAK_DOOR, {
			{{3, 2, 10}, {5, 5, 3}, {9, 5, 3}, {5, 8, 3}, {9, 8, 3}, },
                {{5, 2, 3}, {9, 2, 3}, {5, 5, 3}, {9, 5, 3}, {3, 15, 10}, },
                {{3, 2, 14}, {8, 3, 2}, {8, 6, 2}, {12, 3, 2}, {12, 6, 2}, },
                {{4, 3, 2}, {4, 6, 2}, {8, 3, 2}, {8, 6, 2}, {12, 2, 14}, }
		}},
		{blocks::FLINT, {
			{{8, 2, 3}, {7, 3, 1}, {11, 3, 1}, {6, 4, 1}, {5, 5, 1}, {12, 5, 1}, {4, 6, 1}, {3, 7, 1}, {13, 7, 1}, {2, 8, 1}, {14, 9, 1}, },
                {{14, 10, 1}, {2, 11, 1}, {13, 11, 1}, {3, 12, 1}, {12, 12, 1}, {4, 13, 1}, {11, 13, 1}, {5, 14, 6}, },
                {{2, 8, 4}, {3, 7, 1}, {3, 12, 1}, {4, 6, 1}, {4, 13, 1}, {5, 5, 1}, {5, 14, 1}, {6, 4, 1}, {7, 3, 1}, {8, 2, 1}, },
                {{10, 2, 1}, {10, 14, 1}, {11, 3, 2}, {11, 13, 1}, {12, 5, 2}, {12, 12, 1}, {13, 7, 2}, {13, 11, 1}, {14, 9, 2}, }
		}}, 
		{blocks::FLINT_AND_STEEL, {
			{{4, 1, 2}, {3, 2, 1}, {6, 2, 1}, {2, 3, 1}, {7, 3, 1}, {6, 7, 2}, {11, 7, 1}, {5, 8, 1}, {10, 8, 1}, {12, 8, 1}, {9, 9, 1}, {8, 10, 1}, {13, 10, 1}, {7, 11, 1}, {14, 12, 1}, },
                {{5, 3, 1}, {6, 4, 2}, {2, 8, 1}, {7, 8, 1}, {3, 9, 1}, {6, 9, 1}, {4, 10, 2}, {7, 13, 1}, {14, 13, 1}, {8, 14, 1}, {11, 14, 3}, {9, 15, 2}, },
                {{2, 3, 6}, {3, 2, 1}, {3, 9, 1}, {4, 1, 1}, {4, 10, 1}, {6, 4, 1}, {6, 7, 1}, {7, 11, 3}, {8, 10, 1}, {8, 14, 1}, {9, 9, 1}, {9, 15, 1}, {10, 8, 1}, {11, 7, 1}, },
                {{4, 4, 4}, {5, 1, 1}, {5, 10, 1}, {6, 2, 1}, {6, 9, 1}, {7, 3, 2}, {7, 7, 2}, {10, 15, 1}, {11, 7, 1}, {12, 8, 2}, {13, 10, 2}, {13, 14, 1}, {14, 12, 2}, }
		}}, 
	};

	/* return false if item not in map
	 * drawItem3D uses particle shader
	 * front, right, up should be normalized
	 */
	bool drawItem3D( std::vector<t_shaderInput> &arr, int type, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size )
	{
		auto search = exList.find(type);
		if (search == exList.end()) {
			return (false);
		}
		light <<= 24;

		int texStartX = s_blocks[type]->texX(face_dir::MINUSY, 2) * 16, texStartY = s_blocks[type]->texY(face_dir::MINUSY, 2) * 16;
		// front face
		int spec = texStartX + (texStartY << 8) + light;
		t_shaderInput v0 = {spec, pos};
		t_shaderInput v1 = {spec + 16 + (1 << 17), pos + right * size};
		t_shaderInput v2 = {spec + (16 << 8) + (1 << 18), pos - up * size};
		t_shaderInput v3 = {spec + 16 + (1 << 17) + (16 << 8) + (1 << 18), pos + right * size - up * size};
		arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
		// back face
		spec += 16;
		v0 = {spec + (1 << 17), pos + front * size / 16.0f + right * size};
		v1 = {spec - 16, pos + front * size / 16.0f};
		v2 = {spec + (1 << 17) + (16 << 8) + (1 << 18), pos + front * size / 16.0f + right * size - up * size};
		v3 = {spec - 16 + (16 << 8) + (1 << 18), pos + front * size / 16.0f - up * size};
		arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
		// up faces
		for (auto u : search->second.up) {
			spec = texStartX + u.x + ((texStartY + u.y) << 8) + light;
			v0 = {spec, pos + front * size / 16.0f + right * size * static_cast<float>(u.x) / 16.0f - up * size * static_cast<float>(u.y) / 16.0f};
			v1 = {spec + u.z + (1 << 17), pos + front * size / 16.0f + right * size * static_cast<float>(u.x + u.z) / 16.0f - up * size * static_cast<float>(u.y) / 16.0f};
			v2 = {spec + (1 << 8) + (1 << 18), pos + right * size * static_cast<float>(u.x) / 16.0f - up * size * static_cast<float>(u.y) / 16.0f};
			v3 = {spec + u.z + (1 << 17) + (1 << 8) + (1 << 18), pos + right * size * static_cast<float>(u.x + u.z) / 16.0f - up * size * static_cast<float>(u.y) / 16.0f};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
		}
		// down faces
		for (auto d : search->second.down) {
			spec = texStartX + d.x + ((texStartY + d.y) << 8) + light;
			v0 = {spec, pos + right * size * static_cast<float>(d.x) / 16.0f - up * size * static_cast<float>(d.y + 1) / 16.0f};
			v1 = {spec + d.z + (1 << 17), pos + right * size * static_cast<float>(d.x + d.z) / 16.0f - up * size * static_cast<float>(d.y + 1) / 16.0f};
			v2 = {spec + (1 << 8) + (1 << 18), pos + front * size / 16.0f + right * size * static_cast<float>(d.x) / 16.0f - up * size * static_cast<float>(d.y + 1) / 16.0f};
			v3 = {spec + d.z + (1 << 17) + (1 << 8) + (1 << 18),  pos + front * size / 16.0f + right * size * static_cast<float>(d.x + d.z) / 16.0f - up * size * static_cast<float>(d.y + 1) / 16.0f};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
		}
		// left faces
		for (auto l : search->second.left) {
			spec = texStartX + l.x + ((texStartY + l.y) << 8) + light;
			v0 = {spec, pos + front * size / 16.0f + right * size * static_cast<float>(l.x) / 16.0f - up * size * static_cast<float>(l.y) / 16.0f};
			v1 = {spec + 1 + (1 << 17), pos + right * size * static_cast<float>(l.x) / 16.0f - up * size * static_cast<float>(l.y) / 16.0f};
			v2 = {spec + (l.z << 8) + (1 << 18), pos + front * size / 16.0f + right * size * static_cast<float>(l.x) / 16.0f - up * size * static_cast<float>(l.y + l.z) / 16.0f};
			v3 = {spec + 1 + (1 << 17) + (l.z << 8) + (1 << 18), pos + right * size * static_cast<float>(l.x) / 16.0f - up * size * static_cast<float>(l.y + l.z) / 16.0f};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
		}
		// right faces
		for (auto r : search->second.right) {
			spec = texStartX + r.x + ((texStartY + r.y) << 8) + light;
			v0 = {spec, pos + right * size * static_cast<float>(r.x + 1) / 16.0f - up * size * static_cast<float>(r.y) / 16.0f};
			v1 = {spec + 1 + (1 << 17), pos + front * size / 16.0f + right * size * static_cast<float>(r.x + 1) / 16.0f - up * size * static_cast<float>(r.y) / 16.0f};
			v2 = {spec + (r.z << 8) + (1 << 18), pos + right * size * static_cast<float>(r.x + 1) / 16.0f - up * size * static_cast<float>(r.y + r.z) / 16.0f};
			v3 = {spec + 1 + (1 << 17) + (r.z << 8) + (1 << 18), pos + front * size / 16.0f + right * size * static_cast<float>(r.x + 1) / 16.0f - up * size * static_cast<float>(r.y + r.z) / 16.0f};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
		}
		return (true);
	}
}
