#include <mutex>
#include <iostream>
#include "OpenGL_Manager.hpp"

std::mutex mtx, mtx_perimeter, mtx_deleted_chunks, mtx_backup; // TODO declare those elsewhere
siv::PerlinNoise::seed_type perlin_seed = 123456u;
int WIN_WIDTH = 0, WIN_HEIGHT = 0;

int main( int ac, char **av )
{
	(void)av;
	if (ac != 1) {
		std::cerr << "Error\nFormat required: ./vox" << std::endl;
		for (int index = 0; index < S_BLOCKS_SIZE; index++) {
			delete s_blocks[index];
		}
		return (1);
	}
	std::cout << std::endl << " ---- Hello ----" << std::endl;
	// std::cout << "hardware concurrency of this computer: " << std::thread::hardware_concurrency() << std::endl;

	{
		OpenGL_Manager render;

		render.setup_window();
		std::cout << std::endl;
		render.create_shaders();
		render.setup_communication_shaders();
		render.load_texture();
		std::cout << std::endl;
		render.main_loop();
	}

	for (int index = 0; index < S_BLOCKS_SIZE; index++) {
		delete s_blocks[index];
	}
	std::cout << std::endl << " ---- Goodbye ----" << std::endl;
	// system("leaks vox");
	return (0);
}
