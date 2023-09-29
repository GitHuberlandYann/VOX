#ifndef CALLBACKS_HPP
# define CALLBACKS_HPP

void set_cursor_position_callback( void *ptr );
void set_scroll_callback( void *ptr );
void cursor_position_callback( GLFWwindow* window, double xpos, double ypos );
void scroll_callback( GLFWwindow* window, double xoffset, double yoffset );


#endif
