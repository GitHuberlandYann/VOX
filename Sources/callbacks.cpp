#include "Player.hpp"
#include "Menu.hpp"
#include "Inventory.hpp"
#include "callbacks.hpp"
#include "logs.hpp"
#include <fstream>

namespace inputs
{
	Player* playerPtr = NULL;
	Menu* menuPtr = NULL;
	Inventory* scroll_inventory = NULL;

	double lastX = WIN_WIDTH / 2.0f, lastY = WIN_HEIGHT / 2.0f;
	bool backFromMenu = false;

	void set_cursor_position_callback( Player* player, Menu* men )
	{
		playerPtr = player;
		menuPtr = men;
	}

	void centerMouse( void )
	{
		lastX = WIN_WIDTH * .5f;
		lastY = WIN_HEIGHT * .5f;
		backFromMenu = true;
	}

	void set_scroll_callback( Inventory* ptr )
	{
		scroll_inventory = ptr;
	}

	void cursor_position_callback( GLFWwindow* window, double xpos, double ypos )
	{
		(void)window;

		double x_offset = lastX - xpos;
		double y_offset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		if (backFromMenu) {
			backFromMenu = false;
		} else if (playerPtr) {
			playerPtr->processMouseMovement(x_offset / 10, y_offset / 10);
		} else if (menuPtr) {
			menuPtr->processMouseMovement(xpos, ypos);
		}
	}

	void scroll_callback( GLFWwindow* window, double xoffset, double yoffset )
	{
		(void)window;
		(void)xoffset;
		if (!scroll_inventory) {
			if (menuPtr) {
				menuPtr->handleScroll(yoffset * 20);
			}
			return ;
		}

		scroll_inventory->setSlot(scroll_inventory->getSlotNum() + ((yoffset > 0) ? 1 : -1));
	}

	std::string message;
	size_t cursor = 0;

	void character_callback( GLFWwindow* window, unsigned int codepoint )
	{
		(void)window;
		if (codepoint < 32 || codepoint > 126) {
			LOGERROR(__func__ << ": codepoint out of range: " << codepoint);
			return ;
		}
		// std::cout << "codepoint you just pressed: " << codepoint << " => " << static_cast<char>(codepoint) << std::endl;
		if (cursor == message.size()) {
			message += static_cast<char>(codepoint);
		} else {
			message = message.substr(0, cursor) + static_cast<char>(codepoint) + message.substr(cursor);
		}
		++cursor;
	}

	void moveCursor( bool right, bool control )
	{
		if (!right && !cursor) {
			return ;
		}
		cursor += (right) ? 1 : -1;
		if (cursor > message.size()) {
			cursor = message.size();
		} else if (control && message[cursor] != ' ' && message[cursor] != '\n') {
			moveCursor(right, control);
		}
	}

	void resetMessage( void )
	{
		message.clear();
		cursor = 0;
	}

	void rmLetter( void )
	{
		if (!cursor) return ;
		message = message.substr(0, cursor - 1) + message.substr(cursor);
		--cursor;
	}

	void addLetter( char c )
	{
		message = message.substr(0, cursor) + c + message.substr(cursor);
		++cursor;
	}

	std::string getCurrentMessage( void )
	{
		return (message);
	}

	std::string getCurrentInputStr( char c )
	{
		return (message.substr(0, cursor) + c + message.substr(cursor));
	}

	void setCurrentMessage( std::string str )
	{
		message = str;
		cursor = str.size();
	}

	// ************************************************************************** //
	//                               Key inputs                                   //
	// ************************************************************************** //

	int last_input = -1;
	std::array<bool, key_size> down = {false};
	std::array<bool, key_size> updated = {false};
	std::map<int, int> key_map = {
		{GLFW_KEY_BACKSPACE, quit_program},
		{GLFW_KEY_ENTER, enter},
		{GLFW_KEY_ESCAPE, close},
		{GLFW_KEY_E, inventory},
		{GLFW_KEY_T, chat},
		{GLFW_KEY_SLASH, command},
		{GLFW_KEY_F2, screenshot},
		{GLFW_KEY_F3, debug},
		{GLFW_KEY_G, gamemode},
		{GLFW_KEY_F5, perspective},
		{GLFW_KEY_F1, hotbar},
		{GLFW_KEY_O, block_highlight},
		{GLFW_KEY_C, zoom},
		{GLFW_KEY_I, info},
		{GLFW_KEY_RIGHT_BRACKET, daycycle_up},
		{GLFW_KEY_LEFT_BRACKET, daycycle_down},
		{GLFW_MOUSE_BUTTON_LEFT, destroy},
		{GLFW_MOUSE_BUTTON_MIDDLE, sample},
		{GLFW_MOUSE_BUTTON_RIGHT, use},
		{GLFW_KEY_Q, drop},
		{GLFW_KEY_F, wireframe},
		{GLFW_KEY_W, move_forwards},
		{GLFW_KEY_S, move_backwards},
		{GLFW_KEY_D, move_right},
		{GLFW_KEY_A, move_left},
		{GLFW_KEY_SPACE, jump},
		{GLFW_KEY_LEFT_SHIFT, sneak},
		{GLFW_KEY_UP, look_up},
		{GLFW_KEY_DOWN, look_down},
		{GLFW_KEY_RIGHT, look_right},
		{GLFW_KEY_LEFT, look_left},
		{GLFW_KEY_LEFT_CONTROL, run},
		{GLFW_KEY_SPACE, jump},
		{GLFW_KEY_KP_ADD, fly_speed_up},
		{GLFW_KEY_KP_SUBTRACT, fly_speed_down},
		{GLFW_KEY_1, slot_0},
		{GLFW_KEY_2, slot_1},
		{GLFW_KEY_3, slot_2},
		{GLFW_KEY_4, slot_3},
		{GLFW_KEY_5, slot_4},
		{GLFW_KEY_6, slot_5},
		{GLFW_KEY_7, slot_6},
		{GLFW_KEY_8, slot_7},
		{GLFW_KEY_9, slot_8},
	}, initial_key_map = key_map;

	void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
	{
		(void)window;(void)scancode;(void)mods;
		if (action == GLFW_REPEAT) return ;

		if (action == GLFW_PRESS) {
			last_input = key;
		}
		if (key == GLFW_KEY_LEFT_SHIFT) {
			down[left_shift] = (action == GLFW_PRESS);
			updated[left_shift] = true;
		} else if (key == GLFW_KEY_LEFT_CONTROL) {
			down[left_control] = (action == GLFW_PRESS);
			updated[left_control] = true;
		}

		auto search = key_map.find(key);
		if (search == key_map.end()) {
			// std::cout << "couldn't find key " << key << " in key_map" << std::endl;
			return ;
		}
		// std::cout << "found key " << key << " in key_map" << std::endl;

		down[search->second] = (action == GLFW_PRESS);
		updated[search->second] = true;
	}

	void mouse_button_callback( GLFWwindow *window, int button, int action, int mods )
	{
		(void)window;(void)mods;

		if (action == GLFW_PRESS) {
			last_input = button;
		}
		if (button == GLFW_MOUSE_BUTTON_1) {
			down[left_click] = (action == GLFW_PRESS);
			updated[left_click] = true;
		} else if (button == GLFW_MOUSE_BUTTON_2) {
			down[right_click] = (action == GLFW_PRESS);
			updated[right_click] = true;
		}

		auto search = key_map.find(button);
		if (search == key_map.end()) return ;

		down[search->second] = (action == GLFW_PRESS);
		updated[search->second] = true;
	}

	/**
	 * @brief get state of key with given index
	 * @param index index of key you want to query the state of
	 * @return true if key is currently down
	*/
	bool key_down( int index )
	{
		if (index < 0 || index >= key_size) return (false);
		return (down[index]);
	}

	/**
	 * @brief tells if key has been updated since last time you called this function
	 * if (key_down(KEY_E) && key_updated(KEY_E)) tells you if it's the first frame the key has been pressed
	 * @param index index of key you want to query the upate fo
	 * @return true if key has been updated since last time you called this function
	*/
	bool key_update( int index )
	{
		if (index < 0 || index >= key_size) return (false);
		bool res = updated[index];
		if (res) updated[index] = false;
		return (res);
	}

	/**
	 * @brief find action connected to key and forcefully reset its update status
	 */
	void force_reset_key_update( int key )
	{
		auto search = key_map.find(key);
		if (search == key_map.end()) return ;

		updated[search->second] = false;
	}

	/**
	 * @brief find key binded to given action and return its string representation
	 * @param action one of enum values
	 */
	const char* get_key_name( int action )
	{
		if (action < 0 || action >= key_size) {
			return ("Out of bounds.");
		}
		auto result = std::find_if(key_map.begin(), key_map.end(),
				[action](auto& it) {return (it.second == action); });

		if (result == key_map.end()) {
			return ("NONE");
		}
		int key = result->first;
		// std::cout << "Key found in map: " << key << " for action " << action << std::endl;
		if (key <= GLFW_MOUSE_BUTTON_LAST) {
			std::string tmp("BUTTON " + std::to_string(key + 1));
			const char* res = tmp.c_str();
			return (res);
		}
		const char* res = glfwGetKeyName(key, 0);
		if (!res) {
			switch (key) {
				case GLFW_KEY_SPACE:
					return ("SPACE");
				case GLFW_KEY_DELETE:
					return ("DELETE");
				case GLFW_KEY_LEFT_CONTROL:
					return ("LCONTROL");
				case GLFW_KEY_RIGHT_CONTROL:
					return ("RCONTROL");
				case GLFW_KEY_LEFT_SHIFT:
					return ("LSHIFT");
				case GLFW_KEY_RIGHT_SHIFT:
					return ("RSHIFT");
				case GLFW_KEY_LEFT_ALT:
					return ("LALT");
				case GLFW_KEY_RIGHT_ALT:
					return ("RALT");
				case GLFW_KEY_TAB:
					return ("TAB");
				case GLFW_KEY_CAPS_LOCK:
					return ("CAPS_LOCK");
				case GLFW_KEY_NUM_LOCK:
					return ("NUM_LOCK");
				case GLFW_KEY_HOME:
					return ("HOME");
				case GLFW_KEY_END:
					return ("END");
				case GLFW_KEY_PAGE_UP:
					return ("PAGE_UP");
				case GLFW_KEY_PAGE_DOWN:
					return ("PAGE_DOWN");
				case GLFW_KEY_KP_ENTER:
					return ("KP_ENTER");
				default:
					return ("NULL");
			}
		}
		return (res);
	}

	/**
	 * @brief update key_map to pair key and action
	 * if key already used for another action, this does nothing
	 */
	void set_key_bind( int key, int action )
	{
		if (action < 0 || action > key_size) return ;

		auto search = key_map.find(key);
		if (search != key_map.end()) return ; // key already being used

		auto result = std::find_if(key_map.begin(), key_map.end(),
				[action](auto& it) {return (it.second == action); });
		if(result != key_map.end()) { // unset previous bind
			key_map.erase(result);
		}

		key_map[key] = action; // bind new key
	}

	/**
	 * @brief checks if key binded to given action is initial key bind
	 */
	bool is_initial_key_bind( int action )
	{
		auto current_bind = std::find_if(key_map.begin(), key_map.end(),
							[action](auto& it) {return (it.second == action); });
		if (current_bind == key_map.end()) {
			LOGERROR("inputs::is_initial_key_bind action not found in key_map");
			return (false);
		}
		auto initial_bind = std::find_if(initial_key_map.begin(), initial_key_map.end(),
							[action](auto& it) {return (it.second == action); });
		if (initial_bind == initial_key_map.end()) {
			LOGERROR("inputs::is_initial_key_bind action not found in initial_key_map");
			return (false);
		}

		return (current_bind->first == initial_bind->first);
	}

	void reset_key_binds( void )
	{
		key_map = initial_key_map;
	}

	void reset_key_bind( int action )
	{
		auto current_bind = std::find_if(key_map.begin(), key_map.end(),
							[action](auto& it) {return (it.second == action); });
		if (current_bind != key_map.end()) {
			key_map.erase(current_bind); // erase previous key
		}
		auto initial_bind = std::find_if(initial_key_map.begin(), initial_key_map.end(),
							[action](auto& it) {return (it.second == action); });
		if (initial_bind == initial_key_map.end()) {
			LOGERROR("inputs::reset_key_bind action not found in initial_key_map");
			return ;
		}

		key_map[initial_bind->first] = initial_bind->second; // bind new key
	}

	/**
	 * @brief set last_input to -1, used to change key binds from controls menu
	 */
	void reset_last_input( void )
	{
		last_input = -1;
	}

	/**
	 * @brief return last_input, used to change key binds from controls menu
	 */
	int get_last_input( void )
	{
		return (last_input);
	}

	/**
	 * @brief return string containing bindings settings
	 */
	std::string saveBindings( void )
	{
		std::string res = "\"bindings\": {";

		bool start = true;
		for (auto bind : key_map) {
			if (!start) {
				res += ", ";
			}
			res += '\"' + std::to_string(bind.first) + "\": " + std::to_string(bind.second);
			start = false;
		}
		res += '}';
		return (res);
	}

	/**
	 * @brief load bindings from string containing bindings settings
	 */
	void loadBindings( std::ofstream& ofs, std::string str )
	{
		key_map.clear();
		size_t index = 13;
		while (str[index] == '\"') {
			int key = std::atoi(&str[index + 1]);
			for (index = index + 3; str[index] != ':'; ++index);
			int action = std::atoi(&str[index + 2]);
			for (index = index + 3; str[index] && str[index] != '\"'; ++index);
			key_map[key] = action;
			ofs << "binding key " << key << " to action " << action << std::endl;
		}
	}
}
