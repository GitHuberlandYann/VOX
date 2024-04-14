#include <fstream>
#include "Chunk.hpp"

#include "SOIL/SOIL.h"
typedef struct {
	unsigned char *content;
	int width;
	int height;
}				t_tex;

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

std::string	trim_spaces( std::string str )
{
	int index = 0;
	std::string new_string;
	while (str[index] == ' ' || str[index] == '\t')
		++index;
	while (str[index])
	{
		if (str[index] == '\t')
			new_string += ' ';
		else
			new_string += str[index];
		if (str[index] == ' ' || str[index] == '\t')
		{
			while (str[index] == ' ' || str[index] == '\t')
				++index;
			--index;
		}
		++index;
	}
	if (new_string.empty())
		return ("");
	if (new_string[new_string.size() - 1] == '\r')
		new_string = new_string.substr(0, new_string.size() - 1);
	if (new_string[new_string.size() - 1] == ' ')
		new_string = new_string.substr(0, new_string.size() - 1);
	return (new_string);
}

std::vector<std::string> split( std::string &str, char sep )
{
	std::string tmp = "";
	std::vector<std::string> res;
	
	for (size_t i = 0; i < str.size(); ++i) {
		if (str[i] != sep) {
			tmp += std::tolower(str[i]);
		} else if (tmp[0]) {
			res.push_back(tmp);
			tmp = "";
		}
	}
	if (tmp[0]) {
		res.push_back(tmp);
	}
	return (res);
}

static void compile_shader( GLuint ptrShader, std::string name )
{
	glCompileShader(ptrShader);

    GLint status;
    glGetShaderiv(ptrShader, GL_COMPILE_STATUS, &status);
	if (status) {
		std::cout << name << " shader compiled successfully" << std::endl;
	} else {
		char buffer[512];
		glGetShaderInfoLog(ptrShader, 512, NULL, buffer);

		std::cerr << name << " shader did not compile, error log:" << std::endl << buffer << std::endl;
		exit(1);
	}
}

GLuint createShaderProgram( std::string vertex, std::string geometry, std::string fragment )
{
	std::string vertex_shader_data = get_file_content("Sources/Shaders/" + vertex + ".glsl");
	char *vertexSource = &vertex_shader_data[0];

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	compile_shader(vertexShader, vertex);

	GLuint geometryShader;
	if (geometry[0]) {
		std::string geometry_shader_data = get_file_content("Sources/Shaders/" + geometry + ".glsl");
		char *geometrySource = &geometry_shader_data[0];

		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &geometrySource, NULL);
		compile_shader(geometryShader, geometry);
	}

	std::string fragment_shader_data = get_file_content("Sources/Shaders/" + fragment + ".glsl");
	char *fragmentSource = &fragment_shader_data[0];

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	compile_shader(fragmentShader, fragment);

	// Combining shaders into a program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	if (geometry[0]) {
		glAttachShader(shaderProgram, geometryShader);
	}
	glAttachShader(shaderProgram, fragmentShader);

	glDeleteShader(fragmentShader);
	if (geometry[0]) {
		glDeleteShader(geometryShader);
	}
    glDeleteShader(vertexShader);

	return (shaderProgram);
}

void check_glstate( std::string str, bool displayDebug )
{
	GLenum error_check = glGetError();	
	if (error_check) {
		std::cerr << "glGetError set to " << error_check << " when trying to " << str << ", quitting now" << std::endl;
		exit(1);
	}
	if (displayDebug) {
		std::cout << str << std::endl;
	}
}

void loadTextureShader( int index, GLuint texture, std::string texture_file )
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, texture);

	// load image
	t_tex img;
	img.content = SOIL_load_image(texture_file.c_str(), &img.width, &img.height, 0, SOIL_LOAD_RGBA);
	if (!img.content) {
		std::cerr << "failed to load image " << texture_file << " because:" << std::endl << SOIL_last_result() << std::endl;
		exit(1);
	}

	// load image as texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, img.content);
			
	// set settings for texture wraping and size modif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST because pixel art, otherwise GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SOIL_free_image_data(img.content);

	check_glstate("Succesfully loaded " + texture_file + " to shader", true);
}

int chunk_pos( int pos )
{
	if (CHUNK_SIZE == 16) {
		(pos >= 0 || !(pos & 0xF))
				? pos -= pos & 0xF
				: pos -= CHUNK_SIZE + pos & 0xF;
		return (pos);
	}
	(pos >= 0 || !(pos % CHUNK_SIZE))
			? pos -= pos % CHUNK_SIZE
			: pos -= CHUNK_SIZE + pos % CHUNK_SIZE;
	return (pos);
}

// takes value in range [start; end] and outputs gradient in range [value_start; value_end]
// if value out of range, outputs value_start or value_end
double gradient( double value, double start, double end, double value_start, double value_end )
{
	if (value <= start) {
		return (value_start);
	} else if (value >= end) {
		return (value_end);
	}

	double progress = (value - start) / (end - start);
	return (value_start + progress * (value_end - value_start));
}

float dist2( glm::vec3 pa, glm::vec3 pb )
{
	float a = pb.x - pa.x;
	float b = pb.y - pa.y;
	float c = pb.z - pa.z;
	return (a * a + b * b + c * c);
}

int maxi( int a, int b ) {
	return ((a > b) ? a : b);
}

int mini( int a, int b )
{
	return ((a > b) ? b : a);
}

char maxc( char a, char b ) {
	return ((a > b) ? a : b);
}

short maxs( short a, short b ) {
	return ((a > b) ? a : b);
}

std::string doubleDigits( int nb )
{
	if (nb < 10) {
		return (std::string("0") + static_cast<char>('0' + nb));
	}
	return (std::to_string(nb));
}

/* * we simulate that flowers are air block in order to display adjacent blocks properly
   * also use this for all 'see-through' blocks like leaves, water..
   * if air_leaves, leaves are considered as air
   * if air_glass, glass is considered as air
   * if air_water, water is NOT considered as air (I know this is dumb)*/
int air_flower( int value, bool air_leaves, bool air_glass, bool air_water )
{
	value &= 0xFF;
	if (air_water && value >= blocks::WATER) {
		return (value);
	}
	if (value >= blocks::POPPY || value == blocks::CACTUS
		|| (air_leaves && (value + (value < 0) * blocks::NOTVISIBLE) == blocks::OAK_LEAVES)
		|| (air_glass && (value + (value < 0) * blocks::NOTVISIBLE) == blocks::GLASS)) {
		return (0);
	}
	return (value);
}

face_dir opposite_dir( int dir )
{
	switch (dir) {
		case face_dir::MINUSX:
			return (face_dir::PLUSX);
		case face_dir::PLUSX:
			return (face_dir::MINUSX);
		case face_dir::MINUSY:
			return (face_dir::PLUSY);
		case face_dir::PLUSY:
			return (face_dir::MINUSY);
		case face_dir::MINUSZ:
			return (face_dir::PLUSZ);
		case face_dir::PLUSZ:
			return (face_dir::MINUSZ);
	}
	return (face_dir::MINUSX);
}

// shapewise, all bottom slabs are treated the same
static int blockShape( int value )
{
	if (value >= blocks::POPPY) {
		return (blocks::AIR);
	}
	switch (value) {
		case blocks::CHEST:
		case blocks::CACTUS:
			return (blocks::AIR);
		case blocks::DIRT_PATH:
			return (blocks::FARMLAND);
		// case blocks::*_SLAB_BOTTOM:
		// 	return (blocks::OAK_SLAB_BOTTOM);
		// case blocks::*_STAIRS_BOTTOM:
		// return (blocks::OAK_STAIRS_BOTTOM)
	}
	return (value);
}

/**
 * @brief checks if a specific face should be drawn by using neighbour
 * @param value  value of block being drawn
 * @param next   value of neighbouring block
 * @param dir    dir from value to next
 * @return bool face_should_be_drawn
 */
bool visible_face( int value, int next, face_dir dir )
{
	value = blockShape(value & 0xFF);
	next = blockShape(next & 0xFF);
	if (value == blocks::AIR) {
		return (false);
	}
	if (next == blocks::AIR || next == blocks::OAK_DOOR || next == blocks::OAK_TRAPDOOR
		|| ((next == blocks::GLASS || next == blocks::OAK_FENCE || next == blocks::GLASS_PANE) && value != next)) {
		return (true);
	}
	if (next == blocks::OAK_LEAVES && (value != blocks::OAK_LEAVES
		|| (value == blocks::OAK_LEAVES
		&& (dir == face_dir::PLUSX || dir == face_dir::PLUSY || dir == face_dir::PLUSZ)))) {
		return (true);
	}
	if (dir == face_dir::PLUSZ && (value == blocks::OAK_SLAB_BOTTOM || value == blocks::FARMLAND)) {
		return (true);
	}
	if (dir == face_dir::MINUSZ && value == blocks::OAK_SLAB_TOP) {
		return (true);
	}
	switch (next) {
		case blocks::OAK_SLAB_BOTTOM:
			if (dir == face_dir::MINUSZ) {
				return (true);
			}
			return (value != next);
		case blocks::OAK_SLAB_TOP:
			if (dir == face_dir::PLUSZ) {
				return (true);
			}
			return (value != next);
		case blocks::OAK_STAIRS_BOTTOM:
			if (dir == face_dir::PLUSZ) {
				return (false);
			}
			return (true);
		case blocks::OAK_STAIRS_TOP:
			if (dir == face_dir::MINUSZ) {
				return (false);
			}
			return (true);
		case blocks::FARMLAND:
			if (dir == face_dir::MINUSZ) {
				return (true);
			} else if (dir == face_dir::PLUSZ) {
				return (false);
			}
			return (value != blocks::OAK_SLAB_BOTTOM && value != blocks::FARMLAND);
	}
	return (false);
}

void sort_chunks( glm::vec3 pos, std::vector<Chunk *> &chunks )
{
	// Bench b;
	int posX = chunk_pos(pos.x);
	int posY = chunk_pos(pos.y);

	#if 1
	int size = chunks.size();
	std::vector<std::pair<int, Chunk *>> dists;
	dists.reserve(chunks.capacity());
	for (auto& c: chunks) {
		dists.push_back(std::pair<int, Chunk *>(c->manhattanDist(posX, posY), c));
	}
	// std::cout << "in sort chunks, dists size = " << dists.size() << std::endl;
	// b.stamp("SORT - manhattan");
	for (int index = 0; index < size; index++) {
		int minDist = dists[index].first, minIndex = index;
		for (int jindex = index + 1; jindex < size; jindex++) {
			if (dists[jindex].first > minDist) {
				minIndex = jindex;
				minDist = dists[minIndex].first;
			}
		}
		if (minIndex != index) {
			std::pair<int, Chunk *> tmp = dists[minIndex];
			dists[minIndex] = dists[index];
			dists[index] = tmp;
		}
	}
	// b.stamp("SORT - chunks");

	chunks.clear();
	chunks.reserve(dists.capacity());
	// int cnt = 0;
	// sky and water sorted only if new chunk or chunk in cross centered on player
	for (auto d: dists) {
		Chunk *c = d.second;
		int diffX = posX - c->getStartX();
		int diffY = posY - c->getStartY();
		if (!c->getSortedOnce() || (diffX <= CHUNK_SIZE && diffX >= -CHUNK_SIZE) || (diffY <= CHUNK_SIZE && diffY >= -CHUNK_SIZE)) {
			c->sort_sky(pos, false);
			c->sort_water(pos, false);
			// ++cnt;
		}
		chunks.push_back(c);
	}
	// std::cout << "\tsorted " << cnt << std::endl;
	// b.stamp("SORT - sky water");
	dists.clear();
	#else
	std::multimap<int, Chunk*> dists;
	for (auto c: chunks) {
		dists.insert({c->manhattanDist(posX, posY), c});
	}
	// std::cout << "in sort chunks, dists size = " << dists.size() << std::endl;
	b.stamp("SORT - manhattan");
	std::sort(dists.begin(), dists.end());
	b.stamp("SORT - chunks");

	chunks.clear();
	chunks.reserve(dists.size());
	// int cnt = 0;
	// sky and water sorted only if new chunk or chunk in cross centered on player
	for (auto it = dists.rbegin(); it != dists.rend(); it++) {
		Chunk *c = it->second;
		int diffX = posX - c->getStartX();
		int diffY = posY - c->getStartY();
		if (!c->getSortedOnce() || (diffX <= CHUNK_SIZE && diffX >= -CHUNK_SIZE) || (diffY <= CHUNK_SIZE && diffY >= -CHUNK_SIZE)) {
			c->sort_sky(pos, false);
			c->sort_water(pos, false);
			// ++cnt;
		}
		chunks.push_back(c);
	}
	// std::cout << "\tsorted " << cnt << std::endl;
 	b.stamp("SORT - sky water");
	dists.clear();

	#endif
	// std::cout << "out sort chunks, chunks size " << chunks.size() << std::endl;
}

/* takes offsets of face's corners and compute texture coordinates to represent direction of liquid's flow
 * offsets [0:7], 0 being highest
 * 1 << 8 => waterStill
 * 1 << 9 => waterFlow
 * 1 << 10 => texture point +1 x
 * 1 << 11 => texture point +1 y
 * res[0] = top left
 * res[1] = top right
 * res[2] = bottom left
 * res[3] = bottom right
 * res[4] = texture used
*/
std::array<int, 5> compute_texcoord_offsets( int o0, int o1, int o2, int o3 )
{
	if (o0 == o1 && o0 == o2 && o0 == o3) {
		return {0, 1 << 10, 2 << 10, 3 << 10, 1 << 8}; // still water
	}
	// from now on, flowing water
	int max = std::min({o0, o1, o2, o3}); // min is highest point
	if (max == o0) {
		if (o0 == o2) {
			return {1 << 10, 3 << 10, 0, 2 << 10, 1 << 9}; // +x 0y
		} else if (o0 == o1) {
			return {0, 1 << 10, 2 << 10, 3 << 10, 1 << 9}; // 0x -y
		}
		return {4 << 10, 9 << 10, 8 << 10, 6 << 10, 1 << 9}; // +x -y
	} else if (max == o1) {
		if (o1 == o3) {
			return {2 << 10, 0, 3 << 10, 1 << 10, 1 << 9}; // -x 0y
		}
		return {8 << 10, 4 << 10, 6 << 10, 9 << 10, 1 << 9}; // -x -y
	} else if (max == o2) {
		if (o2 == o3) {
			return {3 << 10, 2 << 10, 1 << 10, 0, 1 << 9}; // 0x +y
		}
		return {9 << 10, 6 << 10, 4 << 10, 8 << 10, 1 << 9}; // +x +y

	}
	return {6 << 10, 8 << 10, 9 << 10, 4 << 10, 1 << 9}; // -x +y
}


void face_vertices( std::vector<t_shaderInput> &vertices, t_shaderInput v0, t_shaderInput v1, t_shaderInput v2, t_shaderInput v3 )
{
	vertices.push_back(v0);
	vertices.push_back(v1);
	vertices.push_back(v2);

	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v2);
}

void face_water_vertices( GLint *vertices, glm::ivec4 &v0, glm::ivec4 &v1, glm::ivec4 &v2, glm::ivec4 &v3, size_t & vindex )
{
	vertices[vindex] = v0.x;
	vertices[vindex + 1] = v0.y;
	vertices[vindex + 2] = v0.z;
	vertices[vindex + 3] = v0.w;
	vertices[vindex + 4] = v1.x;
	vertices[vindex + 5] = v1.y;
	vertices[vindex + 6] = v1.z;
	vertices[vindex + 7] = v1.w;
	vertices[vindex + 8] = v2.x;
	vertices[vindex + 9] = v2.y;
	vertices[vindex + 10] = v2.z;
	vertices[vindex + 11] = v2.w;
	vindex += 12;

	vertices[vindex] = v1.x;
	vertices[vindex + 1] = v1.y;
	vertices[vindex + 2] = v1.z;
	vertices[vindex + 3] = v1.w;
	vertices[vindex + 4] = v3.x;
	vertices[vindex + 5] = v3.y;
	vertices[vindex + 6] = v3.z;
	vertices[vindex + 7] = v3.w;
	vertices[vindex + 8] = v2.x;
	vertices[vindex + 9] = v2.y;
	vertices[vindex + 10] = v2.z;
	vertices[vindex + 11] = v2.w;
	vindex += 12;
}

// looks though vertices to check if elements starting at pos index represent a face of the torch v012345678
bool torchFace( std::vector<t_shaderInput> &vertices, glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, glm::vec3 &v4, glm::vec3 &v6, size_t index )
{
	// 4062, 1537, 0123, 5476, 4501, 2367
	return ((vertices[index].pos == v0 && vertices[index + 1].pos == v1 && vertices[index + 2].pos == v2)
		|| (vertices[index].pos == v4 && vertices[index + 1].pos == v0 && vertices[index + 2].pos == v6)
		|| (vertices[index].pos == v1 && vertices[index + 1].pos == v0 && vertices[index + 2].pos == v3)
		|| (vertices[index].pos == v0 && vertices[index + 1].pos == v4 && vertices[index + 2].pos == v2));
}

// looks though vertices to check if elements starting at pos index represent a face of the flower v012345678
bool crossFace( std::vector<t_shaderInput> &vertices, glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, glm::vec3 &v4, glm::vec3 &v5, size_t index )
{
	// 4062, 1537, 0123, 5476, 4501, 2367
	return ((vertices[index].pos == v0 && vertices[index + 1].pos == v5 && vertices[index + 2].pos == v2)
		|| (vertices[index].pos == v1 && vertices[index + 1].pos == v4 && vertices[index + 2].pos == v3));
}

// looks though vertices to check if elements starting at pos index represent a face of the cube v012345678
bool blockFace( std::vector<t_shaderInput> &vertices, std::array<glm::vec3, 8> v, size_t index, bool special )
{
	// 4062, 1537, 0123, 5476, 4501, 2367
	if ((vertices[index].pos == v[4] && vertices[index + 1].pos == v[0] && vertices[index + 2].pos == v[6])
		|| (vertices[index].pos == v[1] && vertices[index + 1].pos == v[5] && vertices[index + 2].pos == v[3])
		|| (vertices[index].pos == v[0] && vertices[index + 1].pos == v[1] && vertices[index + 2].pos == v[2])
		|| (vertices[index].pos == v[5] && vertices[index + 1].pos == v[4] && vertices[index + 2].pos == v[7])) {
		return (true);
	}
	if (special) {
		v[4].z -= ONE_SIXTEENTH;
		v[5].z -= ONE_SIXTEENTH;
		v[0].z -= ONE_SIXTEENTH;
		v[1].z -= ONE_SIXTEENTH;
	}
	return ((vertices[index].pos == v[4] && vertices[index + 1].pos == v[5] && vertices[index + 2].pos == v[0])
		|| (vertices[index].pos == v[2] && vertices[index + 1].pos == v[3] && vertices[index + 2].pos == v[6]));
}

// # include <glm/glm.hpp>
// # include <glm/gtc/matrix_transform.hpp>
// # include <glm/gtc/type_ptr.hpp>
// # include <iostream>
// # include <vector>

/**
 * @brief returns all the voxels that are traversed by a ray going from start to end
 * @param start : world position where the ray starts
 * @param end   : world position where the ray end
 * @return vector of voxel ids hit by the ray in temporal order
 *
 * J. Amanatides, A. Woo. A Fast Voxel Traversal Algorithm for Ray Tracing. Eurographics '87
 */
std::vector<glm::ivec3> voxel_traversal( glm::vec3 &ray_start, glm::vec3 ray_end ) {
	std::vector<glm::ivec3> visited_voxels;

	// This id of the first/current voxel hit by the ray.
	// Using floor (round down) is actually very important,
	// the implicit int-casting will round up for negative numbers.
	glm::ivec3 current_voxel(std::floor(ray_start.x),
								std::floor(ray_start.y),
								std::floor(ray_start.z));

	// The id of the last voxel hit by the ray.
	glm::ivec3 last_voxel(std::floor(ray_end.x),
								std::floor(ray_end.y),
								std::floor(ray_end.z));

	// Compute ray direction.
	glm::vec3 ray = ray_end - ray_start;

	// In which direction the voxel ids are incremented.
	int stepX = (ray.x >= 0) ? 1 : -1;
	int stepY = (ray.y >= 0) ? 1 : -1;
	int stepZ = (ray.z >= 0) ? 1 : -1;

	// Distance along the ray to the next voxel border from the current position.
	float next_voxel_boundary_x = current_voxel.x + stepX;
	float next_voxel_boundary_y = current_voxel.y + stepY;
	float next_voxel_boundary_z = current_voxel.z + stepZ;

	// we do this because if we are in x = 1.7 and go to x--, dist to next block is 0.7 and not 1.7
	if (current_voxel.x != last_voxel.x && ray.x < 0) { ++next_voxel_boundary_x; }
	if (current_voxel.y != last_voxel.y && ray.y < 0) { ++next_voxel_boundary_y; }
	if (current_voxel.z != last_voxel.z && ray.z < 0) { ++next_voxel_boundary_z; }

	// tMaxX, tMaxY, tMaxZ -- time until next intersection with voxel-border
	// the value of t at which the ray crosses the first vertical voxel boundary
	float tMaxX = (ray.x != 0) ? (next_voxel_boundary_x - ray_start.x) / ray.x : FLT_MAX; //
	float tMaxY = (ray.y != 0) ? (next_voxel_boundary_y - ray_start.y) / ray.y : FLT_MAX; //
	float tMaxZ = (ray.z != 0) ? (next_voxel_boundary_z - ray_start.z) / ray.z : FLT_MAX; //

	// tDeltaX, tDeltaY, tDeltaZ --
	// time it takes to travel a distance of 1! voxel in a given direction
	// can only be FLT_MAX if we never go in that direction
	float tDeltaX = (ray.x != 0) ? 1.0f / ray.x * stepX : FLT_MAX;
	float tDeltaY = (ray.y != 0) ? 1.0f / ray.y * stepY : FLT_MAX;
	float tDeltaZ = (ray.z != 0) ? 1.0f / ray.z * stepZ : FLT_MAX;

	visited_voxels.push_back(current_voxel);
	while (last_voxel != current_voxel) {
		if (tMaxX < tMaxY && tMaxX < tMaxZ) { // x will reach border before others
			current_voxel.x += stepX;
			tMaxX += tDeltaX;
		} else if (tMaxY < tMaxZ) { // y will reach border before others
			current_voxel.y += stepY;
			tMaxY += tDeltaY;
		} else { // z will reach border before others
			current_voxel.z += stepZ;
			tMaxZ += tDeltaZ;
		}
		visited_voxels.push_back(current_voxel);
	}
	return (visited_voxels);
}

/**
 * @brief returns intersection point between line and plane
 * @param camPos : world position where the ray starts (la)
 * @param camDir : vector director of ray (lab)
 * @param p0 : point in plane
 * @param cross : vectorial | cross product (p01 x p02)
 * @return Intersection point. Or vec3 with z == -1 if no intersection.
 *
 * https://en.wikipedia.org/wiki/Line-plane_intersection
 */
glm::vec3 line_plane_intersection( glm::vec3 camPos, glm::vec3 camDir, glm::vec3 p0, glm::vec3 cross ) {
	float determinant = -glm::dot(camDir, cross);
	if (!determinant) { // div zero -> line parallel to plane -> no intersection
		return {FLT_MIN, FLT_MIN, FLT_MIN};
	}
	float t = glm::dot(cross, camPos - p0) / determinant;
	return {camPos.x + camDir.x * t, camPos.y + camDir.y * t, camPos.z + camDir.z * t};
}

static bool line_rectangle_intersection( glm::vec3 &camPos, glm::vec3 &camDir, glm::vec3 p0, glm::vec3 size )
{
	glm::vec3 res;
	if (size.z) {
		res = line_plane_intersection(camPos, camDir, p0, glm::cross(glm::vec3(size.x, size.y, 0), glm::vec3(0, 0, size.z)));
	} else {
		res = line_plane_intersection(camPos, camDir, p0, glm::cross(glm::vec3(size.x, 0, 0), glm::vec3(0, size.y, 0)));
	}
	return (res.x >= p0.x && res.x <= p0.x + size.x
			&& res.y >= p0.y && res.y <= p0.y + size.y
			&& res.z >= p0.z && res.z <= p0.z + size.z);
}

// 'cube' but actually parallelepiped rectangle
bool line_cube_intersection( glm::vec3 camPos, glm::vec3 camDir, glm::vec3 cubeCenter, glm::vec3 cubeHalfSize )
{
	// std::cout << "line_cube_intersection: " << camPos.x << ", " << camPos.y << ", " << camPos.z << " -> " << camDir.x << ", " << camDir.y << ", " << camDir.z << " and " << cubeCenter.x << ", " << cubeCenter.y << ", " << cubeCenter.z << " -> " << cubeHalfSize.x << ", " << cubeHalfSize.y << ", " << cubeHalfSize.z << std::endl;
	return (line_rectangle_intersection(camPos, camDir, cubeCenter - cubeHalfSize, {2 * cubeHalfSize.x, 0, 2 * cubeHalfSize.z}) // face_dir::MINUSY
			|| line_rectangle_intersection(camPos, camDir, cubeCenter - cubeHalfSize, {0, 2 * cubeHalfSize.y, 2 * cubeHalfSize.z}) // face_dir::MINUSX
			|| line_rectangle_intersection(camPos, camDir, {cubeCenter.x + cubeHalfSize.x, cubeCenter.y - cubeHalfSize.y, cubeCenter.z - cubeHalfSize.z}, {0, 2 * cubeHalfSize.y, 2 * cubeHalfSize.z}) // face_dir::PLUSX
			|| line_rectangle_intersection(camPos, camDir, {cubeCenter.x - cubeHalfSize.x, cubeCenter.y + cubeHalfSize.y, cubeCenter.z - cubeHalfSize.z}, {2 * cubeHalfSize.x, 0, 2 * cubeHalfSize.z}) // face_dir::PLUSY
			|| line_rectangle_intersection(camPos, camDir, {cubeCenter.x - cubeHalfSize.x, cubeCenter.y - cubeHalfSize.y, cubeCenter.z + cubeHalfSize.z}, {2 * cubeHalfSize.x, 2 * cubeHalfSize.y, 0}) // face_dir::PLUSZ
			// no need for this check|| line_rectangle_intersection(camPos, camDir, {cubeCenter.x - cubeHalfSize.x, cubeCenter.y - cubeHalfSize.y, cubeCenter.z - cubeHalfSize.z}, {2 * cubeHalfSize.x, 2 * cubeHalfSize.y, 0}) // face_dir::MINUSZ
			);
}

// returns if cubes a and b intersect
// posA, posB are center points of cubes
// WATCHOUT posA.z is bottom of cube a
// sizeA, sizeB are halfSizes of cubes
// WATCHOUT sizeA.z is fullHeight of cube a
bool cube_cube_intersection( glm::vec3 posA, glm::vec3 sizeA, glm::vec3 posB, glm::vec3 sizeB )
{
	return (posA.x - sizeA.x < posB.x + sizeB.x && posA.x + sizeA.x > posB.x - sizeB.x
			&& posA.y - sizeA.y < posB.y + sizeB.y && posA.y + sizeA.y > posB.y - sizeB.y
			&& posA.z < posB.z + sizeB.z && posA.z + sizeA.z > posB.z - sizeB.z);
}

/*
int main( void )
{
// 	glm::vec3 ray_start(0.6, 0.75, 0.92468);
// 	glm::vec3 ray_end(-10.75, -10.75, 10.91693);
// 	std::cout << "Voxel size: 1" << std::endl;
// 	std::cout << "Starting position: " << ray_start.x << ", " << ray_start.y << ", " << ray_start.z << std::endl;
// 	std::cout << "Ending position: " << ray_end.x << ", " << ray_end.y << ", " << ray_end.z << std::endl;
// 	std::cout << "Voxel ID's from start to end:" << std::endl;
// 	std::vector<glm::ivec3> ids = voxel_traversal(ray_start,ray_end);

// 	for (auto& i : ids) {
// 		std::cout << "> " << i.x << ", " << i.y << ", " << i.z << std::endl;
// 	}
// 	std::cout << "Total number of traversed voxels: " << ids.size() << std::endl;


	glm::vec3 camPos = {0.5, 0, 0}, camDir = {1, 0, 0}, p0 = {-1, -1, 0}, p1 = {1, -1, 0}, p2 = {1, 1, 0};

	glm::vec3 inter = line_plane_intersection(camPos, camDir, p0, glm::cross(p1 - p0, p2 - p0));
	std::cout << "inter is " << inter.x << ", " << inter.y << ", " << inter.z << std::endl;
}*/
