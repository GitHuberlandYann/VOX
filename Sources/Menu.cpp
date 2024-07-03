#include "Menu.hpp"
#include "Ui.hpp"
#include "Inventory.hpp"
#include "callbacks.hpp"
#include "Settings.hpp"
#include <dirent.h>
#include "logs.hpp"
extern std::mutex mtx;
extern siv::PerlinNoise::seed_type perlin_seed;

Menu::Menu( void ) : _gui_size(3), _state(menu::main), _selection(0), _selected_world(0), _vaoSet(false),
	_textBar(true), _input_world(false), _input_seed(false), _change_to_apply(false), _book_content(NULL),
	_inventory(NULL), _ui(NULL), _chat(NULL), _text(NULL), _chest(NULL), _furnace(NULL)
{
	_world_file = "";
}

Menu::~Menu( void )
{
	MAINLOG(LOG("Destructor of Menu called"));
	_worlds.clear();
	_selection_list.clear();
}

void Menu::deleteBuffers( void )
{
	_vabo.deleteBuffers();
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void Menu::blit_to_screen( void )
{
	_ui->textToScreen(_state >= menu::pause);
	_ui->useShader();
	_vabo.bindVertexArray();
	glDrawArrays(GL_TRIANGLES, 0, _nb_points);
}

void Menu::reset_values( void )
{
	_selection = 0;
	_selected_block = {0};
	if (_chest) {
		_chest->setState(chest_state::CLOSING);
		_chest = NULL;
	}
	_furnace = NULL;
	_selected_world = 0;
	_worlds.clear();
	_resource_packs.clear();
	_selection_list.clear();
	if (_input_world || _input_seed) {
		glfwSetCharCallback(_window, NULL);
		_input_world = false;
		_input_seed = false;
	}
	_moving_slider = false;
	_change_to_apply = false;
	_drop_down_menu = false;
	_scroll = 0;
	if (_book_content) {
		while (_book_content->size() > 1 && _book_content->at(_book_content->size() - 1) == "") {
			_book_content->pop_back();
		}
		_book_content = NULL;
	}
}

menu::ret Menu::main_menu( void )
{
	if (inputs::key_down(inputs::left_click) && inputs::key_update(inputs::left_click)) {
		if (_selection == 1) { //singleplayer
			return (enter_world_select_menu());
		} else if (_selection == 5) { // options
			_state = menu::main_options;
			reset_values();
			return (menu::ret::no_change);
		} else if (_selection == 6) { //quit game
			glfwSetWindowShouldClose(_window, GL_TRUE);
			saveSettings();
			return (menu::ret::quit);
		}
	}

   _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size, 200 * _gui_size, 20 * _gui_size, 8 * _gui_size, true, settings::consts::depth::menu::str, "Singleplayer");
   _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size, 200 * _gui_size, 20 * _gui_size, 8 * _gui_size, true, settings::consts::depth::menu::str, "Multiplayer");
   _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 40 * _gui_size, 200 * _gui_size, 20 * _gui_size, 8 * _gui_size, true, settings::consts::depth::menu::str, "Minecraft Realms");
   _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 80 * _gui_size, 95 * _gui_size, 20 * _gui_size, 8 * _gui_size, true, settings::consts::depth::menu::str, "Options...");
   _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 80 * _gui_size, 95 * _gui_size, 20 * _gui_size, 8 * _gui_size, true, settings::consts::depth::menu::str, "Quit Game");

	setup_array_buffer_main();
	blit_to_screen();
	return (menu::ret::no_change);
}

menu::ret Menu::enter_world_select_menu( void )
{
	_state = menu::world_select;
	reset_values();			
	DIR *dir = opendir("Worlds");
	if (dir == NULL) {
		LOGERROR("failed to open dir Worlds/");
		return (menu::ret::no_change);
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
	return (world_select_menu());
}

menu::ret Menu::world_select_menu( void )
{
	if (inputs::key_down(inputs::close) && inputs::key_update(inputs::close)) {
		_state = menu::main;
		reset_values();
		return (main_menu());
	}
	if (inputs::key_down(inputs::left_click) && inputs::key_update(inputs::left_click)) {
		if (_selection == 1) { // play selected world
			_world_file = _worlds[_selected_world - 1];
			_state = menu::load;
			reset_values();
			return (menu::ret::world_selected);
		} else if (_selection == 4) { // create new world
			_state = menu::world_create;
			perlin_seed = 0;
			reset_values();
			_world_file = "";
			return (world_create_menu(false));
		} else if (_selection == 6) { // cancel, go back to main menu
			_state = menu::main;
			reset_values();
			return (main_menu());
		} else if (_selection > 6) {
			_selected_world = _selection - 6;
		}
	}

	_text->addText(WIN_WIDTH / 2 - 120, 30, 24, TEXT::WHITE, settings::consts::depth::menu::str, "Select World");

	_text->addCenteredText(WIN_WIDTH / 2 - 155 * _gui_size, WIN_HEIGHT / 2 + 90 * _gui_size, 150 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Play Selected World");
	_text->addCenteredText(WIN_WIDTH / 2 - 155 * _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size, 73 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Edit");
	_text->addCenteredText(WIN_WIDTH / 2 - 78 * _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size, 73 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Delete");
	_text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 90 * _gui_size, 150 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Create New World");
	_text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size, 73 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Re-Create");
	_text->addCenteredText(WIN_WIDTH / 2 + 82 * _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size, 73 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Cancel");

	for (int index = 0; index < static_cast<int>(static_cast<int>(_worlds.size())) && index < 8; index++) {
		std::string str = _worlds[index].substr(0, _worlds[index].size() - 5);
		_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, (30 + 20 * index) * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, str);
	}

	setup_array_buffer_select();
	blit_to_screen();
	return (menu::ret::no_change);
}

/**
 * @brief Menu used for world creation, used to input world name, world seed, world type (flat or not)
 * @return status used by OpengGL_Manager - NO_CHANGE or WORLD_SELECTED
*/
menu::ret Menu::world_create_menu( bool animUpdate )
{
	if (inputs::key_down(inputs::close) && inputs::key_update(inputs::close)) {
		return (enter_world_select_menu());
	}
	if (inputs::key_down(inputs::left_click) && inputs::key_update(inputs::left_click)) {
		if ((_selection != 1 && _input_world) || (_selection != 2 && _input_seed)) {
			_input_world = false;
			_input_seed = false;
			glfwSetCharCallback(_window, NULL);
		}
		if (_selection == 1 && !_input_world) { // change name
			glfwSetCharCallback(_window, inputs::character_callback);
			inputs::setCurrentMessage(_world_file);
			_input_world = true;
		} else if (_selection == 2 && !_input_seed) { // change seed
			glfwSetCharCallback(_window, inputs::character_callback);
			inputs::setCurrentMessage((perlin_seed) ? std::to_string(perlin_seed) : "");
			_input_seed = true;
		} else if (_selection == 3) { // toggle world type
			Settings::Get()->setBool(settings::bools::flat_world, !Settings::Get()->getBool(settings::bools::flat_world));
		} else if (_selection == 4) { // toggle flat world block
			int current = Settings::Get()->getInt(settings::ints::flat_world_block);
			int i = 0;
			for (; i < 5; ++i) {
				if (current == settings::fw_types[i]) {
					break ;
				}
			}
			i = (i + 1) % 5;
			Settings::Get()->setInt(settings::ints::flat_world_block, settings::fw_types[i]);
		} else if (_selection == 5) { // confirm, go to load menu
			if (!perlin_seed) {
				const auto p1 = std::chrono::system_clock::now(); // works without #include <chrono> #include <ctime> ?
				perlin_seed = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();
			}
			_world_file += ".json";
			_state = menu::load;
			reset_values();
			return (menu::ret::world_created);
		} else if (_selection == 6) { // cancel, go back to wold select menu
			return (enter_world_select_menu());
		}
	}
	if (inputs::key_down(inputs::del) && inputs::key_update(inputs::del)) {
		inputs::rmLetter();
	}
	if (inputs::key_down(inputs::look_right) && inputs::key_update(inputs::look_right)) {
		inputs::moveCursor(true, inputs::key_down(inputs::run));
	}
	if (inputs::key_down(inputs::look_left) && inputs::key_update(inputs::look_left)) {
		inputs::moveCursor(false, inputs::key_down(inputs::run));
	}

	_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 110 * _gui_size, 200 * _gui_size, 20 * _gui_size, (_gui_size + 1) * 7, false, settings::consts::depth::menu::str, "Create New World");

	_text->addText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 77 * _gui_size, 6 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "World Name");
	if (_input_world) {
		_world_file = inputs::getCurrentMessage();
		if (animUpdate) { _textBar = !_textBar; }
		_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, inputs::getCurrentInputStr((_textBar) ? '|' : '.')); // World Name
	} else {
		_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, _world_file); // World Name
	}
	_text->addText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 32 * _gui_size, 6 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "World Seed (leave empty for random seed)");
	if (_input_seed) {
		std::string input = inputs::getCurrentMessage();
		for (int i = 0; input[i]; ++i) {
			if (!std::isdigit(input[i])) {
				input = input.substr(0, i);
				inputs::setCurrentMessage(input);
				break ;
			}
		}
		perlin_seed = std::atoi(input.c_str());
		if (animUpdate) { _textBar = !_textBar; }
		_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, inputs::getCurrentInputStr((_textBar) ? '|' : '.')); // Seed
	} else {
		_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (perlin_seed) ? std::to_string(perlin_seed) : ""); // Seed
	}
    bool flat = Settings::Get()->getBool(settings::bools::flat_world);
	_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (flat) ? "Flat World" : "Normal World"); // World Type
	if (flat) {
		_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, s_blocks[Settings::Get()->getInt(settings::ints::flat_world_block)]->name); // Flat World Block
	}

	_text->addCenteredText(WIN_WIDTH / 2 - 155 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 150 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Create New World");
	_text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 150 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Cancel");

	setup_array_buffer_create();
	blit_to_screen();
	return (menu::ret::no_change);
}

menu::ret Menu::loading_screen_menu( void )
{
	GLint current_size = 0, newVaoCounter = 0;
	for (auto& c : _chunks) {
		mtx.lock();
		current_size += c->isLoaded(newVaoCounter);
		mtx.unlock();
	}

	int render_dist = Settings::Get()->getInt(settings::ints::render_dist);
	GLint goal = (1 + 2 * render_dist) * (1 + 2 * render_dist);
	// std::cout << "CURRENT IS " << current_size << ", GOAL IS " << goal << std::endl;
	if (current_size >= goal) {
		_state = menu::pause;
		return (menu::ret::back_to_game);
	}
	float percentage = current_size / static_cast<float>(goal) * 100;

	std::string str = "LOADING.. " + std::to_string(percentage).substr(0, 5) + '%' + " (" + std::to_string(current_size) + '/' + std::to_string(goal) + ')';
	_text->addText(WIN_WIDTH / 2 - 24 * str.length() / 2, WIN_HEIGHT / 2 - 150, 24, TEXT::WHITE, settings::consts::depth::menu::str, str);

	setup_array_buffer_load(percentage * 10);
	blit_to_screen();
	return (menu::ret::no_change);
}

menu::ret Menu::death_menu( void )
{
	if (inputs::key_down(inputs::left_click) && inputs::key_update(inputs::left_click)) {
		if (_selection == 1) { // Respawn
			_state = menu::load;
			reset_values();
			return (menu::ret::respawn_player);
		} else if (_selection == 2) { // Save and Quit to Title
			_state = menu::main;
			reset_values();
			return (menu::ret::respawn_save_quit);
		}
	}

	_text->addCenteredText(0, WIN_HEIGHT / 2 - 60 * _gui_size, WIN_WIDTH, 20 * _gui_size, 14 * _gui_size, true, settings::consts::depth::menu::str, "You Died!");
    _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Respawn");
    _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Title Screen");

	setup_array_buffer_death();
	blit_to_screen();
	return (menu::ret::no_change);
}

menu::ret Menu::pause_menu( void )
{
	if (inputs::key_down(inputs::left_click) && inputs::key_update(inputs::left_click)) {
		if (_selection == 1) { //Back to Game
			reset_values();
			return (menu::ret::back_to_game);
		} else if (_selection == 6) { // Options...
			_state = menu::options;
			reset_values();
			return (options_menu());
		} else if (_selection == 8) { //Save and Quit to Title
			_state = menu::main;
			reset_values();
			return (menu::ret::save_and_quit);
		}
	}
	if (inputs::key_down(inputs::close) && inputs::key_update(inputs::close)) {
		return (menu::ret::back_to_game);
	}

	_text->addText(WIN_WIDTH / 2 - 25 * (_gui_size + 1), WIN_HEIGHT / 2 - 60 * _gui_size - 40, (_gui_size + 1) * 7, TEXT::WHITE, settings::consts::depth::menu::str, "Game Menu");
	_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 60 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Back to Game");
    _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 35 * _gui_size, 95 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Advancements");
    _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size, 95 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Give Feedback");
    _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size, 95 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Options...");
    _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 35 * _gui_size, 95 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Statistics");
    _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size, 95 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Report Bugs");
    _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size, 95 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Open to LAN");
    _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 40 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Save and Quit to Title");

	setup_array_buffer_pause();
	blit_to_screen();
	return (menu::ret::no_change);
}

menu::ret Menu::options_menu( void )
{
	_moving_slider = false;
	if (inputs::key_down(inputs::left_click)) {
		if (_selection == 1) { // FOV
			_moving_slider = true;
			Settings::Get()->setFloat(settings::floats::fov, _fov_gradient);
		} else if (!inputs::key_update(inputs::left_click)) {
		} else if (_selection == 3) { // skin_customization
			_state = (_state == menu::options) ? menu::skin_customization : menu::main_skin_customization;
			reset_values();
			return (skin_customization_menu());
		} else if (_selection == 5) { // video_settings
			_state = (_state == menu::options) ? menu::video_settings : menu::main_video_settings;
			reset_values();
			return (video_menu());
		} else if (_selection == 6) { // controls
			_state = (_state == menu::options) ? menu::controls : menu::main_controls;
			reset_values();
			return (controls_menu());
		} else if (_selection == 9) { // resource_packs
			_state = (_state == menu::options) ? menu::resource_packs : menu::main_resource_packs;
			reset_values();
			_active_resource_packs = Settings::Get()->getResourcePacks();
			DIR *dir = opendir("Resources/Resource_Packs");
			if (dir == NULL) {
				LOGERROR("failed to open dir Resource/Resource_Packs");
				_state = (_state == menu::resource_packs) ? menu::options : menu::main_options;
				return (menu::ret::no_change);
			}
			struct dirent *dent;
			while ((dent = readdir(dir)) != NULL)
			{
				std::string file = dent->d_name;
				if (file != "." && file != ".." && file.size() >= 6 && !file.compare(file.size() - 5, 5, ".json")) {
					_resource_packs.push_back(file);
					for (auto &pack: _active_resource_packs) {
						if (pack == file) {
							_resource_packs.pop_back();
						}
					}
				}
			}
			closedir(dir);
			return (resource_packs_menu());
		} else if (_selection == 11) { // Done
			_state = (_state == menu::options) ? menu::pause : menu::main;
			reset_values();
			return ((_state == menu::pause) ? pause_menu() : main_menu());
		}
	}
	if (inputs::key_down(inputs::close) && inputs::key_update(inputs::close)) {
		_state = (_state == menu::options) ? menu::pause : menu::main;
		reset_values();
		return ((_state == menu::pause) ? pause_menu() : main_menu());
	}

	_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 110 * _gui_size, 200 * _gui_size, 20 * _gui_size, (_gui_size + 1) * 7, false, settings::consts::depth::menu::str, "Options");
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, std::string("FOV: ") + ((static_cast<int>(_fov_gradient) == 70) ? "Normal" : std::to_string(static_cast<int>(_fov_gradient))));
	_text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Realms Notifications");
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Skin Customization");
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Video Settings...");
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Language");
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Resource Packs...");
	_text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Music & Sounds...");
    _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Controls...");
    _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Chat Settings");
    _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Accessibility Settings");
    _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Done");

	setup_array_buffer_options();
	blit_to_screen();
	return (_moving_slider ? menu::ret::fov_update : menu::ret::no_change);
}

menu::ret Menu::skin_customization_menu( void )
{
	if (inputs::key_down(inputs::left_click)) {
		if (!inputs::key_update(inputs::left_click)) {
		} else if (_selection == 11) { // Done
			_state = (_state == menu::skin_customization) ? menu::options : menu::main_options;
			reset_values();
			return (options_menu());
		}
	}
	if (inputs::key_down(inputs::close) && inputs::key_update(inputs::close)) {
		_state = (_state == menu::skin_customization) ? menu::options : menu::main_options;
		reset_values();
		return (options_menu());
	}

	_text->addCenteredText(WIN_WIDTH / 2, 20 * _gui_size, 0, 0, (_gui_size + 1) * 7, false, settings::consts::depth::menu::controls::str, "Skin Customization");

	setup_array_buffer_skin();
	blit_to_screen();
	return (menu::ret::no_change);
}

menu::ret Menu::video_menu( void )
{
	_moving_slider = false;
	if (inputs::key_down(inputs::left_click)) {
		if (_selection == 1) { // Render dist
			_moving_slider = true;
			Settings::Get()->setInt(settings::ints::render_dist, static_cast<int>(_render_gradient));
		} else if (_selection == 7) { // Brightness
			_moving_slider = true;
			Settings::Get()->setFloat(settings::floats::brightness, _brightness_gradient);
		} else if (!inputs::key_update(inputs::left_click)) {
		} else if (_selection == 2) { // Resolution
			_drop_down_menu = !_drop_down_menu;
		} else if (_selection == 3) { // Clouds
			Settings::Get()->setInt(settings::ints::clouds, (Settings::Get()->getInt(settings::ints::clouds) + 1) % (settings::OFF + 1));
		} else if (_selection == 4) { // FullScreen
			// Settings::Get()->setBool(settings::bools::fullscreen, !Settings::Get()->getBool(settings::bools::fullscreen)); (Settings::Get()->getBool(settings::bools::fullscreen)) ? glfwSetWindowMonitor(_window, glfwGetPrimaryMonitor(), 0, 0, WIN_WIDTH, WIN_HEIGHT, GLFW_DONT_CARE)	: glfwSetWindowMonitor(_window, nullptr, 0, 0, WIN_WIDTH, WIN_HEIGHT, GLFW_DONT_CARE);
		} else if (_selection == 5) { // Gui scale
			changeGuiSize();
		} else if (_selection == 6) { // Skybox
			Settings::Get()->setBool(settings::bools::skybox, !Settings::Get()->getBool(settings::bools::skybox));
		} else if (_selection == 8) { // Particles
			Settings::Get()->setBool(settings::bools::particles, !Settings::Get()->getBool(settings::bools::particles));
		} else if (_selection == 9) { // Face Culling
			bool culling = !Settings::Get()->getBool(settings::bools::face_culling);
			Settings::Get()->setBool(settings::bools::face_culling, culling);
			// (culling) ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
		} else if (_selection == 10) { // Smooth Lighting
			bool smooth = !Settings::Get()->getBool(settings::bools::smooth_lighting);
			Settings::Get()->setBool(settings::bools::smooth_lighting, smooth);
		} else if (_selection == 11) { // Done
			_state = (_state == menu::video_settings) ? menu::options : menu::main_options;
			reset_values();
			return (options_menu());
		} else if (_selection >= 70) {
			WIN_WIDTH = menu::resolutions[_selection - 70][0];
			WIN_HEIGHT = menu::resolutions[_selection - 70][1];
			#if __linux__
				glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
			#endif
			glfwSetWindowSize(_window, WIN_WIDTH, WIN_HEIGHT);
			// update shaders uniforms
			_text->updateWinSize();
			_ui->updateWinSize();
			--_gui_size;
			changeGuiSize();
			_drop_down_menu = false;
		}
	}
	if (inputs::key_down(inputs::close) && inputs::key_update(inputs::close)) {
		_state = (_state == menu::video_settings) ? menu::options : menu::main_options;
		reset_values();
		return (options_menu());
	}

	_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 110 * _gui_size, 200 * _gui_size, 20 * _gui_size, (_gui_size + 1) * 7, false, settings::consts::depth::menu::str, "Video");
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Render dist " + std::to_string(static_cast<int>(_render_gradient)));
	_text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Resolution: " + std::to_string(WIN_WIDTH) + "x" + std::to_string(WIN_HEIGHT));
	const std::array<std::string, 3> clouds = {"Clouds - Fancy", "Clouds - Fast", "Clouds - OFF"};
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, clouds[Settings::Get()->getInt(settings::ints::clouds)]);
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Gui scale " + std::to_string(_gui_size));
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Brightness " + std::to_string(static_cast<int>(_brightness_gradient * 100)));
	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, std::string("Face Culling ") + ((Settings::Get()->getBool(settings::bools::face_culling)) ? "ON" : "OFF"));
	if (_drop_down_menu) {
		for (int index = 0; index < Settings::Get()->getInt(settings::ints::available_res); ++index) {
			_text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - (65 - 18 * index) * _gui_size, 200 * _gui_size, 18 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::controls::str, std::to_string(menu::resolutions[index][0]) + "x" + std::to_string(menu::resolutions[index][1]));
		}
	}
	_text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, std::string("FullScreen ") + ((Settings::Get()->getBool(settings::bools::fullscreen)) ? "ON" : "OFF"));
	_text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, std::string("Skybox ") + ((Settings::Get()->getBool(settings::bools::skybox)) ? "ON" : "OFF"));
	_text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, std::string("Particles ") + ((Settings::Get()->getBool(settings::bools::particles)) ? "ON" : "OFF"));
	_text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, std::string("Smooth Lighting ") + ((Settings::Get()->getBool(settings::bools::smooth_lighting)) ? "ON" : "OFF"));

    _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Done");

	setup_array_buffer_video();
	blit_to_screen();
	return ((_moving_slider && _state == menu::video_settings)
		? ((_selection == 1) ? menu::ret::render_dist_update : menu::ret::brightness_update)
		: menu::ret::no_change);
}

menu::ret Menu::controls_menu( void )
{
	const std::array<int, 27> actions = {inputs::destroy, inputs::drop, inputs::sample, inputs::run, inputs::use, -1,
				inputs::slot_0, inputs::slot_1, inputs::slot_2, inputs::slot_3, inputs::slot_4, inputs::slot_5, inputs::slot_6, inputs::slot_7, inputs::slot_8, inputs::inventory, -1,
				inputs::jump, inputs::sneak, inputs::move_left, inputs::move_right, inputs::move_backwards, inputs::move_forwards, -1,
				-1, inputs::chat, inputs::command};
	if (inputs::key_down(inputs::left_click)) {
		if (!inputs::key_update(inputs::left_click)) {
		} else if (_selected_world != _selection) {
			_selected_world = _selection;
			switch (_selection) {
				case 0:
				case 14:
				case 36:
				case 50:
				case 52:
					break ;
				case 1: // Done
					_state = (_state == menu::controls) ? menu::options : menu::main_options;
					reset_values();
					return (options_menu());
				case 2: // Reset
					if (_change_to_apply) {
						inputs::reset_key_binds();
					}
					break ;
				default:
					if (_selection & 0b1) { // key bind
						inputs::reset_last_input();
					} else { // reset
						inputs::reset_key_bind(actions[(_selection - 4) / 2]);
					}
			}
		}
	}
	if (inputs::key_down(inputs::close) && inputs::key_update(inputs::close)) {
		_state = (_state == menu::controls) ? menu::options : menu::main_options;
		reset_values();
		return (options_menu());
	}
	if (_selected_world > 2 && (_selected_world & 0b1)) {
		int key = inputs::get_last_input();
		if (key != -1) {
			inputs::set_key_bind(key, actions[(_selected_world - 3) / 2]);
			_selected_world = 0;
		}
	}

	_text->addCenteredText(WIN_WIDTH / 2, 20 * _gui_size, 0, 0, (_gui_size + 1) * 7, false, settings::consts::depth::menu::controls::str, "Controls");

	_text->addCenteredText(WIN_WIDTH / 2, (66.5f - _scroll) * _gui_size, 0, 0, 7 * _gui_size, false, settings::consts::depth::menu::str, "Gameplay");
	{
		_text->addText(WIN_WIDTH / 4, (91.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Attack/Destroy");
		_text->addCenteredText(WIN_WIDTH / 2, (85 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 3) ? inputs::get_key_name(inputs::destroy) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (85 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (116.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Drop Item");
		_text->addCenteredText(WIN_WIDTH / 2, (110 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 5) ? inputs::get_key_name(inputs::drop) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (110 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (141.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Pick Block");
		_text->addCenteredText(WIN_WIDTH / 2, (135 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 7) ? inputs::get_key_name(inputs::sample) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (135 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (166.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Sprint");
		_text->addCenteredText(WIN_WIDTH / 2, (160 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 9) ? inputs::get_key_name(inputs::run) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (160 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (191.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Use Item/Place Block");
		_text->addCenteredText(WIN_WIDTH / 2, (185 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 11) ? inputs::get_key_name(inputs::use) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (185 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
	}
	_text->addCenteredText(WIN_WIDTH / 2, (216.5f - _scroll) * _gui_size, 0, 0, 7 * _gui_size, false, settings::consts::depth::menu::str, "Inventory");
	{
		_text->addText(WIN_WIDTH / 4, (241.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Hotbar Slot 1");
		_text->addCenteredText(WIN_WIDTH / 2, (235 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 15) ? inputs::get_key_name(inputs::slot_0) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (235 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (266.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Hotbar Slot 2");
		_text->addCenteredText(WIN_WIDTH / 2, (260 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 17) ? inputs::get_key_name(inputs::slot_1) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (260 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (291.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Hotbar Slot 3");
		_text->addCenteredText(WIN_WIDTH / 2, (285 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 19) ? inputs::get_key_name(inputs::slot_2) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (285 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (316.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Hotbar Slot 4");
		_text->addCenteredText(WIN_WIDTH / 2, (310 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 21) ? inputs::get_key_name(inputs::slot_3) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (310 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (341.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Hotbar Slot 5");
		_text->addCenteredText(WIN_WIDTH / 2, (335 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 23) ? inputs::get_key_name(inputs::slot_4) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (335 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (366.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Hotbar Slot 6");
		_text->addCenteredText(WIN_WIDTH / 2, (360 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 25) ? inputs::get_key_name(inputs::slot_5) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (360 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (391.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Hotbar Slot 7");
		_text->addCenteredText(WIN_WIDTH / 2, (385 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 27) ? inputs::get_key_name(inputs::slot_6) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (385 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (416.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Hotbar Slot 8");
		_text->addCenteredText(WIN_WIDTH / 2, (410 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 29) ? inputs::get_key_name(inputs::slot_7) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (410 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (441.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Hotbar Slot 9");
		_text->addCenteredText(WIN_WIDTH / 2, (435 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 31) ? inputs::get_key_name(inputs::slot_8) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (435 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (466.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Inventory");
		_text->addCenteredText(WIN_WIDTH / 2, (460 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 33) ? inputs::get_key_name(inputs::inventory) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (460 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
	}
	_text->addCenteredText(WIN_WIDTH / 2, (491.5f - _scroll) * _gui_size, 0, 0, 7 * _gui_size, false, settings::consts::depth::menu::str, "Movement");
	{
		_text->addText(WIN_WIDTH / 4, (516.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Jump");
		_text->addCenteredText(WIN_WIDTH / 2, (510 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 37) ? inputs::get_key_name(inputs::jump) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (510 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (541.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Sneak");
		_text->addCenteredText(WIN_WIDTH / 2, (535 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 39) ? inputs::get_key_name(inputs::sneak) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (535 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (566.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Strafe Left");
		_text->addCenteredText(WIN_WIDTH / 2, (560 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 41) ? inputs::get_key_name(inputs::move_left) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (560 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (591.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Strafe Right");
		_text->addCenteredText(WIN_WIDTH / 2, (585 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 43) ? inputs::get_key_name(inputs::move_right) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (585 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (616.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Walk Backwards");
		_text->addCenteredText(WIN_WIDTH / 2, (610 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 45) ? inputs::get_key_name(inputs::move_backwards) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (610 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (641.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Walk Forwards");
		_text->addCenteredText(WIN_WIDTH / 2, (635 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 47) ? inputs::get_key_name(inputs::move_forwards) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (635 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
	}
	_text->addCenteredText(WIN_WIDTH / 2, (666.5f - _scroll) * _gui_size, 0, 0, 7 * _gui_size, false, settings::consts::depth::menu::str, "Multiplayer");
	{
		_text->addText(WIN_WIDTH / 4, (691.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "List Players");
		_text->addCenteredText(WIN_WIDTH / 2, (685 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "TAB");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (685 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (716.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Open Chat");
		_text->addCenteredText(WIN_WIDTH / 2, (710 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 53) ? inputs::get_key_name(inputs::chat) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (710 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
		_text->addText(WIN_WIDTH / 4, (741.5f - _scroll) * _gui_size, 7 * _gui_size, TEXT::WHITE, settings::consts::depth::menu::str, "Open Command");
		_text->addCenteredText(WIN_WIDTH / 2, (735 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, (_selected_world != 55) ? inputs::get_key_name(inputs::command) : "");
    	_text->addCenteredText(WIN_WIDTH / 2 + 110 * _gui_size, (735 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Reset");
	}

	_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT - 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::controls::str, "Done");
    _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT - 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::controls::str, "Reset");
	// _text->addText(20, 20, 30, TEXT::WHITE, settings::consts::depth::menu::controls::str, "select " + std::to_string(_selection) + " | key " + std::to_string(inputs::get_last_input()) + " | modif " + std::to_string(_selected_world));

	setup_array_buffer_controls();
	blit_to_screen();
	return (menu::ret::no_change);
}

menu::ret Menu::resource_packs_menu( void )
{
	menu::ret ret = menu::ret::no_change;
	if (inputs::key_down(inputs::left_click)) {
		if (_moving_slider) {
		} else if (!inputs::key_update(inputs::left_click)) {
		} else if (_selection == 1 && _change_to_apply) { // Apply Change
			_change_to_apply = false;
			Settings::Get()->setResourcePacks(_active_resource_packs); // TODO visual feedback if missing field in resource_packs
			ret = menu::ret::apply_resource_packs;
		} else if (_selection == 2) { // Done
			_state = (_state == menu::resource_packs) ? menu::options : menu::main_options;
			reset_values();
			return (options_menu());
		} else if (_selection > 2) {
			_moving_slider = true;
		}
	} else {
		_moving_slider = false;
	}
	if (inputs::key_down(inputs::close) && inputs::key_update(inputs::close)) {
		_state = (_state == menu::resource_packs) ? menu::options : menu::main_options;
		reset_values();
		return (options_menu());
	}

	// resource packs' texts are written in setup_array_buffer_resource_packs
	_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 110 * _gui_size, 200 * _gui_size, 20 * _gui_size, (_gui_size + 1) * 7, false, settings::consts::depth::menu::str, "Select Resource Packs");
    _text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Apply Changes");
    _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Done");
	// _text->addText(20, 20, 30, TEXT::WHITE, "select " + std::to_string(_selection));

	setup_array_buffer_resource_packs();
	blit_to_screen();
	return (ret);
}

menu::ret Menu::ingame_inputs( void )
{
	int craft = _state + 1 - menu::inventory; // craft = 1: inventory, 2: crafting, 3: furnace
	if ((inputs::key_down(inputs::close) && inputs::key_update(inputs::close))
		|| (inputs::key_down(inputs::inventory) && inputs::key_update(inputs::inventory))) {
		if (_selected_block.type != blocks::air) {
			if (!_inventory->restoreBlock(_selected_block)) {
				_drops.push_back(_selected_block);
			}
		}
		_inventory->restoreiCraft(_drops);
		_inventory->restoreCraft(_drops);
		_inventory->setModif(true);
		reset_values();
		return ((_drops.size()) ? menu::ret::back_to_game_after_drop : menu::ret::back_to_game);
	}
	if (inputs::key_down(inputs::left_click) && inputs::key_update(inputs::left_click)) {
		if (_selection > 0) {
			if (_selected_block.type == blocks::air) {
				if (inputs::key_down(inputs::left_shift)) {
					_inventory->shiftBlockAt(craft, _selection - 1, _furnace, _chest);
				} else {
					_selected_block = _inventory->pickBlockAt(craft, _selection - 1, _furnace, _chest);
					if (_selected_block.type != blocks::air && inputs::key_down(inputs::left_control)
						&& Settings::Get()->getInt(settings::ints::game_mode) == settings::consts::gamemode::creative
						&& _selection != CELLS::product + 1) {
						_selected_block.amount = s_blocks[_selected_block.type]->stackSize;
					}
				}
			} else {
				_selected_block = _inventory->putBlockAt(craft, _selection - 1, _selected_block, _furnace, _chest);
					_ui->addFace({glm::ivec2(0, 0), {}, {}, {}}, 0, 0, 0, false, true); // TODO better way to update ui than this
			}
		} else if (_selection == -1 && _selected_block.type != blocks::air) { // drop selected item
			return (menu::ret::drop_item_stack);
		}
	}
	if (inputs::key_down(inputs::right_click)) {
		bool first_frame = inputs::key_update(inputs::right_click);
		if (_selection > 0) {
			if (_selected_block.type == blocks::air) {
				if (first_frame) {
					_selected_block = _inventory->pickHalfBlockAt(craft, _selection - 1, _furnace, _chest);
					if (_selected_block.type != blocks::air) {
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
					_selected_block = _inventory->putOneBlockAt(craft, _selection - 1, _selected_block, _furnace, _chest);
					if (_selected_block.amount < count) {
						_selection_list.push_back(_selection);
					}
					_ui->addFace({glm::ivec2(0, 0), {}, {}, {}}, 0, 0, 0, false, true);
				}
			}
		} else if (_selection == -1 && _selected_block.type != blocks::air) { // drop one item
			return (menu::ret::drop_item);
		}
	} else if (inputs::key_update(inputs::right_click)) {
		_selection_list.clear();
	}
	if (inputs::key_down(inputs::slot_0) && inputs::key_update(inputs::slot_0)) {
		_inventory->swapCells(0, _selection - 1);
	}
	if (inputs::key_down(inputs::slot_1) && inputs::key_update(inputs::slot_1)) {
		_inventory->swapCells(1, _selection - 1);
	}
	if (inputs::key_down(inputs::slot_2) && inputs::key_update(inputs::slot_2)) {
		_inventory->swapCells(2, _selection - 1);
	}
	if (inputs::key_down(inputs::slot_3) && inputs::key_update(inputs::slot_3)) {
		_inventory->swapCells(3, _selection - 1);
	}
	if (inputs::key_down(inputs::slot_4) && inputs::key_update(inputs::slot_4)) {
		_inventory->swapCells(4, _selection - 1);
	}
	if (inputs::key_down(inputs::slot_5) && inputs::key_update(inputs::slot_5)) {
		_inventory->swapCells(5, _selection - 1);
	}
	if (inputs::key_down(inputs::slot_6) && inputs::key_update(inputs::slot_6)) {
		_inventory->swapCells(6, _selection - 1);
	}
	if (inputs::key_down(inputs::slot_7) && inputs::key_update(inputs::slot_7)) {
		_inventory->swapCells(7, _selection - 1);
	}
	if (inputs::key_down(inputs::slot_8) && inputs::key_update(inputs::slot_8)) {
		_inventory->swapCells(8, _selection - 1);
	}
	return (menu::ret::no_change);
}

menu::ret Menu::ingame_menu( void )
{
	menu::ret ret = ingame_inputs();
	if (ret == menu::ret::back_to_game_after_drop) {
		return (ret);
	}

	switch (_state) {
		case menu::inventory:
			setup_array_buffer_inventory();
			break ;
		case menu::crafting:
			setup_array_buffer_crafting();
			break ;
		case menu::chest:
			setup_array_buffer_chest();
			break ;
		case menu::furnace:
			setup_array_buffer_furnace();
			break ;
	}

	// adding hovered item's info
	if (_selection > 0 && _selected_block.type == blocks::air) {
		t_item hovered = _inventory->getHoveredAt(_state, _selection - 1, _furnace, _chest);
		if (hovered.type != blocks::air) {
			glm::ivec2 pos = computeScreenPosFromSelection(_selection);
			int font = 5 * _gui_size;
			addUnstretchedQuads(settings::consts::tex::inventory, settings::consts::depth::menu::item_info_back, pos.x + 14 * _gui_size, pos.y + 14 * _gui_size, Utils::Text::textWidth(font, s_blocks[hovered.type]->name) + 10 * _gui_size, font + 10 * _gui_size, 165, 166, 24, 24, 3);
			_text->addText(pos.x + 19 * _gui_size, pos.y + 19 * _gui_size, font, TEXT::GRAY, settings::consts::depth::menu::item_info_str, s_blocks[hovered.type]->name);
		}
	}

	blit_to_screen();
	return (ret);
}

menu::ret Menu::chat_menu( bool animUpdate )
{
	if (inputs::key_down(inputs::close) && inputs::key_update(inputs::close)) {
		glfwSetCharCallback(_window, NULL);
		inputs::resetMessage();
		_chat->resetHistoCursor();
		return (menu::ret::back_to_game);
	}
	if (inputs::key_down(inputs::enter) && inputs::key_update(inputs::enter)) {
		bool cmd = _chat->sendMessage(inputs::getCurrentMessage());
		inputs::resetMessage();
		if (cmd) {
			glfwSetCharCallback(_window, NULL);
			_chat->resetHistoCursor();
			return (menu::ret::back_to_game);
		}
	}
	if (inputs::key_down(inputs::del) && inputs::key_update(inputs::del)) {
		inputs::rmLetter();
	}
	if (inputs::key_down(inputs::look_up) && inputs::key_update(inputs::look_up)) {
		inputs::setCurrentMessage(_chat->getHistoMsg(true));
	}
	if (inputs::key_down(inputs::look_down) && inputs::key_update(inputs::look_down)) {
		inputs::setCurrentMessage(_chat->getHistoMsg(false));
	}
	if (inputs::key_down(inputs::look_right) && inputs::key_update(inputs::look_right)) {
		inputs::moveCursor(true, inputs::key_down(inputs::left_control));
	}
	if (inputs::key_down(inputs::look_left) && inputs::key_update(inputs::look_left)) {
		inputs::moveCursor(false, inputs::key_down(inputs::left_control));
	}

	if (animUpdate) {
		_textBar = !_textBar;
	}
	_chat->blitPastMessages();
	_text->addText(36, WIN_HEIGHT - 48 - 12, 12, TEXT::WHITE, settings::consts::depth::chat, inputs::getCurrentInputStr((_textBar) ? '|' : '.'));

	setup_array_buffer_chat();
	blit_to_screen();
	return (menu::ret::no_change);
}

menu::ret Menu::sign_menu( bool animUpdate )
{
	if (_selection == 1 && inputs::key_down(inputs::left_click) && inputs::key_update(inputs::left_click)) { // Done
		glfwSetCharCallback(_window, NULL);
		std::string line = inputs::getCurrentMessage();
		_sign_content.push_back(line);
		inputs::resetMessage();
		reset_values();
		return ((_sign_content.size() > 0) ? menu::ret::sign_done : menu::ret::back_to_game);
	}
	if (inputs::key_down(inputs::close) && inputs::key_update(inputs::close)) {
		glfwSetCharCallback(_window, NULL);
		inputs::resetMessage();
		reset_values();
		return (menu::ret::back_to_game);
	}
	if (inputs::key_down(inputs::enter) && inputs::key_update(inputs::enter)) {
		if (_sign_content.size() < 3) {
			std::string line = inputs::getCurrentMessage();
			_sign_content.push_back(line);
			inputs::resetMessage();
		} 
	}
	if (inputs::key_down(inputs::del) && inputs::key_update(inputs::del)) {
		inputs::rmLetter();
	}
	if (Utils::Text::textWidth(7, inputs::getCurrentMessage()) > 90) {
		inputs::rmLetter();
	}
	// if (inputs::key_down(inputs::look_up) && inputs::key_update(inputs::look_up)) {
	// 	inputs::setCurrentMessage(_chat->getHistoMsg(true));
	// }
	// if (inputs::key_down(inputs::look_down) && inputs::key_update(inputs::look_down)) {
	// 	inputs::setCurrentMessage(_chat->getHistoMsg(false));
	// }
	if (inputs::key_down(inputs::look_right) && inputs::key_update(inputs::look_right)) {
		inputs::moveCursor(true, inputs::key_down(inputs::left_control));
	}
	if (inputs::key_down(inputs::look_left) && inputs::key_update(inputs::look_left)) {
		inputs::moveCursor(false, inputs::key_down(inputs::left_control));
	}

	_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 110 * _gui_size, 200 * _gui_size, 20 * _gui_size, (_gui_size + 1) * 7, false, settings::consts::depth::menu::str, "Edit Sign Message");
	size_t index = 0;
	for (; index < _sign_content.size(); ++index) {
		_text->addCenteredText(WIN_WIDTH / 2, WIN_HEIGHT / 2 - 50 * _gui_size + index * 10 * _gui_size, 0, 0, 7 * _gui_size, false, settings::consts::depth::menu::str, _sign_content[index]);
	}
	if (animUpdate) {
		_textBar = !_textBar;
	}
	_text->addCenteredText(WIN_WIDTH / 2, WIN_HEIGHT / 2 - 50 * _gui_size + index * 10 * _gui_size, 0, 0, 7 * _gui_size, false, settings::consts::depth::menu::str, inputs::getCurrentInputStr((_textBar) ? '|' : '.'));
    _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Done");

	setup_array_buffer_sign();
	blit_to_screen();
	return (menu::ret::no_change);
}

menu::ret Menu::book_and_quill_menu( bool animUpdate )
{
	if (!_book_content || (inputs::key_down(inputs::close) && inputs::key_update(inputs::close))) {
		if (!_book_content) { _chat->chatMessage("Missing book content ptr.", TEXT::RED); }
		glfwSetCharCallback(_window, NULL);
		inputs::resetMessage();
		reset_values();
		return (menu::ret::back_to_game);
	}
	if (inputs::key_down(inputs::left_click) && inputs::key_update(inputs::left_click)) {
		if (_selection == 1) { // Sign
			_state = menu::book_and_quill_sign;
			_book_content->at(_scroll) = inputs::getCurrentMessage();
			inputs::resetMessage();
			return (book_sign_menu(animUpdate));
		} else if (_selection == 2) { // Done
			_book_content->at(_scroll) = inputs::getCurrentMessage();
			glfwSetCharCallback(_window, NULL);
			inputs::resetMessage();
			reset_values();
			return (menu::ret::back_to_game);
		} else if (_selection == 3 && _scroll) { // left arrow
			_book_content->at(_scroll) = inputs::getCurrentMessage();
			--_scroll;
			inputs::setCurrentMessage(_book_content->at(_scroll));
		} else if (_selection == 4 && _scroll < 99) { // right arrow
			_book_content->at(_scroll) = inputs::getCurrentMessage();
			++_scroll;
			while (_book_content->size() <= static_cast<size_t>(_scroll)) {
				_book_content->push_back("");
			}
			inputs::setCurrentMessage(_book_content->at(_scroll));
		}
	}
	if (inputs::key_down(inputs::enter) && inputs::key_update(inputs::enter)) {
		std::string line = inputs::getCurrentMessage();
		int nbrLines = 0;
		for (auto c : line) {
			nbrLines += (c == '\n');
		}
		if (nbrLines < 16) {
			inputs::addLetter('\n');
			_textBar = true;
		} 
	}
	if (inputs::key_down(inputs::del) && inputs::key_update(inputs::del)) {
		inputs::rmLetter();
	}
	if (Utils::Text::textWidth(6, inputs::getCurrentMessage()) > 114) {
		inputs::rmLetter();
	}
	// if (inputs::key_down(inputs::look_up) && inputs::key_update(inputs::look_up)) {
	// 	inputs::setCurrentMessage(_chat->getHistoMsg(true));
	// }
	// if (inputs::key_down(inputs::look_down) && inputs::key_update(inputs::look_down)) {
	// 	inputs::setCurrentMessage(_chat->getHistoMsg(false));
	// }
	if (inputs::key_down(inputs::look_right) && inputs::key_update(inputs::look_right)) {
		_textBar = true;
		inputs::moveCursor(true, inputs::key_down(inputs::left_control));
	}
	if (inputs::key_down(inputs::look_left) && inputs::key_update(inputs::look_left)) {
		_textBar = true;
		inputs::moveCursor(false, inputs::key_down(inputs::left_control));
	}

	std::string pageStr = "Page " + std::to_string(_scroll + 1) + " of " + std::to_string(_book_content->size());
	_text->addText(WIN_WIDTH / 2 + 55 * _gui_size - Utils::Text::textWidth(6 * _gui_size, pageStr), WIN_HEIGHT / 2 - 115 * _gui_size, 6 * _gui_size, TEXT::BLACK, settings::consts::depth::menu::str, pageStr);
	if (animUpdate) {
		_textBar = !_textBar;
	}
	_text->addText(WIN_WIDTH / 2 - 58 * _gui_size, WIN_HEIGHT / 2 - 107 * _gui_size, 6 * _gui_size, TEXT::BLACK, settings::consts::depth::menu::str, inputs::getCurrentInputStr((_textBar) ? '|' : '.'));
    _text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 95 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Sign");
    _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 95 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, "Done");

	setup_array_buffer_book();
	blit_to_screen();
	return (menu::ret::no_change);
}

menu::ret Menu::book_sign_menu( bool animUpdate )
{
	if (inputs::key_down(inputs::close) && inputs::key_update(inputs::close)) {
		inputs::setCurrentMessage(_book_content->at(_scroll));
		_state = menu::book_and_quill;
		return (book_and_quill_menu(animUpdate));
	}
	if (inputs::key_down(inputs::left_click) && inputs::key_update(inputs::left_click)) {
		if (_selection == 1) { // Sign and close
			glfwSetCharCallback(_window, NULL);
			inputs::resetMessage();
			reset_values();
			return (menu::ret::sign_book);
		} else if (_selection == 2) { // Cancel
			inputs::setCurrentMessage(_book_content->at(_scroll));
			_state = menu::book_and_quill;
			return (book_and_quill_menu(animUpdate));
		}
	}
	if (inputs::key_down(inputs::del) && inputs::key_update(inputs::del)) {
		inputs::rmLetter();
	}
	if (Utils::Text::textWidth(6, inputs::getCurrentMessage()) > 114) {
		inputs::rmLetter();
	}
	if (inputs::key_down(inputs::look_right) && inputs::key_update(inputs::look_right)) {
		_textBar = true;
		inputs::moveCursor(true, inputs::key_down(inputs::left_control));
	}
	if (inputs::key_down(inputs::look_left) && inputs::key_update(inputs::look_left)) {
		_textBar = true;
		inputs::moveCursor(false, inputs::key_down(inputs::left_control));
	}

	_text->addText((WIN_WIDTH - Utils::Text::textWidth(6 * _gui_size, "Enter Book Title:")) / 2, WIN_HEIGHT / 2 - 90 * _gui_size, 6 * _gui_size, TEXT::BLACK, settings::consts::depth::menu::str, "Enter Book Title:");
	if (animUpdate) {
		_textBar = !_textBar;
	}
	_text->addText(WIN_WIDTH / 2 - 58 * _gui_size, WIN_HEIGHT / 2 - 80 * _gui_size, 6 * _gui_size, TEXT::BLACK, settings::consts::depth::menu::str, inputs::getCurrentInputStr((_textBar) ? '|' : '.')); // TODO _text->addCursorText(str, cursorPos, cursorChar) which draws cursorChar on top of char at cursorPos
	_text->addText(WIN_WIDTH / 2 - 58 * _gui_size, WIN_HEIGHT / 2 - 50 * _gui_size, 6 * _gui_size, TEXT::BLACK, settings::consts::depth::menu::str, "Note! Once you sign\nthe book, it will no\nlonger be editable.");
	
	_text->addCenteredText(WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 95 * _gui_size, 20 * _gui_size, 6 * _gui_size, true, settings::consts::depth::menu::str, "Sign and Close");
    _text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 95 * _gui_size, 20 * _gui_size, 6 * _gui_size, true, settings::consts::depth::menu::str, "Cancel");

	setup_array_buffer_book();
	blit_to_screen();
	return (menu::ret::no_change);
}

void Menu::addQuads( int atlas, int depth, int posX, int posY, int width, int height, int texX, int texY, int texWidth, int texHeight )
{
	--texWidth;--texHeight;
	_vertices.push_back({texX + 0        + ((texY + 0        ) << 8) + (0 << 16) + (0 << 17) + (atlas << 18) + (depth << 24), posX,         posY});
	_vertices.push_back({texX + texWidth + ((texY + 0        ) << 8) + (1 << 16) + (0 << 17) + (atlas << 18) + (depth << 24), posX + width, posY});
	_vertices.push_back({texX + 0        + ((texY + texHeight) << 8) + (0 << 16) + (1 << 17) + (atlas << 18) + (depth << 24), posX,         posY + height});

	_vertices.push_back({texX + texWidth + ((texY + 0        ) << 8) + (1 << 16) + (0 << 17) + (atlas << 18) + (depth << 24), posX + width, posY});
	_vertices.push_back({texX + texWidth + ((texY + texHeight) << 8) + (1 << 16) + (1 << 17) + (atlas << 18) + (depth << 24), posX + width, posY + height});
	_vertices.push_back({texX + 0        + ((texY + texHeight) << 8) + (0 << 16) + (1 << 17) + (atlas << 18) + (depth << 24), posX,         posY + height});
}

/**
 * called for container texture, of which we don't want the border to be stretched
 */
void Menu::addUnstretchedQuads( int atlas, int depth, int posX, int posY, int width, int height, int texX, int texY, int texWidth, int texHeight, int border )
{
	// left border
	addQuads(atlas, depth, posX, posY, border * _gui_size, height, texX, texY, border, texHeight);
	// right border
	addQuads(atlas, depth, posX + width - border * _gui_size, posY, border * _gui_size, height, texX + texWidth - border, texY, border, texHeight);
	// top border
	addQuads(atlas, depth, posX + border * _gui_size, posY, width - 2 * border * _gui_size, border * _gui_size, texX + border, texY, texWidth - 2 * border, border);
	// bottom border
	addQuads(atlas, depth, posX + border * _gui_size, posY + height - border * _gui_size, width - 2 * border * _gui_size, border * _gui_size, texX + border, texY + texHeight - border, texWidth - 2 * border, border);
	// inner part
	addQuads(atlas, depth, posX + border * _gui_size, posY + border * _gui_size, width - 2 * border * _gui_size, height - 2 * border * _gui_size, texX + border, texY + border, texWidth - 2 * border, texHeight - 2 * border);
}

void Menu::setup_array_buffer_main( void )
{
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 129 * _gui_size, WIN_HEIGHT / 2 - 104 * _gui_size, 256 * _gui_size, 64 * _gui_size, 0, 131, 256, 64); // MINECRAFT

    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 1) ? 111 : 91, 200, 20); // Singleplayer
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20); // Multiplayer
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 40 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20); // Minecraft Realms

    addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 125 * _gui_size, WIN_HEIGHT / 2 + 80 * _gui_size, 15 * _gui_size, 20 * _gui_size, 0, 71, 200, 20, 3); // Lang settings
    addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 80 * _gui_size, 95 * _gui_size, 20 * _gui_size, 0, (_selection == 5) ? 111 : 91, 200, 20, 3); // Options...
    addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 80 * _gui_size, 95 * _gui_size, 20 * _gui_size, 0, (_selection == 6) ? 111 : 91, 200, 20, 3); // Quit Game
    addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 110 * _gui_size, WIN_HEIGHT / 2 + 80 * _gui_size, 15 * _gui_size, 20 * _gui_size, 0, 71, 200, 20, 3); // accessibility settings

	setup_shader();
}

void Menu::setup_array_buffer_select( void )
{
	addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 155 * _gui_size, WIN_HEIGHT / 2 + 90 * _gui_size, 150 * _gui_size, 20 * _gui_size, 0, (91 + 20 * (_selection == 1)) * (_selected_world != 0) + 71 * (_selected_world == 0), 200, 20, 3); // playSelectedWorld
	addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 155 * _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size, 73 * _gui_size, 20 * _gui_size, 0, 71, 200, 20, 3); // edit
	addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 78 * _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size, 73 * _gui_size, 20 * _gui_size, 0, 71, 200, 20, 3); // delete
	addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 90 * _gui_size, 150 * _gui_size, 20 * _gui_size, 0, (_selection == 4) ? 111 : 91, 200, 20, 3); // createNewWorld
	addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size, 73 * _gui_size, 20 * _gui_size, 0, 71, 200, 20, 3); // reCreate
	addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 82 * _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size, 73 * _gui_size, 20 * _gui_size, 0, (_selection == 6) ? 111 : 91, 200, 20, 3); // cancel

	for (int index = 0; index < static_cast<int>(static_cast<int>(_worlds.size())) && index < 8; index++) {
		addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, (30 + 20 * index) * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71 + 20 * (_selected_world - 1 == index), 200, 20); // world #index
	}

	setup_shader();
}

/**
 * @brief sets up array buffer for Create New World menu
*/
void Menu::setup_array_buffer_create( void )
{
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_input_world) ? 195 : 71, 200, 20); // World Name
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_input_seed) ? 195 : 71, 200, 20); // Seed
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 3) ? 111 : 91, 200, 20); // World Type
	if (Settings::Get()->getBool(settings::bools::flat_world)) {
		addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 4) ? 111 : 91, 200, 20); // Flat World Block
	}

	addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 155 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 150 * _gui_size, 20 * _gui_size, 0, (_world_file == "") ? 71 : (_selection == 5) ? 111 : 91, 200, 20, 3); // Create New World
	addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 150 * _gui_size, 20 * _gui_size, 0, (_selection == 6) ? 111 : 91, 200, 20, 3); // Cancel

	setup_shader();
}

void Menu::setup_array_buffer_load( int completion )
{
	// std::cout << "current completion : " << completion << std::endl;
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 110, WIN_HEIGHT / 2 - 120, 220, 240, 0, 71, 200, 20);

	int step = 1, turnCounter = 0, numSteps = 1, stepSize = 20, state = 0, x = WIN_WIDTH / 2 - 20, y = WIN_HEIGHT / 2;
	for (; step < 1 + 100; step++) {
		int offset = (step < 1 + completion / 10) ? 48 : 32; // cobblestone or stone
		addQuads(settings::consts::tex::ui, settings::consts::depth::menu::str, x, y, stepSize, stepSize, offset, 0, 16, 16);
		if (step == 1 + completion / 10) {
			addQuads(settings::consts::tex::ui, settings::consts::depth::menu::dura, x, y, stepSize, stepSize, 80 + 16 * (completion % 10), 0, 16, 16); // break frame
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
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::occlusion, 0, 0, WIN_WIDTH, WIN_HEIGHT, 16, 15, 1, 1); // occult window in redish
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 1) ? 111 : 91, 200, 20); // Respawn
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 2) ? 111 : 91, 200, 20); // Title Screen

	setup_shader();
}

void Menu::setup_array_buffer_pause( void )
{
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::occlusion, 0, 0, WIN_WIDTH, WIN_HEIGHT, 3, 29, 1, 1); // occult window
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 60 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 1) ? 111 : 91, 200, 20); // Back to Game

    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 35 * _gui_size, 95 * _gui_size, 20 * _gui_size, 0, 71, 200, 20); // Advancements
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size, 95 * _gui_size, 20 * _gui_size, 0, 71, 200, 20); // Give Feedback
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size, 95 * _gui_size, 20 * _gui_size, 0, (_selection == 6) ? 111 : 91, 200, 20); // Options...

    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 35 * _gui_size, 95 * _gui_size, 20 * _gui_size, 0, 71, 200, 20); // Statistics
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size, 95 * _gui_size, 20 * _gui_size, 0, 71, 200, 20); // Report Bugs
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 15 * _gui_size, 95 * _gui_size, 20 * _gui_size, 0, 71, 200, 20); // Open to LAN

    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 40 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 8) ? 111 : 91, 200, 20); // Save and Quit to Title

	setup_shader();
}

void Menu::setup_array_buffer_options( void )
{
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::occlusion, 0, 0, WIN_WIDTH, WIN_HEIGHT, 3, 29, 1, 1); // occult window
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20); // FOV
    addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::slider, WIN_WIDTH / 2 - 205 * _gui_size + static_cast<int>(gradient(_fov_gradient, 50, 110, 0, 190)) * _gui_size, WIN_HEIGHT / 2 - 84 * _gui_size, 10 * _gui_size, 18 * _gui_size, 0, (_selection == 1) ? 111 : 91, 200, 20, 3); // FOV Slider
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20); // Realms Notifications

    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 3) ? 111 : 91, 200, 20); // Skin Customization...
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 5) ? 111 : 91, 200, 20); // Video Settings...
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20); // Language...
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 9) ? 111 : 91, 200, 20); // Resource Packs...

    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20); // Music & Sounds...
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 6) ? 111 : 91, 200, 20); // Controls...
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20); // Chat Settings...
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20); // Accessibility Settings...

    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 11) ? 111 : 91, 200, 20); // Done

	setup_shader();
}

void Menu::setup_array_buffer_skin( void )
{
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::occlusion, 0, 40 * _gui_size, WIN_WIDTH, WIN_HEIGHT - 40 * _gui_size, 1, 72, 1, 1); // occult central part

	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2, (85) * _gui_size, 90 * _gui_size, 20 * _gui_size, 0, 71, 200, 20); // List Players currently disabled

	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::controls::occlusion, 0, 0, WIN_WIDTH, 40 * _gui_size, 0, 91, 1, 1); // occult top part
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::controls::occlusion, 0, WIN_HEIGHT - 40 * _gui_size, WIN_WIDTH, 40 * _gui_size, 0, 91, 1, 1); // occult bottom part
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::controls::bars, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT - 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 1) ? 111 : 91, 200, 20); // Done
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::controls::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT - 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_change_to_apply) ? (_selection == 2) ? 111 : 91 : 71, 200, 20); // Reset
	setup_shader();
}

void Menu::setup_array_buffer_video( void )
{
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::occlusion, 0, 0, WIN_WIDTH, WIN_HEIGHT, 3, 29, 1, 1); // occult window
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20); // Render dist
    addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::slider, WIN_WIDTH / 2 - 205 * _gui_size + static_cast<int>(gradient(_render_gradient, 8, 32, 0, 190)) * _gui_size, WIN_HEIGHT / 2 - 84 * _gui_size, 10 * _gui_size, 18 * _gui_size, 0, (_selection == 1) ? 111 : 91, 200, 20, 3); // render Slider
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 2) ? 111 : 91, 200, 20); // Resolution

    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 3) ? 111 : 91, 200, 20); // Clouds
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 5) ? 111 : 91, 200, 20); // Gui scale
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20); // brightness
    addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::slider, WIN_WIDTH / 2 - 205 * _gui_size + static_cast<int>(gradient(_brightness_gradient, 0, 1, 0, 190)) * _gui_size, WIN_HEIGHT / 2 + 6 * _gui_size, 10 * _gui_size, 18 * _gui_size, 0, (_selection == 7) ? 111 : 91, 200, 20, 3); // brightness Slider
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 9) ? 111 : 91, 200, 20); // Face Culling

    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, 71, 200, 20); // Fullscreen
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 6) ? 111 : 91, 200, 20); // Skybox
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 8) ? 111 : 91, 200, 20); // Particles
    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 10) ? 111 : 91, 200, 20); // Smooth Lighting

    addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 11) ? 111 : 91, 200, 20); // Done

	if (_drop_down_menu) {
		for (int index = 0; index < Settings::Get()->getInt(settings::ints::available_res); ++index) {
			addQuads(settings::consts::tex::ui, settings::consts::depth::menu::controls::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - (65 - 18 * index) * _gui_size, 200 * _gui_size, 18 * _gui_size, 0, 72, 200, 18);
		}
	}
	setup_shader();
}

void Menu::setup_array_buffer_controls( void )
{
	const std::array<int, 27> actions = {inputs::destroy, inputs::drop, inputs::sample, inputs::run, inputs::use, -1,
				inputs::slot_0, inputs::slot_1, inputs::slot_2, inputs::slot_3, inputs::slot_4, inputs::slot_5, inputs::slot_6, inputs::slot_7, inputs::slot_8, inputs::inventory, -1,
				inputs::jump, inputs::sneak, inputs::move_left, inputs::move_right, inputs::move_backwards, inputs::move_forwards, -1,
				-1, inputs::chat, inputs::command};

	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::occlusion, 0, 40 * _gui_size, WIN_WIDTH, WIN_HEIGHT - 40 * _gui_size, 1, 72, 1, 1); // occult central part

	_change_to_apply = false;
	for (int i = 0; i < 27; ++i) {
		if (i == 5 || i == 16 || i == 23 || i == 24) {
			continue ;
		}
		addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2, (85 + 25 * i - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 0, (_selection == 3 + i * 2) ? 111 : 91, 200, 20, 3); // key bind
		bool canReset = !inputs::is_initial_key_bind(actions[i]);
		if (canReset) _change_to_apply = true;
		addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 110 * _gui_size, (85 + 25 * i - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 0, (canReset) ? (_selection == 4 + i * 2) ? 111 : 91 : 71, 200, 20, 3); // reset
	}
	addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2, (85 + 25 * 24 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 0, 71, 200, 20, 3); // List Players currently disabled
	addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 110 * _gui_size, (85 + 25 * 24 - _scroll) * _gui_size, 90 * _gui_size, 20 * _gui_size, 0, 71, 200, 20, 3);

	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::controls::occlusion, 0, 0, WIN_WIDTH, 40 * _gui_size, 0, 91, 1, 1); // occult top part
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::controls::occlusion, 0, WIN_HEIGHT - 40 * _gui_size, WIN_WIDTH, 40 * _gui_size, 0, 91, 1, 1); // occult bottom part
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::controls::bars, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT - 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 1) ? 111 : 91, 200, 20); // Done
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::controls::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT - 30 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_change_to_apply) ? (_selection == 2) ? 111 : 91 : 71, 200, 20); // Reset
	setup_shader();
}

void Menu::setup_array_buffer_resource_packs( void )
{
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::occlusion, 0, 0, WIN_WIDTH, WIN_HEIGHT, 3, 29, 1, 1); // occult window

	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::occlusion_bis, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 135 * _gui_size, 3, 29, 1, 1); // occult available section
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::occlusion_bis, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 135 * _gui_size, 3, 29, 1, 1); // occult selected section

	int rp_size = 0;
	for (;rp_size < static_cast<int>(_resource_packs.size()); ++rp_size) {
		if (_moving_slider && _selection == 3 + rp_size) {
			double mouseX, mouseY;
			glfwGetCursorPos(_window, &mouseX, &mouseY);
			addQuads(settings::consts::tex::ui, settings::consts::depth::menu::controls::bars, static_cast<int>(mouseX) - 95 * _gui_size, static_cast<int>(mouseY) - 10 * _gui_size, 190 * _gui_size, 20 * _gui_size, 0, 71, 200, 20);
			_text->addCenteredText(mouseX, mouseY, 0, 0, 7 * _gui_size, true, settings::consts::depth::menu::controls::str, _resource_packs[rp_size].substr(0, _resource_packs[rp_size].size() - 5));
		} else {
			addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 200 * _gui_size, WIN_HEIGHT / 2 - (80 - rp_size * 20) * _gui_size, 190 * _gui_size, 20 * _gui_size, 0, 71, 200, 20);
			_text->addCenteredText(WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - (80 - rp_size * 20) * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, _resource_packs[rp_size].substr(0, _resource_packs[rp_size].size() - 5));
		}
	}

	for (int index = 0; index < static_cast<int>(_active_resource_packs.size()); ++index) {
		if (_moving_slider && _selection == 4 + rp_size + index) {
			double mouseX, mouseY;
			glfwGetCursorPos(_window, &mouseX, &mouseY);
			addQuads(settings::consts::tex::ui, settings::consts::depth::menu::controls::bars, static_cast<int>(mouseX) - 95 * _gui_size, static_cast<int>(mouseY) - 10 * _gui_size, 190 * _gui_size, 20 * _gui_size, 0, 71, 200, 20);
			_text->addCenteredText(mouseX, mouseY, 0, 0, 7 * _gui_size, true, settings::consts::depth::menu::controls::str, _active_resource_packs[index].substr(0, _active_resource_packs[index].size() - 5));
		} else {
			addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 10 * _gui_size, WIN_HEIGHT / 2 - (80 - index * 20) * _gui_size, 190 * _gui_size, 20 * _gui_size, 0, 71, 200, 20);
			_text->addCenteredText(WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - (80 - index * 20) * _gui_size, 200 * _gui_size, 20 * _gui_size, 7 * _gui_size, true, settings::consts::depth::menu::str, _active_resource_packs[index].substr(0, _active_resource_packs[index].size() - 5));
		}
	}

	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_change_to_apply) ? (_selection == 1) ? 111 : 91 : 71, 200, 20); // Apply
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 2) ? 111 : 91, 200, 20); // Done

	setup_shader();
}

void Menu::setup_array_buffer_chat( void )
{
	int nbr = _chat->computeHeight();
	addQuads(settings::consts::tex::ui, settings::consts::depth::chat_back, CHAT_BOX_X, WIN_HEIGHT - 48 - 8 - 18 * (nbr + 1), CHAT_BOX_WIDTH, 4 + 18 * nbr, 3, 29, 1, 1); // occult chat box
	addQuads(settings::consts::tex::ui, settings::consts::depth::chat_back, CHAT_BOX_X, WIN_HEIGHT - 48 - 18, CHAT_BOX_WIDTH, 20, 3, 29, 1, 1); // occult input box

	setup_shader();
}

void Menu::setup_array_buffer_sign( void )
{
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::occlusion, 0, 0, WIN_WIDTH, WIN_HEIGHT, 3, 29, 1, 1); // occult window

	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 50 * _gui_size, WIN_HEIGHT / 2 - 60 * _gui_size, 100 * _gui_size, 50 * _gui_size, 24, 47, 24, 12); // sign main body
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 4.1666f * _gui_size, WIN_HEIGHT / 2 - 10 * _gui_size, 8.333f * _gui_size, 50 * _gui_size, 48, 47, 2, 12); // sign post
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size, 0, (_selection == 1) ? 111 : 91, 200, 20); // Done

	setup_shader();
}

void Menu::setup_array_buffer_book( void )
{
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::occlusion, 0, 0, WIN_WIDTH, WIN_HEIGHT, 3, 29, 1, 1); // occult window

	addQuads(settings::consts::tex::book, settings::consts::depth::menu::container, WIN_WIDTH / 2 - 74 * _gui_size, WIN_HEIGHT / 2 - 130 * _gui_size, 146 * _gui_size, 180 * _gui_size, 20, 1, 146, 180);
	addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 95 * _gui_size, 20 * _gui_size, 0, (_selection == 1) ? 111 : 91, 200, 20, 3); // Sign
	addUnstretchedQuads(settings::consts::tex::ui, settings::consts::depth::menu::bars, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 95 * _gui_size, 20 * _gui_size, 0, (_selection == 2) ? 111 : 91, 200, 20, 3); // Done
	
	if (_state != menu::book_and_quill_sign) {
		if (_scroll) {
			addQuads(settings::consts::tex::book, settings::consts::depth::menu::selection, WIN_WIDTH / 2 - 58 * _gui_size, WIN_HEIGHT / 2 + 23 * _gui_size, 23 * _gui_size, 13 * _gui_size, (_selection == 3) ? 23 : 0, 205, 23, 13); // left arrow
		}
		if (_scroll < 99) {
			addQuads(settings::consts::tex::book, settings::consts::depth::menu::selection, WIN_WIDTH / 2 + 25 * _gui_size, WIN_HEIGHT / 2 + 23 * _gui_size, 23 * _gui_size, 13 * _gui_size, (_selection == 4) ? 23 : 0, 192, 23, 13); // right arrow
		}
	}

	setup_shader();
}

glm::ivec2 Menu::computeScreenPosFromSelection( int selection )
{
	if (!selection) {
	} else if (selection < 10) { // hot bar slot
		int index = selection - 1;
		return {(WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * index * _gui_size) + _gui_size * 3, WIN_HEIGHT / 2 + 59 * _gui_size};
	} else if (selection < 37) { // backpack
		int index = selection - 10;
		return {(WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (index % 9) * _gui_size) + _gui_size * 3, WIN_HEIGHT / 2 + _gui_size + 18 * _gui_size * (index / 9)};
	} else if (selection < 41) { // icraft
		int index = selection - 37;
		return {(WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (5 + index % 2) * _gui_size) + _gui_size * 3, WIN_HEIGHT / 2 - 65 * _gui_size + 18 * _gui_size * (index / 2)};
	} else if (selection == 41) { // crafted
		if (_state == menu::inventory) {
			return {(WIN_WIDTH - (166 * _gui_size)) / 2 + 149 * _gui_size, WIN_HEIGHT / 2 - 55 * _gui_size};
		} else if (_state == menu::crafting) {
			return {(WIN_WIDTH - (166 * _gui_size)) / 2 + 119 * _gui_size, WIN_HEIGHT / 2 - 48 * _gui_size};
		} else if (_state == menu::furnace) {
			return {(WIN_WIDTH - (166 * _gui_size)) / 2 + 111 * _gui_size, WIN_HEIGHT / 2 - 48 * _gui_size};
		}
	} else if (selection < 51) { // craft
		int index = selection - 42;
		return {(WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (1 + index % 3) * _gui_size) + _gui_size * 7, WIN_HEIGHT / 2 - 66 * _gui_size + 18 * _gui_size * (index / 3)};
	} else if (selection == 51) { // composant
		return {(WIN_WIDTH - (166 * _gui_size)) / 2 + 51 * _gui_size, WIN_HEIGHT / 2 - 66 * _gui_size};
	} else if (selection == 52) { // fuel
		return {(WIN_WIDTH - (166 * _gui_size)) / 2 + 51 * _gui_size, WIN_HEIGHT / 2 - 30 * _gui_size};
	} else if (selection < 80) { // chest
		int index = selection - 53;
		return {(WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (index % 9) * _gui_size) + _gui_size * 3, WIN_HEIGHT / 2 - 65 * _gui_size + 18 * _gui_size * (index / 9)};
	}
	return {0, 0};
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
		if (cell > 0) {
			glm::ivec2 pos = computeScreenPosFromSelection(cell);
			addQuads(settings::consts::tex::ui, settings::consts::depth::menu::selection, pos.x, pos.y, 16 * _gui_size, 16 * _gui_size, 16, 14, 1, 1);
		}
	}
	if (!found) {
		_selection_list.pop_back();
	}
}

void Menu::add_dura_value( std::shared_ptr<ATag> tag, int x, int y )
{
	if (!tag || tag->getType() != tags::tool_tag) {
		return ;
	}
	float percent = static_cast<ToolTag*>(tag.get())->getDuraPercent();
	if (percent > .99f) {
		return ;
	}
	// adding grey bar first
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::dura_back, x + _gui_size, y + 14 * _gui_size, 14 * _gui_size, _gui_size, 16, 0, 1, 1);
	// adding progress bar second
	addQuads(settings::consts::tex::ui, settings::consts::depth::menu::dura, x + _gui_size, y + 14 * _gui_size, static_cast<int>(14 * _gui_size * percent), _gui_size, (percent < .6f) ? (percent < .3f) ? 19 : 18 : 17, 0, 1, 1);
}

void Menu::display_item_value( int x, int y, int amount, int depth )
{
	if (amount <= 1) {
		return ;
	}
	if (amount > 9) {
		_text->addText(x + 11 * _gui_size - 6 * _gui_size + _gui_size, y + 9 * _gui_size + _gui_size, 8 * _gui_size, TEXT::BLACK, depth + 1, std::to_string(amount / 10));
		_text->addText(x + 11 * _gui_size - 6 * _gui_size, y + 9 * _gui_size, 8 * _gui_size, TEXT::WHITE, depth, std::to_string(amount / 10));
	}
	_text->addText(x + 11 * _gui_size + _gui_size, y + 9 * _gui_size + _gui_size, 8 * _gui_size, TEXT::BLACK, depth + 1, std::to_string(amount % 10));
	_text->addText(x + 11 * _gui_size, y + 9 * _gui_size, 8 * _gui_size, TEXT::WHITE, depth, std::to_string(amount % 10));
}

void Menu::add_item_value( t_item item, int x, int y, int depth, bool movement )
{
	display_item_value(x, y, item.amount, depth);
	add_dura_value(item.tag, x, y);
	s_blocks[item.type]->addItem(_ui, x, y, _gui_size, 16, (depth == settings::consts::depth::menu::item_str) ? settings::consts::depth::item::menu : settings::consts::depth::item::held, true, movement);
}

void Menu::add_slot_value( int index )
{
	t_item item = _inventory->getSlotBlock(index);
	if (item.type == blocks::air) {
		return ;
	}
	int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * index * _gui_size) + _gui_size * 3;
	int y = WIN_HEIGHT / 2 + 59 * _gui_size;
	add_item_value(item, x, y, settings::consts::depth::menu::item_str);
}

void Menu::add_backpack_value( int index )
{
	t_item item = _inventory->getBackpackBlock(index);
	if (item.type == blocks::air) {
		return ;
	}
	int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (index % 9) * _gui_size) + _gui_size * 3;
	int y = WIN_HEIGHT / 2 + _gui_size + 18 * _gui_size * (index / 9);
	add_item_value(item, x, y, settings::consts::depth::menu::item_str);
}

void Menu::add_icraft_value( int index )
{
	t_item item = _inventory->getiCraftBlock(index);
	if (item.type == blocks::air) {
		return ;
	}
	int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (5 + index % 2) * _gui_size) + _gui_size * 3;
	int y = WIN_HEIGHT / 2 - 65 * _gui_size + 18 * _gui_size * (index / 2);
	add_item_value(item, x, y, settings::consts::depth::menu::item_str);
}

void Menu::add_craft_value( int index )
{
	t_item item = _inventory->getCraftBlock(index);
	if (item.type == blocks::air) {
		return ;
	}
	int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (1 + index % 3) * _gui_size) + _gui_size * 7;
	int y = WIN_HEIGHT / 2 - 66 * _gui_size + 18 * _gui_size * (index / 3);
	add_item_value(item, x, y, settings::consts::depth::menu::item_str);
}

void Menu::add_crafted_value( void )
{
	t_item item = _inventory->getCrafted();
	if (item.type == blocks::air) {
		return ;
	}
	if (_state == menu::inventory) {
		int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + 149 * _gui_size;
		int y = WIN_HEIGHT / 2 - 55 * _gui_size;
		add_item_value(item, x, y, settings::consts::depth::menu::item_str);
	} else if (_state == menu::crafting) {
		int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + 119 * _gui_size;
		int y = WIN_HEIGHT / 2 - 48 * _gui_size;
		add_item_value(item, x, y, settings::consts::depth::menu::item_str);
	}
}

void Menu::add_chest_value( int index )
{
	t_item *item = _chest->getItem(index);
	if (!item) {
		// std::cout << "add_chest_value NULL item at " << index << std::endl;
		return ;
	}
	if (item->type == blocks::air) {
		return ;
	}
	int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (index % 9) * _gui_size) + _gui_size * 3;
	int y = WIN_HEIGHT / 2 - 65 * _gui_size + 18 * _gui_size * (index / 9);
	add_item_value(*item, x, y, settings::consts::depth::menu::item_str);
}

void Menu::add_furnace_value( void )
{
	t_item item = _furnace->getComposant();
	if (item.type != blocks::air) {
		int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + 51 * _gui_size;
		int y = WIN_HEIGHT / 2 - 66 * _gui_size;
		add_item_value(item, x, y, settings::consts::depth::menu::item_str);
	}
	int progress = 1 + glm::floor(_furnace->getFuelTime() * 13);
	if (_furnace->getFuelTime()) {
		addQuads(settings::consts::tex::furnace, settings::consts::depth::menu::dura, (WIN_WIDTH - (166 * _gui_size)) / 2 + 50 * _gui_size + _gui_size, WIN_HEIGHT / 2 - 48 * _gui_size + progress * _gui_size + _gui_size, 14 * _gui_size, (14 - progress) * _gui_size, 176, progress, 14, 14 - progress);
	}
	progress = 1 + glm::floor(_furnace->getComposantTime() * 23);
	if (_furnace->getComposantTime()) {
		addQuads(settings::consts::tex::furnace, settings::consts::depth::menu::dura, (WIN_WIDTH - (166 * _gui_size)) / 2 + 74 * _gui_size, WIN_HEIGHT / 2 - 49 * _gui_size, progress * _gui_size, 17 * _gui_size, 176, 14, progress, 17);
	}
	item = _furnace->getFuel();
	if (item.type != blocks::air) {
		int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + 51 * _gui_size;
		int y = WIN_HEIGHT / 2 - 30 * _gui_size;
		add_item_value(item, x, y, settings::consts::depth::menu::item_str);
	}
	item = _furnace->getProduction();
	if (item.type != blocks::air) {
		int x = (WIN_WIDTH - (166 * _gui_size)) / 2 + 111 * _gui_size;
		int y = WIN_HEIGHT / 2 - 48 * _gui_size;
		add_item_value(item, x, y, settings::consts::depth::menu::item_str);
	}
}

void Menu::setup_array_buffer_inventory( void )
{
	addQuads(settings::consts::tex::inventory, settings::consts::depth::menu::container, WIN_WIDTH / 2 - 88 * _gui_size, WIN_HEIGHT / 2 - 83 * _gui_size, 176 * _gui_size, 166 * _gui_size, 0, 0, 176, 166);

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

	if (_selected_block.type != blocks::air) {
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		add_item_value(_selected_block, mouseX - 8 * _gui_size, mouseY - 8 * _gui_size, settings::consts::depth::menu::held_str, true);
	}

	setup_shader();
}

void Menu::setup_array_buffer_crafting( void )
{
	addQuads(settings::consts::tex::crafting_table, settings::consts::depth::menu::container, WIN_WIDTH / 2 - 88 * _gui_size, WIN_HEIGHT / 2 - 83 * _gui_size, 176 * _gui_size, 166 * _gui_size, 0, 0, 176, 166);

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

	if (_selected_block.type != blocks::air) {
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		add_item_value(_selected_block, mouseX - 8 * _gui_size, mouseY - 8 * _gui_size, settings::consts::depth::menu::held_str, true);
	}

	setup_shader();
}

void Menu::setup_array_buffer_chest( void )
{
	addQuads(settings::consts::tex::chest, settings::consts::depth::menu::container, WIN_WIDTH / 2 - 88 * _gui_size, WIN_HEIGHT / 2 - 83 * _gui_size, 176 * _gui_size, 166 * _gui_size, 0, 0, 176, 166);

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

	if (_selected_block.type != blocks::air) {
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		add_item_value(_selected_block, mouseX - 8 * _gui_size, mouseY - 8 * _gui_size, settings::consts::depth::menu::held_str, true);
	}

	setup_shader();
}

void Menu::setup_array_buffer_furnace( void )
{
	addQuads(settings::consts::tex::furnace, settings::consts::depth::menu::container, WIN_WIDTH / 2 - 88 * _gui_size, WIN_HEIGHT / 2 - 83 * _gui_size, 176 * _gui_size, 166 * _gui_size, 0, 0, 176, 166);
	occult_selection();
	for (int index = 0; index < 9; index++) {
		add_slot_value(index);
	}
	for (int index = 0; index < 27; index++) {
		add_backpack_value(index);
	}
	add_furnace_value();

	if (_selected_block.type != blocks::air) {
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);
		add_item_value(_selected_block, mouseX - 8 * _gui_size, mouseY - 8 * _gui_size, settings::consts::depth::menu::held_str, true);
	}

	setup_shader();
}

void Menu::setup_shader( void )
{
	_nb_points = _vertices.size();
	if (!_nb_points) {
		return ;
	}

	_vaoSet = true;
	_vabo.uploadData(_nb_points, &_vertices[0][0]);

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
	case menu::main:
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
	case menu::world_select:
		if (_selected_world && inRectangle(posX, posY, WIN_WIDTH / 2 - 155 * _gui_size, WIN_HEIGHT / 2 + 90 * _gui_size, 150 * _gui_size, 20 * _gui_size)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 90 * _gui_size, 150 * _gui_size, 20 * _gui_size)) {
			_selection = 4;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 82 * _gui_size, WIN_HEIGHT / 2 + 115 * _gui_size, 73 * _gui_size, 20 * _gui_size)) {
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
	case menu::world_create:
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 65 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 2;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 3;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = (Settings::Get()->getBool(settings::bools::flat_world)) ? 4 : 0;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 155 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 150 * _gui_size, 20 * _gui_size)) {
			_selection = (_world_file == "") ? 0 : 5;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 150 * _gui_size, 20 * _gui_size)) {
			_selection = 6;
		} else {
			_selection = 0;
		}
		break ;
	case menu::death:
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 - 5 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 2;
		} else {
			_selection = 0;
		}
		break ;
	case menu::pause:
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
	case menu::options:
	case menu::main_options:
		if (_moving_slider) {
			_fov_gradient = gradient(posX, WIN_WIDTH / 2 - 200 * _gui_size, WIN_WIDTH / 2 - 10 * _gui_size, 50, 110);
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 3;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 5;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 6;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 9;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 11;
		} else {
			_selection = 0;
		}
		break ;
	case menu::main_video_settings:
	case menu::video_settings:
		if (_moving_slider) {
			if (_selection == 1) {
				_render_gradient = gradient(posX, WIN_WIDTH / 2 - 200 * _gui_size, WIN_WIDTH / 2 - 10 * _gui_size, 8, 32);
			} else if (_selection == 7) {
				_brightness_gradient = gradient(posX, WIN_WIDTH / 2 - 200 * _gui_size, WIN_WIDTH / 2 - 10 * _gui_size, 0, 1);
			}
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 85 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 2;
		} else if (_drop_down_menu) {
			for (int i = 0; i < Settings::Get()->getInt(settings::ints::available_res); ++i) {
				if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - (65 - 18 * i) * _gui_size, 200 * _gui_size, 18 * _gui_size)) {
					_selection = 70 + i;
					return ;
				}
			}
			_drop_down_menu = false;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 3;
		// } else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 45 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
		// 	_selection = 4;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 5;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 - 20 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 6;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 7;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 5 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 8;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 9;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 30 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 10;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 11;
		} else {
			_selection = 0;
		}
		break ;
	case menu::main_controls:
	case menu::controls:
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT - 30 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 1;
			return ;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT - 30 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 2;
			return ;
		} else if (posX >= WIN_WIDTH / 2 && posX <= WIN_WIDTH / 2 + 90 * _gui_size) {
			// change key
			for (int i = 0; i < 27; ++i) {
				if (posY >= (85 + 25 * i - _scroll) * _gui_size && posY <= (105 + 25 * i - _scroll) * _gui_size) {
					_selection = 3 + i * 2;
					return ;
				}
			}
		} else if (posX >= WIN_WIDTH / 2 + 110 * _gui_size && posX <= WIN_WIDTH / 2 + 200 * _gui_size) {
			// reset key
			for (int i = 0; i < 27; ++i) {
				if (posY >= (85 + 25 * i - _scroll) * _gui_size && posY <= (105 + 25 * i - _scroll) * _gui_size) {
					_selection = 4 + i * 2;
					return ;
				}
			}
		}
		_selection = 0;
		break ;
	case menu::main_resource_packs:
	case menu::resource_packs:
		if (_moving_slider) {
			int cursor_selection = -1, rp_size = _resource_packs.size(), arp_size = _active_resource_packs.size();
			// move selected resource pack around, which in turn moves every other packs accordingly
			if (posX < WIN_WIDTH / 2) { // _resource_packs column
				for (int index = 0; index < rp_size; ++index) {
					if (posY < WIN_HEIGHT / 2 - (80 - index * 20) * _gui_size + 20 * _gui_size) {
						cursor_selection = 3 + index;
						break ;
					}
				}
				if (cursor_selection == -1) {
					cursor_selection = (_selection == 2 + rp_size) ? _selection : 3 + rp_size;
				}
			} else { // _active_resource_packs column
				for (int index = 0; index < arp_size; ++index) {
					if (posY < WIN_HEIGHT / 2 - (80 - index * 20) * _gui_size + 20 * _gui_size) {
						cursor_selection = 4 + rp_size + index;
						break ;
					}
				}
				if (cursor_selection == -1) {
					cursor_selection = (_selection == 3 + rp_size + arp_size) ? _selection : 4 + rp_size + arp_size;
				}
			}
			if (_selection != cursor_selection) {
				std::string change;
				if (_selection < 4 + rp_size) {
					change = _resource_packs[_selection - 3];
					_resource_packs.erase(_resource_packs.begin() + (_selection - 3));
				} else {
					change = _active_resource_packs[_selection - 4 - rp_size];
					_active_resource_packs.erase(_active_resource_packs.begin() + (_selection - 4 - rp_size));
					_change_to_apply = true;
				}
				if (cursor_selection == 3 + rp_size) {
					_resource_packs.push_back(change);
					_selection = cursor_selection;
				} else if (cursor_selection == 4 + rp_size + arp_size) {
					_active_resource_packs.push_back(change);
					_selection = cursor_selection - 1;
					_change_to_apply = true;
				} else if (cursor_selection < 4 + rp_size) {
					_resource_packs.insert(_resource_packs.begin() + (cursor_selection - 3), change);
					_selection = cursor_selection;
				} else {
					_active_resource_packs.insert(_active_resource_packs.begin() + (cursor_selection - 4 - rp_size), change);
					_selection = cursor_selection;
					_change_to_apply = true;
				}
			}
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 205 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 2;
		} else {
			int rp_size = 0;
			for (; rp_size < static_cast<int>(_resource_packs.size()); ++rp_size) {
				if (inRectangle(posX, posY, WIN_WIDTH / 2 - 200 * _gui_size, WIN_HEIGHT / 2 - (80 - rp_size * 20) * _gui_size, 190 * _gui_size, 20 * _gui_size)) {
					_selection = 3 + rp_size;
					return ;
				}
			}
			for (int index = 0; index < static_cast<int>(_active_resource_packs.size()); ++index) {
				if (inRectangle(posX, posY, WIN_WIDTH / 2 + 10 * _gui_size, WIN_HEIGHT / 2 - (80 - index * 20) * _gui_size, 190 * _gui_size, 20 * _gui_size)) {
					_selection = 4 + rp_size + index;
					return ;
				}
			}
			_selection = 0;
		}
		break ;
	case menu::sign:
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 200 * _gui_size, 20 * _gui_size)) {
			_selection = 1;
			return ;
		}
		_selection = 0;
		break ;
	case menu::book_and_quill:
	case menu::book_and_quill_sign:
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 95 * _gui_size, 20 * _gui_size)) {
			_selection = 1;
			return ;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * _gui_size, WIN_HEIGHT / 2 + 65 * _gui_size, 95 * _gui_size, 20 * _gui_size)) {
			_selection = 2;
			return ;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 58 * _gui_size, WIN_HEIGHT / 2 + 23 * _gui_size, 23 * _gui_size, 13 * _gui_size)) {
			_selection = 3;
			return ;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 25 * _gui_size, WIN_HEIGHT / 2 + 23 * _gui_size, 23 * _gui_size, 13 * _gui_size)) {
			_selection = 4;
			return ;
		}
		_selection = 0;
		break ;
	default: // ingame menus
		for (int index = 0; index < 9; index++) {
			if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * index * _gui_size) + _gui_size * 3, WIN_HEIGHT / 2 + 59 * _gui_size, 16 * _gui_size, 16 * _gui_size)) {
				_selection = CELLS::hotbar_first + index + 1;
				return ;
			}
		}
		for (int index = 0; index < 27; index++) {
			if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (index % 9) * _gui_size) + _gui_size * 3, WIN_HEIGHT / 2 + _gui_size + 18 * _gui_size * (index / 9), 16 * _gui_size, 16 * _gui_size)) {
				_selection = CELLS::backpack_first + index + 1;
				return ;
			}
		}
		switch (_state) {
			case menu::inventory:
				for (int index = 0; index < 4; index++) {
					if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (5 + index % 2) * _gui_size) + _gui_size * 3, WIN_HEIGHT / 2 - 65 * _gui_size + 18 * _gui_size * (index / 2), 16 * _gui_size, 16 * _gui_size)) {
						_selection = CELLS::icraft_first + index + 1;
						return ;
					}
				}
				if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + 149 * _gui_size, WIN_HEIGHT / 2 - 55 * _gui_size, 16 * _gui_size, 16 * _gui_size)) {
					_selection = CELLS::product + 1;
					return ;
				}
				break ;
			case menu::crafting:
				for (int index = 0; index < 9; index++) {
					if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (1 + index % 3) * _gui_size) + _gui_size * 7, WIN_HEIGHT / 2 - 66 * _gui_size + 18 * _gui_size * (index / 3), 16 * _gui_size, 16 * _gui_size)) {
						_selection = CELLS::table_craft_first + index + 1;
						return ;
					}
				}
				if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + 119 * _gui_size, WIN_HEIGHT / 2 - 48 * _gui_size, 16 * _gui_size, 16 * _gui_size)) {
					_selection = CELLS::product + 1;
					return ;
				}
				break ;
			case menu::chest:
				for (int index = 0; index < 27; index++) {
					if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + (18 * (index % 9) * _gui_size) + _gui_size * 3, WIN_HEIGHT / 2 - 65 * _gui_size + 18 * _gui_size * (index / 9), 16 * _gui_size, 16 * _gui_size)) {
						_selection = CELLS::chest_first + index + 1;
						return ;
					}
				}
				break ;
			case menu::furnace:
				if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + 51 * _gui_size, WIN_HEIGHT / 2 - 66 * _gui_size, 16 * _gui_size, 16 * _gui_size)) {
					_selection = CELLS::furnace_composant + 1;
					return ;
				}
				if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + 51 * _gui_size, WIN_HEIGHT / 2 - 30 * _gui_size, 16 * _gui_size, 16 * _gui_size)) {
					_selection = CELLS::furnace_fuel + 1;
					return ;
				}
				if (inRectangle(posX, posY, (WIN_WIDTH - (166 * _gui_size)) / 2 + 111 * _gui_size, WIN_HEIGHT / 2 - 48 * _gui_size, 16 * _gui_size, 16 * _gui_size)) {
					_selection = CELLS::product + 1;
					return ;
				}
				break ;
		}
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 88 * _gui_size, WIN_HEIGHT / 2 - 83 * _gui_size, 176 * _gui_size, 166 * _gui_size)) {
			_selection = 0;
			return ;
		}
		_selection = -1;
	}
}

void Menu::setWindow( GLFWwindow *window )
{
	_window = window;
	Settings::Get()->setInt(settings::ints::monitor_width, WIN_WIDTH);
	Settings::Get()->setInt(settings::ints::monitor_height, WIN_HEIGHT);
	Settings::Get()->setInt(settings::ints::available_res, menu::resolutions_size);
	for (int i = 0; i < menu::resolutions_size; ++i) {
		if (WIN_WIDTH < menu::resolutions[i][0] || WIN_HEIGHT < menu::resolutions[i][1]) {
			Settings::Get()->setInt(settings::ints::available_res, i);
			break ;
		}
	}
	loadSettings();

	_vabo.genBuffers();
	_vabo.addAttribute(settings::consts::shader::attributes::specifications, 1, GL_INT);
	_vabo.addAttribute(settings::consts::shader::attributes::position, 2, GL_INT);
}

void Menu::setPtrs( Inventory* inventory, UI* ui )
{
	_inventory = inventory;
	_ui = ui;
	if (ui) {
		_chat = ui->getChatPtr();
		_text = ui->getTextPtr();
	}
}

void Menu::setChunks( std::list<Chunk*>& chunks )
{
	if (_state == menu::load) {
		_chunks = chunks;
	}
}

void Menu::setChestInstance( ChestInstance* chest )
{
	_chest = chest;
}

void Menu::setFurnaceInstance( FurnaceInstance* furnace )
{
	_furnace = furnace;
}

void Menu::setSignPos( glm::ivec3 pos )
{
	_sign_pos = pos;
}

void Menu::setBookContent( std::vector<std::string>* content )
{
	_book_content = content;
	if (content && !content->size()) {
		content->push_back("");
	}
	inputs::setCurrentMessage((content) ? content->at(0) : "");
}

std::vector<t_item> Menu::getDrops( void )
{
	std::vector<t_item> res = _drops;
	_drops.clear();
	return (res);
}


t_item Menu::dropSelectedBlock( bool stack )
{
	if (_selected_block.type == blocks::air) {
		return (t_item());
	}
	t_item res = _selected_block;
	if (stack) {
		_selected_block = {0};
	} else {
		res.amount = 1;
		--_selected_block.amount;
		if (_selected_block.amount <= 0) {
			_selected_block = {0};
		}
	}
	return (res);
}

void Menu::handleScroll( int offset )
{
	if (!(_state == menu::main_controls || _state == menu::controls)) {
		return ;
	}
	_scroll += offset;
	if (_scroll < 0) _scroll = 0;
	if (_scroll > 450) _scroll = 450;
}

void Menu::setState( int state )
{
	_state = state;
	#if !__linux__
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		if (glfwRawMouseMotionSupported()) {
			glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
		}
	#endif
	set_cursor_position_callback(NULL, this);
	set_scroll_callback(NULL);

	switch (state) {
		case menu::chat:
		case menu::sign:
		case menu::book:
		case menu::book_and_quill:
			glfwSetCharCallback(_window, inputs::character_callback);
			break ;
		case menu::command:
			_state = menu::chat;
			glfwSetCharCallback(_window, inputs::character_callback);
			inputs::setCurrentMessage("/");
			break ;
	}
}

int Menu::getState( void )
{
	return (_state);
}

void Menu::changeGuiSize( void )
{
	if (++_gui_size > settings::consts::gui_max()) {
		_gui_size = settings::consts::gui_min;
	}
	_ui->setGuiSize(_gui_size + 1);
}

std::string Menu::getWorldFile( void )
{
	return (_world_file);
}

t_sign_info Menu::getSignContent( void )
{
	t_sign_info res = {_sign_pos, _sign_content};
	_sign_content.clear();
	return (res);
}

menu::ret Menu::run( bool animUpdate )
{
	if (inputs::key_down(inputs::quit_program) && _state != menu::chat && _state != menu::sign && _state != menu::book_and_quill && _state != menu::book_and_quill_sign && !_input_world && !_input_seed) {
		glfwSetWindowShouldClose(_window, GL_TRUE);
		return (menu::ret::quit);
	}

	switch (_state) {
		case menu::main:
			return (main_menu());
		case menu::world_select:
			return (world_select_menu());
		case menu::world_create:
			return (world_create_menu(animUpdate));
		case menu::load:
			return (loading_screen_menu());
		case menu::death:
			return (death_menu());
		case menu::pause:
			return (pause_menu());
		case menu::main_options:
		case menu::options:
			return (options_menu());
		case menu::main_skin_customization:
		case menu::skin_customization:
			return (skin_customization_menu());
		case menu::main_video_settings:
		case menu::video_settings:
			return (video_menu());
		case menu::main_controls:
		case menu::controls:
			return (controls_menu());
		case menu::main_resource_packs:
		case menu::resource_packs:
			return (resource_packs_menu());
		case menu::inventory:
		case menu::crafting:
		case menu::chest:
		case menu::furnace:
			return (ingame_menu());
		case menu::chat:
			return (chat_menu(animUpdate));
		case menu::sign:
			return (sign_menu(animUpdate));
		// case menu::book:
		// 	return (book_menu());
		case menu::book_and_quill:
			return (book_and_quill_menu(animUpdate));
		case menu::book_and_quill_sign:
			return (book_sign_menu(animUpdate));
		default:
			LOGERROR("ERROR defaulting on Menu::run");
			return (menu::ret::back_to_game);
	}
	return (menu::ret::no_change);
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
