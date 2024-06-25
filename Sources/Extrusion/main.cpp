#include <iostream>
#define TEXSIZE 16

#include "SOIL/SOIL.h"
typedef struct {
	unsigned char *content;
	int width;
	int height;
}				t_tex;

/* Program used to extrude items sprites into cubes
   Inputs coords of 16x16 texture sprite we want to extrude from blockAtlas
   Outputs lists of all up-, down-, left-, and right-facing faces in format offsetX, offsetY, size
*/
int main( int ac, char **av )
{
	if (ac != 2) {
		std::cerr << "Expected format: ./extrude <path to file>" << std::endl;
		return (1);
	}

	t_tex img;
	img.content = SOIL_load_image(av[1], &img.width, &img.height, 0, SOIL_LOAD_RGBA);
	if (!img.content) {
		std::cerr << "failed to load image " << av[1] << " because:" << std::endl << SOIL_last_result() << std::endl;
		exit(1);
	}

	bool arr[TEXSIZE * TEXSIZE];
	int index = 0;

	for (int row = 0; row < TEXSIZE; ++row) {
		for (int col = 0; col < TEXSIZE; ++col) {
			unsigned char *pixel = &img.content[(row * img.width + col) * 4];
			arr[index++] = static_cast<int>(pixel[3]) == 255;
			// std::cout << "at pixel " << row << ", " << col << ": R " << static_cast<int>(pixel[0]) << ", G " << static_cast<int>(pixel[1]) << ", B " << static_cast<int>(pixel[2]) << ", A " << static_cast<int>(pixel[3]) << std::endl;
		}
	}

	// display grid
	for (int row = 0; row < TEXSIZE; row++) {
		for (int col = 0; col < TEXSIZE; col++) {
			std::cout << ((arr[row * TEXSIZE + col]) ? "X " : "O ");
		}
		std::cout << std::endl;
	}
	std::cout << " ---" << std::endl;
	// up faces
	std::cout << "{";
	for (int row = 0; row < TEXSIZE; row++) {
		for (int col = 0; col < TEXSIZE; col++) {
			if (!row) {
				if (arr[row * TEXSIZE + col]) {
					int size = 0;
					for (; col + size < TEXSIZE && arr[row * TEXSIZE + col + size]; ++size);
					std::cout << "{" << col << ", " << row << ", " << size << "}, ";
					col += size - 1;
				}
			} else if (arr[row * TEXSIZE + col] && !arr[(row - 1) * TEXSIZE + col]) {
				int size = 0;
				for (; col + size < TEXSIZE && arr[row * TEXSIZE + col + size] && !arr[(row - 1) * TEXSIZE + col + size]; ++size);
				std::cout << "{" << col << ", " << row << ", " << size << "}, ";
				col += size - 1;
			}
		}
	}
	std::cout << "}," << std::endl << "\t\t{";
	// std::cout << " ---" << std::endl;
	// down faces
	for (int row = 0; row < TEXSIZE; row++) {
		for (int col = 0; col < TEXSIZE; col++) {
			if (row == TEXSIZE - 1) {
				if (arr[row * TEXSIZE + col]) {
					int size = 0;
					for (; col + size < TEXSIZE && arr[row * TEXSIZE + col + size]; ++size);
					std::cout << "{" << col << ", " << row << ", " << size << "}, ";
					col += size - 1;
				}
			} else if (arr[row * TEXSIZE + col] && !arr[(row + 1) * TEXSIZE + col]) {
				int size = 0;
				for (; col + size < TEXSIZE && arr[row * TEXSIZE + col + size] && !arr[(row + 1) * TEXSIZE + col + size]; ++size);
				std::cout << "{" << col << ", " << row << ", " << size << "}, ";
				col += size - 1;
			}
		}
	}
	std::cout << "}," << std::endl << "\t\t{";
	// std::cout << " ---" << std::endl;
	// left faces
	for (int col = 0; col < TEXSIZE; col++) {
		for (int row = 0; row < TEXSIZE; row++) {
			if (!col) {
				if (arr[row * TEXSIZE + col]) {
					int size = 0;
					for (; row + size < TEXSIZE && arr[(row + size) * TEXSIZE + col]; ++size);
					std::cout << "{" << col << ", " << row << ", " << size << "}, ";
					row += size - 1;
				}
			} else if (arr[row * TEXSIZE + col] && !arr[row * TEXSIZE + col - 1]) {
				int size = 0;
				for (; row + size < TEXSIZE && arr[(row + size) * TEXSIZE + col] && !arr[(row + size) * TEXSIZE + col - 1]; ++size);
				std::cout << "{" << col << ", " << row << ", " << size << "}, ";
				row += size - 1;
			}
		}
	}
	std::cout << "}," << std::endl << "\t\t{";
	// std::cout << " ---" << std::endl;
	// right faces
	for (int col = 0; col < TEXSIZE; col++) {
		for (int row = 0; row < TEXSIZE; row++) {
			if (col == TEXSIZE - 1) {
				if (arr[row * TEXSIZE + col]) {
					int size = 0;
					for (; row + size < TEXSIZE && arr[(row + size) * TEXSIZE + col]; ++size);
					std::cout << "{" << col << ", " << row << ", " << size << "}, ";
					row += size - 1;
				}
			} else if (arr[row * TEXSIZE + col] && !arr[row * TEXSIZE + col + 1]) {
				int size = 0;
				for (; row + size < TEXSIZE && arr[(row + size) * TEXSIZE + col] && !arr[(row + size) * TEXSIZE + col + 1]; ++size);
				std::cout << "{" << col << ", " << row << ", " << size << "}, ";
				row += size - 1;
			}
		}
	}
	std::cout << "}" << std::endl;

	SOIL_free_image_data(img.content);
	return (0);
}
