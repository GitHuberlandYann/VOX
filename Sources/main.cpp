#include "vox.h"

int main( int ac, char **av )
{
	if (ac > 1) {
		std::cerr << "vox doesn't take any parameter for now" << std::endl;
		(void) av;
		return (1);
	}
	std::cout << "Hello wolrd !" << std::endl;
	return (0);
}
