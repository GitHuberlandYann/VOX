#ifndef UTILS_HPP
# define UTILS_HPP

class Chunk;

std::string get_file_content( std::string file_name );
std::string	trim_spaces( std::string str );
void compile_shader( GLuint ptrShader, std::string name );
void check_glstate( std::string str );
int chunk_pos( int pos );
double gradient( double value, double start, double end, double value_start, double value_end );
int blockAtlasX( int block );
int blockAtlasY( int block );
std::list<Chunk *> sort_chunks( glm::vec3 pos, std::list<Chunk *> chunks );
void face_vertices( GLint *vertices, glm::ivec3 v0, glm::ivec3 v1, glm::ivec3 v2, glm::ivec3 v3, int & vindex );
std::vector<glm::ivec3> voxel_traversal(glm::vec3 ray_start, glm::vec3 ray_end);

#endif
