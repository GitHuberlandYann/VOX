#include "vox.h"

Inventory::Inventory( void ) : _slot(0), _modif(false)
{
    for (int index = 0; index < 9; index++) {
        _content[index] = glm::ivec2(blocks::CRAFTING_TABLE * (index == 0), 64 * (index == 0));
    }
    for (int index = 0; index < 27; index++) {
        _backpack[index] = glm::ivec2(blocks::OAK_LOG * (index == 0), 64 * (index == 0));
    }
    for (int index = 0; index < 4; index++) {
        _icraft[index] = glm::ivec2(blocks::AIR, 0);
    }
    for (int index = 0; index < 9; index++) {
        _craft[index] = glm::ivec2(blocks::AIR, 0);
    }
    _crafted = glm::ivec2(blocks::AIR, 0);
}

Inventory::~Inventory( void )
{

}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void Inventory::changeCrafted( int craft )
{
	if (!craft) {
		return ;
	} else if (craft == 1) {
		for (int index = 0; index < IRECEIPT_SIZE; index++) {
			bool match = true;
			for (int rindex = 0; rindex < 4; rindex++) {
				if (_icraft[rindex].x != ireceipt[index][rindex]) {
					match = false;
				}
			}
			if (match) {
				_crafted = glm::ivec2(ireceipt[index][4], ireceipt[index][5]);
				return ;
			}
		}
	} else if (craft == 2) {
		for (int index = 0; index < RECEIPT_SIZE; index++) {
			bool match = true;
			for (int rindex = 0; rindex < 9; rindex++) {
				if (_craft[rindex].x != receipt[index][rindex]) {
					match = false;
				}
			}
			if (match) {
				_crafted = glm::ivec2(receipt[index][9], receipt[index][10]);
				return ;
			}
		}
	}
	_crafted = glm::ivec2(blocks::AIR, 0);
}

void Inventory::produceCraft( int craft )
{
	if (craft == 1) {
		// std::cout << "produce i craft" << std::endl;
		for (int index = 0; index < 4; index++) {
			if (_icraft[index].y > 0) {
				_icraft[index].y--;
				if (_icraft[index].y == 0) {
					_icraft[index].x = blocks::AIR;
				}
			}
		}
	} else if (craft == 2) {
		// std::cout << "produce craft" << std::endl;
		for (int index = 0; index < 9; index++) {
			if (_craft[index].y > 0) {
				_craft[index].y--;
				if (_craft[index].y == 0) {
					_craft[index].x = blocks::AIR;
				}
			}
		}
	}
	changeCrafted(craft);
}

glm::ivec2 Inventory::pickCrafted( int craft, glm::ivec2 block )
{
	if (_crafted.x == blocks::AIR) {
		return (block);
	}
	if (block.x == blocks::AIR) {
		glm::ivec2 ret = _crafted;
		produceCraft(craft);
		return (ret);
	} else if (block.x == _crafted.x && block.y + _crafted.y <= 64) {
		block.y += _crafted.y;
		produceCraft(craft);
	}
	return (block);
}

int Inventory::findEmptyCell( void )
{
	for (int index = 0; index < 27; index++) {
		if (_backpack[index].x == blocks::AIR) {
			return (index + 9);
		}
	}
	for (int index = 0; index < 9; index++) {
		if (_content[index].x == blocks::AIR) {
			return (index);
		}
	}
	return (0);
}

void Inventory::pickAllCrafted( int craft )
{
	int location = findEmptyCell();
	if (!location) {
		return ;
	}
	glm::ivec2 *bat;
	if (location < 9) {
		bat = &_content[location];
	} else {
		bat = &_backpack[location - 9];
	}
	while (_crafted.x != blocks::AIR && bat->y < 64) {
			*bat = pickCrafted(craft, *bat);
	}
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

glm::ivec2 Inventory::getCraftBlock( int slot )
{
	if (slot < 0 || slot >= 9) {
		return (glm::vec2(0, 0));
	}
	return (_craft[slot]);
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

int Inventory::countCraft( void )
{
	int res = 0;
	for (int index = 0; index < 9; index++) {
		res += _craft[index].x != blocks::AIR;
	}
	return (res);
}

glm::ivec2 Inventory::pickBlockAt( int craft, int value )
{
	// std::cout << "pickBlockAt " << value << std::endl;
	if (value == 40) {
		return (pickCrafted(craft, glm::ivec2 (blocks::AIR, 0)));
	}
	if (value < 0 || value > 49) {
		return (glm::ivec2(blocks::AIR, 0));
	}
	glm::ivec2 *bat;
	int craft_place = 0;
	if (value < 9) {
		_modif = true;
		bat = &_content[value];
	} else if (value < 36) {
		bat = &_backpack[value - 9];
	} else if (value < 40) {
		craft_place = 1;
		bat = &_icraft[value - 36];
	} else {
		craft_place = 2;
		bat = &_craft[value - 41];
	}
	if (bat->x != blocks::AIR) {
		glm::ivec2 res = *bat;
		*bat = glm::ivec2(blocks::AIR, 0);
		changeCrafted(craft);
		return (res);
	}
	changeCrafted(craft_place);
	return (glm::ivec2(blocks::AIR, 0));
}

glm::ivec2 Inventory::pickHalfBlockAt( int craft, int value )
{
	if (value == 40) {
		pickAllCrafted(craft);
		return (glm::ivec2(blocks::AIR, 0));
	}
	if (value < 0 || value > 49) {
		return (glm::ivec2(blocks::AIR, 0));
	}
	glm::ivec2 *bat;
	int craft_place = 0;
	if (value < 9) {
		_modif = true;
		bat = &_content[value];
	} else if (value < 36) {
		bat = &_backpack[value - 9];
	} else if (value < 40) {
		craft_place = 1;
		bat = &_icraft[value - 36];
	} else {
		craft_place = 2;
		bat = &_craft[value - 41];
	}
	if (bat->x != blocks::AIR) {
		glm::ivec2 res = *bat;
		res.y = res.y / 2 + (res.y % 2);
		bat->y /= 2;
		if (!bat->y) {
			*bat = glm::ivec2(blocks::AIR, 0);
		}
		changeCrafted(craft_place);
		return (res);
	}
	changeCrafted(craft_place);
	return (glm::ivec2(blocks::AIR, 0));
}

glm::ivec2 Inventory::putBlockAt( int craft, int value, glm::ivec2 block )
{
	if (value == 40) {
		return (pickCrafted(craft, block));
	}
	if (value < 0 || value > 49) {
		return (block);
	}
	glm::ivec2 *bat;
	int craft_place = 0;
	if (value < 9) {
		_modif = true;
		bat = &_content[value];
	} else if (value < 36) {
		bat = &_backpack[value - 9];
	} else if (value < 40) {
		craft_place = 1;
		bat = &_icraft[value - 36];
	} else {
		craft_place = 2;
		bat = &_craft[value - 41];
	}
	glm::ivec2 res = *bat;
	if (res.x == block.x) {
		res.y += block.y;
		if (res.y > 64) {
			block.y = res.y - 64;
			res.y = 64;
			*bat = res;
			changeCrafted(craft_place);
			return (block);
		}
		*bat = res;
		changeCrafted(craft_place);
		return (glm::ivec2(blocks::AIR, 0));
	}
	*bat = block;
	changeCrafted(craft_place);
	return (res);
}

glm::ivec2 Inventory::putOneBlockAt( int craft,  int value, glm::ivec2 block )
{
	if (value == 40) {
		pickAllCrafted(craft);
		return (block);
	}
	if (value < 0 || value > 49) {
		return (block);
	}
	glm::ivec2 *bat;
	int craft_place = 0;
	if (value < 9) {
		_modif = true;
		bat = &_content[value];
	} else if (value < 36) {
		bat = &_backpack[value - 9];
	} else if (value < 40) {
		craft_place = 1;
		bat = &_icraft[value - 36];
	} else {
		craft_place = 2;
		bat = &_craft[value - 41];
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
	changeCrafted(craft_place);
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

void Inventory::restoreCraft( void )
{
	for (int index = 0; index < 9; index++) {
		if (_craft[index].x != blocks::AIR) {
			restoreBlock(_craft[index]);
			_craft[index] = glm::ivec2(blocks::AIR, 0);
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
	} else if (type == blocks::STONE) {
		type = blocks::COBBLESTONE;
	} else if (type == blocks::COAL_ORE) {
		type = blocks::COAL;
	} else if (type == blocks::DIAMOND_ORE) {
		type = blocks::DIAMOND;
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
	for (int index = 0; index < 27; index++) {
		current = _backpack[index];
		if ((current.x == blocks::AIR || current.x == type) && current.y != 64) {
			_backpack[index].x = type;
			_backpack[index].y++;
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
