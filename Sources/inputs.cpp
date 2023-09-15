#include "vox.h"

Camera camera(glm::vec3(1.0f, -2.0f, 0.0f));/*
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

void OpenGL_Manager::chunk_update( void )
{
	std::list<Chunk *>::iterator ite = _chunks.end();
	for (std::list<Chunk *>::iterator it = _chunks.begin(); it != ite; it++) {
		(*it)->setVisibility(false);
		if ((*it)->shouldDelete(camera._position, _render_distance * 2 * 16)) {
			// delete *it;
			std::list<Chunk *>::iterator tmp = it;
			--it;
			_chunks.erase(tmp);
		}
	}

	for (int row = -_render_distance; row <= _render_distance; row++) {
		for (int col = -_render_distance; col <= _render_distance; col++) {
			glm::vec2 pos = glm::vec2(camera._position.x + row * 16, camera._position.y + col * 16);
			bool isInChunk = false;
		
			for (std::list<Chunk *>::iterator it = _chunks.begin(); it != ite; it++) {
				if ((*it)->isInChunk(pos)) {
					isInChunk = true;
					(*it)->setVisibility(true);
					break ;
				}
			}

			// std::cout << "x: " << pos.x << ", y: " << pos.y << std::endl;
			if (!isInChunk) {
				//create new chunk where player stands
				Chunk *newChunk = new Chunk(pos);

				newChunk->setup_array_buffer(0.0f);
				_chunks.push_back(newChunk);
				// std::cout << "currently at " << _chunks.size() << " chunks" << std::endl;
			}
		}
	}
}

void OpenGL_Manager::user_inputs( void )
{
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(_window, GL_TRUE);
	/*
	GLint key_rotation_z = (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS);
	GLint key_rotation_x = (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS);
	GLint key_rotation_y = (glfwGetKey(_window, GLFW_KEY_R) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_E) == GLFW_PRESS);
	if (key_rotation_z || key_rotation_y || key_rotation_x) {
		_rotation.x += key_rotation_x * _rotation_speed;
		_rotation.y += key_rotation_y * _rotation_speed;
		_rotation.z += key_rotation_z * _rotation_speed;
		glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(_rotation.x), glm::vec3(-1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(_uniModel, 1, GL_FALSE, glm::value_ptr(model));
	}
	*/
	GLint key_cam_speed = (glfwGetKey(_window, GLFW_KEY_KP_ADD) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS);
	if (key_cam_speed && camera._movement_speed + 0.01f * key_cam_speed >= 0) {
		camera._movement_speed += 0.01f * key_cam_speed;
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
		switch (_fill) {
			case FILL:
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
			case LINE:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			case POINT:
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				break;
		}
	} else if (glfwGetKey(_window, GLFW_KEY_F) == GLFW_RELEASE)
		_key_fill = 0;
	/*
	GLint key_point_size = (glfwGetKey(_window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_LEFTACKET) == GLFW_PRESS);
	if (key_point_size && _point_size + 0.1f * key_point_size >= 1.0f && _point_size + 0.1f * key_point_size < 10.0f) {
		_point_size += 0.1f * key_point_size;
		glPointSize(_point_size);
	}

	GLint key_zoom = (glfwGetKey(_window, GLFW_KEY_EQUAL) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_MINUS) == GLFW_PRESS);
	if (key_zoom && _zoom + 0.1f * key_zoom >= 0) {
		_zoom += 0.1f * key_zoom;
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(_zoom));
		glUniformMatrix4fv(_uniScale, 1, GL_FALSE, glm::value_ptr(scale));
	}

	GLint key_light = (glfwGetKey(_window, GLFW_KEY_Z) == GLFW_PRESS);

	GLint key_col_r = (glfwGetKey(_window, GLFW_KEY_KP_7) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_KP_4) == GLFW_PRESS);
	GLint key_col_g = (glfwGetKey(_window, GLFW_KEY_KP_8) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_KP_5) == GLFW_PRESS);
	GLint key_col_b = (glfwGetKey(_window, GLFW_KEY_KP_9) == GLFW_PRESS) - (glfwGetKey(_window, GLFW_KEY_KP_6) == GLFW_PRESS);
	if (key_col_r || key_col_g || key_col_b) {
		if (!key_light) {
			_background_color.x = glm::clamp(_background_color.x + key_col_r * 0.01f, 0.f, 1.f);
			_background_color.y = glm::clamp(_background_color.y + key_col_g * 0.01f, 0.f, 1.f);
			_background_color.z = glm::clamp(_background_color.z + key_col_b * 0.01f, 0.f, 1.f);
			glClearColor(_background_color.x, _background_color.y, _background_color.z, 1.0f);
		} else {
			_light_col.x = glm::clamp(_light_col.x + key_col_r * 0.01f, 0.f, 1.f);
			_light_col.y = glm::clamp(_light_col.y + key_col_g * 0.01f, 0.f, 1.f);
			_light_col.z = glm::clamp(_light_col.z + key_col_b * 0.01f, 0.f, 1.f);
			glUniform3fv(_uniLightColor, 1, glm::value_ptr(_light_col));
		}
	}

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
	
	if (key_cam_v || key_cam_h) {
		chunk_update();
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
