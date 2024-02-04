#ifndef CALLBACKS_HPP
# define CALLBACKS_HPP

void set_cursor_position_callback( Camera *cam, Menu *men );
void set_scroll_callback( void *ptr );
void cursor_position_callback( GLFWwindow* window, double xpos, double ypos );
void scroll_callback( GLFWwindow* window, double xoffset, double yoffset );
void character_callback(GLFWwindow* window, unsigned int codepoint);

namespace INPUT
{
	void character_callback( GLFWwindow* window, unsigned int codepoint );
	void moveCursor( bool right, bool control );
	void resetMessage( void );
	void rmLetter( void );
	std::string getCurrentMessage( void );
	std::string getCurrentInputStr( char c );
	void setCurrentMessage( std::string str );
}

#endif
