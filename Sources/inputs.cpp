#include "OpenGL_Manager.hpp"
#include "callbacks.hpp"
#include "utils.h"
#include <iostream>

Chunk *current_chunk_ptr = NULL;
Chunk *chunk_hit = NULL;

void OpenGL_Manager::resetInputsPtrs( void )
{
	current_chunk_ptr = NULL;
	chunk_hit = NULL;
	_perimeter_chunks.clear();
	_visible_chunks.clear();
}

t_hit OpenGL_Manager::get_block_hit( void )
{
	t_hit res = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 0, 0};
	std::vector<glm::ivec3> ids = _camera->get_ray_casting((_game_mode == CREATIVE) ? (_render_distance << CHUNK_SHIFT) >> 1 : REACH);

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
					update_visible_chunks();
				} else {
					std::cout << "chunk out of bound at " << posX << ", " << posY << std::endl;
				}
				return (res);
			}
		}
		if (first_loop) {
			current_chunk_ptr = chunk;
			_camera->setCurrentChunkPtr(chunk);
			first_loop = false;
		}
		// std::cout << "current_chunk should be " << current_chunk.x << ", " << current_chunk.y << std::endl;
		int value = chunk->isHit(i); // TODO restore &0xFF in isHit()
		// if ((value & 0xFF) == blocks::OAK_STAIRS_BOTTOM) {
		// 	_ui->chatMessage("hit stair MM " + std::to_string((value >> 12) & CORNERS::MM)
		// 		+ " MP " + std::to_string((value >> 12) & CORNERS::MP)
		// 		+ " PM " + std::to_string((value >> 12) & CORNERS::PM)
		// 		+ " PP " + std::to_string((value >> 12) & CORNERS::PP));
		// }
		value &= 0xFF;
		if (value == blocks::WATER) {
			if (!res.water_value) {
				res.water_pos = i;
				res.water_value = true;
			}
			res.prev_pos = i;
		} else if (value) {
			// we know cube is hit, now check if hitbox is hit (only on non cube-filling values)
			// TODO move hitboxes of torches depending on the wall they hang on
			if (!s_blocks[value]->hasHitbox || line_cube_intersection(_camera->getEyePos(), _camera->getDir(), glm::vec3(i) + s_blocks[value]->hitboxCenter, s_blocks[value]->hitboxHalfSize)) {
				chunk_hit = chunk;
				res.pos = i;
				res.value = value;
				return (res);
			}
		} else {
			res.prev_pos = i;
		}
	}
	return (res);
}

void OpenGL_Manager::handle_add_rm_block( bool adding, bool collect )
{
	if (!adding) {
		if (_block_hit.value == blocks::AIR) {
			return ;
		}
		if (chunk_hit) {
			chunk_hit->handleHit(collect, 0, _block_hit.pos, Modif::REMOVE);
			return ;
		}
		int posX = chunk_pos(_block_hit.pos.x);
		int posY = chunk_pos(_block_hit.pos.y);

		std::cout << "rm: chunk out of bound at " << posX << ", " << posY << std::endl;
		return ;
	}
	// from now on adding = true
	if (_block_hit.value == blocks::CRAFTING_TABLE) {
		_paused = true;
		_menu->setState(MENU::CRAFTING);
		return ;
	} else if (_block_hit.value == blocks::CHEST) {
		if (air_flower(chunk_hit->getBlockAt(_block_hit.pos.x - chunk_hit->getStartX(), _block_hit.pos.y - chunk_hit->getStartY(), _block_hit.pos.z + 1, true), true, false, false)) {
			return ;
		}
		chunk_hit->openChest(_block_hit.pos);
		_paused = true;
		_menu->setState(MENU::CHEST);
		_menu->setChestInstance(chunk_hit->getChestInstance(_block_hit.pos));
		return ;
	} else if (_block_hit.value == blocks::FURNACE) {
		_paused = true;
		_menu->setState(MENU::FURNACE);
		_menu->setFurnaceInstance(chunk_hit->getFurnaceInstance(_block_hit.pos));
		return ;
	} else if (_block_hit.value == blocks::TNT && _hand_content == blocks::FLINT_AND_STEEL) {
		if (current_chunk_ptr) {
			current_chunk_ptr->handleHit(false, blocks::TNT, _block_hit.pos, Modif::LITNT);
		}
		return ;
	}
	int type = _hand_content;
	// std::cout << "aiming " << s_blocks[type]->name << " towards " << s_blocks[_block_hit.value]->name << std::endl;;
	if (type == blocks::WATER_BUCKET) { // use it like any other block
		type = blocks::WATER;
	} else if (type == blocks::BUCKET) { // special case, add but remove water instead
		if (_block_hit.water_value) {
			current_chunk_ptr->handleHit(collect, type, _block_hit.water_pos, Modif::REMOVE);
		}
		return ;
	} else if (type >= blocks::WOODEN_HOE && type <= blocks::DIAMOND_HOE
		&& (_block_hit.value == blocks::DIRT || _block_hit.value == blocks::GRASS_BLOCK)) { // special case, add but change block to farmland instead
		type = blocks::FARMLAND;
		chunk_hit->handleHit(true, type, _block_hit.pos, Modif::REPLACE);
		return ;
	} else if (type >= blocks::WOODEN_SHOVEL && type <= blocks::DIAMOND_SHOVEL
		&& (_block_hit.value == blocks::DIRT || _block_hit.value == blocks::GRASS_BLOCK)) { // special case, add but change block to farmland instead
		type = blocks::DIRT_PATH;
		chunk_hit->handleHit(true, type, _block_hit.pos, Modif::REPLACE);
		return ;
	} else if (type == blocks::WHEAT_SEEDS) {
		type = blocks::WHEAT_CROP;
	} else if (type == blocks::AIR || type >= blocks::STICK) {
		// std::cout << "can't add block if no object in inventory" << std::endl;
		return ;
	} else if (type == blocks::TORCH) {
		if (_block_hit.pos.z != _block_hit.prev_pos.z) {
			type += (face_dir::MINUSZ << 9);
		} else if (_block_hit.pos.x != _block_hit.prev_pos.x) {
			type += (((_block_hit.pos.x > _block_hit.prev_pos.x) ? face_dir::PLUSX : face_dir::MINUSX) << 9);
		} else {
			type += (((_block_hit.pos.y > _block_hit.prev_pos.y) ? face_dir::PLUSY : face_dir::MINUSY) << 9);
		}
	} else if (type == blocks::OAK_SLAB || type == blocks::OAK_STAIRS) {
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
	}

	if (_block_hit.value) { // rm if statement for nice cheat
		current_chunk_ptr->handleHit(collect, type, _block_hit.prev_pos, Modif::ADD);
	}
}

void OpenGL_Manager::update_cam_view( void )
{
	glm::mat4 view = _camera->getViewMatrix();
	glUseProgram(_skyShaderProgram);
	glUniformMatrix4fv(_skyUniView, 1, GL_FALSE, glm::value_ptr(view));
	glUseProgram(_particleShaderProgram);
	glUniformMatrix4fv(_partUniView, 1, GL_FALSE, glm::value_ptr(view));
	glUseProgram(_shaderProgram);
	glUniformMatrix4fv(_uniView, 1, GL_FALSE, glm::value_ptr(view));

	// glUniform3fv(_uniCamPos, 1, glm::value_ptr(_camera->getPos()));
}

void OpenGL_Manager::update_cam_perspective( void )
{
	glm::mat4 proj = _camera->getPerspectiveMatrix();
	glUseProgram(_skyShaderProgram);
	glUniformMatrix4fv(_skyUniProj, 1, GL_FALSE, glm::value_ptr(proj));
	glUseProgram(_particleShaderProgram);
	glUniformMatrix4fv(_partUniProj, 1, GL_FALSE, glm::value_ptr(proj));
	glUseProgram(_shaderProgram);
	glUniformMatrix4fv(_uniProj, 1, GL_FALSE, glm::value_ptr(proj));
}

void OpenGL_Manager::update_anim_frame( void )
{
	static int current_frame;
	glUniform1i(_skyUniAnim, current_frame);
	if (++current_frame == 31) {
		current_frame = 0;
	}
}

void OpenGL_Manager::update_visible_chunks( void )
{
	std::vector<Chunk *> newvis_chunks;
	newvis_chunks.reserve(_visible_chunks.capacity());
	mtx_perimeter.lock();
	for (auto& peri: _perimeter_chunks) {
		if (_camera->chunkInFront(_current_chunk, peri->getStartX(), peri->getStartY())) {
			peri->show();
			newvis_chunks.push_back(peri);
		} else {
			peri->hide();
		}
	}
	mtx_perimeter.unlock();
	_visible_chunks = newvis_chunks;
}

void OpenGL_Manager::chunk_update( void )
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
	int needed_tool = s_blocks[_block_hit.value]->needed_tool;
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

	float damage = speedMultiplier / s_blocks[_block_hit.value]->hardness;
	if (damage < 0) { // bedrock and such
		return (FLT_MAX);
	}
	if (canHarvest) {
		damage /= 30;
	} else {
		damage /= 100;
	}

	// _ui->chatMessage(s_blocks[_block_hit.value]->name + " using " + s_blocks[_hand_content]->name);
	// Instant breaking
	if (damage > 1) {
		// _ui->chatMessage("\tINSTANT BREAK");
		return (0);
	}

	int ticks = glm::ceil(1 / damage);
	float seconds = ticks / 20.0f;
	// _ui->chatMessage(std::to_string(seconds) + " vs " + std::to_string(s_blocks[_block_hit.value]->getBreakTime(_hand_content)));
	return (seconds);
}

void OpenGL_Manager::user_inputs( float deltaTime, bool rayCast )
{
	// open menu
	if (INPUT::key_down(INPUT::CLOSE) && INPUT::key_update(INPUT::CLOSE)) {
		_paused = true;
		_menu->setState(MENU::PAUSE);
		return ;
	}
	// open inventory
	if (INPUT::key_down(INPUT::INVENTORY) && INPUT::key_update(INPUT::INVENTORY)) {
		_paused = true;
		_menu->setState(MENU::INVENTORY);
		return ;
	}
	// toggle chat
	if (INPUT::key_down(INPUT::CHAT) && INPUT::key_update(INPUT::CHAT)) {
		_paused = true;
		_menu->setState(MENU::CHAT);
		return ;
	}
	// quit program
	if (INPUT::key_down(INPUT::QUIT_PROGRAM)) {
		glfwSetWindowShouldClose(_window, GL_TRUE);
		return ;
	}

	// take screenshot
	if (INPUT::key_down(INPUT::SCREENSHOT) && INPUT::key_update(INPUT::SCREENSHOT)) {
		screenshot();
	}
	// toggle debug mode on off F3
	if (INPUT::key_down(INPUT::DEBUG) && INPUT::key_update(INPUT::DEBUG)) {
		_debug_mode = !_debug_mode;
	}
	// toggle game mode
	if (INPUT::key_down(INPUT::GAMEMODE) && INPUT::key_update(INPUT::GAMEMODE)) {
		setGamemode(!_game_mode);
	}
	// toggle F5 mode
	if (INPUT::key_down(INPUT::CAMERA) && INPUT::key_update(INPUT::CAMERA)) {
		_camera->changeCamPlacement();
	}
	// toggle hotbar F1
	if (INPUT::key_down(INPUT::HOTBAR) && INPUT::key_update(INPUT::HOTBAR)) {
		_ui->_hideUI = !_ui->_hideUI;
		_camera->setHideUI(_ui->_hideUI);
		_ui->chatMessage(std::string("UI ") + ((_ui->_hideUI) ? "HIDDEN" : "SHOWN"));
	}
	// toggle outline
	if (INPUT::key_down(INPUT::BLOCK_HIGHLIGHT) && INPUT::key_update(INPUT::BLOCK_HIGHLIGHT)) {
		_ui->chatMessage(std::string("outlines ") + ((_outline) ? "HIDDEN" : "SHOWN"));
		_outline = !_outline;
		if (_break_frame != _outline && chunk_hit) {
			chunk_hit->updateBreak({_block_hit.pos, _block_hit.value}, _outline);
		}
		_break_frame = _outline;
	}
	// change time multiplier
	if (INPUT::key_down(INPUT::DAYCYCLE_UP) && INPUT::key_update(INPUT::DAYCYCLE_UP)) {
		DayCycle::Get()->updateTimeMultiplier(1);
		loadTextureShader(0, _textures[0], "Resources/cleanAtlas.png");
	} else if (INPUT::key_down(INPUT::DAYCYCLE_DOWN) && INPUT::key_update(INPUT::DAYCYCLE_DOWN)) {
		DayCycle::Get()->updateTimeMultiplier(-1);
		loadTextureShader(0, _textures[0], "Resources/blockAtlas.png");
	}

	// add and remove blocks
	_camera->setDelta(deltaTime);
	_hand_content = _inventory->getCurrentSlot();
	if (INPUT::key_down(INPUT::BREAK)) {
		if (_game_mode == SURVIVAL) {
			_camera->setArmAnimation(true);
			_break_time += deltaTime;
			bool can_collect = s_blocks[_block_hit.value]->canCollect(_hand_content);
			float break_time = getBreakTime(can_collect);
			if (_block_hit.value != blocks::AIR && _break_time >= break_time) {
				_break_time = (break_time > 0) ? -0.3f : 0;
				_break_frame = _outline;
				handle_add_rm_block(false, can_collect);
				_camera->updateExhaustion(EXHAUSTION_BREAKING_BLOCK);
				_inventory->decrementDurabitilty();
			} else {
				int break_frame = static_cast<int>(10 * _break_time / break_time) + 2;
				if (_break_frame != break_frame) {
					if (chunk_hit) {
						chunk_hit->updateBreak({_block_hit.pos, _block_hit.value}, break_frame);
					}
					_break_frame = break_frame;
				}
			}
		} else if (INPUT::key_update(INPUT::BREAK)) {
			handle_add_rm_block(false, false);
		}
	} else if (INPUT::key_update(INPUT::BREAK)) {
		_camera->setArmAnimation(false);
		_break_time = 0;
		if (_break_frame != _outline && chunk_hit) {
			chunk_hit->updateBreak({_block_hit.pos, _block_hit.value}, _outline);
		}
		_break_frame = _outline;
	} else {
		_camera->setArmAnimation(false);
	}
	if (INPUT::key_down(INPUT::USE)) {
		if (_game_mode == SURVIVAL && s_blocks[_hand_content]->isFood) {
			_eat_timer += deltaTime;
			if (_eat_timer >= 1.61f) {
				if (_camera->canEatFood(_hand_content)) {
					_inventory->removeBlock(false);
				}
				_eat_timer = 0;
			}
		} else if (_hand_content == blocks::BOW) {
			_bow_timer += deltaTime;
		} else if (INPUT::key_update(INPUT::USE)) {
			handle_add_rm_block(true, _game_mode == SURVIVAL);
			_camera->setArmAnimation(true);
		}
	} else if (INPUT::key_update(INPUT::USE)) {
		if (_hand_content == blocks::BOW && _bow_timer && current_chunk_ptr) {
			// TODO rm arrow from inventory (once implemented)
			_inventory->decrementDurabitilty();
			current_chunk_ptr->shootArrow(_bow_timer);
		}
		_eat_timer = 0;
		_bow_timer = 0;
	}
	// drop item
	if (INPUT::key_down(INPUT::DROP)) {
		if (current_chunk_ptr) {
			t_item details = _inventory->removeBlock(true);
			if (details.type != blocks::AIR) {
				mtx.lock();
				current_chunk_ptr->dropEntity(_camera->getDir(), details);
				mtx.unlock();
			}
		}
	}
	if (_block_hit.value != blocks::AIR
		&& INPUT::key_down(INPUT::SAMPLE) && INPUT::key_update(INPUT::SAMPLE)) { // pick up in creative mode, or swap
		_inventory->replaceSlot(_block_hit.value, _game_mode == CREATIVE);
	}

	// toggle polygon mode fill / lines
	if (INPUT::key_down(INPUT::WIREFRAME) && INPUT::key_update(INPUT::WIREFRAME)) {
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
	// change render dist
	GLint key_render_dist = (INPUT::key_down(INPUT::RENDER_DIST_UP) && INPUT::key_update(INPUT::RENDER_DIST_UP))
		- (INPUT::key_down(INPUT::RENDER_DIST_DOWN) && INPUT::key_update(INPUT::RENDER_DIST_DOWN));
	if (key_render_dist && _render_distance + key_render_dist > 0) {
		_mtx.lock();
		_render_distance += key_render_dist;
		_mtx.unlock();
		_ui->chatMessage("Render distance set to " + std::to_string(_render_distance));
		glUniform1f(_uniFog, (1 + _render_distance) << CHUNK_SHIFT);
		glUseProgram(_skyShaderProgram);
		glUniform1f(_skyUniFog, (1 + _render_distance) << CHUNK_SHIFT);
		glUseProgram(_shaderProgram);
		setThreadUpdate(true);
		// update_visible_chunks();
		// std::cout << "render distance set to " << _render_distance << std::endl;
	}
	// change gui size
	GLint key_gui_size = (INPUT::key_down(INPUT::GUI_UP) && INPUT::key_update(INPUT::GUI_UP))
		- (INPUT::key_down(INPUT::GUI_DOWN) && INPUT::key_update(INPUT::GUI_DOWN));
	if (key_gui_size) {
		_menu->changeGuiSize(key_gui_size);
		_ui->changeGuiSize(key_gui_size);
	}

	// camera work
	GLint key_cam_v = INPUT::key_down(INPUT::MOVE_FORWARD) - INPUT::key_down(INPUT::MOVE_BACKWARD);
	GLint key_cam_h = INPUT::key_down(INPUT::MOVE_RIGHT) - INPUT::key_down(INPUT::MOVE_LEFT);
	bool key_cam_zup = INPUT::key_down(INPUT::JUMP);
	bool key_cam_zdown = INPUT::key_down(INPUT::SNEAK);

	// this will be commented at some point
	GLint key_cam_yaw = INPUT::key_down(INPUT::LOOK_LEFT) - INPUT::key_down(INPUT::LOOK_RIGHT);
	if (key_cam_yaw) {
		_camera->processYaw(key_cam_yaw * 5);
	}
	GLint key_cam_pitch = INPUT::key_down(INPUT::LOOK_UP) - INPUT::key_down(INPUT::LOOK_DOWN);
	if (key_cam_pitch) {
		_camera->processPitch(key_cam_pitch * 5);
	}

	if (!key_cam_v && !key_cam_h) {
		_camera->setRun(false);
	} else {
		_camera->setRun(INPUT::key_down(INPUT::RUN));
	}

	if (_game_mode == CREATIVE) { // no collision check, free to move however you want
		_camera->moveFly(key_cam_v, key_cam_h, key_cam_zup - key_cam_zdown);
	}

	// we update the current chunk before we update cam view, because we check in current chunk for collision
	// update block hit
	if (rayCast) {
		if (_game_mode == SURVIVAL && current_chunk_ptr) { // on first frame -> no current_chunk_ptr
			_camera->setSneak(key_cam_zdown);
			_camera->setJump(key_cam_zup && INPUT::key_update(INPUT::JUMP));
			_camera->moveHuman(Z_AXIS, key_cam_v, key_cam_h, key_cam_zup - key_cam_zdown); // used for underwater movement
			glm::vec3 originalPos = _camera->getPos();
			_camera->moveHuman(X_AXIS, key_cam_v, key_cam_h, 0); // move on X_AXIS
			float hitBoxHeight = _camera->getHitBox();
			mtx.lock();
			t_collision coll = current_chunk_ptr->collisionBox(_camera->getPos(), 0.3f, hitBoxHeight, hitBoxHeight);
			mtx.unlock();
			// _ui->chatMessage("coll " + std::to_string(coll.type) + ", " + std::to_string(coll.minZ) + " ~ " + std::to_string(coll.maxZ) + " h " + std::to_string(hitBoxHeight));
			if (coll.type == COLLISION::TOTAL) {
				if (!_camera->customObstacle(coll.minZ, coll.maxZ)) {
					_camera->unmoveHuman(originalPos); // if collision after movement, undo movement
					_camera->setRun(false);
				}
			} else if (coll.type == COLLISION::PARTIAL) {
				_camera->customObstacle(coll.minZ, coll.maxZ);
				coll = current_chunk_ptr->collisionBox(_camera->getPos(), 0.3f, hitBoxHeight, hitBoxHeight);
				if (coll.type != COLLISION::NONE) {
					_camera->unmoveHuman(originalPos); // if collision after slab correction, undo movement
					_camera->setRun(false);
				}
			}
			originalPos = _camera->getPos();
			_camera->moveHuman(Y_AXIS, key_cam_v, key_cam_h, 0); // move on Y_AXIS
			mtx.lock();
			coll = current_chunk_ptr->collisionBox(_camera->getPos(), 0.3f, hitBoxHeight, hitBoxHeight);
			mtx.unlock();
			if (coll.type == COLLISION::TOTAL) {
				if (!_camera->customObstacle(coll.minZ, coll.maxZ)) {
					_camera->unmoveHuman(originalPos); // if collision after movement, undo movement
					_camera->setRun(false);
				}
			} else if (coll.type == COLLISION::PARTIAL) {
				_camera->customObstacle(coll.minZ, coll.maxZ);
				coll = current_chunk_ptr->collisionBox(_camera->getPos(), 0.3f, hitBoxHeight, hitBoxHeight);
				if (coll.type != COLLISION::NONE) {
					_camera->unmoveHuman(originalPos); // if collision after slab correction, undo movement
					_camera->setRun(false);
				}
			}
			mtx.lock();
			current_chunk_ptr->applyGravity(); // move on Z_AXIS
			mtx.unlock();
			glm::vec3 camPos = _camera->getPos();
			_camera->setWaterStatus(false, current_chunk_ptr->collisionBoxWater(camPos, 0.3f, 0));
			camPos = _camera->getEyePos();
			_camera->setWaterStatus(true, current_chunk_ptr->collisionBoxWater(camPos, 0.05f, 0));
		}

		Chunk *save_chunk = chunk_hit;
		t_hit block_hit = get_block_hit();
		if (_block_hit.pos != block_hit.pos) {
			if (_break_frame && save_chunk) {
				save_chunk->updateBreak({_block_hit.pos, _block_hit.value}, 0);
			}
			if (_outline && chunk_hit) {
				chunk_hit->updateBreak({block_hit.pos, block_hit.value}, 1);
			}
			_break_time = 0;
			_break_frame = _outline;
		}
		_block_hit = block_hit;

		if (!_camera->_health_points && current_chunk_ptr) { // dead
			_inventory->spillInventory(current_chunk_ptr);
			_paused = true;
			_menu->setState(MENU::DEATH);
			return ;
		}
	}
	glm::vec3 camPos = _camera->getEyePos();
	glm::ivec3 current_block = {glm::floor(camPos.x), glm::floor(camPos.y), glm::floor(camPos.z)};
	if (current_block != _camera->_current_block) {
		_camera->_current_block = current_block;
		if (current_chunk_ptr) {
			current_chunk_ptr->sort_sky(camPos, true);
			current_chunk_ptr->sort_water(camPos, true);
		}
	}

	if (_camera->_fovUpdate) {
		_camera->_fovUpdate = false;
		update_cam_perspective();
	}
	if (_camera->_update) {
		update_cam_view();
		update_visible_chunks();
	}

	GLint key_cam_speed = INPUT::key_down(INPUT::FLY_SPEED_UP) - INPUT::key_down(INPUT::FLY_SPEED_DOWN);
	if (key_cam_speed) {
		_camera->update_movement_speed(key_cam_speed);
	}

	// inventory slot selection
	if (INPUT::key_down(INPUT::SLOT_0) && INPUT::key_update(INPUT::SLOT_0)) {
		_inventory->setSlot(0);
	}
	if (INPUT::key_down(INPUT::SLOT_1) && INPUT::key_update(INPUT::SLOT_1)) {
		_inventory->setSlot(1);
	}
	if (INPUT::key_down(INPUT::SLOT_2) && INPUT::key_update(INPUT::SLOT_2)) {
		_inventory->setSlot(2);
	}
	if (INPUT::key_down(INPUT::SLOT_3) && INPUT::key_update(INPUT::SLOT_3)) {
		_inventory->setSlot(3);
	}
	if (INPUT::key_down(INPUT::SLOT_4) && INPUT::key_update(INPUT::SLOT_4)) {
		_inventory->setSlot(4);
	}
	if (INPUT::key_down(INPUT::SLOT_5) && INPUT::key_update(INPUT::SLOT_5)) {
		_inventory->setSlot(5);
	}
	if (INPUT::key_down(INPUT::SLOT_6) && INPUT::key_update(INPUT::SLOT_6)) {
		_inventory->setSlot(6);
	}
	if (INPUT::key_down(INPUT::SLOT_7) && INPUT::key_update(INPUT::SLOT_7)) {
		_inventory->setSlot(7);
	}
	if (INPUT::key_down(INPUT::SLOT_8) && INPUT::key_update(INPUT::SLOT_8)) {
		_inventory->setSlot(8);
	}
}
