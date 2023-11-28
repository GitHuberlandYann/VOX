#include <mutex>
#include <iostream>
#include "OpenGL_Manager.hpp"

std::mutex mtx, mtx_perimeter, mtx_deleted_chunks, mtx_inventory, mtx_backup; // TODO declare those elsewhere
siv::PerlinNoise::seed_type perlin_seed = 123456u;

int main( int ac, char **av )
{
	if (ac > 2) {
		std::cerr << "Error\nFormat required: ./vox [path-to-texture]" << std::endl;
		for (int index = 0; index < S_BLOCKS_SIZE; index++) {
			delete s_blocks[index];
		}
		return (1);
	}
	std::cout << std::endl << " ---- Hello ----" << std::endl;
	// std::cout << "hardware concurrency of this computer: " << std::thread::hardware_concurrency() << std::endl;

	OpenGL_Manager *render = new OpenGL_Manager();
	std::string texture_file = (ac == 2) ? av[1] : "Resources/blockAtlas.png";

	render->setup_window();
	std::cout << std::endl;
	render->create_shaders();
	render->setup_communication_shaders();
	render->load_texture(texture_file);
	std::cout << std::endl;
	render->main_loop();

	delete render;
	for (int index = 0; index < S_BLOCKS_SIZE; index++) {
		delete s_blocks[index];
	}
	std::cout << std::endl << " ---- Goodbye ----" << std::endl;
	return (0);
}
