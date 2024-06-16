#include "OpenGL_Manager.hpp"
#include "callbacks.hpp"
#include "utils.h"
#include "Settings.hpp"
#include "WorldEdit.hpp"
#include <iostream>

Chunk *OpenGL_Manager::getCurrentChunkPtr( void )
{
	return (_current_chunk_ptr);
}

void OpenGL_Manager::resetInputsPtrs( void )
{
	_current_chunk_ptr = NULL;
	_chunk_hit = NULL;
	_perimeter_chunks.clear();
	_visible_chunks.clear();
}

t_hit OpenGL_Manager::getBlockHit( void )
{
	t_hit res = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 0, 0, 0};
	std::vector<glm::ivec3> ids = _camera->computeRayCasting((_game_mode == settings::consts::gamemode::creative) ? settings::consts::chunk_size : REACH);

	glm::ivec2 current_chunk = glm::ivec2(INT_MAX, INT_MAX);
	Chunk *chunk = NULL;
	bool first_loop = true;
	for (auto& i : ids) {
		// std::cout << "checking > " << i.x << ", " << i.y << ", " << i.z << std::endl;
		int posX = chunk_pos(i.x);
		int posY = chunk_pos(i.y);
		
		if (!chunk || posX != current_chunk.x || posY != current_chunk.y) {
			current_chunk = glm::ivec2(posX, posY);
			chunk = NULL;
			for (auto c : _visible_chunks) {
				if (c->isInChunk(posX, posY)) {
					chunk = c;
					break ;
				}
			}
			if (!chunk) {
				if (!_visible_chunks.size()) {
					updateVisibleChunks();
				} else {
					std::cout << "chunk out of bound at " << posX << ", " << posY << std::endl;
				}
				return (res);
			}
		}
		if (first_loop) {
			_current_chunk_ptr = chunk;
			_camera->setCurrentChunkPtr(chunk);
			first_loop = false;
		}
		// std::cout << "current_chunk should be " << current_chunk.x << ", " << current_chunk.y << std::endl;
		int value = chunk->isHit(i);
		// if ((value & mask::blocks::type) == blocks::glass_pane) {
		// 	_ui->chatMessage("orientation " + std::to_string((value >> offset::blocks::orientation) & 0x7)
		// 		+ " bitfield " + std::to_string(value >> 12));
		// }
		int type = value & mask::blocks::type;
		if (type == blocks::water) {
			if (!res.water_value) {
				res.water_pos = i;
				res.water_value = true;
			}
			res.prev_pos = i;
		} else if (type) {
			// we know cube is hit, now check if hitbox is hit (only on non cube-filling values)
			// TODO move hitboxes of torches depending on the wall they hang on
			const auto& target = s_blocks[type];
			if (!target->hasHitbox || line_cube_intersection(_camera->getEyePos(), _camera->getDir(), glm::vec3(i) + target->hitboxCenter, target->hitboxHalfSize)) {
				_chunk_hit = chunk;
				res.pos = i;
				res.value = value;
				res.type = type;
				// _ui->chatMessage(s_blocks[type]->name + ((value & mask::redstone::powered) ? " block hit is powered" : " block hit is not powered"));
				return (res);
			} else if (target->hasOrientedHitbox) {
				glm::vec3 hitbox[2];
				target->getSecondaryHitbox(hitbox, (value >> offset::blocks::orientation) & 0x7, value >> offset::blocks::bitfield);
				if (line_cube_intersection(_camera->getEyePos(), _camera->getDir(), glm::vec3(i) + hitbox[0], hitbox[1])) {
					_chunk_hit = chunk;
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

void OpenGL_Manager::handleBlockModif( bool adding, bool collect )
{
	if (!adding) {
		if (_block_hit.type == blocks::air) {
			return ;
		} else if (_hand_content == blocks::worldedit_wand) {
			return (WorldEdit::Get()->setSelectionStart(_block_hit.pos));
		}
		if (_game_mode == settings::consts::gamemode::adventure && !(_block_hit.value & mask::adventure_block)) {
			_ui->chatMessage("[Adventure mode] you can't break blocks you didn't place yourself.", TEXT::RED);
			return ;
		}
		if (_chunk_hit) {
			_chunk_hit->handleHit(collect, 0, _block_hit.pos, Modif::rm);
			return ;
		}
		int posX = chunk_pos(_block_hit.pos.x);
		int posY = chunk_pos(_block_hit.pos.y);

		std::cout << "rm: chunk out of bound at " << posX << ", " << posY << std::endl;
		return ;
	}
	// from now on adding = true
	switch (_block_hit.type) {
		case blocks::crafting_table:
			_paused = true;
			_menu->setState(menu::crafting);
			return ;
		case blocks::chest:
			if (!s_blocks[_chunk_hit->getBlockAt(_block_hit.pos.x - _chunk_hit->getStartX(), _block_hit.pos.y - _chunk_hit->getStartY(), _block_hit.pos.z + 1) & mask::blocks::type]->transparent) {
				return ;
			}
			_chunk_hit->openChest(_block_hit.pos);
			_paused = true;
			_menu->setState(menu::chest);
			_menu->setChestInstance(_chunk_hit->getChestInstance(_block_hit.pos));
			return ;
		case blocks::furnace:
			_paused = true;
			_menu->setState(menu::furnace);
			_menu->setFurnaceInstance(_chunk_hit->getFurnaceInstance(_block_hit.pos));
			return ;
		case blocks::tnt:
			if (_hand_content == blocks::flint_and_steel && _current_chunk_ptr) {
				_current_chunk_ptr->handleHit(false, blocks::tnt, _block_hit.pos, Modif::litnt);
			}
			break ;
		case blocks::oak_door:
		case blocks::oak_trapdoor:
		case blocks::lever:
		case blocks::repeater:
		case blocks::comparator:
		case blocks::stone_button:
		case blocks::oak_button:
			if (_current_chunk_ptr) {
				_current_chunk_ptr->handleHit(false, _block_hit.type, _block_hit.pos, Modif::use);
			}
			return ;
	}
	int type = _hand_content;
	geometry shape = s_blocks[type]->geometry;
	if (type >= blocks::wooden_hoe && type <= blocks::diamond_hoe
		&& (_block_hit.type == blocks::dirt || _block_hit.type == blocks::grass_block)) { // special case, add but change block to farmland instead
		type = blocks::farmland;
		_chunk_hit->handleHit(true, type, _block_hit.pos, Modif::replace);
		return ;
	} else if (type >= blocks::wooden_shovel && type <= blocks::diamond_shovel
		&& (_block_hit.type == blocks::dirt || _block_hit.type == blocks::grass_block)) { // special case, add but change block to farmland instead
		type = blocks::dirt_path;
		_chunk_hit->handleHit(true, type, _block_hit.pos, Modif::replace);
		return ;
	} else if (type == blocks::zombie_egg) {
		_chunk_hit->handleHit(collect, type, _block_hit.prev_pos, Modif::use);
		return ;
	}
	if (_game_mode == settings::consts::gamemode::adventure && type != blocks::air) {
		int dist = glm::abs(_block_hit.pos.x - _block_hit.prev_pos.x) + glm::abs(_block_hit.pos.y - _block_hit.prev_pos.y) + glm::abs(_block_hit.pos.z - _block_hit.prev_pos.z);
		if (dist > 1) {
			return ;
		}
		if (_block_hit.type != s_blocks[type]->adventure_block) {
			if (type == blocks::bucket || type == blocks::water_bucket) {
				_ui->chatMessage("[Adventure mode] you can only use " + s_blocks[type]->name + " on " + s_blocks[s_blocks[type]->adventure_block]->name + ".", TEXT::RED);
				return ;
			}
			_ui->chatMessage("[Adventure mode] you can only place " + s_blocks[type]->name + " on " + s_blocks[s_blocks[type]->adventure_block]->name + ".", TEXT::RED);
			return ;
		}
	}
	// std::cout << "aiming " << s_blocks[type]->name << " towards " << s_blocks[_block_hit.type]->name << std::endl;;
	if (type == blocks::water_bucket) { // use it like any other block
		type = blocks::water;
	} else if (type == blocks::bucket) { // special case, add but remove water instead
		if (_block_hit.water_value) {
			_current_chunk_ptr->handleHit(collect, type, _block_hit.water_pos, Modif::rm);
		}
		return ;
	} else if (type == blocks::wheat_seeds) {
		type = blocks::wheat_crop;
	} else if (s_blocks[type]->isItemOnly) {
		// std::cout << "can't add block if no object in inventory" << std::endl;
		if (type == blocks::worldedit_wand) {
			return (WorldEdit::Get()->setSelectionEnd(_block_hit.pos));
		}
		return ;
	} else if (type == blocks::torch || type == blocks::redstone_torch) {
		if (_block_hit.pos.z != _block_hit.prev_pos.z) {
			type += (face_dir::minus_z << offset::blocks::orientation);
		} else if (_block_hit.pos.x != _block_hit.prev_pos.x) {
			type += (((_block_hit.pos.x > _block_hit.prev_pos.x) ? face_dir::plus_x : face_dir::minus_x) << offset::blocks::orientation);
		} else {
			type += (((_block_hit.pos.y > _block_hit.prev_pos.y) ? face_dir::plus_y : face_dir::minus_y) << offset::blocks::orientation);
		}
	} else if (type == blocks::oak_sign) {
		if (_block_hit.pos.x != _block_hit.prev_pos.x) {
			type += (((_block_hit.pos.x > _block_hit.prev_pos.x) ? face_dir::plus_x : face_dir::minus_x) << offset::blocks::orientation);
		} else if (_block_hit.pos.y != _block_hit.prev_pos.y) {
			type += (((_block_hit.pos.y > _block_hit.prev_pos.y) ? face_dir::plus_y : face_dir::minus_y) << offset::blocks::orientation);
		} else {
			return ;
		}
		_paused = true;
		_menu->setState(menu::sign);
		_menu->setSignPos(_block_hit.prev_pos);
	} else if (type == blocks::lever || shape == geometry::button) {
		if (s_blocks[_block_hit.type]->geometry != geometry::cube) { // TODO chnge this
			return ;
		}
		if (_block_hit.pos.z > _block_hit.prev_pos.z) {
			type += (placement::ceiling << offset::blocks::bitfield);
		} else if (_block_hit.pos.z < _block_hit.prev_pos.z) {
			type += (placement::floor << offset::blocks::bitfield);
		} else {
			type += (placement::wall << offset::blocks::bitfield);
			if (_block_hit.pos.x != _block_hit.prev_pos.x) {
				type += (((_block_hit.pos.x > _block_hit.prev_pos.x) ? face_dir::minus_x : face_dir::plus_x) << offset::blocks::orientation);
			} else {
				type += (((_block_hit.pos.y > _block_hit.prev_pos.y) ? face_dir::minus_y : face_dir::plus_y) << offset::blocks::orientation);
			}
		}
	} else if (shape == geometry::slab_bottom || shape == geometry::stairs_bottom) { // TODO get rid of oak_slab_top and oak_stairs_top and use door::upper_half instead
		if (_block_hit.pos.z != _block_hit.prev_pos.z) {
			type = ((_block_hit.pos.z < _block_hit.prev_pos.z) ? type : type + 1); // oak_slab_top = oak_slab_bottom + 1
		} else if (_block_hit.pos.x != _block_hit.prev_pos.x) {
			glm::vec3 p0 = _block_hit.pos + ((_block_hit.pos.x > _block_hit.prev_pos.x) ? glm::ivec3(0, 0, 0) : glm::ivec3(1, 0, 0));
			glm::vec3 intersect = line_plane_intersection(_camera->getEyePos(), _camera->getDir(), p0, {1, 0, 0});
			type = ((intersect.z - static_cast<int>(intersect.z) < 0.5f) ? type : type + 1);
			// _ui->chatMessage("block hit " + std::to_string(_block_hit.pos.x) + ", " + std::to_string(_block_hit.pos.y) + ", " + std::to_string(_block_hit.pos.z));
			// _ui->chatMessage("p0 at " + std::to_string(p0.x) + ", " + std::to_string(p0.y) + ", " + std::to_string(p0.z));
			// _ui->chatMessage("intersect at " + std::to_string(intersect.x) + ", " + std::to_string(intersect.y) + ", " + std::to_string(intersect.z));
		} else {
			glm::vec3 p0 = _block_hit.pos + ((_block_hit.pos.y > _block_hit.prev_pos.y) ? glm::ivec3(0, 0, 0) : glm::ivec3(0, 1, 0));
			glm::vec3 intersect = line_plane_intersection(_camera->getEyePos(), _camera->getDir(), p0, {0, 1, 0});
			type = ((intersect.z - static_cast<int>(intersect.z) < 0.5f) ? type : type + 1);
		}
	} else if (shape == geometry::trapdoor) {
		if (_block_hit.pos.z != _block_hit.prev_pos.z) {
			type += ((_block_hit.pos.z < _block_hit.prev_pos.z) ? 0 : (door::upper_half << 12));
		} else if (_block_hit.pos.x != _block_hit.prev_pos.x) {
			glm::vec3 p0 = _block_hit.pos + ((_block_hit.pos.x > _block_hit.prev_pos.x) ? glm::ivec3(0, 0, 0) : glm::ivec3(1, 0, 0));
			glm::vec3 intersect = line_plane_intersection(_camera->getEyePos(), _camera->getDir(), p0, {1, 0, 0});
			type += ((intersect.z - static_cast<int>(intersect.z) < 0.5f) ? 0 : (door::upper_half << 12));
			// _ui->chatMessage("block hit " + std::to_string(_block_hit.pos.x) + ", " + std::to_string(_block_hit.pos.y) + ", " + std::to_string(_block_hit.pos.z));
			// _ui->chatMessage("p0 at " + std::to_string(p0.x) + ", " + std::to_string(p0.y) + ", " + std::to_string(p0.z));
			// _ui->chatMessage("intersect at " + std::to_string(intersect.x) + ", " + std::to_string(intersect.y) + ", " + std::to_string(intersect.z));
		} else {
			glm::vec3 p0 = _block_hit.pos + ((_block_hit.pos.y > _block_hit.prev_pos.y) ? glm::ivec3(0, 0, 0) : glm::ivec3(0, 1, 0));
			glm::vec3 intersect = line_plane_intersection(_camera->getEyePos(), _camera->getDir(), p0, {0, 1, 0});
			type += ((intersect.z - static_cast<int>(intersect.z) < 0.5f) ? 0 : (door::upper_half << 12));
		}
	} else if (type == blocks::oak_log) {
		if (_block_hit.pos.z != _block_hit.prev_pos.z) {
			type += (face_dir::plus_z << offset::blocks::orientation);
		} else if (_block_hit.pos.x != _block_hit.prev_pos.x) {
			type += (face_dir::plus_x << offset::blocks::orientation);
		} else {
			type += (face_dir::plus_y << offset::blocks::orientation);
		}
	}

	if (_block_hit.type) { // rm if statement for nice cheat
		if (_game_mode == settings::consts::gamemode::adventure && type != blocks::water) {
			type |= mask::adventure_block;
		}
		_current_chunk_ptr->handleHit(collect, type, _block_hit.prev_pos, Modif::add);
	}
}

void OpenGL_Manager::updateCamView( void )
{
	glm::mat4 view = _camera->getViewMatrix();
	_skybox->updateCamView(view);
	glUniformMatrix4fv(_modelShader.getUniform(settings::consts::shader::uniform::view), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(_particleShader.getUniform(settings::consts::shader::uniform::view), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(_skyShader.getUniform(settings::consts::shader::uniform::view), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(_shader.getUniform(settings::consts::shader::uniform::view), 1, GL_FALSE, glm::value_ptr(view));

	// glUniform3fv(_uniCamPos, 1, glm::value_ptr(_camera->getPos()));
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
		if (_camera->chunkInFront(_current_chunk, peri->getStartX(), peri->getStartY())) {
			_visible_chunks.push_back(peri);
		}
	}
	mtx_perimeter.unlock();
}

void OpenGL_Manager::chunkUpdate( void )
{
	int posX = chunk_pos(static_cast<int>(glm::floor(_camera->getPos().x)));
	int posY = chunk_pos(static_cast<int>(glm::floor(_camera->getPos().y)));
	
	if (posX == _current_chunk.x && posY == _current_chunk.y) {
		return ;
	}
	// std::cout << "new chunk " << posX << ", " << posY << " !" << std::endl;
	_mtx.lock();
	_current_chunk = {posX, posY};
	_mtx.unlock();

	setThreadUpdate(true);
}

float OpenGL_Manager::getBreakTime( bool canHarvest )
{
	float speedMultiplier = 1;
	int needed_tool = s_blocks[_block_hit.type]->needed_tool;
	bool isBestTool = _hand_content >= needed_tool && _hand_content < needed_tool + 4;

	if (isBestTool) {
		speedMultiplier = (1 + _hand_content - needed_tool) << 1; // wood 2 stone 4 iron 6 diamond 8
		// if (toolEfficiency) { // will be used once enchantments are implemented
		// 	speedMultiplier += efficiencyLevel ^ 2 + 1
		// }
	}

	// if (hasteEffect) // will be used if beacons implemented
	// 	speedMultiplier *= 0.2 * hasteLevel + 1
	// if (miningFatigue) // will be used if implemented
	// 	speedMultiplier *= 0.3 ^ min(miningFatigueLevel, 4)
	if (_camera->isUnderwater()) { // && !hasAquaAffinity // will be used once implemented
		speedMultiplier /= 5;
	}
	if (!_camera->_touchGround) {
		speedMultiplier /= 5;
	}

	float damage = speedMultiplier / s_blocks[_block_hit.type]->hardness;
	if (damage < 0) { // bedrock and such
		return (FLT_MAX);
	}
	if (canHarvest) {
		damage /= 30;
	} else {
		damage /= 100;
	}

	// _ui->chatMessage(s_blocks[_block_hit.type]->name + " using " + s_blocks[_hand_content]->name);
	// Instant breaking
	if (damage > 1) {
		// _ui->chatMessage("\tINSTANT BREAK");
		return (0);
	}

	int ticks = glm::ceil(1 / damage);
	float seconds = ticks / 20.0f;
	// _ui->chatMessage(std::to_string(seconds) + " vs " + std::to_string(s_blocks[_block_hit.type]->getBreakTime(_hand_content)));
	return (seconds);
}

void OpenGL_Manager::userInputs( float deltaTime, bool rayCast )
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
	// toggle 'command' chat, id open chat, whit '/' already written
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
		_debug_mode = !_debug_mode;
	}
	// toggle game mode
	if (inputs::key_down(inputs::gamemode) && inputs::key_update(inputs::gamemode)) {
		if (inputs::key_down(inputs::debug)) {
			Settings::Get()->setBool(settings::bools::visible_chunk_border, !Settings::Get()->getBool(settings::bools::visible_chunk_border));
		} else {
			setGamemode(!_game_mode);
		}
	}
	// toggle F3 + I == display full info about block hit
	if (inputs::key_down(inputs::info) && inputs::key_update(inputs::info) && inputs::key_down(inputs::debug)) {
		t_hit bHit = getBlockHit();
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
		_camera->setHideUI(_ui->_hideUI);
		_ui->chatMessage(std::string("UI ") + ((_ui->_hideUI) ? "HIDDEN" : "SHOWN"));
	}
	// toggle outline
	if (inputs::key_down(inputs::block_highlight) && inputs::key_update(inputs::block_highlight)) {
		_ui->chatMessage(std::string("outlines ") + ((_outline) ? "HIDDEN" : "SHOWN"));
		_outline = !_outline;
		_break_frame = _outline;
	}
	// change time multiplier
	if (inputs::key_down(inputs::daycycle_up) && inputs::key_update(inputs::daycycle_up)) {
		DayCycle::Get()->updateTimeMultiplier(1);
		loadTextureShader(0, _textures[0], "Resources/Textures/cleanAtlas.png");
	} else if (inputs::key_down(inputs::daycycle_down) && inputs::key_update(inputs::daycycle_down)) {
		DayCycle::Get()->updateTimeMultiplier(-1);
		loadTextureShader(0, _textures[0], "Resources/Textures/blockAtlas.png");
	}

	// add and remove blocks
	_camera->setDelta(deltaTime);
	_hand_content = _inventory->getCurrentSlot();
	if (inputs::key_down(inputs::destroy)) {
		bool firstFrame = inputs::key_update(inputs::destroy);
		AMob* mobHit = NULL;
		if (firstFrame && _current_chunk_ptr) { // check if mob hit by punch
			mobHit = _current_chunk_ptr->mobHit(_block_hit);
		}
		if (mobHit) {
			_camera->setArmAnimation(true);
			mobHit->receiveDamage(15.0f, _camera->getEyePos());
			// TODO set _punch to true to avoid breaking block behind mob
		} else if (_game_mode != settings::consts::gamemode::creative) {
			if (_block_hit.type == blocks::air) {
				_camera->setArmAnimation(firstFrame);
			} else {
				_camera->setArmAnimation(true);
				_break_time += deltaTime;
				bool can_collect = s_blocks[_block_hit.type]->canCollect(_hand_content);
				float break_time = getBreakTime(can_collect);
				if (_break_time >= break_time) {
					_break_time = (break_time > 0) ? -0.3f : 0;
					_break_frame = _outline;
					handleBlockModif(false, can_collect);
					_camera->updateExhaustion(EXHAUSTION_BREAKING_BLOCK);
					_inventory->decrementDurabitilty();
				} else {
					int break_frame = static_cast<int>(10 * _break_time / break_time) + 2;
					if (_break_frame != break_frame) {
						if (_chunk_hit) {
							_chunk_hit->updateBreak({_block_hit.pos, _block_hit.type});
						}
						_break_frame = break_frame;
					}
				}
			}
		} else if (firstFrame) {
			handleBlockModif(false, false);
		}
	} else if (inputs::key_update(inputs::destroy)) {
		_camera->setArmAnimation(false);
		_break_time = 0;
		_break_frame = _outline;
	} else {
		_camera->setArmAnimation(false);
	}
	if (inputs::key_down(inputs::use)) {
		if (_game_mode != settings::consts::gamemode::creative && s_blocks[_hand_content]->isFood) {
			_eat_timer += deltaTime;
			if (_eat_timer >= 1.61f) {
				if (_camera->canEatFood(_hand_content)) {
					_inventory->removeBlock(false);
				}
				_eat_timer = 0;
			}
		} else if (_hand_content == blocks::bow) {
			_bow_timer += deltaTime;
		} else if (inputs::key_update(inputs::use)) {
			handleBlockModif(true, _game_mode != settings::consts::gamemode::creative);
			_camera->setArmAnimation(true);
		}
	} else if (inputs::key_update(inputs::use)) {
		if (_hand_content == blocks::bow && _bow_timer && _current_chunk_ptr) {
			// TODO rm arrow from inventory (once implemented)
			_inventory->decrementDurabitilty();
			_current_chunk_ptr->shootArrow(_bow_timer);
		}
		_eat_timer = 0;
		_bow_timer = 0;
	}
	// drop item
	if (inputs::key_down(inputs::drop)) {
		if (_current_chunk_ptr) {
			t_item details = _inventory->removeBlock(true);
			if (details.type != blocks::air) {
				mtx.lock();
				_current_chunk_ptr->dropEntity(_camera->getDir(), details);
				mtx.unlock();
			}
		}
	}
	if (_block_hit.type != blocks::air
		&& inputs::key_down(inputs::sample) && inputs::key_update(inputs::sample)) { // pick up in creative mode, or swap
		_inventory->replaceSlot(_block_hit.type, _game_mode == settings::consts::gamemode::creative);
	}

	// toggle polygon mode fill / lines
	if (inputs::key_down(inputs::wireframe) && inputs::key_update(inputs::wireframe)) {
		++_fill;
		if (_fill == F_LAST)
			_fill = FILL;
		// std::cout << "fill mode: " << _fill << std::endl;
		switch (_fill) {
			case FILL:
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				_ui->chatMessage("Triangles FILLED");
				break;
			case LINE:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				_ui->chatMessage("Triangles EMPTY");
				break;
		}
	}

	// camera work
	GLint key_cam_v = inputs::key_down(inputs::move_forwards) - inputs::key_down(inputs::move_backwards);
	GLint key_cam_h = inputs::key_down(inputs::move_right) - inputs::key_down(inputs::move_left);
	bool key_cam_zup = inputs::key_down(inputs::jump);
	bool key_cam_zdown = inputs::key_down(inputs::sneak);

	// this will be commented at some point
	GLint key_cam_yaw = inputs::key_down(inputs::look_left) - inputs::key_down(inputs::look_right);
	if (key_cam_yaw) {
		_camera->processYaw(key_cam_yaw * 5);
	}
	GLint key_cam_pitch = inputs::key_down(inputs::look_up) - inputs::key_down(inputs::look_down);
	if (key_cam_pitch) {
		_camera->processPitch(key_cam_pitch * 5);
	}

	if (!key_cam_v && !key_cam_h) {
		_camera->setRun(false);
	} else {
		_camera->setRun(inputs::key_down(inputs::run));
	}

	if (_game_mode == settings::consts::gamemode::creative) { // no collision check, free to move however you want
		_camera->moveFly(key_cam_v, key_cam_h, key_cam_zup - key_cam_zdown);
	}

	// we update the current chunk before we update cam view, because we check in current chunk for collision
	// update block hit
	if (rayCast) {
		if (_game_mode != settings::consts::gamemode::creative && _current_chunk_ptr) { // on first frame -> no _current_chunk_ptr
			_camera->setSneak(key_cam_zdown);
			_camera->setJump(key_cam_zup && inputs::key_update(inputs::jump));
			_camera->moveHuman(Z_AXIS, key_cam_v, key_cam_h, key_cam_zup - key_cam_zdown); // used for underwater movement
			glm::vec3 originalPos = _camera->getPos();
			_camera->moveHuman(X_AXIS, key_cam_v, key_cam_h, 0); // move on X_AXIS
			float hitBoxHeight = _camera->getHitBox();
			t_collision coll = _current_chunk_ptr->collisionBox(_camera->getPos(), 0.3f, hitBoxHeight, hitBoxHeight);
			if (coll.type != COLLISION::NONE) {
				// _ui->chatMessage("xcoll " + std::to_string(coll.type) + ", " + std::to_string(coll.minZ) + " ~ " + std::to_string(coll.maxZ) + " h " + std::to_string(hitBoxHeight));
				if (!_camera->customObstacle(coll.minZ, coll.maxZ)
					|| _current_chunk_ptr->collisionBox(_camera->getPos(), 0.3f, hitBoxHeight, hitBoxHeight).type != COLLISION::NONE) {
					_camera->unmoveHuman(originalPos); // if collision after movement, undo movement + setRun(false)
				}
			} else if (key_cam_zdown && _camera->_touchGround && _current_chunk_ptr->collisionBox(_camera->getPos() - glm::vec3(0, 0, 0.6f), 0.3f, 0.7f, 0.7f).type == COLLISION::NONE) {
				// if sneaking and touch ground and after move we have gap of more than 0.6 under our feet, undo movement
				_camera->unmoveHuman(originalPos);
			}
			originalPos = _camera->getPos();
			_camera->moveHuman(Y_AXIS, key_cam_v, key_cam_h, 0); // move on Y_AXIS
			coll = _current_chunk_ptr->collisionBox(_camera->getPos(), 0.3f, hitBoxHeight, hitBoxHeight);
			if (coll.type != COLLISION::NONE) {
				if (!_camera->customObstacle(coll.minZ, coll.maxZ)
					|| _current_chunk_ptr->collisionBox(_camera->getPos(), 0.3f, hitBoxHeight, hitBoxHeight).type != COLLISION::NONE) {
					_camera->unmoveHuman(originalPos);
				}
			} else if (key_cam_zdown && _camera->_touchGround && _current_chunk_ptr->collisionBox(_camera->getPos() - glm::vec3(0, 0, 0.6f), 0.3f, 0.7f, 0.7f).type == COLLISION::NONE) {
				_camera->unmoveHuman(originalPos);
			}
			_current_chunk_ptr->applyGravity(); // move on Z_AXIS
			_camera->setWaterStatus(false, _current_chunk_ptr->collisionBoxWater(_camera->getPos(), 0.3f, 0));
			_camera->setWaterStatus(true, _current_chunk_ptr->collisionBoxWater(_camera->getEyePos(), 0.05f, 0));
		}

		// Chunk *save_chunk = _chunk_hit;
		t_hit block_hit = getBlockHit();
		if (_block_hit.pos != block_hit.pos) {
			_break_time = 0;
			_break_frame = _outline;
		}
		_block_hit = block_hit;

		if (!_camera->_health_points && _current_chunk_ptr) { // dead
			_inventory->spillInventory(_current_chunk_ptr);
			_paused = true;
			_menu->setState(menu::death);
			return ;
		}
	}
	_camera->updateCurrentBlock();

	if (_camera->_fovUpdate) {
		_camera->_fovUpdate = false;
		updateCamPerspective();
	}
	if (_camera->_update) {
		updateCamView();
		updateVisibleChunks();
	}

	GLint key_cam_speed = inputs::key_down(inputs::fly_speed_up) - inputs::key_down(inputs::fly_speed_down);
	if (key_cam_speed) {
		_camera->update_movement_speed(key_cam_speed);
	}

	// inventory slot selection
	if (inputs::key_down(inputs::slot_0) && inputs::key_update(inputs::slot_0)) {
		_inventory->setSlot(0);
	}
	if (inputs::key_down(inputs::slot_1) && inputs::key_update(inputs::slot_1)) {
		_inventory->setSlot(1);
	}
	if (inputs::key_down(inputs::slot_2) && inputs::key_update(inputs::slot_2)) {
		_inventory->setSlot(2);
	}
	if (inputs::key_down(inputs::slot_3) && inputs::key_update(inputs::slot_3)) {
		_inventory->setSlot(3);
	}
	if (inputs::key_down(inputs::slot_4) && inputs::key_update(inputs::slot_4)) {
		_inventory->setSlot(4);
	}
	if (inputs::key_down(inputs::slot_5) && inputs::key_update(inputs::slot_5)) {
		_inventory->setSlot(5);
	}
	if (inputs::key_down(inputs::slot_6) && inputs::key_update(inputs::slot_6)) {
		_inventory->setSlot(6);
	}
	if (inputs::key_down(inputs::slot_7) && inputs::key_update(inputs::slot_7)) {
		_inventory->setSlot(7);
	}
	if (inputs::key_down(inputs::slot_8) && inputs::key_update(inputs::slot_8)) {
		_inventory->setSlot(8);
	}
}
