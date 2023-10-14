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

void compile_shader( GLuint ptrShader, std::string name )
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

int chunk_pos( int pos )
{
	(pos >= 0 || !(pos % CHUNK_SIZE))
			? pos -= pos % CHUNK_SIZE
			: pos -= CHUNK_SIZE + pos % CHUNK_SIZE;
	return (pos);
}

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

int blockAtlasX( int block ) // x coord in blockAtlas in pxl
{
	if (block < 16) {
		return (2 * (block >= 8) * 16);
	}
	return ((3 + block / 16) * 16);
}

int blockAtlasY( int block ) // y coord in blockAtlas in pxl
{
	return ((block % 16) * 16);
}

/* * we simulate that flowers are air block in order to display adjacent blocks properly
   * also use this for all 'see-through' blocks like leaves, water..*/
int air_flower( int value, bool air_leaves, bool air_water )
{
	if (air_water && value >= blocks::WATER) {
		return (value);
	}
	if (value >= blocks::POPPY || value == blocks::CACTUS || (air_leaves && value == blocks::OAK_LEAVES)) {
		return (0);
	}
	return (value);
}

std::vector<Chunk *> sort_chunks( glm::vec3 pos, std::vector<Chunk *> chunks )
{
	int posX = chunk_pos(pos.x);
	int posY = chunk_pos(pos.y);

	int size = chunks.size();
	std::vector<std::pair<int, Chunk *>> dists;
	dists.reserve(chunks.capacity());
	for (auto& c: chunks) {
		dists.push_back(std::pair<int, Chunk *>(c->manhattanDist(posX, posY), c));
	}
	// std::cout << "in sort chunks, dists size = " << dists.size() << std::endl;

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

	chunks.clear();
	for (auto& d: dists) {
		d.second->sort_sky(pos, false);
		d.second->sort_water(pos, false);
		chunks.push_back(d.second);
	}
	dists.clear();
	// std::cout << "out sort chunks, chunks size " << chunks.size() << std::endl;
	return (chunks);
}

void face_vertices( GLint *vertices, glm::ivec4 v0, glm::ivec4 v1, glm::ivec4 v2, glm::ivec4 v3, int & vindex )
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

// # include <glm/glm.hpp>
// # include <glm/gtc/matrix_transform.hpp>
// # include <glm/gtc/type_ptr.hpp>
// # include <iostream>
// # include <vector>

/**
 * @brief returns all the voxels that are traversed by a ray going from start to end
 * @param start : continous world position where the ray starts
 * @param end   : continous world position where the ray end
 * @return vector of voxel ids hit by the ray in temporal order
 *
 * J. Amanatides, A. Woo. A Fast Voxel Traversal Algorithm for Ray Tracing. Eurographics '87
 */
std::vector<glm::ivec3> voxel_traversal(glm::vec3 ray_start, glm::vec3 ray_end) {
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
	while(last_voxel != current_voxel) {
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

// int main( void )
// {
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
// }