#include "Camera.hpp"
#include "Menu.hpp"
#include "Inventory.hpp"
#include "callbacks.hpp"

Camera* cameraPtr = NULL;
Menu* menuPtr = NULL;
Inventory* scroll_inventory = NULL;

double lastX = WIN_WIDTH / 2.0f, lastY = WIN_HEIGHT / 2.0f;

void set_cursor_position_callback( Camera* cam, Menu* men )
{
	cameraPtr = cam;
	menuPtr = men;
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

	if (cameraPtr) {
		cameraPtr->processMouseMovement(x_offset / 10, y_offset / 10);
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

namespace inputs
{
	std::string message;
	size_t cursor = 0;

	void character_callback( GLFWwindow* window, unsigned int codepoint )
	{
		(void)window;
		if (codepoint < 32 || codepoint > 126) {
			std::cout << __func__ << ": codepoint out of range: " << codepoint << std::endl;
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
		} else if (control && message[cursor] != ' ') {
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
	};

	void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
	{
		(void)window;(void)scancode;(void)mods;
		if (action == GLFW_REPEAT) return ;

		auto search = key_map.find(key);
		if (search == key_map.end()) {
			// std::cout << "couldn't find key " << key << " in key_map" << std::endl;
			return ;
		}
		// std::cout << "found key " << key << " in key_map" << std::endl;

		down[search->second] = action == GLFW_PRESS;
		updated[search->second] = true;
	}

	void mouse_button_callback( GLFWwindow *window, int button, int action, int mods )
	{
		(void)window;(void)mods;

		auto search = key_map.find(button);
		if (search == key_map.end()) return ;

		down[search->second] = action == GLFW_PRESS;
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
	 * @brief find key binded to given action and return its string representation
	 * @param action one of enum values
	 */
	const char* get_key_name( int action )
	{
		if (action < 0 || action >= key_size) {
			return ("Out of bounds.");
		}
		auto result = std::find_if(
				key_map.begin(),
				key_map.end(),
				[action](auto& it) {return (it.second == action); });

		if(result == key_map.end()) {
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
				case GLFW_KEY_LEFT_CONTROL:
					return ("LCONTROL");
				case GLFW_KEY_RIGHT_CONTROL:
					return ("RCONTROL");
				case GLFW_KEY_LEFT_SHIFT:
					return ("LSHIFT");
				case GLFW_KEY_RIGHT_SHIFT:
					return ("RSHIFT");
				default:
					return ("NULL");
			}
		}
		return (res);
	}
}
