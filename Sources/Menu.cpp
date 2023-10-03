#include "vox.h"

Menu::Menu( Inventory & inventory, Text *text ) : _state(MAIN_MENU), _selection(0), _vaoSet(false),
	_esc_released(false), _e_released(false), _left_released(false), _right_released(false),
	_key_1(0), _key_2(0), _key_3(0), _key_4(0), _key_5(0), _key_6(0), _key_7(0), _key_8(0), _key_9(0),
	_inventory(inventory), _text(text), _furnace(NULL)
{
	_selected_block = glm::ivec2(blocks::AIR, 0);
}

Menu::~Menu( void )
{
	// delete _text; // nope, this is done in ui as they share same ptr
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void Menu::reset_values( void )
{
	_selection = 0;
	_esc_released = false;
	_e_released = false;
	if (_selected_block.x != blocks::AIR) { // important to do this first as they use same variable
		_inventory.restoreBlock(_selected_block);
	}
	_inventory.restoreiCraft();
	_inventory.restoreCraft();
	_selected_block = glm::ivec2(blocks::AIR, 0);
	_furnace = NULL;
}

int Menu::main_menu( void )
{
	if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (_selection == 1) { //singleplayer
			_state = LOAD_MENU;
			reset_values();
			return (2);
		} else if (_selection == 6) { //quit game
			glfwSetWindowShouldClose(_window, GL_TRUE);
			return (-1);
		}
	}
	if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		_state = LOAD_MENU;
		reset_values();
		return (2);
	}

	setup_array_buffer_main();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	_text->displayText(WIN_WIDTH / 2 - 220, WIN_HEIGHT / 2 - 120 * 3, 24, glm::vec3(1.0f, 1.0f, 1.0f), "Press space to start");

	int mult = 3;
	// shadows
	_text->displayText(WIN_WIDTH / 2 - 110 + mult, WIN_HEIGHT / 2 - 10 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Singleplayer");
	_text->displayText(WIN_WIDTH / 2 - 100 + mult, WIN_HEIGHT / 2 + 15 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Multiplayer");
	_text->displayText(WIN_WIDTH / 2 - 150 + mult, WIN_HEIGHT / 2 + 40 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Minecraft Realms");
	_text->displayText(WIN_WIDTH / 2 - 50 * mult - 80 + mult, WIN_HEIGHT / 2 + 80 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Options...");
	_text->displayText(WIN_WIDTH / 2 + 50 * mult - 75 + mult, WIN_HEIGHT / 2 + 80 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Quit Game");
	// white
	_text->displayText(WIN_WIDTH / 2 - 110, WIN_HEIGHT / 2 - 10 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Singleplayer");
	_text->displayText(WIN_WIDTH / 2 - 100, WIN_HEIGHT / 2 + 15 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Multiplayer");
	_text->displayText(WIN_WIDTH / 2 - 150, WIN_HEIGHT / 2 + 40 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Minecraft Realms");
	_text->displayText(WIN_WIDTH / 2 - 50 * mult - 80, WIN_HEIGHT / 2 + 80 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Options...");
	_text->displayText(WIN_WIDTH / 2 + 50 * mult - 75, WIN_HEIGHT / 2 + 80 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Quit Game");
	return (0);
}

int Menu::loading_screen( GLint render_dist )
{
	GLint current_size = 0, newVaoCounter = 0;
	for (auto& c : _chunks) {
		mtx.lock();
		current_size += c->isLoaded(newVaoCounter);
		mtx.unlock();
	}

	GLint goal = (1 + 2 * render_dist) * (1 + 2 * render_dist);
	// std::cout << "CURRENT IS " << current_size << ", GOAL IS " << goal << std::endl;
	if (current_size >= goal) {
		_state = PAUSE_MENU;
		return (1);
	}
	float percentage = current_size / static_cast<float>(goal) * 100;

	setup_array_buffer_load(percentage * 10);
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	std::string str = "LOADING.. " + std::to_string(percentage) + '%' + " (" + std::to_string(current_size) + '/' + std::to_string(goal) + ')';
	_text->displayText(WIN_WIDTH / 2 - 24 * str.length() / 2, WIN_HEIGHT / 2 - 150, 24, glm::vec3(1.0f, 1.0f, 1.0f), str);
	return (0);
}

int Menu::pause_menu( void )
{
	if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (_selection == 1) { //Back to Game
			reset_values();
			return (1);
		} else if (_selection == 8) { //Save and Quit to Title
			_state = MAIN_MENU;
			reset_values();
			return (3);
		}
	}
	if (_esc_released && glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		_esc_released = false;
		return (1);
	} else if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
		_esc_released = true;
	}

	setup_array_buffer_pause();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	int mult = 3;
	// first draw text shadow
	// _text->displayText(WIN_WIDTH / 2 - 100 + mult, WIN_HEIGHT / 2 - 60 * mult - 40 + mult, 24, glm::vec3(0.0f, 0.0f, 0.0f), "Game Menu");
	_text->displayText(WIN_WIDTH / 2 - 110 + mult, WIN_HEIGHT / 2 - 60 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Back to Game");
	_text->displayText(WIN_WIDTH / 2 - 50 * mult - 120 + mult, WIN_HEIGHT / 2 - 35 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Advancements");
	_text->displayText(WIN_WIDTH / 2 - 50 * mult - 125 + mult, WIN_HEIGHT / 2 - 10 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Give Feedback");
	_text->displayText(WIN_WIDTH / 2 - 50 * mult - 80 + mult, WIN_HEIGHT / 2 + 15 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Options...");
	_text->displayText(WIN_WIDTH / 2 + 50 * mult - 70 + mult, WIN_HEIGHT / 2 - 35 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Statistics");
	_text->displayText(WIN_WIDTH / 2 + 50 * mult - 100 + mult, WIN_HEIGHT / 2 - 10 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Report Bugs");
	_text->displayText(WIN_WIDTH / 2 + 50 * mult - 100 + mult, WIN_HEIGHT / 2 + 15 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Open to LAN");
	_text->displayText(WIN_WIDTH / 2 - 200 + mult, WIN_HEIGHT / 2 + 40 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Save and Quit to Title");

	// then draw text in white
	_text->displayText(WIN_WIDTH / 2 - 100, WIN_HEIGHT / 2 - 60 * mult - 40, 24, glm::vec3(1.0f, 1.0f, 1.0f), "Game Menu");
	_text->displayText(WIN_WIDTH / 2 - 110, WIN_HEIGHT / 2 - 60 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Back to Game");
	_text->displayText(WIN_WIDTH / 2 - 50 * mult - 120, WIN_HEIGHT / 2 - 35 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Advancements");
	_text->displayText(WIN_WIDTH / 2 - 50 * mult - 125, WIN_HEIGHT / 2 - 10 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Give Feedback");
	_text->displayText(WIN_WIDTH / 2 - 50 * mult - 80, WIN_HEIGHT / 2 + 15 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Options...");
	_text->displayText(WIN_WIDTH / 2 + 50 * mult - 70, WIN_HEIGHT / 2 - 35 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Statistics");
	_text->displayText(WIN_WIDTH / 2 + 50 * mult - 100, WIN_HEIGHT / 2 - 10 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Report Bugs");
	_text->displayText(WIN_WIDTH / 2 + 50 * mult - 100, WIN_HEIGHT / 2 + 15 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Open to LAN");
	_text->displayText(WIN_WIDTH / 2 - 200, WIN_HEIGHT / 2 + 40 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Save and Quit to Title");
	return (0);
}

int Menu::ingame_inputs( void )
{
	int craft = _state + 1 - INVENTORY_MENU; // craft = 1: inventory, 2: crafting, 3: furnace
	if (_esc_released && glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		reset_values();
		return (1);
	} else if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
		_esc_released = true;
	}
	if (_e_released && glfwGetKey(_window, GLFW_KEY_E) == GLFW_PRESS) {
		reset_values();
		return (1);
	} else if (glfwGetKey(_window, GLFW_KEY_E) == GLFW_RELEASE) {
		_e_released = true;
	}
	if (_selection && glfwGetKey(_window, GLFW_KEY_Q) == GLFW_PRESS) {
		_inventory.removeBlockAt(_selection - 1, _furnace);
	}
	if (_left_released && glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		_left_released = false;
		if (_selection) {
			if (_selected_block.x == blocks::AIR) {
				_selected_block = _inventory.pickBlockAt(craft, _selection - 1, _furnace);
			} else {
				_selected_block = _inventory.putBlockAt(craft, _selection - 1, _selected_block, _furnace);
			}
		}
	} else if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		_left_released = true;
	}
	if (_right_released && glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		_right_released = false;
		if (_selection) {
			if (_selected_block.x == blocks::AIR) {
				_selected_block = _inventory.pickHalfBlockAt(craft, _selection - 1, _furnace);
			} else {
				_selected_block = _inventory.putOneBlockAt(craft, _selection - 1, _selected_block, _furnace);
			}
		}
	} else if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
		_right_released = true;
	}
	if ((glfwGetKey(_window, GLFW_KEY_1) == GLFW_PRESS) && ++_key_1 == 1) {
		_inventory.swapCells(0, _selection - 1);
	} else if (glfwGetKey(_window, GLFW_KEY_1) == GLFW_RELEASE) {
		_key_1 = 0;
	}
	if ((glfwGetKey(_window, GLFW_KEY_2) == GLFW_PRESS) && ++_key_2 == 1) {
		_inventory.swapCells(1, _selection - 1);
	} else if (glfwGetKey(_window, GLFW_KEY_2) == GLFW_RELEASE) {
		_key_2 = 0;
	}
	if ((glfwGetKey(_window, GLFW_KEY_3) == GLFW_PRESS) && ++_key_3 == 1) {
		_inventory.swapCells(2, _selection - 1);
	} else if (glfwGetKey(_window, GLFW_KEY_3) == GLFW_RELEASE) {
		_key_3 = 0;
	}
	if ((glfwGetKey(_window, GLFW_KEY_4) == GLFW_PRESS) && ++_key_4 == 1) {
		_inventory.swapCells(3, _selection - 1);
	} else if (glfwGetKey(_window, GLFW_KEY_4) == GLFW_RELEASE) {
		_key_4 = 0;
	}
	if ((glfwGetKey(_window, GLFW_KEY_5) == GLFW_PRESS) && ++_key_5 == 1) {
		_inventory.swapCells(4, _selection - 1);
	} else if (glfwGetKey(_window, GLFW_KEY_5) == GLFW_RELEASE) {
		_key_5 = 0;
	}
	if ((glfwGetKey(_window, GLFW_KEY_6) == GLFW_PRESS) && ++_key_6 == 1) {
		_inventory.swapCells(5, _selection - 1);
	} else if (glfwGetKey(_window, GLFW_KEY_6) == GLFW_RELEASE) {
		_key_6 = 0;
	}
	if ((glfwGetKey(_window, GLFW_KEY_7) == GLFW_PRESS) && ++_key_7 == 1) {
		_inventory.swapCells(6, _selection - 1);
	} else if (glfwGetKey(_window, GLFW_KEY_7) == GLFW_RELEASE) {
		_key_7 = 0;
	}
	if ((glfwGetKey(_window, GLFW_KEY_8) == GLFW_PRESS) && ++_key_8 == 1) {
		_inventory.swapCells(7, _selection - 1);
	} else if (glfwGetKey(_window, GLFW_KEY_8) == GLFW_RELEASE) {
		_key_8 = 0;
	}
	if ((glfwGetKey(_window, GLFW_KEY_9) == GLFW_PRESS) && ++_key_9 == 1) {
		_inventory.swapCells(8, _selection - 1);
	} else if (glfwGetKey(_window, GLFW_KEY_9) == GLFW_RELEASE) {
		_key_9 = 0;
	}
	return (0);
}

int Menu::ingame_menu( void )
{
	int ret = ingame_inputs();
	if (ret) {
		return (ret);
	}

	switch (_state) {
		case INVENTORY_MENU:
			setup_array_buffer_inventory();
			break ;
		case CRAFTING_MENU:
			setup_array_buffer_crafting();
			break ;
		case FURNACE_MENU:
			setup_array_buffer_furnace();
			break ;
	}

	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	for (int index = 0; index < 9; index++) {
		display_slot_value(index);
	}
	for (int index = 0; index < 27; index++) {
		display_backpack_value(index);
	}
	for (int index = 0; index < 4 + 5 * (_state == CRAFTING_MENU) - 1 * (_state == FURNACE_MENU); index++) {
		switch (_state) {
			case INVENTORY_MENU:
				display_icraft_value(index);
				break ;
			case CRAFTING_MENU:
				display_craft_value(index);
				break ;
		}
	}
	if (_state == FURNACE_MENU) {
		display_furnace_value();
	}
	mtx_inventory.lock();
	int value = _inventory.getCrafted().y;
	mtx_inventory.unlock();
	if (value > 1) {
		switch (_state) {
			case (INVENTORY_MENU):
				_text->displayText((WIN_WIDTH - (166 * 3)) / 2 + 155 * 3, WIN_HEIGHT / 2 - 49 * 3, 12, glm::vec3(1.0f, 1.0f, 1.0f), std::to_string(value));
				break ;
			case (CRAFTING_MENU):
				_text->displayText((WIN_WIDTH - (166 * 3)) / 2 + 125 * 3, WIN_HEIGHT / 2 - 42 * 3, 12, glm::vec3(1.0f, 1.0f, 1.0f), std::to_string(value));
				break ;
		}
	}
	if (_selected_block.y > 1) {
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		value = _selected_block.y;
		_text->displayText(mouseX - 6, mouseY - 6, 12, glm::vec3(1.0f, 1.0f, 1.0f), std::to_string(value));
	}
	return (0);
}

void Menu::setup_array_buffer_main( void )
{
	_nb_points = 8;
	int mult = 3;
    GLint vertices[] = { // pos: x y width height textcoord: x y width height
        1, WIN_WIDTH / 2 - 129 * mult, WIN_HEIGHT / 2 - 104 * mult, 256 * mult, 64 * mult, 0, 131, 256, 64, // MINECRAFT

        1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 10 * mult, 200 * mult, 20 * mult, 0, 91 + 20 * (_selection == 1), 200, 20, // Singleplayer
        1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 15 * mult, 200 * mult, 20 * mult, 0, 71, 200, 20, // Multiplayer
        1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 40 * mult, 200 * mult, 20 * mult, 0, 71, 200, 20, // Minecraft Realms

        1, WIN_WIDTH / 2 - 125 * mult, WIN_HEIGHT / 2 + 80 * mult, 15 * mult, 20 * mult, 0, 71, 200, 20, // Lang settings
        1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 80 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20, // Options...
        1, WIN_WIDTH / 2 + 5 * mult, WIN_HEIGHT / 2 + 80 * mult, 95 * mult, 20 * mult, 0, 91 + 20 * (_selection == 6), 200, 20, // Quit Game
        1, WIN_WIDTH / 2 + 110 * mult, WIN_HEIGHT / 2 + 80 * mult, 15 * mult, 20 * mult, 0, 71, 200, 20, // accessibility settings
    };

	setup_shader(vertices);
}

void Menu::setup_array_buffer_load( int completion )
{
	// std::cout << "current completion : " << completion << std::endl;
	_nb_points = 1 + 100 + 1; // background + 10 * 10 cubes + breaking frame
	GLint *vertices = new GLint[9 * _nb_points]; // pos: x y width height textcoord: x y width height

	vertices[0] = 1;
	vertices[1] = WIN_WIDTH / 2 - 110;
	vertices[2] = WIN_HEIGHT / 2 - 120;
	vertices[3] = 220;
	vertices[4] = 240;
	vertices[5] = 0;
	vertices[6] = 71;
	vertices[7] = 200;
	vertices[8] = 20;

	int step = 1, turnCounter = 0, numSteps = 1, stepSize = 20, state = 0, x = WIN_WIDTH / 2 - 20, y = WIN_HEIGHT / 2;
	for (; step < 1 + 100; step++) {
		vertices[step * 9] = 0;
		vertices[step * 9 + 1] = x;
		vertices[step * 9 + 2] = y;
		vertices[step * 9 + 3] = stepSize;
		vertices[step * 9 + 4] = stepSize;
		if (step < 1 + completion / 10) {
			vertices[step * 9 + 5] = 64; // cobblestone
			vertices[step * 9 + 6] = 48;
		} else {
			vertices[step * 9 + 5] = 64; // stone
			vertices[step * 9 + 6] = 32;
		}
		vertices[step * 9 + 7] = 16;
		vertices[step * 9 + 8] = 16;
		if (step == 1 + completion / 10) {
			vertices[101 * 9] = 0;
			vertices[101 * 9 + 1] = x;
			vertices[101 * 9 + 2] = y;
			vertices[101 * 9 + 3] = stepSize;
			vertices[101 * 9 + 4] = stepSize;
			vertices[101 * 9 + 5] = 240; // break frame
			vertices[101 * 9 + 6] = 32 * ((completion % 10) != 0) + 16 * (completion % 10);
			vertices[101 * 9 + 7] = 16;
			vertices[101 * 9 + 8] = 16;
		}
		switch (state) {
			case 0:
				x += stepSize;
				break ;
			case 1:
				y -= stepSize;
				break ;
			case 2:
				x -= stepSize;
				break ;
			case 3:
				y += stepSize;
				break ;
		}
		if (step % numSteps == 0) {
			state = (state + 1) % 4;
			turnCounter++;
			if (turnCounter % 2 == 0) {
				numSteps++;
			}
		}
	}

	setup_shader(vertices);
	delete [] vertices;
}

void Menu::setup_array_buffer_pause( void )
{
	_nb_points = 9;
	int mult = 3;
    GLint vertices[] = { // pos: x y width height textcoord: x y width height
		1, 0, 0, WIN_WIDTH, WIN_HEIGHT, 3, 29, 1, 1, // occult window
        1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 60 * mult, 200 * mult, 20 * mult, 0, 91 + 20 * (_selection == 1), 200, 20, // Back to Game

        1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 35 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20, // Advancements
        1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 10 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20, // Give Feedback
        1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 15 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20, // Options...

        1, WIN_WIDTH / 2 + 5 * mult, WIN_HEIGHT / 2 - 35 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20, // Statistics
        1, WIN_WIDTH / 2 + 5 * mult, WIN_HEIGHT / 2 - 10 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20, // Report Bugs
        1, WIN_WIDTH / 2 + 5 * mult, WIN_HEIGHT / 2 + 15 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20, // Open to LAN

        1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 40 * mult, 200 * mult, 20 * mult, 0, 91 + 20 * (_selection == 8), 200, 20, // Save and Quit to Title
    };

	setup_shader(vertices);
}

void Menu::display_slot_value( int index )
{
	if (index < 0 || index >= 9) {
		return ;
	}
	int mult = 3, value = _inventory.getSlotBlock(index).y;
	if (value > 1) {
		_text->displayText((WIN_WIDTH - (166 * mult)) / 2 + 18 * index * mult + mult * 10 - 2 * mult * (value > 9), WIN_HEIGHT / 2 + (59 + 6) * mult, 12, glm::vec3(1.0f, 1.0f, 1.0f), std::to_string(value));
	}
}

void Menu::display_backpack_value( int index )
{
	if (index < 0 || index >= 27) {
		return ;
	}
	int mult = 3, value = _inventory.getBackpackBlock(index).y;
	if (value > 1) {
		_text->displayText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (index % 9) * mult) + mult * 10 - 2 * mult * (value > 9), WIN_HEIGHT / 2 + 7 * mult + 18 * mult * (index / 9), 12, glm::vec3(1.0f, 1.0f, 1.0f), std::to_string(value));
	}
}

void Menu::display_icraft_value( int index )
{
	if (index < 0 || index >= 4) {
		return ;
	}
	int mult = 3, value = _inventory.getiCraftBlock(index).y;
	if (value > 1) {
		_text->displayText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (5 + index % 2) * mult) + mult * 10 - 2 * mult * (value > 9), WIN_HEIGHT / 2 - 59 * mult + 18 * mult * (index / 2), 12, glm::vec3(1.0f, 1.0f, 1.0f), std::to_string(value));
	}
}

void Menu::display_furnace_value( void )
{
	if (!_furnace) {
		return ;
	}
	int mult = 3, value = _furnace->getComposant().y;
	if (value > 1) {
		_text->displayText((WIN_WIDTH - (166 * mult)) / 2 + 51 * mult + mult * 7 - 2 * mult * (value > 9),  WIN_HEIGHT / 2 - 60 * mult, 12, glm::vec3(1.0f, 1.0f, 1.0f), std::to_string(value));
	}
	value = _furnace->getFuel().y;
	if (value > 1) {
		_text->displayText((WIN_WIDTH - (166 * mult)) / 2 + 51 * mult + mult * 7 - 2 * mult * (value > 9), WIN_HEIGHT / 2 - 24 * mult, 12, glm::vec3(1.0f, 1.0f, 1.0f), std::to_string(value));
	}
	value = _furnace->getProduction().y;
	if (value > 1) {
		_text->displayText((WIN_WIDTH - (166 * mult)) / 2 + 111 * mult + mult * 7 - 2 * mult * (value > 9), WIN_HEIGHT / 2 - 42 * mult, 12, glm::vec3(1.0f, 1.0f, 1.0f), std::to_string(value));
	}
}

void Menu::display_craft_value( int index )
{
	if (index < 0 || index >= 9) {
		return ;
	}
	int mult = 3, value = _inventory.getCraftBlock(index).y;
	if (value > 1) {
		_text->displayText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (1 + index % 3) * mult) + mult * 14 - 2 * mult * (value > 9), WIN_HEIGHT / 2 - 60 * mult + 18 * mult * (index / 3), 12, glm::vec3(1.0f, 1.0f, 1.0f), std::to_string(value));
	}
}

void Menu::add_slot_value( GLint *vertices, int mult, int index, int & vindex )
{
	mtx_inventory.lock();
	int value = _inventory.getSlotBlock(index).x;
	mtx_inventory.unlock();
	if (value == blocks::AIR) {
		return ;
	}
	// std::cout << "nb points " << _nb_points << ", vindex at " << vindex << std::endl;
	vertices[vindex + 0] = 0;
	vertices[vindex + 1] = (WIN_WIDTH - (166 * mult)) / 2 + (18 * index * mult) + mult * 3;
	vertices[vindex + 2] = WIN_HEIGHT / 2 + 59 * mult;
	vertices[vindex + 3] = 16 * mult;
	vertices[vindex + 4] = 16 * mult;
	vertices[vindex + 5] = blockAtlasX(value);
	vertices[vindex + 6] = blockAtlasY(value);
	vertices[vindex + 7] = 16;
	vertices[vindex + 8] = 16;
	vindex += 9;
}

void Menu::add_backpack_value( GLint *vertices, int mult, int index, int & vindex )
{
	mtx_inventory.lock();
	int value = _inventory.getBackpackBlock(index).x;
	mtx_inventory.unlock();
	if (value == blocks::AIR) {
		return ;
	}
	// std::cout << "in back; nb points " << _nb_points << ", vindex at " << vindex << std::endl;
	vertices[vindex + 0] = 0;
	vertices[vindex + 1] = (WIN_WIDTH - (166 * mult)) / 2 + (18 * (index % 9) * mult) + mult * 3;
	vertices[vindex + 2] = WIN_HEIGHT / 2 + mult + 18 * mult * (index / 9);
	vertices[vindex + 3] = 16 * mult;
	vertices[vindex + 4] = 16 * mult;
	vertices[vindex + 5] = blockAtlasX(value);
	vertices[vindex + 6] = blockAtlasY(value);
	vertices[vindex + 7] = 16;
	vertices[vindex + 8] = 16;
	vindex += 9;
}

void Menu::add_icraft_value( GLint *vertices, int mult, int index, int & vindex )
{
	mtx_inventory.lock();
	int value = _inventory.getiCraftBlock(index).x;
	mtx_inventory.unlock();
	if (value == blocks::AIR) {
		return ;
	}
	// std::cout << "in back; nb points " << _nb_points << ", vindex at " << vindex << std::endl;
	vertices[vindex + 0] = 0;
	vertices[vindex + 1] = (WIN_WIDTH - (166 * mult)) / 2 + (18 * (5 + index % 2) * mult) + mult * 3;
	vertices[vindex + 2] = WIN_HEIGHT / 2 - 65 * mult + 18 * mult * (index / 2);
	vertices[vindex + 3] = 16 * mult;
	vertices[vindex + 4] = 16 * mult;
	vertices[vindex + 5] = blockAtlasX(value);
	vertices[vindex + 6] = blockAtlasY(value);
	vertices[vindex + 7] = 16;
	vertices[vindex + 8] = 16;
	vindex += 9;
}

void Menu::add_craft_value( GLint *vertices, int mult, int index, int & vindex )
{
	mtx_inventory.lock();
	int value = _inventory.getCraftBlock(index).x;
	mtx_inventory.unlock();
	if (value == blocks::AIR) {
		return ;
	}
	// std::cout << "in back; nb points " << _nb_points << ", vindex at " << vindex << std::endl;
	vertices[vindex + 0] = 0;
	vertices[vindex + 1] = (WIN_WIDTH - (166 * mult)) / 2 + (18 * (1 + index % 3) * mult) + mult * 7;
	vertices[vindex + 2] = WIN_HEIGHT / 2 - 66 * mult + 18 * mult * (index / 3);
	vertices[vindex + 3] = 16 * mult;
	vertices[vindex + 4] = 16 * mult;
	vertices[vindex + 5] = blockAtlasX(value);
	vertices[vindex + 6] = blockAtlasY(value);
	vertices[vindex + 7] = 16;
	vertices[vindex + 8] = 16;
	vindex += 9;
}

static int screenPosXFromlocation( int mult, int location )
{
	if (location < 9) { // action
		return ((WIN_WIDTH - (166 * mult)) / 2 + (18 * location * mult) + mult * 3);
	} else if (location < 36) { // backpack
		return ((WIN_WIDTH - (166 * mult)) / 2 + (18 * (location % 9) * mult) + mult * 3);
	} else if (location < 40) { // icraft
		return ((WIN_WIDTH - (166 * mult)) / 2 + (18 * (5 + location % 2) * mult) + mult * 3);
	} else if (location == 41) { // crafted, ignore
	} else { // craft
		return ((WIN_WIDTH - (166 * mult)) / 2 + (18 * (1 + (location - 41) % 3) * mult) + mult * 7);
	}
	return (0);
}

static int screenPosYFromlocation( int mult, int location )
{
	if (location < 9) {
		return (WIN_HEIGHT / 2 + 59 * mult);
	} else if (location < 36) {
		return (WIN_HEIGHT / 2 + mult + 18 * mult * ((location - 9) / 9));
	} else if (location < 40) {
		return (WIN_HEIGHT / 2 - 65 * mult + 18 * mult * ((location - 36) / 2));
	} else if (location == 41) {
	} else {
		return (WIN_HEIGHT / 2 - 66 * mult + 18 * mult * ((location - 41) / 3));
	}
	return (0);
}

void Menu::add_dura_value( GLint *vertices, int mult, int index, int & vindex )
{
	mtx_inventory.lock();
	glm::ivec3 value = _inventory.getDuraFromIndex(index);
	mtx_inventory.unlock();
	if (value.y == 0) {
		return ;
	}
	// adding grey bar first
	vertices[vindex + 0] = 0;
	vertices[vindex + 1] = screenPosXFromlocation(mult, value.x) + mult;
	vertices[vindex + 2] = screenPosYFromlocation(mult, value.x) + 14 * mult;
	vertices[vindex + 3] = 14 * mult;
	vertices[vindex + 4] = mult;
	vertices[vindex + 5] = 64;
	vertices[vindex + 6] = 0;
	vertices[vindex + 7] = 1;
	vertices[vindex + 8] = 1;
	vindex += 9;
	vertices[vindex + 0] = 0; // adding progress bar second
	vertices[vindex + 1] = screenPosXFromlocation(mult, value.x) + mult;
	vertices[vindex + 2] = screenPosYFromlocation(mult, value.x) + 14 * mult;
	float percent = 1.0f * value.y / value.z;
	vertices[vindex + 3] = 14 * mult * percent;
	vertices[vindex + 4] = mult;
	vertices[vindex + 5] = 103 * (percent < 0.6f) - (percent < 0.3);
	vertices[vindex + 6] = 16 + 9 * (percent < 0.6f) - 18 * (percent < 0.3f);
	vertices[vindex + 7] = 1;
	vertices[vindex + 8] = 1;
	vindex += 9;
}

void Menu::add_crafted_value( GLint *vertices, int mult, int & vindex )
{
	mtx_inventory.lock();
	int value = _inventory.getCrafted().x;
	mtx_inventory.unlock();
	if (value == blocks::AIR) {
		return ;
	}
	// std::cout << "in back; nb points " << _nb_points << ", vindex at " << vindex << std::endl;
	vertices[vindex + 0] = 0;
	if (_state == INVENTORY_MENU) {
		vertices[vindex + 1] = (WIN_WIDTH - (166 * mult)) / 2 + 149 * mult;
		vertices[vindex + 2] = WIN_HEIGHT / 2 - 55 * mult;
	} else if (_state == CRAFTING_MENU) {
		vertices[vindex + 1] = (WIN_WIDTH - (166 * mult)) / 2 + 119 * mult;
		vertices[vindex + 2] = WIN_HEIGHT / 2 - 48 * mult;
	}
	vertices[vindex + 3] = 16 * mult;
	vertices[vindex + 4] = 16 * mult;
	vertices[vindex + 5] = blockAtlasX(value);
	vertices[vindex + 6] = blockAtlasY(value);
	vertices[vindex + 7] = 16;
	vertices[vindex + 8] = 16;
	vindex += 9;
}

void Menu::setup_array_buffer_inventory( void )
{
	mtx_inventory.lock();
	int duras = _inventory.countDura();
	_nb_points = 1 + _inventory.countSlots() + _inventory.countBackpack() + _inventory.countiCraft() + 2 * duras + _inventory.getCrafted().x + (_selected_block.x != blocks::AIR);
	mtx_inventory.unlock();
	int mult = 3;
    GLint *vertices = new GLint[_nb_points * 9]; // pos: x y width height textcoord: x y width height

	vertices[0] = 2;
	vertices[1] = WIN_WIDTH / 2 - 88 * mult;
	vertices[2] = WIN_HEIGHT / 2 - 83 * mult;
	vertices[3] = 176 * mult;
	vertices[4] = 166 * mult;
	vertices[5] = 0;
	vertices[6] = 0;
	vertices[7] = 128; // textcoord are in 256*256
	vertices[8] = 128;

	int vindex = 9;
	for (int index = 0; index < 9; index++) {
		add_slot_value(vertices, mult, index, vindex);
	}
	for (int index = 0; index < 27; index++) {
		add_backpack_value(vertices, mult, index, vindex);
	}
	for (int index = 0; index < 4; index++) {
		add_icraft_value(vertices, mult, index, vindex);
	}
	for (int index = 0; index < duras; index++) {
		add_dura_value(vertices, mult, index, vindex);
	}
	add_crafted_value(vertices, mult, vindex);

	if (_selected_block.x != blocks::AIR) {
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		vertices[vindex + 0] = 0;
		vertices[vindex + 1] = mouseX - 8 * mult;
		vertices[vindex + 2] = mouseY - 8 * mult;
		vertices[vindex + 3] = 16 * mult;
		vertices[vindex + 4] = 16 * mult;
		vertices[vindex + 5] = blockAtlasX(_selected_block.x);
		vertices[vindex + 6] = blockAtlasY(_selected_block.x);
		vertices[vindex + 7] = 16;
		vertices[vindex + 8] = 16;
	}

	setup_shader(vertices);
	delete [] vertices;
}

void Menu::setup_array_buffer_crafting( void )
{
	mtx_inventory.lock();
	int duras = _inventory.countDura();
	_nb_points = 1 + _inventory.countSlots() + _inventory.countBackpack() + _inventory.countCraft() + 2 * duras + _inventory.getCrafted().x + (_selected_block.x != blocks::AIR);
	mtx_inventory.unlock();
	int mult = 3;
    GLint *vertices = new GLint[_nb_points * 9]; // pos: x y width height textcoord: x y width height

	vertices[0] = 2;
	vertices[1] = WIN_WIDTH / 2 - 88 * mult;
	vertices[2] = WIN_HEIGHT / 2 - 83 * mult;
	vertices[3] = 176 * mult;
	vertices[4] = 166 * mult;
	vertices[5] = 128;
	vertices[6] = 0;
	vertices[7] = 128; // textcoord are in 256*256
	vertices[8] = 128;

	int vindex = 9;
	for (int index = 0; index < 9; index++) {
		add_slot_value(vertices, mult, index, vindex);
	}
	for (int index = 0; index < 27; index++) {
		add_backpack_value(vertices, mult, index, vindex);
	}
	for (int index = 0; index < duras; index++) {
		add_dura_value(vertices, mult, index, vindex);
	}
	for (int index = 0; index < 9; index++) {
		add_craft_value(vertices, mult, index, vindex);
	}
	add_crafted_value(vertices, mult, vindex);

	if (_selected_block.x != blocks::AIR) {
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		vertices[vindex + 0] = 0;
		vertices[vindex + 1] = mouseX - 8 * mult;
		vertices[vindex + 2] = mouseY - 8 * mult;
		vertices[vindex + 3] = 16 * mult;
		vertices[vindex + 4] = 16 * mult;
		vertices[vindex + 5] = blockAtlasX(_selected_block.x);
		vertices[vindex + 6] = blockAtlasY(_selected_block.x);
		vertices[vindex + 7] = 16;
		vertices[vindex + 8] = 16;
	}

	setup_shader(vertices);
	delete [] vertices;
}

void Menu::add_furnace_value( GLint *vertices, int mult, int & vindex )
{
	int value = _furnace->getComposant().x;
	if (value != blocks::AIR) {
		vertices[vindex + 0] = 0;
		vertices[vindex + 1] = (WIN_WIDTH - (166 * mult)) / 2 + 51 * mult;
		vertices[vindex + 2] = WIN_HEIGHT / 2 - 66 * mult;
		vertices[vindex + 3] = 16 * mult;
		vertices[vindex + 4] = 16 * mult;
		vertices[vindex + 5] = blockAtlasX(value);
		vertices[vindex + 6] = blockAtlasY(value);
		vertices[vindex + 7] = 16;
		vertices[vindex + 8] = 16;
		vindex += 9;
	}
	int progress = 1 + glm::floor(_furnace->getFuelTime() * 13);
	if (_furnace->getFuelTime()) {
		vertices[vindex + 0] = 1;
		vertices[vindex + 1] = (WIN_WIDTH - (166 * mult)) / 2 + 51 * mult + mult;
		vertices[vindex + 2] = WIN_HEIGHT / 2 - 48 * mult + (14 - progress) * mult + mult;
		vertices[vindex + 3] = 14 * mult;
		vertices[vindex + 4] = progress * mult;
		vertices[vindex + 5] = 24;
		vertices[vindex + 6] = 47 + (14 - progress);
		vertices[vindex + 7] = 14;
		vertices[vindex + 8] = progress;
		vindex += 9;
	}
	progress = 1 + glm::floor(_furnace->getComposantTime() * 23);
	if (_furnace->getComposantTime()) {
		vertices[vindex + 0] = 1;
		vertices[vindex + 1] = (WIN_WIDTH - (166 * mult)) / 2 + 74 * mult;
		vertices[vindex + 2] = WIN_HEIGHT / 2 - 48 * mult;
		vertices[vindex + 3] = progress * mult;
		vertices[vindex + 4] = 17 * mult;
		vertices[vindex + 5] = 38;
		vertices[vindex + 6] = 47;
		vertices[vindex + 7] = progress;
		vertices[vindex + 8] = 17;
		vindex += 9;
	}
	value = _furnace->getFuel().x;
	if (value != blocks::AIR) {
		vertices[vindex + 0] = 0;
		vertices[vindex + 1] = (WIN_WIDTH - (166 * mult)) / 2 + 51 * mult;
		vertices[vindex + 2] = WIN_HEIGHT / 2 - 30 * mult;
		vertices[vindex + 3] = 16 * mult;
		vertices[vindex + 4] = 16 * mult;
		vertices[vindex + 5] = blockAtlasX(value);
		vertices[vindex + 6] = blockAtlasY(value);
		vertices[vindex + 7] = 16;
		vertices[vindex + 8] = 16;
		vindex += 9;
	}
	value = _furnace->getProduction().x;
	if (value != blocks::AIR) {
		vertices[vindex + 0] = 0;
		vertices[vindex + 1] = (WIN_WIDTH - (166 * mult)) / 2 + 111 * mult;
		vertices[vindex + 2] = WIN_HEIGHT / 2 - 48 * mult;
		vertices[vindex + 3] = 16 * mult;
		vertices[vindex + 4] = 16 * mult;
		vertices[vindex + 5] = blockAtlasX(value);
		vertices[vindex + 6] = blockAtlasY(value);
		vertices[vindex + 7] = 16;
		vertices[vindex + 8] = 16;
		vindex += 9;
	}
}

void Menu::setup_array_buffer_furnace( void )
{
	int furnaceCount = (_furnace) ? _furnace->count() : 0;
	// std::cout << "FURNACE count is " << furnaceCount << std::endl;
	_nb_points = 1 + _inventory.countSlots() + _inventory.countBackpack() + furnaceCount + (_selected_block.x != blocks::AIR);
	int mult = 3;
    GLint *vertices = new GLint[_nb_points * 9]; // pos: x y width height textcoord: x y width height

	vertices[0] = 2;
	vertices[1] = WIN_WIDTH / 2 - 88 * mult;
	vertices[2] = WIN_HEIGHT / 2 - 83 * mult;
	vertices[3] = 176 * mult;
	vertices[4] = 166 * mult;
	vertices[5] = 0;
	vertices[6] = 128;
	vertices[7] = 128; // textcoord are in 256*256
	vertices[8] = 128;

	int vindex = 9;
	for (int index = 0; index < 9; index++) {
		add_slot_value(vertices, mult, index, vindex);
	}
	for (int index = 0; index < 27; index++) {
		add_backpack_value(vertices, mult, index, vindex);
	}
	if (furnaceCount) {
		add_furnace_value(vertices, mult, vindex);
	}

	if (_selected_block.x != blocks::AIR) {
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		vertices[vindex + 0] = 0;
		vertices[vindex + 1] = mouseX - 8 * mult;
		vertices[vindex + 2] = mouseY - 8 * mult;
		vertices[vindex + 3] = 16 * mult;
		vertices[vindex + 4] = 16 * mult;
		vertices[vindex + 5] = blockAtlasX(_selected_block.x);
		vertices[vindex + 6] = blockAtlasY(_selected_block.x);
		vertices[vindex + 7] = 16;
		vertices[vindex + 8] = 16;
	}

	setup_shader(vertices);
	delete [] vertices;
}

void Menu::setup_shader( GLint *vertices )
{
	glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
	_vaoSet = true;

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _nb_points * 9 * sizeof(GLint), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(UI_ATLASATTRIB);
	glVertexAttribIPointer(UI_ATLASATTRIB, 1, GL_INT, 9 * sizeof(GLint), 0);

    glEnableVertexAttribArray(UI_POSATTRIB);
	glVertexAttribIPointer(UI_POSATTRIB, 4, GL_INT, 9 * sizeof(GLint), (void *)(sizeof(GLint)));
	
	glEnableVertexAttribArray(UI_TEXATTRIB);
	glVertexAttribIPointer(UI_TEXATTRIB, 4, GL_INT, 9 * sizeof(GLint), (void *)(5 * sizeof(GLint)));

	check_glstate("NO");
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

static bool inRectangle( float posX, float posY, int rx, int ry, int width, int height )
{
	return (posX >= rx && posX <= rx + width && posY >= ry && posY <= ry + height);
}

void Menu::processMouseMovement( float posX, float posY )
{
	if (_state == MAIN_MENU) {
		int mult = 3;
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 10 * mult, 200 * mult, 20 * mult)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * mult, WIN_HEIGHT / 2 + 80 * mult, 95 * mult, 20 * mult)) {
			_selection = 6;
		} else {
			_selection = 0;
		}
	} else if (_state == PAUSE_MENU) {
		int mult = 3;
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 60 * mult, 200 * mult, 20 * mult)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 40 * mult, 200 * mult, 20 * mult)) {
			_selection = 8;
		} else {
			_selection = 0;
		}
	} else if (_state >= INVENTORY_MENU) {
		int mult = 3;
		for (int index = 0; index < 9; index++) {
			if (inRectangle(posX, posY, (WIN_WIDTH - (166 * mult)) / 2 + (18 * index * mult) + mult * 3, WIN_HEIGHT / 2 + 59 * mult, 16 * mult, 16 * mult)) {
				_selection = index + 1;
				return ;
			}
		}
		for (int index = 0; index < 27; index++) {
			if (inRectangle(posX, posY, (WIN_WIDTH - (166 * mult)) / 2 + (18 * (index % 9) * mult) + mult * 3, WIN_HEIGHT / 2 + mult + 18 * mult * (index / 9), 16 * mult, 16 * mult)) {
				_selection = index + 10;
				return ;
			}
		}
		if (_state == INVENTORY_MENU) {
			for (int index = 0; index < 4; index++) {
				if (inRectangle(posX, posY, (WIN_WIDTH - (166 * mult)) / 2 + (18 * (5 + index % 2) * mult) + mult * 3, WIN_HEIGHT / 2 - 65 * mult + 18 * mult * (index / 2), 16 * mult, 16 * mult)) {
					_selection = index + 37;
					return ;
				}
			}
			if (inRectangle(posX, posY, (WIN_WIDTH - (166 * mult)) / 2 + 149 * mult, WIN_HEIGHT / 2 - 55 * mult, 16 * mult, 16 * mult)) {
				_selection = 41;
				return ;
			}
		} else if (_state == CRAFTING_MENU) {
			for (int index = 0; index < 9; index++) {
				if (inRectangle(posX, posY, (WIN_WIDTH - (166 * mult)) / 2 + (18 * (1 + index % 3) * mult) + mult * 7, WIN_HEIGHT / 2 - 66 * mult + 18 * mult * (index / 3), 16 * mult, 16 * mult)) {
					_selection = index + 42;
					return ;
				}
			}
			if (inRectangle(posX, posY, (WIN_WIDTH - (166 * mult)) / 2 + 119 * mult, WIN_HEIGHT / 2 - 48 * mult, 16 * mult, 16 * mult)) {
				_selection = 41;
				return ;
			}
		} else if (_state == FURNACE_MENU) {
			if (inRectangle(posX, posY, (WIN_WIDTH - (166 * mult)) / 2 + 51 * mult, WIN_HEIGHT / 2 - 66 * mult, 16 * mult, 16 * mult)) {
				_selection = 51;
				return ;
			}
			if (inRectangle(posX, posY, (WIN_WIDTH - (166 * mult)) / 2 + 51 * mult, WIN_HEIGHT / 2 - 30 * mult, 16 * mult, 16 * mult)) {
				_selection = 52;
				return ;
			}
			if (inRectangle(posX, posY, (WIN_WIDTH - (166 * mult)) / 2 + 111 * mult, WIN_HEIGHT / 2 - 48 * mult, 16 * mult, 16 * mult)) {
				_selection = 41;
				return ;
			}
		}
		_selection = 0;
	}
}

void Menu::setWindow( GLFWwindow *window )
{
	_window = window;
}

void Menu::setShaderProgram( GLuint shaderProgram )
{
	_shaderProgram = shaderProgram;
}

void Menu::setChunks( std::list<Chunk *> chunks )
{
	if (_state == LOAD_MENU) {
		_chunks = chunks;
	}
}

void Menu::setFurnaceInstance( FurnaceInstance *furnace )
{
	_furnace = furnace;
}

void Menu::setState( int state )
{
	_state = state;
	if (!IS_LINUX) {
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		if (glfwRawMouseMotionSupported()) {
			glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
		}
	}
	set_cursor_position_callback(NULL, this);
	set_scroll_callback(NULL);
}

int Menu::getState( void )
{
	return (_state);
}

int Menu::run( GLint render_dist )
{
	if (glfwGetKey(_window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(_window, GL_TRUE);
		return (-1);
	}

	switch (_state) {
		case MAIN_MENU:
			return (main_menu());
		case LOAD_MENU:
			return (loading_screen(render_dist));
		case PAUSE_MENU:
			return (pause_menu());
		case INVENTORY_MENU:
			return (ingame_menu());
		case CRAFTING_MENU:
			return (ingame_menu());
		case FURNACE_MENU:
			return (ingame_menu());
		default:
			return (1);
	}
	return (0);
}

std::string Menu::getFurnaceString( void )
{
	if (!_furnace) {
		return ("");
	}
	return (_furnace->getInfoString());
}
