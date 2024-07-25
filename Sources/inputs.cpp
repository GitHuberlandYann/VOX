#include "OpenGL_Manager.hpp"
#include "callbacks.hpp"
#include "utils.h"
#include "Settings.hpp"
#include "WorldEdit.hpp"
#include "logs.hpp"

void OpenGL_Manager::setCurrentChunkPtr( std::shared_ptr<Chunk>& chunk )
{
	_current_chunk_ptr = chunk;
}

Chunk *OpenGL_Manager::getCurrentChunkPtr( void )
{
	return (_current_chunk_ptr.get());
}

void Player::resetInputsPtrs( void )
{
	_chunkHit = NULL;
	_chunk = NULL;
}

void OpenGL_Manager::resetInputsPtrs( void )
{
	_current_chunk_ptr = NULL;
	_player->resetInputsPtrs();
	_perimeter_chunks.clear();
	_visible_chunks.clear();
}

void OpenGL_Manager::pauseGame( void )
{
	_paused = true;
}

std::shared_ptr<Chunk> OpenGL_Manager::getChunkAt( int posX, int posY )
{
	for (auto c : _visible_chunks) {
		if (c->isInChunk(posX, posY)) {
			return (c);
		}
	}
	if (!_visible_chunks.size()) {
		updateVisibleChunks();
	} else {
		LOGERROR("chunk out of bound at " << POSXY(posX, posY));
	}
	return (NULL);
}

t_hit Player::computeBlockHit( void )
{
	t_hit res = {{0, 0, 0}, getEyePos(), {0, 0, 0}, 0, 0, 0};
	std::vector<glm::ivec3> ids = computeRayCasting((_gameMode == settings::consts::gamemode::creative) ? (_handContent == blocks::worldedit_brush) ? settings::consts::reach::brush : settings::consts::reach::creative : settings::consts::reach::survival);

	glm::ivec2 current_chunk = {INT_MAX, INT_MAX};
	std::shared_ptr<Chunk> chunk = NULL;
	bool first_loop = true;
	for (auto& i : ids) {
		int posX = utils::math::chunk_pos(i.x);
		int posY = utils::math::chunk_pos(i.y);
		
		if (!chunk || posX != current_chunk.x || posY != current_chunk.y) {
			current_chunk = {posX, posY};
			chunk = _oglMan->getChunkAt(posX, posY);
			if (!chunk) {
				return (res);
			}
		}
		if (first_loop) {
			_oglMan->setCurrentChunkPtr(chunk);
			_chunk = chunk.get();
			first_loop = false;
		}

		int value = chunk->isHit(i);
		int type = TYPE(value);
		if (type == blocks::water) {
			if (!res.water_value) {
				res.water_pos = i;
				res.water_value = true;
			}
			res.prev_pos = i;
		} else if (type) {
			// we know cube is hit, now check if hitbox is hit (only on non cube-filling values)
			const auto& target = s_blocks[type];
			glm::vec3 hitbox[2];
			if (target->hasHitbox) {
				target->getHitbox(hitbox, value);
			}
			if (!target->hasHitbox || utils::math::line_cube_intersection(getEyePos(), _front, glm::vec3(i) + hitbox[0], hitbox[1])) {
				_chunkHit = chunk;
				res.pos = i;
				res.value = value;
				res.type = type;
				return (res);
			} else if (target->hasSecondaryHitbox) {
				target->getSecondaryHitbox(hitbox, value);
				if (utils::math::line_cube_intersection(getEyePos(), _front, glm::vec3(i) + hitbox[0], hitbox[1])) {
					_chunkHit = chunk;
					res.pos = i;
					res.value = value;
					res.type = type;
					return (res);
				}
			}
		} else {
			res.prev_pos = i;
		}
	}
	return (res);
}

void Player::handleBlockModif( bool adding, bool collect )
{
	if (!adding) {
		if (_blockHit.type == blocks::air) {
			return ;
		} else if (_handContent == blocks::worldedit_wand) {
			return (WorldEdit::Get()->setSelectionStart(_blockHit.pos));
		}
		if (_gameMode == settings::consts::gamemode::adventure && !(_blockHit.value & mask::adventure_block)) {
			if (TYPE(_blockHit.value) == blocks::item_frame && !(_blockHit.value & mask::frame::locked)) {
				_chunkHit->handleHit(true, 0, _blockHit.pos, Modif::popItem);
			}
			_ui->chatMessage("[Adventure mode] you can't break blocks you didn't place yourself.", argb::red);
			return ;
		}
		if (_chunkHit) {
			_chunkHit->handleHit(collect, 0, _blockHit.pos, Modif::rm);
			return ;
		}
		LOGERROR("rm: chunk out of bound at " << utils::math::chunk_pos(_blockHit.pos.x) << ", " << utils::math::chunk_pos(_blockHit.pos.y));
		return ;
	}
	// from now on adding = true
	switch (_blockHit.type) {
		case blocks::crafting_table:
			_oglMan->pauseGame();
			_menu->setState(menu::crafting);
			return ;
		case blocks::chest:
			if (!s_blocks[TYPE(_chunkHit->getBlockAtAbsolute(_blockHit.pos + glm::ivec3(0, 0, 1)))]->transparent) {
				return ;
			}
			_chunkHit->openChest(_blockHit.pos);
			_oglMan->pauseGame();
			_menu->setState(menu::chest);
			_menu->setChestInstance(_chunkHit->getChestInstance(_blockHit.pos));
			return ;
		case blocks::furnace:
			_oglMan->pauseGame();
			_menu->setState(menu::furnace);
			_menu->setFurnaceInstance(_chunkHit->getFurnaceInstance(_blockHit.pos));
			return ;
		case blocks::anvil:
			_oglMan->pauseGame();
			_menu->setState(menu::anvil);
			return ;
		case blocks::tnt:
			if (_handContent == blocks::flint_and_steel && _chunkHit) {
				_chunkHit->handleHit(false, blocks::tnt, _blockHit.pos, Modif::litnt);
			}
			break ;
		case blocks::item_frame:
			_chunkHit->handleHit(collect, _handContent, _blockHit.pos, Modif::use);
			return ;
		case blocks::lectern:
			if (_chunkHit->bookedLectern(_menu, _blockHit.pos, false)) {
				_oglMan->pauseGame();
				_menu->setState(menu::lectern);
				return ;
			} else if (_handContent == blocks::written_book && _chunkHit) {
				_chunkHit->handleHit(collect, _handContent, _blockHit.pos, Modif::use);
				return ;
			}
			return ;
		case blocks::oak_door:
		case blocks::oak_trapdoor:
		case blocks::lever:
		case blocks::repeater:
		case blocks::comparator:
		case blocks::stone_button:
		case blocks::oak_button:
			if (_chunkHit) {
				_chunkHit->handleHit(false, _blockHit.type, _blockHit.pos, Modif::use);
			}
			return ;
	}
	int type = _handContent;
	geometry shape = s_blocks[type]->geometry;
	if (type >= blocks::wooden_hoe && type <= blocks::diamond_hoe
		&& (_blockHit.type == blocks::dirt || _blockHit.type == blocks::grass_block)) { // special case, add but change block to farmland instead
		type = blocks::farmland;
		_chunkHit->handleHit(true, type, _blockHit.pos, Modif::replace);
		return ;
	} else if (type >= blocks::wooden_shovel && type <= blocks::diamond_shovel
		&& (_blockHit.type == blocks::dirt || _blockHit.type == blocks::grass_block)) { // special case, add but change block to farmland instead
		type = blocks::dirt_path;
		_chunkHit->handleHit(true, type, _blockHit.pos, Modif::replace);
		return ;
	} else if (type == blocks::zombie_egg || type == blocks::skeleton_egg) {
		_chunkHit->handleHit(collect, type, _blockHit.prev_pos, Modif::use);
		return ;
	}
	if (_gameMode == settings::consts::gamemode::adventure) {
		if (type == blocks::air) {
			return ;
		}
		int dist = glm::abs(_blockHit.pos.x - _blockHit.prev_pos.x) + glm::abs(_blockHit.pos.y - _blockHit.prev_pos.y) + glm::abs(_blockHit.pos.z - _blockHit.prev_pos.z);
		if (dist > 1) {
			return ;
		}
		if (_blockHit.type != s_blocks[type]->adventure_block) {
			_ui->chatMessage(((type == blocks::bucket || type == blocks::water_bucket) ? "[Adventure mode] you can only use " : "[Adventure mode] you can only place ")
							+ s_blocks[type]->name + " on " + s_blocks[s_blocks[type]->adventure_block]->name + ".", argb::red);
			return ;
		}
	}
	// std::cout << "aiming " << s_blocks[type]->name << " towards " << s_blocks[_blockHit.type]->name << std::endl;;
	if (type == blocks::water_bucket) { // use it like any other block
		type = blocks::water;
	} else if (type == blocks::bucket) { // special case, add but remove water instead
		if (_blockHit.water_value) {
			_chunk->handleHit(collect, type, _blockHit.water_pos, Modif::rm);
		}
		return ;
	} else if (type == blocks::wheat_seeds) {
		type = blocks::wheat_crop;
	} else if (type == blocks::book_and_quill) {
		_oglMan->pauseGame();
		_menu->setState(menu::book_and_quill);
		t_item item = _inventory->getSlotBlock(_inventory->getSlotNum());
		if (item.tag && item.tag->getType() == tags::book_tag) {
			_menu->setBookContent(&static_cast<BookTag*>(item.tag.get())->getContent());
		}
		return ;
	} else if (type == blocks::written_book) {
		_oglMan->pauseGame();
		_menu->setState(menu::book);
		t_item item = _inventory->getSlotBlock(_inventory->getSlotNum());
		if (item.tag && item.tag->getType() == tags::book_tag) {
			_menu->setBookContent(&static_cast<BookTag*>(item.tag.get())->getContent());
		}
		return ;
	} else if (s_blocks[type]->isItemOnly) {
		if (type == blocks::worldedit_wand) {
			return (WorldEdit::Get()->setSelectionEnd(_blockHit.pos));
		}
		// std::cout << "can't add block if no object in inventory" << std::endl;
		return ;
	} else if (type == blocks::torch || type == blocks::redstone_torch) {
		if (_blockHit.pos.z != _blockHit.prev_pos.z) {
			type += (face_dir::minus_z << offset::blocks::orientation);
		} else if (_blockHit.pos.x != _blockHit.prev_pos.x) {
			type += (((_blockHit.pos.x > _blockHit.prev_pos.x) ? face_dir::plus_x : face_dir::minus_x) << offset::blocks::orientation);
		} else {
			type += (((_blockHit.pos.y > _blockHit.prev_pos.y) ? face_dir::plus_y : face_dir::minus_y) << offset::blocks::orientation);
		}
	} else if (type == blocks::oak_sign) {
		if (_blockHit.pos.x != _blockHit.prev_pos.x) {
			type += (((_blockHit.pos.x > _blockHit.prev_pos.x) ? face_dir::plus_x : face_dir::minus_x) << offset::blocks::orientation);
		} else if (_blockHit.pos.y != _blockHit.prev_pos.y) {
			type += (((_blockHit.pos.y > _blockHit.prev_pos.y) ? face_dir::plus_y : face_dir::minus_y) << offset::blocks::orientation);
		} else {
			return ;
		}
		_oglMan->pauseGame();
		_menu->setState(menu::sign);
		_menu->setSignPos(_blockHit.prev_pos);
	} else if (type == blocks::lever || shape == geometry::button || type == blocks::item_frame) {
		if (s_blocks[_blockHit.type]->geometry != geometry::cube) { // TODO chnge this
			return ;
		}
		if (_blockHit.pos.z > _blockHit.prev_pos.z) {
			type += (placement::ceiling << offset::blocks::bitfield);
		} else if (_blockHit.pos.z < _blockHit.prev_pos.z) {
			type += (placement::floor << offset::blocks::bitfield);
		} else {
			type += (placement::wall << offset::blocks::bitfield);
			if (_blockHit.pos.x != _blockHit.prev_pos.x) {
				type += (((_blockHit.pos.x > _blockHit.prev_pos.x) ? face_dir::minus_x : face_dir::plus_x) << offset::blocks::orientation);
			} else {
				type += (((_blockHit.pos.y > _blockHit.prev_pos.y) ? face_dir::minus_y : face_dir::plus_y) << offset::blocks::orientation);
			}
		}
	} else if (shape == geometry::slab || shape == geometry::stairs) {
		if (_blockHit.pos.z != _blockHit.prev_pos.z) {
			type = ((_blockHit.pos.z < _blockHit.prev_pos.z) ? type : type | ((shape == geometry::slab) ? mask::slab::top : mask::stairs::top));
		} else if (_blockHit.pos.x != _blockHit.prev_pos.x) {
			glm::vec3 p0 = _blockHit.pos + ((_blockHit.pos.x > _blockHit.prev_pos.x) ? glm::ivec3(0, 0, 0) : glm::ivec3(1, 0, 0));
			glm::vec3 intersect = utils::math::line_plane_intersection(getEyePos(), _front, p0, {1, 0, 0});
			type = ((intersect.z - static_cast<int>(intersect.z) < 0.5f) ? type : type | ((shape == geometry::slab) ? mask::slab::top : mask::stairs::top));
			// _ui->chatMessage("block hit " + std::to_string(_blockHit.pos.x) + ", " + std::to_string(_blockHit.pos.y) + ", " + std::to_string(_blockHit.pos.z));
			// _ui->chatMessage("p0 at " + std::to_string(p0.x) + ", " + std::to_string(p0.y) + ", " + std::to_string(p0.z));
			// _ui->chatMessage("intersect at " + std::to_string(intersect.x) + ", " + std::to_string(intersect.y) + ", " + std::to_string(intersect.z));
		} else {
			glm::vec3 p0 = _blockHit.pos + ((_blockHit.pos.y > _blockHit.prev_pos.y) ? glm::ivec3(0, 0, 0) : glm::ivec3(0, 1, 0));
			glm::vec3 intersect = utils::math::line_plane_intersection(getEyePos(), _front, p0, {0, 1, 0});
			type = ((intersect.z - static_cast<int>(intersect.z) < 0.5f) ? type : type | ((shape == geometry::slab) ? mask::slab::top : mask::stairs::top));
		}
	} else if (shape == geometry::trapdoor) {
		if (_blockHit.pos.z != _blockHit.prev_pos.z) {
			type += ((_blockHit.pos.z < _blockHit.prev_pos.z) ? 0 : (door::upper_half << offset::blocks::bitfield));
		} else if (_blockHit.pos.x != _blockHit.prev_pos.x) {
			glm::vec3 p0 = _blockHit.pos + ((_blockHit.pos.x > _blockHit.prev_pos.x) ? glm::ivec3(0, 0, 0) : glm::ivec3(1, 0, 0));
			glm::vec3 intersect = utils::math::line_plane_intersection(getEyePos(), _front, p0, {1, 0, 0});
			type += ((intersect.z - static_cast<int>(intersect.z) < 0.5f) ? 0 : (door::upper_half << offset::blocks::bitfield));
			// _ui->chatMessage("block hit " + std::to_string(_blockHit.pos.x) + ", " + std::to_string(_blockHit.pos.y) + ", " + std::to_string(_blockHit.pos.z));
			// _ui->chatMessage("p0 at " + std::to_string(p0.x) + ", " + std::to_string(p0.y) + ", " + std::to_string(p0.z));
			// _ui->chatMessage("intersect at " + std::to_string(intersect.x) + ", " + std::to_string(intersect.y) + ", " + std::to_string(intersect.z));
		} else {
			glm::vec3 p0 = _blockHit.pos + ((_blockHit.pos.y > _blockHit.prev_pos.y) ? glm::ivec3(0, 0, 0) : glm::ivec3(0, 1, 0));
			glm::vec3 intersect = utils::math::line_plane_intersection(getEyePos(), _front, p0, {0, 1, 0});
			type += ((intersect.z - static_cast<int>(intersect.z) < 0.5f) ? 0 : (door::upper_half << offset::blocks::bitfield));
		}
	} else if (type == blocks::oak_log) {
		if (_blockHit.pos.z != _blockHit.prev_pos.z) {
			type += (face_dir::plus_z << offset::blocks::orientation);
		} else if (_blockHit.pos.x != _blockHit.prev_pos.x) {
			type += (face_dir::plus_x << offset::blocks::orientation);
		} else {
			type += (face_dir::plus_y << offset::blocks::orientation);
		}
	}

	if (_blockHit.type) { // rm if statement for nice cheat
		if (_gameMode == settings::consts::gamemode::adventure && type != blocks::water) {
			type |= mask::adventure_block;
		}
		_chunkHit->handleHit(collect, type, _blockHit.prev_pos, Modif::add);
	}
}

void Player::setItemFrame( bool visible, bool lock )
{
	if (TYPE(_blockHit.value) != blocks::item_frame) {
		return (_ui->chatMessage("Block hit is not an item frame.", argb::red));
	}
	if (!_chunkHit) {
		return (_ui->chatMessage("Cmd failure: missing chunk ptr.", argb::red));
	}
	if (visible) {
		_blockHit.value ^= mask::frame::notVisible;
		_ui->chatMessage("Item frame set as " + std::string((_blockHit.value & mask::frame::notVisible) ? "invisible." : "visible."));
	}
	if (lock) {
		_blockHit.value ^= mask::frame::locked;
		_ui->chatMessage("Item frame set as " + std::string((_blockHit.value & mask::frame::locked) ? "locked." : "unlocked."));
	}
	_chunkHit->setBlockAtAbsolute(_blockHit.value, _blockHit.pos, false);
}

void OpenGL_Manager::setItemFrame( bool visible, bool lock )
{
	_player->setItemFrame(visible, lock);
}

void OpenGL_Manager::updateCamView( void )
{
	glm::mat4 view = _camera->getViewMatrix();
	_skybox->updateCamView(view);
	glUniformMatrix4fv(_modelShader.getUniform(settings::consts::shader::uniform::view), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(_particleShader.getUniform(settings::consts::shader::uniform::view), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(_skyShader.getUniform(settings::consts::shader::uniform::view), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(_shader.getUniform(settings::consts::shader::uniform::view), 1, GL_FALSE, glm::value_ptr(view));
}

void OpenGL_Manager::updateCamPerspective( void )
{
	glm::mat4 proj = _camera->getPerspectiveMatrix();
	_skybox->updateCamPerspective(proj);
	glUniformMatrix4fv(_modelShader.getUniform(settings::consts::shader::uniform::proj), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(_particleShader.getUniform(settings::consts::shader::uniform::proj), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(_skyShader.getUniform(settings::consts::shader::uniform::proj), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(_shader.getUniform(settings::consts::shader::uniform::proj), 1, GL_FALSE, glm::value_ptr(proj));
}

void OpenGL_Manager::updateAnimFrame( void )
{
	static int current_frame;
	glUniform1i(_skyShader.getUniform(settings::consts::shader::uniform::animation), current_frame);
	if (++current_frame == 31) {
		current_frame = 0;
	}
}

void OpenGL_Manager::updateVisibleChunks( void )
{
	_visible_chunks.clear();
	mtx_perimeter.lock();
	for (auto& peri: _perimeter_chunks) {
		if (_player->chunkInFront(_camera->getCamPlacement(), _current_chunk, peri->getStartX(), peri->getStartY())) {
			_visible_chunks.push_back(peri);
		}
	}
	mtx_perimeter.unlock();
}

void OpenGL_Manager::chunkUpdate( void )
{
	int posX = utils::math::chunk_pos(static_cast<int>(glm::floor(_player->getPos().x)));
	int posY = utils::math::chunk_pos(static_cast<int>(glm::floor(_player->getPos().y)));
	
	if (posX == _current_chunk.x && posY == _current_chunk.y) {
		return ;
	}
	// std::cout << "new chunk " << posX << ", " << posY << " !" << std::endl;
	_mtx.lock();
	_current_chunk = {posX, posY};
	_mtx.unlock();

	setThreadUpdate(true);
}

void Player::inputToggle( void )
{
	// toggle game mode
	if (inputs::key_down(inputs::gamemode) && inputs::key_update(inputs::gamemode)) {
		if (inputs::key_down(inputs::debug)) {
			Settings::Get()->setBool(settings::bools::visible_chunk_border, !Settings::Get()->getBool(settings::bools::visible_chunk_border));
		} else {
			setGameMode(!_gameMode);
		}
	}
	// toggle outline
	if (inputs::key_down(inputs::block_highlight) && inputs::key_update(inputs::block_highlight)) {
		bool outline = !Settings::Get()->getBool(settings::bools::outline);
		Settings::Get()->setBool(settings::bools::outline, outline);
		_ui->chatMessage(std::string("outlines ") + ((outline) ? "SHOWN" : "HIDDEN"));
		_breakFrame = outline;
	}
}

float Player::getBreakTime( bool canHarvest )
{
	float speedMultiplier = 1;
	int needed_tool = s_blocks[_blockHit.type]->needed_tool;
	bool isBestTool = _handContent >= needed_tool && _handContent < needed_tool + 4;

	if (isBestTool) {
		speedMultiplier = (1 + _handContent - needed_tool) << 1; // wood 2 stone 4 iron 6 diamond 8
		// if (toolEfficiency) { // will be used once enchantments are implemented
		// 	speedMultiplier += efficiencyLevel ^ 2 + 1
		// }
	}

	// if (hasteEffect) // will be used if beacons implemented
	// 	speedMultiplier *= 0.2 * hasteLevel + 1
	// if (miningFatigue) // will be used if implemented
	// 	speedMultiplier *= 0.3 ^ min(miningFatigueLevel, 4)
	if (_waterHead) { // && !hasAquaAffinity // will be used once implemented
		speedMultiplier /= 5;
	}
	if (!_touchGround) {
		speedMultiplier /= 5;
	}

	float damage = speedMultiplier / s_blocks[_blockHit.type]->hardness;
	if (damage < 0) { // bedrock and such
		return (FLT_MAX);
	}
	if (canHarvest) {
		damage /= 30;
	} else {
		damage /= 100;
	}

	// _ui->chatMessage(s_blocks[_blockHit.type]->name + " using " + s_blocks[_hand_content]->name);
	// Instant breaking
	if (damage > 1) {
		// _ui->chatMessage("\tINSTANT BREAK");
		return (0);
	}

	int ticks = glm::ceil(1 / damage);
	float seconds = ticks / 20.0f;
	// _ui->chatMessage(std::to_string(seconds) + " vs " + std::to_string(s_blocks[_blockHit.type]->getBreakTime(_hand_content)));
	return (seconds);
}

void Player::blockInputUpdate( void )
{
	_handContent = _inventory->getCurrentSlot();
	if (inputs::key_down(inputs::destroy)) {
		bool firstFrame = inputs::key_update(inputs::destroy);
		AMob* mobHit = NULL;
		if (firstFrame && _chunk) { // check if mob hit by punch
			mobHit = _chunk->mobHit(_blockHit);
		}
		if (mobHit) {
			setArmAnimation(true);
			mobHit->receiveDamage(6.0f, _front);
			// TODO set _punch to true to avoid breaking block behind mob
		} else if (_handContent == blocks::worldedit_brush) {
			WorldEdit::Get()->useBrush(_blockHit.pos, false);
		} else if (_gameMode != settings::consts::gamemode::creative) {
			if (_blockHit.type == blocks::air || (!firstFrame && TYPE(_blockHit.type) == blocks::item_frame)) {
				setArmAnimation(firstFrame);
			} else {
				setArmAnimation(true);
				_breakTime += _deltaTime;
				bool can_collect = s_blocks[_blockHit.type]->canCollect(_handContent);
				float break_time = getBreakTime(can_collect);
				if (_breakTime >= break_time) {
					_breakTime = (break_time > 0) ? -0.3f : 0;
					_breakFrame = Settings::Get()->getBool(settings::bools::outline);
					handleBlockModif(false, can_collect);
					updateExhaustion(settings::consts::exhaustion::breaking_block);
					_inventory->decrementDurabitilty();
				} else {
					int break_frame = (_breakTime < .0f) ? 0 : static_cast<int>(10 * _breakTime / break_time) + 2;
					if (_breakFrame != break_frame) {
						if (_chunkHit) {
							_chunkHit->updateBreak(_blockHit);
						}
						_breakFrame = break_frame;
					}
				}
			}
		} else if (firstFrame) {
			handleBlockModif(false, false);
		}
	} else if (inputs::key_update(inputs::destroy)) {
		setArmAnimation(false);
		_breakTime = 0;
		_breakFrame = Settings::Get()->getBool(settings::bools::outline);
	} else {
		setArmAnimation(false);
	}
	if (inputs::key_down(inputs::use)) {
		if (_handContent == blocks::worldedit_brush && _blockHit.pos != glm::ivec3(0)) {
			WorldEdit::Get()->useBrush(_blockHit.prev_pos, true);
		} else if (_gameMode != settings::consts::gamemode::creative && s_blocks[_handContent]->isFood) {
			_eatTime += _deltaTime;
			if (_eatTime >= 1.61f) {
				if (canEatFood(_handContent)) {
					_inventory->removeBlock(false);
				}
				_eatTime = 0;
			}
		} else if (_handContent == blocks::bow) {
			_bowTime += _deltaTime;
		} else if (inputs::key_update(inputs::use)) {
			handleBlockModif(true, _gameMode != settings::consts::gamemode::creative);
			setArmAnimation(true);
		}
	} else if (inputs::key_update(inputs::use)) {
		if (_handContent == blocks::bow && _bowTime && _chunk) {
			// TODO rm arrow from inventory (once implemented)
			_inventory->decrementDurabitilty();
			_chunk->shootArrow(getEyePos(), _front, _bowTime);
		}
		_eatTime = 0;
		_bowTime = 0;
	}
	// drop item
	if (inputs::key_down(inputs::drop)) {
		if (_chunk) {
			t_item details = _inventory->removeBlock(true);
			if (details.type != blocks::air) {
				mtx.lock();
				_chunk->dropEntity(_front, details);
				mtx.unlock();
			}
		}
	}
	if (_blockHit.type != blocks::air
		&& inputs::key_down(inputs::sample) && inputs::key_update(inputs::sample)) { // pick up in creative mode, or swap
		_inventory->replaceSlot(_blockHit.type, _gameMode == settings::consts::gamemode::creative);
	}
}

void OpenGL_Manager::userInputs( bool rayCast )
{
	// open menu
	if (inputs::key_down(inputs::close) && inputs::key_update(inputs::close)) {
		_paused = true;
		_menu->setState(menu::pause);
		return ;
	}
	// open inventory
	if (inputs::key_down(inputs::inventory) && inputs::key_update(inputs::inventory)) {
		_paused = true;
		_menu->setState(menu::inventory);
		return ;
	}
	// toggle chat
	if ((inputs::key_down(inputs::chat) && inputs::key_update(inputs::chat))
		|| (inputs::key_down(inputs::enter) && inputs::key_update(inputs::enter))) {
		_paused = true;
		_menu->setState(menu::chat);
		return ;
	}
	// toggle 'command' chat, ie open chat, with '/' already written
	if (inputs::key_down(inputs::command) && inputs::key_update(inputs::command)) {
		_paused = true;
		_menu->setState(menu::command);
		return ;
	}
	// quit program
	if (inputs::key_down(inputs::quit_program)) {
		glfwSetWindowShouldClose(_window, GL_TRUE);
		return ;
	}

	// take screenshot
	if (inputs::key_down(inputs::screenshot) && inputs::key_update(inputs::screenshot)) {
		screenshot();
	}
	// toggle debug mode on off F3
	if (inputs::key_down(inputs::debug) && inputs::key_update(inputs::debug)) {
		_debugMode = !_debugMode;
	}
	// toggle F3 + I == display full info about block hit
	if (inputs::key_down(inputs::info) && inputs::key_update(inputs::info) && inputs::key_down(inputs::debug)) {
		t_hit bHit = _player->getBlockHit();
		_ui->chatMessage("*******************");
		_ui->chatMessage("Info about " + s_blocks[bHit.type]->name + " at " + std::to_string(bHit.pos.x) + ", " + std::to_string(bHit.pos.y) + ", " + std::to_string(bHit.pos.z));
		_ui->chatMessage(std::string("visible: ") + ((bHit.value & mask::blocks::notVisible) ? "FALSE" : "TRUE"));
		_ui->chatMessage(std::string("adventure block: ") + ((bHit.value & mask::adventure_block) ? "TRUE" : "FALSE"));
		_ui->chatMessage(std::string("orientation: ") + std::to_string((bHit.value >> offset::blocks::orientation) & 0x7));
		_ui->chatMessage(std::string("transparent: ") + ((s_blocks[bHit.type]->isTransparent(bHit.value)) ? "TRUE" : "FALSE"));
		_ui->chatMessage(std::string("powered: ") + ((bHit.value & mask::redstone::powered) ? "TRUE" : "FALSE"));
		_ui->chatMessage(std::string("weakdy powered: ") + ((bHit.value & mask::redstone::weakdy_powered) ? "TRUE" : "FALSE"));
		_ui->chatMessage(std::string("activated: ") + ((bHit.value & mask::redstone::activated) ? "TRUE" : "FALSE"));
		_ui->chatMessage(std::string("strength: ") + std::to_string((bHit.value >> offset::redstone::strength) & 0xF));
		_ui->chatMessage(std::string("bitfield: ") + std::to_string(bHit.value >> offset::blocks::bitfield));
		switch (bHit.type) {
			case blocks::piston:
			case blocks::sticky_piston:
				_ui->chatMessage(std::string("moving piston: ") + ((bHit.value & mask::redstone::piston::moving) ? "TRUE" : "FALSE"));
			break ;
		}
		_ui->chatMessage("*******************");
	}
	// toggle F5 mode
	if (inputs::key_down(inputs::perspective) && inputs::key_update(inputs::perspective)) {
		_camera->changeCamPlacement();
	}
	// toggle hotbar F1
	if (inputs::key_down(inputs::hotbar) && inputs::key_update(inputs::hotbar)) {
		_ui->_hideUI = !_ui->_hideUI;
		Settings::Get()->setBool(settings::bools::hide_ui, _ui->_hideUI);
		_ui->chatMessage(std::string("UI ") + ((_ui->_hideUI) ? "HIDDEN" : "SHOWN"));
	}
	// change block atlas
	if (inputs::key_down(inputs::block_atlas_clean) && inputs::key_update(inputs::block_atlas_clean)) {
		Settings::Get()->setString(settings::strings::block_atlas, "Resources/Resource_Packs/Clean/Textures/");
		loadTextures();
	} else if (inputs::key_down(inputs::block_atlas_default) && inputs::key_update(inputs::block_atlas_default)) {
		Settings::Get()->setString(settings::strings::block_atlas, "Resources/Textures/");
		loadTextures();
	}
	_player->inputToggle();

	// add and remove blocks
	_player->setDelta(_time.deltaTime);
	_player->blockInputUpdate();

	// toggle polygon mode fill / lines
	if (inputs::key_down(inputs::wireframe) && inputs::key_update(inputs::wireframe)) {
		_fill = !_fill;
		glPolygonMode(GL_FRONT_AND_BACK, (_fill) ? GL_FILL : GL_LINE);
		_ui->chatMessage((_fill) ? "Triangles FILLED" : "Triangles EMPTIED");
	}

	_player->inputUpdate(rayCast);

	if (rayCast) {
		if (!_player->getHealth() && _current_chunk_ptr) { // dead
			_player->getInventory()->spillInventory(_current_chunk_ptr.get());
			_paused = true;
			_menu->setState(menu::death);
			return ;
		}
	}

	if (_player->getResetFovUpdate() || inputs::key_update(inputs::zoom)) {
		updateCamPerspective();
	}
	if (_player->getCamUpdate()) {
		updateCamView();
		updateVisibleChunks();
	}

	// inventory slot selection
	for (int index = 0; index < 8; ++index) {
		if (inputs::key_down(inputs::slot_0 + index) && inputs::key_update(inputs::slot_0 + index)) {
			_ui->inventoryMessage(_player->getInventory()->setSlot(index));
			break ;
		}
	}
}
