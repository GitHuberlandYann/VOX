#ifndef INVENTORY_HPP
# define INVENTORY_HPP

class Inventory
{
    private:
        glm::ivec2 _content[9];
        int _slot;

    public:
        Inventory( void );
        ~Inventory( void );

        int getCurrentSlot( void );
        void setSlot( int value );
        void addBlock( int type );
        void removeBlock( void );

        std::string getInventoryString( void );
};

#endif
