#include "Camera.hpp"
#include "Menu.hpp"
#include "Inventory.hpp"
#include "callbacks.hpp"

Camera *camera = NULL;
Menu *menu = NULL;
Inventory *scroll_inventory = NULL;

double lastX = WIN_WIDTH / 2.0f, lastY = WIN_HEIGHT / 2.0f;

void set_cursor_position_callback( Camera *cam, Menu *men )
{
	camera = cam;
	menu = men;
}

void set_scroll_callback( void *ptr )
{
	scroll_inventory = static_cast<Inventory *>(ptr);
}

void cursor_position_callback( GLFWwindow* window, double xpos, double ypos )
{
	(void)window;

    double x_offset = lastX - xpos;
    double y_offset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

	if (camera) {
		camera->processMouseMovement(x_offset / 10, y_offset / 10);
	} else if (menu) {
		menu->processMouseMovement(xpos, ypos);
	}
}

void scroll_callback( GLFWwindow* window, double xoffset, double yoffset )
{
	(void)window;
	(void)xoffset;

	if (!scroll_inventory) {
		return ;
	}

	if (yoffset > 0) {
		scroll_inventory->setSlot(scroll_inventory->getSlotNum() + 1);
	} else if (yoffset < 0) {
		scroll_inventory->setSlot(scroll_inventory->getSlotNum() - 1);
	}
}

namespace INPUT
{
	std::string message;
	int cursor = 0;

	void character_callback( GLFWwindow* window, unsigned int codepoint )
	{
		(void)window;
		if (codepoint < 32 || codepoint > 126) {
			std::cout << __func__ << ": codepoint out of range: " << codepoint << std::endl;
			return ;
		}
		// std::cout << "codepoint you just pressed: " << codepoint << " => " << ALPHABETA[codepoint - 32] << std::endl;
		if (cursor == static_cast<int>(message.size())) {
			message += ALPHABETA[codepoint - 32];
		} else {
			message = message.substr(0, cursor) + ALPHABETA[codepoint - 32] + message.substr(cursor);
		}
		++cursor;
	}

	void moveCursor( bool right, bool control )
	{
		cursor += (right) ? 1 : -1;
		if (cursor > static_cast<int>(message.size())) {
			cursor = message.size();
		} else if (cursor < 0) {
			cursor = 0;
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

	std::array<bool, KEY_SIZE> down = {false};
	std::array<bool, KEY_SIZE> updated = {false};
	std::map<int, int> key_map = {
		{GLFW_KEY_BACKSPACE, QUIT_PROGRAM},
		{GLFW_KEY_ENTER, ENTER},
		{GLFW_KEY_ESCAPE, CLOSE},
		{GLFW_KEY_E, INVENTORY},
		{GLFW_KEY_T, CHAT},
		{GLFW_KEY_F2, SCREENSHOT},
		{GLFW_KEY_F3, DEBUG},
		{GLFW_KEY_G, GAMEMODE},
		{GLFW_KEY_F5, CAMERA},
		{GLFW_KEY_F1, HOTBAR},
		{GLFW_KEY_O, BLOCK_HIGHLIGHT},
		{GLFW_KEY_RIGHT_BRACKET, DAYCYCLE_UP},
		{GLFW_KEY_LEFT_BRACKET, DAYCYCLE_DOWN},
		{GLFW_MOUSE_BUTTON_LEFT, BREAK},
		{GLFW_MOUSE_BUTTON_MIDDLE, SAMPLE},
		{GLFW_MOUSE_BUTTON_RIGHT, USE},
		{GLFW_KEY_Q, DROP},
		{GLFW_KEY_F, WIREFRAME},
		{GLFW_KEY_EQUAL, RENDER_DIST_UP},
		{GLFW_KEY_MINUS, RENDER_DIST_DOWN},
		{GLFW_KEY_KP_MULTIPLY, GUI_UP},
		{GLFW_KEY_KP_DIVIDE, GUI_DOWN},
		{GLFW_KEY_W, MOVE_FORWARD},
		{GLFW_KEY_S, MOVE_BACKWARD},
		{GLFW_KEY_D, MOVE_RIGHT},
		{GLFW_KEY_A, MOVE_LEFT},
		{GLFW_KEY_SPACE, JUMP},
		{GLFW_KEY_LEFT_SHIFT, SNEAK},
		{GLFW_KEY_UP, LOOK_UP},
		{GLFW_KEY_DOWN, LOOK_DOWN},
		{GLFW_KEY_RIGHT, LOOK_RIGHT},
		{GLFW_KEY_LEFT, LOOK_LEFT},
		{GLFW_KEY_LEFT_CONTROL, RUN},
		{GLFW_KEY_SPACE, JUMP},
		{GLFW_KEY_KP_ADD, FLY_SPEED_UP},
		{GLFW_KEY_KP_SUBTRACT, FLY_SPEED_DOWN},
		{GLFW_KEY_1, SLOT_0},
		{GLFW_KEY_2, SLOT_1},
		{GLFW_KEY_3, SLOT_2},
		{GLFW_KEY_4, SLOT_3},
		{GLFW_KEY_5, SLOT_4},
		{GLFW_KEY_6, SLOT_5},
		{GLFW_KEY_7, SLOT_6},
		{GLFW_KEY_8, SLOT_7},
		{GLFW_KEY_9, SLOT_8},
	};

	void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
	{
		(void)window;(void)scancode;(void)mods;
		if (action == GLFW_REPEAT) return ;

		auto search = key_map.find(key);
		if (search == key_map.end()) return ;

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
		if (index < 0 || index >= KEY_SIZE) return (false);
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
		if (index < 0 || index >= KEY_SIZE) return (false);
		bool res = updated[index];
		if (res) updated[index] = false;
		return (res);
	}
}
