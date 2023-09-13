#ifndef VOX_H
# define VOX_H

# include <iostream>
# include <string>
// # include <thread>
# include <fstream>
// # include <vector>
// # include <math.h>

# define GLEW_STATIC
# include <GL/glew.h> // must be before glfw
# include "GLFW/glfw3.h"
# include "SOIL/SOIL.h"
# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>

typedef struct {
	unsigned char *content;
	int width;
	int height;
}				t_tex;

# include "utils.h"
# include "OpenGL_Manager.hpp"

#endif
