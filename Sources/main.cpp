#include <mutex>
#include <iostream>
#include "OpenGL_Manager.hpp"

std::mutex mtx, mtx_perimeter, mtx_deleted_chunks, mtx_backup; // TODO declare those elsewhere
siv::PerlinNoise::seed_type perlin_seed = 123456u;
int WIN_WIDTH = 0, WIN_HEIGHT = 0;
// #include "Settings.hpp"

int main( int ac, char **av )
{
	(void)av;
	if (ac != 1) {
		std::cerr << "Error\nFormat required: ./vox" << std::endl;
		return (1);
	}
	/*std::cout << "sizeof(Block) " << sizeof(Block) << std::endl;
	std::cout << "sizeof(OpenGL_Manager) " << sizeof(OpenGL_Manager) << std::endl;
	std::cout << "sizeof(Chunk) " << sizeof(Chunk) << std::endl;
	std::cout << "sizeof(Menu) " << sizeof(Menu) << std::endl;
	std::cout << "sizeof(UI) " << sizeof(UI) << std::endl;
	std::cout << "sizeof(Inventory) " << sizeof(Inventory) << std::endl;
	std::cout << "sizeof(Camera) " << sizeof(Camera) << std::endl;
	std::cout << "sizeof(Chat) " << sizeof(Chat) << std::endl;
	std::cout << "sizeof(Text) " << sizeof(Text) << std::endl;
	std::cout << "sizeof(ChestInstance) " << sizeof(ChestInstance) << std::endl;
	std::cout << "sizeof(DayCycle) " << sizeof(DayCycle) << std::endl;
	std::cout << "sizeof(Entity) " << sizeof(Entity) << std::endl;
	std::cout << "sizeof(FurnaceInstance) " << sizeof(FurnaceInstance) << std::endl;
	std::cout << "sizeof(Particle) " << sizeof(Particle) << std::endl;
	std::cout << "sizeof(Settings) " << sizeof(Settings) << std::endl;
	std::cout << "sizeof(SignInstance) " << sizeof(SignInstance) << std::endl;
	std::cout << "sizeof(Skybox) " << sizeof(Skybox) << std::endl;
	return 0;  // */
	std::cout << std::endl << " ---- Hello ----" << std::endl;
	// std::cout << "hardware concurrency of this computer: " << std::thread::hardware_concurrency() << std::endl;

	{
		OpenGL_Manager render;

		render.setupWindow();
		std::cout << std::endl;
		render.createShaders();
		render.setupCommunicationShaders();
		render.loadTextures();
		std::cout << std::endl;
		render.main_loop();
	}

	std::cout << std::endl << " ---- Goodbye ----" << std::endl;
	// system("leaks vox");
	return (0);
}
