#ifndef CALLBACKS_HPP
# define CALLBACKS_HPP

void set_cursor_position_callback( Camera* cam, Menu* men );
void set_scroll_callback( void* ptr );
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

	enum {
		MOVE_FORWARD,
		MOVE_BACKWARD,
		MOVE_RIGHT,
		MOVE_LEFT,
		SNEAK,
		RUN,
		JUMP,
		BREAK,
		SAMPLE,
		USE,
		INVENTORY,
		DROP,
		GAMEMODE,
		NBR_KEYS_PACKET,

		CHAT = NBR_KEYS_PACKET,
		COMMAND,
		CLOSE,
		ENTER,
		DELETE,
		QUIT_PROGRAM = DELETE,
		WIREFRAME,
		BLOCK_HIGHLIGHT,
		INFO,
		HOTBAR,
		SCREENSHOT,
		DEBUG,
		CAMERA,
		DAYCYCLE_UP,
		DAYCYCLE_DOWN,
		FLY_SPEED_UP,
		FLY_SPEED_DOWN,
		LOOK_UP,
		LOOK_DOWN,
		LOOK_RIGHT,
		LOOK_LEFT,
		SLOT_0,
		SLOT_1,
		SLOT_2,
		SLOT_3,
		SLOT_4,
		SLOT_5,
		SLOT_6,
		SLOT_7,
		SLOT_8,
		SLOT_9,

		KEY_SIZE,
	};
}

#endif
