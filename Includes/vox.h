#ifndef VOX_H
# define VOX_H

# include <iostream>
# include <string>
# include <thread>
# include <mutex>
# include <fstream>
# include <list>
# include <random>
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

extern std::mutex mtx;
extern std::mutex mtx_visible_chunks;
extern std::mutex mtx_delete_chunks;
extern std::mutex mtx_inventory;

# include "utils.h"
# include "Camera.hpp"
# include "PerlinNoise.hpp"
# include "Blocks.hpp"
# include "Biomes.hpp"
# include "Inventory.hpp"
# include "Chunk.hpp"
# include "Text.hpp"
# include "Ui.hpp"
# include "OpenGL_Manager.hpp"

extern siv::PerlinNoise::seed_type perlin_seed;

#endif
