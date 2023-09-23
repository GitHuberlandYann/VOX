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
  // TODO: what happens if the end point is on a border?
  glm::ivec3 last_voxel(std::floor(ray_end.x),
                            std::floor(ray_end.y),
                            std::floor(ray_end.z));

  // Compute ray direction.
  glm::vec3 ray = ray_end - ray_start;

  // In which direction the voxel ids are incremented.
  int stepX = (ray.x >= 0) ? 1 : -1;
  int stepY = (ray.y >= 0) ? 1 : -1;
  int stepZ = (ray.z >= 0) ? 1 : -1;

  // Distance along the ray to the next voxel border from the current position (tMaxX, tMaxY, tMaxZ).
  float next_voxel_boundary_x = current_voxel.x + stepX;
  float next_voxel_boundary_y = current_voxel.y + stepY;
  float next_voxel_boundary_z = current_voxel.z + stepZ;

	// we do this because if we are in x = 1.7 and go to x--, dist to next block is 0.7 and not 1.7
  if (current_voxel.x != last_voxel.x && ray.x < 0) { ++next_voxel_boundary_x; }
  if (current_voxel.y != last_voxel.y && ray.y < 0) { ++next_voxel_boundary_y; }
  if (current_voxel.z != last_voxel.z && ray.z < 0) { ++next_voxel_boundary_z; }

  // tMaxX, tMaxY, tMaxZ -- distance until next intersection with voxel-border
  // the value of t at which the ray crosses the first vertical voxel boundary
  float tMaxX = (ray.x != 0) ? (next_voxel_boundary_x - ray_start.x) / ray.x : FLT_MAX; //
  float tMaxY = (ray.y != 0) ? (next_voxel_boundary_y - ray_start.y) / ray.y : FLT_MAX; //
  float tMaxZ = (ray.z != 0) ? (next_voxel_boundary_z - ray_start.z) / ray.z : FLT_MAX; //

  // tDeltaX, tDeltaY, tDeltaZ --
  // how far along the ray we must move for the horizontal component to equal the width of a voxel
  // the direction in which we traverse the grid
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
  return visited_voxels;
}
// double _bin_size = 1;

// /**
//  * @brief returns all the voxels that are traversed by a ray going from start to end
//  * @param start : continous world position where the ray starts
//  * @param end   : continous world position where the ray end
//  * @return vector of voxel ids hit by the ray in temporal order
//  *
//  * J. Amanatides, A. Woo. A Fast Voxel Traversal Algorithm for Ray Tracing. Eurographics '87
//  */

// std::vector<Eigen::Vector3i> voxel_traversal(Eigen::Vector3d ray_start, Eigen::Vector3d ray_end) {
//   std::vector<Eigen::Vector3i> visited_voxels;

//   // This id of the first/current voxel hit by the ray.
//   // Using floor (round down) is actually very important,
//   // the implicit int-casting will round up for negative numbers.
//   Eigen::Vector3i current_voxel(std::floor(ray_start[0]/_bin_size),
//                                 std::floor(ray_start[1]/_bin_size),
//                                 std::floor(ray_start[2]/_bin_size));

//   // The id of the last voxel hit by the ray.
//   // TODO: what happens if the end point is on a border?
//   Eigen::Vector3i last_voxel(std::floor(ray_end[0]/_bin_size),
//                              std::floor(ray_end[1]/_bin_size),
//                              std::floor(ray_end[2]/_bin_size));

//   // Compute normalized ray direction.
//   Eigen::Vector3d ray = ray_end-ray_start;
//   //ray.normalize();

//   // In which direction the voxel ids are incremented.
//   double stepX = (ray[0] >= 0) ? 1:-1; // correct
//   double stepY = (ray[1] >= 0) ? 1:-1; // correct
//   double stepZ = (ray[2] >= 0) ? 1:-1; // correct

//   // Distance along the ray to the next voxel border from the current position (tMaxX, tMaxY, tMaxZ).
//   double next_voxel_boundary_x = (current_voxel[0]+stepX)*_bin_size; // correct
//   double next_voxel_boundary_y = (current_voxel[1]+stepY)*_bin_size; // correct
//   double next_voxel_boundary_z = (current_voxel[2]+stepZ)*_bin_size; // correct

//   // tMaxX, tMaxY, tMaxZ -- distance until next intersection with voxel-border
//   // the value of t at which the ray crosses the first vertical voxel boundary
//   double tMaxX = (ray[0]!=0) ? (next_voxel_boundary_x - ray_start[0])/ray[0] : DBL_MAX; //
//   double tMaxY = (ray[1]!=0) ? (next_voxel_boundary_y - ray_start[1])/ray[1] : DBL_MAX; //
//   double tMaxZ = (ray[2]!=0) ? (next_voxel_boundary_z - ray_start[2])/ray[2] : DBL_MAX; //

//   // tDeltaX, tDeltaY, tDeltaZ --
//   // how far along the ray we must move for the horizontal component to equal the width of a voxel
//   // the direction in which we traverse the grid
//   // can only be FLT_MAX if we never go in that direction
//   double tDeltaX = (ray[0]!=0) ? _bin_size/ray[0]*stepX : DBL_MAX;
//   double tDeltaY = (ray[1]!=0) ? _bin_size/ray[1]*stepY : DBL_MAX;
//   double tDeltaZ = (ray[2]!=0) ? _bin_size/ray[2]*stepZ : DBL_MAX;

//   Eigen::Vector3i diff(0,0,0);
//   bool neg_ray=false;
//   if (current_voxel[0]!=last_voxel[0] && ray[0]<0) { diff[0]--; neg_ray=true; }
//   if (current_voxel[1]!=last_voxel[1] && ray[1]<0) { diff[1]--; neg_ray=true; }
//   if (current_voxel[2]!=last_voxel[2] && ray[2]<0) { diff[2]--; neg_ray=true; }
//   visited_voxels.push_back(current_voxel);
//   if (neg_ray) {
//     current_voxel+=diff;
//     visited_voxels.push_back(current_voxel);
//   }

//   while(last_voxel != current_voxel) {
//     if (tMaxX < tMaxY) {
//       if (tMaxX < tMaxZ) {
//         current_voxel[0] += stepX;
//         tMaxX += tDeltaX;
//       } else {
//         current_voxel[2] += stepZ;
//         tMaxZ += tDeltaZ;
//       }
//     } else {
//       if (tMaxY < tMaxZ) {
//         current_voxel[1] += stepY;
//         tMaxY += tDeltaY;
//       } else {
//         current_voxel[2] += stepZ;
//         tMaxZ += tDeltaZ;
//       }
//     }
//     visited_voxels.push_back(current_voxel);
//   }
//   return visited_voxels;
// }

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