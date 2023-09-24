#include "vox.h"

Inventory::Inventory( void ) : _slot(0)
{
    for (int index = 0; index < 9; index++) {
        _content[index] = glm::ivec2(blocks::AIR, 0);
    }
}

Inventory::~Inventory( void )
{

}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

int Inventory::getCurrentSlot( void )
{
    return (_content[_slot].x);
}

int Inventory::getSlot( void )
{
	return (_slot);
}

void Inventory::setSlot( int value )
{
    _slot = value;
}

void Inventory::addBlock( int type )
{
    glm::ivec2 current = _content[_slot];
    if ((current.x == blocks::AIR || current.x == type) && current.y != 64) {
        _content[_slot].x = type;
        _content[_slot].y++;
        return ;
    }
    for (int index = 0; index < 9; index++)
    {
        current = _content[index];
        if ((current.x == blocks::AIR || current.x == type) && current.y != 64) {
            _content[index].x = type;
            _content[index].y++;
            return ;
        }
    }
}

void Inventory::removeBlock( void )
{
    if (_content[_slot].x == blocks::AIR) {
        return ;
    }
    if (--_content[_slot].y <= 0) {
        _content[_slot] = glm::ivec2(blocks::AIR, 0);
    }
}

std::string Inventory::getInventoryString( void )
{
    std::string res = "\nCurrent Slot > " + std::to_string(_slot);
    for (int index = 0; index < 9; index++) {
        glm::ivec2 current = _content[index];
        res += "\nslot " + std::to_string(index) + '<' + std::to_string(current.x) + "> times " + std::to_string(current.y);
    }
    return (res);
}
