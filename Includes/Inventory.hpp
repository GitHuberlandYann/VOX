#ifndef INVENTORY_HPP
# define INVENTORY_HPP

const int receipt[1][6] = {
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_TRUNK, blocks::OAK_PLANKS, 4}
};

class Inventory
{
    private:
        glm::ivec2 _content[9]; // (block_type, nb_blocks)
		glm::ivec2 _backpack[27];
		glm::ivec2 _icraft[4];
		// glm::ivec2 _craft[9];
		glm::ivec2 _crafted;
        int _slot;
		bool _modif;

		void changeCrafted( bool craft );
		void produceiCraft( void );
		glm::ivec2 pickiCrafted( glm::ivec2 block );
		void pickAlliCrafted( void );

    public:
        Inventory( void );
        ~Inventory( void );

        int getCurrentSlot( void );
		glm::ivec2 getSlotBlock( int slot );
		glm::ivec2 getBackpackBlock( int slot );
		glm::ivec2 getiCraftBlock( int slot );
		glm::ivec2 getCrafted( void );
		int getSlotNum( void );
        void setSlot( int value );
		int countSlots( void );
		int countBackpack( void );
		int countiCraft( void );
		glm::ivec2 pickBlockAt( int value );
		glm::ivec2 pickHalfBlockAt( int value );
		glm::ivec2 putBlockAt( int value, glm::ivec2 block );
		glm::ivec2 putOneBlockAt( int value, glm::ivec2 block );
		void restoreBlock( glm::ivec2 block );
		void restoreiCraft( void );
		bool getModif( void );
		void setModif( bool value );
        void addBlock( int type );
		void removeBlockAt( int value );
        void removeBlock( void );
		void replaceSlot( int type );

        std::string getInventoryString( void );
};

#endif
