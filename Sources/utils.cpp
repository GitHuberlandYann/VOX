#include "vox.h"

std::string get_file_content( std::string file_name )
{
	std::ifstream indata (file_name.c_str());
	if (!indata.is_open()) {
		std::cerr << "Error\nCould not open " << file_name << std::endl;
		exit(1);
	}
	std::string data;
	std::string line;
	while (!indata.eof()) {
		std::getline( indata, line );
		data += line;
		if (!indata.eof())
			data += '\n';
	}
	indata.close();
	return (data);
}
