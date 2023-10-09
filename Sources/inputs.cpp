#include "vox.h"

Chunk *current_chunk_ptr = NULL;
// Chunk *prev_chunk_ptr = NULL;
Chunk *chunk_hit = NULL;

Chunk *OpenGL_Manager::get_current_chunk_ptr( void )
{
	return (current_chunk_ptr);
}

void OpenGL_Manager::resetInputsPtrs( void )
{
	current_chunk_ptr = NULL;
	// prev_chunk_ptr = NULL;
	chunk_hit = NULL;
}
/*
void OpenGL_Manager::update_cam_matrix( void )
{
	glm::mat4 proj = _camera->getPerspectiveMatrix();
	glm::mat4 view = _camera->getViewMatrix();

	glUniformMatrix4fv(_uniPV, 1, GL_FALSE, glm::value_ptr(proj * view));
}*/

glm::ivec4 OpenGL_Manager::get_block_hit( void )
{
	std::vector<glm::ivec3> ids = _camera->get_ray_casting((_game_mode == CREATIVE) ? _render_distance * CHUNK_SIZE / 2 : 10);

	glm::ivec2 current_chunk = glm::ivec2(INT_MAX, INT_MAX), previous_chunk;
	Chunk *chunk = NULL;
	bool first_loop = true;
	for (auto& i : ids) {
		// std::cout << "checking > " << i.x << ", " << i.y << ", " << i.z << std::endl;
		int posX = chunk_pos(i.x);
		int posY = chunk_pos(i.y);
		
		if (!chunk || posX != current_chunk.x || posY != current_chunk.y) {
			current_chunk = glm::ivec2(posX, posY);
			chunk = NULL;
			for (auto& c : _visible_chunks) {
				if (c->isInChunk(posX, posY)) {
					chunk = c;
					break ;
				}
			}
			if (!chunk) {
				std::cout << "chunk out of bound at " << posX << ", " << posY << std::endl;
				return (glm::ivec4(0, 0, 0, blocks::AIR));
			}
		}
		if (first_loop) {
			// (current_chunk_ptr) ? prev_chunk_ptr = current_chunk_ptr : prev_chunk_ptr = chunk;
			current_chunk_ptr = chunk;
			first_loop = false;
		}
		// std::cout << "current_chunk should be " << current_chunk.x << ", " << current_chunk.y << std::endl;
		int value = chunk->isHit(i);
		if (value) {
			chunk_hit = chunk;
			return (glm::ivec4(i, (value > 0) ? value : value + blocks::NOTVISIBLE));
		}
		previous_chunk = current_chunk;
	}
	return (glm::ivec4(0, 0, 0, blocks::AIR));
}

void OpenGL_Manager::handle_add_rm_block( bool adding, bool collect )
{
	if (_block_hit.w == blocks::AIR) {
		return ;
	}
	if (!adding) {
		int posX = chunk_pos(_block_hit.x);
		int posY = chunk_pos(_block_hit.y);
		
		for (auto& c : _visible_chunks) {
			if (c->isInChunk(posX, posY)) {
				// std::cout << "handle hit at pos " << _block_hit.x << ", " << _block_hit.y << ", " << _block_hit.z << std::endl;
				(collect)
					? c->handleHit(_inventory, 0, glm::ivec3(_block_hit.x, _block_hit.y, _block_hit.z), adding, _visible_chunks)
					: c->handleHit(NULL, 0, glm::ivec3(_block_hit.x, _block_hit.y, _block_hit.z), adding, _visible_chunks);
				return ;
			}
		}
		std::cout << "rm: chunk out of bound at " << posX << ", " << posY << std::endl;
		return ;
	}
	// from now on adding = true
	if (_block_hit.w == blocks::CRAFTING_TABLE) {
		_paused = true;
		_esc_released = false;
		_e_released = false;
		_menu->setState(CRAFTING_MENU);
		return ;
	} else if (_block_hit.w == blocks::FURNACE) {
		_paused = true;
		_esc_released = false;
		_e_released = false;
		_menu->setState(FURNACE_MENU);
		_menu->setFurnaceInstance(chunk_hit->getFurnaceInstance(_block_hit));
		return ;
	}
	int type = _inventory->getCurrentSlot();
	if (type == blocks::AIR || type >= blocks::STICK) {
		// std::cout << "can't add block if no object in inventory" << std::endl;
		return ;
	}
	std::vector<glm::ivec3> ids = _camera->get_ray_casting((_game_mode == CREATIVE) ? _render_distance * CHUNK_SIZE / 2 : 10);

	glm::ivec2 current_chunk = glm::ivec2(INT_MAX, INT_MAX), previous_chunk;
	glm::ivec3 player_pos, previous_block;
	Chunk *chunk = NULL, *prev_chunk = NULL;
	bool start = true;
	for (auto& i : ids) {
		if (start) {
			player_pos = i;
			start = false;
		}
		// std::cout << "checking > " << i.x << ", " << i.y << ", " << i.z << std::endl;
		int posX = chunk_pos(i.x);
		int posY = chunk_pos(i.y);
		
		if (!chunk || posX != current_chunk.x || posY != current_chunk.y) {
			current_chunk = glm::ivec2(posX, posY);
			for (auto& c : _visible_chunks) {
				if (c->isInChunk(posX, posY)) {
					prev_chunk = chunk;
					chunk = c;
					break ;
				}
			}
			if (!chunk) {
				std::cout << "add: chunk out of bound at " << posX << ", " << posY << std::endl;
				break ;
			}
		}
		// std::cout << "current_chunk should be " << current_chunk.x << ", " << current_chunk.y << std::endl;
		if (chunk->isHit(i)) {
			// std::cout << "we have a hit ! " << i.x << ", " << i.y << ", " << i.z << ", " << std::endl;
			if (i == player_pos || previous_block == player_pos || previous_block == glm::ivec3(player_pos.x, player_pos.y, player_pos.z - 1)) {
				// std::cout << "abort because hit is player pos" << std::endl;
				return ;
			}
			if (previous_chunk != current_chunk) {
				(collect)
						? prev_chunk->handleHit(_inventory, type, previous_block, adding, _visible_chunks)
						: prev_chunk->handleHit(NULL, type, previous_block, adding, _visible_chunks);
				return ;
			}
			(collect)
					? chunk->handleHit(_inventory, type, previous_block, adding, _visible_chunks)
					: chunk->handleHit(NULL, type, previous_block, adding, _visible_chunks);
			return ;
		}
		previous_block = i;
		previous_chunk = current_chunk;
	}
}

void OpenGL_Manager::update_cam_view( void )
{
	glm::mat4 view = _camera->getViewMatrix();
	glUniformMatrix4fv(_uniView, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(_skyUniView, 1, GL_FALSE, glm::value_ptr(view));

	// glUniform3fv(_uniCamPos, 1, glm::value_ptr(_camera->_position));
}

void OpenGL_Manager::update_cam_perspective( void )
{
	glm::mat4 proj = _camera->getPerspectiveMatrix();
	glUniformMatrix4fv(_uniProj, 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(_skyUniProj, 1, GL_FALSE, glm::value_ptr(proj));
}

void OpenGL_Manager::update_visible_chunks( void ) // TODO turn this into thread ?
{
	std::list<Chunk *> newvis_chunks;
	mtx_perimeter.lock();
	for (auto& peri: _perimeter_chunks) {
		if (_camera->chunkInFront(_current_chunk, peri->getStartX(), peri->getStartY())) {
			peri->setVisibility(&newvis_chunks, _current_chunk.x, _current_chunk.y, _render_distance * CHUNK_SIZE);
		} else {
			peri->hide();
		}
	}
	mtx_perimeter.unlock();
	_visible_chunks = newvis_chunks;
}

static void thread_chunk_update( std::list<Chunk *> *chunks, std::list<Chunk *> *perimeter_chunks, std::list<Chunk *> *deleted_chunks, std::map<std::pair<int, int>, s_backup> *backups,
								 Camera *camera, GLint render_dist, int posX, int posY )
{
	std::list<Chunk *> newperi_chunks;
	std::list<Chunk *> newdel_chunks;
	mtx.lock();
	std::list<Chunk *>::iterator ite = chunks->end();
	std::list<Chunk *>::iterator it = chunks->begin();
	mtx.unlock();
	// std::cout << "IN THREAD UPDATE, nb chunks: " << chunks->size() << std::endl;
	for (; it != ite;) {
		mtx.lock();
		if ((*it)->shouldDelete(camera->_position, render_dist * 2 * CHUNK_SIZE)) {
			std::list<Chunk *>::iterator tmp = it;
			--it;
			mtx.unlock();
			(*tmp)->setBackup(backups);
			newdel_chunks.push_back(*tmp);
			mtx.lock();
			chunks->erase(tmp);
		} else if ((*it)->inPerimeter(posX, posY, render_dist * CHUNK_SIZE)) {
			// std::cout << "IN PERIMETER" << std::endl;
			newperi_chunks.push_back(*it);
		}
		++it;
		mtx.unlock();
	}
	newperi_chunks = sort_chunks(camera->_position, newperi_chunks);
	mtx_perimeter.lock();
	*perimeter_chunks = newperi_chunks;
	mtx_perimeter.unlock();
	mtx_deleted_chunks.lock();
	*deleted_chunks = newdel_chunks;
	mtx_deleted_chunks.unlock();

	for (int row = -render_dist; row <= render_dist; row++) {
		for (int col = -render_dist; col <= render_dist; col++) {
			bool isInChunk = false;
			
			mtx.lock();
			ite = chunks->end();
			it = chunks->begin();
			mtx.unlock();
			for (; it != ite;) {
				mtx.lock();
				if ((*it)->isInChunk(posX + row * CHUNK_SIZE, posY + col * CHUNK_SIZE)) {
					isInChunk = true;
					mtx.unlock();
					break ;
				}
				it++;
				mtx.unlock();
			}

			if (!isInChunk) {
				//create new chunk where player stands
				Chunk *newChunk = new Chunk(camera, posX + row * CHUNK_SIZE, posY + col * CHUNK_SIZE);
				std::map<std::pair<int, int>, s_backup>::iterator search = backups->find(std::pair<int, int>(posX + row * CHUNK_SIZE, posY + col * CHUNK_SIZE));
				if (search != backups->end()) {
					newChunk->restoreBackup(search->second);
					backups->erase(search);
				}
				newChunk->generate_chunk(chunks); // TODO remove this from thread because it launches its own thread and there's data races..
			}
		}
	}
}

void OpenGL_Manager::chunk_update( void )
{
	int posX = chunk_pos(static_cast<int>(glm::floor(_camera->_position.x)));
	int posY = chunk_pos(static_cast<int>(glm::floor(_camera->_position.y)));
	
	if (posX == _current_chunk.x && posY == _current_chunk.y) {
		return ;
	}
	// std::cout << "new chunk " << posX << ", " << posY << " !" << std::endl;
	_current_chunk.x = posX;
	_current_chunk.y = posY;

	if (_thread.joinable()) {
		_thread.join();
	}
	_thread = std::thread(thread_chunk_update, &_chunks, &_perimeter_chunks, &_deleted_chunks, &_backups, _camera, _render_distance, posX, posY);
}

void OpenGL_Manager::user_inputs( float deltaTime, bool rayCast )
{
	if (_esc_released && glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		_paused = true;
		_esc_released = false;
		_menu->setState(PAUSE_MENU);
		return ;
	} else if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
		_esc_released = true;
	}
	if (_e_released && glfwGetKey(_window, GLFW_KEY_E) == GLFW_PRESS) {
		_paused = true;
		_e_released = false;
		_esc_released = false;
		_menu->setState(INVENTORY_MENU);
		return ;
	} else if (glfwGetKey(_window, GLFW_KEY_E) == GLFW_RELEASE) {
		_e_released = true;
	}

	if (glfwGetKey(_window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(_window, GL_TRUE);
		return ;
	}

	// toggle debug mode on off
	if ((glfwGetKey(_window, GLFW_KEY_H) == GLFW_PRESS) && ++_key_h == 1) {
		_debug_mode = !_debug_mode;
	} else if (glfwGetKey(_window, GLFW_KEY_H) == GLFW_RELEASE) {
		_key_h = 0;
	}
	// toggle game mode
	if ((glfwGetKey(_window, GLFW_KEY_G) == GLFW_PRESS) && ++_key_g == 1) {
		_game_mode = !_game_mode;
		_camera->_fall_immunity = true;
	} else if (glfwGetKey(_window, GLFW_KEY_G) == GLFW_RELEASE) {
		_key_g = 0;
	}
	// toggle F5 mode
	if ((glfwGetKey(_window, GLFW_KEY_J) == GLFW_PRESS) && ++_key_j == 1) {
		_f5_mode = !_f5_mode;
	} else if (glfwGetKey(_window, GLFW_KEY_J) == GLFW_RELEASE) {
		_key_j = 0;
	}
	// toggle outline
	if ((glfwGetKey(_window, GLFW_KEY_O) == GLFW_PRESS) && ++_key_o == 1) {
		_outline = !_outline;
	} else if (glfwGetKey(_window, GLFW_KEY_O) == GLFW_RELEASE) {
		_key_o = 0;
	}

	// add and remove blocks
	if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (_game_mode == SURVIVAL) {
			_break_time += deltaTime;
			mtx_inventory.lock();
			float break_time = s_blocks[_block_hit.w].getBreakTime(_inventory->getCurrentSlot());
			bool can_collect = s_blocks[_block_hit.w].canCollect(_inventory->getCurrentSlot());
			mtx_inventory.unlock();
			if (_block_hit.w != blocks::AIR && _break_time >= break_time) {
				_break_time = 0;
				_break_frame = _outline;
				handle_add_rm_block(false, can_collect);
				_inventory->decrementDurabitilty();
			} else {
				int break_frame = static_cast<int>(10 * _break_time / break_time) + 2;
				if (_break_frame != break_frame) {
					if (chunk_hit) {
						chunk_hit->updateBreak(_block_hit, break_frame);
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
			chunk_hit->updateBreak(_block_hit, _outline);
		}
		_break_frame = _outline;
	}
	if (_key_rm_block != 1 && glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT) && ++_key_add_block == 1) { // I don't want to try to del and add at the same time
		handle_add_rm_block(true, _game_mode == SURVIVAL);
	} else if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
		_key_add_block = 0;
	}
	if (glfwGetKey(_window, GLFW_KEY_Q) == GLFW_PRESS) {
		_inventory->removeBlock();
	}
	if (_game_mode == CREATIVE && _block_hit.w != blocks::AIR && _key_rm_block != 1 && _key_add_block != 1
		&& glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_MIDDLE) && ++_key_pick_block == 1) { // pick up in creative mode
		_inventory->replaceSlot(_block_hit.w);
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
				break;
			case LINE:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
		}
	} else if (glfwGetKey(_window, GLFW_KEY_F) == GLFW_RELEASE)
		_key_fill = 0;

	// change render dist
	GLint key_render_dist = (glfwGetKey(_window, GLFW_KEY_EQUAL) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_MINUS) == GLFW_PRESS);
	if (key_render_dist && ++_key_rdist == 1 && _render_distance + key_render_dist > 0) {
		_render_distance += key_render_dist;
		if (_thread.joinable()) {
			_thread.join();
		}
		_thread = std::thread(thread_chunk_update, &_chunks, &_perimeter_chunks, &_deleted_chunks, &_backups, _camera, _render_distance, _current_chunk.x, _current_chunk.y);
		// update_visible_chunks();
		// std::cout << "render distance set to " << _render_distance << std::endl;
	} else if (!key_render_dist) {
		_key_rdist = 0;
	}

	// change background color
	GLint key_col_r = (glfwGetKey(_window, GLFW_KEY_KP_7) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_KP_4) == GLFW_PRESS);
	GLint key_col_g = (glfwGetKey(_window, GLFW_KEY_KP_8) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_KP_5) == GLFW_PRESS);
	GLint key_col_b = (glfwGetKey(_window, GLFW_KEY_KP_9) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_KP_6) == GLFW_PRESS);
	if (key_col_r || key_col_g || key_col_b) {
		_background_color.x = glm::clamp(_background_color.x + key_col_r * 0.01f, 0.f, 1.f);
		_background_color.y = glm::clamp(_background_color.y + key_col_g * 0.01f, 0.f, 1.f);
		_background_color.z = glm::clamp(_background_color.z + key_col_b * 0.01f, 0.f, 1.f);
		glClearColor(_background_color.x, _background_color.y, _background_color.z, 1.0f);
	}

	// camera work 
	_camera->setDelta(deltaTime);
	GLint key_cam_v = (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS);
	GLint key_cam_h = (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS);
	GLint key_cam_z = (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);

	// this will be commented at some point
	GLint key_cam_yaw = (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS);
	if (key_cam_yaw) {
		_camera->processYaw(key_cam_yaw);
	}
	GLint key_cam_pitch = (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS);
	if (key_cam_pitch) {
		_camera->processPitch(key_cam_pitch);
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
			_camera->moveHuman((key_cam_z == 1 && ++_key_jump == 1) ? UP : DOWN, 0, 0); // sets inJump variable, no actual movement
			if (key_cam_z < 1) {
				_key_jump = 0;
			}
			_camera->moveHuman(X_AXIS, key_cam_v, key_cam_h); // move on X_AXIS
			mtx.lock();
			if (current_chunk_ptr->collisionBox(_camera->_position, 0.3f, 1.8f)) {
				mtx.unlock();
				_camera->moveHuman(X_AXIS, -key_cam_v, -key_cam_h); // if collision after movement, undo movement
				// prev_chunk_ptr->collision(_camera->_position, *_camera);
				// current_chunk_ptr = prev_chunk_ptr;
				_camera->setRun(false);
				mtx.lock();
			}
			mtx.unlock();
			_camera->moveHuman(Y_AXIS, key_cam_v, key_cam_h); // move on Y_AXIS
			mtx.lock();
			if (current_chunk_ptr->collisionBox(_camera->_position, 0.3f, 1.8f)) {
				mtx.unlock();
				_camera->moveHuman(Y_AXIS, -key_cam_v, -key_cam_h); // if collision after movement, undo movement
				_camera->setRun(false);
				mtx.lock();
			}
			mtx.unlock();
			mtx.lock();
			if (!current_chunk_ptr->collisionBox(_camera->_position, 0.3f, 1.8f)) {
				current_chunk_ptr->applyGravity(_camera); // move on Z_AXIS
			}
		}
		mtx.unlock();

		Chunk *save_chunk = chunk_hit;
		glm::ivec4 block_hit = get_block_hit();
		if (_block_hit != block_hit) {
			if (_break_frame && save_chunk) {
				save_chunk->updateBreak(_block_hit, 0);
			}
			_block_hit = block_hit;
			if (_outline && chunk_hit) {
				chunk_hit->updateBreak(_block_hit, 1);
			}
			_break_time = 0;
			_break_frame = _outline;
		}
	}

	if (_camera->_fovUpdate) {
		update_cam_perspective();
	}
	if (_camera->_update)
	{
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
