#include "vox.h"

int main( int ac, char **av )
{
	std::string texture_file = "Resources/desert.png";
	if (ac > 3) {
		std::cerr << "vox doesn't take any parameter for now" << std::endl;
		return (1);
	} else if (ac == 2) {
		texture_file = av[1];
	}
	std::cout << std::endl << " ---- Hello ----" << std::endl;

	OpenGL_Manager *render = new OpenGL_Manager();

	render->setup_window();
	render->setup_array_buffer();
	std::cout << std::endl;
	render->create_shaders();
	render->setup_communication_shaders();
	render->load_texture(texture_file);
	std::cout << std::endl;
	render->main_loop();

	delete render;
	// if (provided_tex)
	// 	SOIL_free_image_data(provided_tex->texture);
	// delete provided_tex;

	std::cout << std::endl << " ---- Goodbye ----" << std::endl;
	return (0);
}
