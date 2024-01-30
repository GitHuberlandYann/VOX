#ifndef UTILS_HPP
# define UTILS_HPP

# include <vector>
# include <array>
# include <iostream>

# include "Blocks.hpp"

# define CHUNK_SIZE 16
# define ONE_SIXTEENTH 1.0f / 16.0f
# define THREE_SIXTEENTH 3.0f / 16.0f
# define THIRTEEN_SIXTEENTH 13.0f / 16.0f
# define FIFTEEN_SIXTEENTH 15.0f / 16.0f

# if __linux__
#  if 1
#   define WIN_WIDTH 1680
#   define WIN_HEIGHT 1050
#  else
#   define WIN_WIDTH 1920
#   define WIN_HEIGHT 1080
#  endif
#  define IS_LINUX true
# else
#  define WIN_WIDTH 2560
#  define WIN_HEIGHT 1440
#  define IS_LINUX false
# endif

enum {
	CREATIVE = false,
	SURVIVAL = true
};

class Chunk;

std::string get_file_content( std::string file_name );
std::string	trim_spaces( std::string str );
std::vector<std::string> split( std::string &str, char sep );

void compile_shader( GLuint ptrShader, std::string name );
void check_glstate( std::string str, bool displayDebug );
int chunk_pos( int pos );

double gradient( double value, double start, double end, double value_start, double value_end );
float dist2( glm::vec3 pa, glm::vec3 pb );
int maxi( int a, int b );
int mini( int a, int b );
char maxc( char a, char b );
short maxs( short a, short b );

std::string doubleDigits( int nb );

int air_flower( int value, bool air_leaves, bool air_glass, bool air_water );
face_dir opposite_dir( int dir );
bool visible_face( int value, int next, face_dir dir );
bool isSandOrGravel( int type );

void sort_chunks( glm::vec3 pos, std::vector<Chunk *> &chunks );

std::array<int, 5> compute_texcoord_offsets( int o0, int o1, int o2, int o3 );
void face_vertices( void *vertices, std::pair<int, glm::vec3> &v0, std::pair<int, glm::vec3> &v1, std::pair<int, glm::vec3> &v2, std::pair<int, glm::vec3> &v3, size_t & vindex );
void face_water_vertices( GLint *vertices, glm::ivec4 &v0, glm::ivec4 &v1, glm::ivec4 &v2, glm::ivec4 &v3, size_t & vindex );
bool torchFace( GLfloat *vertices, glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, glm::vec3 &v4, glm::vec3 &v6, size_t index );
bool crossFace( GLfloat *vertices, glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, glm::vec3 &v4, glm::vec3 &v5, size_t index );
bool blockFace( GLfloat *vertices, std::array<glm::vec3, 8> v, size_t index, bool special );
std::vector<glm::ivec3> voxel_traversal( glm::vec3 &ray_start, glm::vec3 ray_end );
bool line_cube_intersection( glm::vec3 camPos, glm::vec3 camDir, glm::vec3 cubeCenter, glm::vec3 cubeHalfSize );

#endif
