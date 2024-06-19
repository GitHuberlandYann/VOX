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
		{blocks::glass_pane, {
			{{0, 0, 16}, {4, 2, 1}, {3, 3, 1}, {2, 4, 1}, {13, 12, 1}, {12, 13, 1}, {1, 15, 14}, },
			{{1, 0, 14}, {4, 2, 1}, {3, 3, 1}, {2, 4, 1}, {13, 12, 1}, {12, 13, 1}, {0, 15, 16}, },
			{{0, 0, 16}, {2, 4, 1}, {3, 3, 1}, {4, 2, 1}, {12, 13, 1}, {13, 12, 1}, {15, 1, 14}, },
			{{0, 1, 14}, {2, 4, 1}, {3, 3, 1}, {4, 2, 1}, {12, 13, 1}, {13, 12, 1}, {15, 0, 16}, }
		}},
		{blocks::poppy, {
			{{8, 5, 2}, {6, 6, 2}, {10, 6, 1}, {5, 7, 1}, {5, 13, 2}, {10, 13, 2}, {9, 14, 1}, {7, 15, 1}},
			{{10, 7, 1}, {5, 8, 1}, {9, 8, 1}, {6, 9, 2}, {5, 13, 1}, {10, 13, 2}, {6, 14, 1}, {7, 15, 3}},
			{{5, 7, 2}, {5, 13, 1}, {6, 6, 1}, {6, 9, 1}, {6, 14, 1}, {7, 15, 1}, {8, 5, 1}, {8, 10, 5}, {10, 13, 1}},
			{{6, 13, 2}, {8, 9, 5}, {9, 5, 1}, {9, 8, 1}, {9, 14, 2}, {10, 6, 2}, {11, 13, 1}}
		}},
		{blocks::dandelion, {
			{{7, 7, 2}, {6, 8, 1}, {9, 8, 1}, {5, 9, 1}, {10, 9, 1}, {5, 12, 2}, {9, 13, 2}, },
                {{5, 9, 1}, {10, 9, 1}, {6, 10, 1}, {9, 10, 1}, {5, 12, 1}, {6, 13, 1}, {10, 13, 1}, {9, 14, 1}, {7, 15, 2}, },
                {{5, 9, 1}, {5, 12, 1}, {6, 8, 1}, {6, 10, 1}, {6, 13, 1}, {7, 7, 1}, {7, 11, 1}, {7, 14, 2}, },
                {{8, 7, 1}, {8, 11, 2}, {8, 15, 1}, {9, 8, 1}, {9, 10, 1}, {9, 14, 1}, {10, 9, 1}, {10, 13, 1}, }
		}},
		{blocks::blue_orchid, {
			{{6, 4, 2}, {5, 5, 1}, {8, 5, 1}, {13, 5, 1}, {12, 6, 1}, {14, 6, 1}, {8, 8, 1}, {11, 8, 1}, {3, 9, 2}, {10, 9, 1}, {2, 10, 1}, {5, 10, 1}, {7, 10, 1}, {9, 10, 1}, {6, 11, 1}, {7, 12, 1}, {9, 14, 1}, },
                {{5, 6, 1}, {8, 6, 1}, {14, 6, 1}, {6, 7, 2}, {12, 7, 2}, {11, 8, 1}, {2, 10, 1}, {4, 10, 2}, {7, 10, 1}, {10, 10, 1}, {3, 11, 1}, {9, 11, 1}, {6, 12, 1}, {7, 13, 1}, {9, 14, 1}, {8, 15, 1}, },
                {{2, 10, 1}, {3, 9, 1}, {3, 11, 1}, {5, 5, 2}, {6, 4, 1}, {6, 7, 1}, {6, 11, 2}, {7, 10, 1}, {7, 13, 1}, {8, 8, 2}, {8, 11, 1}, {8, 14, 2}, {10, 9, 1}, {11, 8, 1}, {12, 6, 2}, {13, 5, 1}, },
                {{3, 11, 1}, {4, 9, 1}, {5, 10, 1}, {6, 11, 1}, {7, 4, 1}, {7, 7, 1}, {8, 5, 2}, {8, 8, 2}, {8, 12, 2}, {8, 15, 1}, {9, 11, 1}, {9, 14, 1}, {10, 9, 2}, {11, 8, 1}, {13, 5, 1}, {13, 7, 1}, {14, 6, 1}, }
		}},
		{blocks::allium, {
			{{6, 2, 3}, {5, 3, 1}, {9, 3, 1}, {4, 4, 1}, {10, 4, 1}, },
                {{4, 5, 1}, {10, 5, 1}, {5, 6, 1}, {9, 6, 1}, {6, 7, 1}, {8, 7, 1}, {7, 15, 1}, },
                {{4, 4, 2}, {5, 3, 1}, {5, 6, 1}, {6, 2, 1}, {6, 7, 1}, {7, 8, 8}, },
                {{7, 8, 8}, {8, 2, 1}, {8, 7, 1}, {9, 3, 1}, {9, 6, 1}, {10, 4, 2}, }
		}},
		{blocks::cornflower, {
			{{5, 4, 1}, {7, 4, 3}, {4, 5, 1}, {6, 5, 1}, {10, 5, 1}, {4, 10, 1}, {5, 11, 1}, {6, 12, 1}, {7, 13, 1}, },
                {{4, 5, 1}, {5, 6, 2}, {9, 6, 2}, {7, 8, 1}, {4, 10, 1}, {5, 12, 1}, {6, 13, 1}, {7, 14, 1}, {8, 15, 1}, },
                {{4, 5, 1}, {4, 10, 1}, {5, 4, 1}, {5, 6, 1}, {5, 11, 2}, {6, 13, 1}, {7, 4, 1}, {7, 7, 2}, {7, 14, 1}, {8, 9, 4}, {8, 15, 1}, },
                {{4, 10, 1}, {5, 4, 1}, {5, 11, 1}, {6, 12, 1}, {8, 7, 9}, {9, 4, 1}, {10, 5, 2}, }
		}},
		{blocks::pink_tulip, {
			{{6, 3, 1}, {8, 3, 1}, {7, 4, 1}, {5, 7, 1}, {6, 8, 1}, {9, 8, 1}, {8, 10, 1}, },
                {{6, 5, 1}, {8, 5, 1}, {5, 12, 1}, {9, 12, 1}, {6, 14, 1}, {8, 14, 1}, {7, 15, 1}, },
                {{5, 7, 6}, {6, 3, 3}, {6, 13, 2}, {7, 6, 2}, {7, 15, 1}, {8, 3, 1}, {9, 8, 2}, },
                {{5, 7, 1}, {6, 3, 1}, {7, 6, 4}, {7, 15, 1}, {8, 3, 3}, {8, 13, 2}, {9, 8, 5}, }
		}},
		{blocks::grass, {
			{{4, 2, 1}, {7, 2, 1}, {10, 2, 1}, {13, 2, 1}, {5, 3, 1}, {12, 3, 1}, {3, 4, 1}, {9, 4, 1}, {1, 5, 1}, {4, 5, 1}, {11, 5, 1}, {14, 5, 1}, {2, 6, 1}, {8, 6, 1}, {13, 6, 1}, {6, 7, 1}, {3, 8, 1}, {7, 8, 1}, {10, 8, 1}, {0, 9, 1}, {12, 9, 1}, {14, 9, 1}, {6, 10, 1}, {9, 10, 1}, {15, 10, 1}, {1, 11, 1}, {4, 11, 1}, {13, 11, 1}, {2, 12, 1}, {7, 12, 1}, {11, 12, 1}, {14, 12, 1}, {1, 13, 1}, {15, 14, 1}, {0, 15, 1}, },
                {{4, 2, 1}, {13, 2, 1}, {3, 4, 1}, {10, 4, 1}, {12, 4, 1}, {1, 5, 1}, {14, 5, 1}, {4, 6, 1}, {7, 6, 1}, {6, 8, 1}, {9, 8, 1}, {13, 8, 1}, {7, 9, 1}, {11, 9, 1}, {2, 10, 1}, {14, 10, 1}, {1, 11, 1}, {15, 12, 1}, {0, 13, 1}, {0, 15, 16}, },
                {{0, 9, 5}, {0, 15, 1}, {1, 5, 1}, {1, 14, 1}, {2, 6, 5}, {2, 12, 1}, {3, 4, 1}, {3, 11, 1}, {4, 2, 1}, {4, 5, 2}, {5, 3, 2}, {5, 7, 4}, {7, 2, 5}, {7, 9, 1}, {8, 7, 1}, {8, 10, 2}, {9, 4, 2}, {10, 2, 2}, {10, 9, 1}, {11, 5, 3}, {12, 3, 2}, {12, 10, 2}, {13, 2, 1}, {13, 6, 3}, {14, 5, 1}, {14, 9, 2}, {15, 11, 1}, },
                {{0, 9, 2}, {0, 12, 1}, {1, 5, 1}, {1, 11, 1}, {2, 6, 2}, {3, 4, 1}, {3, 8, 3}, {4, 2, 1}, {5, 3, 4}, {5, 9, 1}, {6, 7, 1}, {6, 10, 2}, {7, 2, 4}, {8, 9, 1}, {9, 5, 3}, {10, 2, 3}, {10, 10, 2}, {11, 5, 4}, {12, 3, 2}, {12, 9, 2}, {13, 2, 1}, {13, 6, 3}, {13, 11, 1}, {14, 5, 1}, {14, 9, 1}, {14, 13, 1}, {15, 10, 3}, {15, 14, 2}, }
		}},
		{blocks::sugar_cane, {
			{{1, 0, 2}, {5, 0, 2}, {9, 0, 2}, {13, 0, 2}, {3, 2, 1}, {11, 2, 1}, {7, 3, 1}, {15, 6, 1}, {0, 8, 1}, {4, 10, 1}, {8, 10, 1}, {12, 13, 1}, },
                {{11, 2, 1}, {3, 3, 1}, {7, 4, 1}, {15, 7, 1}, {0, 9, 1}, {4, 10, 1}, {8, 11, 1}, {12, 13, 1}, {1, 15, 2}, {5, 15, 2}, {9, 15, 2}, {13, 15, 2}, },
                {{0, 8, 2}, {1, 0, 8}, {1, 10, 6}, {4, 10, 1}, {5, 0, 10}, {5, 11, 5}, {8, 10, 2}, {9, 0, 10}, {9, 12, 4}, {12, 13, 1}, {13, 0, 13}, {13, 14, 2}, },
                {{2, 0, 2}, {2, 4, 12}, {3, 2, 2}, {6, 0, 3}, {6, 5, 11}, {7, 3, 2}, {10, 0, 2}, {10, 3, 13}, {11, 2, 1}, {14, 0, 6}, {14, 8, 8}, {15, 6, 2}, }
		}},
		{blocks::dead_bush, {
			{{11, 0, 1}, {4, 1, 1}, {10, 1, 1}, {8, 2, 1}, {5, 3, 1}, {9, 3, 1}, {6, 4, 1}, {11, 4, 1}, {7, 5, 1}, {2, 6, 3}, {9, 6, 1}, {13, 6, 2}, {5, 7, 1}, {11, 7, 2}, {6, 8, 1}, {10, 8, 1}, {4, 11, 2}, {10, 11, 1}, {6, 12, 2}, {11, 13, 2}, {10, 14, 1}, },
                {{11, 1, 1}, {4, 3, 1}, {10, 3, 1}, {9, 4, 1}, {11, 4, 1}, {5, 5, 1}, {2, 6, 1}, {6, 6, 1}, {14, 6, 1}, {3, 7, 2}, {12, 7, 2}, {5, 8, 1}, {11, 8, 1}, {6, 9, 1}, {10, 9, 1}, {7, 10, 1}, {4, 11, 1}, {10, 11, 1}, {5, 12, 1}, {6, 13, 1}, {12, 13, 1}, {10, 14, 2}, {7, 15, 3}, },
                {{2, 6, 1}, {3, 7, 1}, {4, 1, 3}, {4, 11, 1}, {5, 4, 2}, {5, 8, 1}, {5, 12, 1}, {6, 6, 1}, {6, 9, 1}, {6, 13, 1}, {7, 7, 1}, {7, 10, 1}, {7, 14, 2}, {8, 2, 3}, {8, 11, 1}, {10, 1, 2}, {11, 0, 1}, {11, 4, 1}, {11, 7, 1}, {11, 13, 1}, {13, 6, 1}, },
                {{4, 1, 2}, {4, 6, 1}, {5, 3, 1}, {5, 7, 1}, {5, 11, 1}, {6, 4, 1}, {8, 2, 1}, {8, 5, 1}, {9, 4, 1}, {9, 6, 2}, {9, 10, 1}, {9, 12, 2}, {9, 15, 1}, {10, 2, 2}, {10, 9, 1}, {10, 11, 1}, {11, 0, 2}, {11, 4, 1}, {11, 8, 1}, {11, 14, 1}, {12, 13, 1}, {13, 7, 1}, {14, 6, 1}, }
		}},
		{blocks::oak_sapling, {
			{{11, 1, 1}, {7, 2, 1}, {10, 2, 1}, {4, 3, 1}, {6, 3, 1}, {12, 3, 2}, {5, 4, 1}, {8, 4, 2}, {2, 5, 2}, {7, 6, 1}, {14, 6, 1}, {8, 7, 1}, {5, 8, 1}, {11, 8, 2}, {3, 9, 2}, {10, 9, 1}, {13, 9, 1}, {2, 10, 1}, {14, 10, 1}, {1, 11, 1}, },
                {{7, 4, 1}, {2, 5, 1}, {8, 5, 1}, {11, 5, 1}, {3, 6, 3}, {10, 6, 1}, {12, 6, 1}, {13, 7, 2}, {4, 10, 1}, {12, 10, 1}, {1, 11, 3}, {11, 11, 1}, {13, 11, 1}, {14, 12, 1}, {5, 13, 2}, {10, 14, 1}, {7, 15, 3}, },
                {{1, 11, 1}, {2, 5, 1}, {2, 10, 1}, {3, 6, 1}, {3, 9, 1}, {4, 3, 2}, {5, 8, 1}, {5, 11, 3}, {6, 3, 1}, {6, 7, 1}, {7, 2, 1}, {7, 14, 2}, {8, 5, 1}, {9, 6, 1}, {10, 2, 2}, {11, 1, 1}, {11, 8, 1}, {12, 6, 1}, {13, 7, 1}, {13, 11, 1}, {14, 12, 1}, },
                {{3, 11, 1}, {4, 3, 1}, {6, 5, 1}, {7, 2, 2}, {7, 6, 1}, {9, 7, 2}, {9, 15, 1}, {10, 6, 1}, {10, 12, 3}, {11, 1, 2}, {11, 11, 1}, {12, 8, 1}, {13, 3, 3}, {13, 9, 1}, {14, 6, 2}, {14, 10, 3}, }
		}},
		{blocks::torch, {
			{{7, 6, 2}, },
                {{7, 15, 2}, },
                {{7, 6, 10}, },
                {{8, 6, 10}, }
		}},
		{blocks::redstone_torch, {
			{{7, 5, 2}, {6, 6, 1}, {9, 6, 1}, },
                {{6, 7, 1}, {9, 7, 1}, {7, 15, 2}, },
                {{6, 6, 2}, {7, 5, 1}, {7, 8, 8}, },
                {{8, 5, 1}, {8, 8, 8}, {9, 6, 2}, }
		}},
		{blocks::redstone_dust, {
			{{7, 3, 2}, {6, 4, 1}, {9, 4, 1}, {5, 5, 1}, {10, 5, 1}, {4, 6, 1}, {11, 6, 1}, {3, 7, 1}, {12, 7, 1}, {2, 8, 1}, {13, 8, 1}, },
                {{2, 10, 1}, {13, 10, 1}, {3, 11, 1}, {12, 11, 1}, {4, 12, 2}, {10, 12, 2}, {6, 13, 4}, },
                {{2, 8, 3}, {3, 7, 1}, {3, 11, 1}, {4, 6, 1}, {4, 12, 1}, {5, 5, 1}, {6, 4, 1}, {6, 13, 1}, {7, 3, 1}, },
                {{8, 3, 1}, {9, 4, 1}, {9, 13, 1}, {10, 5, 1}, {11, 6, 1}, {11, 12, 1}, {12, 7, 1}, {12, 11, 1}, {13, 8, 3}, }
		}},
		{blocks::repeater, {
			{{12, 2, 2}, {2, 7, 2}, {8, 7, 3}, {6, 8, 2}, {4, 9, 2}, {1, 11, 1}, },
                {{12, 7, 2}, {9, 10, 2}, {7, 11, 2}, {5, 12, 2}, {1, 13, 4}, },
                {{1, 11, 3}, {2, 7, 4}, {6, 8, 1}, {8, 7, 1}, {12, 2, 6}, },
                {{3, 7, 2}, {4, 13, 1}, {6, 12, 1}, {8, 11, 1}, {10, 7, 4}, {13, 2, 6}, }
		}},
		{blocks::lever, {
			{{7, 6, 2}, },
                {{7, 15, 2}, },
                {{7, 6, 10}, },
                {{8, 6, 10}, }
		}},
		{blocks::oak_sign, {
			{{2, 2, 13}, },
                {{2, 10, 5}, {10, 10, 5}, {7, 15, 3}, },
                {{2, 2, 9}, {7, 11, 5}, },
                {{9, 11, 5}, {14, 2, 9}, }
		}},
		{blocks::stick, {
			{{13, 2, 2}, {12, 3, 1}, {11, 4, 1}, {10, 5, 1}, {9, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{14, 3, 1}, {13, 4, 1}, {12, 5, 1}, {11, 6, 1}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 9, 1}, {7, 8, 1}, {8, 7, 1}, {9, 6, 1}, {10, 5, 1}, {11, 4, 1}, {12, 3, 1}, {13, 2, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 7, 1}, {11, 6, 1}, {12, 5, 1}, {13, 4, 1}, {14, 2, 2}, }
		}}, 
		{blocks::wooden_shovel, {
			{{11, 2, 3}, {10, 3, 1}, {14, 3, 1}, {9, 4, 1}, {8, 5, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {2, 12, 2}, },
                {{8, 5, 1}, {14, 5, 1}, {13, 6, 1}, {10, 7, 1}, {12, 7, 1}, {9, 8, 1}, {11, 8, 1}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {2, 13, 1}, {3, 14, 2}, },
                {{2, 12, 2}, {3, 14, 1}, {4, 11, 1}, {5, 10, 1}, {6, 9, 1}, {7, 8, 1}, {8, 5, 1}, {8, 7, 1}, {9, 4, 1}, {9, 6, 1}, {10, 3, 1}, {11, 2, 1}, {11, 8, 1}, },
                {{4, 13, 2}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {11, 8, 1}, {12, 7, 1}, {13, 2, 1}, {13, 6, 1}, {14, 3, 3}, }
		}}, 
		{blocks::stone_shovel, {
			{{11, 2, 3}, {10, 3, 1}, {14, 3, 1}, {9, 4, 1}, {8, 5, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {2, 12, 2}, },
                {{8, 5, 1}, {14, 5, 1}, {13, 6, 1}, {10, 7, 1}, {12, 7, 1}, {9, 8, 1}, {11, 8, 1}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {2, 13, 1}, {3, 14, 2}, },
                {{2, 12, 2}, {3, 14, 1}, {4, 11, 1}, {5, 10, 1}, {6, 9, 1}, {7, 8, 1}, {8, 5, 1}, {8, 7, 1}, {9, 4, 1}, {9, 6, 1}, {10, 3, 1}, {11, 2, 1}, {11, 8, 1}, },
                {{4, 13, 2}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {11, 8, 1}, {12, 7, 1}, {13, 2, 1}, {13, 6, 1}, {14, 3, 3}, }
		}}, 
		{blocks::iron_shovel, {
			{{11, 2, 3}, {10, 3, 1}, {14, 3, 1}, {9, 4, 1}, {8, 5, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {2, 12, 2}, },
                {{8, 5, 1}, {14, 5, 1}, {13, 6, 1}, {10, 7, 1}, {12, 7, 1}, {9, 8, 1}, {11, 8, 1}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {2, 13, 1}, {3, 14, 2}, },
                {{2, 12, 2}, {3, 14, 1}, {4, 11, 1}, {5, 10, 1}, {6, 9, 1}, {7, 8, 1}, {8, 5, 1}, {8, 7, 1}, {9, 4, 1}, {9, 6, 1}, {10, 3, 1}, {11, 2, 1}, {11, 8, 1}, },
                {{4, 13, 2}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {11, 8, 1}, {12, 7, 1}, {13, 2, 1}, {13, 6, 1}, {14, 3, 3}, }
		}}, 
		{blocks::diamond_shovel, {
			{{11, 2, 3}, {10, 3, 1}, {14, 3, 1}, {9, 4, 1}, {8, 5, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {2, 12, 2}, },
                {{8, 5, 1}, {14, 5, 1}, {13, 6, 1}, {10, 7, 1}, {12, 7, 1}, {9, 8, 1}, {11, 8, 1}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {2, 13, 1}, {3, 14, 2}, },
                {{2, 12, 2}, {3, 14, 1}, {4, 11, 1}, {5, 10, 1}, {6, 9, 1}, {7, 8, 1}, {8, 5, 1}, {8, 7, 1}, {9, 4, 1}, {9, 6, 1}, {10, 3, 1}, {11, 2, 1}, {11, 8, 1}, },
                {{4, 13, 2}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {11, 8, 1}, {12, 7, 1}, {13, 2, 1}, {13, 6, 1}, {14, 3, 3}, }
		}}, 
		{blocks::wooden_axe, {
			{{9, 1, 2}, {8, 2, 1}, {11, 2, 1}, {7, 3, 1}, {6, 4, 1}, {12, 4, 1}, {13, 6, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{6, 5, 1}, {7, 6, 1}, {10, 7, 1}, {13, 7, 1}, {9, 8, 1}, {11, 8, 2}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 4, 2}, {6, 9, 1}, {7, 3, 1}, {7, 6, 1}, {7, 8, 1}, {8, 2, 1}, {8, 7, 1}, {9, 1, 1}, {11, 8, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 1, 1}, {11, 2, 2}, {12, 4, 2}, {12, 8, 1}, {13, 6, 2}, }
		}}, 
		{blocks::worldedit_wand, {
			{{9, 1, 2}, {8, 2, 1}, {11, 2, 1}, {7, 3, 1}, {6, 4, 1}, {12, 4, 1}, {13, 6, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{6, 5, 1}, {7, 6, 1}, {10, 7, 1}, {13, 7, 1}, {9, 8, 1}, {11, 8, 2}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 4, 2}, {6, 9, 1}, {7, 3, 1}, {7, 6, 1}, {7, 8, 1}, {8, 2, 1}, {8, 7, 1}, {9, 1, 1}, {11, 8, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 1, 1}, {11, 2, 2}, {12, 4, 2}, {12, 8, 1}, {13, 6, 2}, }
		}}, 
		{blocks::stone_axe, {
			{{9, 1, 2}, {8, 2, 1}, {11, 2, 1}, {7, 3, 1}, {6, 4, 1}, {12, 4, 1}, {13, 6, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{6, 5, 1}, {7, 6, 1}, {10, 7, 1}, {13, 7, 1}, {9, 8, 1}, {11, 8, 2}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 4, 2}, {6, 9, 1}, {7, 3, 1}, {7, 6, 1}, {7, 8, 1}, {8, 2, 1}, {8, 7, 1}, {9, 1, 1}, {11, 8, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 1, 1}, {11, 2, 2}, {12, 4, 2}, {12, 8, 1}, {13, 6, 2}, }
		}}, 
		{blocks::iron_axe, {
			{{9, 1, 2}, {8, 2, 1}, {11, 2, 1}, {7, 3, 1}, {6, 4, 1}, {12, 4, 1}, {13, 6, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{6, 5, 1}, {7, 6, 1}, {10, 7, 1}, {13, 7, 1}, {9, 8, 1}, {11, 8, 2}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 4, 2}, {6, 9, 1}, {7, 3, 1}, {7, 6, 1}, {7, 8, 1}, {8, 2, 1}, {8, 7, 1}, {9, 1, 1}, {11, 8, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 1, 1}, {11, 2, 2}, {12, 4, 2}, {12, 8, 1}, {13, 6, 2}, }
		}}, 
		{blocks::diamond_axe, {
			{{9, 1, 2}, {8, 2, 1}, {11, 2, 1}, {7, 3, 1}, {6, 4, 1}, {12, 4, 1}, {13, 6, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{6, 5, 1}, {7, 6, 1}, {10, 7, 1}, {13, 7, 1}, {9, 8, 1}, {11, 8, 2}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 4, 2}, {6, 9, 1}, {7, 3, 1}, {7, 6, 1}, {7, 8, 1}, {8, 2, 1}, {8, 7, 1}, {9, 1, 1}, {11, 8, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 1, 1}, {11, 2, 2}, {12, 4, 2}, {12, 8, 1}, {13, 6, 2}, }
		}}, 
		{blocks::wooden_pickaxe, {
			{{6, 2, 5}, {5, 3, 1}, {11, 3, 3}, {9, 6, 1}, {14, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{5, 3, 1}, {6, 4, 4}, {11, 6, 1}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {12, 10, 1}, {14, 10, 1}, {6, 11, 1}, {13, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 3, 1}, {5, 10, 1}, {6, 2, 1}, {6, 4, 1}, {6, 9, 1}, {7, 8, 1}, {8, 7, 1}, {9, 6, 1}, {10, 5, 1}, {12, 7, 4}, {13, 11, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 2, 1}, {10, 7, 1}, {13, 3, 3}, {13, 11, 1}, {14, 6, 5}, }
		}}, 
		{blocks::stone_pickaxe, {
			{{6, 2, 5}, {5, 3, 1}, {11, 3, 3}, {9, 6, 1}, {14, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{5, 3, 1}, {6, 4, 4}, {11, 6, 1}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {12, 10, 1}, {14, 10, 1}, {6, 11, 1}, {13, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 3, 1}, {5, 10, 1}, {6, 2, 1}, {6, 4, 1}, {6, 9, 1}, {7, 8, 1}, {8, 7, 1}, {9, 6, 1}, {10, 5, 1}, {12, 7, 4}, {13, 11, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 2, 1}, {10, 7, 1}, {13, 3, 3}, {13, 11, 1}, {14, 6, 5}, }
		}}, 
		{blocks::iron_pickaxe, {
			{{6, 2, 5}, {5, 3, 1}, {11, 3, 3}, {9, 6, 1}, {14, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{5, 3, 1}, {6, 4, 4}, {11, 6, 1}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {12, 10, 1}, {14, 10, 1}, {6, 11, 1}, {13, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 3, 1}, {5, 10, 1}, {6, 2, 1}, {6, 4, 1}, {6, 9, 1}, {7, 8, 1}, {8, 7, 1}, {9, 6, 1}, {10, 5, 1}, {12, 7, 4}, {13, 11, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 2, 1}, {10, 7, 1}, {13, 3, 3}, {13, 11, 1}, {14, 6, 5}, }
		}}, 
		{blocks::diamond_pickaxe, {
			{{6, 2, 5}, {5, 3, 1}, {11, 3, 3}, {9, 6, 1}, {14, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{5, 3, 1}, {6, 4, 4}, {11, 6, 1}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {12, 10, 1}, {14, 10, 1}, {6, 11, 1}, {13, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 3, 1}, {5, 10, 1}, {6, 2, 1}, {6, 4, 1}, {6, 9, 1}, {7, 8, 1}, {8, 7, 1}, {9, 6, 1}, {10, 5, 1}, {12, 7, 4}, {13, 11, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 2, 1}, {10, 7, 1}, {13, 3, 3}, {13, 11, 1}, {14, 6, 5}, }
		}}, 
		{blocks::bow, {
			{{11, 1, 4}, {8, 2, 3}, {15, 2, 1}, {6, 3, 2}, {5, 4, 1}, {4, 5, 1}, {12, 5, 1}, {3, 6, 1}, {11, 6, 1}, {10, 7, 1}, {2, 8, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {1, 11, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, },
                {{15, 2, 1}, {11, 3, 2}, {14, 3, 1}, {8, 4, 3}, {13, 4, 1}, {7, 5, 1}, {12, 5, 1}, {6, 6, 1}, {11, 6, 1}, {5, 7, 1}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {4, 10, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {1, 14, 1}, {3, 14, 1}, {2, 15, 1}, },
                {{1, 11, 4}, {2, 8, 3}, {2, 15, 1}, {3, 6, 2}, {4, 5, 1}, {5, 4, 1}, {5, 12, 1}, {6, 3, 1}, {6, 11, 1}, {7, 10, 1}, {8, 2, 1}, {8, 9, 1}, {9, 8, 1}, {10, 7, 1}, {11, 1, 1}, {11, 6, 1}, {12, 5, 1}, {13, 4, 1}, },
                {{2, 15, 1}, {3, 11, 2}, {3, 14, 1}, {4, 8, 3}, {4, 13, 1}, {5, 7, 1}, {5, 12, 1}, {6, 6, 1}, {6, 11, 1}, {7, 5, 1}, {7, 10, 1}, {8, 9, 1}, {9, 8, 1}, {10, 4, 1}, {10, 7, 1}, {11, 6, 1}, {12, 5, 1}, {13, 4, 1}, {14, 1, 1}, {14, 3, 1}, {15, 2, 1}, }
		}}, 
		{blocks::arrow, {
			{{12, 2, 3}, {10, 3, 2}, {9, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {3, 11, 2}, {2, 12, 1}, },
                {{14, 3, 1}, {11, 5, 1}, {13, 5, 1}, {10, 6, 1}, {12, 6, 1}, {9, 7, 1}, {8, 8, 1}, {7, 9, 1}, {6, 10, 1}, {5, 12, 1}, {2, 13, 1}, {4, 13, 1}, {3, 14, 1}, },
                {{2, 12, 2}, {3, 11, 1}, {3, 14, 1}, {5, 10, 1}, {6, 9, 1}, {7, 8, 1}, {8, 7, 1}, {9, 6, 1}, {10, 3, 3}, {12, 2, 1}, {12, 6, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 11, 2}, {6, 10, 1}, {7, 9, 1}, {8, 8, 1}, {9, 7, 1}, {10, 6, 1}, {12, 6, 1}, {13, 4, 2}, {14, 2, 2}, }
		}}, 
		{blocks::coal, {
			{{5, 2, 4}, {4, 3, 1}, {9, 3, 1}, {3, 4, 1}, {10, 4, 1}, {11, 5, 1}, {12, 6, 1}, {2, 7, 1}, {13, 7, 1}, {14, 9, 1}, },
                {{2, 11, 1}, {14, 11, 1}, {3, 12, 1}, {13, 12, 1}, {4, 13, 1}, {9, 13, 4}, {5, 14, 4}, },
                {{2, 7, 5}, {3, 4, 3}, {3, 12, 1}, {4, 3, 1}, {4, 13, 1}, {5, 2, 1}, {5, 14, 1}, },
                {{8, 2, 1}, {8, 14, 1}, {9, 3, 1}, {10, 4, 1}, {11, 5, 1}, {12, 6, 1}, {12, 13, 1}, {13, 7, 2}, {13, 12, 1}, {14, 9, 3}, }
		}}, 
		{blocks::charcoal, {
			{{7, 1, 3}, {5, 2, 2}, {10, 2, 1}, {4, 3, 1}, {11, 3, 1}, {3, 4, 1}, {12, 4, 1}, {13, 5, 1}, {14, 6, 1}, {2, 7, 1}, },
                {{2, 11, 1}, {14, 11, 1}, {3, 12, 1}, {13, 12, 1}, {4, 13, 1}, {9, 13, 4}, {5, 14, 4}, },
                {{2, 7, 5}, {3, 4, 3}, {3, 12, 1}, {4, 3, 1}, {4, 13, 1}, {5, 2, 1}, {5, 14, 1}, {7, 1, 1}, },
                {{8, 14, 1}, {9, 1, 1}, {10, 2, 1}, {11, 3, 1}, {12, 4, 1}, {12, 13, 1}, {13, 5, 1}, {13, 12, 1}, {14, 6, 6}, }
		}}, 
		{blocks::iron_ingot, {
			{{10, 2, 2}, {7, 3, 3}, {12, 3, 1}, {4, 4, 3}, {13, 4, 1}, {1, 5, 3}, {14, 5, 1}, {0, 6, 1}, {15, 6, 1}, },
                {{15, 9, 1}, {0, 10, 1}, {13, 10, 2}, {1, 11, 1}, {10, 11, 3}, {2, 12, 1}, {7, 12, 3}, {3, 13, 4}, },
                {{0, 6, 5}, {1, 5, 1}, {1, 11, 1}, {2, 12, 1}, {3, 13, 1}, {4, 4, 1}, {7, 3, 1}, {10, 2, 1}, },
                {{6, 13, 1}, {9, 12, 1}, {11, 2, 1}, {12, 3, 1}, {12, 11, 1}, {13, 4, 1}, {14, 5, 1}, {14, 10, 1}, {15, 6, 4}, }
		}}, 
		{blocks::diamond, {
			{{6, 2, 4}, {5, 3, 1}, {10, 3, 1}, {4, 4, 1}, {11, 4, 1}, {3, 5, 1}, {12, 5, 1}, {2, 7, 1}, {13, 7, 1}, },
                {{2, 10, 1}, {13, 10, 1}, {3, 12, 1}, {12, 12, 1}, {4, 13, 1}, {11, 13, 1}, {5, 14, 6}, },
                {{2, 7, 4}, {3, 5, 2}, {3, 11, 2}, {4, 4, 1}, {4, 13, 1}, {5, 3, 1}, {5, 14, 1}, {6, 2, 1}, },
                {{9, 2, 1}, {10, 3, 1}, {10, 14, 1}, {11, 4, 1}, {11, 13, 1}, {12, 5, 2}, {12, 11, 2}, {13, 7, 4}, }
		}}, 
		{blocks::bucket, {
			{{5, 1, 6}, {3, 2, 2}, {11, 2, 2}, {2, 3, 1}, {13, 3, 1}, },
                {{2, 10, 1}, {13, 10, 1}, {3, 12, 1}, {12, 12, 1}, {4, 13, 1}, {11, 13, 1}, {5, 14, 6}, },
                {{2, 3, 8}, {3, 2, 1}, {3, 11, 2}, {4, 13, 1}, {5, 1, 1}, {5, 14, 1}, },
                {{10, 1, 1}, {10, 14, 1}, {11, 13, 1}, {12, 2, 1}, {12, 11, 2}, {13, 3, 8}, }
		}}, 
		{blocks::water_bucket, {
			{{5, 1, 6}, {3, 2, 2}, {11, 2, 2}, {2, 3, 1}, {13, 3, 1}, },
                {{2, 10, 1}, {13, 10, 1}, {3, 12, 1}, {12, 12, 1}, {4, 13, 1}, {11, 13, 1}, {5, 14, 6}, },
                {{2, 3, 8}, {3, 2, 1}, {3, 11, 2}, {4, 13, 1}, {5, 1, 1}, {5, 14, 1}, },
                {{10, 1, 1}, {10, 14, 1}, {11, 13, 1}, {12, 2, 1}, {12, 11, 2}, {13, 3, 8}, }
		}}, 
		{blocks::wooden_hoe, {
			{{7, 1, 3}, {6, 2, 1}, {10, 2, 1}, {11, 3, 3}, {9, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{6, 2, 1}, {7, 3, 2}, {9, 4, 1}, {13, 5, 1}, {11, 6, 2}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 2, 1}, {6, 9, 1}, {7, 1, 1}, {7, 3, 1}, {7, 8, 1}, {8, 7, 1}, {9, 4, 1}, {9, 6, 1}, {10, 5, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 1, 1}, {9, 8, 1}, {10, 2, 1}, {10, 7, 1}, {12, 6, 1}, {13, 3, 3}, }
		}}, 
		{blocks::stone_hoe, {
			{{7, 1, 3}, {6, 2, 1}, {10, 2, 1}, {11, 3, 3}, {9, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{6, 2, 1}, {7, 3, 2}, {9, 4, 1}, {13, 5, 1}, {11, 6, 2}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 2, 1}, {6, 9, 1}, {7, 1, 1}, {7, 3, 1}, {7, 8, 1}, {8, 7, 1}, {9, 4, 1}, {9, 6, 1}, {10, 5, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 1, 1}, {9, 8, 1}, {10, 2, 1}, {10, 7, 1}, {12, 6, 1}, {13, 3, 3}, }
		}}, 
		{blocks::iron_hoe, {
			{{7, 1, 3}, {6, 2, 1}, {10, 2, 1}, {11, 3, 3}, {9, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{6, 2, 1}, {7, 3, 2}, {9, 4, 1}, {13, 5, 1}, {11, 6, 2}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 2, 1}, {6, 9, 1}, {7, 1, 1}, {7, 3, 1}, {7, 8, 1}, {8, 7, 1}, {9, 4, 1}, {9, 6, 1}, {10, 5, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 1, 1}, {9, 8, 1}, {10, 2, 1}, {10, 7, 1}, {12, 6, 1}, {13, 3, 3}, }
		}}, 
		{blocks::diamond_hoe, {
			{{7, 1, 3}, {6, 2, 1}, {10, 2, 1}, {11, 3, 3}, {9, 6, 1}, {8, 7, 1}, {7, 8, 1}, {6, 9, 1}, {5, 10, 1}, {4, 11, 1}, {3, 12, 1}, {2, 13, 1}, },
                {{6, 2, 1}, {7, 3, 2}, {9, 4, 1}, {13, 5, 1}, {11, 6, 2}, {10, 7, 1}, {9, 8, 1}, {8, 9, 1}, {7, 10, 1}, {6, 11, 1}, {5, 12, 1}, {4, 13, 1}, {2, 14, 2}, },
                {{2, 13, 2}, {3, 12, 1}, {4, 11, 1}, {5, 10, 1}, {6, 2, 1}, {6, 9, 1}, {7, 1, 1}, {7, 3, 1}, {7, 8, 1}, {8, 7, 1}, {9, 4, 1}, {9, 6, 1}, {10, 5, 1}, },
                {{3, 14, 1}, {4, 13, 1}, {5, 12, 1}, {6, 11, 1}, {7, 10, 1}, {8, 9, 1}, {9, 1, 1}, {9, 8, 1}, {10, 2, 1}, {10, 7, 1}, {12, 6, 1}, {13, 3, 3}, }
		}}, 
		{blocks::wheat_seeds, {
			{{5, 4, 1}, {11, 4, 1}, {12, 5, 1}, {13, 6, 1}, {4, 7, 1}, {8, 7, 2}, {6, 8, 1}, {3, 9, 1}, {8, 10, 1}, {11, 10, 2}, {7, 11, 1}, },
                {{11, 4, 1}, {5, 6, 1}, {12, 7, 2}, {8, 8, 2}, {6, 9, 1}, {3, 10, 2}, {11, 11, 2}, {7, 12, 2}, },
                {{3, 9, 2}, {4, 7, 2}, {5, 4, 3}, {6, 8, 2}, {7, 11, 2}, {8, 7, 2}, {8, 10, 1}, {11, 4, 1}, {11, 10, 2}, {12, 5, 3}, },
                {{4, 7, 4}, {5, 4, 3}, {6, 8, 2}, {8, 10, 3}, {9, 7, 2}, {11, 4, 1}, {12, 5, 1}, {12, 10, 2}, {13, 6, 2}, }
		}}, 
		{blocks::wheat, {
			{{10, 0, 1}, {8, 1, 2}, {12, 1, 1}, {6, 2, 1}, {11, 2, 1}, {15, 2, 1}, {7, 3, 1}, {12, 3, 3}, {14, 5, 2}, {5, 7, 1}, {15, 7, 1}, {4, 9, 1}, {3, 10, 1}, {1, 11, 2}, {13, 11, 1}, },
                {{12, 1, 1}, {15, 2, 1}, {14, 3, 1}, {15, 5, 1}, {15, 7, 1}, {13, 8, 2}, {9, 10, 2}, {7, 11, 2}, {11, 11, 3}, {6, 12, 1}, {2, 14, 2}, {5, 14, 1}, {1, 15, 1}, {4, 15, 1}, },
                {{1, 11, 5}, {3, 10, 1}, {4, 9, 1}, {4, 15, 1}, {5, 7, 2}, {6, 2, 5}, {8, 1, 2}, {10, 0, 1}, {11, 11, 1}, {12, 1, 1}, {15, 2, 1}, },
                {{1, 15, 1}, {4, 15, 1}, {5, 13, 2}, {6, 2, 1}, {6, 12, 1}, {8, 11, 1}, {10, 0, 2}, {11, 2, 1}, {12, 1, 1}, {12, 9, 2}, {13, 4, 1}, {13, 11, 1}, {14, 3, 1}, {14, 6, 1}, {14, 8, 1}, {15, 2, 1}, {15, 5, 1}, {15, 7, 1}, }
		}}, 
		{blocks::bread, {
			{{9, 1, 4}, {7, 2, 2}, {13, 2, 1}, {6, 3, 1}, {14, 3, 1}, {5, 4, 1}, {4, 5, 1}, {3, 6, 1}, {2, 7, 1}, {1, 9, 1}, },
                {{14, 7, 1}, {13, 8, 1}, {12, 9, 1}, {11, 10, 1}, {10, 11, 1}, {9, 12, 1}, {1, 13, 1}, {8, 13, 1}, {2, 14, 1}, {7, 14, 1}, {3, 15, 4}, },
                {{1, 9, 5}, {2, 7, 2}, {2, 14, 1}, {3, 6, 1}, {3, 15, 1}, {4, 5, 1}, {5, 4, 1}, {6, 3, 1}, {7, 2, 1}, {9, 1, 1}, },
                {{6, 15, 1}, {7, 14, 1}, {8, 13, 1}, {9, 12, 1}, {10, 11, 1}, {11, 10, 1}, {12, 1, 1}, {12, 9, 1}, {13, 2, 1}, {13, 8, 1}, {14, 3, 5}, }
		}}, 
		{blocks::apple, {
			{{9, 1, 1}, {8, 2, 1}, {5, 4, 3}, {9, 4, 2}, {3, 5, 2}, {11, 5, 2}, {2, 6, 1}, {13, 6, 1}, },
                {{9, 2, 1}, {2, 10, 1}, {13, 10, 1}, {3, 12, 1}, {12, 12, 1}, {4, 13, 1}, {11, 13, 1}, {5, 14, 6}, },
                {{2, 6, 5}, {3, 5, 1}, {3, 11, 2}, {4, 13, 1}, {5, 4, 1}, {5, 14, 1}, {8, 2, 2}, {9, 1, 1}, },
                {{8, 3, 1}, {9, 1, 2}, {10, 4, 1}, {10, 14, 1}, {11, 13, 1}, {12, 5, 1}, {12, 11, 2}, {13, 6, 5}, }
		}}, 
		{blocks::oak_door, {
			{{3, 2, 10}, {5, 5, 3}, {9, 5, 3}, {5, 8, 3}, {9, 8, 3}, },
                {{5, 2, 3}, {9, 2, 3}, {5, 5, 3}, {9, 5, 3}, {3, 15, 10}, },
                {{3, 2, 14}, {8, 3, 2}, {8, 6, 2}, {12, 3, 2}, {12, 6, 2}, },
                {{4, 3, 2}, {4, 6, 2}, {8, 3, 2}, {8, 6, 2}, {12, 2, 14}, }
		}},
		{blocks::flint, {
			{{8, 2, 3}, {7, 3, 1}, {11, 3, 1}, {6, 4, 1}, {5, 5, 1}, {12, 5, 1}, {4, 6, 1}, {3, 7, 1}, {13, 7, 1}, {2, 8, 1}, {14, 9, 1}, },
                {{14, 10, 1}, {2, 11, 1}, {13, 11, 1}, {3, 12, 1}, {12, 12, 1}, {4, 13, 1}, {11, 13, 1}, {5, 14, 6}, },
                {{2, 8, 4}, {3, 7, 1}, {3, 12, 1}, {4, 6, 1}, {4, 13, 1}, {5, 5, 1}, {5, 14, 1}, {6, 4, 1}, {7, 3, 1}, {8, 2, 1}, },
                {{10, 2, 1}, {10, 14, 1}, {11, 3, 2}, {11, 13, 1}, {12, 5, 2}, {12, 12, 1}, {13, 7, 2}, {13, 11, 1}, {14, 9, 2}, }
		}}, 
		{blocks::flint_and_steel, {
			{{4, 1, 2}, {3, 2, 1}, {6, 2, 1}, {2, 3, 1}, {7, 3, 1}, {6, 7, 2}, {11, 7, 1}, {5, 8, 1}, {10, 8, 1}, {12, 8, 1}, {9, 9, 1}, {8, 10, 1}, {13, 10, 1}, {7, 11, 1}, {14, 12, 1}, },
                {{5, 3, 1}, {6, 4, 2}, {2, 8, 1}, {7, 8, 1}, {3, 9, 1}, {6, 9, 1}, {4, 10, 2}, {7, 13, 1}, {14, 13, 1}, {8, 14, 1}, {11, 14, 3}, {9, 15, 2}, },
                {{2, 3, 6}, {3, 2, 1}, {3, 9, 1}, {4, 1, 1}, {4, 10, 1}, {6, 4, 1}, {6, 7, 1}, {7, 11, 3}, {8, 10, 1}, {8, 14, 1}, {9, 9, 1}, {9, 15, 1}, {10, 8, 1}, {11, 7, 1}, },
                {{4, 4, 4}, {5, 1, 1}, {5, 10, 1}, {6, 2, 1}, {6, 9, 1}, {7, 3, 2}, {7, 7, 2}, {10, 15, 1}, {11, 7, 1}, {12, 8, 2}, {13, 10, 2}, {13, 14, 1}, {14, 12, 2}, }
		}}, 
		{blocks::zombie_egg, {
			{{7, 2, 2}, {5, 3, 2}, {9, 3, 2}, {4, 4, 1}, {11, 4, 1}, {3, 6, 1}, {12, 6, 1}, {2, 8, 1}, {13, 8, 1}, },
                {{2, 10, 1}, {13, 10, 1}, {3, 12, 1}, {12, 12, 1}, {4, 13, 2}, {10, 13, 2}, {6, 14, 4}, },
                {{2, 8, 3}, {3, 6, 2}, {3, 11, 2}, {4, 4, 2}, {4, 13, 1}, {5, 3, 1}, {6, 14, 1}, {7, 2, 1}, },
                {{8, 2, 1}, {9, 14, 1}, {10, 3, 1}, {11, 4, 2}, {11, 13, 1}, {12, 6, 2}, {12, 11, 2}, {13, 8, 3}, }
		}},
		{blocks::skeleton_egg, {
			{{7, 2, 2}, {5, 3, 2}, {9, 3, 2}, {4, 4, 1}, {11, 4, 1}, {3, 6, 1}, {12, 6, 1}, {2, 8, 1}, {13, 8, 1}, },
                {{2, 10, 1}, {13, 10, 1}, {3, 12, 1}, {12, 12, 1}, {4, 13, 2}, {10, 13, 2}, {6, 14, 4}, },
                {{2, 8, 3}, {3, 6, 2}, {3, 11, 2}, {4, 4, 2}, {4, 13, 1}, {5, 3, 1}, {6, 14, 1}, {7, 2, 1}, },
                {{8, 2, 1}, {9, 14, 1}, {10, 3, 1}, {11, 4, 2}, {11, 13, 1}, {12, 6, 2}, {12, 11, 2}, {13, 8, 3}, }
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

		int texStartX = s_blocks[type]->texX(face_dir::minus_y, blocks::item) * 16, texStartY = s_blocks[type]->texY(face_dir::minus_y, blocks::item) * 16;
		// front face
		int spec = texStartX + (texStartY << 8) + light;
		utils::shader::addQuads(arr, {pos, pos + right * size, pos - up * size, pos + (right - up) * size}, spec, 16, 16 << 8);
		// back face
		spec += 16;
		utils::shader::addQuads(arr, {pos + front * size * one16th + right * size, pos + front * size * one16th, pos + front * size * one16th + (right - up) * size, pos + front * size * one16th - up * size}, spec + (1 << 17), -16 - (2 << 17), 16 << 8);
		// up faces
		for (auto u : search->second.up) {
			spec = texStartX + u.x + ((texStartY + u.y) << 8) + light;
			utils::shader::addQuads(arr, {pos + (front + right * static_cast<float>(u.x) - up * static_cast<float>(u.y)) * size * one16th,
										pos + (front + right * static_cast<float>(u.x + u.z) - up * static_cast<float>(u.y)) * size * one16th,
										pos + (right * static_cast<float>(u.x) - up * static_cast<float>(u.y)) * size * one16th,
										pos + (right * static_cast<float>(u.x + u.z) - up * static_cast<float>(u.y)) * size * one16th}, spec, u.z, 1 << 8);
		}
		// down faces
		for (auto d : search->second.down) {
			spec = texStartX + d.x + ((texStartY + d.y) << 8) + light;
			utils::shader::addQuads(arr, {pos + (right * static_cast<float>(d.x) - up * static_cast<float>(d.y + 1)) * size * one16th,
										pos + (right * static_cast<float>(d.x + d.z) - up * static_cast<float>(d.y + 1)) * size * one16th,
										pos + (front + right * static_cast<float>(d.x) - up * static_cast<float>(d.y + 1)) * size * one16th,
										pos + (front + right * static_cast<float>(d.x + d.z) - up * static_cast<float>(d.y + 1)) * size * one16th}, spec, d.z, 1 << 8);
		}
		// left faces
		for (auto l : search->second.left) {
			spec = texStartX + l.x + ((texStartY + l.y) << 8) + light;
			utils::shader::addQuads(arr, {pos + (front + right * static_cast<float>(l.x) - up * static_cast<float>(l.y)) * size * one16th,
										pos + (right * static_cast<float>(l.x) - up * static_cast<float>(l.y)) * size * one16th,
										pos + (front + right * static_cast<float>(l.x) - up * static_cast<float>(l.y + l.z)) * size * one16th,
										pos + (right * static_cast<float>(l.x) - up * static_cast<float>(l.y + l.z)) * size * one16th}, spec, 1, l.z << 8);
		}
		// right faces
		for (auto r : search->second.right) {
			spec = texStartX + r.x + ((texStartY + r.y) << 8) + light;
			utils::shader::addQuads(arr, {pos + (right * static_cast<float>(r.x + 1) - up * static_cast<float>(r.y)) * size * one16th,
										pos + (front + right * static_cast<float>(r.x + 1) - up * static_cast<float>(r.y)) * size * one16th,
										pos + (right * static_cast<float>(r.x + 1) - up * static_cast<float>(r.y + r.z)) * size * one16th,
										pos + (front + right * static_cast<float>(r.x + 1) - up * static_cast<float>(r.y + r.z)) * size * one16th}, spec, 1, r.z << 8);
		}
		return (true);
	}
}
