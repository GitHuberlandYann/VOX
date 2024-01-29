#ifndef CALLBACKS_HPP
# define CALLBACKS_HPP

void set_cursor_position_callback( Camera *cam, Menu *men );
void set_scroll_callback( void *ptr );
void cursor_position_callback( GLFWwindow* window, double xpos, double ypos );
void scroll_callback( GLFWwindow* window, double xoffset, double yoffset );
void character_callback(GLFWwindow* window, unsigned int codepoint);
void rmLetter( void );
void resetMessage( void );
std::string getCurrentMessage( void );

#endif
