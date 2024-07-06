#ifndef CALLBACKS_HPP
# define CALLBACKS_HPP

class Menu;

namespace inputs
{
	// mouse callback
	void set_cursor_position_callback( Player* player, Menu* men );
	void centerMouse( void );
	void set_scroll_callback( Inventory* ptr );
	void cursor_position_callback( GLFWwindow* window, double xpos, double ypos );
	void scroll_callback( GLFWwindow* window, double xoffset, double yoffset );

	// character callback
	void character_callback( GLFWwindow* window, unsigned int codepoint );
	void moveCursor( bool right, bool control );
	void resetMessage( void );
	void rmLetter( void );
	void addLetter( char c );
	int getCursor( void );
	std::string getCurrentMessage( void );
	void setCurrentMessage( std::string str );

	// key callback
	void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods );
	void mouse_button_callback( GLFWwindow *window, int button, int action, int mods );
	bool key_down( int index );
	bool key_update( int index );
	void force_reset_key_update( int key );
	const char* get_key_name( int action );
	void set_key_bind( int key, int action );
	bool is_initial_key_bind( int action );
	void reset_key_binds( void );
	void reset_key_bind( int action );
	void reset_last_input( void );
	int get_last_input( void );
	std::string saveBindings( void );
	void loadBindings( std::ofstream& ofs, std::string str );

	enum {
		move_forwards,
		move_backwards,
		move_right,
		move_left,
		sneak,
		run,
		jump,
		destroy,
		sample,
		use,
		inventory,
		drop,
		gamemode,
		NBR_KEYS_PACKET,

		chat = NBR_KEYS_PACKET,
		command,
		close,
		enter,
		del,
		quit_program = del,
		wireframe,
		block_highlight,
		zoom,
		info,
		hotbar,
		screenshot,
		debug,
		perspective,
		daycycle_up,
		daycycle_down,
		fly_speed_up,
		fly_speed_down,
		look_up,
		look_down,
		look_right,
		look_left,
		slot_0,
		slot_1,
		slot_2,
		slot_3,
		slot_4,
		slot_5,
		slot_6,
		slot_7,
		slot_8,
		left_click,
		right_click,
		left_shift,
		left_control,

		key_size,
	};
}

#endif
