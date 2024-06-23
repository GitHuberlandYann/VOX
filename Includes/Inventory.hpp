#ifndef INVENTORY_HPP
# define INVENTORY_HPP

# include "Blocks.hpp"
# include <list>
# include <array>
# include <map>
class Chunk;
class FurnaceInstance;
class ChestInstance;
class UI;

const std::map<int, std::vector<int>> recipes = {
	{1 + (1 << 2), { // 1x1
		blocks::oak_log, blocks::oak_planks, 4,

		blocks::coal_block, blocks::coal, 9,
		blocks::iron_block, blocks::iron_ingot, 9,
		blocks::diamond_block, blocks::diamond, 9,

		blocks::stone, blocks::stone_button, 1,
		blocks::oak_planks, blocks::oak_button, 1,
	}},

	{1 + (2 << 2), { // 1x2
		blocks::oak_planks, blocks::oak_planks, blocks::stick, 4,
		blocks::coal, blocks::stick, blocks::torch, 4,
		blocks::charcoal, blocks::stick, blocks::torch, 4,
		blocks::redstone_dust, blocks::stick, blocks::redstone_torch, 4,
		blocks::stick, blocks::cobblestone, blocks::lever, 1,
	}},

	{1 + (3 << 2), { // 1x3
		blocks::oak_planks, blocks::stick, blocks::stick, blocks::wooden_shovel, 1,
		blocks::cobblestone, blocks::stick, blocks::stick, blocks::stone_shovel, 1,
		blocks::iron_ingot, blocks::stick, blocks::stick, blocks::iron_shovel, 1,
		blocks::diamond, blocks::stick, blocks::stick, blocks::diamond_shovel, 1,
	}},

	{2 + (1 << 2), { // 2x1
		blocks::white_wool, blocks::white_wool, blocks::white_carpet, 2,
	}},

	{2 + (2 << 2), { // 2x2
		blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::crafting_table, 1,
		blocks::stone, blocks::stone, blocks::stone, blocks::stone, blocks::stone_bricks, 4,
		blocks::string, blocks::string, blocks::string, blocks::string, blocks::white_wool, 1,
		blocks::iron_ingot, blocks::air, blocks::air, blocks::flint, blocks::flint_and_steel, 1,
	}},

	{2 + (3 << 2), { // 2x3
		blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::oak_door, 3,
		blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::stick, blocks::air, blocks::stick, blocks::wooden_axe, 1,
		blocks::cobblestone, blocks::cobblestone, blocks::cobblestone, blocks::stick, blocks::air, blocks::stick, blocks::stone_axe, 1,
		blocks::iron_ingot, blocks::iron_ingot, blocks::iron_ingot, blocks::stick, blocks::air, blocks::stick, blocks::iron_axe, 1,
		blocks::diamond, blocks::diamond, blocks::diamond, blocks::stick, blocks::air, blocks::stick, blocks::diamond_axe, 1,
		blocks::oak_planks, blocks::oak_planks, blocks::air, blocks::stick, blocks::air, blocks::stick, blocks::wooden_hoe, 1,
		blocks::cobblestone, blocks::cobblestone, blocks::air, blocks::stick, blocks::air, blocks::stick, blocks::stone_hoe, 1,
		blocks::iron_ingot, blocks::iron_ingot, blocks::air, blocks::stick, blocks::air, blocks::stick, blocks::iron_hoe, 1,
		blocks::diamond, blocks::diamond, blocks::air, blocks::stick, blocks::air, blocks::stick, blocks::diamond_hoe, 1,
	}},

	{3 + (1 << 2), { // 3x1
		blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::oak_slab, 6,
		blocks::stone, blocks::stone, blocks::stone, blocks::stone_slab, 6,
		blocks::smooth_stone, blocks::smooth_stone, blocks::smooth_stone, blocks::smooth_stone_slab, 6,
		blocks::cobblestone, blocks::cobblestone, blocks::cobblestone, blocks::cobblestone_slab, 6,
		blocks::stone_bricks, blocks::stone_bricks, blocks::stone_bricks, blocks::stone_bricks_slab, 6,
		blocks::wheat, blocks::wheat, blocks::wheat, blocks::bread, 1,
	}},

	{3 + (2 << 2), { // 3x2
		blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::oak_trapdoor, 2,
		blocks::oak_planks, blocks::stick, blocks::oak_planks, blocks::oak_planks, blocks::stick, blocks::oak_planks, blocks::oak_fence, 3,
		blocks::glass, blocks::glass, blocks::glass, blocks::glass, blocks::glass, blocks::glass, blocks::glass_pane, 16,
		blocks::iron_ingot, blocks::air, blocks::iron_ingot, blocks::air, blocks::iron_ingot, blocks::air, blocks::bucket, 1,
	}},

	{3 + (3 << 2), { // 3x3
		blocks::oak_planks, blocks::air, blocks::air, blocks::oak_planks, blocks::oak_planks, blocks::air, blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::oak_stairs_bottom, 4,
		blocks::stone, blocks::air, blocks::air, blocks::stone, blocks::stone, blocks::air, blocks::stone, blocks::stone, blocks::stone, blocks::stone_stairs_bottom, 4,
		blocks::smooth_stone, blocks::air, blocks::air, blocks::smooth_stone, blocks::smooth_stone, blocks::air, blocks::smooth_stone, blocks::smooth_stone, blocks::smooth_stone, blocks::smooth_stone_stairs_bottom, 4,
		blocks::cobblestone, blocks::air, blocks::air, blocks::cobblestone, blocks::cobblestone, blocks::air, blocks::cobblestone, blocks::cobblestone, blocks::cobblestone, blocks::cobblestone_stairs_bottom, 4,
		blocks::stone_bricks, blocks::air, blocks::air, blocks::stone_bricks, blocks::stone_bricks, blocks::air, blocks::stone_bricks, blocks::stone_bricks, blocks::stone_bricks, blocks::stone_bricks_stairs_bottom, 4,
		
		blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::air, blocks::stick, blocks::air, blocks::oak_sign, 3,
	
		blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::air, blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::chest, 1,
		blocks::cobblestone, blocks::cobblestone, blocks::cobblestone, blocks::cobblestone, blocks::air, blocks::cobblestone, blocks::cobblestone, blocks::cobblestone, blocks::cobblestone, blocks::furnace, 1,

		blocks::oak_planks, blocks::oak_planks, blocks::oak_planks, blocks::air, blocks::stick, blocks::air, blocks::air, blocks::stick, blocks::air, blocks::wooden_pickaxe, 1,
		blocks::cobblestone, blocks::cobblestone, blocks::cobblestone, blocks::air, blocks::stick, blocks::air, blocks::air, blocks::stick, blocks::air, blocks::stone_pickaxe, 1,
		blocks::iron_ingot, blocks::iron_ingot, blocks::iron_ingot, blocks::air, blocks::stick, blocks::air, blocks::air, blocks::stick, blocks::air, blocks::iron_pickaxe, 1,
		blocks::diamond, blocks::diamond, blocks::diamond, blocks::air, blocks::stick, blocks::air, blocks::air, blocks::stick, blocks::air, blocks::diamond_pickaxe, 1,
		blocks::air, blocks::stick, blocks::string, blocks::stick, blocks::air, blocks::string, blocks::air, blocks::stick, blocks::string, blocks::bow, 1,

		blocks::coal, blocks::coal, blocks::coal, blocks::coal, blocks::coal, blocks::coal, blocks::coal, blocks::coal, blocks::coal, blocks::coal_block, 1,
		blocks::iron_ingot, blocks::iron_ingot, blocks::iron_ingot, blocks::iron_ingot, blocks::iron_ingot, blocks::iron_ingot, blocks::iron_ingot, blocks::iron_ingot, blocks::iron_ingot, blocks::iron_block, 1,
		blocks::diamond, blocks::diamond, blocks::diamond, blocks::diamond, blocks::diamond, blocks::diamond, blocks::diamond, blocks::diamond, blocks::diamond, blocks::diamond_block, 1,
		blocks::redstone_dust, blocks::redstone_dust, blocks::redstone_dust, blocks::redstone_dust, blocks::redstone_dust, blocks::redstone_dust, blocks::redstone_dust, blocks::redstone_dust, blocks::redstone_dust, blocks::redstone_block, 1,
	}},
};

namespace CELLS {
	const int hotbar_first = 0;
	const int hotbar_last = 8;
	const int backpack_first = 9;
	const int backpack_last = 35;
	const int icraft_first = 36;
	const int icraft_last = 39;
	const int product = 40;
	const int table_craft_first = 41;
	const int table_craft_last = 49;
	const int furnace_composant = 50;
	const int furnace_fuel = 51;
	const int chest_first = 52;
	const int chest_last = 78;
};

typedef struct s_item {
	s_item( int t = blocks::air, int a = 0, glm::ivec2 d = {0, 0} ) : type(t), amount(a), dura(d) {}
	int type;
	int amount;
	glm::ivec2 dura;
	// enchantements will come here
}			t_item;

class Inventory
{
    public:
        Inventory( void );
        ~Inventory( void );

        int getCurrentSlot( void );
		t_item getSlotBlock( int slot );
		t_item getBackpackBlock( int slot );
		t_item getiCraftBlock( int slot );
		t_item getCraftBlock( int slot );
		t_item getCrafted( void );
		int getSlotNum( void );
        void setSlot( int value );
		void shiftBlockAt( int craft, int value, FurnaceInstance *furnace, ChestInstance *chest );
		t_item pickBlockAt( int craft, int value, FurnaceInstance *furnace, ChestInstance *chest );
		t_item pickHalfBlockAt( int craft, int value, FurnaceInstance *furnace, ChestInstance *chest );
		t_item putBlockAt( int craft, int value, t_item block, FurnaceInstance *furnace, ChestInstance *chest, bool swap = true );
		t_item putOneBlockAt( int craft, int value, t_item block, FurnaceInstance *furnace, ChestInstance *chest );
		bool restoreBlock( t_item block, bool hotbar_first = false );
		bool absorbItem( t_item block );
		void restoreiCraft( void );
		void restoreCraft( void );
		bool getModif( void );
		void setModif( bool value );
        void addBlock( int type );
		// void removeBlockAt( int value, FurnaceInstance *furnace, ChestInstance *chest );
        t_item removeBlock( bool thrown );
		void replaceSlot( int type, bool creative );
		void swapCells( int slot, int location );
		void decrementDurabitilty( void );

		void spillInventory( Chunk* chunk );

        std::string getInventoryString( void );
		std::string getSlotString( void );
		std::string saveString( void );
		void setUIPtr( UI *ui );
		void loadWorld( std::ofstream & ofs, std::ifstream & indata );

    private:
        std::array<t_item, 9> _content;
		std::array<t_item, 27> _backpack;
		std::array<t_item, 4> _icraft;
		std::array<t_item, 9> _craft;
		t_item _crafted;
        int _slot;
		bool _modif;
		UI *_ui;

		t_item *getBlockPtr( int value, int & craft_place, FurnaceInstance *furnace, ChestInstance *chest );
		void changeCrafted( int craft );
		void produceCraft( int craft );
		t_item pickCrafted( int craft, t_item block );
		int findEmptyCell( t_item block, bool hotbar_first = false );
		int findBlockCell( int type );
		void pickAllCrafted( int craft );
};

#endif
