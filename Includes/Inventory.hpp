#ifndef INVENTORY_HPP
# define INVENTORY_HPP

# define IRECEIPT_SIZE 7
# define RECEIPT_SIZE 28

const int ireceipt[IRECEIPT_SIZE][6] = {
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_LOG, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::AIR, blocks::OAK_LOG, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::OAK_LOG, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::OAK_LOG, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::STICK, 4},
	{blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::STICK, 4},
	{blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::CRAFTING_TABLE, 1},
};

const int receipt[RECEIPT_SIZE][11] = {
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_LOG, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_LOG, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_LOG, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_LOG, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_LOG, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_LOG, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::AIR, blocks::OAK_LOG, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::OAK_LOG, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::OAK_LOG, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, 4},

	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::STICK, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::STICK, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::STICK, 4},
	{blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::AIR, blocks::STICK, 4},
	{blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::STICK, 4},
	{blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::STICK, 4},

	{blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::CRAFTING_TABLE, 4},
	{blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::AIR, blocks::CRAFTING_TABLE, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::CRAFTING_TABLE, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::CRAFTING_TABLE, 4},

	{blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::WOODEN_SHOVEL, 1},
	{blocks::AIR, blocks::STONE, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::STONE_SHOVEL, 1},
	{blocks::AIR, blocks::COBBLESTONE, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::STONE_SHOVEL, 1},
	{blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::WOODEN_AXE, 1},
	{blocks::STONE, blocks::STONE, blocks::AIR, blocks::STONE, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::STONE_AXE, 1},
	{blocks::COBBLESTONE, blocks::COBBLESTONE, blocks::AIR, blocks::COBBLESTONE, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::STONE_AXE, 1},
	{blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::WOODEN_PICKAXE, 1},
	{blocks::STONE, blocks::STONE, blocks::STONE, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::STONE_PICKAXE, 1},
	{blocks::COBBLESTONE, blocks::COBBLESTONE, blocks::COBBLESTONE, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::STONE_PICKAXE, 1},
};

class Inventory
{
    private:
        glm::ivec2 _content[9]; // (block_type, nb_blocks)
		glm::ivec2 _backpack[27];
		glm::ivec2 _icraft[4];
		glm::ivec2 _craft[9];
		glm::ivec2 _crafted;
        int _slot;
		bool _modif;

		void changeCrafted( int craft );
		void produceCraft( int craft );
		glm::ivec2 pickCrafted( int craft, glm::ivec2 block );
		int findEmptyCell( void );
		void pickAllCrafted( int craft );

    public:
        Inventory( void );
        ~Inventory( void );

        int getCurrentSlot( void );
		glm::ivec2 getSlotBlock( int slot );
		glm::ivec2 getBackpackBlock( int slot );
		glm::ivec2 getiCraftBlock( int slot );
		glm::ivec2 getCraftBlock( int slot );
		glm::ivec2 getCrafted( void );
		int getSlotNum( void );
        void setSlot( int value );
		int countSlots( void );
		int countBackpack( void );
		int countiCraft( void );
		int countCraft( void );
		glm::ivec2 pickBlockAt( int craft, int value );
		glm::ivec2 pickHalfBlockAt( int craft, int value );
		glm::ivec2 putBlockAt( int craft, int value, glm::ivec2 block );
		glm::ivec2 putOneBlockAt( int craft, int value, glm::ivec2 block );
		void restoreBlock( glm::ivec2 block );
		void restoreiCraft( void );
		void restoreCraft( void );
		bool getModif( void );
		void setModif( bool value );
        void addBlock( int type );
		void removeBlockAt( int value );
        void removeBlock( void );
		void replaceSlot( int type );

        std::string getInventoryString( void );
};

#endif
