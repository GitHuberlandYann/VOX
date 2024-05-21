#include "Camera.hpp"
#include "random.hpp"

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

/* generate one or many entities depending on block broken */
void Chunk::entity_block( int posX, int posY, int posZ, int type )
{
	// std::cout << "breaking " << s_blocks[type]->name << std::endl;
	float random;
	switch (type) {
	case blocks::GRASS:
		random = Random::randomFloat(_seed);
		if (random <= 0.125f) {
			_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, {blocks::WHEAT_SEEDS, 1, {0, 0}}));
		}
		break ;
	case blocks::OAK_LEAVES:
		random = Random::randomFloat(_seed);
		if (random <= 0.05f) {
			_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, {blocks::OAK_SAPLING, 1, {0, 0}}));
		}
		random = Random::randomFloat(_seed);
		if (random <= 0.02f) {
			_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, {blocks::STICK, 2, {0, 0}}));
		}
		random = Random::randomFloat(_seed);
		if (random <= 0.005f) {
			_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, {blocks::APPLE, 1, {0, 0}}));
		}
		break ;
	case blocks::WHEAT_CROP7:
		_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, {blocks::WHEAT, 1, {0, 0}}));
		random = Random::randomFloat(_seed);
		if (random <= 0.0787f) {
			_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, {blocks::WHEAT_SEEDS, 1, {0, 0}}));
		} else if (random <= 0.0787f + 0.3149f) {
			_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, {blocks::WHEAT_SEEDS, 2, {0, 0}}));
		} else if (random <= 0.0787f + 0.3149f + 0.4198f) {
			_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, {blocks::WHEAT_SEEDS, 3, {0, 0}}));
		} else {
			_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, {blocks::WHEAT_SEEDS, 4, {0, 0}}));
		}
		break ;
	case blocks::GRAVEL:
		random = Random::randomFloat(_seed);
		if (random < 0.1f) {
			_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, {blocks::FLINT, 1, {0, 0}}));
			return ;
		} // no break on gravel so it drops a gravel block
	default:
		_entities.push_back(new Entity(this, _inventory, {posX + _startX + 0.5f, posY + _startY + 0.5f, posZ + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, {s_blocks[type]->mined, 1, {0, 0}}));
	}
}

void Chunk::remove_block( bool useInventory, glm::ivec3 pos )
{
	// std::cout << "in chunk " << _startX << ", " << _startY << ":rm block " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
	// std::cout << "nb displayed blocks before: " << _displayed_blocks << std::endl;
	int offset = (((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z;
	int value = _blocks[offset], type = value & 0xFF; // TODO better handling of invis block gets deleted
	_added.erase(offset);
	_removed.insert(offset);
	if (type == blocks::CHEST) {
		auto search = _chests.find(offset);
		if (search != _chests.end()) {
			t_item *item;
			for (int index = 0; index < 27; ++index) {
				item = search->second->getItem(index);
				if (item->amount) {
					_entities.push_back(new Entity(this, _inventory, {pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f}, {glm::normalize(glm::vec2(Random::randomFloat(_seed) * 2 - 1, Random::randomFloat(_seed) * 2 - 1)), 1.0f}, false, *item));
				}
			}
		}
		delete _chests[offset]; // TODO what happens if chest explodes while we use it ? prob heap use after free in Menu.cpp
		_chests.erase(offset);
	} else if (type == blocks::FURNACE) {
		auto search = _furnaces.find(offset); // drop furnace's items
		if (search != _furnaces.end()) {
			t_item items = search->second->getComposant();
			if (items.amount) {
				_entities.push_back(new Entity(this, _inventory, {pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f}, {glm::normalize(glm::vec2(-1, -1)), 1.0f}, false, items));
			}
			items = search->second->getFuel();
			if (items.amount) {
				_entities.push_back(new Entity(this, _inventory, {pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f}, {glm::normalize(glm::vec2(1, 9)), 1.0f}, false, items));
			}
			items = search->second->getProduction();
			if (items.amount) {
				_entities.push_back(new Entity(this, _inventory, {pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 0.5f}, {glm::normalize(glm::vec2(-4, 5)), 1.0f}, false, items));
			}
		}
		delete _furnaces[offset];
		_furnaces.erase(offset);
	} else if (type == blocks::OAK_DOOR) {
		// break other part of door
		_blocks[offset + (((value >> 12) & DOOR::UPPER_HALF) ? -1 : 1)] = blocks::AIR;
		_added.erase(offset + (((value >> 12) & DOOR::UPPER_HALF) ? -1 : 1));
		_removed.insert(offset + (((value >> 12) & DOOR::UPPER_HALF) ? -1 : 1));
	}
	if (useInventory && type == blocks::WATER) {
		_inventory->addBlock(type); // water bucket
	} else if (useInventory) {
		entity_block(pos.x, pos.y, pos.z, type);
	}
	int type_above = blocks::AIR;
	if (pos.z < 255) {
		type_above = _blocks[offset + 1] & 0xFF;
		if (type_above == blocks::SAND || type_above == blocks::GRAVEL) { // sand falls if block underneath deleted
			// std::cout << "new scheduled_to_fall" << std::endl;
			_scheduled_to_fall.push_back(offset + 1);
		}
	}
	_blocks[offset] = blocks::AIR;
	if (type == blocks::GLASS) { // TODO check neighbours and spread water
		return ;
	} else if (type > blocks::AIR && type < blocks::WATER) {
		if (s_blocks[type]->light_level) {
			std::cout << "rm light" << std::endl;
			if (type == blocks::TORCH) {
				delete _flames[offset];
				_flames.erase(offset);
			} else if (type == blocks::REDSTONE_TORCH) {
				updateRedstoneTorch(pos, value);
			}
			_lights[offset] &= 0xFF00;
			light_spread(pos.x, pos.y, pos.z, false); // spread block light
			_light_update = false;
			std::cout << "over" << std::endl;
		} else if (type == blocks::OAK_LEAVES) {
			_lights[offset] &= 0xFF;
		} else if (type == blocks::LEVER) {
			flickLever(pos, value, false);
		} else if (type == blocks::REDSTONE_DUST) {
			updateRedstoneDust(pos);
		} else if (value & REDSTONE::POWERED) { // we remove a strongly powered block
			weaklyPower(pos, {0, 0, 0}, REDSTONE::OFF, false);
		} else if (value & REDSTONE::WEAKDY_POWERED) { // we remove a weakdy powered block
			weaklyPower(pos, {0, 0, 0}, REDSTONE::OFF, true);
		}
	} else if (type == blocks::WATER) { // use bucket on water source
		// std::cout << "bucket on water" << std::endl;
		_fluids.insert(offset);
		return ;
	}
	for (int i = 0; i < 15; ++i) {
		_particles.push_back(new Particle(this, {pos.x + _startX + Random::randomFloat(_seed), pos.y + _startY + Random::randomFloat(_seed), pos.z + Random::randomFloat(_seed)}, PARTICLES::BREAKING, 0, type));
	}
	if (air_flower(type, false, false, true)) {
		light_spread(pos.x, pos.y, pos.z, true); // spread sky light
		light_spread(pos.x, pos.y, pos.z, false); // spread block light
		for (int index = 0; index < 6; index++) {
			const glm::ivec3 delta = adj_blocks[index];
			if (pos.x + delta.x < 0 || pos.x + delta.x >= CHUNK_SIZE || pos.y + delta.y < 0 || pos.y + delta.y >= CHUNK_SIZE || pos.z + delta.z < 0 || pos.z + delta.z > 255) {
			} else {
				int adj_offset = ((((pos.x + delta.x) << CHUNK_SHIFT) + pos.y + delta.y) << WORLD_SHIFT) + pos.z + delta.z;
				int adj = _blocks[adj_offset];
				if (adj & blocks::NOTVISIBLE) {
					_blocks[adj_offset] = adj - blocks::NOTVISIBLE;
					_added[adj_offset] = adj - blocks::NOTVISIBLE;
				} else if (air_flower(adj, false, false, false)) {
				} else if (index != face_dir::MINUSZ && (adj & 0xFF) >= blocks::WATER) { // if water under, it is not updated
					_fluids.insert(adj_offset);
				} else if (((adj & 0xFF) == blocks::TORCH || (adj & 0xFF) == blocks::REDSTONE_TORCH) && ((adj >> 9) & 0x7) == opposite_dir(index)) {
					std::cout << "rm adj torch" << std::endl;
					remove_block(useInventory, {pos.x + delta.x, pos.y + delta.y, pos.z + delta.z});
				}
				if ((adj & 0xFF) == blocks::OAK_FENCE || (adj & 0xFF) == blocks::GLASS_PANE) {
					switch (index) {
						case face_dir::MINUSX:
							_blocks[((((pos.x + delta.x) << CHUNK_SHIFT) + pos.y + delta.y) << WORLD_SHIFT) + pos.z + delta.z] ^= (FENCE::PX << 12);
							_added[((((pos.x + delta.x) << CHUNK_SHIFT) + pos.y + delta.y) << WORLD_SHIFT) + pos.z + delta.z] ^= (FENCE::PX << 12);
							break ;
						case face_dir::PLUSX:
							_blocks[((((pos.x + delta.x) << CHUNK_SHIFT) + pos.y + delta.y) << WORLD_SHIFT) + pos.z + delta.z] ^= (FENCE::MX << 12);
							_added[((((pos.x + delta.x) << CHUNK_SHIFT) + pos.y + delta.y) << WORLD_SHIFT) + pos.z + delta.z] ^= (FENCE::MX << 12);
							break ;
						case face_dir::MINUSY:
							_blocks[((((pos.x + delta.x) << CHUNK_SHIFT) + pos.y + delta.y) << WORLD_SHIFT) + pos.z + delta.z] ^= (FENCE::PY << 12);
							_added[((((pos.x + delta.x) << CHUNK_SHIFT) + pos.y + delta.y) << WORLD_SHIFT) + pos.z + delta.z] ^= (FENCE::PY << 12);
							break ;
						case face_dir::PLUSY:
							_blocks[((((pos.x + delta.x) << CHUNK_SHIFT) + pos.y + delta.y) << WORLD_SHIFT) + pos.z + delta.z] ^= (FENCE::MY << 12);
							_added[((((pos.x + delta.x) << CHUNK_SHIFT) + pos.y + delta.y) << WORLD_SHIFT) + pos.z + delta.z] ^= (FENCE::MY << 12);
							break ;
					}
				}
			}
		}
	}
	handle_border_block(pos, air_flower(value, true, false, true), false); // if block at border of chunk gets deleted, we update neighbours. doing it after light spread
	if (type_above != blocks::TORCH && type_above != blocks::REDSTONE_TORCH && type_above != blocks::CHEST && ((type_above >= blocks::POPPY && type_above < blocks::WATER) || type_above == blocks::CACTUS
		|| (type_above == blocks::OAK_DOOR && type != blocks::OAK_DOOR))) { // del flower if block underneath deleted
		(type_above == blocks::GRASS)
			? remove_block(false, {pos.x, pos.y, pos.z + 1}) // if grass above breaked block, don't collect it
			: remove_block(useInventory, {pos.x, pos.y, pos.z + 1});
	}
	// std::cout << "nb displayed blocks after: " << _displayed_blocks << std::endl;
}

// takes stair_bottom at given pos and check if it should be corner stair
void Chunk::handle_stair_corners( glm::ivec3 pos, int &type )
{
	int behind, front, left, right;
	switch ((type >> 9) & 0x7) {
		case face_dir::MINUSX:
			type |= ((CORNERS::PM | CORNERS::PP) << 12);
			behind = getBlockAt(pos.x + 1, pos.y, pos.z, true);
			if ((behind & 0xFF) == (type & 0xFF)) {
				if (((behind >> 9) & 0x7) == face_dir::MINUSY) {
					type ^= (CORNERS::PM << 12); // outside corner
					goto SKIP_FRONT_MINUSX;
				} else if (((behind >> 9) & 0x7) == face_dir::PLUSY) {
					type ^= (CORNERS::PP << 12);
					goto SKIP_FRONT_MINUSX;
				}
			}
			front = getBlockAt(pos.x - 1, pos.y, pos.z, true);
			if ((front & 0xFF) == (type & 0xFF)) {
				if (((front >> 9) & 0x7) == face_dir::MINUSY) {
					type |= (CORNERS::MP << 12); // inside corner
				} else if (((front >> 9) & 0x7) == face_dir::PLUSY) {
					type |= (CORNERS::MM << 12);
				}
			}
			SKIP_FRONT_MINUSX:
			left = getBlockAt(pos.x, pos.y + 1, pos.z, true);
			if ((left & 0xFF) == (type & 0xFF)) {
				if (((left >> 9) & 0x7) == face_dir::MINUSY) {
					if (((left >> 12) & 0xF) == (CORNERS::MP | CORNERS::PP)) {
						left |= (CORNERS::PM << 12); // transform left stair into inside corner stair
						setBlockAt(left, pos.x, pos.y + 1, pos.z, true);
					}
				} else if (((left >> 9) & 0x7) == face_dir::PLUSY) {
					if (((left >> 12) & 0xF) == (CORNERS::MM | CORNERS::PM)) {
						left ^= (CORNERS::MM << 12); // transform left stair into outside corner stair
						setBlockAt(left, pos.x, pos.y + 1, pos.z, true);
					}
				}
			}
			right = getBlockAt(pos.x, pos.y - 1, pos.z, true);
			if ((right & 0xFF) == (type & 0xFF)) {
				if (((right >> 9) & 0x7) == face_dir::MINUSY) {
					if (((right >> 12) & 0xF) == (CORNERS::MP | CORNERS::PP)) {
						right ^= (CORNERS::MP << 12); // transform left stair into outside corner stair
						setBlockAt(right, pos.x, pos.y - 1, pos.z, true);
					}
				} else if (((right >> 9) & 0x7) == face_dir::PLUSY) {
					if (((right >> 12) & 0xF) == (CORNERS::MM | CORNERS::PM)) {
						right |= (CORNERS::PP << 12); // transform right stair into inside corner stair
						setBlockAt(right, pos.x, pos.y - 1, pos.z, true);
					}
				}
			}
			break ;
		case face_dir::PLUSX:
			type |= ((CORNERS::MM | CORNERS::MP) << 12);
			behind = getBlockAt(pos.x - 1, pos.y, pos.z, true);
			if ((behind & 0xFF) == (type & 0xFF)) {
				if (((behind >> 9) & 0x7) == face_dir::MINUSY) {
					type ^= (CORNERS::MM << 12); // outside corner
					goto SKIP_FRONT_PLUSX;
				} else if (((behind >> 9) & 0x7) == face_dir::PLUSY) {
					type ^= (CORNERS::MP << 12);
					goto SKIP_FRONT_PLUSX;
				}
			}
			front = getBlockAt(pos.x + 1, pos.y, pos.z, true);
			if ((front & 0xFF) == (type & 0xFF)) {
				if (((front >> 9) & 0x7) == face_dir::MINUSY) {
					type |= (CORNERS::PP << 12); // inside corner
				} else if (((front >> 9) & 0x7) == face_dir::PLUSY) {
					type |= (CORNERS::PM << 12);
				}
			}
			SKIP_FRONT_PLUSX:
			left = getBlockAt(pos.x, pos.y - 1, pos.z, true);
			if ((left & 0xFF) == (type & 0xFF)) {
				if (((left >> 9) & 0x7) == face_dir::MINUSY) {
					if (((left >> 12) & 0xF) == (CORNERS::MP | CORNERS::PP)) {
						left ^= (CORNERS::PP << 12); // transform left stair into outside corner stair
						setBlockAt(left, pos.x, pos.y - 1, pos.z, true);
					}
				} else if (((left >> 9) & 0x7) == face_dir::PLUSY) {
					if (((left >> 12) & 0xF) == (CORNERS::MM | CORNERS::PM)) {
						left |= (CORNERS::MP << 12); // transform left stair into inside corner stair
						setBlockAt(left, pos.x, pos.y - 1, pos.z, true);
					}
				}
			}
			right = getBlockAt(pos.x, pos.y + 1, pos.z, true);
			if ((right & 0xFF) == (type & 0xFF)) {
				if (((right >> 9) & 0x7) == face_dir::MINUSY) {
					if (((right >> 12) & 0xF) == (CORNERS::MP | CORNERS::PP)) {
						right |= (CORNERS::MM << 12); // transform right stair into inside corner stair
						setBlockAt(right, pos.x, pos.y + 1, pos.z, true);
					}
				} else if (((right >> 9) & 0x7) == face_dir::PLUSY) {
					if (((right >> 12) & 0xF) == (CORNERS::MM | CORNERS::PM)) {
						right ^= (CORNERS::PM << 12); // transform left stair into outside corner stair
						setBlockAt(right, pos.x, pos.y + 1, pos.z, true);
					}
				}
			}
			break ;
		case face_dir::MINUSY:
			type |= ((CORNERS::MP | CORNERS::PP) << 12);
			behind = getBlockAt(pos.x, pos.y + 1, pos.z, true);
			if ((behind & 0xFF) == (type & 0xFF)) {
				if (((behind >> 9) & 0x7) == face_dir::MINUSX) {
					type ^= (CORNERS::MP << 12); // outside corner
					goto SKIP_FRONT_MINUSY;
				} else if (((behind >> 9) & 0x7) == face_dir::PLUSX) {
					type ^= (CORNERS::PP << 12);
					goto SKIP_FRONT_MINUSY;
				}
			}
			front = getBlockAt(pos.x, pos.y - 1, pos.z, true);
			if ((front & 0xFF) == (type & 0xFF)) {
				if (((front >> 9) & 0x7) == face_dir::MINUSX) {
					type |= (CORNERS::PM << 12); // inside corner
				} else if (((front >> 9) & 0x7) == face_dir::PLUSX) {
					type |= (CORNERS::MM << 12);
				}
			}
			SKIP_FRONT_MINUSY:
			left = getBlockAt(pos.x - 1, pos.y, pos.z, true);
			if ((left & 0xFF) == (type & 0xFF)) {
				if (((left >> 9) & 0x7) == face_dir::MINUSX) {
					if (((left >> 12) & 0xF) == (CORNERS::PM | CORNERS::PP)) {
						left ^= (CORNERS::PM << 12); // transform left stair into outside corner stair
						setBlockAt(left, pos.x - 1, pos.y, pos.z, true);
					}
				} else if (((left >> 9) & 0x7) == face_dir::PLUSX) {
					if (((left >> 12) & 0xF) == (CORNERS::MM | CORNERS::MP)) {
						left |= (CORNERS::PP << 12); // transform left stair into inside corner stair
						setBlockAt(left, pos.x - 1, pos.y, pos.z, true);
					}
				}
			}
			right = getBlockAt(pos.x + 1, pos.y, pos.z, true);
			if ((right & 0xFF) == (type & 0xFF)) {
				if (((right >> 9) & 0x7) == face_dir::MINUSX) {
					if (((right >> 12) & 0xF) == (CORNERS::PM | CORNERS::PP)) {
						right |= (CORNERS::MP << 12); // transform right stair into inside corner stair
						setBlockAt(right, pos.x + 1, pos.y, pos.z, true);
					}
				} else if (((right >> 9) & 0x7) == face_dir::PLUSX) {
					if (((right >> 12) & 0xF) == (CORNERS::MM | CORNERS::MP)) {
						right ^= (CORNERS::MM << 12); // transform left stair into outside corner stair
						setBlockAt(right, pos.x + 1, pos.y, pos.z, true);
					}
				}
			}
			break ;
		case face_dir::PLUSY:
			type |= ((CORNERS::MM | CORNERS::PM) << 12);
			behind = getBlockAt(pos.x, pos.y - 1, pos.z, true);
			if ((behind & 0xFF) == (type & 0xFF)) {
				if (((behind >> 9) & 0x7) == face_dir::MINUSX) {
					type ^= (CORNERS::MM << 12); // outside corner
					goto SKIP_FRONT_PLUSY;
				} else if (((behind >> 9) & 0x7) == face_dir::PLUSX) {
					type ^= (CORNERS::PM << 12);
					goto SKIP_FRONT_PLUSY;
				}
			}
			front = getBlockAt(pos.x, pos.y + 1, pos.z, true);
			if ((front & 0xFF) == (type & 0xFF)) {
				if (((front >> 9) & 0x7) == face_dir::MINUSX) {
					type |= (CORNERS::PP << 12); // inside corner
				} else if (((front >> 9) & 0x7) == face_dir::PLUSX) {
					type |= (CORNERS::MP << 12);
				}
			}
			SKIP_FRONT_PLUSY:
			left = getBlockAt(pos.x + 1, pos.y, pos.z, true);
			if ((left & 0xFF) == (type & 0xFF)) {
				if (((left >> 9) & 0x7) == face_dir::MINUSX) {
					if (((left >> 12) & 0xF) == (CORNERS::PM | CORNERS::PP)) {
						left |= (CORNERS::MM << 12); // transform left stair into inside corner stair
						setBlockAt(left, pos.x + 1, pos.y, pos.z, true);
					}
				} else if (((left >> 9) & 0x7) == face_dir::PLUSX) {
					if (((left >> 12) & 0xF) == (CORNERS::MM | CORNERS::MP)) {
						left ^= (CORNERS::MP << 12); // transform left stair into outside corner stair
						setBlockAt(left, pos.x + 1, pos.y, pos.z, true);
					}
				}
			}
			right = getBlockAt(pos.x - 1, pos.y, pos.z, true);
			if ((right & 0xFF) == (type & 0xFF)) {
				if (((right >> 9) & 0x7) == face_dir::MINUSX) {
					if (((right >> 12) & 0xF) == (CORNERS::PM | CORNERS::PP)) {
						right ^= (CORNERS::PP << 12); // transform left stair into outside corner stair
						setBlockAt(right, pos.x - 1, pos.y, pos.z, true);
					}
				} else if (((right >> 9) & 0x7) == face_dir::PLUSX) {
					if (((right >> 12) & 0xF) == (CORNERS::MM | CORNERS::MP)) {
						right |= (CORNERS::PM << 12); // transform right stair into inside corner stair
						setBlockAt(right, pos.x - 1, pos.y, pos.z, true);
					}
				}
			}
			break ;
	}
}

void Chunk::handle_door_placement( glm::ivec3 pos, int &type )
{
	int below = getBlockAt(pos.x, pos.y, pos.z - 1, false);
	if (!s_blocks[below & 0xFF]->collisionHitbox_1x1x1) {
		type = blocks::AIR;
		return ;
	}
	int above = getBlockAt(pos.x, pos.y, pos.z + 1, false);
	if (above != blocks::AIR) {
		type = blocks::AIR;
		return ;
	}
	// TODO DOOR::RIGHT_HINGE
}

// used by glass pane too
void Chunk::handle_fence_placement( glm::ivec3 pos, int &type )
{
	int next = getBlockAt(pos.x - 1, pos.y, pos.z, true);
	if (s_blocks[next & 0xFF]->collisionHitbox_1x1x1 || (next & 0xFF) == (type & 0xFF)) {
		type |= (FENCE::MX << 12);
	}
	next = getBlockAt(pos.x + 1, pos.y, pos.z, true);
	if (s_blocks[next & 0xFF]->collisionHitbox_1x1x1 || (next & 0xFF) == (type & 0xFF)) {
		type |= (FENCE::PX << 12);
	}
	next = getBlockAt(pos.x, pos.y - 1, pos.z, true);
	if (s_blocks[next & 0xFF]->collisionHitbox_1x1x1 || (next & 0xFF) == (type & 0xFF)) {
		type |= (FENCE::MY << 12);
	}
	next = getBlockAt(pos.x, pos.y + 1, pos.z, true);
	if (s_blocks[next & 0xFF]->collisionHitbox_1x1x1 || (next & 0xFF) == (type & 0xFF)) {
		type |= (FENCE::PY << 12);
	}
}

void Chunk::addFlame( int offset, glm::vec3 pos, int source, int orientation )
{
	if (source == blocks::TORCH) {
		switch (orientation) {
		case face_dir::MINUSX:
			_flames.emplace(offset, new Particle(this, {pos.x + _startX + 0.25f, pos.y + _startY + 0.5f, pos.z + 15.0f / 16.0f}, PARTICLES::FLAME));
			break ;
		case face_dir::PLUSX:
			_flames.emplace(offset, new Particle(this, {pos.x + _startX + 0.75f, pos.y + _startY + 0.5f, pos.z + 15.0f / 16.0f}, PARTICLES::FLAME));
			break ;
		case face_dir::MINUSY:
			_flames.emplace(offset, new Particle(this, {pos.x + _startX + 0.5f, pos.y + _startY + 0.25f, pos.z + 15.0f / 16.0f}, PARTICLES::FLAME));
			break ;
		case face_dir::PLUSY:
			_flames.emplace(offset, new Particle(this, {pos.x + _startX + 0.5f, pos.y + _startY + 0.75f, pos.z + 15.0f / 16.0f}, PARTICLES::FLAME));
			break ;
		case face_dir::MINUSZ:
			_flames.emplace(offset, new Particle(this, {pos.x + _startX + 0.5f, pos.y + _startY + 0.5f, pos.z + 11.0f / 16.0f + 0.05f}, PARTICLES::FLAME));
			break ;
		}
	}
}

void Chunk::add_block( bool useInventory, glm::ivec3 pos, int block_value, int previous )
{
	// std::cout << "in chunk " << _startX << ", " << _startY << ":add block " << _startX + pos.x << ", " << _startY + pos.y << ", " << pos.z << std::endl;
	// std::cout << "nb displayed blocks before: " << _displayed_blocks << std::endl;
	int offset = (((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z;
	int type = block_value & 0xFF;
	int shape = s_blocks[type]->geometry;
	if (type == blocks::SAND || type == blocks::GRAVEL) {
		int type_under = (_blocks[offset - 1] & 0xFF);
		if (type_under == blocks::AIR) {
			if (useInventory) {
				_inventory->removeBlock(false);
			}
			_entities.push_back(new FallingBlockEntity(this, {pos.x + _startX, pos.y + _startY, pos.z}, {type, 1, {0, 0}}));
			return ;
		}
	}
	if (type == blocks::WATER) { // we place water
		if (previous < blocks::WATER) {
			_hasWater = true;
		}
	} else if (shape == GEOMETRY::TORCH) {
		// check if orientation possible (wall available to hang from)
		// if not, check if block underneath and change orientation
		// else abort mission
		int neighbourShape = GEOMETRY::NONE;
		switch ((block_value >> 9) & 0x7) {
		case face_dir::PLUSX:
			neighbourShape = s_blocks[getBlockAt(pos.x + 1, pos.y, pos.z, true) & 0xFF]->geometry;
			break;
		case face_dir::MINUSX:
			neighbourShape = s_blocks[getBlockAt(pos.x - 1, pos.y, pos.z, true) & 0xFF]->geometry;
			break;
		case face_dir::PLUSY:
			neighbourShape = s_blocks[getBlockAt(pos.x, pos.y + 1, pos.z, true) & 0xFF]->geometry;
			break;
		case face_dir::MINUSY:
			neighbourShape = s_blocks[getBlockAt(pos.x, pos.y - 1, pos.z, true) & 0xFF]->geometry;
			break;
		}
		if (neighbourShape != GEOMETRY::CUBE) {
			block_value = type + (face_dir::MINUSZ << 9);
			neighbourShape = s_blocks[getBlockAt(pos.x, pos.y, pos.z - 1, false) & 0xFF]->geometry;
			if (!(neighbourShape == GEOMETRY::CUBE || neighbourShape == GEOMETRY::SLAB_TOP
				|| neighbourShape == GEOMETRY::STAIRS_TOP || neighbourShape == GEOMETRY::FENCE)) {
				return ;
			}
		}
		if (type == blocks::TORCH) {
			addFlame(offset, pos, blocks::TORCH, (block_value >> 9) & 0x7);
		}
	} else if (shape == GEOMETRY::PISTON) {
		block_value += (_camera->getOrientation6() << 9);
	} else if (s_blocks[type]->oriented) {
		if ((type != blocks::LEVER && shape != GEOMETRY::BUTTON) || ((block_value >> 12) & 0x3) != PLACEMENT::WALL) {
			block_value += (_camera->getOrientation() << 9);
		}
		switch (shape) {
			case GEOMETRY::STAIRS_BOTTOM:
			case GEOMETRY::STAIRS_TOP:
				handle_stair_corners(pos, block_value);
				break ;
			case GEOMETRY::DOOR:
				handle_door_placement(pos, block_value);
				if (block_value == blocks::AIR) return ; // door couldn't be placed
				else if (!((block_value >> 12) & DOOR::UPPER_HALF)) { // place upper_half
					_blocks[offset + 1] = block_value | (DOOR::UPPER_HALF << 12);
					_removed.erase(offset + 1);
					_added[offset + 1] = block_value | (DOOR::UPPER_HALF << 12);
				}
				break ;
			case GEOMETRY::REPEATER:
				previous = getBlockAt(pos.x, pos.y, pos.z - 1, false);
				if (s_blocks[previous & 0xFF]->transparent && (previous & 0xFF) != blocks::GLASS) {
					return ;
				}
				block_value ^= (1 << 9); // inverse dir (0->1, 1->0, 2->3, 3->2)
				initRepeater(pos, block_value, true);
				break ;
		}
	} else if (type == blocks::OAK_FENCE || type == blocks::GLASS_PANE) {
		handle_fence_placement(pos, block_value);
	} else if ((shape == GEOMETRY::CROSS || shape == GEOMETRY::CROP) && pos.z > 0) {
		if (previous >= blocks::WATER) {
			return ;
		}
		int type_below = _blocks[offset - 1] & 0xFF;
		if (type == blocks::WHEAT_CROP) {
		} else if (type_below != blocks::GRASS_BLOCK && type_below != blocks::DIRT && type_below != blocks::SAND) {
			return ; // can't place flower on something else than grass/dirt block
		} else if (!(type_below > blocks::AIR && type_below < blocks::POPPY) || s_blocks[type_below]->hasHitbox) {
			return ;
		}
	} else if (shape == GEOMETRY::DUST) {
		int shape_below = s_blocks[_blocks[offset - 1] & 0xFF]->geometry;
		if (!(shape_below == GEOMETRY::CUBE || shape_below == GEOMETRY::SLAB_TOP || shape_below == GEOMETRY::PISTON
			|| shape_below == GEOMETRY::STAIRS_TOP || shape_below == GEOMETRY::GLASS)) {
			return ;
		}
		_blocks[offset] = block_value; // place it down temporarily to be used in connectRedstoneDust
		connectRedstoneDust(pos, block_value, true);
	} else if (previous >= blocks::WATER) { // replace water block with something else
		_fluids.insert(offset);
	}
	if (useInventory) {
		_inventory->removeBlock(false);
	}

	_blocks[offset] = block_value;
	_removed.erase(offset);
	_added[offset] = block_value;
	if (type == blocks::WATER) {
		_fluids.insert(offset);
		// std::cout << "adding water" << std::endl;
		return ;
	}
	if (type == blocks::CHEST) {
		_chests.emplace(offset, new ChestInstance(this, {pos.x + _startX, pos.y + _startY, pos.z}, _camera->getOrientation()));
	} else if (type == blocks::FURNACE) {
		_furnaces.emplace(offset, new FurnaceInstance());
	} else if (type == blocks::TORCH || type == blocks::REDSTONE_TORCH) {
		if (type == blocks::REDSTONE_TORCH) {
			updateRedstoneTorch(pos, block_value);
		}
		std::cout << "add light" << std::endl;
		_lights[offset] &= 0xFF00;
		_lights[offset] += s_blocks[type]->light_level + (s_blocks[type]->light_level << 4);
		light_spread(pos.x, pos.y, pos.z, false);
		handle_border_block(pos, type, true); // torch is special case flower
		_light_update = false;
		std::cout << "over" << std::endl;
	} else if (type == blocks::GLASS) {
		_hasWater = true; // glass considered as invis block
	} else if (type == blocks::REDSTONE_DUST) {
		updateRedstoneDust(pos);
	} else if (s_blocks[type]->redstoneComponant) {
		int active = getRedstoneStrength(pos, {0, 0, 0}, REDSTONE::OFF, true);
		if (active) {
			block_value |= REDSTONE::ACTIVATED;
			int powered = getRedstoneStrength(pos, {0, 0, 0}, REDSTONE::OFF, false);
			if (powered) {
				block_value |= REDSTONE::POWERED;
			}
			_blocks[offset] = block_value;
			_added[offset] = block_value;
			if (type == blocks::REDSTONE_LAMP) {
				_lights[offset] &= 0xFF00;
				_lights[offset] += s_blocks[blocks::REDSTONE_LAMP]->light_level + (s_blocks[blocks::REDSTONE_LAMP]->light_level << 4);
				light_spread(pos.x, pos.y, pos.z, false);
				_light_update = false;
			}
		}
	}
	if (!air_flower(type, true, true, false)) {
		return ;
	}
	if (!s_blocks[type]->transparent && type != blocks::REDSTONE_LAMP) {
		if (type == blocks::REDSTONE_BLOCK) {
			block_value |= REDSTONE::POWERED;
			_blocks[offset] = block_value;
			_added[offset] = block_value;
			weaklyPower(pos, {0, 0, 0}, REDSTONE::ON, false);
		} else {
			stronglyPower(pos, {0, 0, 0}, REDSTONE::OFF);
		}
		_lights[offset] = 0; // rm light if solid block added
	}
	for (int index = 0; index < 6; index++) {
		const glm::ivec3 delta = adj_blocks[index];
		if (pos.z + delta.z < 0 || pos.z + delta.z > 255) {
			continue ;
		}
		GLint adj = getBlockAt(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z, true);
		if (s_blocks[adj & 0xFF]->transparent) { // update light status around placed block
			if (index != face_dir::PLUSZ) {
				light_try_spread(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z, 1, true, LIGHT_RECURSE); // spread sky light, but not upwards duh
			}
			light_try_spread(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z, 1, false, LIGHT_RECURSE);
		} else if (!face_count(adj & 0xFF, pos.x + delta.x, pos.y + delta.y, pos.z + delta.z)) {
			// was exposed before, but isn't anymore
			setBlockAt(adj | blocks::NOTVISIBLE, pos.x + delta.x, pos.y + delta.y, pos.z + delta.z, true);
		}
		if ((adj & 0xFF) == blocks::OAK_FENCE || (adj & 0xFF) == blocks::GLASS_PANE) {
			switch (index) {
				case face_dir::MINUSX:
					setBlockAt(adj | (FENCE::PX << 12), pos.x + delta.x, pos.y + delta.y, pos.z + delta.z, true);
					break ;
				case face_dir::PLUSX:
					setBlockAt(adj | (FENCE::MX << 12), pos.x + delta.x, pos.y + delta.y, pos.z + delta.z, true);
					break ;
				case face_dir::MINUSY:
					setBlockAt(adj | (FENCE::PY << 12), pos.x + delta.x, pos.y + delta.y, pos.z + delta.z, true);
					break ;
				case face_dir::PLUSY:
					setBlockAt(adj | (FENCE::MY << 12), pos.x + delta.x, pos.y + delta.y, pos.z + delta.z, true);
					break ;
			}
		}
		if ((adj & 0xFF) == blocks::REDSTONE_DUST && (delta.z == -1 || type == blocks::TARGET)) {
			connectRedstoneDust(pos + delta, adj, false);
			setBlockAt(adj, pos.x + delta.x, pos.y + delta.y, pos.z + delta.z, true);
		}
	}
}

void Chunk::replace_block( bool useInventory, glm::ivec3 pos, int type, int previous )
{
	// std::cout << "replace_block " << useInventory << std::endl;
	if (useInventory) {
		_inventory->decrementDurabitilty();
	}
	int offset = (((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z;
	_blocks[offset] = blocks::AIR;
	handle_border_block(pos, previous, false);
	// std::cout << "\t_displayed_faces after rm " << _displayed_faces << std::endl;
	_blocks[offset] = type;
	_added[offset] = type;
	light_spread(pos.x, pos.y, pos.z, false);
	light_spread(pos.x, pos.y, pos.z, true);
	_light_update = false;
	for (int index = 0; index < 6; index++) {
		const glm::ivec3 delta = adj_blocks[index];
		if (pos.x + delta.x < 0 || pos.x + delta.x >= CHUNK_SIZE || pos.y + delta.y < 0 || pos.y + delta.y >= CHUNK_SIZE || pos.z + delta.z < 0 || pos.z + delta.z > 255) {
		} else {
			GLint adj = _blocks[((((pos.x + delta.x) << CHUNK_SHIFT) + pos.y + delta.y) << WORLD_SHIFT) + pos.z + delta.z];
			if (visible_face(adj, type, opposite_dir(index))) {
				if (adj & blocks::NOTVISIBLE) {
					_blocks[((((pos.x + delta.x) << CHUNK_SHIFT) + pos.y + delta.y) << WORLD_SHIFT) + pos.z + delta.z] = adj - blocks::NOTVISIBLE;
				}
			}
		}
	}
	// std::cout << "\t_displayed_faces after add neighbours " << _displayed_faces << std::endl;
	handle_border_block(pos, type, true);
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
	int previous_type = _blocks[(((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z] & 0xFF;
	switch (modif) {
		case Modif::REMOVE:
			if (previous_type == blocks::AIR || previous_type == blocks::BEDROCK) { // can't rm bedrock
				return ;
			}
			// std::cout << "remove_block. displayed before " << _displayed_faces << std::endl;
			remove_block(useInventory, pos);
		break ;
		case Modif::ADD:
			if (type == blocks::WHEAT_CROP && (_blocks[(((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z - 1] & 0xFF) != blocks::FARMLAND) { // can't place crop on something other than farmland
				return ;
			} else if ((previous_type != blocks::AIR && previous_type < blocks::WATER) || type == blocks::AIR) { // can't replace block
				return ;
			}
			// std::cout << "ADD BLOCK " << s_blocks[previous_type]->name << " -> " << s_blocks[type]->name << std::endl;
			add_block(useInventory, pos, type, previous_type);
			break ;
		case Modif::REPLACE:
			if (type == blocks::DIRT_PATH && pos.z < 254 && (_blocks[(((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z + 1] & 0xFF) != blocks::AIR) { // can't turn dirt to dirt path if anything above it
				return ;
			} else if ((type == blocks::DIRT_PATH || type == blocks::FARMLAND) && previous_type != blocks::DIRT && previous_type != blocks::GRASS_BLOCK) {
				return ;
			}
			replace_block(useInventory, pos, type, previous_type);
			break ;
		case Modif::LITNT:
			remove_block(false, pos);
			_entities.push_back(new TNTEntity(this, {pos.x + _startX, pos.y + _startY, pos.z}, {0.2f, 0.2f, 0.4f}));
			break ;
		case Modif::USE:
			int offset = (((pos.x << CHUNK_SHIFT) + pos.y) << WORLD_SHIFT) + pos.z;
			int value = _blocks[offset];
			if ((value & 0xFF) != type) {
				std::cerr << "Chunk::regeneration type diff " << s_blocks[value & 0xFF]->name << " vs " << s_blocks[type]->name << std::endl;
				return ;
			}
			switch (type) {
			case blocks::OAK_DOOR:
				value ^= (DOOR::OPEN << 12);
				_blocks[offset] = value;
				_added[offset] = value;
				offset += ((value >> 12) & DOOR::UPPER_HALF) ? -1 : 1;
				value ^= (DOOR::UPPER_HALF << 12);
				break ;
			case blocks::OAK_TRAPDOOR:
				value ^= (DOOR::OPEN << 12);
				break ;
			case blocks::LEVER:
				value ^= REDSTONE::POWERED;
				_blocks[offset] = value; // used by adj dust
				flickLever(pos, value, (value >> REDSTONE::POWERED_OFFSET) & 0x1);
				break ;
			case blocks::REPEATER: // add 1 tick delay, mod it by 4
				value = (value & (-1 - (0x3 << REDSTONE::REPEAT_TICKS))) + (((((value >> REDSTONE::REPEAT_TICKS) & 0x3) + 1) & 0x3) << REDSTONE::REPEAT_TICKS);
				break ;
			case blocks::COMPARATOR: // switch mode between compare and substract
				value ^= REDSTONE::COMPARATOR_MODE;
				break ;
			case blocks::STONE_BUTTON:
			case blocks::OAK_BUTTON:
				std::cout << "button pressed" << std::endl;
				value |= REDSTONE::POWERED | REDSTONE::STRENGTH;
				_blocks[offset] = value; // used by adj dust
				stronglyPower(pos + getAttachedDir(value), -getAttachedDir(value), 0xF);
				weaklyPower(pos, {0, 0, 0}, REDSTONE::ON, false);
				scheduleRedstoneTick({pos, ((type == blocks::STONE_BUTTON) ? 10 : 15) * REDSTONE::TICK});
				break ;
			default:
				std::cout << "ERROR Chunk::regeneration case Modif::USE defaulted on: " << s_blocks[value & 0xFF]->name << std::endl;
				return ;
			}
			_blocks[offset] = value;
			_added[offset] = value;
			break ;
	}
	if (type == blocks::WATER || type == blocks::BUCKET) {
		// std::cout << "modif to water with " << ((type == blocks::WATER) ? "water" : "bucket") << std::endl;
		// std::cout << "water count before " << waterSave << ", after " << _water_count << std::endl;
		return ;
	} else if (type == blocks::GLASS || previous_type == blocks::GLASS) {
		sort_water(_camera->getPos(), true);
		return ;
	}
	// std::cout << "_displayed_faces after modif " << _displayed_faces << " at " << _startX << ", " << _startY << std::endl;
	if (!_light_update) {
		fill_vertex_array();
	}
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

/**
 * @brief calls Chunk::getBlockAt after substracting _startX and _startY from posX and posY.
 */
GLint Chunk::getBlockAtAbsolute( int posX, int posY, int posZ, bool askNeighbours )
{
	return (getBlockAt(posX - _startX, posY - _startY, posZ, askNeighbours));
}

GLint Chunk::getBlockAt( int posX, int posY, int posZ, bool askNeighbours )
{
	if (!_blocks || posZ < 0) {
		return (blocks::BEDROCK);
	} else if (posZ >= WORLD_HEIGHT) {
		return (blocks::AIR);
	}
	int res = blocks::BEDROCK;
	if (posX < 0) {
		if (askNeighbours && _neighbours[face_dir::MINUSX]) {
			res = _neighbours[face_dir::MINUSX]->getBlockAt(posX + CHUNK_SIZE, posY, posZ, askNeighbours);
		}
	} else if (posX >= CHUNK_SIZE) {
		if (askNeighbours && _neighbours[face_dir::PLUSX]) {
			res = _neighbours[face_dir::PLUSX]->getBlockAt(posX - CHUNK_SIZE, posY, posZ, askNeighbours);
		}
	} else if (posY < 0) {
		if (askNeighbours && _neighbours[face_dir::MINUSY]) {
			res = _neighbours[face_dir::MINUSY]->getBlockAt(posX, posY + CHUNK_SIZE, posZ, askNeighbours);
		}
	} else if (posY >= CHUNK_SIZE) {
		if (askNeighbours && _neighbours[face_dir::PLUSY]) {
			res = _neighbours[face_dir::PLUSY]->getBlockAt(posX, posY - CHUNK_SIZE, posZ, askNeighbours);
		}
	} else {
		res = _blocks[(((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + posZ];
	}
	return (res);
}

/**
 * @brief calls Chunk::setBlockAt after substracting _startX and _startY from posX and posY.
 */
void Chunk::setBlockAtAbsolute( int value, int posX, int posY, int posZ, bool askNeighbours )
{
	setBlockAt(value, posX - _startX, posY - _startY, posZ, askNeighbours);
}

void Chunk::setBlockAt( int value, int posX, int posY, int posZ, bool askNeighbours )
{
	if (!_blocks || posZ < 0 || posZ >= WORLD_HEIGHT) {
		return ;
	}
	if (posX < 0) {
		if (askNeighbours && _neighbours[face_dir::MINUSX]) {
			_neighbours[face_dir::MINUSX]->setBlockAt(value, posX + CHUNK_SIZE, posY, posZ, askNeighbours);
		}
	} else if (posX >= CHUNK_SIZE) {
		if (askNeighbours && _neighbours[face_dir::PLUSX]) {
			_neighbours[face_dir::PLUSX]->setBlockAt(value, posX - CHUNK_SIZE, posY, posZ, askNeighbours);
		}
	} else if (posY < 0) {
		if (askNeighbours && _neighbours[face_dir::MINUSY]) {
			_neighbours[face_dir::MINUSY]->setBlockAt(value, posX, posY + CHUNK_SIZE, posZ, askNeighbours);
		}
	} else if (posY >= CHUNK_SIZE) {
		if (askNeighbours && _neighbours[face_dir::PLUSY]) {
			_neighbours[face_dir::PLUSY]->setBlockAt(value, posX, posY - CHUNK_SIZE, posZ, askNeighbours);
		}
	} else {
		int offset = (((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + posZ;
		_blocks[offset] = value;
		if (value) {
			_added[offset] = value;
			_removed.erase(offset);
		} else {
			_added.erase(offset);
			_removed.insert(offset);
		}
		_vertex_update = true;
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
		if (_neighbours[face_dir::MINUSX]) {
			return (_neighbours[face_dir::MINUSX]->handleHit(useInventory, type, pos, modif));
		}
	} else if (chunk_pos.x >= CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSX]) {
			return (_neighbours[face_dir::PLUSX]->handleHit(useInventory, type, pos, modif));
		}
	} else if (chunk_pos.y < 0) {
		if (_neighbours[face_dir::MINUSY]) {
			return (_neighbours[face_dir::MINUSY]->handleHit(useInventory, type, pos, modif));
		}
	} else if (chunk_pos.y >= CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSY]) {
			return (_neighbours[face_dir::PLUSY]->handleHit(useInventory, type, pos, modif));
		}
	} else {
		// std::cout << _startX << " " << _startY << ": handle hit at pos " << chunk_pos.x << ", " << chunk_pos.y << ", " << chunk_pos.z << std::endl;
		regeneration(useInventory, type, chunk_pos, modif);
		return ;
	}
	
	std::cout << _startX << ", " << _startY << " ERROR BLOCK OUT OF CHUNK " << chunk_pos.x << ", " << chunk_pos.y << ", " << chunk_pos.z << std::endl;
}
