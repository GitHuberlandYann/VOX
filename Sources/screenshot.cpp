#include "OpenGL_Manager.hpp"
#include "logs.hpp"
#include <fstream>

#if __linux__
# define SHOT_WIDTH WIN_WIDTH
# define SHOT_HEIGHT WIN_HEIGHT
#else
# define SHOT_WIDTH WIN_WIDTH * 2
# define SHOT_HEIGHT WIN_HEIGHT * 2
#endif

void OpenGL_Manager::screenshot( void )
{
	std::vector<GLubyte> pixels;
	pixels.reserve(SHOT_WIDTH * SHOT_HEIGHT * 3);

	glReadPixels(0, 0, SHOT_WIDTH, SHOT_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, &pixels[0]);
	// glReadPixels(0, 0, SHOT_WIDTH, SHOT_HEIGHT, GL_BGR, GL_UNSIGNED_BYTE, pixels); // gives cool effect

	try {
		std::string file = "Resources/Screenshots/" + _world_name.substr(0, _world_name.size() - 5) + "_" + DayCycle::Get()->getTime() + ".ppm";
		std::ofstream ofs(file, std::ofstream::out | std::ofstream::trunc);
		ofs << "P3\n" << SHOT_WIDTH << ' ' << SHOT_HEIGHT << "\n255\n";
		for (int row = SHOT_HEIGHT - 1; row >= 0; --row) {
			for (int col = 0; col < SHOT_WIDTH; ++col) {
				ofs << (pixels[(row * SHOT_WIDTH + col) * 3] & 0xFF) << ' ' << (pixels[(row * SHOT_WIDTH + col) * 3 + 1] & 0xFF) << ' ' << (pixels[(row * SHOT_WIDTH + col) * 3 + 2] & 0xFF) << '\n';
			}
		}
		ofs.close();
		_ui->chatMessage("Screenshot saved under " + file);
		MAINLOG(LOG("Screenshot saved under " << file));
	}
	catch (std::exception & e) {
		LOGERROR(e.what() << std::endl << "Screenshot to " << _world_name << ".ppm failure");
	}
}
