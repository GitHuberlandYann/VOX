#ifndef UTILS_HPP
# define UTILS_HPP

std::string get_file_content( std::string file_name );
void compile_shader( GLuint ptrShader, std::string name );
void check_glstate( std::string str );
int chunk_pos( int pos );
std::vector<glm::ivec3> voxel_traversal(glm::vec3 ray_start, glm::vec3 ray_end);

#endif
