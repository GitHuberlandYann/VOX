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

void check_glstate( std::string str )
{
	GLenum error_check = glGetError();	
	if (error_check) {
		std::cerr << "glGetError set to " << error_check << ", quitting now" << std::endl;
		exit(1);
	}
	if (str == "NO") {
		return ;
	}
	std::cout << str << std::endl;
}

// void *thread_create_chunk(void *arg)
// {
// 	if (!arg) {
// 		return (NULL);
// 	}
// 	std::list<Chunk *> *_chunks = arg;
// 	std::cout << "hello from thread :)" << std::endl;
// 	return (NULL);
// }
