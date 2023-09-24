#include "vox.h"

Inventory::Inventory( void ) : _slot(0), _modif(false)
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

glm::ivec2 Inventory::getSlotBlock( int slot )
{
	if (slot < 0 || slot >= 9) {
		return (glm::vec2(0, 0));
	}
	return (_content[slot]);
}

int Inventory::getSlot( void )
{
	return (_slot);
}

void Inventory::setSlot( int value )
{
	if (value < 0 || value >= 9) {
		return ;
	}
    _slot = value;
	_modif = true;
}

bool Inventory::getModif( void )
{
	return (_modif);
}

void Inventory::setModif( bool value )
{
	_modif = value;
}

void Inventory::addBlock( int type )
{
    glm::ivec2 current = _content[_slot];
    if ((current.x == blocks::AIR || current.x == type) && current.y != 64) {
        _content[_slot].x = type;
        _content[_slot].y++;
		_modif = true;
        return ;
    }
    for (int index = 0; index < 9; index++)
    {
        current = _content[index];
        if ((current.x == blocks::AIR || current.x == type) && current.y != 64) {
            _content[index].x = type;
            _content[index].y++;
			_modif = true;
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
		_modif = true;
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
