#include "vox.h"

Camera *camera = NULL;
Menu *menu = NULL;
Inventory *scroll_inventory = NULL;

double lastX = WIN_WIDTH / 2.0f, lastY = WIN_HEIGHT / 2.0f;

void set_cursor_position_callback( void *ptr )
{
	if (!camera) {
		camera = static_cast<Camera *>(ptr);
		menu = NULL;
	} else {
		camera = NULL;
		menu = static_cast<Menu *>(ptr);
	}
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
		menu->processMouseMovement(x_offset / 10, y_offset / 10);
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
		scroll_inventory->setSlot(scroll_inventory->getSlot() + 1);
	} else if (yoffset < 0) {
		scroll_inventory->setSlot(scroll_inventory->getSlot() - 1);
	}
}