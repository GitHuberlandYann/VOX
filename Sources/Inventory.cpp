#include "vox.h"

Inventory::Inventory( void ) : _slot(0), _modif(false)
{
    for (int index = 0; index < 9; index++) {
        _content[index] = glm::ivec2(blocks::AIR, 0);
    }
    for (int index = 0; index < 27; index++) {
        _backpack[index] = glm::ivec2(blocks::OAK_TRUNK * (index % 2), (index + 1) * (index % 2));
    }
    for (int index = 0; index < 4; index++) {
        _icraft[index] = glm::ivec2(blocks::BEDROCK, 1);
    }
    _crafted = glm::ivec2(blocks::AIR, 0);
}

Inventory::~Inventory( void )
{

}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void Inventory::changeCrafted( bool craft )
{
	if (!craft) {
		return ;
	}
	for (int index = 0; index < 1; index++) {
		bool match = true;
		for (int rindex = 0; rindex < 4; rindex++) {
			if (_icraft[rindex].x != receipt[index][rindex]) {
				match = false;
			}
		}
		if (match) {
			_crafted = glm::ivec2(receipt[index][4], receipt[index][5]);
			return ;
		}
	}
	_crafted = glm::ivec2(blocks::AIR, 0);
}

void Inventory::produceiCraft( void )
{
	for (int index = 0; index < 4; index++) {
		if (_icraft[index].y > 0) {
			_icraft[index].y--;
			if (_icraft[index].y == 0) {
				_icraft[index].x = blocks::AIR;
			}
		}
	}
	changeCrafted(true);
}

glm::ivec2 Inventory::pickiCrafted( glm::ivec2 block )
{
	if (_crafted.x == blocks::AIR) {
		return (block);
	}
	if (block.x == blocks::AIR) {
		glm::ivec2 ret = _crafted;
		produceiCraft();
		return (ret);
	} else if (block.x == _crafted.x && block.y + _crafted.y <= 64) {
		block.y += _crafted.y;
		produceiCraft();
	}
	return (block);
}

void Inventory::pickAlliCrafted( void )
{

}

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

glm::ivec2 Inventory::getBackpackBlock( int slot )
{
	if (slot < 0 || slot >= 27) {
		return (glm::vec2(0, 0));
	}
	return (_backpack[slot]);
}

glm::ivec2 Inventory::getiCraftBlock( int slot )
{
	if (slot < 0 || slot >= 4) {
		return (glm::vec2(0, 0));
	}
	return (_icraft[slot]);
}

glm::ivec2 Inventory::getCrafted( void )
{
	return (_crafted);
}

int Inventory::getSlotNum( void )
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

int Inventory::countSlots( void )
{
	int res = 0;
	for (int index = 0; index < 9; index++) {
		res += _content[index].x != blocks::AIR;
	}
	return (res);
}

int Inventory::countBackpack( void )
{
	int res = 0;
	for (int index = 0; index < 27; index++) {
		res += _backpack[index].x != blocks::AIR;
	}
	return (res);
}

int Inventory::countiCraft( void )
{
	int res = 0;
	for (int index = 0; index < 4; index++) {
		res += _icraft[index].x != blocks::AIR;
	}
	return (res);
}

glm::ivec2 Inventory::pickBlockAt( int value )
{
	// std::cout << "pickBlockAt " << value << std::endl;
	if (value == 40) {
		return (pickiCrafted(glm::ivec2 (blocks::AIR, 0)));
	}
	if (value < 0 || value > 39) {
		return (glm::ivec2(blocks::AIR, 0));
	}
	glm::ivec2 *bat;
	bool craft = false;
	if (value < 9) {
		_modif = true;
		bat = &_content[value];
	} else if (value < 36) {
		bat = &_backpack[value - 9];
	} else {
		craft = true;
		bat = &_icraft[value - 36];
	}
	if (bat->x != blocks::AIR) {
		glm::ivec2 res = *bat;
		*bat = glm::ivec2(blocks::AIR, 0);
		changeCrafted(craft);
		return (res);
	}
	changeCrafted(craft);
	return (glm::ivec2(blocks::AIR, 0));
}

glm::ivec2 Inventory::pickHalfBlockAt( int value )
{
	if (value == 40) {
		pickAlliCrafted();
		return (glm::ivec2(blocks::AIR, 0));
	}
	if (value < 0 || value > 39) {
		return (glm::ivec2(blocks::AIR, 0));
	}
	glm::ivec2 *bat;
	bool craft = false;
	if (value < 9) {
		_modif = true;
		bat = &_content[value];
	} else if (value < 36) {
		bat = &_backpack[value - 9];
	} else {
		craft = true;
		bat = &_icraft[value - 36];
	}
	if (bat->x != blocks::AIR) {
		glm::ivec2 res = *bat;
		res.y = res.y / 2 + (res.y % 2);
		bat->y /= 2;
		if (!bat->y) {
			*bat = glm::ivec2(blocks::AIR, 0);
		}
		changeCrafted(craft);
		return (res);
	}
	changeCrafted(craft);
	return (glm::ivec2(blocks::AIR, 0));
}

glm::ivec2 Inventory::putBlockAt( int value, glm::ivec2 block )
{
	if (value == 40) {
		return (pickiCrafted(block));
	}
	if (value < 0 || value > 39) {
		return (block);
	}
	glm::ivec2 *bat;
	bool craft = false;
	if (value < 9) {
		_modif = true;
		bat = &_content[value];
	} else if (value < 36) {
		bat = &_backpack[value - 9];
	} else {
		craft = true;
		bat = &_icraft[value - 36];
	}
	glm::ivec2 res = *bat;
	if (res.x == block.x) {
		res.y += block.y;
		if (res.y > 64) {
			block.y = res.y - 64;
			res.y = 64;
			*bat = res;
			changeCrafted(craft);
			return (block);
		}
		*bat = res;
		changeCrafted(craft);
		return (glm::ivec2(blocks::AIR, 0));
	}
	*bat = block;
	changeCrafted(craft);
	return (res);
}

glm::ivec2 Inventory::putOneBlockAt( int value, glm::ivec2 block )
{
	if (value == 40) {
		pickAlliCrafted();
		return (block);
	}
	if (value < 0 || value > 39) {
		return (block);
	}
	glm::ivec2 *bat;
	bool craft = false;
	if (value < 9) {
		_modif = true;
		bat = &_content[value];
	} else if (value < 36) {
		bat = &_backpack[value - 9];
	} else {
		craft = true;
		bat = &_icraft[value - 36];
	}
	if (bat->y == 64) {
		return (block);
	}
	if (bat->x == block.x) {
		bat->y++;
	} else if (bat->x == blocks::AIR) {
		*bat = glm::ivec2(block.x, 1);
	} else {
		return (block);
	}
	changeCrafted(craft);
	block.y--;
	if (!block.y) {
		return (glm::ivec2(blocks::AIR, 0));
	}
	return (block);
}

void Inventory::restoreBlock( glm::ivec2 block )
{
	for (int index = 0; index < 9; index++) {
		if (_content[index].x == blocks::AIR) { // TODO add on top of block of the same kind if found + dispatch rest of stack correctly
			_content[index] = block;
			_modif = true;
			return ;
		}
	}
	for (int index = 0; index < 27; index++) {
		if (_backpack[index].x == blocks::AIR) {
			_backpack[index] = block;
			return ;
		}
	}
}

void Inventory::restoreiCraft( void )
{
	for (int index = 0; index < 4; index++) {
		if (_icraft[index].x != blocks::AIR) {
			restoreBlock(_icraft[index]);
			_icraft[index] = glm::ivec2(blocks::AIR, 0);
		}
	}
	_crafted = glm::ivec2(blocks::AIR, 0);
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
	if (type == blocks::GRASS_BLOCK) {
		type = blocks::DIRT;
	}
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

void Inventory::removeBlockAt( int value )
{
	if (value < 0 || value > 36) {
		return ;
	}
	if (value < 9) {
		if (_content[value].x != blocks::AIR && --_content[value].y <= 0) {
			_content[value] = glm::ivec2(blocks::AIR, 0);
			_modif = true;
		}
	} else {
		if (_backpack[value - 9].x != blocks::AIR && --_backpack[value - 9].y <= 0) {
			_backpack[value - 9] = glm::ivec2(blocks::AIR, 0);
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

void Inventory::replaceSlot( int type )
{
	_content[_slot] = glm::ivec2(type, 1);
	_modif = true;
}
