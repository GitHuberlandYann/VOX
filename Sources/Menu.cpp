#include "Menu.hpp"
#include "Ui.hpp"
#include "callbacks.hpp"
#include <dirent.h>
extern std::mutex mtx;
extern std::mutex mtx_inventory;
extern siv::PerlinNoise::seed_type perlin_seed;

Menu::Menu( Inventory & inventory, UI *ui ) : _state(MAIN_MENU), _selection(0), _selected_world(0), _vaoSet(false),
	_esc_released(false), _e_released(false), _left_released(false), _right_released(false), _textBar(true),
	_key_1(0), _key_2(0), _key_3(0), _key_4(0), _key_5(0), _key_6(0), _key_7(0), _key_8(0), _key_9(0), _chat_released(0),
	_inventory(inventory), _ui(ui), _text(ui->getTextPtr()), _chat(ui->getChatPtr()), _chest(NULL), _furnace(NULL)
{
	_selected_block = glm::ivec2(blocks::AIR, 0);
	_world_file = "";
}

Menu::~Menu( void )
{
	std::cout << "Destructor of Menu called" << std::endl;
	_worlds.clear();
	_selection_list.clear();
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
	_inventory.setModif(true);
	_selected_block = glm::ivec2(blocks::AIR, 0);
	if (_chest) {
		_chest->setState(chest_state::CLOSING);
		_chest = NULL;
	}
	_furnace = NULL;
	_selected_world = 0;
	_worlds.clear();
	_selection_list.clear();
}

int Menu::main_menu( void )
{
	if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (_selection == 1) { //singleplayer
			_state = WORLD_SELECT_MENU;
			reset_values();			
			DIR *dir = opendir("Worlds");
			if (dir == NULL) {
				std::cerr << "failed to open dir Worlds/" << std::endl;
				return (0);
			}
			struct dirent *dent;
			while ((dent = readdir(dir)) != NULL)
			{
				std::string file = dent->d_name;
				if (file != "." && file != ".." && file.size() >= 6 && !file.compare(file.size() - 5, 5, ".json")) {
					_worlds.push_back(file);
				}
			}
			closedir(dir);
			return (0);
		} else if (_selection == 6) { //quit game
			glfwSetWindowShouldClose(_window, GL_TRUE);
			return (-1);
		}
	}
	if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS) { // skip world selection and launch with default seed
		_state = LOAD_MENU;
		reset_values();
		return (4);
	}

	setup_array_buffer_main();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	_text->addText(WIN_WIDTH / 2 - 220, WIN_HEIGHT / 2 - 120 * 3, 24, true, "Press space to start");

	int mult = 3;
	// shadows
	_text->addText(WIN_WIDTH / 2 - 110 + mult, WIN_HEIGHT / 2 - 10 * mult + 6 * mult + mult, 20, false, "Singleplayer");
	_text->addText(WIN_WIDTH / 2 - 100 + mult, WIN_HEIGHT / 2 + 15 * mult + 6 * mult + mult, 20, false, "Multiplayer");
	_text->addText(WIN_WIDTH / 2 - 150 + mult, WIN_HEIGHT / 2 + 40 * mult + 6 * mult + mult, 20, false, "Minecraft Realms");
	_text->addText(WIN_WIDTH / 2 - 50 * mult - 80 + mult, WIN_HEIGHT / 2 + 80 * mult + 6 * mult + mult, 20, false, "Options...");
	_text->addText(WIN_WIDTH / 2 + 50 * mult - 75 + mult, WIN_HEIGHT / 2 + 80 * mult + 6 * mult + mult, 20, false, "Quit Game");
	// white
	_text->addText(WIN_WIDTH / 2 - 110, WIN_HEIGHT / 2 - 10 * mult + 6 * mult, 20, true, "Singleplayer");
	_text->addText(WIN_WIDTH / 2 - 100, WIN_HEIGHT / 2 + 15 * mult + 6 * mult, 20, true, "Multiplayer");
	_text->addText(WIN_WIDTH / 2 - 150, WIN_HEIGHT / 2 + 40 * mult + 6 * mult, 20, true, "Minecraft Realms");
	_text->addText(WIN_WIDTH / 2 - 50 * mult - 80, WIN_HEIGHT / 2 + 80 * mult + 6 * mult, 20, true, "Options...");
	_text->addText(WIN_WIDTH / 2 + 50 * mult - 75, WIN_HEIGHT / 2 + 80 * mult + 6 * mult, 20, true, "Quit Game");
	return (0);
}

int Menu::world_select_menu( void )
{
	if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (_selection == 1) { //play selected world
			_world_file = _worlds[_selected_world - 1];
			_state = LOAD_MENU;
			reset_values();
			return (2);
		} else if (_selection == 4) { // new random seed
			const auto p1 = std::chrono::system_clock::now(); // works without #include <chrono> #include <ctime> ?
			perlin_seed = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();
			_state = LOAD_MENU;
			reset_values();
			return (4);
		} else if (_selection == 6) { //cancel, go back to main menu
			_state = MAIN_MENU;
			reset_values();
			return (0);
		} else if (_selection > 6) {
			_selected_world = _selection - 6;
		}
	}

	setup_array_buffer_select();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	_text->addText(WIN_WIDTH / 2 - 120, 30, 24, true, "Select World");

	int mult = 3;
	// shadows
	_text->addText(WIN_WIDTH / 2 - 406 + mult, WIN_HEIGHT / 2 + 90 * mult + 6 * mult + mult, 20, false, "Play Selected World");
	_text->addText(WIN_WIDTH / 2 - 390 + mult, WIN_HEIGHT / 2 + 115 * mult + 6 * mult + mult, 20, false, "Edit");
	_text->addText(WIN_WIDTH / 2 - 180 + mult, WIN_HEIGHT / 2 + 115 * mult + 6 * mult + mult, 20, false, "Delete");
	_text->addText(WIN_WIDTH / 2 + 76 + mult, WIN_HEIGHT / 2 + 90 * mult + 6 * mult + mult, 20, false, "Create New World");
	_text->addText(WIN_WIDTH / 2 + 40 * mult - 80 + mult, WIN_HEIGHT / 2 + 115 * mult + 6 * mult + mult, 20, false, "Re-Create");
	_text->addText(WIN_WIDTH / 2 + 128 * mult - 80 + mult, WIN_HEIGHT / 2 + 115 * mult + 6 * mult + mult, 20, false, "Cancel");
	// white
	_text->addText(WIN_WIDTH / 2 - 406, WIN_HEIGHT / 2 + 90 * mult + 6 * mult, 20, true, "Play Selected World");
	_text->addText(WIN_WIDTH / 2 - 390, WIN_HEIGHT / 2 + 115 * mult + 6 * mult, 20, true, "Edit");
	_text->addText(WIN_WIDTH / 2 - 180, WIN_HEIGHT / 2 + 115 * mult + 6 * mult, 20, true, "Delete");
	_text->addText(WIN_WIDTH / 2 + 76, WIN_HEIGHT / 2 + 90 * mult + 6 * mult, 20, true, "Create New World");
	_text->addText(WIN_WIDTH / 2 + 40 * mult - 80, WIN_HEIGHT / 2 + 115 * mult + 6 * mult, 20, true, "Re-Create");
	_text->addText(WIN_WIDTH / 2 + 128 * mult - 80, WIN_HEIGHT / 2 + 115 * mult + 6 * mult, 20, true, "Cancel");

	for (int index = 0; index < static_cast<int>(_worlds.size()) && index < 8; index++) {
		std::string str = _worlds[index].substr(0, _worlds[index].size() - 5);
		_text->addText(WIN_WIDTH / 2 - str.size() * 10 + mult, (30 + 20 * index) * mult + 6 * mult + mult, 20, false, str);
		_text->addText(WIN_WIDTH / 2 - str.size() * 10, (30 + 20 * index) * mult + 6 * mult, 20, true, str);
	}
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

	std::string str = "LOADING.. " + std::to_string(percentage).substr(0, 5) + '%' + " (" + std::to_string(current_size) + '/' + std::to_string(goal) + ')';
	_text->addText(WIN_WIDTH / 2 - 24 * str.length() / 2, WIN_HEIGHT / 2 - 150, 24, true, str);
	return (0);
}

int Menu::death_menu( void )
{
	if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (_selection == 1) { // Respawn
			_state = LOAD_MENU;
			reset_values();
			return (5);
		} else if (_selection == 2) { // Save and Quit to Title
			_state = MAIN_MENU;
			reset_values();
			return (6);
		}
	}

	setup_array_buffer_death();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	int mult = 3;
	// first draw text shadow
	_text->addText(WIN_WIDTH / 2 - 150 + mult, WIN_HEIGHT / 2 - 60 * mult - 29 + mult, 40, false, "You Died!");
	_text->addText(WIN_WIDTH / 2 - 70 + mult, WIN_HEIGHT / 2 - 5 * mult + 6 * mult + mult, 20, false, "Respawn");
	_text->addText(WIN_WIDTH / 2 - 110 + mult, WIN_HEIGHT / 2 + 30 * mult + 6 * mult + mult, 20, false, "Title Screen");

	// then draw text in white
	_text->addText(WIN_WIDTH / 2 - 150, WIN_HEIGHT / 2 - 60 * mult - 29, 40, true, "You Died!");
	_text->addText(WIN_WIDTH / 2 - 70, WIN_HEIGHT / 2 + - 5 * mult + 6 * mult, 20, true, "Respawn");
	_text->addText(WIN_WIDTH / 2 - 110, WIN_HEIGHT / 2 + 30 * mult + 6 * mult, 20, true, "Title Screen");
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
	// _text->addText(WIN_WIDTH / 2 - 100 + mult, WIN_HEIGHT / 2 - 60 * mult - 40 + mult, 24, false, "Game Menu");
	_text->addText(WIN_WIDTH / 2 - 110 + mult, WIN_HEIGHT / 2 - 60 * mult + 6 * mult + mult, 20, false, "Back to Game");
	_text->addText(WIN_WIDTH / 2 - 50 * mult - 120 + mult, WIN_HEIGHT / 2 - 35 * mult + 6 * mult + mult, 20, false, "Advancements");
	_text->addText(WIN_WIDTH / 2 - 50 * mult - 125 + mult, WIN_HEIGHT / 2 - 10 * mult + 6 * mult + mult, 20, false, "Give Feedback");
	_text->addText(WIN_WIDTH / 2 - 50 * mult - 80 + mult, WIN_HEIGHT / 2 + 15 * mult + 6 * mult + mult, 20, false, "Options...");
	_text->addText(WIN_WIDTH / 2 + 50 * mult - 70 + mult, WIN_HEIGHT / 2 - 35 * mult + 6 * mult + mult, 20, false, "Statistics");
	_text->addText(WIN_WIDTH / 2 + 50 * mult - 100 + mult, WIN_HEIGHT / 2 - 10 * mult + 6 * mult + mult, 20, false, "Report Bugs");
	_text->addText(WIN_WIDTH / 2 + 50 * mult - 100 + mult, WIN_HEIGHT / 2 + 15 * mult + 6 * mult + mult, 20, false, "Open to LAN");
	_text->addText(WIN_WIDTH / 2 - 200 + mult, WIN_HEIGHT / 2 + 40 * mult + 6 * mult + mult, 20, false, "Save and Quit to Title");

	// then draw text in white
	_text->addText(WIN_WIDTH / 2 - 100, WIN_HEIGHT / 2 - 60 * mult - 40, 24, true, "Game Menu");
	_text->addText(WIN_WIDTH / 2 - 110, WIN_HEIGHT / 2 - 60 * mult + 6 * mult, 20, true, "Back to Game");
	_text->addText(WIN_WIDTH / 2 - 50 * mult - 120, WIN_HEIGHT / 2 - 35 * mult + 6 * mult, 20, true, "Advancements");
	_text->addText(WIN_WIDTH / 2 - 50 * mult - 125, WIN_HEIGHT / 2 - 10 * mult + 6 * mult, 20, true, "Give Feedback");
	_text->addText(WIN_WIDTH / 2 - 50 * mult - 80, WIN_HEIGHT / 2 + 15 * mult + 6 * mult, 20, true, "Options...");
	_text->addText(WIN_WIDTH / 2 + 50 * mult - 70, WIN_HEIGHT / 2 - 35 * mult + 6 * mult, 20, true, "Statistics");
	_text->addText(WIN_WIDTH / 2 + 50 * mult - 100, WIN_HEIGHT / 2 - 10 * mult + 6 * mult, 20, true, "Report Bugs");
	_text->addText(WIN_WIDTH / 2 + 50 * mult - 100, WIN_HEIGHT / 2 + 15 * mult + 6 * mult, 20, true, "Open to LAN");
	_text->addText(WIN_WIDTH / 2 - 200, WIN_HEIGHT / 2 + 40 * mult + 6 * mult, 20, true, "Save and Quit to Title");
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
	// if (_selection && glfwGetKey(_window, GLFW_KEY_Q) == GLFW_PRESS) {
	// 	_inventory.removeBlockAt(_selection - 1, _furnace, _chest);
	// }
	if (_left_released && glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		_left_released = false;
		if (_selection) {
			if (_selected_block.x == blocks::AIR) {
				_selected_block = _inventory.pickBlockAt(craft, _selection - 1, _furnace, _chest);
			} else {
				_selected_block = _inventory.putBlockAt(craft, _selection - 1, _selected_block, _furnace, _chest);
				_ui->addFace({0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, false, true); // TODO better way to update ui than this
			}
		}
	} else if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		_left_released = true;
	}
	if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		if (_selection) {
			if (_selected_block.x == blocks::AIR) {
				if (_right_released) {
					_selected_block = _inventory.pickHalfBlockAt(craft, _selection - 1, _furnace, _chest);
					_selection_list.push_back(_selection);
				}
			} else {
				bool inList = false;
				for (auto& s: _selection_list) {
					if (s == _selection) {
						inList = true;
						break ;
					}
				}
				if (!inList) {
					int count = _selected_block.y;
					_selected_block = _inventory.putOneBlockAt(craft, _selection - 1, _selected_block, _furnace, _chest);
					if (_selected_block.y < count) {
						_selection_list.push_back(_selection);
					}
					_ui->addFace({0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, false, true);
				}
			}
		}
		_right_released = false;
	} else if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
		_right_released = true;
		_selection_list.clear();
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
		case CHEST_MENU:
			setup_array_buffer_chest();
			break ;
		case FURNACE_MENU:
			setup_array_buffer_furnace();
			break ;
	}

	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	for (int index = 0; index < 9; index++) {
		display_slot_value(index); // TODO call these from add_slot_value
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
	int mult = 3;
	mtx_inventory.lock();
	int value = _inventory.getCrafted().y; // TODO do this only if inventory or crafting menus
	mtx_inventory.unlock();
	if (value > 1) {
		switch (_state) {
			case (INVENTORY_MENU):
				if (value > 9) {
					_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 160 * mult - 6 * mult + mult, WIN_HEIGHT / 2 - 46 * mult + 1 + mult, 22, false, std::to_string(value / 10));
					_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 160 * mult - 6 * mult, WIN_HEIGHT / 2 - 46 * mult + 2, 22, true, std::to_string(value / 10));
				}
				_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 160 * mult + mult, WIN_HEIGHT / 2 - 46 * mult + 1 + mult, 22, false, std::to_string(value % 10));
				_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 160 * mult, WIN_HEIGHT / 2 - 46 * mult + 2, 22, true, std::to_string(value % 10));
				break ;
			case (CRAFTING_MENU):
				if (value > 9) {
					_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 130 * mult - 6 * mult + mult, WIN_HEIGHT / 2 - 39 * mult + 1 + mult, 22, false, std::to_string(value / 10));
					_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 130 * mult - 6 * mult, WIN_HEIGHT / 2 - 39 * mult + 2, 22, true, std::to_string(value / 10));
				}
				_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 130 * mult + mult, WIN_HEIGHT / 2 - 39 * mult + 1 + mult, 22, false, std::to_string(value % 10));
				_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 130 * mult, WIN_HEIGHT / 2 - 39 * mult + 2, 22, true, std::to_string(value % 10));
				break ;
		}
	}
	if (_selected_block.y > 1) {
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		value = _selected_block.y;
		if (value > 1) {
			if (value > 9) {
				_text->addText(mouseX + 3 * mult - 6 * mult + mult, mouseY + mult + 1 + mult, 22, false, std::to_string(value / 10));
				_text->addText(mouseX + 3 * mult - 6 * mult, mouseY + mult + 2, 22, true, std::to_string(value / 10));
			}
			_text->addText(mouseX + 3 * mult + mult, mouseY + mult + 1 + mult, 22, false, std::to_string(value % 10));
			_text->addText(mouseX + 3 * mult, mouseY + mult + 2, 22, true, std::to_string(value % 10));
		}
	}
	return (0);
}

int Menu::chat_menu( bool animUpdate )
{
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetCharCallback(_window, NULL);
		INPUT::resetMessage();
		_chat->resetHistoCursor();
		return (1);
	}
	if (++_chat_released == 1 && glfwGetKey(_window, GLFW_KEY_ENTER) == GLFW_PRESS) {
		_chat->sendMessage(INPUT::getCurrentMessage());
		INPUT::resetMessage();
	}
	if (_chat_released == 1 && glfwGetKey(_window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
		INPUT::rmLetter();
	}
	if (_chat_released == 1 && glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) {
		INPUT::setCurrentMessage(_chat->getHistoMsg(true));
	}
	if (_chat_released == 1 && glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		INPUT::setCurrentMessage(_chat->getHistoMsg(false));
	}
	if (_chat_released == 1 && glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		INPUT::moveCursor(true, glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
	}
	if (_chat_released == 1 && glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		INPUT::moveCursor(false, glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
	}
	if (glfwGetKey(_window, GLFW_KEY_ENTER) == GLFW_RELEASE && glfwGetKey(_window, GLFW_KEY_BACKSPACE) == GLFW_RELEASE
		&& glfwGetKey(_window, GLFW_KEY_UP) == GLFW_RELEASE && glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_RELEASE
		&& glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_RELEASE && glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_RELEASE) {
		_chat_released = 0;
	}


	setup_array_buffer_chat();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	if (animUpdate) {
		_textBar = !_textBar;
	}
	_chat->blitPastMessages();
	_text->addText(36, WIN_HEIGHT - 48 - 12, 12, true, INPUT::getCurrentInputStr((_textBar) ? '|' : '.'));
	return (0);
}

void Menu::fill_vertices( std::vector<int> &vertices, std::array<int, 9> values )
{
	for (int index = 0; index < 9; index++) {
		vertices.push_back(values[index]);
	}
}

void Menu::setup_array_buffer_main( void )
{
	int mult = 3;
    std::vector<int> vertices; // pos: x y width height textcoord: x y width height
	fill_vertices(vertices, {1, WIN_WIDTH / 2 - 129 * mult, WIN_HEIGHT / 2 - 104 * mult, 256 * mult, 64 * mult, 0, 131, 256, 64}); // MINECRAFT

    fill_vertices(vertices, {1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 10 * mult, 200 * mult, 20 * mult, 0, 91 + 20 * (_selection == 1), 200, 20}); // Singleplayer
    fill_vertices(vertices, {1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 15 * mult, 200 * mult, 20 * mult, 0, 71, 200, 20}); // Multiplayer
    fill_vertices(vertices, {1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 40 * mult, 200 * mult, 20 * mult, 0, 71, 200, 20}); // Minecraft Realms

    fill_vertices(vertices, {1, WIN_WIDTH / 2 - 125 * mult, WIN_HEIGHT / 2 + 80 * mult, 15 * mult, 20 * mult, 0, 71, 200, 20}); // Lang settings
    fill_vertices(vertices, {1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 80 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20}); // Options...
    fill_vertices(vertices, {1, WIN_WIDTH / 2 + 5 * mult, WIN_HEIGHT / 2 + 80 * mult, 95 * mult, 20 * mult, 0, 91 + 20 * (_selection == 6), 200, 20}); // Quit Game
    fill_vertices(vertices, {1, WIN_WIDTH / 2 + 110 * mult, WIN_HEIGHT / 2 + 80 * mult, 15 * mult, 20 * mult, 0, 71, 200, 20}); // accessibility settings

	setup_shader(vertices);
}

void Menu::setup_array_buffer_select( void )
{
	int mult = 3;
    std::vector<int> vertices; // pos: x y width height textcoord: x y width height

	fill_vertices(vertices, {1, WIN_WIDTH / 2 - 155 * mult, WIN_HEIGHT / 2 + 90 * mult, 150 * mult, 20 * mult, 0, (91 + 20 * (_selection == 1)) * (_selected_world != 0) + 71 * (_selected_world == 0), 200, 20}); // playSelectedWorld
	fill_vertices(vertices, {1, WIN_WIDTH / 2 - 155 * mult, WIN_HEIGHT / 2 + 115 * mult, 73 * mult, 20 * mult, 0, 71, 200, 20}); // edit
	fill_vertices(vertices, {1, WIN_WIDTH / 2 - 78 * mult, WIN_HEIGHT / 2 + 115 * mult, 73 * mult, 20 * mult, 0, 71, 200, 20}); // delete
	fill_vertices(vertices, {1, WIN_WIDTH / 2 + 5 * mult, WIN_HEIGHT / 2 + 90 * mult, 150 * mult, 20 * mult, 0, 91 + 20 * (_selection == 4), 200, 20}); // createNewWorld
	fill_vertices(vertices, {1, WIN_WIDTH / 2 + 5 * mult, WIN_HEIGHT / 2 + 115 * mult, 73 * mult, 20 * mult, 0, 71, 200, 20}); // reCreate
	fill_vertices(vertices, {1, WIN_WIDTH / 2 + 82 * mult, WIN_HEIGHT / 2 + 115 * mult, 73 * mult, 20 * mult, 0, 91 + 20 * (_selection == 6), 200, 20}); // cancel

	for (int index = 0; index < static_cast<int>(static_cast<int>(_worlds.size())) && index < 8; index++) {
		fill_vertices(vertices, {1, WIN_WIDTH / 2 - 100 * mult, (30 + 20 * index) * mult, 200 * mult, 20 * mult, 0, 71 + 20 * (_selected_world - 1 == index), 200, 20}); // world #index
	}

	setup_shader(vertices);
}

void Menu::setup_array_buffer_load( int completion )
{
	// std::cout << "current completion : " << completion << std::endl;
	std::vector<int> vertices; // pos: x y width height textcoord: x y width height

	fill_vertices(vertices, {1, WIN_WIDTH / 2 - 110, WIN_HEIGHT / 2 - 120, 220, 240, 0, 71, 200, 20});

	int step = 1, turnCounter = 0, numSteps = 1, stepSize = 20, state = 0, x = WIN_WIDTH / 2 - 20, y = WIN_HEIGHT / 2;
	for (; step < 1 + 100; step++) {
		int offset = (step < 1 + completion / 10) ? 48 : 32; // cobblestone or stone
		fill_vertices(vertices, {0, x, y, stepSize, stepSize, 64, offset, 16, 16});
		if (step == 1 + completion / 10) {
			fill_vertices(vertices, {0, x, y, stepSize, stepSize, 240, 32 * ((completion % 10) != 0) + 16 * (completion % 10), 16, 16}); // break frame
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
}

void Menu::setup_array_buffer_death( void )
{
	int mult = 3;
	std::vector<int> vertices; // pos: x y width height textcoord: x y width height
	fill_vertices(vertices, {1, 0, 0, WIN_WIDTH, WIN_HEIGHT, 16, 15, 1, 1}); // occult window in redish
    fill_vertices(vertices, {1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 5 * mult, 200 * mult, 20 * mult, 0, 91 + 20 * (_selection == 1), 200, 20}); // Respawn
    fill_vertices(vertices, {1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 30 * mult, 200 * mult, 20 * mult, 0, 91 + 20 * (_selection == 2), 200, 20}); // Title Screen

	setup_shader(vertices);
}

void Menu::setup_array_buffer_pause( void )
{
	int mult = 3;
    std::vector<int> vertices; // pos: x y width height textcoord: x y width height
	fill_vertices(vertices, {1, 0, 0, WIN_WIDTH, WIN_HEIGHT, 3, 29, 1, 1}); // occult window
    fill_vertices(vertices, {1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 60 * mult, 200 * mult, 20 * mult, 0, 91 + 20 * (_selection == 1), 200, 20}); // Back to Game

    fill_vertices(vertices, {1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 35 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20}); // Advancements
    fill_vertices(vertices, {1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 10 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20}); // Give Feedback
    fill_vertices(vertices, {1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 15 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20}); // Options...

    fill_vertices(vertices, {1, WIN_WIDTH / 2 + 5 * mult, WIN_HEIGHT / 2 - 35 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20}); // Statistics
    fill_vertices(vertices, {1, WIN_WIDTH / 2 + 5 * mult, WIN_HEIGHT / 2 - 10 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20}); // Report Bugs
    fill_vertices(vertices, {1, WIN_WIDTH / 2 + 5 * mult, WIN_HEIGHT / 2 + 15 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20}); // Open to LAN

    fill_vertices(vertices, {1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 40 * mult, 200 * mult, 20 * mult, 0, 91 + 20 * (_selection == 8), 200, 20}); // Save and Quit to Title

	setup_shader(vertices);
}

void Menu::setup_array_buffer_chat( void )
{
	int nbr = _chat->computeHeight();
    std::vector<int> vertices; // altlas index, pos: {x y width height},  textcoord: {x y width height}
    fill_vertices(vertices, {1, CHAT_BOX_X, WIN_HEIGHT - 48 - 8 - 18 * (nbr + 1), CHAT_BOX_WIDTH, 4 + 18 * nbr, 3, 29, 1, 1}); // occult chat box
    fill_vertices(vertices, {1, CHAT_BOX_X, WIN_HEIGHT - 48 - 18, CHAT_BOX_WIDTH, 20, 3, 29, 1, 1}); // occult input box

	setup_shader(vertices);
}

void Menu::display_slot_value( int index )
{
	if (index < 0 || index >= 9) {
		return ;
	}
	int mult = 3, value = _inventory.getSlotBlock(index).y;
	/*	 + 11 * mult X, + 9 * mult + 2 (only + 1 for shadow) Y  */
	if (value > 1) {
		if (value > 9) {
			_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 18 * index * mult + mult * 14 - 6 * mult + mult, WIN_HEIGHT / 2 + 1 + (59 + 9) * mult + mult, 22, false, std::to_string(value / 10));
			_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 18 * index * mult + mult * 14 - 6 * mult, WIN_HEIGHT / 2 + 2 + (59 + 9) * mult, 22, true, std::to_string(value / 10));
		}
		_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 18 * index * mult + mult * 14 + mult, WIN_HEIGHT / 2 + 1 + (59 + 9) * mult + mult, 22, false, std::to_string(value % 10));
		_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 18 * index * mult + mult * 14, WIN_HEIGHT / 2 + 2 + (59 + 9) * mult, 22, true, std::to_string(value % 10));
	}
}

void Menu::display_backpack_value( int index )
{
	if (index < 0 || index >= 27) {
		return ;
	}
	int mult = 3, value = _inventory.getBackpackBlock(index).y;
	if (value > 1) {
		if (value > 9) {
			_text->addText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (index % 9) * mult) + mult * 14 - 6 * mult + mult, WIN_HEIGHT / 2 + 10 * mult + 18 * mult * (index / 9) + 1 + mult, 22, false, std::to_string(value / 10));
			_text->addText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (index % 9) * mult) + mult * 14 - 6 * mult, WIN_HEIGHT / 2 + 10 * mult + 18 * mult * (index / 9) + 2, 22, true, std::to_string(value / 10));
		}
		_text->addText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (index % 9) * mult) + mult * 14 + mult, WIN_HEIGHT / 2 + 10 * mult + 18 * mult * (index / 9) + 1 + mult, 22, false, std::to_string(value % 10));
		_text->addText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (index % 9) * mult) + mult * 14, WIN_HEIGHT / 2 + 10 * mult + 18 * mult * (index / 9) + 2, 22, true, std::to_string(value % 10));
	}
}

void Menu::display_icraft_value( int index )
{
	if (index < 0 || index >= 4) {
		return ;
	}
	int mult = 3, value = _inventory.getiCraftBlock(index).y;
	if (value > 1) {
		if (value > 9) {
			_text->addText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (5 + index % 2) * mult) + mult * 14 - 6 * mult + mult, WIN_HEIGHT / 2 - 56 * mult + 18 * mult * (index / 2) + 1 + mult, 22, false, std::to_string(value / 10));
			_text->addText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (5 + index % 2) * mult) + mult * 14 - 6 * mult, WIN_HEIGHT / 2 - 56 * mult + 18 * mult * (index / 2) + 2, 22, true, std::to_string(value / 10));
		}
		_text->addText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (5 + index % 2) * mult) + mult * 14 + mult, WIN_HEIGHT / 2 - 56 * mult + 18 * mult * (index / 2) + 1 + mult, 22, false, std::to_string(value % 10));
		_text->addText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (5 + index % 2) * mult) + mult * 14, WIN_HEIGHT / 2 - 56 * mult + 18 * mult * (index / 2) + 2, 22, true, std::to_string(value % 10));
	}
}

void Menu::display_furnace_value( void )
{
	if (!_furnace) {
		return ;
	}
	int mult = 3, value = _furnace->getComposant().y;
	if (value > 1) {
		if (value > 9) {
			_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 62 * mult - 6 * mult + mult, WIN_HEIGHT / 2 - 57 * mult + 1 + mult, 22, false, std::to_string(value / 10));
			_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 62 * mult - 6 * mult, WIN_HEIGHT / 2 - 57 * mult + 2, 22, true, std::to_string(value / 10));
		}
		_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 62 * mult + mult, WIN_HEIGHT / 2 - 57 * mult + 1 + mult, 22, false, std::to_string(value % 10));
		_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 62 * mult, WIN_HEIGHT / 2 - 57 * mult + 2, 22, true, std::to_string(value % 10));
	}
	value = _furnace->getFuel().y;
	if (value > 1) {
		if (value > 9) {
			_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 62 * mult - 6 * mult + mult, WIN_HEIGHT / 2 - 21 * mult + 1 + mult, 22, false, std::to_string(value / 10));
			_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 62 * mult - 6 * mult, WIN_HEIGHT / 2 - 21 * mult + 2, 22, true, std::to_string(value / 10));
		}
		_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 62 * mult + mult, WIN_HEIGHT / 2 - 21 * mult + 1 + mult, 22, false, std::to_string(value % 10));
		_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 62 * mult, WIN_HEIGHT / 2 - 21 * mult + 2, 22, true, std::to_string(value % 10));
	}
	value = _furnace->getProduction().y;
	if (value > 1) {
		if (value > 9) {
			_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 122 * mult - 6 * mult + mult, WIN_HEIGHT / 2 - 39 * mult + 1 + mult, 22, false, std::to_string(value / 10));
			_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 122 * mult - 6 * mult, WIN_HEIGHT / 2 - 39 * mult + 2, 22, true, std::to_string(value / 10));
		}
		_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 122 * mult + mult, WIN_HEIGHT / 2 - 39 * mult + 1 + mult, 22, false, std::to_string(value % 10));
		_text->addText((WIN_WIDTH - (166 * mult)) / 2 + 122 * mult, WIN_HEIGHT / 2 - 39 * mult + 2, 22, true, std::to_string(value % 10));
	}
}

void Menu::display_craft_value( int index )
{
	if (index < 0 || index >= 9) {
		return ;
	}
	int mult = 3, value = _inventory.getCraftBlock(index).y;
	if (value > 1) {
		if (value > 9) {
			_text->addText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (1 + index % 3) * mult) + mult * 18 - 6 * mult + mult, WIN_HEIGHT / 2 - 57 * mult + 18 * mult * (index / 3) + 1 + mult, 22, false, std::to_string(value / 10));
			_text->addText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (1 + index % 3) * mult) + mult * 18 - 6 * mult, WIN_HEIGHT / 2 - 57 * mult + 18 * mult * (index / 3) + 2, 22, true, std::to_string(value / 10));
		}
		_text->addText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (1 + index % 3) * mult) + mult * 18 + mult, WIN_HEIGHT / 2 - 57 * mult + 18 * mult * (index / 3) + 1 + mult, 22, false, std::to_string(value % 10));
		_text->addText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (1 + index % 3) * mult) + mult * 18, WIN_HEIGHT / 2 - 57 * mult + 18 * mult * (index / 3) + 2, 22, true, std::to_string(value % 10));
	}
}

void Menu::occult_selection( std::vector<int> &vertices, int mult )
{
	bool found = false;
	for (int s : _selection_list) {
		if (s == _selection) {
			found = true;
			break ;
		}
	}
	if (!found) {
		_selection_list.push_back(_selection);
	}
	for (int cell : _selection_list) {
		if (!cell) {
		} else if (cell < 10) { // hot bar slot
			int index = cell - 1;
			fill_vertices(vertices, {1, (WIN_WIDTH - (166 * mult)) / 2 + (18 * index * mult) + mult * 3, WIN_HEIGHT / 2 + 59 * mult, 16 * mult, 16 * mult, 16, 14, 1, 1});
		} else if (cell < 37) { // backpack
			int index = cell - 10;
			fill_vertices(vertices, {1, (WIN_WIDTH - (166 * mult)) / 2 + (18 * (index % 9) * mult) + mult * 3, WIN_HEIGHT / 2 + mult + 18 * mult * (index / 9), 16 * mult, 16 * mult, 16, 14, 1, 1});
		} else if (cell < 41) { // icraft
			int index = cell - 37;
			fill_vertices(vertices, {1, (WIN_WIDTH - (166 * mult)) / 2 + (18 * (5 + index % 2) * mult) + mult * 3, WIN_HEIGHT / 2 - 65 * mult + 18 * mult * (index / 2), 16 * mult, 16 * mult, 16, 14, 1, 1});
		} else if (cell == 41) { // crafted
			if (_state == INVENTORY_MENU) {
				fill_vertices(vertices, {1, (WIN_WIDTH - (166 * mult)) / 2 + 149 * mult, WIN_HEIGHT / 2 - 55 * mult, 16 * mult, 16 * mult, 16, 14, 1, 1});
			} else if (_state == CRAFTING_MENU) {
				fill_vertices(vertices, {1, (WIN_WIDTH - (166 * mult)) / 2 + 119 * mult, WIN_HEIGHT / 2 - 48 * mult, 16 * mult, 16 * mult, 16, 14, 1, 1});
			} else if (_state == FURNACE_MENU) {
				fill_vertices(vertices, {1, (WIN_WIDTH - (166 * mult)) / 2 + 111 * mult, WIN_HEIGHT / 2 - 48 * mult, 16 * mult, 16 * mult, 16, 14, 1, 1});
			}
		} else if (cell < 51) { // craft
			int index = cell - 42;
			fill_vertices(vertices, {1, (WIN_WIDTH - (166 * mult)) / 2 + (18 * (1 + index % 3) * mult) + mult * 7, WIN_HEIGHT / 2 - 66 * mult + 18 * mult * (index / 3), 16 * mult, 16 * mult, 16, 14, 1, 1});
		} else if (cell == 51) { // composant
			fill_vertices(vertices, {1, (WIN_WIDTH - (166 * mult)) / 2 + 51 * mult, WIN_HEIGHT / 2 - 66 * mult, 16 * mult, 16 * mult, 16, 14, 1, 1});
		} else if (cell == 52) { // fuel
			fill_vertices(vertices, {1, (WIN_WIDTH - (166 * mult)) / 2 + 51 * mult, WIN_HEIGHT / 2 - 30 * mult, 16 * mult, 16 * mult, 16, 14, 1, 1});
		} else if (cell < 80) { // chest
			int index = cell - 53;
			fill_vertices(vertices, {1, (WIN_WIDTH - (166 * mult)) / 2 + (18 * (index % 9) * mult) + mult * 3, WIN_HEIGHT / 2 - 65 * mult + 18 * mult * (index / 9), 16 * mult, 16 * mult, 16, 14, 1, 1});
		}
	}
	if (!found) {
		_selection_list.pop_back();
	}
}

void Menu::add_item_value( int type, int x, int y, int mult, bool movement )
{
	if (!s_blocks[type]->item3D) {
		int spec = s_blocks[type]->texX(face_dir::MINUSX, 0) + (s_blocks[type]->texY(face_dir::MINUSX, 0) << 4) + (3 << 19);
		// int faceLight = computeLight(row - 1, col, level);
		int cornerLight = 15;//computeSmoothLight(faceLight, row - 1, col, level, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		int shade = 0;//computeShade(row - 1, col, level, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		// spec += (faceLight << 24);
		glm::ivec3 v0 = {spec + (cornerLight << 24) + (shade << 22), x, y};
		// cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		// shade = computeShade(row - 1, col, level, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		glm::ivec3 v1 = {spec + (cornerLight << 24) + (shade << 22) + 1 + (1 << 9) + (1 << 8), x + 16 * mult, y};
		// cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		// shade = computeShade(row - 1, col, level, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		glm::ivec3 v2 = {spec + (cornerLight << 24) + (shade << 22) + (1 << 4) + (1 << 10) + (1 << 12), x, y + 16 * mult};
		// cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		// shade = computeShade(row - 1, col, level, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		glm::ivec3 v3 = {spec + (cornerLight << 24) + (shade << 22) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), x + 16 * mult, y + 16 * mult};
		_ui->addFace(v0, v1, v2, v3, true, movement);
		return ;
	}
	x += mult;
	int offset = face_dir::PLUSX;
	// top face
	int spec = (15 << 24) + s_blocks[type]->texX(face_dir::PLUSZ, offset) + (s_blocks[type]->texY(face_dir::PLUSZ, offset) << 4);
	// if (type == blocks::DIRT_PATH) {
	// 	p4.z -= ONE_SIXTEENTH;
	// 	p5.z -= ONE_SIXTEENTH;
	// 	p0.z -= ONE_SIXTEENTH;
	// 	p1.z -= ONE_SIXTEENTH;
	// }
	glm::ivec3 v0 = {spec, x, y + 16 * mult * 81.25f / 362.5f};
	glm::ivec3 v1 = {spec + 1 + (1 << 9) + (1 << 8), x + 7 * mult, y};
	glm::ivec3 v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), x + 7 * mult, y + 16 * mult * 162.5f / 362.5f};
	glm::ivec3 v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), x + 14 * mult, y + 16 * mult * 81.25f / 362.5f};
	_ui->addFace(v0, v1, v2, v3, true, movement);
	// left face
	spec = (10 << 24) + s_blocks[type]->texX(face_dir::MINUSY, offset) + (s_blocks[type]->texY(face_dir::MINUSY, offset) << 4);
	v0 = {spec, x, y + 16 * mult * 81.25f / 362.5f};
	v1 = {spec + 1 + (1 << 9) + (1 << 8), x + 7 * mult, y + 16 * mult * 162.5f / 362.5f};
	v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), x, y + 16 * mult * 281.25f / 362.5f};
	v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), x + 7 * mult, y + 16 * mult};
	_ui->addFace(v0, v1, v2, v3, true, movement);
	// right face
	spec = (7 << 24) + s_blocks[type]->texX(face_dir::PLUSX, offset) + (s_blocks[type]->texY(face_dir::PLUSX, offset) << 4);
	v0 = {spec, x + 7 * mult, y + 16 * mult * 162.5f / 362.5f};
	v1 = {spec + 1 + (1 << 9) + (1 << 8), x + 14 * mult, y + 16 * mult * 81.25f / 362.5f};
	v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), x + 7 * mult, y + 16 * mult};
	v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), x + 14 * mult, y + 16 * mult * 281.25f / 362.5f};
	_ui->addFace(v0, v1, v2, v3, true, movement);
}

void Menu::add_slot_value( int mult, int index )
{
	mtx_inventory.lock();
	int type = _inventory.getSlotBlock(index).x;
	mtx_inventory.unlock();
	if (type == blocks::AIR) {
		return ;
	}
	int x = (WIN_WIDTH - (166 * mult)) / 2 + (18 * index * mult) + mult * 3;
	int y = WIN_HEIGHT / 2 + 59 * mult;
	add_item_value(type, x, y, mult);
}

void Menu::add_backpack_value( int mult, int index )
{
	mtx_inventory.lock();
	int type = _inventory.getBackpackBlock(index).x;
	mtx_inventory.unlock();
	if (type == blocks::AIR) {
		return ;
	}
	int x = (WIN_WIDTH - (166 * mult)) / 2 + (18 * (index % 9) * mult) + mult * 3;
	int y = WIN_HEIGHT / 2 + mult + 18 * mult * (index / 9);
	add_item_value(type, x, y, mult);
}

void Menu::add_icraft_value( int mult, int index )
{
	mtx_inventory.lock();
	int type = _inventory.getiCraftBlock(index).x;
	mtx_inventory.unlock();
	if (type == blocks::AIR) {
		return ;
	}
	int x = (WIN_WIDTH - (166 * mult)) / 2 + (18 * (5 + index % 2) * mult) + mult * 3;
	int y = WIN_HEIGHT / 2 - 65 * mult + 18 * mult * (index / 2);
	add_item_value(type, x, y, mult);
}

void Menu::add_craft_value( int mult, int index )
{
	mtx_inventory.lock();
	int type = _inventory.getCraftBlock(index).x;
	mtx_inventory.unlock();
	if (type == blocks::AIR) {
		return ;
	}
	int x = (WIN_WIDTH - (166 * mult)) / 2 + (18 * (1 + index % 3) * mult) + mult * 7;
	int y = WIN_HEIGHT / 2 - 66 * mult + 18 * mult * (index / 3);
	add_item_value(type, x, y, mult);
}

static int screenPosXFromlocation( int mult, int location )
{
	if (location < 9) { // action
		return ((WIN_WIDTH - (166 * mult)) / 2 + (18 * location * mult) + mult * 3);
	} else if (location < 36) { // backpack
		return ((WIN_WIDTH - (166 * mult)) / 2 + (18 * (location % 9) * mult) + mult * 3);
	} else if (location < 40) { // icraft
		return ((WIN_WIDTH - (166 * mult)) / 2 + (18 * (5 + location % 2) * mult) + mult * 3);
	} else if (location == 40) { // crafted, ignore
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
	} else if (location == 40) {
	} else {
		return (WIN_HEIGHT / 2 - 66 * mult + 18 * mult * ((location - 41) / 3));
	}
	return (0);
}

void Menu::add_dura_value( std::vector<int> &vertices, int mult, int index )
{
	mtx_inventory.lock();
	glm::ivec3 value = _inventory.getDuraFromIndex(index, true);
	mtx_inventory.unlock();
	if (value.y == 0) {
		return ;
	}
	// adding grey bar first
	fill_vertices(vertices, {0, screenPosXFromlocation(mult, value.x) + mult, screenPosYFromlocation(mult, value.x) + 14 * mult, 14 * mult, mult, 64, 0, 1, 1});
	// adding progress bar second
	float percent = 1.0f * value.y / value.z;
	fill_vertices(vertices, {0, screenPosXFromlocation(mult, value.x) + mult, screenPosYFromlocation(mult, value.x) + 14 * mult, static_cast<int>(14 * mult * percent), mult, 103 * (percent < 0.6f) - (percent < 0.3), 16 + 9 * (percent < 0.6f) - 18 * (percent < 0.3f), 1, 1});
}

void Menu::add_crafted_value( int mult )
{
	mtx_inventory.lock();
	int type = _inventory.getCrafted().x;
	mtx_inventory.unlock();
	if (type == blocks::AIR) {
		return ;
	}
	if (_state == INVENTORY_MENU) {
		int x = (WIN_WIDTH - (166 * mult)) / 2 + 149 * mult;
		int y = WIN_HEIGHT / 2 - 55 * mult;
		add_item_value(type, x, y, mult);
	} else if (_state == CRAFTING_MENU) {
		int x = (WIN_WIDTH - (166 * mult)) / 2 + 119 * mult;
		int y = WIN_HEIGHT / 2 - 48 * mult;
		add_item_value(type, x, y, mult);
	}
}

void Menu::add_chest_value( int mult, int index )
{
	glm::ivec2 *item = _chest->getItem(index);
	if (!item) {
		// std::cout << "add_chest_value NULL item at " << index << std::endl;
		return ;
	}
	int type = item->x;
	if (type == blocks::AIR) {
		return ;
	}
	int x = (WIN_WIDTH - (166 * mult)) / 2 + (18 * (index % 9) * mult) + mult * 3;
	int y = WIN_HEIGHT / 2 - 65 * mult + 18 * mult * (index / 9);
	add_item_value(type, x, y, mult);

	int amount = item->y;
	if (amount > 1) {
		if (amount > 9) {
			_text->addText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (index % 9) * mult) + mult * 14 - 6 * mult + mult, WIN_HEIGHT / 2 - 56 * mult + 18 * mult * (index / 9) + 1 + mult, 22, false, std::to_string(amount / 10));
			_text->addText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (index % 9) * mult) + mult * 14 - 6 * mult, WIN_HEIGHT / 2 - 56 * mult + 18 * mult * (index / 9) + 2, 22, true, std::to_string(amount / 10));
		}
		_text->addText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (index % 9) * mult) + mult * 14 + mult, WIN_HEIGHT / 2 - 56 * mult + 18 * mult * (index / 9) + 1 + mult, 22, false, std::to_string(amount % 10));
		_text->addText((WIN_WIDTH - (166 * mult)) / 2 + (18 * (index % 9) * mult) + mult * 14, WIN_HEIGHT / 2 - 56 * mult + 18 * mult * (index / 9) + 2, 22, true, std::to_string(amount % 10));
	}
}

void Menu::add_furnace_value( std::vector<int> &vertices, int mult )
{
	int value = _furnace->getComposant().x;
	if (value != blocks::AIR) {
		int x = (WIN_WIDTH - (166 * mult)) / 2 + 51 * mult;
		int y = WIN_HEIGHT / 2 - 66 * mult;
		add_item_value(value, x, y, mult);
	}
	int progress = 1 + glm::floor(_furnace->getFuelTime() * 13);
	if (_furnace->getFuelTime()) {
		fill_vertices(vertices, {1, (WIN_WIDTH - (166 * mult)) / 2 + 50 * mult + mult, WIN_HEIGHT / 2 - 48 * mult + progress * mult + mult, 14 * mult, (14 - progress) * mult, 24, 47 + progress, 14, 14 - progress});
	}
	progress = 1 + glm::floor(_furnace->getComposantTime() * 23);
	if (_furnace->getComposantTime()) {
		fill_vertices(vertices, {1, (WIN_WIDTH - (166 * mult)) / 2 + 74 * mult, WIN_HEIGHT / 2 - 49 * mult, progress * mult, 17 * mult, 38, 47, progress, 17});
	}
	value = _furnace->getFuel().x;
	if (value != blocks::AIR) {
		int x = (WIN_WIDTH - (166 * mult)) / 2 + 51 * mult;
		int y = WIN_HEIGHT / 2 - 30 * mult;
		add_item_value(value, x, y, mult);
	}
	value = _furnace->getProduction().x;
	if (value != blocks::AIR) {
		int x = (WIN_WIDTH - (166 * mult)) / 2 + 111 * mult;
		int y = WIN_HEIGHT / 2 - 48 * mult;
		add_item_value(value, x, y, mult);
	}
}

void Menu::setup_array_buffer_inventory( void )
{
	int mult = 3;
    std::vector<int> vertices; // pos: x y width height textcoord: x y width height

	fill_vertices(vertices, {2, WIN_WIDTH / 2 - 88 * mult, WIN_HEIGHT / 2 - 83 * mult, 176 * mult, 166 * mult, 0, 0, 128, 128});

	occult_selection(vertices, mult);
	for (int index = 0; index < 9; index++) {
		add_slot_value(mult, index);
	}
	for (int index = 0; index < 27; index++) {
		add_backpack_value(mult, index);
	}
	for (int index = 0; index < 4; index++) {
		add_icraft_value(mult, index);
	}
	mtx_inventory.lock();
	int duras = _inventory.countDura(true);
	mtx_inventory.unlock();
	for (int index = 0; index < duras; index++) {
		add_dura_value(vertices, mult, index);
	}
	add_crafted_value(mult);

	if (_selected_block.x != blocks::AIR) {
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		add_item_value(_selected_block.x, mouseX - 8 * mult, mouseY - 8 * mult, mult, true);
	}

	setup_shader(vertices);
}

void Menu::setup_array_buffer_crafting( void )
{
	int mult = 3;
    std::vector<int> vertices; // pos: x y width height textcoord: x y width height

	fill_vertices(vertices, {2, WIN_WIDTH / 2 - 88 * mult, WIN_HEIGHT / 2 - 83 * mult, 176 * mult, 166 * mult, 128, 0, 128, 128});

	occult_selection(vertices, mult);
	for (int index = 0; index < 9; index++) {
		add_slot_value(mult, index);
	}
	for (int index = 0; index < 27; index++) {
		add_backpack_value(mult, index);
	}
	for (int index = 0; index < 9; index++) {
		add_craft_value(mult, index);
	}
	mtx_inventory.lock();
	int duras = _inventory.countDura(true);
	mtx_inventory.unlock();
	for (int index = 0; index < duras; index++) {
		add_dura_value(vertices, mult, index);
	}
	add_crafted_value(mult);

	if (_selected_block.x != blocks::AIR) {
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		add_item_value(_selected_block.x, mouseX - 8 * mult, mouseY - 8 * mult, mult, true);
	}

	setup_shader(vertices);
}

void Menu::setup_array_buffer_chest( void )
{
	int mult = 3;
    std::vector<int> vertices; // pos: x y width height textcoord: x y width height

	fill_vertices(vertices, {2, WIN_WIDTH / 2 - 88 * mult, WIN_HEIGHT / 2 - 83 * mult, 176 * mult, 166 * mult, 128, 128, 128, 128});

	occult_selection(vertices, mult);
	for (int index = 0; index < 9; index++) {
		add_slot_value(mult, index);
	}
	for (int index = 0; index < 27; index++) {
		add_backpack_value(mult, index);
	}
	mtx_inventory.lock();
	int duras = _inventory.countDura(true) + ((_chest) ? 0 : 0); // TODO _chests->countDura
	mtx_inventory.unlock();
	for (int index = 0; index < duras; index++) {
		add_dura_value(vertices, mult, index);
	}
	for (int index = 0; index < 27; index++) {
		add_chest_value(mult, index);
	}

	if (_selected_block.x != blocks::AIR) {
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		add_item_value(_selected_block.x, mouseX - 8 * mult, mouseY - 8 * mult, mult, true);
	}

	setup_shader(vertices);
}

void Menu::setup_array_buffer_furnace( void )
{
	int mult = 3;

    std::vector<int> vertices; // pos: x y width height textcoord: x y width height

	fill_vertices(vertices, {2, WIN_WIDTH / 2 - 88 * mult, WIN_HEIGHT / 2 - 83 * mult, 176 * mult, 166 * mult, 0, 128, 128, 128});

	occult_selection(vertices, mult);
	for (int index = 0; index < 9; index++) {
		add_slot_value(mult, index);
	}
	for (int index = 0; index < 27; index++) {
		add_backpack_value(mult, index);
	}
	mtx_inventory.lock();
	int duras = _inventory.countDura(true);
	mtx_inventory.unlock();
	for (int index = 0; index < duras; index++) {
		add_dura_value(vertices, mult, index);
	}
	add_furnace_value(vertices, mult);

	if (_selected_block.x != blocks::AIR) {
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		add_item_value(_selected_block.x, mouseX - 8 * mult, mouseY - 8 * mult, mult, true);
	}

	setup_shader(vertices);
}

void Menu::setup_shader( std::vector<int> &vertices )
{
	_nb_points = vertices.size() / 9;
	if (!_nb_points) {
		return ;
	}

	glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
	_vaoSet = true;

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _nb_points * 9 * sizeof(GLint), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(UI_ATLASATTRIB);
	glVertexAttribIPointer(UI_ATLASATTRIB, 1, GL_INT, 9 * sizeof(GLint), 0);

    glEnableVertexAttribArray(UI_POSATTRIB);
	glVertexAttribIPointer(UI_POSATTRIB, 4, GL_INT, 9 * sizeof(GLint), (void *)(sizeof(GLint)));
	
	glEnableVertexAttribArray(UI_TEXATTRIB);
	glVertexAttribIPointer(UI_TEXATTRIB, 4, GL_INT, 9 * sizeof(GLint), (void *)(5 * sizeof(GLint)));

	check_glstate("Menu::setup_shader", false);
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
	int mult = 3;
	if (_state == MAIN_MENU) {
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 10 * mult, 200 * mult, 20 * mult)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * mult, WIN_HEIGHT / 2 + 80 * mult, 95 * mult, 20 * mult)) {
			_selection = 6;
		} else {
			_selection = 0;
		}
	} else if (_state == WORLD_SELECT_MENU) {
		if (_selected_world && inRectangle(posX, posY, WIN_WIDTH / 2 - 155 * mult, WIN_HEIGHT / 2 + 90 * mult, 150 * mult, 20 * mult)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * mult, WIN_HEIGHT / 2 + 90 * mult, 150 * mult, 20 * mult)) {
			_selection = 4;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 82 * mult, WIN_HEIGHT / 2 + 105 * mult, 73 * mult, 20 * mult)) {
			_selection = 6;
		} else {
			_selection = 0;
		}
		for (int index = 0; index < static_cast<int>(_worlds.size()) && index < 8; index++) {
			if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * mult, (30 + 20 * index) * mult, 200 * mult, 20 * mult)) {
				_selection = index + 7;
			}
		}
	} else if (_state == DEATH_MENU) {
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 5 * mult, 200 * mult, 20 * mult)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 30 * mult, 200 * mult, 20 * mult)) {
			_selection = 2;
		} else {
			_selection = 0;
		}
	} else if (_state == PAUSE_MENU) {
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 60 * mult, 200 * mult, 20 * mult)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 40 * mult, 200 * mult, 20 * mult)) {
			_selection = 8;
		} else {
			_selection = 0;
		}
	} else if (_state >= INVENTORY_MENU) {
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
		switch (_state) {
			case INVENTORY_MENU:
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
				break ;
			case CRAFTING_MENU:
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
				break ;
			case CHEST_MENU:
				for (int index = 0; index < 27; index++) {
					if (inRectangle(posX, posY, (WIN_WIDTH - (166 * mult)) / 2 + (18 * (index % 9) * mult) + mult * 3, WIN_HEIGHT / 2 - 65 * mult + 18 * mult * (index / 9), 16 * mult, 16 * mult)) {
						_selection = index + 53;
						return ;
					}
				}
				break ;
			case FURNACE_MENU:
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
				break ;
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

void Menu::setChunks( std::list<Chunk *> &chunks )
{
	if (_state == LOAD_MENU) {
		_chunks = chunks;
	}
}

void Menu::setChestInstance( ChestInstance *chest )
{
	_chest = chest;
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

	if (state == CHAT_MENU) {
		glfwSetCharCallback(_window, INPUT::character_callback);
	}
}

int Menu::getState( void )
{
	return (_state);
}

std::string Menu::getWorldFile( void )
{
	return (_world_file);
}

int Menu::run( GLint render_dist, bool animUpdate )
{
	if (_state != CHAT_MENU && glfwGetKey(_window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(_window, GL_TRUE);
		return (-1);
	}

	switch (_state) {
		case MAIN_MENU:
			return (main_menu());
		case WORLD_SELECT_MENU:
			return (world_select_menu());
		case LOAD_MENU:
			return (loading_screen(render_dist));
		case DEATH_MENU:
			return (death_menu());
		case PAUSE_MENU:
			return (pause_menu());
		case INVENTORY_MENU:
			return (ingame_menu());
		case CRAFTING_MENU:
			return (ingame_menu());
		case CHEST_MENU:
			return (ingame_menu());
		case FURNACE_MENU:
			return (ingame_menu());
		case CHAT_MENU:
			return (chat_menu(animUpdate));
		default:
			return (1);
	}
	return (0);
}

std::string Menu::getInfoString( void )
{
	if (_furnace) {
		return (_chat->getInfoString() + _furnace->getInfoString());
	}
	if (_chest) {
		return (_chat->getInfoString() + "\nSelection\t> " + std::to_string(_selection));// + _chest->getInfoString());
	}
	return (_chat->getInfoString());
}
