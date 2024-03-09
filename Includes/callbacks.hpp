#ifndef CALLBACKS_HPP
# define CALLBACKS_HPP

void set_cursor_position_callback( Camera *cam, Menu *men );
void set_scroll_callback( void *ptr );
void cursor_position_callback( GLFWwindow* window, double xpos, double ypos );
void scroll_callback( GLFWwindow* window, double xoffset, double yoffset );

namespace INPUT
{
	void character_callback( GLFWwindow* window, unsigned int codepoint );
	void moveCursor( bool right, bool control );
	void resetMessage( void );
	void rmLetter( void );
	std::string getCurrentMessage( void );
	std::string getCurrentInputStr( char c );
	void setCurrentMessage( std::string str );

	// key callback
	void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods );
	void mouse_button_callback( GLFWwindow *window, int button, int action, int mods );
	bool key_down( int index );
	bool key_update( int index );


	const int MOVE_FORWARD     = 0;
	const int MOVE_BACKWARD    = 1;
	const int MOVE_RIGHT       = 2;
	const int MOVE_LEFT        = 3;
	const int SNEAK            = 4;
	const int RUN              = 5;
	const int JUMP             = 6;
	const int BREAK            = 7;
	const int SAMPLE           = 8;
	const int USE              = 9;
	const int INVENTORY        = 10;
	const int DROP             = 11;
	const int GAMEMODE         = 12;
	const int NBR_KEYS_PACKET  = 13;
	// from here, we don't send to server
	const int CHAT             = 13;
	const int CLOSE            = 14;
	const int ENTER            = 15;
	const int DELETE           = 16;
	const int QUIT_PROGRAM     = 16;
	const int WIREFRAME        = 17;
	const int BLOCK_HIGHLIGHT  = 18;
	const int HOTBAR           = 19;
	const int SCREENSHOT       = 20;
	const int DEBUG            = 21;
	const int CAMERA           = 22;
	const int DAYCYCLE_UP      = 23;
	const int DAYCYCLE_DOWN    = 24;
	const int RENDER_DIST_UP   = 25;
	const int RENDER_DIST_DOWN = 26;
	const int FLY_SPEED_UP     = 27;
	const int FLY_SPEED_DOWN   = 28;
	const int GUI_UP           = 29;
	const int GUI_DOWN         = 30;
	const int LOOK_UP          = 31;
	const int LOOK_DOWN        = 32;
	const int LOOK_RIGHT       = 33;
	const int LOOK_LEFT        = 34;
	const int SLOT_0           = 35;
	const int SLOT_1           = 36;
	const int SLOT_2           = 37;
	const int SLOT_3           = 38;
	const int SLOT_4           = 39;
	const int SLOT_5           = 40;
	const int SLOT_6           = 41;
	const int SLOT_7           = 42;
	const int SLOT_8           = 43;
	const int SLOT_9           = 44;

	const int KEY_SIZE         = 45;
}

#endif
