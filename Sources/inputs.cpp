#include "vox.h"

Camera camera(glm::vec3(1.0f, -2.0f, 66.0f));/*
float lastX = WIN_WIDTH / 2.0f;
float lastY = WIN_HEIGHT / 2.0f;
bool first_mouse = true;

void cursor_position_callback( GLFWwindow* window, double xpos, double ypos )
{
	(void)window;

	float x_pos = static_cast<float>(xpos);
    float y_pos = static_cast<float>(ypos);

    if (first_mouse)
    {
        lastX = x_pos;
        lastY = y_pos;
        first_mouse = false;
    }

    float x_offset = x_pos - lastX;
    float y_offset = lastY - y_pos; // reversed since y-coordinates go from bottom to top
	// std::cout << "x " << x_pos << "\t\ty " << y_pos << "\t\tx_offset: " << x_offset << "\t\ty_offset: " << y_offset << std::endl;

    lastX = x_pos;
    lastY = y_pos;

    camera.processMouseMovement(x_offset / 10, y_offset / 10);
}

void scroll_callback( GLFWwindow* window, double xoffset, double yoffset )
{
	(void)window;
	(void)xoffset;

    camera.processMouseScroll(static_cast<float>(yoffset));
}
*/

// void OpenGL_Manager::update_cam_matrix( void )
// {
// 	glm::mat4 proj = camera.getPerspectiveMatrix();
// 	glm::mat4 view = camera.getViewMatrix();

// 	glUniformMatrix4fv(_uniPV, 1, GL_FALSE, glm::value_ptr(proj * view));
// }

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

// void thread_block_update( std::list<Chunk *> *chunks, glm::vec3 camPos, glm::vec3 camFront, GLint current_chunk[2], int action )
// {
// 	mtx_visible_chunks.lock();
// 	std::list<Chunk *>::iterator ite = chunks->end();
// 	std::list<Chunk *>::iterator it = chunks->begin();
// 	mtx_visible_chunks.unlock();
// 	for (; it != ite;) {
// 		mtx_visible_chunks.lock();
// 		if ((*it)->isInChunk(current_chunk[0], current_chunk[1])) {
// 			(*it)->action_block(camPos, camFront, action); // for now, can't access neighbouring chunks
// 			mtx_visible_chunks.unlock();
// 			return ;
// 		}
// 		it++;
// 		mtx_visible_chunks.unlock();
// 	}
// }

static void thread_chunk_update( std::list<Chunk *> *chunks, std::list<Chunk *> *visible_chunks, GLint render_dist, glm::vec3 camPos, int posX, int posY )
{
	std::list<Chunk *> newvis_chunks;
	mtx.lock();
	std::list<Chunk *>::iterator ite = chunks->end();
	std::list<Chunk *>::iterator it = chunks->begin();
	mtx.unlock();
	for (; it != ite; it++) {
		if ((*it)->shouldDelete(camPos, render_dist * 2 * CHUNK_SIZE)) {
			std::list<Chunk *>::iterator tmp = it;
			--it;
			mtx.lock();
			delete *tmp;
			chunks->erase(tmp);
			mtx.unlock();
		} else {
			(*it)->setVisibility(&newvis_chunks, posX, posY, render_dist * CHUNK_SIZE);
		}
	}
	mtx_visible_chunks.lock();
	*visible_chunks = newvis_chunks;
	mtx_visible_chunks.unlock();

	for (int row = -render_dist; row <= render_dist; row++) {
		for (int col = -render_dist; col <= render_dist; col++) {
			bool isInChunk = false;
		
			mtx.lock();
			it = chunks->begin();
			mtx.unlock();
			for (; it != ite; it++) {
				mtx.lock();
				if ((*it)->isInChunk(posX + row * CHUNK_SIZE, posY + col * CHUNK_SIZE)) {
					mtx.unlock();
					isInChunk = true;
					break ;
				}
				mtx.unlock();
			}

			// std::cout << "x: " << pos.x << ", y: " << pos.y << std::endl;
			if (!isInChunk) {
				//create new chunk where player stands
				Chunk *newChunk = new Chunk(posX + row * CHUNK_SIZE, posY + col * CHUNK_SIZE);
				newChunk->generate_chunk(chunks);
			}
		}
	}
}

void OpenGL_Manager::chunk_update( void )
{
	int posX = static_cast<int>(glm::floor(camera._position.x)); //TODO check what floor does on neg number
	int posY = static_cast<int>(glm::floor(camera._position.y));

	(posX >= 0)
		? posX -= posX % CHUNK_SIZE
		: posX -= CHUNK_SIZE + posX % CHUNK_SIZE;
	(posY >= 0)
		? posY -= posY % CHUNK_SIZE
		: posY -= CHUNK_SIZE + posY % CHUNK_SIZE;
	
	if (posX == _current_chunk[0] && posY == _current_chunk[1]) {
		return ;
	}
	// std::cout << "new chunk " << posX << ", " << posY << " !" << std::endl;
	_current_chunk[0] = posX;
	_current_chunk[1] = posY;

	if (_thread.joinable()) {
		_thread.join();
	}
	_thread = std::thread(thread_chunk_update, &_chunks, &_visible_chunks, _render_distance, camera._position, posX, posY);
}

void OpenGL_Manager::user_inputs( float deltaTime )
{
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(_window, GL_TRUE);

	// add and remove blocks
	// GLint key_add_block = (glfwGetKey(_window, GLFW_KEY_Q) == GLFW_PRESS);
	// if (key_add_block && ++_key_add_block == 1) {
	// 	if (_thread_block.joinable()) {
	// 		_thread_block.join();
	// 	}
	// 	_thread_block = std::thread(thread_block_update, &_visible_chunks, camera._position, camera._front, _current_chunk, ADD_BLOCK);
	// 	// std::cout << "add block" << std::endl;
	// } else {
	// 	_key_add_block = 0;
	// }
	/*
	if (glfwGetKey(_window, GLFW_KEY_C) == GLFW_PRESS && ++_key_color_mode == 1) {
		++_color_mode;
		if (_color_mode == MATERIAL && !_vtp_size)
			++_color_mode;
		if (_color_mode == TEXTURE && (!_nb_textures || _nb_textures == _omore_tex))
			++_color_mode;
		if (_color_mode == PROVIDED && !_omore_tex)
			++_color_mode;
		if (_color_mode == LAST)
			_color_mode = DEFAULT;
		glUniform1i(_uniColorMode, _color_mode);
	} else if (glfwGetKey(_window, GLFW_KEY_C) == GLFW_RELEASE)
		_key_color_mode = 0;

	if (glfwGetKey(_window, GLFW_KEY_B) == GLFW_PRESS && ++_key_depth == 1) {
		if (glIsEnabled(GL_DEPTH_TEST))
			glDisable(GL_DEPTH_TEST);
		else
			glEnable(GL_DEPTH_TEST);
	} else if (glfwGetKey(_window, GLFW_KEY_B) == GLFW_RELEASE)
		_key_depth = 0;
*/
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
		std::cout << "render distance set to " << _render_distance << std::endl;
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
	GLint key_cam_v = (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS);
	if (key_cam_v) {
		(key_cam_v == 1)
			? camera.processKeyboard(FORWARD)
			: camera.processKeyboard(BACKWARD);
	}
	GLint key_cam_h = (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS);
	if (key_cam_h) {
		(key_cam_h == 1)
			? camera.processKeyboard(LEFT)
			: camera.processKeyboard(RIGHT);
	}
	GLint key_cam_z = (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
	if (key_cam_z) {
		(key_cam_z == 1)
			? camera.processKeyboard(UP)
			: camera.processKeyboard(DOWN);
	}

	GLint key_cam_yaw = (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS);
	if (key_cam_yaw) {
		camera.processYaw(key_cam_yaw);
	}
	GLint key_cam_pitch = (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS);
	if (key_cam_pitch) {
		camera.processPitch(key_cam_pitch);
	}
	if (key_cam_v || key_cam_h || key_cam_z || key_cam_pitch || key_cam_yaw)
	{
		update_cam_view();
		// update_cam_matrix();
		camera._mouse_update = false;
	}

	GLint key_cam_speed = (glfwGetKey(_window, GLFW_KEY_KP_ADD) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS);
	if (key_cam_speed) {
		(key_cam_speed == 1)
			? camera._movement_speed *= 1.2f
			: camera._movement_speed *= 0.8f;
	}

}
