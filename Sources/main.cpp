#include <mutex>
#include <iostream>
#include "OpenGL_Manager.hpp"
#include "Server.hpp"
#include "logs.hpp"

std::mutex mtx, mtx_perimeter, mtx_deleted_chunks, mtx_backup; // TODO declare those elsewhere
siv::PerlinNoise::seed_type perlin_seed = 123456u;
int WIN_WIDTH = 0, WIN_HEIGHT = 0;

int main( int ac, char **av )
{
	(void)av;
	if (ac != 1) {
		LOGERROR("Error\nFormat required: ./vox");
		return (1);
	}

	MAINLOG(LOG("\n ---- Hello ----"));
	// std::cout << "hardware concurrency of this computer: " << std::thread::hardware_concurrency() << std::endl;

	{
		std::unique_ptr<Server> server = nullptr;
		{
			OpenGL_Manager render;
			
			if (render.run()) {
				render.createServer(server);
			}
		}
		if (server) {
			server->run();
		}
	}

	MAINLOG(LOG(std::endl << " ---- Goodbye ----"));
	// system("leaks vox");
	return (0);
}
