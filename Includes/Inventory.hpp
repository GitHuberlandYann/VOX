#ifndef INVENTORY_HPP
# define INVENTORY_HPP

class Inventory
{
    private:
        glm::ivec2 _content[9]; // (block_type, nb_blocks)
        int _slot;
		bool _modif;

    public:
        Inventory( void );
        ~Inventory( void );

        int getCurrentSlot( void );
		glm::ivec2 getSlotBlock( int slot );
		int getSlot( void );
        void setSlot( int value );
		bool getModif( void );
		void setModif( bool value );
        void addBlock( int type );
        void removeBlock( void );

        std::string getInventoryString( void );
};

#endif
