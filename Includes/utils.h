#ifndef UTILS_HPP
# define UTILS_HPP

# include <vector>
# include <array>
# include <iostream>

# include "Blocks.hpp"
# include "Settings.hpp"

class Chunk;
typedef struct s_shaderInput t_shaderInput;
typedef struct s_shaderInputModel t_shaderInputModel;
typedef struct s_shaderInputPart t_shaderInputPart;

namespace utils {

	namespace string {
		std::string get_file_content( std::string file_name );
		std::string	trim_spaces( std::string str );
		std::vector<std::string> split( std::string &str, char sep );
		bool case_insensitive_compare( std::string a, std::string b );
		std::string doubleDigits( int nb );
		std::string toBytes( size_t bytes );
	};

	namespace shader {
		GLuint createShaderProgram( std::string vertex, std::string geometry, std::string fragment );
		void check_glstate( std::string str, bool displayDebug );
		void loadTextureShader( int index, GLuint texture, std::string texture_file );
		void loadSubTextureArray( int width, int height, int layer, std::string texture_file );

		void addQuads( std::vector<t_shaderInput>& arr, std::array<glm::vec3, 4> pts, int texture, int light, int dx, int dy, bool xflip = false, bool yflip = false );
		void addQuads( std::vector<t_shaderInput>& arr, std::array<glm::vec3, 4> pts, int texture, std::array<int, 4> lights, int dx, int dy, bool xflip = false, bool yflip = false );

		void addQuads( std::vector<t_shaderInputModel>& arr, std::array<glm::vec3, 4> pts, int spec, int dx, int dy, bool xflip = false, bool yflip = false );
		void addQuads( std::vector<t_shaderInputPart>& arr, std::array<glm::vec3, 4> pts, int spec, int dx, int dy, bool xflip = false, bool yflip = false );
	};

	namespace text {
		int textWidth( int font_size, std::string str, int limit = -1 );
		int textHeight( int font_size, std::string str, int limit = -1 );
	};

	namespace block {
		int air_flower( int value, bool air_leaves, bool air_glass, bool air_water );
		int opposite_dir( int dir );
		bool visible_face( int value, int next, int dir );
	};

	namespace fluid {
		std::array<int, 5> compute_texcoord_offsets( int o0, int o1, int o2, int o3 );
		void face_water_vertices( std::vector<glm::ivec4> &vertices, glm::ivec4 &v0, glm::ivec4 &v1, glm::ivec4 &v2, glm::ivec4 &v3 );
	};

	namespace math {
		double gradient( double value, double start, double end, double value_start, double value_end );
		float dist2( glm::vec3 pa, glm::vec3 pb );
		short maxs( short a, short b );

		int chunk_pos( int pos );
		void sort_chunks( glm::vec3 pos, std::vector<std::shared_ptr<Chunk>>& chunks );

		std::vector<glm::ivec3> voxel_traversal( const glm::vec3 ray_start, const glm::vec3 ray_end );
		glm::vec3 line_plane_intersection( const glm::vec3 camPos, const glm::vec3 camDir, const glm::vec3 p0, const glm::vec3 cross );
		bool line_cube_intersection( const glm::vec3 camPos, const glm::vec3 camDir, const glm::vec3 cubeCenter, const glm::vec3 cubeHalfSize );
		bool cube_cube_intersection( const glm::vec3 posA, const glm::vec3 sizeA, const glm::vec3 posB, const glm::vec3 sizeB );
		bool segment_cube_intersection( const glm::vec3 segStart, const glm::vec3 segEnd, const glm::vec3 cubeCenter, const glm::vec3 cubeHalfSize );
	};

	namespace extrusion {
		bool drawItem3D( std::vector<t_shaderInput>& arr, int type, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size );
	};

	namespace memory {
		void getMemoryUsage( size_t& residentSize, size_t& virtualSize );
		void memwrite( char* dst, const void* src, size_t n, size_t& dstOffset );
		void memread( void* dst, const char* src, size_t n, size_t& srcOffset );
	};

	namespace network {
		std::string getEth0( void );
	};
};

#endif
