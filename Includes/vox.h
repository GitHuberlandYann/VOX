#ifndef VOX_H
# define VOX_H

# include <iostream>
# include <string>
# include <thread>
# include <mutex>
# include <atomic>
# include <fstream>
# include <list>
# include <map>
# include <set>
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
extern std::mutex mtx_perimeter;
extern std::mutex mtx_deleted_chunks;
extern std::mutex mtx_inventory;
extern std::mutex mtx_backup;

# include "utils.h"
# include "Benchmark.hpp"
# include "DayCycle.hpp"
# include "Camera.hpp"
# include "PerlinNoise.hpp"
# include "Blocks.hpp"
# include "Entity.hpp"
# include "FurnaceInstance.hpp"
# include "Biomes.hpp"
# include "Inventory.hpp"
# include "Chunk.hpp"
# include "Text.hpp"
# include "Ui.hpp"
# include "Menu.hpp"
# include "callbacks.hpp"
# include "OpenGL_Manager.hpp"

extern siv::PerlinNoise::seed_type perlin_seed;

#endif
