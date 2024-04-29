#include "Menu.hpp"
#include "Ui.hpp"
#include "callbacks.hpp"
#include "Settings.hpp"
#include <dirent.h>
extern std::mutex mtx;
extern siv::PerlinNoise::seed_type perlin_seed;

Menu::Menu( Inventory & inventory, UI *ui ) : _gui_size(3), _state(MENU::MAIN), _selection(0), _selected_world(0), _vaoSet(false),
	_textBar(true), _fov_gradient(Settings::Get()->getFloat(SETTINGS::FLOAT::FOV)), _render_gradient(Settings::Get()->getInt(SETTINGS::INT::RENDER_DIST)),
	_brightness_gradient(Settings::Get()->getFloat(SETTINGS::FLOAT::BRIGHTNESS)),
	_inventory(inventory), _ui(ui), _text(ui->getTextPtr()), _chat(ui->getChatPtr()), _chest(NULL), _furnace(NULL)
{
	_world_file = "";
	loadSettings();
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
	_moving_slider = false;
}

MENU::RET Menu::main_menu( void )
{
	if (INPUT::key_down(INPUT::BREAK) && INPUT::key_update(INPUT::BREAK)) {
		if (_selection == 1) { //singleplayer
			_state = MENU::WORLD_SELECT;
			reset_values();			
			DIR *dir = opendir("Worlds");
			if (dir == NULL) {
				std::cerr << "failed to open dir Worlds/" << std::endl;
				_state = MENU::MAIN;
				return (MENU::RET::NO_CHANGE);
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
			return (MENU::RET::NO_CHANGE);
		} else if (_selection == 5) { // options
			_state = MENU::MAIN_OPTIONS;
			reset_values();
			return (MENU::RET::NO_CHANGE);
		} else if (_selection == 6) { //quit game
			glfwSetWindowShouldClose(_window, GL_TRUE);
			saveSettings();
			return (MENU::RET::QUIT);
		}
	}
	if (INPUT::key_down(INPUT::JUMP)) { // skip world selection and launch with default seed
		_state = MENU::LOAD;
		reset_values();
		return (MENU::RET::PLAY_DEFAULT);
	}

	setup_array_buffer_main();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	// _text->addText(WIN_WIDTH / 2 - 220, WIN_HEIGHT / 2 - 120 * 3, 24, true, "Press space to start");

   _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size, 200 * _gui_size, 20 * _gui_size, 8 * _gui_size, true, "Singleplayer");
   _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size, 200 * _gui_size, 20 * _gui_size, 8 * _gui_size, true, "Multiplayer");
   _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 40 * _gui_size, 200 * _gui_size, 20 * _gui_size, 8 * _gui_size, true, "Minecraft Realms");
   _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 80 * _gui_size, 95 * _gui_size, 20 * _gui_size, 8 * _gui_size, true, "Options...");
   _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 80 * _gui_size, 95 * _gui_size, 20 * _gui_size, 8 * _gui_size, true, "Quit Game");
	return (MENU::RET::NO_CHANGE);
}

MENU::RET Menu::world_select_menu( void )
{
	if (INPUT::key_down(INPUT::BREAK) && INPUT::key_update(INPUT::BREAK)) {
		if (_selection == 1) { //play selected world
			_world_file = _worlds[_selected_world - 1];
			_state = MENU::LOAD;
			reset_values();
			return (MENU::RET::WORLD_SELECTED);
		} else if (_selection == 4) { // new random seed
			const auto p1 = std::chrono::system_clock::now(); // works without #include <chrono> #include <ctime> ?
			perlin_seed = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();
			_state = MENU::LOAD;
			reset_values();
			return (MENU::RET::PLAY_DEFAULT);
		} else if (_selection == 6) { //cancel, go back to main menu
			_state = MENU::MAIN;
			reset_values();
			return (MENU::RET::NO_CHANGE);
		} else if (_selection > 6) {
			_selected_world = _selection - 6;
		}
	}

	setup_array_buffer_select();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	_text->addText(WIN_WIDTH / 2 - 120, 30, 24, true, "Select World");

	_text->addCenteredText(WIN_WIDTH / 2 - 155 * _gui_size, WIN_HEIGHT / 2 + 90 * _gui_size, 150 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Play Selected World");
	_text->addCenteredText(WIN_WIDTH / 2 - 155 * _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size, 73 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Edit");
	_text->addCenteredText(WIN_WIDTH / 2 - 78 * _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size, 73 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Delete");
	_text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 90 * _gui_size, 150 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Create New World");
	_text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size, 73 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Re-Create");
	_text->addCenteredText(WIN_WIDTH / 2 + 82 * _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size, 73 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Cancel");

	for (int index = 0; index < static_cast<int>(static_cast<int>(_worlds.size())) && index < 8; index++) {
		std::string str = _worlds[index].substr(0, _worlds[index].size() - 5);
		_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, (30 + 20 * index) * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, str);
	}
	return (MENU::RET::NO_CHANGE);
}

MENU::RET Menu::loading_screen( void )
{
	GLint current_size = 0, newVaoCounter = 0;
	for (auto& c : _chunks) {
		mtx.lock();
		current_size += c->isLoaded(newVaoCounter);
		mtx.unlock();
	}

	int render_dist = Settings::Get()->getInt(SETTINGS::INT::RENDER_DIST);
	GLint goal = (1 + 2 * render_dist) * (1 + 2 * render_dist);
	// std::cout << "CURRENT IS " << current_size << ", GOAL IS " << goal << std::endl;
	if (current_size >= goal) {
		_state = MENU::PAUSE;
		return (MENU::RET::BACK_TO_GAME);
	}
	float percentage = current_size / static_cast<float>(goal) * 100;

	setup_array_buffer_load(percentage * 10);
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	std::string str = "LOADING.. " + std::to_string(percentage).substr(0, 5) + '%' + " (" + std::to_string(current_size) + '/' + std::to_string(goal) + ')';
	_text->addText(WIN_WIDTH / 2 - 24 * str.length() / 2, WIN_HEIGHT / 2 - 150, 24, true, str);
	return (MENU::RET::NO_CHANGE);
}

MENU::RET Menu::death_menu( void )
{
	if (INPUT::key_down(INPUT::BREAK) && INPUT::key_update(INPUT::BREAK)) {
		if (_selection == 1) { // Respawn
			_state = MENU::LOAD;
			reset_values();
			return (MENU::RET::RESPAWN_PLAYER);
		} else if (_selection == 2) { // Save and Quit to Title
			_state = MENU::MAIN;
			reset_values();
			return (MENU::RET::RESPAWN_SAVE_QUIT);
		}
	}

	setup_array_buffer_death();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	_text->addCenteredText(0, WIN_HEIGHT / 2 - 60 * _gui_size, WIN_WIDTH, 20 * _gui_size, 14 * _gui_size, true, "You Died!");
    _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Respawn");
    _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Title Screen");
	return (MENU::RET::NO_CHANGE);
}

MENU::RET Menu::pause_menu( void )
{
	if (INPUT::key_down(INPUT::BREAK) && INPUT::key_update(INPUT::BREAK)) {
		if (_selection == 1) { //Back to Game
			reset_values();
			return (MENU::RET::BACK_TO_GAME);
		} else if (_selection == 6) { // Options...
			_state = MENU::OPTIONS;
			reset_values();
			return (options_menu());
		} else if (_selection == 8) { //Save and Quit to Title
			_state = MENU::MAIN;
			reset_values();
			return (MENU::RET::SAVE_AND_QUIT);
		}
	}
	if (INPUT::key_down(INPUT::CLOSE) && INPUT::key_update(INPUT::CLOSE)) {
		return (MENU::RET::BACK_TO_GAME);
	}

	setup_array_buffer_pause();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	_text->addText(WIN_WIDTH / 2 - 25 * (_gui_size + 1), WIN_HEIGHT / 2 - 60 * _gui_size - 40, (_gui_size + 1) * 7, true, "Game Menu");
	_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 60 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Back to Game");
    _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 35 * _gui_size, 95 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Advancements");
    _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size, 95 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Give Feedback");
    _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size, 95 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Options...");
    _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 35 * _gui_size, 95 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Statistics");
    _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size, 95 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Report Bugs");
    _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size, 95 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Open to LAN");
    _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 40 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Save and Quit to Title");
	return (MENU::RET::NO_CHANGE);
}

MENU::RET Menu::options_menu( void )
{
	_moving_slider = false;
	if (INPUT::key_down(INPUT::BREAK)) {
		if (_selection == 1) { // FOV
			_moving_slider = true;
			Settings::Get()->setFloat(SETTINGS::FLOAT::FOV, _fov_gradient);
		} else if (!INPUT::key_update(INPUT::BREAK)) {
		} else if (_selection == 5) { // video_settings
			_state = (_state == MENU::OPTIONS) ? MENU::VIDEO_SETTINGS : MENU::MAIN_VIDEO_SETTINGS;
			reset_values();
			return (video_menu());
		} else if (_selection == 9) { // resource_packs
			_state = (_state == MENU::OPTIONS) ? MENU::RESOURCE_PACKS : MENU::MAIN_RESOURCE_PACKS;
			reset_values();
			return (resource_packs_menu());
		} else if (_selection == 11) { // Done
			_state = (_state == MENU::OPTIONS) ? MENU::PAUSE : MENU::MAIN;
			reset_values();
			return ((_state == MENU::PAUSE) ? pause_menu() : main_menu());
		}
	}
	if (INPUT::key_down(INPUT::CLOSE) && INPUT::key_update(INPUT::CLOSE)) {
		_state = (_state == MENU::OPTIONS) ? MENU::PAUSE : MENU::MAIN;
		reset_values();
		return ((_state == MENU::PAUSE) ? pause_menu() : main_menu());
	}

	setup_array_buffer_options();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 110 * _gui_size, 200 * _gui_size, 20 * _gui_size, (_gui_size + 1) * 7, false, "Options");
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, std::string("FOV: ") + ((static_cast<int>(_fov_gradient) == 70) ? "Normal" : std::to_string(static_cast<int>(_fov_gradient))));
	_text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Realms Notifications");
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Skin Customization");
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Video Settings...");
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Language");
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Resource Packs...");
	_text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Music & Sounds...");
    _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Controls...");
    _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Chat Settings");
    _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Accessibility Settings");
    _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Done");
	return (_moving_slider ? MENU::RET::FOV_UPDATE : MENU::RET::NO_CHANGE);
}

MENU::RET Menu::video_menu( void )
{
	_moving_slider = false;
	if (INPUT::key_down(INPUT::BREAK)) {
		if (_selection == 1) { // Render dist
			_moving_slider = true;
			Settings::Get()->setInt(SETTINGS::INT::RENDER_DIST, static_cast<int>(_render_gradient));
		} else if (_selection == 7) { // Brightness
			_moving_slider = true;
			Settings::Get()->setFloat(SETTINGS::FLOAT::BRIGHTNESS, _brightness_gradient);
		} else if (!INPUT::key_update(INPUT::BREAK)) {
		} else if (_selection == 3) { // Clouds
			Settings::Get()->setInt(SETTINGS::INT::CLOUDS, (Settings::Get()->getInt(SETTINGS::INT::CLOUDS) + 1) % (SETTINGS::OFF + 1));
		} else if (_selection == 4) { // Skybox
			Settings::Get()->setBool(SETTINGS::BOOL::SKYBOX, !Settings::Get()->getBool(SETTINGS::BOOL::SKYBOX));
		} else if (_selection == 5) { // Gui scale
			changeGuiSize();
		} else if (_selection == 11) { // Done
			_state = (_state == MENU::VIDEO_SETTINGS) ? MENU::OPTIONS : MENU::MAIN_OPTIONS;
			reset_values();
			return (options_menu());
		}
	}
	if (INPUT::key_down(INPUT::CLOSE) && INPUT::key_update(INPUT::CLOSE)) {
		_state = (_state == MENU::VIDEO_SETTINGS) ? MENU::OPTIONS : MENU::MAIN_OPTIONS;
		reset_values();
		return (options_menu());
	}

	setup_array_buffer_video();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 110 * _gui_size, 200 * _gui_size, 20 * _gui_size, (_gui_size + 1) * 7, false, "Video");
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Render dist " + std::to_string(static_cast<int>(_render_gradient)));
	_text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Resolution");
	const std::array<std::string, 3> clouds = {"Clouds - Fancy", "Clouds - Fast", "Clouds - OFF"};
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, clouds[Settings::Get()->getInt(SETTINGS::INT::CLOUDS)]);
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Gui scale " + std::to_string(_gui_size));
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Brightness " + std::to_string(static_cast<int>(_brightness_gradient * 100)));
	_text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, std::string("Skybox ") + ((Settings::Get()->getBool(SETTINGS::BOOL::SKYBOX)) ? "ON" : "OFF"));
    _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Done");
	return ((_moving_slider && _state == MENU::VIDEO_SETTINGS)
		? ((_selection == 1) ? MENU::RET::RENDER_DIST_UPDATE : MENU::RET::BRIGHTNESS_UPDATE)
		: MENU::RET::NO_CHANGE);
}

MENU::RET Menu::resource_packs_menu( void )
{
	if (INPUT::key_down(INPUT::BREAK)) {
		if (!INPUT::key_update(INPUT::BREAK)) {
		} else if (_selection == 11) { // Done
			_state = (_state == MENU::RESOURCE_PACKS) ? MENU::OPTIONS : MENU::MAIN_OPTIONS;
			reset_values();
			return (options_menu());
		}
	}
	if (INPUT::key_down(INPUT::CLOSE) && INPUT::key_update(INPUT::CLOSE)) {
		_state = (_state == MENU::RESOURCE_PACKS) ? MENU::OPTIONS : MENU::MAIN_OPTIONS;
		reset_values();
		return (options_menu());
	}

	setup_array_buffer_resource_packs();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 110 * _gui_size, 200 * _gui_size, 20 * _gui_size, (_gui_size + 1) * 7, false, "Select Resource Packs");
    _text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Apply Changes");
    _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, "Done");
	return (MENU::RET::NO_CHANGE);
}

MENU::RET Menu::ingame_inputs( void )
{
	int craft = _state + 1 - MENU::INVENTORY; // craft = 1: inventory, 2: crafting, 3: furnace
	if (INPUT::key_down(INPUT::CLOSE) && INPUT::key_update(INPUT::CLOSE)) {
		reset_values();
		return (MENU::RET::BACK_TO_GAME);
	}
	if (INPUT::key_down(INPUT::INVENTORY) && INPUT::key_update(INPUT::INVENTORY)) {
		reset_values();
		return (MENU::RET::BACK_TO_GAME);
	}
	if (INPUT::key_down(INPUT::BREAK) && INPUT::key_update(INPUT::BREAK)) {
		if (_selection) {
			if (_selected_block.type == blocks::AIR) {
				_selected_block = _inventory.pickBlockAt(craft, _selection - 1, _furnace, _chest);
			} else {
				_selected_block = _inventory.putBlockAt(craft, _selection - 1, _selected_block, _furnace, _chest);
				_ui->addFace({0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, false, true); // TODO better way to update ui than this
			}
		}
	}
	if (INPUT::key_down(INPUT::USE)) {
		bool first_frame = INPUT::key_update(INPUT::USE);
		if (_selection) {
			if (_selected_block.type == blocks::AIR) {
				if (first_frame) {
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
	} else if (INPUT::key_update(INPUT::USE)) {
		_selection_list.clear();
	}
	if (INPUT::key_down(INPUT::SLOT_0) && INPUT::key_update(INPUT::SLOT_0)) {
		_inventory.swapCells(0, _selection - 1);
	}
	if (INPUT::key_down(INPUT::SLOT_1) && INPUT::key_update(INPUT::SLOT_1)) {
		_inventory.swapCells(1, _selection - 1);
	}
	if (INPUT::key_down(INPUT::SLOT_2) && INPUT::key_update(INPUT::SLOT_2)) {
		_inventory.swapCells(2, _selection - 1);
	}
	if (INPUT::key_down(INPUT::SLOT_3) && INPUT::key_update(INPUT::SLOT_3)) {
		_inventory.swapCells(3, _selection - 1);
	}
	if (INPUT::key_down(INPUT::SLOT_4) && INPUT::key_update(INPUT::SLOT_4)) {
		_inventory.swapCells(4, _selection - 1);
	}
	if (INPUT::key_down(INPUT::SLOT_5) && INPUT::key_update(INPUT::SLOT_5)) {
		_inventory.swapCells(5, _selection - 1);
	}
	if (INPUT::key_down(INPUT::SLOT_6) && INPUT::key_update(INPUT::SLOT_6)) {
		_inventory.swapCells(6, _selection - 1);
	}
	if (INPUT::key_down(INPUT::SLOT_7) && INPUT::key_update(INPUT::SLOT_7)) {
		_inventory.swapCells(7, _selection - 1);
	}
	if (INPUT::key_down(INPUT::SLOT_8) && INPUT::key_update(INPUT::SLOT_8)) {
		_inventory.swapCells(8, _selection - 1);
	}
	return (MENU::RET::NO_CHANGE);
}

MENU::RET Menu::ingame_menu( void )
{
	MENU::RET ret = ingame_inputs();
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
	return (MENU::RET::NO_CHANGE);
}

MENU::RET Menu::chat_menu( bool animUpdate )
{
	if (INPUT::key_down(INPUT::CLOSE) && INPUT::key_update(INPUT::CLOSE)) {
		glfwSetCharCallback(_window, NULL);
		INPUT::resetMessage();
		_chat->resetHistoCursor();
		return (MENU::RET::BACK_TO_GAME);
	}
	if (INPUT::key_down(INPUT::ENTER) && INPUT::key_update(INPUT::ENTER)) {
		_chat->sendMessage(INPUT::getCurrentMessage());
		INPUT::resetMessage();
	}
	if (INPUT::key_down(INPUT::DELETE) && INPUT::key_update(INPUT::DELETE)) {
		INPUT::rmLetter();
	}
	if (INPUT::key_down(INPUT::LOOK_UP) && INPUT::key_update(INPUT::LOOK_UP)) {
		INPUT::setCurrentMessage(_chat->getHistoMsg(true));
	}
	if (INPUT::key_down(INPUT::LOOK_DOWN) && INPUT::key_update(INPUT::LOOK_DOWN)) {
		INPUT::setCurrentMessage(_chat->getHistoMsg(false));
	}
	if (INPUT::key_down(INPUT::LOOK_RIGHT) && INPUT::key_update(INPUT::LOOK_RIGHT)) {
		INPUT::moveCursor(true, INPUT::key_down(INPUT::RUN));
	}
	if (INPUT::key_down(INPUT::LOOK_LEFT) && INPUT::key_update(INPUT::LOOK_LEFT)) {
		INPUT::moveCursor(false, INPUT::key_down(INPUT::RUN));
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
	return (MENU::RET::NO_CHANGE);
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
    _vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size + static_cast<int>(gradient(_fov_gradient, 50, 110, 0, 190)) * _gui_size, WIN_HEIGHT / 2 - 84 * _gui_size, 10 * _gui_size, 18 * _gui_size, 0, (_selection == 1) ? 111 : 91, 200, 20}); // FOV Slider
    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Realms Notifications

    _vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Skin Customization...
    _vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 5) ? 111 : 91, 200, 20}); // Video Settings...
    _vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Language...
    _vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 9) ? 111 : 91, 200, 20}); // Resource Packs...

    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Music & Sounds...
    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Controls...
    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Chat Settings...
    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Accessibility Settings...

    _vertices.push_back({1, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 11) ? 111 : 91, 200, 20}); // Done

	setup_shader();
}

void Menu::setup_array_buffer_video( void )
{
	_vertices.push_back({1, 0, 0, WIN_WIDTH, WIN_HEIGHT, 3, 29, 1, 1}); // occult window
    _vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Render dist
    _vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size + static_cast<int>(gradient(_render_gradient, 8, 32, 0, 190)) * _gui_size, WIN_HEIGHT / 2 - 84 * _gui_size, 10 * _gui_size, 18 * _gui_size, 0, (_selection == 1) ? 111 : 91, 200, 20}); // render Slider
    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // Resolution

    _vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 3) ? 111 : 91, 200, 20}); // Clouds
    _vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 5) ? 111 : 91, 200, 20}); // Gui scale
    _vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // brightness
    _vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size + static_cast<int>(gradient(_brightness_gradient, 0, 1, 0, 190)) * _gui_size, WIN_HEIGHT / 2 + 6 * _gui_size, 10 * _gui_size, 18 * _gui_size, 0, (_selection == 7) ? 111 : 91, 200, 20}); // brightness Slider
    _vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // tbd

    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 4) ? 111 : 91, 200, 20}); // Skybox
    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // tbd
    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // tbd
    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20}); // tbd

    _vertices.push_back({1, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 11) ? 111 : 91, 200, 20}); // Done

	setup_shader();
}

void Menu::setup_array_buffer_resource_packs( void )
{
	_vertices.push_back({1, 0, 0, WIN_WIDTH, WIN_HEIGHT, 3, 29, 1, 1}); // occult window

	_vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 135 * _gui_size, 3, 29, 1, 1}); // occult available section
	_vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 135 * _gui_size, 3, 29, 1, 1}); // occult selected section

    _vertices.push_back({1, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (false) ? (_selection == 10) ? 111 : 91 : 71, 200, 20}); // Apply
    _vertices.push_back({1, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 11) ? 111 : 91, 200, 20}); // Done

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
		// texX texY receive 2 to indicate we wan't item's texture
		int spec = s_blocks[type]->texX(face_dir::MINUSX, 2) + (s_blocks[type]->texY(face_dir::MINUSX, 2) << 4) + (3 << 19);
		// int faceLight = computeLight(row - 1, col, level);
		int cornerLight = 15;//computeSmoothLight(faceLight, row - 1, col, level, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		int shade = 0;//computeShade(row - 1, col, level, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		// spec += (faceLight << 24);
		glm::ivec3 v0 = {spec + (cornerLight << 24) + (shade << 22), x, y};
		// cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		// shade = computeShade(row - 1, col, level, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		glm::ivec3 v1 = {spec + (cornerLight << 24) + (shade << 22) + 1, x + 16 * _gui_size, y};
		// cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		// shade = computeShade(row - 1, col, level, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		glm::ivec3 v2 = {spec + (cornerLight << 24) + (shade << 22) + (1 << 4), x, y + 16 * _gui_size};
		// cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		// shade = computeShade(row - 1, col, level, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		glm::ivec3 v3 = {spec + (cornerLight << 24) + (shade << 22) + 1 + (1 << 4), x + 16 * _gui_size, y + 16 * _gui_size};
		_ui->addFace(v0, v1, v2, v3, true, movement);
		return ;
	}
	x += _gui_size;
	int offset = (type == blocks::OAK_LOG) ? AXIS::Z : face_dir::PLUSX;
	if (type == blocks::OAK_STAIRS) {
		int spec = (15 << 24) + s_blocks[type]->texX(face_dir::PLUSZ, offset) + (s_blocks[type]->texY(face_dir::PLUSZ, offset) << 4);
		// top of second step
		glm::ivec3 v0 = {spec, x, y + 15 * _gui_size * 81.25f / 362.5f};
		glm::ivec3 v1 = {spec + 1, x + 6.5f * _gui_size, y};
		glm::ivec3 v2 = {spec + (1 << 4), x + 3.25f * _gui_size, y + 15 * _gui_size * 121.875f / 362.5f};
		glm::ivec3 v3 = {spec + 1 + (1 << 4), x + 9.75f * _gui_size, y + 15 * _gui_size * 40.625f / 362.5f};
		_ui->addFace(v0, v1, v2, v3, true, movement);
		// top of first step
		v0 = {spec, x + 3.25f * _gui_size, y + 15 * _gui_size * 221.875f / 362.5f};
		v1 = {spec + 1, x + 9.75f * _gui_size, y + 15 * _gui_size * 140.624f / 362.5f};
		v2 = {spec + (1 << 4), x + 6.5f * _gui_size, y + 15 * _gui_size * 262.5f / 362.5f};
		v3 = {spec + 1 + (1 << 4), x + 13 * _gui_size, y + 15 * _gui_size * 181.25f / 362.5f};
		_ui->addFace(v0, v1, v2, v3, true, movement);
		// front of second step
		spec -= (8 << 24);
		v0 = {spec, x + 3.25f * _gui_size, y + 15 * _gui_size * 121.875f / 362.5f};
		v1 = {spec + 1, x + 9.75f * _gui_size, y + 15 * _gui_size * 40.625f / 362.5f};
		v2 = {spec + (1 << 4), x + 3.25f * _gui_size, y + 15 * _gui_size * 221.875f / 362.5f};
		v3 = {spec + 1 + (1 << 4), x + 9.75f * _gui_size, y + 15 * _gui_size * 140.624f / 362.5f};
		_ui->addFace(v0, v1, v2, v3, true, movement);
		// front of first step
		v0 = {spec, x + 6.5f * _gui_size, y + 15 * _gui_size * 262.5f / 362.5f};
		v1 = {spec + 1, x + 13 * _gui_size, y + 15 * _gui_size * 181.25f / 362.5f};
		v2 = {spec + (1 << 4), x + 6.5f * _gui_size, y + 15 * _gui_size};
		v3 = {spec + 1 + (1 << 4), x + 13 * _gui_size, y + 15 * _gui_size * 281.25f / 362.5f};
		_ui->addFace(v0, v1, v2, v3, true, movement);
		// left face
		spec = (10 << 24) + s_blocks[type]->texX(face_dir::MINUSY, offset) + (s_blocks[type]->texY(face_dir::MINUSY, offset) << 4);
		v0 = {spec, x, y + 15 * _gui_size * 81.25f / 362.5f};
		v1 = {spec + 1, x + 6.5f * _gui_size, y + 15 * _gui_size * 162.5f / 362.5f};
		v2 = {spec + (1 << 4), x, y + 15 * _gui_size * 281.25f / 362.5f};
		v3 = {spec + 1 + (1 << 4), x + 6.5f * _gui_size, y + 15 * _gui_size};
		_ui->addFace(v0, v1, v2, v3, true, movement);
		return ;
	}
	int yTop = (type == blocks::OAK_SLAB) ? y + 16 * _gui_size * 100.0f / 362.5f
											: (type == blocks::OAK_TRAPDOOR) ? y + 16 * _gui_size * 162.5f / 362.5f : y;
	int slabOffset = (type == blocks::OAK_SLAB) ? (8 << 8)
											: (type == blocks::OAK_TRAPDOOR) ? (13 << 8) : 0;
	// top face
	int spec = (15 << 24) + s_blocks[type]->texX(face_dir::PLUSZ, offset) + (s_blocks[type]->texY(face_dir::PLUSZ, offset) << 4);
	glm::ivec3 v0 = {spec, x, yTop + 16 * _gui_size * 81.25f / 362.5f};
	glm::ivec3 v1 = {spec + 1, x + 7 * _gui_size, yTop};
	glm::ivec3 v2 = {spec + (1 << 4), x + 7 * _gui_size, yTop + 16 * _gui_size * 162.5f / 362.5f};
	glm::ivec3 v3 = {spec + 1 + (1 << 4), x + 14 * _gui_size, yTop + 16 * _gui_size * 81.25f / 362.5f};
	_ui->addFace(v0, v1, v2, v3, true, movement);
	// left face
	spec = (10 << 24) + s_blocks[type]->texX(face_dir::MINUSY, offset) + (s_blocks[type]->texY(face_dir::MINUSY, offset) << 4);
	v0 = {spec + slabOffset, x, yTop + 16 * _gui_size * 81.25f / 362.5f};
	v1 = {spec + slabOffset + 1, x + 7 * _gui_size, yTop + 16 * _gui_size * 162.5f / 362.5f};
	v2 = {spec + (1 << 4), x, y + 16 * _gui_size * 281.25f / 362.5f};
	v3 = {spec + 1 + (1 << 4), x + 7 * _gui_size, y + 16 * _gui_size};
	_ui->addFace(v0, v1, v2, v3, true, movement);
	// right face
	spec = (7 << 24) + s_blocks[type]->texX(face_dir::PLUSX, offset) + (s_blocks[type]->texY(face_dir::PLUSX, offset) << 4);
	v0 = {spec + slabOffset, x + 7 * _gui_size, yTop + 16 * _gui_size * 162.5f / 362.5f};
	v1 = {spec + slabOffset + 1, x + 14 * _gui_size, yTop + 16 * _gui_size * 81.25f / 362.5f};
	v2 = {spec + (1 << 4), x + 7 * _gui_size, y + 16 * _gui_size};
	v3 = {spec + 1 + (1 << 4), x + 14 * _gui_size, y + 16 * _gui_size * 281.25f / 362.5f};
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
	switch (_state) {
	case MENU::MAIN:
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 80 * _gui_size, 95 * _gui_size, 20 * _gui_size)) {
			_selection = 5;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 80 * _gui_size, 95 * _gui_size, 20 * _gui_size)) {
			_selection = 6;
		} else {
			_selection = 0;
		}
		break ;
	case MENU::WORLD_SELECT:
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
		break ;
	case MENU::DEATH:
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 5 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 2;
		} else {
			_selection = 0;
		}
		break ;
	case MENU::PAUSE:
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 60 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size, 95 * _gui_size, 20 * _gui_size)) {
			_selection = 6;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 40 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 8;
		} else {
			_selection = 0;
		}
		break ;
	case MENU::OPTIONS:
	case MENU::MAIN_OPTIONS:
		if (_moving_slider) {
			_fov_gradient = gradient(posX, WIN_WIDTH / 2 - 200 * _gui_size, WIN_WIDTH / 2 - 10 * _gui_size, 50, 110);
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 5;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 9;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 11;
		} else {
			_selection = 0;
		}
		break ;
	case MENU::MAIN_VIDEO_SETTINGS:
	case MENU::VIDEO_SETTINGS:
		if (_moving_slider) {
			if (_selection == 1) {
				_render_gradient = gradient(posX, WIN_WIDTH / 2 - 200 * _gui_size, WIN_WIDTH / 2 - 10 * _gui_size, 8, 32);
			} else if (_selection == 7) {
				_brightness_gradient = gradient(posX, WIN_WIDTH / 2 - 200 * _gui_size, WIN_WIDTH / 2 - 10 * _gui_size, 0, 1);
			}
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 3;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 4;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 5;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 7;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 11;
		} else {
			_selection = 0;
		}
		break ;
	case MENU::MAIN_RESOURCE_PACKS:
	case MENU::RESOURCE_PACKS:
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 10;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 11;
		} else {
			_selection = 0;
		}
		break ;
	default:
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
	} else if (state == MENU::COMMAND) {
		_state = MENU::CHAT;
		glfwSetCharCallback(_window, INPUT::character_callback);
		INPUT::setCurrentMessage("/");
	}
}

int Menu::getState( void )
{
	return (_state);
}

void Menu::changeGuiSize( void )
{
	if (++_gui_size > GUI_MAX) {
		_gui_size = GUI_MIN;
	}
	_ui->setGuiSize(_gui_size + 1);
}

std::string Menu::getWorldFile( void )
{
	return (_world_file);
}

MENU::RET Menu::run( bool animUpdate )
{
	if (_state != MENU::CHAT && INPUT::key_down(INPUT::QUIT_PROGRAM)) {
		glfwSetWindowShouldClose(_window, GL_TRUE);
		return (MENU::RET::QUIT);
	}

	switch (_state) {
		case MENU::MAIN:
			return (main_menu());
		case MENU::WORLD_SELECT:
			return (world_select_menu());
		case MENU::LOAD:
			return (loading_screen());
		case MENU::DEATH:
			return (death_menu());
		case MENU::PAUSE:
			return (pause_menu());
		case MENU::MAIN_OPTIONS:
		case MENU::OPTIONS:
			return (options_menu());
		case MENU::MAIN_VIDEO_SETTINGS:
		case MENU::VIDEO_SETTINGS:
			return (video_menu());
		case MENU::MAIN_RESOURCE_PACKS:
		case MENU::RESOURCE_PACKS:
			return (resource_packs_menu());
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
			return (MENU::RET::BACK_TO_GAME);
	}
	return (MENU::RET::NO_CHANGE);
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
