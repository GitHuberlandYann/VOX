#ifndef UTILS_HPP
# define UTILS_HPP

# include <vector>
# include <array>
# include <iostream>

# include "Blocks.hpp"

# define CHUNK_SIZE 16
# define TICK 0.05
const float ONE16TH = 0.0625f;
# define ONE_SIXTEENTH 1.0f / 16.0f // TODO get rid of those and only use ONE16TH
# define THREE_SIXTEENTH 3.0f / 16.0f
# define THIRTEEN_SIXTEENTH 13.0f / 16.0f
# define FIFTEEN_SIXTEENTH 15.0f / 16.0f

extern int WIN_WIDTH;
extern int WIN_HEIGHT;
// # if __linux__
// #  if 0
// #   define WIN_WIDTH 1680
// #   define WIN_HEIGHT 1050
// #  else
// #   define WIN_WIDTH 1920
// #   define WIN_HEIGHT 1080
// #  endif
// # else
// #  define WIN_WIDTH 2560
// #  define WIN_HEIGHT 1440
// # endif

# define GUI_MIN 1
# define GUI_MAX glm::max(1, glm::min(WIN_WIDTH / 320, WIN_HEIGHT / 240))

enum {
	CREATIVE = false,
	SURVIVAL = true
};

class Chunk;
typedef struct s_shaderInput t_shaderInput;

std::string get_file_content( std::string file_name );
std::string	trim_spaces( std::string str );
std::vector<std::string> split( std::string &str, char sep );

GLuint createShaderProgram( std::string vertex, std::string geometry, std::string fragment );
void check_glstate( std::string str, bool displayDebug );
void loadTextureShader( int index, GLuint texture, std::string texture_file );
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

void sort_chunks( glm::vec3 pos, std::vector<Chunk *> &chunks );

std::array<int, 5> compute_texcoord_offsets( int o0, int o1, int o2, int o3 );
void face_vertices( std::vector<t_shaderInput> &vertices, t_shaderInput v0, t_shaderInput v1, t_shaderInput v2, t_shaderInput v3 );
void face_water_vertices( GLint *vertices, glm::ivec4 &v0, glm::ivec4 &v1, glm::ivec4 &v2, glm::ivec4 &v3, size_t & vindex );
std::vector<glm::ivec3> voxel_traversal( glm::vec3 &ray_start, glm::vec3 ray_end );
glm::vec3 line_plane_intersection( glm::vec3 camPos, glm::vec3 camDir, glm::vec3 p0, glm::vec3 cross );
bool line_cube_intersection( glm::vec3 camPos, glm::vec3 camDir, glm::vec3 cubeCenter, glm::vec3 cubeHalfSize );
bool cube_cube_intersection( glm::vec3 posA, glm::vec3 sizeA, glm::vec3 posB, glm::vec3 sizeB );

namespace EXTRUSION {
	bool drawItem3D( std::vector<t_shaderInput> &arr, int type, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size );
}

#endif
