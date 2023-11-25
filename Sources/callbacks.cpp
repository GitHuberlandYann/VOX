#include "Camera.hpp"
#include "Menu.hpp"
#include "Inventory.hpp"

# if __linux__
#  if 1
#   define WIN_WIDTH 1680
#   define WIN_HEIGHT 1050
#  else
#   define WIN_WIDTH 1920
#   define WIN_HEIGHT 1080
#  endif
#  define IS_LINUX true
# else
#  define WIN_WIDTH 2560
#  define WIN_HEIGHT 1440
#  define IS_LINUX false
# endif

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