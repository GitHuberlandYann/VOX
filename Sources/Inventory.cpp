#include "Inventory.hpp"
#include "ChestInstance.hpp"
#include "FurnaceInstance.hpp"
#include "OpenGL_Manager.hpp"

Inventory::Inventory( void ) : _slot(0), _modif(false)
{
}

Inventory::~Inventory( void )
{
	std::cout << "Destructor of Inventory called" << std::endl;
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

t_item *Inventory::getBlockPtr( int value, int &craft_place, FurnaceInstance *furnace, ChestInstance *chest )
{
	if (value < 0 || value > 78) {
		return (NULL);
	}
	t_item *res = NULL;
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
	} else if (value < 79) {
		if (!chest) {
			return (NULL);
		}
		res = chest->getItem(value - 52);
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
				if (_icraft[rindex].type != ireceipt[index][rindex]) {
					match = false;
				}
			}
			if (match) {
				int dura = s_blocks[ireceipt[index][4]]->durability;
				_crafted = {ireceipt[index][4], ireceipt[index][5], {dura, dura}};
				return ;
			}
		}
	} else if (craft == 2) {
		for (int index = 0; index < RECEIPT_SIZE; index++) {
			bool match = true;
			for (int rindex = 0; rindex < 9; rindex++) {
				if (_craft[rindex].type != receipt[index][rindex]) {
					match = false;
				}
			}
			if (match) {
				int dura = s_blocks[receipt[index][9]]->durability;
				_crafted = {receipt[index][9], receipt[index][10], {dura, dura}};
				return ;
			}
		}
	}
	_crafted ={0};
}

void Inventory::produceCraft( int craft )
{
	if (craft == 1) {
		// std::cout << "produce i craft" << std::endl;
		for (int index = 0; index < 4; index++) {
			if (_icraft[index].amount > 0) {
				if (--_icraft[index].amount == 0) {
					_icraft[index] = {0};
				}
			}
		}
	} else if (craft == 2) {
		// std::cout << "produce craft" << std::endl;
		for (int index = 0; index < 9; index++) {
			if (_craft[index].amount > 0) {
				if (--_craft[index].amount == 0) {
					_craft[index] = {0};
				}
			}
		}
	}
	changeCrafted(craft);
}

t_item Inventory::pickCrafted( int craft, t_item block )
{
	if (_crafted.type == blocks::AIR) {
		return (block);
	}
	if (block.type == blocks::AIR) {
		t_item ret = _crafted;
		produceCraft(craft);
		return (ret);
	} else if (block.type == _crafted.type && s_blocks[block.type]->stackable && block.amount + _crafted.amount <= 64) {
		block.amount += _crafted.amount;
		produceCraft(craft);
	}
	return (block);
}

int Inventory::findEmptyCell( t_item block, bool swap )
{
	if (s_blocks[block.type]->stackable) {
		for (int index = 0; index < 9; index++) {
			if (_content[index].type == block.type && _content[index].amount + block.amount <= 64) {
				return (index);
			}
		}
		for (int index = 0; index < 27; index++) {
			if (_backpack[index].type == block.type && _backpack[index].amount + block.amount <= 64) {
				return (index + 9);
			}
		}
	}
	if (swap) {
		for (int index = 0; index < 9; index++) {
			if (_content[index].type == blocks::AIR) {
				return (index);
			}
		}
	}
	for (int index = 0; index < 27; index++) {
		if (_backpack[index].type == blocks::AIR) {
			return (index + 9);
		}
	}
	if (!swap) {
		for (int index = 0; index < 9; index++) {
			if (_content[index].type == blocks::AIR) {
				return (index);
			}
		}
	}
	return (-1);
}

int Inventory::findBlockCell( int type )
{
	for (int index = 0; index < 9; index++) {
		if (_content[index].type == type) {
			return (index);
		}
	}
	for (int index = 0; index < 27; index++) {
		if (_backpack[index].type == type) {
			return (index + 9);
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
	t_item *bat;
	if (location < 9) {
		bat = &_content[location];
	} else {
		bat = &_backpack[location - 9];
	}
	while (_crafted.type != blocks::AIR && bat->amount + _crafted.amount <= 64) {
		*bat = pickCrafted(craft, *bat);
		if (_crafted.type != bat->type && _crafted.type != blocks::AIR) {
			return (pickAllCrafted(craft));
		}
		if (!s_blocks[bat->type]->stackable) {
			return (pickAllCrafted(craft));
		}
	}
	if (_crafted.type != blocks::AIR) {
		return (pickAllCrafted(craft));
	}
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

int Inventory::getCurrentSlot( void )
{
    return (_content[_slot].type);
}

t_item Inventory::getSlotBlock( int slot )
{
	if (slot < 0 || slot >= 9) {
		return (t_item({0}));
	}
	return (_content[slot]);
}

t_item Inventory::getBackpackBlock( int slot )
{
	if (slot < 0 || slot >= 27) {
		return (t_item({0}));
	}
	return (_backpack[slot]);
}

t_item Inventory::getiCraftBlock( int slot )
{
	if (slot < 0 || slot >= 4) {
		return (t_item({0}));
	}
	return (_icraft[slot]);
}

t_item Inventory::getCraftBlock( int slot )
{
	if (slot < 0 || slot >= 9) {
		return (t_item({0}));
	}
	return (_craft[slot]);
}

t_item Inventory::getCrafted( void )
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
	int type = _content[_slot].type;
	_ui->inventoryMessage((type != blocks::AIR) ? s_blocks[type]->name : "");
}

t_item Inventory::pickBlockAt( int craft, int value, FurnaceInstance *furnace, ChestInstance *chest )
{
	// std::cout << "pickBlockAt " << value << std::endl;
	if (value == 40) {
		if (furnace) {
			return (furnace->pickProduction());
		}
		return (pickCrafted(craft, t_item({0})));
	}
	int craft_place = 0;
	t_item *bat = getBlockPtr(value, craft_place, furnace, chest);
	if (!bat) {
		return (t_item({0}));
	}
	if (bat->type != blocks::AIR) {
		t_item res = *bat;
		*bat = t_item({0});
		changeCrafted(craft);
		return (res);
	}
	changeCrafted(craft_place);
	return (t_item({0}));
}

t_item Inventory::pickHalfBlockAt( int craft, int value, FurnaceInstance *furnace, ChestInstance *chest )
{
	if (value == 40) {
		if (!furnace) { // for now does nothing if in furnace
			pickAllCrafted(craft);
		}
		return (t_item({0}));
	}
	int craft_place = 0;
	t_item *bat = getBlockPtr(value, craft_place, furnace, chest);
	if (!bat) {
		return (t_item({0}));
	}
	if (bat->type != blocks::AIR) {
		t_item res = *bat;
		res.amount = (res.amount >> 1) + (res.amount & 1);
		bat->amount >>= 1;
		if (!bat->amount) {
			*bat = {0};
		}
		changeCrafted(craft_place);
		return (res);
	}
	changeCrafted(craft_place);
	return (t_item({0}));
}

t_item Inventory::putBlockAt( int craft, int value, t_item block, FurnaceInstance *furnace, ChestInstance *chest )
{
	if (value == 40) {
		if (furnace) {
			return (block);
		}
		return (pickCrafted(craft, block));
	}
	int craft_place = 0;
	t_item *bat = getBlockPtr(value, craft_place, furnace, chest);
	if (!bat) {
		return (block);
	}
	t_item res = *bat;
	if (res.type == block.type && s_blocks[block.type]->stackable) {
		res.amount += block.amount;
		if (res.amount > 64) {
			block.amount = res.amount - 64;
			res.amount = 64;
			*bat = res;
			changeCrafted(craft_place);
			return (block);
		}
		*bat = res;
		changeCrafted(craft_place);
		return (t_item({0}));
	}
	*bat = block;
	changeCrafted(craft_place);
	return (res);
}

t_item Inventory::putOneBlockAt( int craft,  int value, t_item block, FurnaceInstance *furnace, ChestInstance *chest )
{
	if (value == 40) {
		if (!furnace) {
			pickAllCrafted(craft);
		}
		return (block);
	}
	int craft_place = 0;
	t_item *bat = getBlockPtr(value, craft_place, furnace, chest);
	if (!bat) {
		return (block);
	}
	if (bat->amount == 64) {
		return (block);
	}
	if (bat->type == block.type && s_blocks[block.type]->stackable) {
		bat->amount++;
	} else if (bat->type == blocks::AIR) {
		*bat = {block.type, 1, block.dura};
	} else {
		return (block);
	}
	changeCrafted(craft_place);
	if (--block.amount == 0) {
		return (t_item({0}));
	}
	return (block);
}

void Inventory::restoreBlock( t_item block, bool swap )
{
	int location = findEmptyCell(block, swap);
	if (location == -1) {
		return ;
	}
	if (location < 9) {
		_content[location] = {block.type, block.amount + _content[location].amount, block.dura};
		_modif = true;
	} else {
		_backpack[location - 9] = {block.type, block.amount + _backpack[location - 9].amount, block.dura};
	}
}

bool Inventory::absorbItem( t_item block )
{
	int location = findEmptyCell(block, true);
	if (location == -1) {
		return (false);
	}
	if (location < 9) {
		_content[location] = {block.type, block.amount + _content[location].amount, block.dura};
		_modif = true;
	} else {
		_backpack[location - 9] = {block.type, block.amount + _backpack[location - 9].amount, block.dura};
	}
	return (true);
}

void Inventory::restoreiCraft( void )
{
	for (int index = 0; index < 4; index++) {
		if (_icraft[index].type != blocks::AIR) {
			restoreBlock(_icraft[index]);
			_icraft[index] = {0};
		}
	}
	_crafted = {0};
}

void Inventory::restoreCraft( void )
{
	for (int index = 0; index < 9; index++) {
		if (_craft[index].type != blocks::AIR) {
			restoreBlock(_craft[index]);
			_craft[index] = {0};
		}
	}
	_crafted = {0};
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
		if (_content[_slot].type == blocks::BUCKET) {
			if (_content[_slot].amount == 1) {
				_content[_slot].type = blocks::WATER_BUCKET;
				_modif = true;
				// std::cout << "and replacing current slot with it" << std::endl;
				return ;
			}
			// std::cout << "and storing it out of slot" << std::endl;
			_content[_slot].amount--;
		} else {
			return ;
		}
	}
	int dura = s_blocks[type]->durability;
	t_item block = {s_blocks[type]->mined, 1, {dura, dura}};
	restoreBlock(block, true);
}
/*
void Inventory::removeBlockAt( int value, FurnaceInstance *furnace, ChestInstance *chest )
{
	if (value < 9) {
		if (s_blocks[_content[value].x]->durability) {
			getrmDura(value);
		}
		if (_content[value].x != blocks::AIR && --_content[value].y <= 0) {
			_content[value] = glm::ivec2(blocks::AIR, 0);
			_modif = true;
		}
	} else if (value < 37) {
		if (s_blocks[_backpack[value].x]->durability) {
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
	(void)chest;
}*/

t_item Inventory::removeBlock( bool thrown )
{
	t_item res = {_content[_slot].type, 1, _content[_slot].dura};
    if (res.type == blocks::AIR) {
        return (res);
    }
	if (!thrown && res.type == blocks::WATER_BUCKET) {
		_content[_slot].type = blocks::BUCKET;
		_modif = true;
	} else if (--_content[_slot].amount <= 0) {
        _content[_slot] = {0};
		_modif = true;
    }
	return (res);
}

void Inventory::replaceSlot( int type, bool creative )
{
	int shape = s_blocks[type]->geometry;
	if (shape == GEOMETRY::SLAB_TOP || shape == GEOMETRY::STAIRS_TOP) {
		--type;
	} else if (type == blocks::PISTON_HEAD || type == blocks::MOVING_PISTON) {
		return ;
	}
	_modif = true;
	if (creative) {
		_content[_slot] = {type, 1, {0, 0}};
		return ;
	}
	int cell = findBlockCell(type);
	if (cell == -1) return ;
	swapCells(_slot, cell);
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
	t_item *bat;
	if (location < 9) {
		bat = &_content[location];
	} else if (location < 36) {
		bat = &_backpack[location - 9];
	} else if (location < 40) {
		bat = &_icraft[location - 36];
	} else {
		bat = &_craft[location - 41];
	}
	if (_content[slot].type == bat->type && bat->type != blocks::AIR) {
		_content[slot].amount += bat->amount;
		if (_content[slot].amount > 64) {
			bat->amount = _content[slot].amount - 64;
			_content[slot].amount = 64;
		} else {
			*bat = {0};
		}
		return ;
	}
	t_item tmp = _content[slot];
	_content[slot] = *bat;
	*bat = tmp;
}

void Inventory::decrementDurabitilty( void )
{
	if (_content[_slot].dura.x) {
		if (--_content[_slot].dura.x == 0) {
			removeBlock(false);
		}
		_modif = true;
	}
}

void Inventory::spillInventory( Chunk *chunk )
{
	t_item details;

	for (int index = 0; index < 9; index++) {
		if (_content[index].type != blocks::AIR) {
			details = _content[index];
			_content[index] = {0};
			chunk->dropEntity({glm::cos(index), glm::sin(index), 0}, details);
		}
	}
	for (int index = 0; index < 27; index++) {
		if (_backpack[index].type != blocks::AIR) {
			details = _backpack[index];
			_backpack[index] = {0};
			chunk->dropEntity({glm::cos(index + 9), glm::sin(index + 9), 0}, details);
		}
	}
	_modif = true;
}

// -------------------------

std::string Inventory::getInventoryString( void )
{
    std::string res = "\nCurrent Slot > " + std::to_string(_slot);
    for (int index = 0; index < 9; index++) {
        t_item current = _content[index];
        res += "\nslot " + std::to_string(index) + '<' + std::to_string(current.type) + "> times " + std::to_string(current.amount);
    }
    return (res);
}

std::string Inventory::getSlotString( void )
{
	std::string res = "\n\nHolding\t> " + s_blocks[_content[_slot].type]->name;
	if (_content[_slot].dura.x) {
		res += " (" + std::to_string(_content[_slot].dura.x) + '/' + std::to_string(_content[_slot].dura.y) + ')';
	} else if (_content[_slot].type) {
		res += " (" + std::to_string(_content[_slot].amount) + ')';
	}
	return (res);
}

void Inventory::setUIPtr( UI *ui )
{
	_ui = ui;
}
