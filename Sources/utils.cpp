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
		std::cerr << "glGetError set to " << error_check << "when trying to " << str << ", quitting now" << std::endl;
		exit(1);
	}
	if (str == "NO") {
		return ;
	}
	std::cout << str << std::endl;
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

char maxc( char a, char b ) {
	return ((a > b) ? a : b);
}

short maxs( short a, short b ) {
	return ((a > b) ? a : b);
}

int blockGridX( int block, int offset ) // x coord in blockAtlas grid
{
	if (block < 16) {
		return (offset);
	}
	return (3 + block / 16);
}

int blockGridY( int block ) // y coord in blockAtlas grid
{
	return (block & 0xF); // block % 16
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
   * if air_water, water is NOT considered as air (I know this is dumb)*/
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

bool isSandOrGravel( int type )
{
	type += blocks::NOTVISIBLE * (type < blocks::AIR);
	return (type == blocks::SAND || type == blocks::GRAVEL);
}

std::vector<Chunk *> sort_chunks( glm::vec3 pos, std::vector<Chunk *> chunks )
{
	Bench b;
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
	b.stamp("SORT - manhattan");
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
	b.stamp("SORT - chunks");

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
	b.stamp("SORT - sky water");
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
	return (chunks);
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

void face_vertices( GLint *vertices, glm::ivec4 v0, glm::ivec4 v1, glm::ivec4 v2, glm::ivec4 v3, size_t & vindex )
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
bool torchFace( GLint *vertices, glm::ivec3 v0, glm::ivec3 v1, glm::ivec3 v2, glm::ivec3 v3, glm::ivec3 v4, glm::ivec3 v6, size_t index )
{
	// (_vertices[index + 3] == block_hit.z && _vertices[index + 1] == block_hit.x && _vertices[index + 2] == block_hit.y) {
	// 4062, 1537, 0123, 5476, 4501, 2367
	if ((vertices[index + 3] & 0xFF) == v0.z && vertices[index + 1] == v0.x && vertices[index + 2] == v0.y
		&& (vertices[index + 4 + 3] & 0xFF) == v1.z && vertices[index + 4 + 1] == v1.x && vertices[index + 4 + 2] == v1.y
		&& (vertices[index + 8 + 3] & 0xFF) == v2.z && vertices[index + 8 + 1] == v2.x && vertices[index + 8 + 2] == v2.y) {
		return (true);
	}
	if ((vertices[index + 3] & 0xFF) == v4.z && vertices[index + 1] == v4.x && vertices[index + 2] == v4.y
		&& (vertices[index + 4 + 3] & 0xFF) == v0.z && vertices[index + 4 + 1] == v0.x && vertices[index + 4 + 2] == v0.y
		&& (vertices[index + 8 + 3] & 0xFF) == v6.z && vertices[index + 8 + 1] == v6.x && vertices[index + 8 + 2] == v6.y) {
		return (true);
	}
	if ((vertices[index + 3] & 0xFF) == v1.z && vertices[index + 1] == v1.x && vertices[index + 2] == v1.y
		&& (vertices[index + 4 + 3] & 0xFF) == v0.z && vertices[index + 4 + 1] == v0.x && vertices[index + 4 + 2] == v0.y
		&& (vertices[index + 8 + 3] & 0xFF) == v3.z && vertices[index + 8 + 1] == v3.x && vertices[index + 8 + 2] == v3.y) {
		return (true);
	}
	if ((vertices[index + 3] & 0xFF) == v0.z && vertices[index + 1] == v0.x && vertices[index + 2] == v0.y
		&& (vertices[index + 4 + 3] & 0xFF) == v4.z && vertices[index + 4 + 1] == v4.x && vertices[index + 4 + 2] == v4.y
		&& (vertices[index + 8 + 3] & 0xFF) == v2.z && vertices[index + 8 + 1] == v2.x && vertices[index + 8 + 2] == v2.y) {
		return (true);
	}

	return (false);
}

// looks though vertices to check if elements starting at pos index represent a face of the flower v012345678
bool crossFace( GLint *vertices, glm::ivec3 v0, glm::ivec3 v1, glm::ivec3 v2, glm::ivec3 v3, glm::ivec3 v4, glm::ivec3 v5, size_t index )
{
	// (_vertices[index + 3] == block_hit.z && _vertices[index + 1] == block_hit.x && _vertices[index + 2] == block_hit.y) {
	// 4062, 1537, 0123, 5476, 4501, 2367
	if ((vertices[index + 3] & 0xFF) == v0.z && vertices[index + 1] == v0.x && vertices[index + 2] == v0.y
		&& (vertices[index + 4 + 3] & 0xFF) == v5.z && vertices[index + 4 + 1] == v5.x && vertices[index + 4 + 2] == v5.y
		&& (vertices[index + 8 + 3] & 0xFF) == v2.z && vertices[index + 8 + 1] == v2.x && vertices[index + 8 + 2] == v2.y) {
		return (true);
	}
	if ((vertices[index + 3] & 0xFF) == v1.z && vertices[index + 1] == v1.x && vertices[index + 2] == v1.y
		&& (vertices[index + 4 + 3] & 0xFF) == v4.z && vertices[index + 4 + 1] == v4.x && vertices[index + 4 + 2] == v4.y
		&& (vertices[index + 8 + 3] & 0xFF) == v3.z && vertices[index + 8 + 1] == v3.x && vertices[index + 8 + 2] == v3.y) {
		return (true);
	}

	return (false);
}

// looks though vertices to check if elements starting at pos index represent a face of the cube v012345678
bool blockFace( GLint *vertices, glm::ivec3 v0, glm::ivec3 v1, glm::ivec3 v2, glm::ivec3 v3, glm::ivec3 v4, glm::ivec3 v5, glm::ivec3 v6, glm::ivec3 v7, size_t index )
{
	// (_vertices[index + 3] == block_hit.z && _vertices[index + 1] == block_hit.x && _vertices[index + 2] == block_hit.y) {
	// 4062, 1537, 0123, 5476, 4501, 2367
	if ((vertices[index + 3] & 0xFF) == v4.z && vertices[index + 1] == v4.x && vertices[index + 2] == v4.y
		&& (vertices[index + 4 + 3] & 0xFF) == v0.z && vertices[index + 4 + 1] == v0.x && vertices[index + 4 + 2] == v0.y
		&& (vertices[index + 8 + 3] & 0xFF) == v6.z && vertices[index + 8 + 1] == v6.x && vertices[index + 8 + 2] == v6.y) {
		return (true);
	}
	if ((vertices[index + 3] & 0xFF) == v1.z && vertices[index + 1] == v1.x && vertices[index + 2] == v1.y
		&& (vertices[index + 4 + 3] & 0xFF) == v5.z && vertices[index + 4 + 1] == v5.x && vertices[index + 4 + 2] == v5.y
		&& (vertices[index + 8 + 3] & 0xFF) == v3.z && vertices[index + 8 + 1] == v3.x && vertices[index + 8 + 2] == v3.y) {
		return (true);
	}
	if ((vertices[index + 3] & 0xFF) == v0.z && vertices[index + 1] == v0.x && vertices[index + 2] == v0.y
		&& (vertices[index + 4 + 3] & 0xFF) == v1.z && vertices[index + 4 + 1] == v1.x && vertices[index + 4 + 2] == v1.y
		&& (vertices[index + 8 + 3] & 0xFF) == v2.z && vertices[index + 8 + 1] == v2.x && vertices[index + 8 + 2] == v2.y) {
		return (true);
	}
	if ((vertices[index + 3] & 0xFF) == v5.z && vertices[index + 1] == v5.x && vertices[index + 2] == v5.y
		&& (vertices[index + 4 + 3] & 0xFF) == v4.z && vertices[index + 4 + 1] == v4.x && vertices[index + 4 + 2] == v4.y
		&& (vertices[index + 8 + 3] & 0xFF) == v7.z && vertices[index + 8 + 1] == v7.x && vertices[index + 8 + 2] == v7.y) {
		return (true);
	}
	if ((vertices[index + 3] & 0xFF) == v4.z && vertices[index + 1] == v4.x && vertices[index + 2] == v4.y
		&& (vertices[index + 4 + 3] & 0xFF) == v5.z && vertices[index + 4 + 1] == v5.x && vertices[index + 4 + 2] == v5.y
		&& (vertices[index + 8 + 3] & 0xFF) == v0.z && vertices[index + 8 + 1] == v0.x && vertices[index + 8 + 2] == v0.y) {
		return (true);
	}
	if ((vertices[index + 3] & 0xFF) == v2.z && vertices[index + 1] == v2.x && vertices[index + 2] == v2.y
		&& (vertices[index + 4 + 3] & 0xFF) == v3.z && vertices[index + 4 + 1] == v3.x && vertices[index + 4 + 2] == v3.y
		&& (vertices[index + 8 + 3] & 0xFF) == v6.z && vertices[index + 8 + 1] == v6.x && vertices[index + 8 + 2] == v6.y) {
		return (true);
	}

	return (false);
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