#include "vox.h"

Inventory::Inventory( void ) : _slot(0), _saved_durability(0), _modif(false)
{
    for (int index = 0; index < 9; index++) {
        _content[index] = glm::ivec2(blocks::AIR * (index == 0), 0 * (index == 0));
    }
    for (int index = 0; index < 27; index++) {
        _backpack[index] = glm::ivec2(blocks::AIR * (index == 0) + blocks::AIR * (index == 1), 0 * (index <= 1));
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
	_durabilities.clear();
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

int Inventory::getrmDura( int value )
{
	for (auto& dura: _durabilities) {
		if (dura.x == value) {
			int ret = dura.y;
			_durabilities.remove(dura);
			return (ret);
		}
	}
	std::cout << "GETRMDURA not found on emplacement " << value << std::endl;
	return (0);
}

glm::ivec2 *Inventory::getBlockPtr( int value, int &craft_place, FurnaceInstance *furnace )
{
	if (value < 0 || value > 51) {
		return (NULL);
	}
	glm::ivec2 *res = NULL;
	if (value < 9) {
		_modif = true;
		res = &_content[value];
	} else if (value < 36) {
		res = &_backpack[value - 9];
	} else if (value < 40) {
		craft_place = 1;
		res = &_icraft[value - 36];
	} else if (value < 50) {
		craft_place = 2;
		res = &_craft[value - 41];
	} else if (value < 52) {
		if (!furnace) {
			return (NULL);
		}
		res = furnace->pickCompoFuel(value == 51);
	}
	return (res);
}

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
	} else if (s_blocks[_crafted.x].durability) {
		_saved_durability = s_blocks[_crafted.x].durability;
	}
	if (block.x == blocks::AIR) {
		glm::ivec2 ret = _crafted;
		produceCraft(craft);
		return (ret);
	} else if (block.x == _crafted.x && s_blocks[block.x].stackable && block.y + _crafted.y <= 64) {
		block.y += _crafted.y;
		produceCraft(craft);
	}
	return (block);
}

int Inventory::findEmptyCell( glm::ivec2 block, bool swap )
{
	if (s_blocks[block.x].stackable) {
		for (int index = 0; index < 9; index++) {
			if (_content[index].x == block.x && _content[index].y + block.y <= 64) {
				return (index);
			}
		}
		for (int index = 0; index < 27; index++) {
			if (_backpack[index].x == block.x && _backpack[index].y + block.y <= 64) {
				return (index + 9);
			}
		}
	}
	if (swap) {
		for (int index = 0; index < 9; index++) {
			if (_content[index].x == blocks::AIR) {
				return (index);
			}
		}
	}
	for (int index = 0; index < 27; index++) {
		if (_backpack[index].x == blocks::AIR) {
			return (index + 9);
		}
	}
	if (!swap) {
		for (int index = 0; index < 9; index++) {
			if (_content[index].x == blocks::AIR) {
				return (index);
			}
		}
	}
	return (-1);
}

void Inventory::pickAllCrafted( int craft )
{
	int location = findEmptyCell(_crafted);
	if (location == -1) {
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
			if (s_blocks[bat->x].durability) {
				_durabilities.push_back(glm::ivec3(location, _saved_durability, s_blocks[bat->x].durability));
			}
			if (_crafted.x != bat->x && _crafted.x != blocks::AIR) {
				return (pickAllCrafted(craft));
			}
			if (!s_blocks[bat->x].stackable) {
				return (pickAllCrafted(craft));
			}
	}
	if (_crafted.x != blocks::AIR) {
		return (pickAllCrafted(craft));
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

glm::ivec3 Inventory::getDuraFromIndex( int index, bool all )
{
	if (index < 0 || index >= static_cast<int>(_durabilities.size())) {
		std::cerr << "ERROR getDuraFromIndex " << index << std::endl;
		return glm::ivec3(0);
	}
	int i = -1;
	for (auto& dura: _durabilities) {
		if (dura.y != dura.z && (all || dura.x < 9)) {
			i++;
		}
		if (i == index) {
			return (dura);
		}
	}
	return (glm::ivec3(0));
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

int Inventory::countDura( bool all )
{
	int cnt = 0;
	for (auto& dura: _durabilities) {
		if (dura.y != dura.z && (all || dura.x < 9)) {
			cnt++;
		}
	}
	return (cnt);
}

glm::ivec2 Inventory::pickBlockAt( int craft, int value, FurnaceInstance *furnace )
{
	// std::cout << "pickBlockAt " << value << std::endl;
	if (value == 40) {
		if (furnace) {
			return (furnace->pickProduction());
		}
		return (pickCrafted(craft, glm::ivec2 (blocks::AIR, 0)));
	}
	int craft_place = 0;
	glm::ivec2 *bat = getBlockPtr(value, craft_place, furnace);
	if (!bat) {
		return (glm::ivec2(blocks::AIR, 0));
	}
	if (value < 0 || value > 51) {
		return (glm::ivec2(blocks::AIR, 0));
	}
	if (bat->x != blocks::AIR) {
		glm::ivec2 res = *bat;
		if (s_blocks[res.x].durability) {
			_saved_durability = getrmDura(value);
		}
		*bat = glm::ivec2(blocks::AIR, 0);
		changeCrafted(craft);
		return (res);
	}
	changeCrafted(craft_place);
	return (glm::ivec2(blocks::AIR, 0));
}

glm::ivec2 Inventory::pickHalfBlockAt( int craft, int value, FurnaceInstance *furnace )
{
	if (value == 40) {
		if (!furnace) { // for now does nothing if in furnace
			pickAllCrafted(craft);
		}
		return (glm::ivec2(blocks::AIR, 0));
	}
	int craft_place = 0;
	glm::ivec2 *bat = getBlockPtr(value, craft_place, furnace);
	if (!bat) {
		return (glm::ivec2(blocks::AIR, 0));
	}
	if (bat->x != blocks::AIR) {
		glm::ivec2 res = *bat;
		res.y = res.y / 2 + (res.y % 2);
		bat->y /= 2;
		if (!bat->y) {
			*bat = glm::ivec2(blocks::AIR, 0);
		}
		if (s_blocks[res.x].durability) {
			_saved_durability = getrmDura(value);
		}
		changeCrafted(craft_place);
		return (res);
	}
	changeCrafted(craft_place);
	return (glm::ivec2(blocks::AIR, 0));
}

glm::ivec2 Inventory::putBlockAt( int craft, int value, glm::ivec2 block, FurnaceInstance *furnace )
{
	if (value == 40) {
		if (furnace) {
			return (block);
		}
		return (pickCrafted(craft, block));
	}
	int craft_place = 0;
	glm::ivec2 *bat = getBlockPtr(value, craft_place, furnace);
	if (!bat) {
		return (block);
	}
	glm::ivec2 res = *bat;
	if (res.x == block.x && s_blocks[block.x].stackable) {
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
	if (s_blocks[block.x].durability) {
		int save_dura = _saved_durability;
		if (s_blocks[res.x].durability) {
			_saved_durability = getrmDura(value);
		}
		if (save_dura == 0) {
			*bat = glm::ivec2(blocks::AIR, 0);
		} else {
			_durabilities.push_back(glm::ivec3(value, save_dura, s_blocks[bat->x].durability));
		}
	} else if (s_blocks[res.x].durability) {
		_saved_durability = getrmDura(value);
	}
	return (res);
}

glm::ivec2 Inventory::putOneBlockAt( int craft,  int value, glm::ivec2 block, FurnaceInstance *furnace )
{
	if (value == 40) {
		if (!furnace) {
			pickAllCrafted(craft);
		}
		return (block);
	}
	int craft_place = 0;
	glm::ivec2 *bat = getBlockPtr(value, craft_place, furnace);
	if (!bat) {
		return (block);
	}
	if (bat->y == 64) {
		return (block);
	}
	if (bat->x == block.x && s_blocks[block.x].stackable) {
		bat->y++;
	} else if (bat->x == blocks::AIR) {
		*bat = glm::ivec2(block.x, 1);
		if (s_blocks[block.x].durability) { // tool
			if (_saved_durability == 0) {
				*bat = glm::ivec2(blocks::AIR, 0);
			} else {
				_durabilities.push_back(glm::ivec3(value, _saved_durability, s_blocks[block.x].durability));
			}
		}
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

void Inventory::restoreBlock( glm::ivec2 block, bool swap )
{
	int location = findEmptyCell(block, swap);
	if (location == -1) {
		return ;
	}
	if (s_blocks[block.x].durability) {
		_durabilities.push_back(glm::ivec3(location, _saved_durability, s_blocks[block.x].durability));
	}
	if (location < 9) {
		_content[location] = glm::ivec2(block.x, block.y + _content[location].y);
		_modif = true;
	} else {
		_backpack[location - 9] = glm::ivec2(block.x, block.y + _backpack[location - 9].y);
	}
}

bool Inventory::absorbItem( glm::ivec2 block, int dura )
{
	int location = findEmptyCell(block, true);
	if (location == -1) {
		return (false);
	}
	if (dura && s_blocks[block.x].durability) {
		_durabilities.push_back(glm::ivec3(location, dura, s_blocks[block.x].durability));
	}
	if (location < 9) {
		_content[location] = glm::ivec2(block.x, block.y + _content[location].y);
		_modif = true;
	} else {
		_backpack[location - 9] = glm::ivec2(block.x, block.y + _backpack[location - 9].y);
	}
	return (true);
}

void Inventory::restoreiCraft( void )
{
	for (int index = 0; index < 4; index++) {
		if (_icraft[index].x != blocks::AIR) {
			if (s_blocks[_craft[index].x].durability) {
				_saved_durability = getrmDura(index + 36);
			}
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
			if (s_blocks[_craft[index].x].durability) {
				_saved_durability = getrmDura(index + 41);
			}
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
	if (type == blocks::WATER) {
		// std::cout << "adding water bucket to inventory" << std::endl;
		if (_content[_slot].x == blocks::BUCKET) {
			if (_content[_slot].y == 1) {
				_content[_slot].x = blocks::WATER_BUCKET;
				_modif = true;
				// std::cout << "and replacing current slot with it" << std::endl;
				return ;
			}
			// std::cout << "and storing it out of slot" << std::endl;
			_content[_slot].y--;
		} else {
			return ;
		}
	}
	glm::ivec2 block = glm::ivec2(s_blocks[type].mined, 1);
	restoreBlock(block, true);
}

void Inventory::removeBlockAt( int value, FurnaceInstance *furnace )
{
	if (value < 9) {
		if (s_blocks[_content[value].x].durability) {
			getrmDura(value);
		}
		if (_content[value].x != blocks::AIR && --_content[value].y <= 0) {
			_content[value] = glm::ivec2(blocks::AIR, 0);
			_modif = true;
		}
	} else if (value < 37) {
		if (s_blocks[_backpack[value].x].durability) {
			getrmDura(value);
		}
		if (_backpack[value - 9].x != blocks::AIR && --_backpack[value - 9].y <= 0) {
			_backpack[value - 9] = glm::ivec2(blocks::AIR, 0);
		}
	} else if (value == 50 && furnace) {
		furnace->removeComposant();
	} else if (value == 51 && furnace) {
		furnace->removeFuel();
	}
}

glm::ivec3 Inventory::removeBlock( bool thrown )
{
	glm::ivec3 res = {_content[_slot].x, 1, 0}; // block_type, amount, dura
    if (res.x == blocks::AIR) {
        return (res);
    }
	if (!thrown && res.x == blocks::WATER_BUCKET) {
		_content[_slot].x = blocks::BUCKET;
		_modif = true;
	} else if (--_content[_slot].y <= 0) {
		if (s_blocks[res.x].durability) {
			res.z = getrmDura(_slot);
		}
        _content[_slot] = glm::ivec2(blocks::AIR, 0);
		_modif = true;
    }
	return (res);
}

void Inventory::replaceSlot( int type )
{
	if (s_blocks[_content[_slot].x].durability) {
		getrmDura(_slot);
	}
	_content[_slot] = glm::ivec2(type, 1);
	_modif = true;
}

void Inventory::swapCells( int slot, int location )
{
	if (slot == location || location == 40) {
		return ;
	}
	if (location < 0 || location > 49) {
		return ;
	}
	_modif = true;
	glm::ivec2 *bat;
	if (location < 9) {
		bat = &_content[location];
	} else if (location < 36) {
		bat = &_backpack[location - 9];
	} else if (location < 40) {
		bat = &_icraft[location - 36];
	} else {
		bat = &_craft[location - 41];
	}
	if (_content[slot].x == bat->x && bat->x != AIR) {
		_content[slot].y += bat->y;
		if (_content[slot].y > 64) {
			bat->y = _content[slot].y - 64;
			_content[slot].y = 64;
		} else {
			bat->x = blocks::AIR;
			bat->y = 0;
		}
		return ;
	}
	int save_dura0 = 0;
	int save_dura1 = 0;
	if (s_blocks[_content[slot].x].durability) {
		save_dura0 = getrmDura(slot);
	}
	if (s_blocks[bat->x].durability) {
		save_dura1 = getrmDura(location);
	}
	glm::ivec2 tmp = _content[slot];
	_content[slot] = *bat;
	*bat = tmp;
	if (save_dura0) {
		_durabilities.push_back(glm::ivec3(location, save_dura0, s_blocks[bat->x].durability));
	}
	if (save_dura1) {
		_durabilities.push_back(glm::ivec3(slot, save_dura1, s_blocks[_content[slot].x].durability));
	}
}

void Inventory::decrementDurabitilty( void )
{
	if (s_blocks[_content[_slot].x].durability) {
		for (auto& dura: _durabilities) {
			if (dura.x == _slot) {
				dura.y--;
				if (!dura.y) {
					removeBlock(false);
					_durabilities.remove(dura);
				}
				return ;
			}
		}
	}
}

void Inventory::spillInventory( Chunk *chunk )
{
	glm::ivec3 details; // block_type, amount, dura

	for (int index = 0; index < 9; index++) {
		if (_content[index].x != blocks::AIR) {
			details = {_content[index].x, _content[index].y, 0};
			if (s_blocks[details.x].durability) {
				details.z = getrmDura(index);
			}
			_content[index] = {blocks::AIR, 0};
			chunk->addEntity(this, {glm::cos(index), glm::sin(index), 0}, details.x, details.y, details.z); // might want to lock this
		}
	}
	for (int index = 0; index < 27; index++) {
		if (_backpack[index].x != blocks::AIR) {
			details = {_backpack[index].x, _backpack[index].y, 0};
			if (s_blocks[details.x].durability) {
				details.z = getrmDura(index + 9);
			}
			_backpack[index] = {blocks::AIR, 0};
			chunk->addEntity(this, {glm::cos(index + 9), glm::sin(index + 9), 0}, details.x, details.y, details.z); // might want to lock this
		}
	}
	_modif = true;
}

// TODO DEL THOSE
std::string Inventory::getInventoryString( void )
{
    std::string res = "\nCurrent Slot > " + std::to_string(_slot);
    for (int index = 0; index < 9; index++) {
        glm::ivec2 current = _content[index];
        res += "\nslot " + std::to_string(index) + '<' + std::to_string(current.x) + "> times " + std::to_string(current.y);
    }
    return (res);
}

std::string Inventory::getDuraString( void )
{
	std::string res = "\n\nDURAS\t> saved is " + std::to_string(_saved_durability) + "\n";
	for (auto& dura: _durabilities) {
		res += "\t\t\t" + std::to_string(dura.x) + ": " + std::to_string(dura.y) + '\n';
	}
	return (res);
}

std::string Inventory::getSlotString( void )
{
	std::string res = "\n\nHolding\t> " + s_blocks[_content[_slot].x].name;
	if (s_blocks[_content[_slot].x].durability) {
		for (auto& dura: _durabilities) {
			if (dura.x == _slot) {
				res += " (" + std::to_string(dura.y) + '/' + std::to_string(dura.z) + ')';
				break ;
			}
		}
	} else if (_content[_slot].x) {
		res += " (" + std::to_string(_content[_slot].y) + ')';
	}
	return (res);
}
