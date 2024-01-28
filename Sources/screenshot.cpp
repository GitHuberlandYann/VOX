#include "OpenGL_Manager.hpp"
#include <fstream>

void OpenGL_Manager::screenshot( void )
{
	int *buffer = new int[WIN_WIDTH * WIN_HEIGHT * 3];
	// for (int c = 0; c < WIN_WIDTH * WIN_HEIGHT * 3; ++c)
	// 	buffer[c] = 255;

	glReadPixels(0, 0, WIN_WIDTH, WIN_HEIGHT, GL_RGB, GL_UNSIGNED_INT, buffer);
	// glReadPixels(0, 0, WIN_WIDTH, WIN_HEIGHT, GL_BGR, GL_UNSIGNED_INT, buffer); // gives cool effect

	try {
		std::ofstream ofs(_world_name.substr(0, _world_name.size() - 5) + "_" + DayCycle::Get()->getTime() + ".ppm", std::ofstream::out | std::ofstream::trunc);
		ofs << "P3\n" << WIN_WIDTH << ' ' << WIN_HEIGHT << "\n255\n";
		for (int row = WIN_HEIGHT - 1; row >= 0; --row) {
			for (int col = 0; col < WIN_WIDTH; ++col) {
				ofs << (buffer[(row * WIN_WIDTH + col) * 3] & 0xFF) << ' ' << (buffer[(row * WIN_WIDTH + col) * 3 + 1] & 0xFF) << ' ' << (buffer[(row * WIN_WIDTH + col) * 3 + 2] & 0xFF) << '\n';
			}
		}
		ofs.close();
		std::cout << "Screenshot saved under " << _world_name.substr(0, _world_name.size() - 5) << "_" << DayCycle::Get()->getTime() << ".ppm" << std::endl;
	}
	catch (std::exception & e) {
		std::cerr << e.what() << std::endl << "Screenshot to " << _world_name << ".ppm failure" << std::endl;
	}
	delete [] buffer;
}
