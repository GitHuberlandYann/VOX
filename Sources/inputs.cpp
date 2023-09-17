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

// void thread_create_chunk( std::list<Chunk *> *chunks, glm::vec2 pos )
// {
// 	// std::cout << "hello from thread" << std::endl;
// 	Chunk *newChunk = new Chunk(pos);

// 	mtx.lock();
// 	chunks->push_back(newChunk);
// 	mtx.unlock();
// }

static void thread_chunk_update( std::list<Chunk *> *chunks, GLint render_dist, int posX, int posY )
{
	mtx.lock();
	std::list<Chunk *>::iterator ite = chunks->end();
	std::list<Chunk *>::iterator it = chunks->begin();
	mtx.unlock();
	for (; it != ite; it++) {
		(*it)->setVisibility(posX, posY, render_dist * 16);
		if ((*it)->shouldDelete(camera._position, render_dist * 2 * 16)) {
			std::list<Chunk *>::iterator tmp = it;
			--it;
			mtx.lock();
			delete *tmp;
			chunks->erase(tmp);
			mtx.unlock();
		}
	}

	for (int row = -render_dist; row <= render_dist; row++) {
		for (int col = -render_dist; col <= render_dist; col++) {
			// glm::vec2 pos = glm::vec2(camera._position.x + row * 16, camera._position.y + col * 16);
			bool isInChunk = false;
		
			mtx.lock();
			it = chunks->begin();
			mtx.unlock();
			for (; it != ite; it++) {
				mtx.lock();
				if ((*it)->isInChunk(posX + row * 16, posY + col * 16)) {
					mtx.unlock();
					isInChunk = true;
					break ;
				}
				mtx.unlock();
			}

			// std::cout << "x: " << pos.x << ", y: " << pos.y << std::endl;
			if (!isInChunk) {
				//create new chunk where player stands
				Chunk *newChunk = new Chunk(posX + row * 16, posY + col * 16);
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
		? posX -= posX % 16
		: posX -= 16 + posX % 16;
	(posY >= 0)
		? posY -= posY % 16
		: posY -= 16 + posY % 16;
	
	if (posX == _current_chunk[0] && posY == _current_chunk[1]) {
		return ;
	}
	// std::cout << "new chunk " << posX << ", " << posY << " !" << std::endl;
	_current_chunk[0] = posX;
	_current_chunk[1] = posY;

	if (_thread.joinable()) {
		_thread.join();
	}
	_thread = std::thread(thread_chunk_update, &_chunks, _render_distance, posX, posY);
}

void OpenGL_Manager::user_inputs( float deltaTime )
{
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(_window, GL_TRUE);

	GLint key_cam_speed = (glfwGetKey(_window, GLFW_KEY_KP_ADD) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS);
	if (key_cam_speed) {
		(key_cam_speed == 1)
			? camera._movement_speed *= 1.2f
			: camera._movement_speed *= 0.8f;
	}
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
			// case POINT:
			// 	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			// 	break;
		}
	} else if (glfwGetKey(_window, GLFW_KEY_F) == GLFW_RELEASE)
		_key_fill = 0;
	/*
	GLint key_point_size = (glfwGetKey(_window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_LEFTACKET) == GLFW_PRESS);
	if (key_point_size && _point_size + 0.1f * key_point_size >= 1.0f && _point_size + 0.1f * key_point_size < 10.0f) {
		_point_size += 0.1f * key_point_size;
		glPointSize(_point_size);
	}
*/
	GLint key_render_dist = (glfwGetKey(_window, GLFW_KEY_EQUAL) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_MINUS) == GLFW_PRESS);
	if (key_render_dist && ++_key_rdist == 1 && _render_distance + key_render_dist > 0) {
		_render_distance += key_render_dist;
		std::cout << "render distance set to " << _render_distance << std::endl;
	} else if (!key_render_dist) {
		_key_rdist = 0;
	}
/*
	GLint key_light = (glfwGetKey(_window, GLFW_KEY_Z) == GLFW_PRESS);
*/
	GLint key_col_r = (glfwGetKey(_window, GLFW_KEY_KP_7) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_KP_4) == GLFW_PRESS);
	GLint key_col_g = (glfwGetKey(_window, GLFW_KEY_KP_8) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_KP_5) == GLFW_PRESS);
	GLint key_col_b = (glfwGetKey(_window, GLFW_KEY_KP_9) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_KP_6) == GLFW_PRESS);
	if (key_col_r || key_col_g || key_col_b) {
		// if (!key_light) {
			_background_color.x = glm::clamp(_background_color.x + key_col_r * 0.01f, 0.f, 1.f);
			_background_color.y = glm::clamp(_background_color.y + key_col_g * 0.01f, 0.f, 1.f);
			_background_color.z = glm::clamp(_background_color.z + key_col_b * 0.01f, 0.f, 1.f);
			glClearColor(_background_color.x, _background_color.y, _background_color.z, 1.0f);
		// } else {
		// 	_light_col.x = glm::clamp(_light_col.x + key_col_r * 0.01f, 0.f, 1.f);
		// 	_light_col.y = glm::clamp(_light_col.y + key_col_g * 0.01f, 0.f, 1.f);
		// 	_light_col.z = glm::clamp(_light_col.z + key_col_b * 0.01f, 0.f, 1.f);
		// 	glUniform3fv(_uniLightColor, 1, glm::value_ptr(_light_col));
		// }
	}
/*
	GLint key_next_section = (glfwGetKey(_window, GLFW_KEY_N) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_M) == GLFW_PRESS);
	if (key_next_section && ++_key_section == 1) {
		_section += key_next_section;
		if (_section == -1)
			_section = _vtp_size;
		else if (_section == (int)_vtp_size + 1)
			_section = 0;
	} else if (!key_next_section)
		_key_section = 0;
	
	if (glfwGetKey(_window, GLFW_KEY_I) == GLFW_PRESS && ++_key_invert == 1) {
		++_invert_col;
		if (_invert_col == 3)
			_invert_col = 0;
		glUniform1i(_uniInvert, _invert_col);
	} else if (glfwGetKey(_window, GLFW_KEY_I) == GLFW_RELEASE)
		_key_invert = 0;
	
	if (glfwGetKey(_window, GLFW_KEY_L) == GLFW_PRESS && ++_key_use_light == 1) {
		if (_can_light) {
			_use_light = !_use_light;
			glUniform1i(_uniUseLight, _use_light);
		}
	} else if (glfwGetKey(_window, GLFW_KEY_L) == GLFW_RELEASE)
		_key_use_light = 0;
	*/
	// camera and light work 
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

	GLint key_cam_yaw =  (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS);
	if (key_cam_yaw) {
		camera.processYaw(key_cam_yaw);
	}
	GLint key_cam_pitch = (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS);
	if (key_cam_pitch) {
		camera.processPitch(key_cam_pitch);
	}
	if (((key_cam_v || key_cam_h)) || key_cam_z || key_cam_pitch || key_cam_yaw)
	{
		update_cam_view();
		camera._mouse_update = false;
	}

	/*
	if (camera._scroll_update) {
		update_cam_perspective();
		camera._scroll_update = false;
	}
	if (key_light && (key_cam_v || key_cam_h)) {
		_light_pos = 2.0f * glm::vec3(glm::cos(glm::radians(_light_angles.x)) + glm::cos(glm::radians(_light_angles.y)),
								glm::sin(glm::radians(_light_angles.x)) + glm::cos(glm::radians(_light_angles.y)),
								glm::sin(glm::radians(_light_angles.y)));
		glUniform3fv(_uniLightPos, 1, glm::value_ptr(_light_pos));
	}*/
}
