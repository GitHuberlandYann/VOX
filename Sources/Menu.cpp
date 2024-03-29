#include "Menu.hpp"
#include "Ui.hpp"
#include "callbacks.hpp"
#include <dirent.h>
extern std::mutex mtx;
extern siv::PerlinNoise::seed_type perlin_seed;

Menu::Menu( Inventory & inventory, UI *ui ) : _gui_size(3), _state(MENU::MAIN), _selection(0), _selected_world(0), _vaoSet(false),
	_esc_released(false), _e_released(false), _left_released(false), _right_released(false), _textBar(true),
	_key_1(0), _key_2(0), _key_3(0), _key_4(0), _key_5(0), _key_6(0), _key_7(0), _key_8(0), _key_9(0), _chat_released(0),
	_inventory(inventory), _ui(ui), _text(ui->getTextPtr()), _chat(ui->getChatPtr()), _chest(NULL), _furnace(NULL)
{
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
	if (_selected_block.type != blocks::AIR) {
		_inventory.restoreBlock(_selected_block);
	}
	_inventory.restoreiCraft();
	_inventory.restoreCraft();
	_inventory.setModif(true);
	_selected_block = {0};
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
			_state = MENU::WORLD_SELECT;
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
		} else if (_selection == 5) { // options
			_state = MENU::MAIN_OPTIONS;
			reset_values();
			return (0);
		} else if (_selection == 6) { //quit game
			glfwSetWindowShouldClose(_window, GL_TRUE);
			return (-1);
		}
	}
	if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS) { // skip world selection and launch with default seed
		_state = MENU::LOAD;
		reset_values();
		return (4);
	}

	setup_array_buffer_main();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	_text->addText(WIN_WIDTH / 2 - 220, WIN_HEIGHT / 2 - 120 * 3, 24, true, "Press space to start");

	// shadows
	_text->addText(WIN_WIDTH / 2 - 110 + _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size + 6 * _gui_size + _gui_size, 20, false, "Singleplayer");
	_text->addText(WIN_WIDTH / 2 - 100 + _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size + 6 * _gui_size + _gui_size, 20, false, "Multiplayer");
	_text->addText(WIN_WIDTH / 2 - 150 + _gui_size, WIN_HEIGHT / 2 + 40 * _gui_size + 6 * _gui_size + _gui_size, 20, false, "Minecraft Realms");
	_text->addText(WIN_WIDTH / 2 - 50 * _gui_size - 80 + _gui_size, WIN_HEIGHT / 2 + 80 * _gui_size + 6 * _gui_size + _gui_size, 20, false, "Options...");
	_text->addText(WIN_WIDTH / 2 + 50 * _gui_size - 75 + _gui_size, WIN_HEIGHT / 2 + 80 * _gui_size + 6 * _gui_size + _gui_size, 20, false, "Quit Game");
	// white
	_text->addText(WIN_WIDTH / 2 - 110, WIN_HEIGHT / 2 - 10 * _gui_size + 6 * _gui_size, 20, true, "Singleplayer");
	_text->addText(WIN_WIDTH / 2 - 100, WIN_HEIGHT / 2 + 15 * _gui_size + 6 * _gui_size, 20, true, "Multiplayer");
	_text->addText(WIN_WIDTH / 2 - 150, WIN_HEIGHT / 2 + 40 * _gui_size + 6 * _gui_size, 20, true, "Minecraft Realms");
	_text->addText(WIN_WIDTH / 2 - 50 * _gui_size - 80, WIN_HEIGHT / 2 + 80 * _gui_size + 6 * _gui_size, 20, true, "Options...");
	_text->addText(WIN_WIDTH / 2 + 50 * _gui_size - 75, WIN_HEIGHT / 2 + 80 * _gui_size + 6 * _gui_size, 20, true, "Quit Game");
	return (0);
}

int Menu::world_select_menu( void )
{
	if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (_selection == 1) { //play selected world
			_world_file = _worlds[_selected_world - 1];
			_state = MENU::LOAD;
			reset_values();
			return (2);
		} else if (_selection == 4) { // new random seed
			const auto p1 = std::chrono::system_clock::now(); // works without #include <chrono> #include <ctime> ?
			perlin_seed = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();
			_state = MENU::LOAD;
			reset_values();
			return (4);
		} else if (_selection == 6) { //cancel, go back to main menu
			_state = MENU::MAIN;
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

	// shadows
	_text->addText(WIN_WIDTH / 2 - 406 + _gui_size, WIN_HEIGHT / 2 + 90 * _gui_size + 6 * _gui_size + _gui_size, 20, false, "Play Selected World");
	_text->addText(WIN_WIDTH / 2 - 390 + _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size + 6 * _gui_size + _gui_size, 20, false, "Edit");
	_text->addText(WIN_WIDTH / 2 - 180 + _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size + 6 * _gui_size + _gui_size, 20, false, "Delete");
	_text->addText(WIN_WIDTH / 2 + 76 + _gui_size, WIN_HEIGHT / 2 + 90 * _gui_size + 6 * _gui_size + _gui_size, 20, false, "Create New World");
	_text->addText(WIN_WIDTH / 2 + 40 * _gui_size - 80 + _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size + 6 * _gui_size + _gui_size, 20, false, "Re-Create");
	_text->addText(WIN_WIDTH / 2 + 128 * _gui_size - 80 + _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size + 6 * _gui_size + _gui_size, 20, false, "Cancel");
	// white
	_text->addText(WIN_WIDTH / 2 - 406, WIN_HEIGHT / 2 + 90 * _gui_size + 6 * _gui_size, 20, true, "Play Selected World");
	_text->addText(WIN_WIDTH / 2 - 390, WIN_HEIGHT / 2 + 115 * _gui_size + 6 * _gui_size, 20, true, "Edit");
	_text->addText(WIN_WIDTH / 2 - 180, WIN_HEIGHT / 2 + 115 * _gui_size + 6 * _gui_size, 20, true, "Delete");
	_text->addText(WIN_WIDTH / 2 + 76, WIN_HEIGHT / 2 + 90 * _gui_size + 6 * _gui_size, 20, true, "Create New World");
	_text->addText(WIN_WIDTH / 2 + 40 * _gui_size - 80, WIN_HEIGHT / 2 + 115 * _gui_size + 6 * _gui_size, 20, true, "Re-Create");
	_text->addText(WIN_WIDTH / 2 + 128 * _gui_size - 80, WIN_HEIGHT / 2 + 115 * _gui_size + 6 * _gui_size, 20, true, "Cancel");

	for (int index = 0; index < static_cast<int>(_worlds.size()) && index < 8; index++) {
		std::string str = _worlds[index].substr(0, _worlds[index].size() - 5);
		_text->addText(WIN_WIDTH / 2 - str.size() * 10 + _gui_size, (30 + 20 * index) * _gui_size + 6 * _gui_size + _gui_size, 20, false, str);
		_text->addText(WIN_WIDTH / 2 - str.size() * 10, (30 + 20 * index) * _gui_size + 6 * _gui_size, 20, true, str);
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
		_state = MENU::PAUSE;
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
			_state = MENU::LOAD;
			reset_values();
			return (5);
		} else if (_selection == 2) { // Save and Quit to Title
			_state = MENU::MAIN;
			reset_values();
			return (6);
		}
	}

	setup_array_buffer_death();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	// first draw text shadow
	_text->addText(WIN_WIDTH / 2 - 150 + _gui_size, WIN_HEIGHT / 2 - 60 * _gui_size - 29 + _gui_size, 40, false, "You Died!");
	_text->addText(WIN_WIDTH / 2 - 70 + _gui_size, WIN_HEIGHT / 2 - 5 * _gui_size + 6 * _gui_size + _gui_size, 20, false, "Respawn");
	_text->addText(WIN_WIDTH / 2 - 110 + _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size + 6 * _gui_size + _gui_size, 20, false, "Title Screen");

	// then draw text in white
	_text->addText(WIN_WIDTH / 2 - 150, WIN_HEIGHT / 2 - 60 * _gui_size - 29, 40, true, "You Died!");
	_text->addText(WIN_WIDTH / 2 - 70, WIN_HEIGHT / 2 + - 5 * _gui_size + 6 * _gui_size, 20, true, "Respawn");
	_text->addText(WIN_WIDTH / 2 - 110, WIN_HEIGHT / 2 + 30 * _gui_size + 6 * _gui_size, 20, true, "Title Screen");
	return (0);
}

int Menu::pause_menu( void )
{
	if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (_selection == 1) { //Back to Game
			reset_values();
			return (1);
		} else if (_selection == 6) { // Options...
			_state = MENU::OPTIONS;
			reset_values();
			return (0);
		} else if (_selection == 8) { //Save and Quit to Title
			_state = MENU::MAIN;
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

	// first draw text shadow
	// _text->addText(WIN_WIDTH / 2 - 100 + _gui_size, WIN_HEIGHT / 2 - 60 * _gui_size - 40 + _gui_size, 24, false, "Game Menu");
	_text->addText(WIN_WIDTH / 2 - 38 * _gui_size + _gui_size, WIN_HEIGHT / 2 - 60 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "Back to Game");
	_text->addText(WIN_WIDTH / 2 - 50 * _gui_size - 42 * _gui_size + _gui_size, WIN_HEIGHT / 2 - 35 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "Advancements");
	_text->addText(WIN_WIDTH / 2 - 50 * _gui_size - 44 * _gui_size + _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "Give Feedback");
	_text->addText(WIN_WIDTH / 2 - 50 * _gui_size - 28 * _gui_size + _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "Options...");
	_text->addText(WIN_WIDTH / 2 + 50 * _gui_size - 24 * _gui_size + _gui_size, WIN_HEIGHT / 2 - 35 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "Statistics");
	_text->addText(WIN_WIDTH / 2 + 50 * _gui_size - 35 * _gui_size + _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "Report Bugs");
	_text->addText(WIN_WIDTH / 2 + 50 * _gui_size - 35 * _gui_size + _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "Open to LAN");
	_text->addText(WIN_WIDTH / 2 - 70 * _gui_size + _gui_size, WIN_HEIGHT / 2 + 40 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "Save and Quit to Title");

	// then draw text in white
	_text->addText(WIN_WIDTH / 2 - 25 * (_gui_size + 1), WIN_HEIGHT / 2 - 60 * _gui_size - 40, (_gui_size + 1) * 7, true, "Game Menu");
	_text->addText(WIN_WIDTH / 2 - 38 * _gui_size, WIN_HEIGHT / 2 - 60 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "Back to Game");
	_text->addText(WIN_WIDTH / 2 - 50 * _gui_size - 42 * _gui_size, WIN_HEIGHT / 2 - 35 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "Advancements");
	_text->addText(WIN_WIDTH / 2 - 50 * _gui_size - 44 * _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "Give Feedback");
	_text->addText(WIN_WIDTH / 2 - 50 * _gui_size - 28 * _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "Options...");
	_text->addText(WIN_WIDTH / 2 + 50 * _gui_size - 24 * _gui_size, WIN_HEIGHT / 2 - 35 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "Statistics");
	_text->addText(WIN_WIDTH / 2 + 50 * _gui_size - 35 * _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "Report Bugs");
	_text->addText(WIN_WIDTH / 2 + 50 * _gui_size - 35 * _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "Open to LAN");
	_text->addText(WIN_WIDTH / 2 - 70 * _gui_size, WIN_HEIGHT / 2 + 40 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "Save and Quit to Title");
	return (0);
}

int Menu::options_menu( void )
{
	if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (_selection == 1) { // FOV

		} else if (_selection == 11) { // Done
			_state = (_state == MENU::OPTIONS) ? MENU::PAUSE : MENU::MAIN;
			reset_values();
			return (0);
		}
	}
	if (_esc_released && glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		_state = (_state == MENU::OPTIONS) ? MENU::PAUSE : MENU::MAIN;
		reset_values();
		return (0);
	} else if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
		_esc_released = true;
	}

	setup_array_buffer_options();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	// first draw text shadow
	// _text->addText(WIN_WIDTH / 2 - 100 + _gui_size, WIN_HEIGHT / 2 - 60 * _gui_size - 40 + _gui_size, 24, false, "Options");
	_text->addText(WIN_WIDTH / 2 - 102 * _gui_size - 10 * _gui_size + _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "FOV");
	_text->addText(WIN_WIDTH / 2 + 102 * _gui_size - 58 * _gui_size + _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "Realms Notifications");
	_text->addText(WIN_WIDTH / 2 - 102 * _gui_size - 53 * _gui_size + _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "Skin Customization");
	_text->addText(WIN_WIDTH / 2 - 102 * _gui_size - 48 * _gui_size + _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "Video Settings...");
	_text->addText(WIN_WIDTH / 2 - 102 * _gui_size - 33 * _gui_size + _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "Language...");
	_text->addText(WIN_WIDTH / 2 - 102 * _gui_size - 51 * _gui_size + _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "Resource Packs...");
	_text->addText(WIN_WIDTH / 2 + 102 * _gui_size - 48 * _gui_size + _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "Music & Sounds...");
	_text->addText(WIN_WIDTH / 2 + 102 * _gui_size - 29 * _gui_size + _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "Controls...");
	_text->addText(WIN_WIDTH / 2 + 102 * _gui_size - 38 * _gui_size + _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "Chat Settings");
	_text->addText(WIN_WIDTH / 2 + 102 * _gui_size - 61 * _gui_size + _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "Accessibility Settings");
	_text->addText(WIN_WIDTH / 2 - 10 * _gui_size + _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size + 6 * _gui_size + _gui_size, 7 * _gui_size, false, "Done");

	// then draw text in white
	_text->addText(WIN_WIDTH / 2 - 22 * (_gui_size + 1), WIN_HEIGHT / 2 - 105 * _gui_size, (_gui_size + 1) * 7, true, "Options");
	_text->addText(WIN_WIDTH / 2 - 102 * _gui_size - 10 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "FOV");
	_text->addText(WIN_WIDTH / 2 + 102 * _gui_size - 58 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "Realms Notifications");
	_text->addText(WIN_WIDTH / 2 - 102 * _gui_size - 53 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "Skin Customization");
	_text->addText(WIN_WIDTH / 2 - 102 * _gui_size - 47 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "Video Settings...");
	_text->addText(WIN_WIDTH / 2 - 102 * _gui_size - 33 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "Language...");
	_text->addText(WIN_WIDTH / 2 - 102 * _gui_size - 51 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "Resource Packs...");
	_text->addText(WIN_WIDTH / 2 + 102 * _gui_size - 48 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "Music & Sounds...");
	_text->addText(WIN_WIDTH / 2 + 102 * _gui_size - 29 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "Controls...");
	_text->addText(WIN_WIDTH / 2 + 102 * _gui_size - 38 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "Chat Settings");
	_text->addText(WIN_WIDTH / 2 + 102 * _gui_size - 61 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "Accessibility Settings");
	_text->addText(WIN_WIDTH / 2 - 10 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size + 6 * _gui_size, 7 * _gui_size, true, "Done");
	return (0);
}

int Menu::ingame_inputs( void )
{
	int craft = _state + 1 - MENU::INVENTORY; // craft = 1: inventory, 2: crafting, 3: furnace
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
			if (_selected_block.type == blocks::AIR) {
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
			if (_selected_block.type == blocks::AIR) {
				if (_right_released) {
					_selected_block = _inventory.pickHalfBlockAt(craft, _selection - 1, _furnace, _chest);
					if (_selected_block.type != blocks::AIR) {
						_selection_list.push_back(_selection);
					}
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
					int count = _selected_block.amount;
					_selected_block = _inventory.putOneBlockAt(craft, _selection - 1, _selected_block, _furnace, _chest);
					if (_selected_block.amount < count) {
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
		case MENU::INVENTORY:
			setup_array_buffer_inventory();
			break ;
		case MENU::CRAFTING:
			setup_array_buffer_crafting();
			break ;
		case MENU::CHEST:
			setup_array_buffer_chest();
			break ;
		case MENU::FURNACE:
			setup_array_buffer_furnace();
			break ;
	}

	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);
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

void Menu::setup_array_buffer_main( void )
{
	_vertices.push_back({1, WIN_WIDTH / 2 - 129 * _gui_size, WIN_HEIGHT / 2 - 104 * _gui_size, 256 * _gui_size, 64 * _gui_size, 0, 131, 256, 64}); // MINECRAFT

    _vertices.push_back({1, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 1) ? 111 : 91, 200, 20}); // Singleplayer
    _vertices.push_back({1, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Multiplayer
    _vertices.push_back({1, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 40 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Minecraft Realms

    _vertices.push_back({1, WIN_WIDTH / 2 - 125 * _gui_size, WIN_HEIGHT / 2 + 80 * _gui_size, 15 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Lang settings
    _vertices.push_back({1, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 80 * _gui_size, 95 * _gui_size, 20 * _gui_size, 0, (_selection == 5) ? 111 : 91, 200, 20}); // Options...
    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 80 * _gui_size, 95 * _gui_size, 20 * _gui_size, 0, (_selection == 6) ? 111 : 91, 200, 20}); // Quit Game
    _vertices.push_back({1, WIN_WIDTH / 2 + 110 * _gui_size, WIN_HEIGHT / 2 + 80 * _gui_size, 15 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // accessibility settings

	setup_shader();
}

void Menu::setup_array_buffer_select( void )
{
	_vertices.push_back({1, WIN_WIDTH / 2 - 155 * _gui_size, WIN_HEIGHT / 2 + 90 * _gui_size, 150 * _gui_size, 20 * _gui_size, 0, (91 + 20 * (_selection == 1)) * (_selected_world != 0) + 71 * (_selected_world == 0), 200, 20}); // playSelectedWorld
	_vertices.push_back({1, WIN_WIDTH / 2 - 155 * _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size, 73 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // edit
	_vertices.push_back({1, WIN_WIDTH / 2 - 78 * _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size, 73 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // delete
	_vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 90 * _gui_size, 150 * _gui_size, 20 * _gui_size, 0, (_selection == 4) ? 111 : 91, 200, 20}); // createNewWorld
	_vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size, 73 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // reCreate
	_vertices.push_back({1, WIN_WIDTH / 2 + 82 * _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size, 73 * _gui_size, 20 * _gui_size, 0, (_selection == 6) ? 111 : 91, 200, 20}); // cancel

	for (int index = 0; index < static_cast<int>(static_cast<int>(_worlds.size())) && index < 8; index++) {
		_vertices.push_back({1, WIN_WIDTH / 2 - 100 * _gui_size, (30 + 20 * index) * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71 + 20 * (_selected_world - 1 == index), 200, 20}); // world #index
	}

	setup_shader();
}

void Menu::setup_array_buffer_load( int completion )
{
	// std::cout << "current completion : " << completion << std::endl;
	_vertices.push_back({1, WIN_WIDTH / 2 - 110, WIN_HEIGHT / 2 - 120, 220, 240, 0, 71, 200, 20});

	int step = 1, turnCounter = 0, numSteps = 1, stepSize = 20, state = 0, x = WIN_WIDTH / 2 - 20, y = WIN_HEIGHT / 2;
	for (; step < 1 + 100; step++) {
		int offset = (step < 1 + completion / 10) ? 48 : 32; // cobblestone or stone
		_vertices.push_back({0, x, y, stepSize, stepSize, 64, offset, 16, 16});
		if (step == 1 + completion / 10) {
			_vertices.push_back({0, x, y, stepSize, stepSize, 240, 32 * ((completion % 10) != 0) + 16 * (completion % 10), 16, 16}); // break frame
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

	setup_shader();
}

void Menu::setup_array_buffer_death( void )
{
	_vertices.push_back({1, 0, 0, WIN_WIDTH, WIN_HEIGHT, 16, 15, 1, 1}); // occult window in redish
    _vertices.push_back({1, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 1) ? 111 : 91, 200, 20}); // Respawn
    _vertices.push_back({1, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 2) ? 111 : 91, 200, 20}); // Title Screen

	setup_shader();
}

void Menu::setup_array_buffer_pause( void )
{
	_vertices.push_back({1, 0, 0, WIN_WIDTH, WIN_HEIGHT, 3, 29, 1, 1}); // occult window
    _vertices.push_back({1, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 60 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 1) ? 111 : 91, 200, 20}); // Back to Game

    _vertices.push_back({1, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 35 * _gui_size, 95 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Advancements
    _vertices.push_back({1, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size, 95 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Give Feedback
    _vertices.push_back({1, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size, 95 * _gui_size, 20 * _gui_size, 0, (_selection == 6) ? 111 : 91, 200, 20}); // Options...

    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 35 * _gui_size, 95 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Statistics
    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size, 95 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Report Bugs
    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size, 95 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Open to LAN

    _vertices.push_back({1, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 40 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 8) ? 111 : 91, 200, 20}); // Save and Quit to Title

	setup_shader();
}

void Menu::setup_array_buffer_options( void )
{
	_vertices.push_back({1, 0, 0, WIN_WIDTH, WIN_HEIGHT, 3, 29, 1, 1}); // occult window
    _vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // FOV
    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Realms Notifications

    _vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Skin Customization...
    _vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Video Settings...
    _vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Language...
    _vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Resource Packs...

    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Music & Sounds...
    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Controls...
    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Chat Settings...
    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Accessibility Settings...

    _vertices.push_back({1, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 11) ? 111 : 91, 200, 20}); // Done

	setup_shader();
}

void Menu::setup_array_buffer_chat( void )
{
	int nbr = _chat->computeHeight();
    _vertices.push_back({1, CHAT_BOX_X, WIN_HEIGHT - 48 - 8 - 18 * (nbr + 1), CHAT_BOX_WIDTH, 4 + 18 * nbr, 3, 29, 1, 1}); // occult chat box
    _vertices.push_back({1, CHAT_BOX_X, WIN_HEIGHT - 48 - 18, CHAT_BOX_WIDTH, 20, 3, 29, 1, 1}); // occult input box

	setup_shader();
}

void Menu::occult_selection( void )
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
			_vertices.push_back({1, (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * index * _gui_size) + _gui_size * 3, WIN_HEIGHT / 2 + 59 * _gui_size, 16 * _gui_size, 16 * _gui_size, 16, 14, 1, 1});
		} else if (cell < 37) { // backpack
			int index = cell - 10;
			_vertices.push_back({1, (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (index % 9) * _gui_size) + _gui_size * 3, WIN_HEIGHT / 2 + _gui_size + 18 * _gui_size * (index / 9), 16 * _gui_size, 16 * _gui_size, 16, 14, 1, 1});
		} else if (cell < 41) { // icraft
			int index = cell - 37;
			_vertices.push_back({1, (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (5 + index % 2) * _gui_size) + _gui_size * 3, WIN_HEIGHT / 2 - 65 * _gui_size + 18 * _gui_size * (index / 2), 16 * _gui_size, 16 * _gui_size, 16, 14, 1, 1});
		} else if (cell == 41) { // crafted
			if (_state == MENU::INVENTORY) {
				_vertices.push_back({1, (WIN_WIDTH - (166 * _gui_size)) / 2 + 149 * _gui_size, WIN_HEIGHT / 2 - 55 * _gui_size, 16 * _gui_size, 16 * _gui_size, 16, 14, 1, 1});
			} else if (_state == MENU::CRAFTING) {
				_vertices.push_back({1, (WIN_WIDTH - (166 * _gui_size)) / 2 + 119 * _gui_size, WIN_HEIGHT / 2 - 48 * _gui_size, 16 * _gui_size, 16 * _gui_size, 16, 14, 1, 1});
			} else if (_state == MENU::FURNACE) {
				_vertices.push_back({1, (WIN_WIDTH - (166 * _gui_size)) / 2 + 111 * _gui_size, WIN_HEIGHT / 2 - 48 * _gui_size, 16 * _gui_size, 16 * _gui_size, 16, 14, 1, 1});
			}
		} else if (cell < 51) { // craft
			int index = cell - 42;
			_vertices.push_back({1, (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (1 + index % 3) * _gui_size) + _gui_size * 7, WIN_HEIGHT / 2 - 66 * _gui_size + 18 * _gui_size * (index / 3), 16 * _gui_size, 16 * _gui_size, 16, 14, 1, 1});
		} else if (cell == 51) { // composant
			_vertices.push_back({1, (WIN_WIDTH - (166 * _gui_size)) / 2 + 51 * _gui_size, WIN_HEIGHT / 2 - 66 * _gui_size, 16 * _gui_size, 16 * _gui_size, 16, 14, 1, 1});
		} else if (cell == 52) { // fuel
			_vertices.push_back({1, (WIN_WIDTH - (166 * _gui_size)) / 2 + 51 * _gui_size, WIN_HEIGHT / 2 - 30 * _gui_size, 16 * _gui_size, 16 * _gui_size, 16, 14, 1, 1});
		} else if (cell < 80) { // chest
			int index = cell - 53;
			_vertices.push_back({1, (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (index % 9) * _gui_size) + _gui_size * 3, WIN_HEIGHT / 2 - 65 * _gui_size + 18 * _gui_size * (index / 9), 16 * _gui_size, 16 * _gui_size, 16, 14, 1, 1});
		}
	}
	if (!found) {
		_selection_list.pop_back();
	}
}

void Menu::add_dura_value( glm::ivec2 dura, int x, int y )
{
	if (dura.y == 0 || dura.x == dura.y) {
		return ;
	}
	// adding grey bar first
	_vertices.push_back({0, x + _gui_size, y + 14 * _gui_size, 14 * _gui_size, _gui_size, 64, 0, 1, 1});
	// adding progress bar second
	float percent = 1.0f * dura.x / dura.y;
	_vertices.push_back({0, x + _gui_size, y + 14 * _gui_size, static_cast<int>(14 * _gui_size * percent), _gui_size, 103 * (percent < 0.6f) - (percent < 0.3), 16 + 9 * (percent < 0.6f) - 18 * (percent < 0.3f), 1, 1});
}

void Menu::display_item_value( int x, int y, int amount )
{
	if (amount <= 1) {
		return ;
	}
	if (amount > 9) {
		_text->addText(x + 11 * _gui_size - 6 * _gui_size + _gui_size, y + 9 * _gui_size + _gui_size, 8 * _gui_size, false, std::to_string(amount / 10));
		_text->addText(x + 11 * _gui_size - 6 * _gui_size, y + 9 * _gui_size, 8 * _gui_size, true, std::to_string(amount / 10));
	}
	_text->addText(x + 11 * _gui_size + _gui_size, y + 9 * _gui_size + _gui_size, 8 * _gui_size, false, std::to_string(amount % 10));
	_text->addText(x + 11 * _gui_size, y + 9 * _gui_size, 8 * _gui_size, true, std::to_string(amount % 10));
}

void Menu::add_item_value( t_item item, int x, int y, bool movement )
{
	// TODO change item to ivec3(type, amount, dura) + handle durabilities over here
	display_item_value(x, y, item.amount);
	add_dura_value(item.dura, x, y);
	int type = item.type;
	if (!s_blocks[type]->item3D) {
		int spec = s_blocks[type]->texX(face_dir::MINUSX, 0) + (s_blocks[type]->texY(face_dir::MINUSX, 0) << 4) + (3 << 19);
		// int faceLight = computeLight(row - 1, col, level);
		int cornerLight = 15;//computeSmoothLight(faceLight, row - 1, col, level, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		int shade = 0;//computeShade(row - 1, col, level, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		// spec += (faceLight << 24);
		glm::ivec3 v0 = {spec + (cornerLight << 24) + (shade << 22), x, y};
		// cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		// shade = computeShade(row - 1, col, level, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		glm::ivec3 v1 = {spec + (cornerLight << 24) + (shade << 22) + 1 + (1 << 9) + (1 << 8), x + 16 * _gui_size, y};
		// cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		// shade = computeShade(row - 1, col, level, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		glm::ivec3 v2 = {spec + (cornerLight << 24) + (shade << 22) + (1 << 4) + (1 << 10) + (1 << 12), x, y + 16 * _gui_size};
		// cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		// shade = computeShade(row - 1, col, level, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		glm::ivec3 v3 = {spec + (cornerLight << 24) + (shade << 22) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), x + 16 * _gui_size, y + 16 * _gui_size};
		_ui->addFace(v0, v1, v2, v3, true, movement);
		return ;
	}
	x += _gui_size;
	int offset = face_dir::PLUSX;
	// top face
	int spec = (15 << 24) + s_blocks[type]->texX(face_dir::PLUSZ, offset) + (s_blocks[type]->texY(face_dir::PLUSZ, offset) << 4);
	// if (type == blocks::DIRT_PATH) {
	// 	p4.z -= ONE_SIXTEENTH;
	// 	p5.z -= ONE_SIXTEENTH;
	// 	p0.z -= ONE_SIXTEENTH;
	// 	p1.z -= ONE_SIXTEENTH;
	// }
	glm::ivec3 v0 = {spec, x, y + 16 * _gui_size * 81.25f / 362.5f};
	glm::ivec3 v1 = {spec + 1 + (1 << 9) + (1 << 8), x + 7 * _gui_size, y};
	glm::ivec3 v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), x + 7 * _gui_size, y + 16 * _gui_size * 162.5f / 362.5f};
	glm::ivec3 v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), x + 14 * _gui_size, y + 16 * _gui_size * 81.25f / 362.5f};
	_ui->addFace(v0, v1, v2, v3, true, movement);
	// left face
	spec = (10 << 24) + s_blocks[type]->texX(face_dir::MINUSY, offset) + (s_blocks[type]->texY(face_dir::MINUSY, offset) << 4);
	v0 = {spec, x, y + 16 * _gui_size * 81.25f / 362.5f};
	v1 = {spec + 1 + (1 << 9) + (1 << 8), x + 7 * _gui_size, y + 16 * _gui_size * 162.5f / 362.5f};
	v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), x, y + 16 * _gui_size * 281.25f / 362.5f};
	v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), x + 7 * _gui_size, y + 16 * _gui_size};
	_ui->addFace(v0, v1, v2, v3, true, movement);
	// right face
	spec = (7 << 24) + s_blocks[type]->texX(face_dir::PLUSX, offset) + (s_blocks[type]->texY(face_dir::PLUSX, offset) << 4);
	v0 = {spec, x + 7 * _gui_size, y + 16 * _gui_size * 162.5f / 362.5f};
	v1 = {spec + 1 + (1 << 9) + (1 << 8), x + 14 * _gui_size, y + 16 * _gui_size * 81.25f / 362.5f};
	v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), x + 7 * _gui_size, y + 16 * _gui_size};
	v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), x + 14 * _gui_size, y + 16 * _gui_size * 281.25f / 362.5f};
	_ui->addFace(v0, v1, v2, v3, true, movement);
}

void Menu::add_slot_value( int index )
{
	t_item item = _inventory.getSlotBlock(index);
	if (item.type == blocks::AIR) {
		return ;
	}
	int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * index * _gui_size) + _gui_size * 3;
	int y = WIN_HEIGHT / 2 + 59 * _gui_size;
	add_item_value(item, x, y, _gui_size);
}

void Menu::add_backpack_value( int index )
{
	t_item item = _inventory.getBackpackBlock(index);
	if (item.type == blocks::AIR) {
		return ;
	}
	int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (index % 9) * _gui_size) + _gui_size * 3;
	int y = WIN_HEIGHT / 2 + _gui_size + 18 * _gui_size * (index / 9);
	add_item_value(item, x, y, _gui_size);
}

void Menu::add_icraft_value( int index )
{
	t_item item = _inventory.getiCraftBlock(index);
	if (item.type == blocks::AIR) {
		return ;
	}
	int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (5 + index % 2) * _gui_size) + _gui_size * 3;
	int y = WIN_HEIGHT / 2 - 65 * _gui_size + 18 * _gui_size * (index / 2);
	add_item_value(item, x, y, _gui_size);
}

void Menu::add_craft_value( int index )
{
	t_item item = _inventory.getCraftBlock(index);
	if (item.type == blocks::AIR) {
		return ;
	}
	int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (1 + index % 3) * _gui_size) + _gui_size * 7;
	int y = WIN_HEIGHT / 2 - 66 * _gui_size + 18 * _gui_size * (index / 3);
	add_item_value(item, x, y, _gui_size);
}

void Menu::add_crafted_value( void )
{
	t_item item = _inventory.getCrafted();
	if (item.type == blocks::AIR) {
		return ;
	}
	if (_state == MENU::INVENTORY) {
		int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + 149 * _gui_size;
		int y = WIN_HEIGHT / 2 - 55 * _gui_size;
		add_item_value(item, x, y, _gui_size);
	} else if (_state == MENU::CRAFTING) {
		int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + 119 * _gui_size;
		int y = WIN_HEIGHT / 2 - 48 * _gui_size;
		add_item_value(item, x, y, _gui_size);
	}
}

void Menu::add_chest_value( int index )
{
	t_item *item = _chest->getItem(index);
	if (!item) {
		// std::cout << "add_chest_value NULL item at " << index << std::endl;
		return ;
	}
	if (item->type == blocks::AIR) {
		return ;
	}
	int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (index % 9) * _gui_size) + _gui_size * 3;
	int y = WIN_HEIGHT / 2 - 65 * _gui_size + 18 * _gui_size * (index / 9);
	add_item_value(*item, x, y, _gui_size);
}

void Menu::add_furnace_value( void )
{
	t_item item = _furnace->getComposant();
	if (item.type != blocks::AIR) {
		int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + 51 * _gui_size;
		int y = WIN_HEIGHT / 2 - 66 * _gui_size;
		add_item_value(item, x, y, _gui_size);
	}
	int progress = 1 + glm::floor(_furnace->getFuelTime() * 13);
	if (_furnace->getFuelTime()) {
		_vertices.push_back({1, (WIN_WIDTH - (166 * _gui_size)) / 2 + 50 * _gui_size + _gui_size, WIN_HEIGHT / 2 - 48 * _gui_size + progress * _gui_size + _gui_size, 14 * _gui_size, (14 - progress) * _gui_size, 24, 47 + progress, 14, 14 - progress});
	}
	progress = 1 + glm::floor(_furnace->getComposantTime() * 23);
	if (_furnace->getComposantTime()) {
		_vertices.push_back({1, (WIN_WIDTH - (166 * _gui_size)) / 2 + 74 * _gui_size, WIN_HEIGHT / 2 - 49 * _gui_size, progress * _gui_size, 17 * _gui_size, 38, 47, progress, 17});
	}
	item = _furnace->getFuel();
	if (item.type != blocks::AIR) {
		int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + 51 * _gui_size;
		int y = WIN_HEIGHT / 2 - 30 * _gui_size;
		add_item_value(item, x, y, _gui_size);
	}
	item = _furnace->getProduction();
	if (item.type != blocks::AIR) {
		int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + 111 * _gui_size;
		int y = WIN_HEIGHT / 2 - 48 * _gui_size;
		add_item_value(item, x, y, _gui_size);
	}
}

void Menu::setup_array_buffer_inventory( void )
{
	_vertices.push_back({2, WIN_WIDTH / 2 - 88 * _gui_size, WIN_HEIGHT / 2 - 83 * _gui_size, 176 * _gui_size, 166 * _gui_size, 0, 0, 128, 128});

	occult_selection();
	for (int index = 0; index < 9; index++) {
		add_slot_value(index);
	}
	for (int index = 0; index < 27; index++) {
		add_backpack_value(index);
	}
	for (int index = 0; index < 4; index++) {
		add_icraft_value(index);
	}
	add_crafted_value();

	if (_selected_block.type != blocks::AIR) {
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		add_item_value(_selected_block, mouseX - 8 * _gui_size, mouseY - 8 * _gui_size, true);
	}

	setup_shader();
}

void Menu::setup_array_buffer_crafting( void )
{
	_vertices.push_back({2, WIN_WIDTH / 2 - 88 * _gui_size, WIN_HEIGHT / 2 - 83 * _gui_size, 176 * _gui_size, 166 * _gui_size, 128, 0, 128, 128});

	occult_selection();
	for (int index = 0; index < 9; index++) {
		add_slot_value(index);
	}
	for (int index = 0; index < 27; index++) {
		add_backpack_value(index);
	}
	for (int index = 0; index < 9; index++) {
		add_craft_value(index);
	}
	add_crafted_value();

	if (_selected_block.type != blocks::AIR) {
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		add_item_value(_selected_block, mouseX - 8 * _gui_size, mouseY - 8 * _gui_size, true);
	}

	setup_shader();
}

void Menu::setup_array_buffer_chest( void )
{
	_vertices.push_back({2, WIN_WIDTH / 2 - 88 * _gui_size, WIN_HEIGHT / 2 - 83 * _gui_size, 176 * _gui_size, 166 * _gui_size, 128, 128, 128, 128});

	occult_selection();
	for (int index = 0; index < 9; index++) {
		add_slot_value(index);
	}
	for (int index = 0; index < 27; index++) {
		add_backpack_value(index);
	}
	for (int index = 0; index < 27; index++) {
		add_chest_value(index);
	}

	if (_selected_block.type != blocks::AIR) {
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		add_item_value(_selected_block, mouseX - 8 * _gui_size, mouseY - 8 * _gui_size, true);
	}

	setup_shader();
}

void Menu::setup_array_buffer_furnace( void )
{
	_vertices.push_back({2, WIN_WIDTH / 2 - 88 * _gui_size, WIN_HEIGHT / 2 - 83 * _gui_size, 176 * _gui_size, 166 * _gui_size, 0, 128, 128, 128});

	occult_selection();
	for (int index = 0; index < 9; index++) {
		add_slot_value(index);
	}
	for (int index = 0; index < 27; index++) {
		add_backpack_value(index);
	}
	add_furnace_value();

	if (_selected_block.type != blocks::AIR) {
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		add_item_value(_selected_block, mouseX - 8 * _gui_size, mouseY - 8 * _gui_size, true);
	}

	setup_shader();
}

void Menu::setup_shader( void )
{
	_nb_points = _vertices.size();
	if (!_nb_points) {
		return ;
	}

	glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
	_vaoSet = true;

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _nb_points * 9 * sizeof(GLint), &_vertices[0][0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(UI_ATLASATTRIB);
	glVertexAttribIPointer(UI_ATLASATTRIB, 1, GL_INT, 9 * sizeof(GLint), 0);

    glEnableVertexAttribArray(UI_POSATTRIB);
	glVertexAttribIPointer(UI_POSATTRIB, 4, GL_INT, 9 * sizeof(GLint), (void *)(sizeof(GLint)));
	
	glEnableVertexAttribArray(UI_TEXATTRIB);
	glVertexAttribIPointer(UI_TEXATTRIB, 4, GL_INT, 9 * sizeof(GLint), (void *)(5 * sizeof(GLint)));

	check_glstate("Menu::setup_shader", false);

	_vertices.clear();
	_vertices.reserve(_nb_points);
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
	if (_state == MENU::MAIN) {
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 80 * _gui_size, 95 * _gui_size, 20 * _gui_size)) {
			_selection = 5;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 80 * _gui_size, 95 * _gui_size, 20 * _gui_size)) {
			_selection = 6;
		} else {
			_selection = 0;
		}
	} else if (_state == MENU::WORLD_SELECT) {
		if (_selected_world && inRectangle(posX, posY, WIN_WIDTH / 2 - 155 * _gui_size, WIN_HEIGHT / 2 + 90 * _gui_size, 150 * _gui_size, 20 * _gui_size)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 90 * _gui_size, 150 * _gui_size, 20 * _gui_size)) {
			_selection = 4;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 82 * _gui_size, WIN_HEIGHT / 2 + 105 * _gui_size, 73 * _gui_size, 20 * _gui_size)) {
			_selection = 6;
		} else {
			_selection = 0;
		}
		for (int index = 0; index < static_cast<int>(_worlds.size()) && index < 8; index++) {
			if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, (30 + 20 * index) * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
				_selection = index + 7;
			}
		}
	} else if (_state == MENU::DEATH) {
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 5 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 2;
		} else {
			_selection = 0;
		}
	} else if (_state == MENU::PAUSE) {
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 60 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size, 95 * _gui_size, 20 * _gui_size)) {
			_selection = 6;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 40 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 8;
		} else {
			_selection = 0;
		}
	} else if (_state == MENU::OPTIONS || _state == MENU::MAIN_OPTIONS) {
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 11;
		} else {
			_selection = 0;
		}
	} else if (_state >= MENU::INVENTORY) {
		for (int index = 0; index < 9; index++) {
			if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * index * _gui_size) + _gui_size * 3, WIN_HEIGHT / 2 + 59 * _gui_size, 16 * _gui_size, 16 * _gui_size)) {
				_selection = index + 1;
				return ;
			}
		}
		for (int index = 0; index < 27; index++) {
			if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (index % 9) * _gui_size) + _gui_size * 3, WIN_HEIGHT / 2 + _gui_size + 18 * _gui_size * (index / 9), 16 * _gui_size, 16 * _gui_size)) {
				_selection = index + 10;
				return ;
			}
		}
		switch (_state) {
			case MENU::INVENTORY:
				for (int index = 0; index < 4; index++) {
					if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (5 + index % 2) * _gui_size) + _gui_size * 3, WIN_HEIGHT / 2 - 65 * _gui_size + 18 * _gui_size * (index / 2), 16 * _gui_size, 16 * _gui_size)) {
						_selection = index + 37;
						return ;
					}
				}
				if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + 149 * _gui_size, WIN_HEIGHT / 2 - 55 * _gui_size, 16 * _gui_size, 16 * _gui_size)) {
					_selection = 41;
					return ;
				}
				break ;
			case MENU::CRAFTING:
				for (int index = 0; index < 9; index++) {
					if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (1 + index % 3) * _gui_size) + _gui_size * 7, WIN_HEIGHT / 2 - 66 * _gui_size + 18 * _gui_size * (index / 3), 16 * _gui_size, 16 * _gui_size)) {
						_selection = index + 42;
						return ;
					}
				}
				if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + 119 * _gui_size, WIN_HEIGHT / 2 - 48 * _gui_size, 16 * _gui_size, 16 * _gui_size)) {
					_selection = 41;
					return ;
				}
				break ;
			case MENU::CHEST:
				for (int index = 0; index < 27; index++) {
					if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (index % 9) * _gui_size) + _gui_size * 3, WIN_HEIGHT / 2 - 65 * _gui_size + 18 * _gui_size * (index / 9), 16 * _gui_size, 16 * _gui_size)) {
						_selection = index + 53;
						return ;
					}
				}
				break ;
			case MENU::FURNACE:
				if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + 51 * _gui_size, WIN_HEIGHT / 2 - 66 * _gui_size, 16 * _gui_size, 16 * _gui_size)) {
					_selection = 51;
					return ;
				}
				if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + 51 * _gui_size, WIN_HEIGHT / 2 - 30 * _gui_size, 16 * _gui_size, 16 * _gui_size)) {
					_selection = 52;
					return ;
				}
				if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + 111 * _gui_size, WIN_HEIGHT / 2 - 48 * _gui_size, 16 * _gui_size, 16 * _gui_size)) {
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
	if (_state == MENU::LOAD) {
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

	if (state == MENU::CHAT) {
		glfwSetCharCallback(_window, INPUT::character_callback);
	}
}

int Menu::getState( void )
{
	return (_state);
}

void Menu::changeGuiSize( int offset )
{
	_gui_size += offset;
	if (_gui_size < 1) _gui_size = 1;
	else if (_gui_size > 6) _gui_size = 6;
}

std::string Menu::getWorldFile( void )
{
	return (_world_file);
}

int Menu::run( GLint render_dist, bool animUpdate )
{
	if (_state != MENU::CHAT && glfwGetKey(_window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(_window, GL_TRUE);
		return (-1);
	}

	switch (_state) {
		case MENU::MAIN:
			return (main_menu());
		case MENU::WORLD_SELECT:
			return (world_select_menu());
		case MENU::LOAD:
			return (loading_screen(render_dist));
		case MENU::DEATH:
			return (death_menu());
		case MENU::PAUSE:
			return (pause_menu());
		case MENU::MAIN_OPTIONS:
		case MENU::OPTIONS:
			return (options_menu());
		case MENU::INVENTORY:
			return (ingame_menu());
		case MENU::CRAFTING:
			return (ingame_menu());
		case MENU::CHEST:
			return (ingame_menu());
		case MENU::FURNACE:
			return (ingame_menu());
		case MENU::CHAT:
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
