#include "Inventory.hpp"
#include "ChestInstance.hpp"
#include "FurnaceInstance.hpp"
#include "OpenGL_Manager.hpp"
#include "callbacks.hpp"
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
		craft_place = menu::inventory;
		return (&_icraft[value - CELLS::icraft_first]);
	} else if (value <= CELLS::table_craft_last) {
		craft_place = menu::crafting;
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
	} else if (value <= CELLS::anvil_second) {
		craft_place = menu::anvil;
		return (&_anvil[value - CELLS::anvil_first]);
	}
	return (NULL);
}

void Inventory::changeCraftedAnvil( void )
{
	if (_anvil[0].type == blocks::air || _anvil[1].type != blocks::air) {
		_crafted = t_item();
		return ;
	}
	std::string input = inputs::getCurrentMessage();
	if (input == "" && (!_anvil[0].tag || _anvil[0].tag->getName() == "")) {
		_crafted = t_item();
		return ;
	} else if (_anvil[0].tag && _anvil[0].tag->getName() == input) {
		_crafted = t_item();
		return ;
	} else if (!_anvil[0].tag && input == s_blocks[_anvil[0].type]->name) {
		_crafted = t_item();
		return ;
	}
	if (_crafted.type != _anvil[0].type) {
		if (!_anvil[0].tag) {
			_crafted = t_item(_anvil[0].type, _anvil[0].amount, std::make_shared<NameTag>(input));
		} else {
			_crafted = t_item(_anvil[0].type, _anvil[0].amount, _anvil[0].tag->duplicate());
			_crafted.tag->setName(input);
		}
	} else {
		_crafted.amount = _anvil[0].amount;
		_crafted.tag->setName(input);
	}
}

void Inventory::changeCrafted( int state )
{
	if (!state) {
		return ;
	} else if (state == menu::anvil) {
		return (changeCraftedAnvil());
	}
	int minX = 2, minY = 2, maxX = 0, maxY = 0;
	if (state == menu::inventory) {
		for (int index = 0; index < 4; index++) {
			if (_icraft[index].type != blocks::air) {
				minX = glm::min(minX, index & 1);
				maxX = glm::max(maxX, index & 1);
				minY = glm::min(minY, index >> 1);
				maxY = glm::max(maxY, index >> 1);
			}
		}
	} else if (state == menu::crafting) {
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
				if ((state == menu::inventory && _icraft[minX + x + (minY + y) * 2].type != search->second[index + x + y * width])
					|| (state == menu::crafting && _craft[minX + x + (minY + y) * 3].type != search->second[index + x + y * width])) {
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

void Inventory::produceCraft( int state )
{
	if (state == menu::inventory) {
		// std::cout << "produce i craft" << std::endl;
		for (int index = 0; index < 4; index++) {
			if (_icraft[index].amount > 0) {
				if (--_icraft[index].amount == 0) {
					_icraft[index] = {0};
				}
			}
		}
	} else if (state == menu::crafting) {
		// std::cout << "produce craft" << std::endl;
		for (int index = 0; index < 9; index++) {
			if (_craft[index].amount > 0) {
				if (--_craft[index].amount == 0) {
					_craft[index] = {0};
				}
			}
		}
	} else if (state == menu::anvil) {
		_anvil = {t_item(), t_item()};
	}
	changeCrafted(state);
}

t_item Inventory::pickCrafted( int state, t_item block )
{
	if (_crafted.type == blocks::air) {
		return (block);
	}
	if (block.type == blocks::air) {
		t_item ret = _crafted;
		produceCraft(state);
		return (ret);
	} else if (canStack(block, _crafted, false)) {
		block.amount += _crafted.amount;
		produceCraft(state);
	}
	return (block);
}

/**
 * @brief 2 items are considered the same if they are of the same type
 * and have the same tag content
 */
bool Inventory::sameItem( t_item a, t_item b )
{
	if (a.type != b.type) { // not same block X
		return (false);
	}
	if (!a.tag && !b.tag) { // both without tag V
		return (true);
	}
	if (!a.tag || !b.tag) { // only one of them has tag X
		return (false);
	}
	return (!a.tag->compare(b.tag.get()));
}

/**
 * @brief checks if 2 items can stack on each other
 * 2 items stack if one of them is empty
 * or they share the same block and their stacked size doesn't
 * overflow on stackSize and they don't have a name tag / share
 * the same name 
 */
bool Inventory::canStack( t_item a, t_item b, bool emptyAllowed )
{
	if (emptyAllowed && (a.type == blocks::air || b.type == blocks::air)) { // one of them empty
		return (true);
	}
	if (!sameItem(a, b)) {
		return (false);
	}
	return (a.amount + b.amount <= s_blocks[a.type]->stackSize);
}

int Inventory::findEmptyCell( t_item block, bool hotbar_first )
{
	if (s_blocks[block.type]->stackSize > 1) {
		for (int index = 0; index < 9; index++) {
			if (canStack(block, _content[index], false)) {
			// if (_content[index].type == block.type && _content[index].amount + block.amount <= s_blocks[block.type]->stackSize) {
				return (index);
			}
		}
		for (int index = 0; index < 27; index++) {
			if (canStack(block, _backpack[index], false)) {
			// if (_backpack[index].type == block.type && _backpack[index].amount + block.amount <= s_blocks[block.type]->stackSize) {
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

void Inventory::pickAllCrafted( int state )
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
		*bat = pickCrafted(state, *bat);
		if (_crafted.type != bat->type) {
			break ;
		}
	}
	if (_crafted.type != blocks::air) {
		return (pickAllCrafted(state));
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

t_item Inventory::getAnvilblock( int slot )
{
	if (slot < 0 || slot > 1) {
		return (t_item());
	}
	return (_anvil[slot]);
}

int Inventory::getSlotNum( void )
{
	return (_slot);
}

/**
 * @brief set _slot to given arg, then display slot's block's name on ui
 */
void Inventory::setSlot( int value )
{
	if (value < 0 || value >= 9) {
		return ;
	}
    _slot = value;
	_modif = true;
	int type = _content[_slot].type;
	_ui->inventoryMessage((_content[_slot].tag && _content[_slot].tag->getName() != "")
							? _content[_slot].tag->getName()
							: (type != blocks::air) ? s_blocks[type]->name : "");
}

/**
 * @brief moves selected block from one location to the other, depending on args
 * @param craft 0 = inventory, 1 = craft, 2 = furnace
 * @param value selected slot
 * @param furnace ptr to furnace if opened
 * @param chest ptr to chest if opened
*/
void Inventory::shiftBlockAt( std::vector<t_item>& drops, int state, int value, FurnaceInstance *furnace, ChestInstance *chest )
{
	t_item item = pickBlockAt(state, value, furnace, chest);
	if (!item.type) {
		return ;
	}
	switch (state) {
		case menu::inventory:
			if (value <= CELLS::hotbar_last) {
				if (restoreBlock(item)) {
					item.type = blocks::air;
				}
			} else if (restoreBlock(item, true)) {
				item.type = blocks::air;
			}
			break ;
		case menu::crafting:
			if (value <= CELLS::backpack_last) {
				if (s_blocks[item.type]->stackSize > 1) {
					for (int index = 0; index < 9; index++) {
						t_item *bat = &_craft[index];
						if (canStack(*bat, item, false)) {
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
			} else if (restoreBlock(item)) {
				item.type = blocks::air;
			}
			break ;
		default:
			if (furnace) {
				if (value <= CELLS::backpack_last) { // from inventory to furnace
					if (s_blocks[item.type]->isFuel) {
						item = putBlockAt(state, CELLS::furnace_fuel, item, furnace, NULL, false);
					} else {
						item = putBlockAt(state, CELLS::furnace_composant, item, furnace, NULL, false);
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
							if (canStack(*bat, item, false)) {
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
		if (value == CELLS::product) { // drop item on ground
			drops.push_back(item);
		} else {
			putBlockAt(state, value, item, furnace, chest);
		}
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

t_item Inventory::pickBlockAt( int state, int value, FurnaceInstance *furnace, ChestInstance *chest )
{
	// std::cout << "pickBlockAt " << value << std::endl;
	if (value == CELLS::product) {
		if (furnace) {
			return (furnace->pickProduction());
		}
		return (pickCrafted(state, t_item()));
	}
	int craft_place = 0;
	t_item *bat = getBlockPtr(value, craft_place, furnace, chest);
	if (!bat) {
		return (t_item());
	}
	if (bat->type != blocks::air) {
		t_item res = *bat;
		*bat = t_item();
		changeCrafted(craft_place);
		return (res);
	}
	changeCrafted(craft_place);
	return (t_item());
}

t_item Inventory::pickHalfBlockAt( int state, int value, FurnaceInstance *furnace, ChestInstance *chest )
{
	if (value == CELLS::product) {
		if (!furnace) { // for now does nothing if in furnace
			pickAllCrafted(state);
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

t_item Inventory::putBlockAt( int state, int value, t_item block, FurnaceInstance *furnace, ChestInstance *chest, bool swap )
{
	if (value == CELLS::product) {
		if (furnace) {
			return (block);
		}
		return (pickCrafted(state, block));
	}
	int craft_place = 0;
	t_item *bat = getBlockPtr(value, craft_place, furnace, chest);
	if (!bat || (furnace &&  value == CELLS::furnace_fuel && !s_blocks[block.type]->isFuel)) {
		return (block);
	}
	t_item res = *bat;
	int stackSize = s_blocks[block.type]->stackSize;
	if (sameItem(res, block) && stackSize > 1) {
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

t_item Inventory::putOneBlockAt( int state,  int value, t_item block, FurnaceInstance *furnace, ChestInstance *chest )
{
	if (value == CELLS::product) {
		if (!furnace) {
			pickAllCrafted(state);
		}
		return (block);
	}
	int craft_place = 0;
	t_item *bat = getBlockPtr(value, craft_place, furnace, chest);
	if (!bat || (furnace && value == CELLS::furnace_fuel && !s_blocks[block.type]->isFuel)) {
		return (block);
	}
	if (sameItem(*bat, block) && bat->amount + 1 <= s_blocks[block.type]->stackSize) {
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

void Inventory::restoreAnvil( std::vector<t_item>& drops )
{
	for (int index = 0; index < 2; index++) {
		if (_anvil[index].type != blocks::air) {
			if (!restoreBlock(_anvil[index])) {
				drops.push_back(_anvil[index]);
			}
			_anvil[index] = {0};
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

/**
 * @brief called when using a bucket to transform it into
 * water_bucket or place water_bucket in inventory if buckets
 * are stacked or drop water_bucket if no place in inventory
 * and buckets are stacked
 */
t_item Inventory::transformItem( int type )
{
	t_item *bat;
	switch (type) {
		case blocks::water:
			bat = &_content[_slot];
			if (bat->type == blocks::bucket) {
				_modif = true;
				if (bat->amount == 1) {
					bat->type = blocks::water_bucket;
					return (t_item());
				}
				--bat->amount;
				if (!restoreBlock(t_item(blocks::water_bucket, 1), true)) {
					return (t_item(blocks::water_bucket, 1));
				}
			}
			break ;
		default:
			LOGERROR("Inventory::transformItem defaulted with " << type);
	}
	return (t_item());
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
		int dura = s_blocks[TYPE(type)]->durability;
		if (dura) {
			_content[_slot].tag = std::make_shared<ToolTag>(dura);
		} else if (type == blocks::book_and_quill) {
			_content[_slot].tag = std::make_shared<BookTag>();
		} else if (type == blocks::written_book) {
			_content[_slot].tag = std::make_shared<BookTag>();
			_content[_slot].tag->setName("Anonymous");
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
	if (_content[slot].type == bat->type && bat->type != blocks::air) { // TODO check tag here
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

std::string Inventory::getAnvilTag( void )
{
	if (!_crafted.tag) {
		if (_anvil[0].type == blocks::air) {
			return ("");
		}
		if (!_anvil[0].tag || _anvil[0].tag->getName() == "") {
			return (s_blocks[_anvil[0].type]->name);
		}
		return (_anvil[0].tag->getName());
	}
	return (_crafted.tag->getName());
}

void Inventory::updateAnvilTag( void )
{
	changeCraftedAnvil();
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
		LOGERROR("Inventory::signBook item is not book and quill.");
		return ;
	}
	if (!_content[_slot].tag || _content[_slot].tag->getType() != tags::book_tag) {
		LOGERROR("Inventory::signBook invalid tag.");
		return ;
	}
	_content[_slot].type = blocks::written_book;
	_content[_slot].tag->setName(title);
	_modif = true;
}

// -------------------------

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
