#include "Inventory.hpp"
#include "ChestInstance.hpp"
#include "FurnaceInstance.hpp"
#include "OpenGL_Manager.hpp"
#include "logs.hpp"

Inventory::Inventory( void ) : _slot(0), _modif(false)
{
	#if 0 // check if hardcoded recipe book has appropriate dimensions at first glance
	for (auto& entry : recipes) {
		size_t lineSize = (entry.first & 0x3) * (entry.first >> 2) + 2; // widthxheight
		LOGERROR("Checking key " << entry.first << " width " << (entry.first & 0x3) << " height "
				<< (entry.first >> 2) << " lineSize " << lineSize << " total size " << entry.second.size());
		assert((!(entry.second.size() % lineSize)) && " incorrect recipe book.");
	}
	#endif
}

Inventory::~Inventory( void )
{
	MAINLOG(LOG("Destructor of Inventory called"));
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

t_item *Inventory::getBlockPtr( int value, int &craft_place, FurnaceInstance *furnace, ChestInstance *chest )
{
	if (value < CELLS::hotbar_first) {
		return (NULL);
	} else if (value <= CELLS::hotbar_last) {
		_modif = true;
		return (&_content[value]);
	} else if (value <= CELLS::backpack_last) {
		return (&_backpack[value - CELLS::backpack_first]);
	} else if (value <= CELLS::icraft_last) {
		craft_place = 1;
		return (&_icraft[value - CELLS::icraft_first]);
	} else if (value <= CELLS::table_craft_last) {
		craft_place = 2;
		return (&_craft[value - CELLS::table_craft_first]);
	} else if (value <= CELLS::furnace_fuel) {
		if (!furnace) {
			return (NULL);
		}
		return (furnace->pickCompoFuel(value == CELLS::furnace_fuel));
	} else if (value <= CELLS::chest_last) {
		if (!chest) {
			return (NULL);
		}
		return (chest->getItem(value - CELLS::chest_first));
	}
	return (NULL);
}

void Inventory::changeCrafted( int craft )
{
	if (!craft) {
		return ;
	}
	int minX = 2, minY = 2, maxX = 0, maxY = 0;
	if (craft == 1) {
		for (int index = 0; index < 4; index++) {
			if (_icraft[index].type != blocks::air) {
				minX = glm::min(minX, index & 1);
				maxX = glm::max(maxX, index & 1);
				minY = glm::min(minY, index >> 1);
				maxY = glm::max(maxY, index >> 1);
			}
		}
	} else if (craft == 2) {
		for (int index = 0; index < 9; index++) {
			if (_craft[index].type != blocks::air) {
				minX = glm::min(minX, index % 3);
				maxX = glm::max(maxX, index % 3);
				minY = glm::min(minY, index / 3);
				maxY = glm::max(maxY, index / 3);
			}
		}
	}
	if (minX > maxX) { // _icraft/_craft grid is empty
		_crafted = {0};
		return ;
	}
	int width = maxX + 1 - minX, height = maxY + 1 - minY;
	int key = width + (height << 2);
	auto search = recipes.find(key);
	if (search == recipes.end()) {
		_crafted = {0};
		return ;
	}
	size_t line = (width * height) + 2;
	for (size_t index = 0; index < search->second.size(); index += line) {
		bool match = true;
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				if ((craft == 1 && _icraft[minX + x + (minY + y) * 2].type != search->second[index + x + y * width])
					|| (craft == 2 && _craft[minX + x + (minY + y) * 3].type != search->second[index + x + y * width])) {
					match = false;
					break ;
				}
			}
			if (!match) break ;
		}
		if (match) { // recipe matches recipe in book
			_crafted.type = search->second[index + line - 2];
			_crafted.amount = search->second[index + line - 1];
			int dura = s_blocks[_crafted.type]->durability;
			if (dura) {
				_crafted.tag = std::make_shared<ToolTag>(dura);
			} else if (_crafted.type == blocks::book_and_quill) {
				_crafted.tag = std::make_shared<BookTag>();
			}
			return ;
		}
	}
	_crafted = {0};
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
	if (_crafted.type == blocks::air) {
		return (block);
	}
	if (block.type == blocks::air) {
		t_item ret = _crafted;
		produceCraft(craft);
		return (ret);
	} else if (block.type == _crafted.type && block.amount + _crafted.amount <= s_blocks[block.type]->stackSize) {
		block.amount += _crafted.amount;
		produceCraft(craft);
	}
	return (block);
}

int Inventory::findEmptyCell( t_item block, bool hotbar_first )
{
	if (s_blocks[block.type]->stackSize > 1) {
		for (int index = 0; index < 9; index++) {
			if (_content[index].type == block.type && _content[index].amount + block.amount <= s_blocks[block.type]->stackSize) {
				return (index);
			}
		}
		for (int index = 0; index < 27; index++) {
			if (_backpack[index].type == block.type && _backpack[index].amount + block.amount <= s_blocks[block.type]->stackSize) {
				return (index + CELLS::backpack_first);
			}
		}
	}
	if (hotbar_first) {
		for (int index = 0; index < 9; index++) {
			if (_content[index].type == blocks::air) {
				return (index);
			}
		}
	}
	for (int index = 0; index < 27; index++) {
		if (_backpack[index].type == blocks::air) {
			return (index + CELLS::backpack_first);
		}
	}
	if (!hotbar_first) {
		for (int index = 0; index < 9; index++) {
			if (_content[index].type == blocks::air) {
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
			return (index + CELLS::backpack_first);
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
		bat = &_backpack[location - CELLS::backpack_first];
	}
	while (_crafted.type != blocks::air && bat->amount + _crafted.amount <= s_blocks[_crafted.type]->stackSize) {
		*bat = pickCrafted(craft, *bat);
		if (_crafted.type != bat->type) {
			break ;
		}
	}
	if (_crafted.type != blocks::air) {
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
		return (t_item());
	}
	return (_content[slot]);
}

t_item Inventory::getBackpackBlock( int slot )
{
	if (slot < 0 || slot >= 27) {
		return (t_item());
	}
	return (_backpack[slot]);
}

t_item Inventory::getiCraftBlock( int slot )
{
	if (slot < 0 || slot >= 4) {
		return (t_item());
	}
	return (_icraft[slot]);
}

t_item Inventory::getCraftBlock( int slot )
{
	if (slot < 0 || slot >= 9) {
		return (t_item());
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
	_ui->inventoryMessage((type == blocks::written_book && _content[_slot].tag && _content[_slot].tag->getType() == tags::written_book_tag)
							? static_cast<WrittenBookTag*>(_content[_slot].tag.get())->getTitle()
							: (type != blocks::air) ? s_blocks[type]->name : "");
}

/**
 * @brief moves selected block from one location to the other, depending on args
 * @param craft 0 = inventory, 1 = craft, 2 = furnace
 * @param value selected slot
 * @param furnace ptr to furnace if opened
 * @param chest ptr to chest if opened
*/
void Inventory::shiftBlockAt( int craft, int value, FurnaceInstance *furnace, ChestInstance *chest )
{
	t_item item = pickBlockAt(craft, value, furnace, chest);
	if (!item.type) {
		return ;
	}
	switch (craft) {
		case 1: // inventory
			if (value <= CELLS::hotbar_last) {
				restoreBlock(item);
			} else {
				restoreBlock(item, true);
			}
			item.type = blocks::air;
			break ;
		case 2: // table craft
			if (value <= CELLS::backpack_last) {
				if (s_blocks[item.type]->stackSize > 1) {
					for (int index = 0; index < 9; index++) {
						t_item *bat = &_craft[index];
						if (bat->type == item.type && bat->amount + item.amount <= s_blocks[item.type]->stackSize) {
							bat->amount += item.amount;
							return ;
						}
					}
				}
				for (int index = 0; index < 9; index++) {
					t_item *bat = &_craft[index];
					if (!bat->type) {
						bat->type = item.type;
						bat->amount += item.amount;
						return ;
					}
				}
			} else {
				restoreBlock(item);
				item.type = blocks::air;
			}
			break ;
		default:
			if (furnace) {
				if (value <= CELLS::backpack_last) { // from inventory to furnace
					if (s_blocks[item.type]->isFuel) {
						item = putBlockAt(craft, CELLS::furnace_fuel, item, furnace, NULL, false);
					} else {
						item = putBlockAt(craft, CELLS::furnace_composant, item, furnace, NULL, false);
					}
				} else { // from furnace to inventory
					if (restoreBlock(item)) {
						item.type = blocks::air;
					}
				}
			} else if (chest) {
				if (value <= CELLS::backpack_last) { // from inventory to chest
					if (s_blocks[item.type]->stackSize > 1) {
						for (int index = 0; index < 27; index++) {
							t_item *bat = chest->getItem(index);
							if (bat->type == item.type && bat->amount + item.amount <= s_blocks[item.type]->stackSize) {
								bat->amount += item.amount;
								return ;
							}
						}
					}
					for (int index = 0; index < 27; index++) {
						t_item *bat = chest->getItem(index);
						if (!bat->type) {
							bat->type = item.type;
							bat->amount += item.amount;
							return ;
						}
					}
				} else { // from chest to inventory
					if (restoreBlock(item)) {
						item.type = blocks::air;
					}
				}
			}
			break ;
	}
	if (item.type) { // restore blocks to their original pos
		putBlockAt(craft, value, item, furnace, chest);
	}
}

/**
 * same as pickBlockAt, but only get item's info instead of removing it from it's place
 * used by Menu to display hovered item's info
 */
t_item Inventory::getHoveredAt( int state, int value, FurnaceInstance* furnace, ChestInstance* chest )
{
	(void)state;
	if (value == CELLS::product) {
		if (furnace) {
			return (furnace->getProduction());
		}
		return (_crafted);
	}
	int craft_place = 0;
	t_item *bat = getBlockPtr(value, craft_place, furnace, chest);
	if (!bat) {
		return (t_item());
	}
	return (*bat);
}

t_item Inventory::pickBlockAt( int craft, int value, FurnaceInstance *furnace, ChestInstance *chest )
{
	// std::cout << "pickBlockAt " << value << std::endl;
	if (value == CELLS::product) {
		if (furnace) {
			return (furnace->pickProduction());
		}
		return (pickCrafted(craft, t_item()));
	}
	int craft_place = 0;
	t_item *bat = getBlockPtr(value, craft_place, furnace, chest);
	if (!bat) {
		return (t_item());
	}
	if (bat->type != blocks::air) {
		t_item res = *bat;
		*bat = t_item();
		changeCrafted(craft);
		return (res);
	}
	changeCrafted(craft_place);
	return (t_item());
}

t_item Inventory::pickHalfBlockAt( int craft, int value, FurnaceInstance *furnace, ChestInstance *chest )
{
	if (value == CELLS::product) {
		if (!furnace) { // for now does nothing if in furnace
			pickAllCrafted(craft);
		}
		return (t_item());
	}
	int craft_place = 0;
	t_item *bat = getBlockPtr(value, craft_place, furnace, chest);
	if (!bat) {
		return (t_item());
	}
	if (bat->type != blocks::air) {
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
	return (t_item());
}

t_item Inventory::putBlockAt( int craft, int value, t_item block, FurnaceInstance *furnace, ChestInstance *chest, bool swap )
{
	if (value == CELLS::product) {
		if (furnace) {
			return (block);
		}
		return (pickCrafted(craft, block));
	}
	int craft_place = 0;
	t_item *bat = getBlockPtr(value, craft_place, furnace, chest);
	if (!bat || (furnace &&  value == CELLS::furnace_fuel && !s_blocks[block.type]->isFuel)) {
		return (block);
	}
	t_item res = *bat;
	int stackSize = s_blocks[block.type]->stackSize;
	if (res.type == block.type && stackSize > 1) {
		res.amount += block.amount;
		if (res.amount > stackSize) {
			block.amount = res.amount - stackSize;
			res.amount = stackSize;
			*bat = res;
			changeCrafted(craft_place);
			return (block);
		}
		*bat = res;
		changeCrafted(craft_place);
		return (t_item());
	}
	if (!swap) {
		if (bat->type == blocks::air) {
			*bat = block;
			return (t_item());
		}
		return (block);
	}
	*bat = block;
	changeCrafted(craft_place);
	return (res);
}

t_item Inventory::putOneBlockAt( int craft,  int value, t_item block, FurnaceInstance *furnace, ChestInstance *chest )
{
	if (value == CELLS::product) {
		if (!furnace) {
			pickAllCrafted(craft);
		}
		return (block);
	}
	int craft_place = 0;
	t_item *bat = getBlockPtr(value, craft_place, furnace, chest);
	if (!bat || (furnace && value == CELLS::furnace_fuel && !s_blocks[block.type]->isFuel)) {
		return (block);
	}
	if (bat->type == block.type && bat->amount + 1 <= s_blocks[block.type]->stackSize) {
		++bat->amount;
	} else if (bat->type == blocks::air) {
		*bat = {block.type, 1, block.tag};
	} else {
		return (block);
	}
	changeCrafted(craft_place);
	if (--block.amount <= 0) {
		return (t_item());
	}
	return (block);
}

/**
 * @brief find cell capable of receiving block and place block in it
 * @return false if cell not found
*/
bool Inventory::restoreBlock( t_item block, bool hotbar_first )
{
	if (block.type == blocks::air) {
		return (true);
	}
	int location = findEmptyCell(block, hotbar_first);
	if (location == -1) {
		return (false);
	}
	if (location <= CELLS::hotbar_last) {
		_content[location] = {block.type, block.amount + _content[location].amount, block.tag};
		_modif = true;
	} else {
		_backpack[location - CELLS::backpack_first] = {block.type, block.amount + _backpack[location - CELLS::backpack_first].amount, block.tag};
	}
	return (true);
}

bool Inventory::absorbItem( t_item block )
{
	int location = findEmptyCell(block, true);
	if (location == -1) {
		return (false);
	}
	if (location <= CELLS::hotbar_last) {
		_content[location] = {block.type, block.amount + _content[location].amount, block.tag};
		_modif = true;
	} else {
		_backpack[location - CELLS::backpack_first] = {block.type, block.amount + _backpack[location - CELLS::backpack_first].amount, block.tag};
	}
	return (true);
}

void Inventory::restoreiCraft( std::vector<t_item>& drops )
{
	for (int index = 0; index < 4; index++) {
		if (_icraft[index].type != blocks::air) {
			if (!restoreBlock(_icraft[index])) {
				drops.push_back(_icraft[index]);
			}
			_icraft[index] = {0};
		}
	}
	_crafted = {0};
}

void Inventory::restoreCraft( std::vector<t_item>& drops )
{
	for (int index = 0; index < 9; index++) {
		if (_craft[index].type != blocks::air) {
			if (!restoreBlock(_craft[index])) {
				drops.push_back(_craft[index]);
			}
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
	if (type == blocks::water) {
		// std::cout << "adding water bucket to inventory" << std::endl;
		if (_content[_slot].type == blocks::bucket) {
			if (_content[_slot].amount == 1) {
				_content[_slot].type = blocks::water_bucket;
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
	t_item block = {s_blocks[type]->mined, 1};
	if (dura) {
		block.tag = std::make_shared<ToolTag>(dura);
	}
	restoreBlock(block, true);
}
/*
void Inventory::removeBlockAt( int value, FurnaceInstance *furnace, ChestInstance *chest )
{
	if (value < 9) {
		if (s_blocks[_content[value].x]->durability) {
			getrmDura(value);
		}
		if (_content[value].x != blocks::air && --_content[value].y <= 0) {
			_content[value] = glm::ivec2(blocks::air, 0);
			_modif = true;
		}
	} else if (value < 37) {
		if (s_blocks[_backpack[value].x]->durability) {
			getrmDura(value);
		}
		if (_backpack[value - 9].x != blocks::air && --_backpack[value - 9].y <= 0) {
			_backpack[value - 9] = glm::ivec2(blocks::air, 0);
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
	t_item res = {_content[_slot].type, 1, _content[_slot].tag};
    if (res.type == blocks::air) {
        return (res);
    }
	if (!thrown && res.type == blocks::water_bucket) {
		_content[_slot].type = blocks::bucket;
		_modif = true;
	} else if (--_content[_slot].amount <= 0) {
        _content[_slot] = {0};
		_modif = true;
    }
	return (res);
}

void Inventory::replaceSlot( int type, bool creative )
{
	if (type == blocks::piston_head || type == blocks::moving_piston) {
		return ;
	}
	_modif = true;
	if (creative) {
		_content[_slot] = {type, 1};
		int dura = s_blocks[type & mask::blocks::type]->durability;
		if (dura) {
			_content[_slot].tag = std::make_shared<ToolTag>(dura);
		} else if (type == blocks::book_and_quill) {
			_content[_slot].tag = std::make_shared<BookTag>();
		} else if (type == blocks::written_book) {
			_content[_slot].tag = std::make_shared<WrittenBookTag>();
			static_cast<WrittenBookTag*>(_content[_slot].tag.get())->setTitle("Anonymous");
		}
		return ;
	}
	int cell = findBlockCell(type);
	if (cell == -1) return ;
	swapCells(_slot, cell);
}

void Inventory::swapCells( int slot, int location )
{
	if (slot == location || location == CELLS::product) {
		return ;
	}
	if (location < CELLS::hotbar_first || location > CELLS::table_craft_last) {
		return ;
	}
	_modif = true;
	t_item *bat;
	if (location <= CELLS::hotbar_last) {
		bat = &_content[location];
	} else if (location <= CELLS::backpack_last) {
		bat = &_backpack[location - CELLS::backpack_first];
	} else if (location <= CELLS::icraft_last) {
		bat = &_icraft[location - CELLS::icraft_first];
	} else {
		bat = &_craft[location - CELLS::table_craft_first];
	}
	if (_content[slot].type == bat->type && bat->type != blocks::air) {
		_content[slot].amount += bat->amount;
		int stackSize = s_blocks[bat->type]->stackSize;
		if (_content[slot].amount > stackSize) {
			bat->amount = _content[slot].amount - stackSize;
			_content[slot].amount = stackSize;
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
	if (_content[_slot].tag && _content[_slot].tag->getType() == tags::tool_tag) {
		if (static_cast<ToolTag*>(_content[_slot].tag.get())->decrementDura() == 0) {
			removeBlock(false);
		}
		_modif = true;
	}
}

void Inventory::spillInventory( Chunk* chunk )
{
	t_item details;

	for (int index = 0; index < 9; index++) {
		if (_content[index].type != blocks::air) {
			details = _content[index];
			_content[index] = {0};
			if (chunk) {
				chunk->dropEntity({glm::cos(index), glm::sin(index), 0}, details);
			}
		}
	}
	for (int index = 0; index < 27; index++) {
		if (_backpack[index].type != blocks::air) {
			details = _backpack[index];
			_backpack[index] = {0};
			if (chunk) {
				chunk->dropEntity({glm::cos(index + CELLS::backpack_first), glm::sin(index + CELLS::backpack_first), 0}, details);
			}
		}
	}
	_modif = true;
}

void Inventory::signBook( std::string title )
{
	if (_content[_slot].type != blocks::book_and_quill) {
		LOGERROR("Inventory::singBook item is not book and quill.");
		return ;
	}
	if (!_content[_slot].tag || _content[_slot].tag->getType() != tags::book_tag) {
		LOGERROR("Inventory::singBook invalid tag.");
		return ;
	}
	_content[_slot].type = blocks::written_book;
	_content[_slot].tag = std::make_shared<WrittenBookTag>(title, static_cast<BookTag*>(_content[_slot].tag.get())->getContent());
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
	std::string res = "\n\nHolding\t> " + s_blocks[_content[_slot].type]->name + " [id: " + std::to_string(_content[_slot].type) + "]";
	if (_content[_slot].tag && _content[_slot].tag->getType() == tags::tool_tag) {
		res += " (" + std::to_string(static_cast<ToolTag*>(_content[_slot].tag.get())->getDura()) + '/' + std::to_string(s_blocks[_content[_slot].type]->durability) + ')';
	} else if (_content[_slot].type) {
		res += " (" + std::to_string(_content[_slot].amount) + ')';
	}
	return (res);
}

void Inventory::setUIPtr( UI *ui )
{
	_ui = ui;
}
