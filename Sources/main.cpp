#include "vox.h"

std::mutex mtx;
std::mutex mtx_visible_chunks;
std::mutex mtx_delete_chunks;
std::mutex mtx_inventory;
siv::PerlinNoise::seed_type perlin_seed = 123456u;

int main( int ac, char **av )
{
	if (ac >= 3 && std::string(av[1]) != "-seed") {
		std::cerr << "Error\nFormat required: ./vox [path-to-texture]" << std::endl;
		return (1);
	} else if (ac == 3) {
		const auto p1 = std::chrono::system_clock::now(); // works without #include <chrono> #include <ctime> ?
		perlin_seed = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();
		std::cout << "Seed is " << perlin_seed << std::endl;
	}
	std::cout << std::endl << " ---- Hello ----" << std::endl;


	OpenGL_Manager *render = new OpenGL_Manager();
	std::string texture_file = (ac == 2) ? av[1] : "Resources/blockAtlas.png";

	render->setup_window();
	render->initWorld();
	std::cout << std::endl;
	render->create_shaders();
	render->setup_communication_shaders();
	render->load_texture(texture_file);
	std::cout << std::endl;
	render->main_loop();

	delete render;
	std::cout << std::endl << " ---- Goodbye ----" << std::endl;
	return (0);
}
