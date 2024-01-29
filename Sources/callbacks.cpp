#include "Camera.hpp"
#include "Menu.hpp"
#include "Inventory.hpp"

Camera *camera = NULL;
Menu *menu = NULL;
Inventory *scroll_inventory = NULL;
std::string message;

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

void character_callback( GLFWwindow* window, unsigned int codepoint )
{
	(void)window;
	if (codepoint < 32 || codepoint > 126) {
		std::cout << __func__ << ": codepoint out of range: " << codepoint << std::endl;
		return ;
	}
	// std::cout << "codepoint you just pressed: " << codepoint << " => " << ALPHABETA[codepoint - 32] << std::endl;
	message += ALPHABETA[codepoint - 32];
}

void resetMessage( void )
{
	message.clear();
}

void rmLetter( void )
{
	message = message.substr(0, message.size() - 1);
}

std::string getCurrentMessage( void )
{
	return (message);
}
