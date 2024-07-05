#include "Player.hpp"
#include "random.hpp"
#include "logs.hpp"
#include "Menu.hpp"

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

/* generate one or many entities depending on block broken */
void Chunk::entity_block( int posX, int posY, int posZ, int value )
{
	int type = (value & mask::blocks::type);
	BLOCKLOG(LOG("breaking " << s_blocks[type]->name));
	float random;
	switch (type) {
	case blocks::grass:
		random = Random::randomFloat(_seed);
		if (random <= 0.125f) {
			_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f), glm::vec3(glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f), false, t_item(blocks::wheat_seeds, 1)));
		}
		break ;
	case blocks::oak_leaves:
		random = Random::randomFloat(_seed);
		if (random <= 0.05f) {
			_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f), glm::vec3(glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f), false, t_item(blocks::oak_sapling, 1)));
		}
		random = Random::randomFloat(_seed);
		if (random <= 0.02f) {
			_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f), glm::vec3(glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f), false, t_item(blocks::stick, 2)));
		}
		random = Random::randomFloat(_seed);
		if (random <= 0.005f) {
			_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f), glm::vec3(glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f), false, t_item(blocks::apple, 1)));
		}
		break ;
	case blocks::wheat_crop:
		if ((value >> offset::blocks::bitfield) < 7) { // not fully grown
			_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f), glm::vec3(glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f), false, t_item(s_blocks[type]->mined, 1)));
			return ;
		}
		_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f), glm::vec3(glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f), false, t_item(blocks::wheat, 1)));
		random = Random::randomFloat(_seed);
		if (random <= 0.0787f) {
			_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f), glm::vec3(glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f), false, t_item(blocks::wheat_seeds, 1)));
		} else if (random <= 0.0787f + 0.3149f) {
			_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f), glm::vec3(glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f), false, t_item(blocks::wheat_seeds, 2)));
		} else if (random <= 0.0787f + 0.3149f + 0.4198f) {
			_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f), glm::vec3(glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f), false, t_item(blocks::wheat_seeds, 3)));
		} else {
			_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f), glm::vec3(glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f), false, t_item(blocks::wheat_seeds, 4)));
		}
		break ;
	case blocks::gravel:
		random = Random::randomFloat(_seed);
		if (random < 0.1f) {
			_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f), glm::vec3(glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f), false, t_item(blocks::flint, 1)));
			return ;
		} // no break on gravel so it drops a gravel block
	default:
		_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f), glm::vec3(glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f), false, t_item(s_blocks[type]->mined, 1)));
	}
}

void Chunk::remove_block( bool useInventory, glm::ivec3 pos )
{
	int offset = (((pos.x << settings::consts::chunk_shift) + pos.y) << settings::consts::world_shift) + pos.z;
	int value = _blocks[offset], type = value & mask::blocks::type;
	if (type == blocks::chest) {
		auto search = _chests.find(offset);
		if (search != _chests.end()) {
			t_item *item;
			for (int index = 0; index < 27; ++index) {
				item = search->second->getItem(index);
				if (item->amount) {
					_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f), glm::vec3(glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f), false, *item));
				}
			}
			delete _chests[offset]; // TODO what happens if chest explodes while we use it ? prob heap use after free in Menu.cpp
			_chests.erase(offset);
		}
	} else if (type == blocks::furnace) {
		auto search = _furnaces.find(offset); // drop furnace's items
		if (search != _furnaces.end()) {
			t_item items = search->second->getComposant();
			if (items.amount) {
				_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f), glm::vec3(glm::normalize(glm::vec2(-1, -1)), 1.0f), false, items));
			}
			items = search->second->getFuel();
			if (items.amount) {
				_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f), glm::vec3(glm::normalize(glm::vec2(1, 9)), 1.0f), false, items));
			}
			items = search->second->getProduction();
			if (items.amount) {
				_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f), glm::vec3(glm::normalize(glm::vec2(-4, 5)), 1.0f), false, items));
			}
			delete _furnaces[offset];
			_furnaces.erase(offset);
		}
	} else if (type == blocks::oak_sign) {
		auto search = _signs.find(offset);
		if (search != _signs.end()) {
			delete _signs[offset];
			_signs.erase(offset);
		}
	} else if (type == blocks::item_frame) {
		if (value & mask::frame::locked) {
			return ;
		}
		auto search = std::find_if(_entities.begin(), _entities.end(), [this, pos](auto e) { return (e->isAt(pos + glm::ivec3(_startX, _startY, 0))); });
		if (search != _entities.end()) {
			t_item item = static_cast<ItemFrameEntity*>(search->get())->popContent();
			if (item.type != blocks::air) { // pop item from frame instead of destroying frame
				_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f), glm::vec3(glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f), false, item));
				updateItemFrame(pos + getAttachedDir(value));
				return ;
			}
			_entities.erase(search);
		} else {
			LOGERROR("Item frame entity not found when trying to destroy item frame");
		}
	} else if (type == blocks::lectern) {
		auto search = std::find_if(_entities.begin(), _entities.end(), [this, pos](auto e) { return (e->isAt(pos + glm::ivec3(_startX, _startY, 0))); });
		if (search != _entities.end()) {
			t_item item = static_cast<LecternEntity*>(search->get())->popContent();
			if (item.type != blocks::air) { // pop item from lectern instead of destroying lectern
				_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f), glm::vec3(glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f), false, item));
				updateLectern(pos);
				return ;
			}
			_entities.erase(search);
		} else {
			LOGERROR("Lectern entity not found when trying to destroy lectern");
		}
	} else if (type == blocks::oak_door) {
		// break other part of door
		_blocks[offset + (((value >> offset::blocks::bitfield) & door::upper_half) ? -1 : 1)] = blocks::air;
		_added.erase(offset + (((value >> offset::blocks::bitfield) & door::upper_half) ? -1 : 1));
		_removed.insert(offset + (((value >> offset::blocks::bitfield) & door::upper_half) ? -1 : 1));
	} else if (type == blocks::lever && (value & mask::redstone::powered)) {
		flickLever(pos, value & (mask::all_bits - mask::redstone::powered), redstone::off);
	}
	if (useInventory && type == blocks::water) {
		_inventory->addBlock(type); // water bucket
	} else if (useInventory) {
		entity_block(pos.x, pos.y, pos.z, value);
	}

	setBlockAt(blocks::air, pos, true);
}

void Chunk::addFlame( int offset, glm::vec3 pos, int source, int orientation )
{
	if (source == blocks::torch) {
		switch (orientation) {
		case face_dir::minus_x:
			_flames.emplace(offset, new Particle(this, {pos.x + _startX + 0.25f, pos.y + _startY + 0.5f, pos.z + 15.0f / 16.0f}, PARTICLES::FLAME));
			break ;
		case face_dir::plus_x:
			_flames.emplace(offset, new Particle(this, {pos.x + _startX + 0.75f, pos.y + _startY + 0.5f, pos.z + 15.0f / 16.0f}, PARTICLES::FLAME));
			break ;
		case face_dir::minus_y:
			_flames.emplace(offset, new Particle(this, {pos.x + _startX + 0.5f, pos.y + _startY + 0.25f, pos.z + 15.0f / 16.0f}, PARTICLES::FLAME));
			break ;
		case face_dir::plus_y:
			_flames.emplace(offset, new Particle(this, {pos.x + _startX + 0.5f, pos.y + _startY + 0.75f, pos.z + 15.0f / 16.0f}, PARTICLES::FLAME));
			break ;
		case face_dir::minus_z:
			_flames.emplace(offset, new Particle(this, {pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 11.0f / 16.0f + 0.05f}, PARTICLES::FLAME));
			break ;
		}
	}
}

void Chunk::add_block( bool useInventory, glm::ivec3 pos, int block_value, int previous )
{
	int offset = (((pos.x << settings::consts::chunk_shift) + pos.y) << settings::consts::world_shift) + pos.z;
	int type = block_value & mask::blocks::type;
	geometry shape = s_blocks[type]->geometry;
	if (type == blocks::sand || type == blocks::gravel) {
		int type_under = (_blocks[offset - 1] & mask::blocks::type);
		if (type_under == blocks::air) {
			if (useInventory) {
				_inventory->removeBlock(false);
			}
			_entities.push_back(std::make_shared<FallingBlockEntity>(this, glm::vec3(pos.x + _startX, pos.y + _startY, pos.z), t_item(type, 1)));
			return ;
		}
	}
	if (type == blocks::water) { // we place water
		if (previous != blocks::water) {
			_hasWater = true;
		}
	} else if (shape == geometry::torch) {
		// check if orientation possible (wall available to hang from)
		// if not, check if block underneath and change orientation
		// else abort mission
		geometry neighbourShape = geometry::none;
		switch ((block_value >> offset::blocks::orientation) & 0x7) {
		case face_dir::plus_x:
			neighbourShape = s_blocks[getBlockAt(pos.x + 1, pos.y, pos.z) & mask::blocks::type]->geometry;
			break;
		case face_dir::minus_x:
			neighbourShape = s_blocks[getBlockAt(pos.x - 1, pos.y, pos.z) & mask::blocks::type]->geometry;
			break;
		case face_dir::plus_y:
			neighbourShape = s_blocks[getBlockAt(pos.x, pos.y + 1, pos.z) & mask::blocks::type]->geometry;
			break;
		case face_dir::minus_y:
			neighbourShape = s_blocks[getBlockAt(pos.x, pos.y - 1, pos.z) & mask::blocks::type]->geometry;
			break;
		}
		if (neighbourShape != geometry::cube) {
			block_value = type + (face_dir::minus_z << offset::blocks::orientation);
			neighbourShape = s_blocks[getBlockAt(pos.x, pos.y, pos.z - 1) & mask::blocks::type]->geometry;
			if (!(neighbourShape == geometry::cube || neighbourShape == geometry::fence
				|| (neighbourShape == geometry::slab && (getBlockAt(pos.x, pos.y, pos.z - 1) & mask::slab::top))
				|| (neighbourShape == geometry::stairs && (getBlockAt(pos.x, pos.y, pos.z - 1) & mask::stairs::top)))) {
				return ;
			}
		}
		if (type == blocks::torch) {
			addFlame(offset, pos, blocks::torch, (block_value >> offset::blocks::orientation) & 0x7);
		}
	} else if (shape == geometry::piston) {
		block_value += (_player->getOrientation6() << offset::blocks::orientation);
	} else if (type == blocks::observer) {
		block_value += (opposite_dir(_player->getOrientation6()) << offset::blocks::orientation);
	} else if (s_blocks[type]->oriented) {
		if ((type != blocks::lever && shape != geometry::button && type != blocks::item_frame) || ((block_value >> offset::blocks::bitfield) & 0x3) != placement::wall) {
			block_value += (_player->getOrientation() << offset::blocks::orientation);
		}
		switch (shape) {
			case geometry::stairs:
				handle_stair_corners(pos, block_value);
				break ;
			case geometry::door:
				handle_door_placement(pos, block_value);
				if (block_value == blocks::air) return ; // door couldn't be placed
				else if (!((block_value >> offset::blocks::bitfield) & door::upper_half)) { // place upper_half
					_blocks[offset + 1] = block_value | (door::upper_half << offset::blocks::bitfield);
					_removed.erase(offset + 1);
					_added[offset + 1] = block_value | (door::upper_half << offset::blocks::bitfield);
				}
				break ;
			case geometry::repeater:
				previous = getBlockAt(pos.x, pos.y, pos.z - 1);
				if (s_blocks[previous & mask::blocks::type]->isTransparent(previous) && (previous & mask::blocks::type) != blocks::glass && (previous & mask::blocks::type) != blocks::observer) {
					return ;
				}
				block_value ^= (1 << offset::blocks::orientation); // inverse dir (0->1, 1->0, 2->3, 3->2)
				initRepeater(pos, block_value, true);
				break ;
			default:
				break ;
		}
	} else if (type == blocks::oak_fence || type == blocks::glass_pane) {
		handle_fence_placement(pos, block_value);
	} else if ((shape == geometry::cross || shape == geometry::crop) && pos.z > 0) {
		if (previous == blocks::water) {
			return ;
		}
		int type_below = _blocks[offset - 1] & mask::blocks::type;
		if (type == blocks::wheat_crop) {
		} else if (type_below != blocks::grass_block && type_below != blocks::dirt && type_below != blocks::sand) {
			return ; // can't place flower on something else than grass/dirt block
		} else if (!(type_below > blocks::air && type_below < blocks::poppy) || s_blocks[type_below]->hasHitbox) {
			return ;
		}
	} else if (shape == geometry::dust) {
		geometry shape_below = s_blocks[_blocks[offset - 1] & mask::blocks::type]->geometry;
		if (!(shape_below == geometry::cube|| shape_below == geometry::piston || shape_below == geometry::glass
			|| (shape_below == geometry::slab && (getBlockAt(pos.x, pos.y, pos.z - 1) & mask::slab::top))
			|| (shape_below == geometry::stairs && (getBlockAt(pos.x, pos.y, pos.z - 1) & mask::stairs::top)))) {
			return ;
		}
	}
	if (useInventory) {
		_inventory->removeBlock(false);
	}

	setBlockAt(block_value, pos, true);
}

void Chunk::replace_block( bool useInventory, glm::ivec3 pos, int type )
{
	BLOCKLOG(LOG("replace_block " << useInventory));
	if (useInventory) {
		_inventory->decrementDurabitilty();
	}
	setBlockAt(type, pos.x, pos.y, pos.z, true);
	light_spread(pos.x, pos.y, pos.z, false);
	light_spread(pos.x, pos.y, pos.z, true);
}

/**
 * @brief update block at given pos depending of block type
 * @param pos relative position of block being used
*/
void Chunk::use_block( bool useInventory, glm::ivec3 pos, int type )
{
	int offset = (((pos.x << settings::consts::chunk_shift) + pos.y) << settings::consts::world_shift) + pos.z;
	int value = _blocks[offset];
	if ((value & mask::blocks::type) == blocks::item_frame) {
		if (value & mask::frame::locked) {
			return ;
		}
		auto search = std::find_if(_entities.begin(), _entities.end(), [this, pos](auto e) { return (e->isAt(pos + glm::ivec3(_startX, _startY, .0f))); });
		if (search != _entities.end()) {
			if (static_cast<ItemFrameEntity*>(search->get())->rotate(_inventory->getSlotBlock(_inventory->getSlotNum())) && useInventory) {
				_inventory->removeBlock(false);
			}
			updateItemFrame(pos + getAttachedDir(value));
		} else {
			LOGERROR("Item frame entity not found when trying to rotate/place item");
		}
		return ;
	} else if ((value & mask::blocks::type) == blocks::lectern) {
		auto search = std::find_if(_entities.begin(), _entities.end(), [this, pos](auto e) { return (e->isAt(pos + glm::ivec3(_startX, _startY, .0f))); });
		if (search != _entities.end()) {
			static_cast<LecternEntity*>(search->get())->setContent(_inventory->getSlotBlock(_inventory->getSlotNum()));
			_inventory->removeBlock(false);
			updateLectern(pos);
		} else {
			LOGERROR("Lectern entity not found when trying to place book");
		}
		return ;
	}
	if (!s_blocks[type]->isItemOnly && (value & mask::blocks::type) != type) {
		LOGERROR("Chunk::use_block type diff " << s_blocks[value & mask::blocks::type]->name << " vs " << s_blocks[type]->name);
		return ;
	}
	switch (type) {
	case blocks::oak_door:
		value ^= (door::open << offset::blocks::bitfield);
		_blocks[offset] = value;
		_added[offset] = value;
		pos.z += ((value >> offset::blocks::bitfield) & door::upper_half) ? -1 : 1;
		value ^= (door::upper_half << offset::blocks::bitfield);
		break ;
	case blocks::oak_trapdoor:
		value ^= (door::open << offset::blocks::bitfield);
		break ;
	case blocks::lever:
		value ^= mask::redstone::powered;
		_blocks[offset] = value; // used by adj dust
		flickLever(pos, value, (value >> offset::redstone::powered) & 0x1);
		break ;
	case blocks::repeater: // add 1 tick delay, mod it by 4
		value = (value & (mask::all_bits - (0x3 << offset::redstone::repeater::ticks)))
				| (((((value >> offset::redstone::repeater::ticks) & 0x3) + 1) & 0x3) << offset::redstone::repeater::ticks);
		break ;
	case blocks::comparator: // switch mode between compare and substract
		value ^= mask::redstone::comparator::mode;
		updateComparator(pos, value, false);
		break ;
	case blocks::stone_button:
	case blocks::oak_button:
		BLOCKLOG(LOG("button pressed"));
		value |= mask::redstone::powered | mask::redstone::strength;
		_blocks[offset] = value; // used by adj dust
		stronglyPower(pos + getAttachedDir(value), -getAttachedDir(value), 0xF);
		weaklyPower(pos, {0, 0, 0}, redstone::on, false);
		scheduleRedstoneTick({pos, ((type == blocks::stone_button) ? 10 : 15) * redstone::tick});
		break ;
	case blocks::zombie_egg:
		BLOCKLOG(LOG("zombie egg placed"));
		_mobs.push_back(std::make_shared<Zombie>(this, _player, pos + glm::ivec3(_startX, _startY, 0)));
		return ;
	case blocks::skeleton_egg:
		BLOCKLOG(LOG("skeleton egg placed"));
		_mobs.push_back(std::make_shared<Skeleton>(this, _player, pos + glm::ivec3(_startX, _startY, 0)));
		return ;
	default:
		LOGERROR("ERROR Chunk::regeneration case Modif::use defaulted on: " << s_blocks[value & mask::blocks::type]->name);
		return ;
	}
	setBlockAt(value, pos, true);
}

/**
 * @brief calls appropriate funcion from given user-induced block modification
 * @param useInventory if true, we update player's inventory when needed or generate entities when needed
 * @param type block's type
 * @param pos absolute position of block
 * @param modif one of REMOVE-ADD-REPLACE-LITNT-USE
*/
void Chunk::regeneration( bool useInventory, int type, glm::ivec3 pos, Modif modif )
{
	int previous_type = _blocks[(((pos.x << settings::consts::chunk_shift) + pos.y) << settings::consts::world_shift) + pos.z] & mask::blocks::type;
	switch (modif) {
		case Modif::rm:
			if (previous_type == blocks::air || previous_type == blocks::bedrock) { // can't rm bedrock
				return ;
			}
			remove_block(useInventory, pos);
		break ;
		case Modif::add:
			if (type == blocks::wheat_crop && (_blocks[(((pos.x << settings::consts::chunk_shift) + pos.y) << settings::consts::world_shift) + pos.z - 1] & mask::blocks::type) != blocks::farmland) { // can't place crop on something other than farmland
				return ;
			} else if ((previous_type != blocks::air && previous_type != blocks::water) || type == blocks::air) { // can't replace block
				return ;
			}
			add_block(useInventory, pos, type, previous_type);
			break ;
		case Modif::replace:
			if (type == blocks::dirt_path && pos.z < 254 && (_blocks[(((pos.x << settings::consts::chunk_shift) + pos.y) << settings::consts::world_shift) + pos.z + 1] & mask::blocks::type) != blocks::air) { // can't turn dirt to dirt path if anything above it
				return ;
			} else if ((type == blocks::dirt_path || type == blocks::farmland) && previous_type != blocks::dirt && previous_type != blocks::grass_block) {
				return ;
			}
			replace_block(useInventory, pos, type);
			break ;
		case Modif::litnt:
			remove_block(false, pos);
			_entities.push_back(std::make_shared<TNTEntity>(this, glm::vec3(pos.x + _startX, pos.y + _startY, pos.z), glm::vec3(0.2f, 0.2f, 0.4f)));
			break ;
		case Modif::use:
			use_block(useInventory, pos, type);
			break ;
		case Modif::popItem:
			int value = _blocks[(((pos.x << settings::consts::chunk_shift) + pos.y) << settings::consts::world_shift) + pos.z];
			if (value & mask::frame::locked) { return ; }
			auto search = std::find_if(_entities.begin(), _entities.end(), [this, pos](auto e) { return (e->isAt(pos + glm::ivec3(_startX, _startY, .0f))); });
			if (search != _entities.end()) {
				t_item item = static_cast<ItemFrameEntity*>(search->get())->popContent();
				if (item.type != blocks::air) { // pop item from frame
					_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f), glm::vec3(glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f), false, item));
					updateItemFrame(pos + getAttachedDir(value));
				}
				return ;
			} else { LOGERROR("Item frame not found when trying to destroy item frame"); }
			break ;
	}
	if (type == blocks::water || type == blocks::bucket) {
		return ;
	} else if (type == blocks::glass || previous_type == blocks::glass) {
		sort_water(_player->getEyePos(), true);
		return ;
	}
	if (!_light_update) {
		fill_vertex_array();
	}
}

/**
 * @brief update given block, then update its neighbours.
 * This is always called from within the appropriate chunk
 * @param pos relative pos of block in chunk
 * @param previous data_value of block before change occured
 * @param value data_value of block after change occured
*/
void Chunk::update_block( glm::ivec3 pos, int previous, int value )
{
	int type = (value & mask::blocks::type), prev_type = (previous & mask::blocks::type);
	BLOCKLOG(LOG("UPDATE_BLOCK at " << POSXYZ(pos, _startX, _startY, 0) << ". " << s_blocks[prev_type]->name << " -> " << s_blocks[type]->name));
	int offset = (((pos.x << settings::consts::chunk_shift) + pos.y) << settings::consts::world_shift) + pos.z;
	if (type == blocks::sand || type == blocks::gravel) {
		int type_under = (_blocks[offset - 1] & mask::blocks::type);
		if (type_under == blocks::air) {
			if (previous == blocks::air) {
				_entities.push_back(std::make_shared<FallingBlockEntity>(this, glm::vec3(pos.x + _startX, pos.y + _startY, pos.z), t_item(type, 1)));
				setBlockAt(blocks::air, pos, false);
			} else {
				_scheduled_to_fall.push_back(offset);
			}
			return ;
		}
	}
	if (prev_type == blocks::water || type == blocks::water) {
		_fluids.insert(offset);
		if (prev_type != blocks::water) { _hasWater = true; }
	}

	// cut recursion here
	if (prev_type == type) {
		// if (type == blocks::piston || type == blocks::sticky_piston) {
		// 	updatePiston(pos, value);
		// }
		return ;
	}

	// redstone update
	if ((previous & mask::redstone::powered) && prev_type != blocks::lever) {
		weaklyPower(pos, {0, 0, 0}, redstone::off, false);
	} else if (previous & mask::redstone::weakdy_powered) { // we remove a weakdy powered block
		weaklyPower(pos, {0, 0, 0}, redstone::off, true);
	}
	stronglyPower(pos, {0, 0, 0}, redstone::off);
	if (type == blocks::redstone_block) {
		setBlockAt(value | mask::redstone::powered, pos, false);
		weaklyPower(pos, {0, 0, 0}, redstone::on, false);
	} else if (s_blocks[type]->redstoneComponant) {
		int active = getRedstoneStrength(pos, {0, 0, 0}, redstone::off, true);
		if (active) {
			value |= mask::redstone::activated;
			int powered = getRedstoneStrength(pos, {0, 0, 0}, redstone::off, false);
			if (powered) {
				value |= mask::redstone::powered;
			}
			setBlockAt(value, pos, false);
			if (type == blocks::redstone_lamp) {
				_lights[offset] &= 0xFF00;
				_lights[offset] += s_blocks[blocks::redstone_lamp]->light_level + (s_blocks[blocks::redstone_lamp]->light_level << 4);
				light_spread(pos.x, pos.y, pos.z, false);
				_light_update = false;
			}
		}
	}

	// specific updates
	if (type == blocks::chest) {
		_chests.emplace(offset, new ChestInstance(this, {pos.x + _startX, pos.y + _startY, pos.z}, _player->getOrientation()));
	} else if (type == blocks::furnace) {
		_furnaces.emplace(offset, new FurnaceInstance());
	} else if (type == blocks::oak_sign) {
		_signs.emplace(offset, new SignInstance(this, value, pos));
	} else if (type == blocks::item_frame) {
		_entities.push_back(std::make_shared<ItemFrameEntity>(this, glm::ivec3(pos.x + _startX, pos.y + _startY, pos.z), value));
	} else if (type == blocks::lectern) {
		_entities.push_back(std::make_shared<LecternEntity>(this, glm::ivec3(pos.x + _startX, pos.y + _startY, pos.z), value));
	} else if (s_blocks[type]->light_level && type != blocks::redstone_lamp) {
		if (type == blocks::redstone_torch) {
			updateRedstoneTorch(pos, value);
		}
		BLOCKLOG(LOG("add light"));
		_lights[offset] &= 0xFF00;
		_lights[offset] += s_blocks[type]->light_level + (s_blocks[type]->light_level << 4);
		light_spread(pos.x, pos.y, pos.z, false);
		_light_update = false;
		BLOCKLOG(LOG("over"));
	} else if (type == blocks::glass) {
		_hasWater = true; // glass considered as invis block
	} else if (type == blocks::redstone_dust) {
		if (prev_type == blocks::air) {
			connectRedstoneDust(pos, value, true);
			setBlockAt(value, pos, false);
		}
		updateRedstoneDust(pos);
	} else if (type == blocks::air) { // rm block
		if (s_blocks[prev_type]->light_level) {
			BLOCKLOG(LOG("rm light"));
			if (prev_type == blocks::torch) {
				delete _flames[offset];
				_flames.erase(offset);
			} else if (prev_type == blocks::redstone_torch) {
				updateRedstoneTorch(pos, value);
			}
			_lights[offset] &= 0xFF00;
			// light_spread(pos.x, pos.y, pos.z, false); // spread block light
			// _light_update = false;
			BLOCKLOG(LOG("over"));
		} else if (prev_type == blocks::oak_leaves) {
			_lights[offset] &= 0xFF;
		} else if (prev_type == blocks::lever) {
			flickLever(pos, value, false);
		} else if (prev_type == blocks::redstone_dust) {
			updateRedstoneDust(pos);
		}
		if (_inventory->getSlotBlock(_inventory->getSlotNum()).type != blocks::worldedit_brush) {
			for (int i = 0; i < 15; ++i) {
				_particles.push_back(new Particle(this, {pos.x + _startX + Random::randomFloat(_seed), pos.y + _startY + Random::randomFloat(_seed), pos.z + Random::randomFloat(_seed)}, PARTICLES::BREAKING, 0, prev_type));
			}
		}
		light_spread(pos.x, pos.y, pos.z, false);
		light_spread(pos.x, pos.y, pos.z, true);
		_light_update = false;
	}

	if (!s_blocks[type]->isTransparent(value)) {
		_lights[offset] = 0; // rm light if solid block added
	}

	// update adj blocks
	for (int i = 0; i < 6; ++i) {
		const glm::ivec3 delta = adj_blocks[i];
		update_adj_block(pos + delta, i, type);
	}
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

/**
 * @brief calls Chunk::getBlockAt after substracting _startX and _startY from posX and posY.
 */
GLint Chunk::getBlockAtAbsolute( glm::ivec3 pos )
{
	return (getBlockAt(pos.x - _startX, pos.y - _startY, pos.z));
}

/**
 * @brief calls Chunk::getBlockAt after substracting _startX and _startY from posX and posY.
 */
GLint Chunk::getBlockAtAbsolute( int posX, int posY, int posZ )
{
	return (getBlockAt(posX - _startX, posY - _startY, posZ));
}

/**
 * @brief fun overload of getBlockAt to use glm::ivec3 instead of 3 int args
*/
GLint Chunk::getBlockAt( glm::ivec3 pos, bool askNeighbours )
{
	return (getBlockAt(pos.x, pos.y, pos.z, askNeighbours));
}

/**
 * @brief return block at given pos, if posZ < 0 returns bedrock, if posZ >= world_height returns air
 * @param posX, posY, posZ relative coordinates of targeted block
 * @param askNeighbours [optional, defaults to true] whether you want to call adj chunks
 * @return data_value of block at given pos 
*/
GLint Chunk::getBlockAt( int posX, int posY, int posZ, bool askNeighbours )
{
	if (posZ < 0) {
		return (blocks::bedrock);
	} else if (posZ >= settings::consts::world_height) {
		return (blocks::air);
	}
	int res = blocks::bedrock;
	if (posX < 0) {
		if (askNeighbours && _neighbours[face_dir::minus_x]) {
			res = _neighbours[face_dir::minus_x]->getBlockAt(posX + settings::consts::chunk_size, posY, posZ, askNeighbours);
		}
	} else if (posX >= settings::consts::chunk_size) {
		if (askNeighbours && _neighbours[face_dir::plus_x]) {
			res = _neighbours[face_dir::plus_x]->getBlockAt(posX - settings::consts::chunk_size, posY, posZ, askNeighbours);
		}
	} else if (posY < 0) {
		if (askNeighbours && _neighbours[face_dir::minus_y]) {
			res = _neighbours[face_dir::minus_y]->getBlockAt(posX, posY + settings::consts::chunk_size, posZ, askNeighbours);
		}
	} else if (posY >= settings::consts::chunk_size) {
		if (askNeighbours && _neighbours[face_dir::plus_y]) {
			res = _neighbours[face_dir::plus_y]->getBlockAt(posX, posY - settings::consts::chunk_size, posZ, askNeighbours);
		}
	} else {
		res = _blocks[(((posX << settings::consts::chunk_shift) + posY) << settings::consts::world_shift) + posZ];
	}
	return (res);
}

/**
 * @brief calls Chunk::setBlockAt after substracting _startX and _startY from posX and posY.
 */
void Chunk::setBlockAtAbsolute( int value, glm::ivec3 pos, bool update )
{
	setBlockAt(value, pos.x - _startX, pos.y - _startY, pos.z, update, false);
}

/**
 * @brief calls Chunk::setBlockAt after substracting _startX and _startY from posX and posY.
 */
void Chunk::setBlockAtAbsolute( int value, int posX, int posY, int posZ, bool update )
{
	setBlockAt(value, posX - _startX, posY - _startY, posZ, update, false);
}

/**
 * @brief fun overload of setBlockAt to use glm::ivec3 instead of 3 int args
*/
void Chunk::setBlockAt( int value, glm::ivec3 pos, bool update, bool observer )
{
	setBlockAt(value, pos.x, pos.y, pos.z, update, observer);
}

/**
 * @brief set block at given pos to value after moving to correct chunk
 * @param value data_value you want to set the block to
 * @param posX, posY, posZ relative coordinates of block to set
 * @param update whether you want to update adjacent blocks, redstone, and lighting
 * @param observer defaults to true, if false we don't update adj observers
*/
void Chunk::setBlockAt( int value, int posX, int posY, int posZ, bool update, bool observer )
{
	if (posZ < 0 || posZ >= settings::consts::world_height) {
		return ;
	}
	if (posX < 0) {
		if (_neighbours[face_dir::minus_x]) {
			_neighbours[face_dir::minus_x]->setBlockAt(value, posX + settings::consts::chunk_size, posY, posZ, update, observer);
		}
	} else if (posX >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_x]) {
			_neighbours[face_dir::plus_x]->setBlockAt(value, posX - settings::consts::chunk_size, posY, posZ, update, observer);
		}
	} else if (posY < 0) {
		if (_neighbours[face_dir::minus_y]) {
			_neighbours[face_dir::minus_y]->setBlockAt(value, posX, posY + settings::consts::chunk_size, posZ, update, observer);
		}
	} else if (posY >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_y]) {
			_neighbours[face_dir::plus_y]->setBlockAt(value, posX, posY - settings::consts::chunk_size, posZ, update, observer);
		}
	} else {
		int previous;
		if (update) {
			previous = getBlockAt(posX, posY, posZ);
		}
		int offset = (((posX << settings::consts::chunk_shift) + posY) << settings::consts::world_shift) + posZ;
		_blocks[offset] = value;
		if (value) {
			_added[offset] = value;
			_removed.erase(offset);
		} else {
			_added.erase(offset);
			_removed.insert(offset);
		}
		if (update) {
			update_block({posX, posY, posZ}, previous, value);
		}
		_vertex_update = true;
		// update observers
		if (!observer || (value & mask::blocks::type) == blocks::moving_piston) {
			return ;
		}
		if ((value & mask::blocks::type) == blocks::observer && (previous & mask::blocks::type) == blocks::moving_piston) {
			scheduleRedstoneTick({{posX, posY, posZ}, redstone::tick, redstone::on});
		}
		for (int i = 0; i < 6; ++i) {
			const glm::ivec3 delta = adj_blocks[i];
			int adj = getBlockAt(posX + delta.x, posY + delta.y, posZ + delta.z);
			if ((adj & mask::blocks::type) == blocks::observer && delta == -adj_blocks[(adj >> offset::blocks::orientation) & 0x7]) {
				scheduleRedstoneTick({{posX + delta.x, posY + delta.y, posZ + delta.z}, redstone::tick, redstone::on});
			}
		}
	}
}

/**
 * @brief called by update_block, updates block visibility then call update_block again
 * @param pos relative pos of block in chunk
 * @param dir face_dir from source block to being updated adj block
 * @param source type of block genererating update
*/
void Chunk::update_adj_block( glm::ivec3 pos, int dir, int source )
{
	if (pos.z < 0 || pos.z >= settings::consts::world_height) {
		return ;
	}
	if (pos.x < 0) {
		if (_neighbours[face_dir::minus_x]) {
			pos.x += settings::consts::chunk_size;
			_neighbours[face_dir::minus_x]->update_adj_block(pos, dir, source);
		}
	} else if (pos.x >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_x]) {
			pos.x -= settings::consts::chunk_size;
			_neighbours[face_dir::plus_x]->update_adj_block(pos, dir, source);
		}
	} else if (pos.y < 0) {
		if (_neighbours[face_dir::minus_y]) {
			pos.y += settings::consts::chunk_size;
			_neighbours[face_dir::minus_y]->update_adj_block(pos, dir, source);
		}
	} else if (pos.y >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_y]) {
			pos.y -= settings::consts::chunk_size;
			_neighbours[face_dir::plus_y]->update_adj_block(pos, dir, source);
		}
	} else {
		_vertex_update = true;
		int value = getBlockAt(pos), type = (value & mask::blocks::type);

		bool transparent = s_blocks[source]->isTransparent(source);

		// update light status around placed block
		if (transparent || type == blocks::moving_piston) {
			light_try_spread(pos.x, pos.y, pos.z, 1, true, settings::consts::light_recurse);
			light_try_spread(pos.x, pos.y, pos.z, 1, false, settings::consts::light_recurse);
		}

		if (type == blocks::water) {
			_fluids.insert((((pos.x << settings::consts::chunk_shift) + pos.y) << settings::consts::world_shift) + pos.z);
			return ;
		} else if (type == blocks::air) {
			return ;
		}

		// update block's visibility
		bool visible = face_count(type, pos);
		if (visible && (value & mask::blocks::notVisible)) {
			value ^= mask::blocks::notVisible;
			setBlockAt(value, pos, false);
		} else if (!visible && !(value & mask::blocks::notVisible)) {
			value |= mask::blocks::notVisible;
			setBlockAt(value, pos, false);
		}

		// update fence status
		if (type == blocks::oak_fence || type == blocks::glass_pane) {
			switch (dir) {
				case face_dir::minus_x:
					if (!transparent || source == type) {
						setBlockAt(value | (fence::px << offset::blocks::bitfield), pos, false);
					} else if (source == blocks::air && (value & (fence::px << offset::blocks::bitfield))) {
						setBlockAt(value ^ (fence::px << offset::blocks::bitfield), pos, false);
					}
					break ;
				case face_dir::plus_x:
					if (!transparent || source == type) {
						setBlockAt(value | (fence::mx << offset::blocks::bitfield), pos, false);
					} else if (source == blocks::air && (value & (fence::mx << offset::blocks::bitfield))) {
						setBlockAt(value ^ (fence::mx << offset::blocks::bitfield), pos, false);
					}
					break ;
				case face_dir::minus_y:
					if (!transparent || source == type) {
						setBlockAt(value | (fence::py << offset::blocks::bitfield), pos, false);
					} else if (source == blocks::air && (value & (fence::py << offset::blocks::bitfield))) {
						setBlockAt(value ^ (fence::py << offset::blocks::bitfield), pos, false);
					}
					break ;
				case face_dir::plus_y:
					if (!transparent || source == type) {
						setBlockAt(value | (fence::my << offset::blocks::bitfield), pos, false);
					} else if (source == blocks::air && (value & (fence::my << offset::blocks::bitfield))) {
						setBlockAt(value ^ (fence::my << offset::blocks::bitfield), pos, false);
					}
					break ;
			}
		}

		// break attached block whose attachement got removed
		glm::ivec3 attachement = getAttachedDir(value);
		if (attachement != glm::ivec3(0, 0, 0)) {
			int base = (getBlockAt(pos + attachement) & mask::blocks::type);
			if (base == blocks::air || base == blocks::piston_head) {
				if (type == blocks::item_frame) {
					if (value & mask::frame::locked) { return ; }
					auto search = std::find_if(_entities.begin(), _entities.end(), [this, pos](auto e) { return (e->isAt(pos + glm::ivec3(_startX, _startY, .0f))); });
					if (search != _entities.end()) {
						t_item item = static_cast<ItemFrameEntity*>(search->get())->popContent();
						_entities.erase(search); // erase before push_back to avoid obsolete iterator
						if (item.type != blocks::air) { // pop item from frame instead of destroying frame
							_entities.push_back(std::make_shared<Entity>(this, _inventory, glm::vec3(pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f), glm::vec3(glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f), false, item));
						} else {
							entity_block(pos.x, pos.y, pos.z, value);
						}
						updateItemFrame(pos + getAttachedDir(value));
					}
				} else if (s_blocks[type]->byHand) {
					entity_block(pos.x, pos.y, pos.z, value);
				}
				if (type == blocks::oak_sign) {
					int offset = (((pos.x << settings::consts::chunk_shift) + pos.y) << settings::consts::world_shift) + pos.z;
					auto search = _signs.find(offset);
					if (search != _signs.end()) {
						delete _signs[offset];
						_signs.erase(offset);
					}
				}
				setBlockAt(blocks::air, pos, true);
				return ;
			}
		}

		// disconnect adj dust if solid block placed above
		if (type == blocks::redstone_dust && (source == blocks::repeater || source == blocks::comparator
			|| source == blocks::target || (dir == face_dir::minus_z))) {
			connectRedstoneDust(pos, value, false);
			setBlockAt(value, pos, false);
		}

		update_block(pos, value, value);
	}
}

/**
 * @brief makes sure block being added/removed/used is handled from within correct chunk
 * @param useInventory if true, we update player's inventory when needed or generate entities when needed
 * @param type block's type
 * @param pos absolute position of block
 * @param modif one of REMOVE-ADD-REPLACE-LITNT-USE
*/
void Chunk::handleHit( bool useInventory, int type, glm::ivec3 pos, Modif modif )
{
	glm::ivec3 chunk_pos = {pos.x - _startX, pos.y - _startY, pos.z};
	if (chunk_pos.x < 0) {
		if (_neighbours[face_dir::minus_x]) {
			return (_neighbours[face_dir::minus_x]->handleHit(useInventory, type, pos, modif));
		}
	} else if (chunk_pos.x >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_x]) {
			return (_neighbours[face_dir::plus_x]->handleHit(useInventory, type, pos, modif));
		}
	} else if (chunk_pos.y < 0) {
		if (_neighbours[face_dir::minus_y]) {
			return (_neighbours[face_dir::minus_y]->handleHit(useInventory, type, pos, modif));
		}
	} else if (chunk_pos.y >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_y]) {
			return (_neighbours[face_dir::plus_y]->handleHit(useInventory, type, pos, modif));
		}
	} else {
		regeneration(useInventory, type, chunk_pos, modif);
		return ;
	}
	
	LOGERROR(_startX << ", " << _startY << " ERROR BLOCK OUT OF CHUNK " << POS(chunk_pos));
}

/**
 * @brief returns whether lectern at pos currently holds a written book
 * @param menu used to set/get page content
 * @param pos absolute pos of lectern
 * @param turnPage if true we update lectern's page and check if adj comparators needs update
*/
bool Chunk::bookedLectern( Menu* menu, glm::ivec3 pos, bool turnPage )
{
	glm::ivec3 chunk_pos = {pos.x - _startX, pos.y - _startY, pos.z};
	if (chunk_pos.x < 0) {
		if (_neighbours[face_dir::minus_x]) {
			return (_neighbours[face_dir::minus_x]->bookedLectern(menu, pos, turnPage));
		}
	} else if (chunk_pos.x >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_x]) {
			return (_neighbours[face_dir::plus_x]->bookedLectern(menu, pos, turnPage));
		}
	} else if (chunk_pos.y < 0) {
		if (_neighbours[face_dir::minus_y]) {
			return (_neighbours[face_dir::minus_y]->bookedLectern(menu, pos, turnPage));
		}
	} else if (chunk_pos.y >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_y]) {
			return (_neighbours[face_dir::plus_y]->bookedLectern(menu, pos, turnPage));
		}
	} else {
		auto search = std::find_if(_entities.begin(), _entities.end(), [pos](auto e) { return (e->isAt(pos)); });
		if (search != _entities.end()) {
			t_item item = static_cast<LecternEntity*>(search->get())->getContent();
			if (item.type == blocks::written_book) {
				if (!turnPage) {
					menu->setBookContent(&static_cast<WrittenBookTag*>(item.tag.get())->getContent());
					menu->setBookPage(static_cast<LecternEntity*>(search->get())->getPage());
				} else {
					static_cast<LecternEntity*>(search->get())->setPage(menu->getBookPage());
					updateLectern(chunk_pos);
				}
				return (true);
			}
		} else {
			LOGERROR("Lectern entity not found in bookedLectern in chunk " << _startX << ", " << _startY << " at pos " << POS(pos));
		}
		return (false);
	}
	
	LOGERROR(_startX << ", " << _startY << " Chunk::bookedLectern BLOCK OUT OF CHUNK " << POS(chunk_pos));
	return (false);
}



// ************************************************************************** //
//                         Utils::Handle_placement                            //
// ************************************************************************** //

// takes stair_bottom at given pos and check if it should be corner stair
void Chunk::handle_stair_corners( glm::ivec3 pos, int &type )
{
	int behind, front, left, right;
	switch ((type >> offset::blocks::orientation) & 0x7) {
		case face_dir::minus_x:
			type |= ((corners::pm | corners::pp) << offset::blocks::bitfield);
			behind = getBlockAt(pos.x + 1, pos.y, pos.z);
			if ((behind & (mask::blocks::type | mask::stairs::top)) == (type & (mask::blocks::type | mask::stairs::top))) {
				if (((behind >> offset::blocks::orientation) & 0x7) == face_dir::minus_y) {
					type ^= (corners::pm << offset::blocks::bitfield); // outside corner
					goto SKIP_FRONT_minus_x;
				} else if (((behind >> offset::blocks::orientation) & 0x7) == face_dir::plus_y) {
					type ^= (corners::pp << offset::blocks::bitfield);
					goto SKIP_FRONT_minus_x;
				}
			}
			front = getBlockAt(pos.x - 1, pos.y, pos.z);
			if ((front & (mask::blocks::type | mask::stairs::top)) == (type & (mask::blocks::type | mask::stairs::top))) {
				if (((front >> offset::blocks::orientation) & 0x7) == face_dir::minus_y) {
					type |= (corners::mp << offset::blocks::bitfield); // inside corner
				} else if (((front >> offset::blocks::orientation) & 0x7) == face_dir::plus_y) {
					type |= (corners::mm << offset::blocks::bitfield);
				}
			}
			SKIP_FRONT_minus_x:
			left = getBlockAt(pos.x, pos.y + 1, pos.z);
			if ((left & (mask::blocks::type | mask::stairs::top)) == (type & (mask::blocks::type | mask::stairs::top))) {
				if (((left >> offset::blocks::orientation) & 0x7) == face_dir::minus_y) {
					if (((left >> offset::blocks::bitfield) & 0xF) == (corners::mp | corners::pp)) {
						left |= (corners::pm << offset::blocks::bitfield); // transform left stair into inside corner stair
						setBlockAt(left, pos.x, pos.y + 1, pos.z, false);
					}
				} else if (((left >> offset::blocks::orientation) & 0x7) == face_dir::plus_y) {
					if (((left >> offset::blocks::bitfield) & 0xF) == (corners::mm | corners::pm)) {
						left ^= (corners::mm << offset::blocks::bitfield); // transform left stair into outside corner stair
						setBlockAt(left, pos.x, pos.y + 1, pos.z, false);
					}
				}
			}
			right = getBlockAt(pos.x, pos.y - 1, pos.z);
			if ((right & (mask::blocks::type | mask::stairs::top)) == (type & (mask::blocks::type | mask::stairs::top))) {
				if (((right >> offset::blocks::orientation) & 0x7) == face_dir::minus_y) {
					if (((right >> offset::blocks::bitfield) & 0xF) == (corners::mp | corners::pp)) {
						right ^= (corners::mp << offset::blocks::bitfield); // transform left stair into outside corner stair
						setBlockAt(right, pos.x, pos.y - 1, pos.z, false);
					}
				} else if (((right >> offset::blocks::orientation) & 0x7) == face_dir::plus_y) {
					if (((right >> offset::blocks::bitfield) & 0xF) == (corners::mm | corners::pm)) {
						right |= (corners::pp << offset::blocks::bitfield); // transform right stair into inside corner stair
						setBlockAt(right, pos.x, pos.y - 1, pos.z, false);
					}
				}
			}
			break ;
		case face_dir::plus_x:
			type |= ((corners::mm | corners::mp) << offset::blocks::bitfield);
			behind = getBlockAt(pos.x - 1, pos.y, pos.z);
			if ((behind & (mask::blocks::type | mask::stairs::top)) == (type & (mask::blocks::type | mask::stairs::top))) {
				if (((behind >> offset::blocks::orientation) & 0x7) == face_dir::minus_y) {
					type ^= (corners::mm << offset::blocks::bitfield); // outside corner
					goto SKIP_FRONT_plus_x;
				} else if (((behind >> offset::blocks::orientation) & 0x7) == face_dir::plus_y) {
					type ^= (corners::mp << offset::blocks::bitfield);
					goto SKIP_FRONT_plus_x;
				}
			}
			front = getBlockAt(pos.x + 1, pos.y, pos.z);
			if ((front & (mask::blocks::type | mask::stairs::top)) == (type & (mask::blocks::type | mask::stairs::top))) {
				if (((front >> offset::blocks::orientation) & 0x7) == face_dir::minus_y) {
					type |= (corners::pp << offset::blocks::bitfield); // inside corner
				} else if (((front >> offset::blocks::orientation) & 0x7) == face_dir::plus_y) {
					type |= (corners::pm << offset::blocks::bitfield);
				}
			}
			SKIP_FRONT_plus_x:
			left = getBlockAt(pos.x, pos.y - 1, pos.z);
			if ((left & (mask::blocks::type | mask::stairs::top)) == (type & (mask::blocks::type | mask::stairs::top))) {
				if (((left >> offset::blocks::orientation) & 0x7) == face_dir::minus_y) {
					if (((left >> offset::blocks::bitfield) & 0xF) == (corners::mp | corners::pp)) {
						left ^= (corners::pp << offset::blocks::bitfield); // transform left stair into outside corner stair
						setBlockAt(left, pos.x, pos.y - 1, pos.z, false);
					}
				} else if (((left >> offset::blocks::orientation) & 0x7) == face_dir::plus_y) {
					if (((left >> offset::blocks::bitfield) & 0xF) == (corners::mm | corners::pm)) {
						left |= (corners::mp << offset::blocks::bitfield); // transform left stair into inside corner stair
						setBlockAt(left, pos.x, pos.y - 1, pos.z, false);
					}
				}
			}
			right = getBlockAt(pos.x, pos.y + 1, pos.z);
			if ((right & (mask::blocks::type | mask::stairs::top)) == (type & (mask::blocks::type | mask::stairs::top))) {
				if (((right >> offset::blocks::orientation) & 0x7) == face_dir::minus_y) {
					if (((right >> offset::blocks::bitfield) & 0xF) == (corners::mp | corners::pp)) {
						right |= (corners::mm << offset::blocks::bitfield); // transform right stair into inside corner stair
						setBlockAt(right, pos.x, pos.y + 1, pos.z, false);
					}
				} else if (((right >> offset::blocks::orientation) & 0x7) == face_dir::plus_y) {
					if (((right >> offset::blocks::bitfield) & 0xF) == (corners::mm | corners::pm)) {
						right ^= (corners::pm << offset::blocks::bitfield); // transform left stair into outside corner stair
						setBlockAt(right, pos.x, pos.y + 1, pos.z, false);
					}
				}
			}
			break ;
		case face_dir::minus_y:
			type |= ((corners::mp | corners::pp) << offset::blocks::bitfield);
			behind = getBlockAt(pos.x, pos.y + 1, pos.z);
			if ((behind & (mask::blocks::type | mask::stairs::top)) == (type & (mask::blocks::type | mask::stairs::top))) {
				if (((behind >> offset::blocks::orientation) & 0x7) == face_dir::minus_x) {
					type ^= (corners::mp << offset::blocks::bitfield); // outside corner
					goto SKIP_FRONT_minus_y;
				} else if (((behind >> offset::blocks::orientation) & 0x7) == face_dir::plus_x) {
					type ^= (corners::pp << offset::blocks::bitfield);
					goto SKIP_FRONT_minus_y;
				}
			}
			front = getBlockAt(pos.x, pos.y - 1, pos.z);
			if ((front & (mask::blocks::type | mask::stairs::top)) == (type & (mask::blocks::type | mask::stairs::top))) {
				if (((front >> offset::blocks::orientation) & 0x7) == face_dir::minus_x) {
					type |= (corners::pm << offset::blocks::bitfield); // inside corner
				} else if (((front >> offset::blocks::orientation) & 0x7) == face_dir::plus_x) {
					type |= (corners::mm << offset::blocks::bitfield);
				}
			}
			SKIP_FRONT_minus_y:
			left = getBlockAt(pos.x - 1, pos.y, pos.z);
			if ((left & (mask::blocks::type | mask::stairs::top)) == (type & (mask::blocks::type | mask::stairs::top))) {
				if (((left >> offset::blocks::orientation) & 0x7) == face_dir::minus_x) {
					if (((left >> offset::blocks::bitfield) & 0xF) == (corners::pm | corners::pp)) {
						left ^= (corners::pm << offset::blocks::bitfield); // transform left stair into outside corner stair
						setBlockAt(left, pos.x - 1, pos.y, pos.z, false);
					}
				} else if (((left >> offset::blocks::orientation) & 0x7) == face_dir::plus_x) {
					if (((left >> offset::blocks::bitfield) & 0xF) == (corners::mm | corners::mp)) {
						left |= (corners::pp << offset::blocks::bitfield); // transform left stair into inside corner stair
						setBlockAt(left, pos.x - 1, pos.y, pos.z, false);
					}
				}
			}
			right = getBlockAt(pos.x + 1, pos.y, pos.z);
			if ((right & (mask::blocks::type | mask::stairs::top)) == (type & (mask::blocks::type | mask::stairs::top))) {
				if (((right >> offset::blocks::orientation) & 0x7) == face_dir::minus_x) {
					if (((right >> offset::blocks::bitfield) & 0xF) == (corners::pm | corners::pp)) {
						right |= (corners::mp << offset::blocks::bitfield); // transform right stair into inside corner stair
						setBlockAt(right, pos.x + 1, pos.y, pos.z, false);
					}
				} else if (((right >> offset::blocks::orientation) & 0x7) == face_dir::plus_x) {
					if (((right >> offset::blocks::bitfield) & 0xF) == (corners::mm | corners::mp)) {
						right ^= (corners::mm << offset::blocks::bitfield); // transform left stair into outside corner stair
						setBlockAt(right, pos.x + 1, pos.y, pos.z, false);
					}
				}
			}
			break ;
		case face_dir::plus_y:
			type |= ((corners::mm | corners::pm) << offset::blocks::bitfield);
			behind = getBlockAt(pos.x, pos.y - 1, pos.z);
			if ((behind & (mask::blocks::type | mask::stairs::top)) == (type & (mask::blocks::type | mask::stairs::top))) {
				if (((behind >> offset::blocks::orientation) & 0x7) == face_dir::minus_x) {
					type ^= (corners::mm << offset::blocks::bitfield); // outside corner
					goto SKIP_FRONT_plus_y;
				} else if (((behind >> offset::blocks::orientation) & 0x7) == face_dir::plus_x) {
					type ^= (corners::pm << offset::blocks::bitfield);
					goto SKIP_FRONT_plus_y;
				}
			}
			front = getBlockAt(pos.x, pos.y + 1, pos.z);
			if ((front & (mask::blocks::type | mask::stairs::top)) == (type & (mask::blocks::type | mask::stairs::top))) {
				if (((front >> offset::blocks::orientation) & 0x7) == face_dir::minus_x) {
					type |= (corners::pp << offset::blocks::bitfield); // inside corner
				} else if (((front >> offset::blocks::orientation) & 0x7) == face_dir::plus_x) {
					type |= (corners::mp << offset::blocks::bitfield);
				}
			}
			SKIP_FRONT_plus_y:
			left = getBlockAt(pos.x + 1, pos.y, pos.z);
			if ((left & (mask::blocks::type | mask::stairs::top)) == (type & (mask::blocks::type | mask::stairs::top))) {
				if (((left >> offset::blocks::orientation) & 0x7) == face_dir::minus_x) {
					if (((left >> offset::blocks::bitfield) & 0xF) == (corners::pm | corners::pp)) {
						left |= (corners::mm << offset::blocks::bitfield); // transform left stair into inside corner stair
						setBlockAt(left, pos.x + 1, pos.y, pos.z, false);
					}
				} else if (((left >> offset::blocks::orientation) & 0x7) == face_dir::plus_x) {
					if (((left >> offset::blocks::bitfield) & 0xF) == (corners::mm | corners::mp)) {
						left ^= (corners::mp << offset::blocks::bitfield); // transform left stair into outside corner stair
						setBlockAt(left, pos.x + 1, pos.y, pos.z, false);
					}
				}
			}
			right = getBlockAt(pos.x - 1, pos.y, pos.z);
			if ((right & (mask::blocks::type | mask::stairs::top)) == (type & (mask::blocks::type | mask::stairs::top))) {
				if (((right >> offset::blocks::orientation) & 0x7) == face_dir::minus_x) {
					if (((right >> offset::blocks::bitfield) & 0xF) == (corners::pm | corners::pp)) {
						right ^= (corners::pp << offset::blocks::bitfield); // transform left stair into outside corner stair
						setBlockAt(right, pos.x - 1, pos.y, pos.z, false);
					}
				} else if (((right >> offset::blocks::orientation) & 0x7) == face_dir::plus_x) {
					if (((right >> offset::blocks::bitfield) & 0xF) == (corners::mm | corners::mp)) {
						right |= (corners::pm << offset::blocks::bitfield); // transform right stair into inside corner stair
						setBlockAt(right, pos.x - 1, pos.y, pos.z, false);
					}
				}
			}
			break ;
	}
}

void Chunk::handle_door_placement( glm::ivec3 pos, int &type )
{
	int below = getBlockAt(pos.x, pos.y, pos.z - 1, false);
	if (!s_blocks[below & mask::blocks::type]->hasCollisionHitbox_1x1x1) {
		type = blocks::air;
		return ;
	}
	int above = getBlockAt(pos.x, pos.y, pos.z + 1, false);
	if (above != blocks::air) {
		type = blocks::air;
		return ;
	}
	// TODO door::right_hinge
}

// used by glass pane too
void Chunk::handle_fence_placement( glm::ivec3 pos, int &type )
{
	int next = getBlockAt(pos.x - 1, pos.y, pos.z);
	if (s_blocks[next & mask::blocks::type]->hasCollisionHitbox_1x1x1 || (next & mask::blocks::type) == (type & mask::blocks::type)) {
		type |= (fence::mx << offset::blocks::bitfield);
	}
	next = getBlockAt(pos.x + 1, pos.y, pos.z);
	if (s_blocks[next & mask::blocks::type]->hasCollisionHitbox_1x1x1 || (next & mask::blocks::type) == (type & mask::blocks::type)) {
		type |= (fence::px << offset::blocks::bitfield);
	}
	next = getBlockAt(pos.x, pos.y - 1, pos.z);
	if (s_blocks[next & mask::blocks::type]->hasCollisionHitbox_1x1x1 || (next & mask::blocks::type) == (type & mask::blocks::type)) {
		type |= (fence::my << offset::blocks::bitfield);
	}
	next = getBlockAt(pos.x, pos.y + 1, pos.z);
	if (s_blocks[next & mask::blocks::type]->hasCollisionHitbox_1x1x1 || (next & mask::blocks::type) == (type & mask::blocks::type)) {
		type |= (fence::py << offset::blocks::bitfield);
	}
}
