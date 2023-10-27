#ifndef UTILS_HPP
# define UTILS_HPP

class Chunk;

std::string get_file_content( std::string file_name );
std::string	trim_spaces( std::string str );
void compile_shader( GLuint ptrShader, std::string name );
void check_glstate( std::string str );
int chunk_pos( int pos );
double gradient( double value, double start, double end, double value_start, double value_end );
float dist2( glm::vec3 pa, glm::vec3 pb );
int blockGridX( int block, int offset );
int blockGridY( int block );
int blockAtlasX( int block );
int blockAtlasY( int block );
int air_flower( int value, bool air_leaves, bool air_water );
bool isSandOrGravel( int type );
std::vector<Chunk *> sort_chunks( glm::vec3 pos, std::vector<Chunk *> chunks );
std::array<int, 5> compute_texcoord_offsets( int o0, int o1, int o2, int o3 );
void face_vertices( GLint *vertices, glm::ivec4 v0, glm::ivec4 v1, glm::ivec4 v2, glm::ivec4 v3, size_t & vindex );
bool torchFace( GLint *vertices, glm::ivec3 v0, glm::ivec3 v1, glm::ivec3 v2, glm::ivec3 v3, glm::ivec3 v4, glm::ivec3 v6, size_t index );
bool crossFace( GLint *vertices, glm::ivec3 v0, glm::ivec3 v1, glm::ivec3 v2, glm::ivec3 v3, glm::ivec3 v4, glm::ivec3 v5, size_t index );
bool blockFace( GLint *vertices, glm::ivec3 v0, glm::ivec3 v1, glm::ivec3 v2, glm::ivec3 v3, glm::ivec3 v4, glm::ivec3 v5, glm::ivec3 v6, glm::ivec3 v7, size_t index );
std::vector<glm::ivec3> voxel_traversal( glm::vec3 ray_start, glm::vec3 ray_end );

#endif
