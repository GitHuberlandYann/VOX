#include "vox.h"

// extern OpenGL_Manager *render;
Camera camera(glm::vec3(1.0f, -2.0f, 66.0f));
Chunk *current_chunk_ptr = NULL;
Chunk *chunk_hit = NULL;
Inventory *scroll_inventory = NULL;
double lastX = WIN_WIDTH / 2.0f;
double lastY = WIN_HEIGHT / 2.0f;

void cursor_position_callback( GLFWwindow* window, double xpos, double ypos )
{
	(void)window;

    double x_offset = lastX - xpos;
    double y_offset = lastY - ypos;
	// std::cout << "x " << xpos << "\t\ty " << ypos << "\t\tx_offset: " << x_offset << "\t\ty_offset: " << y_offset << std::endl;

    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(x_offset / 10, y_offset / 10);
}

void scroll_callback( GLFWwindow* window, double xoffset, double yoffset )
{
	(void)window;
	(void)xoffset;

	if (!scroll_inventory) {
		return ;
	}

	if (yoffset > 0) {
		scroll_inventory->setSlot(scroll_inventory->getSlot() + 1);
	} else if (yoffset < 0) {
		scroll_inventory->setSlot(scroll_inventory->getSlot() - 1);
	}
}
/*
void OpenGL_Manager::update_cam_matrix( void )
{
	glm::mat4 proj = camera.getPerspectiveMatrix();
	glm::mat4 view = camera.getViewMatrix();

	glUniformMatrix4fv(_uniPV, 1, GL_FALSE, glm::value_ptr(proj * view));
}*/

glm::ivec4 OpenGL_Manager::get_block_hit( void )
{
	std::vector<glm::ivec3> ids = camera.get_ray_casting(10);

	glm::ivec2 current_chunk = glm::ivec2(INT_MAX, INT_MAX), previous_chunk;
	glm::ivec3 player_pos;
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
				std::cout << "chunk out of bound at " << posX << ", " << posY << std::endl;
				break ;
			}
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
				Chunk *chunk = c;
				// std::cout << "handle hit at pos " << _block_hit.x << ", " << _block_hit.y << ", " << _block_hit.z << std::endl;
				(collect)
					? chunk->handleHit(_inventory, 0, glm::ivec3(_block_hit.x, _block_hit.y, _block_hit.z), adding)
					: chunk->handleHit(NULL, 0, glm::ivec3(_block_hit.x, _block_hit.y, _block_hit.z), adding);
				return ;
			}
		}
		std::cout << "chunk out of bound at " << posX << ", " << posY << std::endl;
		return ;
	}
	// from now on adding = true
	int type = _inventory->getCurrentSlot();
	if (type == blocks::AIR) {
		// std::cout << "can't add block if no object in inventory" << std::endl;
		return ;
	}
	std::vector<glm::ivec3> ids = camera.get_ray_casting(10);

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
				std::cout << "chunk out of bound at " << posX << ", " << posY << std::endl;
				break ;
			}
		}
		// std::cout << "current_chunk should be " << current_chunk.x << ", " << current_chunk.y << std::endl;
		if (chunk->isHit(i)) {
			// std::cout << "we have a hit ! " << i.x << ", " << i.y << ", " << i.z << ", " << std::endl;
			if (i == player_pos || previous_block == player_pos) {
				// std::cout << "abort because hit is player pos" << std::endl;
				return ;
			}
			if (previous_chunk != current_chunk) {
				(collect)
						? prev_chunk->handleHit(_inventory, type, previous_block, adding)
						: prev_chunk->handleHit(NULL, type, previous_block, adding);
				return ;
			}
			(collect)
					? chunk->handleHit(_inventory, type, previous_block, adding)
					: chunk->handleHit(NULL, type, previous_block, adding);
			return ;
		}
		previous_block = i;
		previous_chunk = current_chunk;
	}
}

void OpenGL_Manager::update_cam_view( void )
{
	glm::mat4 view = camera.getViewMatrix();
	glUniformMatrix4fv(_uniView, 1, GL_FALSE, glm::value_ptr(view));

	// glUniform3fv(_uniCamPos, 1, glm::value_ptr(camera._position));
}

void OpenGL_Manager::update_cam_perspective( void )
{
	glm::mat4 proj = camera.getPerspectiveMatrix();
	glUniformMatrix4fv(_uniProj, 1, GL_FALSE, glm::value_ptr(proj));
}

void OpenGL_Manager::update_visible_chunks( void ) // TODO turn this into thread ?
{
	std::list<Chunk *> newvis_chunks;
	mtx.lock();
	std::list<Chunk *>::iterator ite = _chunks.end();
	std::list<Chunk *>::iterator it = _chunks.begin();
	mtx.unlock();
	for (; it != ite;) {
		mtx.lock();
		if (camera.chunkInFront((*it)->getStartX(), (*it)->getStartY())) {
			(*it)->setVisibility(&newvis_chunks, _current_chunk.x, _current_chunk.y, _render_distance * CHUNK_SIZE);
		}
		++it;
		mtx.unlock();
	}
	mtx_visible_chunks.lock();
	_visible_chunks = newvis_chunks;
	mtx_visible_chunks.unlock();
}

static void thread_chunk_update( std::list<Chunk *> *chunks, std::list<Chunk *> *visible_chunks, std::list<Chunk *> *delete_chunks,
								 GLint render_dist, int posX, int posY )
{
	std::list<Chunk *> newvis_chunks;
	std::list<Chunk *> newdel_chunks;
	mtx.lock();
	std::list<Chunk *>::iterator ite = chunks->end();
	std::list<Chunk *>::iterator it = chunks->begin();
	mtx.unlock();
	for (; it != ite;) {
		mtx.lock();
		if ((*it)->shouldDelete(camera._position, render_dist * 2 * CHUNK_SIZE)) {
			mtx.unlock();
			std::list<Chunk *>::iterator tmp = it;
			--it;
			newdel_chunks.push_back(*tmp);
			mtx.lock();
			chunks->erase(tmp);
			mtx.unlock();
		} else {
			mtx.unlock();
			mtx.lock();
			if (camera.chunkInFront((*it)->getStartX(), (*it)->getStartY())) {
				(*it)->setVisibility(&newvis_chunks, posX, posY, render_dist * CHUNK_SIZE);
				if ((*it)->isInChunk(posX, posY)) {
						current_chunk_ptr = *it;
				}
			}
			mtx.unlock();
		}
		mtx.lock();
		++it;
		mtx.unlock();
	}
	mtx_visible_chunks.lock();
	*visible_chunks = newvis_chunks;
	mtx_visible_chunks.unlock();
	mtx_delete_chunks.lock();
	*delete_chunks = newdel_chunks;
	mtx_delete_chunks.unlock();

	for (int row = -render_dist; row <= render_dist; row++) {
		for (int col = -render_dist; col <= render_dist; col++) {
			// if (camera.chunkInFront(posX + row * CHUNK_SIZE, posY + col * CHUNK_SIZE)) {
				bool isInChunk = false;
			
				// it = newvis_chunks.begin();
				// for (; it != newvis_chunks.end(); it++) {
				// 	if ((*it)->isInChunk(posX + row * CHUNK_SIZE, posY + col * CHUNK_SIZE)) {
				// 		isInChunk = true;
				// 		break ;
				// 	}
				// }
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

				// std::cout << "x: " << pos.x << ", y: " << pos.y << std::endl;
				if (!isInChunk) {
					//create new chunk where player stands
					Chunk *newChunk = new Chunk(posX + row * CHUNK_SIZE, posY + col * CHUNK_SIZE);
					newChunk->generate_chunk(chunks);
				}
			// }
		}
	}
}

void OpenGL_Manager::chunk_update( void )
{
	int posX = chunk_pos(static_cast<int>(glm::floor(camera._position.x)));
	int posY = chunk_pos(static_cast<int>(glm::floor(camera._position.y)));
	
	if (posX == _current_chunk.x && posY == _current_chunk.y) {
		return ;
	}
	// std::cout << "new chunk " << posX << ", " << posY << " !" << std::endl;
	_current_chunk.x = posX;
	_current_chunk.y = posY;

	if (_thread.joinable()) {
		_thread.join();
	}
	_thread = std::thread(thread_chunk_update, &_chunks, &_visible_chunks, &_delete_chunks, _render_distance, posX, posY);
}

void OpenGL_Manager::user_inputs( float deltaTime )
{
	if (!scroll_inventory) {
		scroll_inventory = _inventory;
	}

	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(_window, GL_TRUE);
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
	} else if (glfwGetKey(_window, GLFW_KEY_G) == GLFW_RELEASE) {
		_key_g = 0;
	}
	// toggle F5 mode
	if ((glfwGetKey(_window, GLFW_KEY_J) == GLFW_PRESS) && ++_key_j == 1) {
		_f5_mode = !_f5_mode;
	} else if (glfwGetKey(_window, GLFW_KEY_J) == GLFW_RELEASE) {
		_key_j = 0;
	}

	// add and remove blocks
	if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (_game_mode == SURVIVAL) {
			_break_time += deltaTime;
			if (_block_hit.w != blocks::AIR && _break_time >= s_blocks[_block_hit.w].break_time_hand) {
				handle_add_rm_block(false, s_blocks[_block_hit.w].byHand); // collect in inventory if byhand allowed, will later use tools
			} else {
				int break_frame = static_cast<int>(10 * _break_time / s_blocks[_block_hit.w].break_time_hand) + 1;
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
		if (_break_frame && chunk_hit) {
			chunk_hit->updateBreak(_block_hit, 0);
		}
		_break_frame = 0;
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

	// update block hit
	Chunk *save_chunk = chunk_hit;
	glm::ivec4 block_hit = get_block_hit();
	if (_block_hit != block_hit) {
		if (_break_frame && save_chunk) {
			save_chunk->updateBreak(_block_hit, 0);
		}
		_block_hit = block_hit;
		_break_time = 0;
		_break_frame = 0;
	}

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

	GLint key_render_dist = (glfwGetKey(_window, GLFW_KEY_EQUAL) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_MINUS) == GLFW_PRESS);
	if (key_render_dist && ++_key_rdist == 1 && _render_distance + key_render_dist > 0) {
		_render_distance += key_render_dist;
		// std::cout << "render distance set to " << _render_distance << std::endl;
	} else if (!key_render_dist) {
		_key_rdist = 0;
	}

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
	camera.setDelta(deltaTime);
	camera.setRun(glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
	if (camera._fovUpdate) {
		update_cam_perspective();
	}
	GLint key_cam_v = (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS);
	if (key_cam_v) {
		(key_cam_v == 1)
			? camera.processKeyboard(FORWARD, _game_mode)
			: camera.processKeyboard(BACKWARD, _game_mode);
	}
	GLint key_cam_h = (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS);
	if (key_cam_h) {
		(key_cam_h == 1)
			? camera.processKeyboard(LEFT, _game_mode)
			: camera.processKeyboard(RIGHT, _game_mode);
	}
	GLint key_cam_z = (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
	if (key_cam_z) {
		(key_cam_z == 1)
			? camera.processKeyboard(UP, _game_mode)
			: camera.processKeyboard(DOWN, _game_mode);
	}

	// this will be commented at some point
	GLint key_cam_yaw = (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS);
	if (key_cam_yaw) {
		camera.processYaw(key_cam_yaw);
	}
	GLint key_cam_pitch = (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS);
	if (key_cam_pitch) {
		camera.processPitch(key_cam_pitch);
	}

	// we update the current chunk before we update cam view, because we check in new chunk for collision
	chunk_update();
	if (_game_mode == SURVIVAL) {
		mtx.lock();
		if (current_chunk_ptr->collision(camera._position, camera)) {
			mtx.unlock();
			if (key_cam_v) {
				(key_cam_v == 1)
					? camera.processKeyboard(BACKWARD, _game_mode) // if collision after movement, undo movement
					: camera.processKeyboard(FORWARD, _game_mode);
			}
			if (key_cam_h) {
				(key_cam_h == 1)
					? camera.processKeyboard(RIGHT, _game_mode)
					: camera.processKeyboard(LEFT, _game_mode);
			}
			current_chunk_ptr->collision(camera._position, camera);
			mtx.lock();
		}
		mtx.unlock();
	}

	if (camera._update)
	{
		update_cam_view();
		if (camera._turnUpdate) {
			update_visible_chunks();
			camera._turnUpdate = false;
		}
	}

	GLint key_cam_speed = (glfwGetKey(_window, GLFW_KEY_KP_ADD) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS);
	if (key_cam_speed) {
		camera.update_movement_speed(key_cam_speed);
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
