#include "OpenGL_Manager.hpp"
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
		int value = chunk->isHit(i);
		if (value == blocks::WATER) {
			if (!res.water_value) {
				res.water_pos = i;
				res.water_value = true;
			}
			res.prev_pos = i;
		} else if (value) {
			// we know cube is hit, now check if hitbox is hit (only on non cube-filling values)
			// TODO move hitboxes of torches depending on the wall they hang on
			if (!s_blocks[value]->hasHitbox || line_cube_intersection(_camera->getPos() + glm::vec3(0, 0, 1 + EYE_LEVEL), _camera->getDir(), glm::vec3(i) + s_blocks[value]->hitboxCenter, s_blocks[value]->hitboxHalfSize)) {
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
		
		// for (auto& c : _visible_chunks) {
		// 	if (c->isInChunk(posX, posY)) {
		// 		// std::cout << "handle hit at pos " << _block_hit.pos.x << ", " << _block_hit.pos.y << ", " << _block_hit.pos.z << std::endl;
		// 		c->handleHit(collect, 0, glm::ivec3(_block_hit.pos.x, _block_hit.pos.y, _block_hit.pos.z), Modif::REMOVE);
		// 		return ;
		// 	}
		// }
		std::cout << "rm: chunk out of bound at " << posX << ", " << posY << std::endl;
		return ;
	}
	// from now on adding = true
	if (_block_hit.value == blocks::CRAFTING_TABLE) {
		_paused = true;
		_esc_released = false;
		_e_released = false;
		_menu->setState(CRAFTING_MENU);
		return ;
	} else if (_block_hit.value == blocks::CHEST) {
		chunk_hit->openChest(_block_hit.pos);
		_paused = true;
		_esc_released = false;
		_e_released = false;
		_menu->setState(CHEST_MENU);
		_menu->setChestInstance(chunk_hit->getChestInstance(_block_hit.pos));
		return ;
	} else if (_block_hit.value == blocks::FURNACE) {
		_paused = true;
		_esc_released = false;
		_e_released = false;
		_menu->setState(FURNACE_MENU);
		_menu->setFurnaceInstance(chunk_hit->getFurnaceInstance(_block_hit.pos));
		return ;
	} else if (_block_hit.value == blocks::TNT && _hand_content == blocks::WHEAT_SEEDS) { // TODO replace seeds with flint and steel
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
	}

	if (_block_hit.value) { // rm if statement for nice cheat
		current_chunk_ptr->handleHit(collect, type, _block_hit.prev_pos, Modif::ADD);
	}
}

void OpenGL_Manager::update_cam_view( void )
{
	glm::mat4 view = _camera->getViewMatrix();
	glUniformMatrix4fv(_uniView, 1, GL_FALSE, glm::value_ptr(view));
	glUseProgram(_skyShaderProgram);
	glUniformMatrix4fv(_skyUniView, 1, GL_FALSE, glm::value_ptr(view));
	glUseProgram(_particleShaderProgram);
	glUniformMatrix4fv(_partUniView, 1, GL_FALSE, glm::value_ptr(view));
	glUseProgram(_shaderProgram);

	// glUniform3fv(_uniCamPos, 1, glm::value_ptr(_camera->getPos()));
}

void OpenGL_Manager::update_cam_perspective( void )
{
	glm::mat4 proj = _camera->getPerspectiveMatrix();
	glUniformMatrix4fv(_uniProj, 1, GL_FALSE, glm::value_ptr(proj));
	glUseProgram(_skyShaderProgram);
	glUniformMatrix4fv(_skyUniProj, 1, GL_FALSE, glm::value_ptr(proj));
	glUseProgram(_particleShaderProgram);
	glUniformMatrix4fv(_partUniProj, 1, GL_FALSE, glm::value_ptr(proj));
	glUseProgram(_shaderProgram);
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

static void thread_chunk_update( OpenGL_Manager *render, GLint render_dist, int posX, int posY )
{
	// Bench b;
	std::set<std::pair<int, int>> coords;
	for (int row = -render_dist; row <= render_dist; row++) {
		for (int col = -render_dist; col <= render_dist; col++) {
			coords.insert({posX + (row << CHUNK_SHIFT), posY + (col << CHUNK_SHIFT)});
		}
	}
	// b.stamp("gen coordinates set");

	std::vector<Chunk *> newperi_chunks;
	newperi_chunks.reserve(render->_perimeter_chunks.capacity());
	std::vector<Chunk *> newdel_chunks;
	newdel_chunks.reserve(render->_deleted_chunks.capacity());
	mtx.lock();
	std::list<Chunk *>::iterator ite = render->_chunks.end();
	std::list<Chunk *>::iterator it = render->_chunks.begin();
	mtx.unlock();
	// std::cout << "IN THREAD UPDATE, nb chunks: " << render->_chunks.size() << std::endl;
	for (; it != ite;) {
		mtx.lock();
		if ((*it)->inPerimeter(posX, posY, render_dist << CHUNK_SHIFT)) {
			// std::cout << "IN PERIMETER" << std::endl;
			(*it)->checkFillVertices();
			newperi_chunks.push_back(*it);
			coords.erase({(*it)->getStartX(), (*it)->getStartY()});
		} else if (!(*it)->inPerimeter(posX, posY, (render_dist << CHUNK_SHIFT) * 2)) {
			std::list<Chunk *>::iterator tmp = it;
			--it;
			mtx.unlock();
			(*tmp)->setBackup(render->_backups);
			newdel_chunks.push_back(*tmp);
			mtx.lock();
			render->_chunks.erase(tmp);
		}
		++it;
		mtx.unlock();
	}
	// b.stamp("delperi");
	sort_chunks(render->_camera->getPos(), newperi_chunks);
	// b.stamp("sort chunks");
	mtx_perimeter.lock();
	render->_perimeter_chunks = newperi_chunks;
	mtx_perimeter.unlock();
	mtx_deleted_chunks.lock();
	render->_deleted_chunks = newdel_chunks;
	mtx_deleted_chunks.unlock();

	// b.stamp("NO");
	for (auto& c: coords) {
		//create new chunk where player stands
		Chunk *newChunk = new Chunk(render->_camera, render->_inventory, c.first, c.second, &render->_chunks);
		mtx_backup.lock();
		std::map<std::pair<int, int>, s_backup>::iterator search = render->_backups.find(std::pair<int, int>(c.first, c.second));
		if (search != render->_backups.end()) {
			newChunk->restoreBackup(search->second);
			render->_backups.erase(search);
		}
		mtx_backup.unlock();
		mtx.lock();
		render->_chunks.push_back(newChunk);
		mtx.unlock();
		newChunk->generate_chunk(); // TODO remove this from thread because it launches its own thread and there's data races..
	}
	// b.stamp("loop and create new chunks");
	// b.stop("chunk update");
	// std::cout << "for now " << count << " new chunks, computed " << coords.size() << std::endl;
}

void OpenGL_Manager::chunk_update( void )
{
	int posX = chunk_pos(static_cast<int>(glm::floor(_camera->getPos().x)));
	int posY = chunk_pos(static_cast<int>(glm::floor(_camera->getPos().y)));
	
	if (posX == _current_chunk.x && posY == _current_chunk.y) {
		return ;
	}
	// std::cout << "new chunk " << posX << ", " << posY << " !" << std::endl;
	_current_chunk.x = posX;
	_current_chunk.y = posY;

	if (_thread.joinable()) {
		_thread.join();
	}
	_thread = std::thread(thread_chunk_update, this, _render_distance, posX, posY);
}

void OpenGL_Manager::user_inputs( float deltaTime, bool rayCast )
{
	// open menu
	if (_esc_released && glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		_paused = true;
		_esc_released = false;
		_menu->setState(PAUSE_MENU);
		return ;
	} else if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
		_esc_released = true;
	}
	// open inventory
	if (_e_released && glfwGetKey(_window, GLFW_KEY_E) == GLFW_PRESS) {
		_paused = true;
		_e_released = false;
		_esc_released = false;
		_menu->setState(INVENTORY_MENU);
		return ;
	} else if (glfwGetKey(_window, GLFW_KEY_E) == GLFW_RELEASE) {
		_e_released = true;
	}
	// toggle chat
	if (glfwGetKey(_window, GLFW_KEY_T) == GLFW_PRESS) {
		_paused = true;
		_esc_released = false;
		_menu->setState(CHAT_MENU);
		return ;
	}
	// quit program
	if (glfwGetKey(_window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(_window, GL_TRUE);
		return ;
	}

	// take screenshot
	if (glfwGetKey(_window, GLFW_KEY_ENTER) == GLFW_PRESS && ++_key_screenshot == 1) {
		screenshot();
	} else if (glfwGetKey(_window, GLFW_KEY_ENTER) == GLFW_RELEASE) {
		_key_screenshot = 0;
	}
	// toggle debug mode on off
	if ((glfwGetKey(_window, GLFW_KEY_H) == GLFW_PRESS) && ++_key_h == 1) {
		_debug_mode = !_debug_mode;
	} else if (glfwGetKey(_window, GLFW_KEY_H) == GLFW_RELEASE) {
		_key_h = 0;
	}
	// toggle game mode
	if ((glfwGetKey(_window, GLFW_KEY_G) == GLFW_PRESS) && ++_key_g == 1) {
		setGamemode(!_game_mode);
	} else if (glfwGetKey(_window, GLFW_KEY_G) == GLFW_RELEASE) {
		_key_g = 0;
	}
	// toggle F5 mode
	if ((glfwGetKey(_window, GLFW_KEY_J) == GLFW_PRESS) && ++_key_j == 1) {
		_ui->_hideUI = !_ui->_hideUI;
		_ui->chatMessage(std::string("UI ") + ((_ui->_hideUI) ? "HIDDEN" : "SHOWN"));
	} else if (glfwGetKey(_window, GLFW_KEY_J) == GLFW_RELEASE) {
		_key_j = 0;
	}
	// toggle outline
	if ((glfwGetKey(_window, GLFW_KEY_O) == GLFW_PRESS) && ++_key_o == 1) {
		_ui->chatMessage(std::string("outlines ") + ((_outline) ? "HIDDEN" : "SHOWN"));
		_outline = !_outline;
	} else if (glfwGetKey(_window, GLFW_KEY_O) == GLFW_RELEASE) {
		_key_o = 0;
	}
	// change time multiplier
	GLint mul = (glfwGetKey(_window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS);
	if (mul && ++_key_time_mul == 1) {
		DayCycle::Get()->updateTimeMultiplier(mul);
	} else if (!mul) {
		_key_time_mul = 0;
	}

	// add and remove blocks
	_hand_content = _inventory->getCurrentSlot();
	if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (_game_mode == SURVIVAL) {
			_break_time += deltaTime;
			float break_time = s_blocks[_block_hit.value]->getBreakTime(_hand_content);
			bool can_collect = s_blocks[_block_hit.value]->canCollect(_hand_content);
			// getBreakTime(break_time); // TODO add fct to blocks.cpp to compute time using fandom pseudo code and cmp results
			if (_block_hit.value != blocks::AIR && _break_time >= break_time) {
				_break_time = 0;
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
		} else if (++_key_rm_block == 1) {
			handle_add_rm_block(false, false);
		}
	} else if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		_key_rm_block = 0;
		_break_time = 0;
		if (_break_frame != _outline && chunk_hit) {
			chunk_hit->updateBreak({_block_hit.pos, _block_hit.value}, _outline);
		}
		_break_frame = _outline;
	}
	if (!_key_rm_block && glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT)) { // I don't want to try to del and add at the same time
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
		} else if (++_key_add_block == 1) {
			handle_add_rm_block(true, _game_mode == SURVIVAL);
		}
	} else if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
		if (_hand_content == blocks::BOW && _bow_timer && current_chunk_ptr) {
			// TODO rm arrow from inventory (once implemented)
			_inventory->decrementDurabitilty();
			current_chunk_ptr->shootArrow(_bow_timer);
		}
		_key_add_block = 0;
		_eat_timer = 0;
		_bow_timer = 0;
	}
	// drop item
	if (glfwGetKey(_window, GLFW_KEY_Q) == GLFW_PRESS) {
		t_item details = _inventory->removeBlock(true);
		if (details.type != blocks::AIR) {
			mtx.lock();
			current_chunk_ptr->dropEntity(_camera->getDir(), details);
			mtx.unlock();
		}
	}
	if (_game_mode == CREATIVE && _block_hit.value != blocks::AIR && _key_rm_block != 1 && _key_add_block != 1
		&& glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_MIDDLE) && ++_key_pick_block == 1) { // pick up in creative mode
		_inventory->replaceSlot(_block_hit.value);
	} else if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE) {
		_key_pick_block = 0;
	}

	// toggle polygon mode fill / lines
	if (glfwGetKey(_window, GLFW_KEY_F) == GLFW_PRESS && ++_key_fill == 1) {
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
	} else if (glfwGetKey(_window, GLFW_KEY_F) == GLFW_RELEASE)
		_key_fill = 0;

	// change render dist
	GLint key_render_dist = (glfwGetKey(_window, GLFW_KEY_EQUAL) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_MINUS) == GLFW_PRESS);
	if (key_render_dist && ++_key_rdist == 1 && _render_distance + key_render_dist > 0) {
		_render_distance += key_render_dist;
		_ui->chatMessage("Render distance set to " + std::to_string(_render_distance));
		glUniform1f(_uniFog, (1 + _render_distance) << CHUNK_SHIFT);
		glUseProgram(_skyShaderProgram);
		glUniform1f(_skyUniFog, (1 + _render_distance) << CHUNK_SHIFT);
		glUseProgram(_shaderProgram);
		if (_thread.joinable()) {
			_thread.join();
		}
		_thread = std::thread(thread_chunk_update, this, _render_distance, _current_chunk.x, _current_chunk.y);
		// update_visible_chunks();
		// std::cout << "render distance set to " << _render_distance << std::endl;
	} else if (!key_render_dist) {
		_key_rdist = 0;
	}
	// change gui size
	GLint key_gui_size = (glfwGetKey(_window, GLFW_KEY_KP_MULTIPLY) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_KP_DIVIDE) == GLFW_PRESS);
	if (key_gui_size && ++_key_guisize == 1) {
		_menu->changeGuiSize(key_gui_size);
		_ui->changeGuiSize(key_gui_size);
	} else if (!key_gui_size) {
		_key_guisize = 0;
	}

	// camera work 
	_camera->setDelta(deltaTime);
	GLint key_cam_v = (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS);
	GLint key_cam_h = (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS);
	GLint key_cam_z = (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);

	// this will be commented at some point
	GLint key_cam_yaw = (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS);
	if (key_cam_yaw) {
		_camera->processYaw(key_cam_yaw * 5);
	}
	GLint key_cam_pitch = (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS);
	if (key_cam_pitch) {
		_camera->processPitch(key_cam_pitch * 5);
	}

	if (!key_cam_v && !key_cam_h) {
		_camera->setRun(false);
	} else {
		_camera->setRun(glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
	}

	if (_game_mode == CREATIVE) { // no collision check, free to move however you want
		_camera->moveFly(key_cam_v, key_cam_h, key_cam_z);
	}

	// we update the current chunk before we update cam view, because we check in current chunk for collision
	// update block hit
	if (rayCast) {
		mtx.lock();
		if (_game_mode == SURVIVAL && current_chunk_ptr) { // on first frame -> no current_chunk_ptr
			mtx.unlock();
			_camera->moveHuman((key_cam_z == 1 && ++_key_jump == 1) ? UP : DOWN, key_cam_v, key_cam_h, key_cam_z); // sets inJump variable, no actual movement
			if (key_cam_z < 1) {
				_key_jump = 0;
			}
			_camera->moveHuman(X_AXIS, key_cam_v, key_cam_h, 0); // move on X_AXIS
			mtx.lock();
			if (current_chunk_ptr->collisionBox(_camera->getPos(), 0.3f, 1.8f)) {
				mtx.unlock();
				_camera->moveHuman(X_AXIS, -key_cam_v, -key_cam_h, 0); // if collision after movement, undo movement
				_camera->setRun(false);
				mtx.lock();
			}
			mtx.unlock();
			_camera->moveHuman(Y_AXIS, key_cam_v, key_cam_h, 0); // move on Y_AXIS
			mtx.lock();
			if (current_chunk_ptr->collisionBox(_camera->getPos(), 0.3f, 1.8f)) {
				mtx.unlock();
				_camera->moveHuman(Y_AXIS, -key_cam_v, -key_cam_h, 0); // if collision after movement, undo movement
				_camera->setRun(false);
				mtx.lock();
			}
			mtx.unlock();
			mtx.lock();
			current_chunk_ptr->applyGravity(); // move on Z_AXIS
			mtx.unlock();
			glm::vec3 camPos = _camera->getPos();
			mtx.lock();
			_camera->setWaterStatus(false, current_chunk_ptr->collisionBoxWater(camPos, 0.3f, 0));
			camPos.z += 1 + EYE_LEVEL;
			_camera->setWaterStatus(true, current_chunk_ptr->collisionBoxWater(camPos, 0.05f, 0));
		}
		mtx.unlock();

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

		if (!_camera->_health_points) { // dead
			_inventory->spillInventory(current_chunk_ptr);
			_paused = true;
			_menu->setState(DEATH_MENU);
			return ;
		}
	}
	glm::vec3 camPos = _camera->getPos();
	glm::ivec3 current_block = {glm::floor(camPos.x), glm::floor(camPos.y), glm::floor(camPos.z)};
	if (current_block != _camera->_current_block) {
		_camera->_current_block = current_block;
		if (current_chunk_ptr) {
			camPos.z += 1 + EYE_LEVEL;
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

	GLint key_cam_speed = (glfwGetKey(_window, GLFW_KEY_KP_ADD) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS);
	if (key_cam_speed) {
		_camera->update_movement_speed(key_cam_speed);
	}

	// inventory slot selection
	if ((glfwGetKey(_window, GLFW_KEY_1) == GLFW_PRESS) && ++_key_1 == 1) {
		_inventory->setSlot(0);
	} else if (glfwGetKey(_window, GLFW_KEY_1) == GLFW_RELEASE) {
		_key_1 = 0;
	}
	if ((glfwGetKey(_window, GLFW_KEY_2) == GLFW_PRESS) && ++_key_2 == 1) {
		_inventory->setSlot(1);
	} else if (glfwGetKey(_window, GLFW_KEY_2) == GLFW_RELEASE) {
		_key_2 = 0;
	}
	if ((glfwGetKey(_window, GLFW_KEY_3) == GLFW_PRESS) && ++_key_3 == 1) {
		_inventory->setSlot(2);
	} else if (glfwGetKey(_window, GLFW_KEY_3) == GLFW_RELEASE) {
		_key_3 = 0;
	}
	if ((glfwGetKey(_window, GLFW_KEY_4) == GLFW_PRESS) && ++_key_4 == 1) {
		_inventory->setSlot(3);
	} else if (glfwGetKey(_window, GLFW_KEY_4) == GLFW_RELEASE) {
		_key_4 = 0;
	}
	if ((glfwGetKey(_window, GLFW_KEY_5) == GLFW_PRESS) && ++_key_5 == 1) {
		_inventory->setSlot(4);
	} else if (glfwGetKey(_window, GLFW_KEY_5) == GLFW_RELEASE) {
		_key_5 = 0;
	}
	if ((glfwGetKey(_window, GLFW_KEY_6) == GLFW_PRESS) && ++_key_6 == 1) {
		_inventory->setSlot(5);
	} else if (glfwGetKey(_window, GLFW_KEY_6) == GLFW_RELEASE) {
		_key_6 = 0;
	}
	if ((glfwGetKey(_window, GLFW_KEY_7) == GLFW_PRESS) && ++_key_7 == 1) {
		_inventory->setSlot(6);
	} else if (glfwGetKey(_window, GLFW_KEY_7) == GLFW_RELEASE) {
		_key_7 = 0;
	}
	if ((glfwGetKey(_window, GLFW_KEY_8) == GLFW_PRESS) && ++_key_8 == 1) {
		_inventory->setSlot(7);
	} else if (glfwGetKey(_window, GLFW_KEY_8) == GLFW_RELEASE) {
		_key_8 = 0;
	}
	if ((glfwGetKey(_window, GLFW_KEY_9) == GLFW_PRESS) && ++_key_9 == 1) {
		_inventory->setSlot(8);
	} else if (glfwGetKey(_window, GLFW_KEY_9) == GLFW_RELEASE) {
		_key_9 = 0;
	}
}
