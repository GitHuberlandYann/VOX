/* this file handles display of items floating on the ground */

#include "Blocks.hpp"
#include "utils.h"

/**
 * pos = center - front - right
 */
void Block::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	int spec = getTex(face_dir::minus_x, face_dir::minus_x << offset::blocks::orientation);
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec, light + (3 << 8), 16, 16);

	spec = getTex(face_dir::plus_x, face_dir::minus_x << offset::blocks::orientation);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec, light + (4 << 8), 16, 16);

	spec = getTex(face_dir::minus_y, face_dir::minus_x << offset::blocks::orientation);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec, light + (1 << 8), 16, 16);

	spec = getTex(face_dir::plus_y, face_dir::minus_x << offset::blocks::orientation);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec, light + (2 << 8), 16, 16);

	spec = getTex(face_dir::plus_z, face_dir::minus_x << offset::blocks::orientation);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec, light + (0 << 8), 16, 16);

	spec = getTex(face_dir::minus_z, face_dir::minus_x << offset::blocks::orientation);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec, light + (5 << 8), 16, 16);
}

void Slab::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	up *= 0.5f;

	int spec = texture;
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec + (8 << 16), light + (3 << 8), 16, 8);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec + (8 << 16), light + (4 << 8), 16, 8);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec + (8 << 16), light + (1 << 8), 16, 8);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec + (8 << 16), light + (2 << 8), 16, 8);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec, light + (0 << 8), 16, 16);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec, light + (5 << 8), 16, 16);
}

void Cactus::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	int spec = settings::consts::shader::block::cactus_side;
	utils::shader::addQuads(arr, {pos + (front * one16th + up) * size, pos + (front * one16th + right + up) * size, pos + front * one16th * size, pos + (front * one16th + right) * size}, spec, light + (3 << 8), 16, 16);
	utils::shader::addQuads(arr, {pos + (front * 15.f * one16th + right + up) * size, pos + (front * 15.f * one16th + up) * size, pos + (front * 15.f * one16th + right) * size, pos + front * 15.f * one16th * size}, spec, light + (4 << 8), 16, 16);
	utils::shader::addQuads(arr, {pos + (front + right * one16th + up) * size, pos + (right * one16th + up) * size, pos + (front + right * one16th) * size, pos + right * one16th * size}, spec, light + (1 << 8), 16, 16);
	utils::shader::addQuads(arr, {pos + (right * 15.f * one16th + up) * size, pos + (front + right * 15.f * one16th + up) * size, pos + right * 15.f * one16th * size, pos + (front + right * 15.f * one16th) * size}, spec, light + (2 << 8), 16, 16);
	spec = settings::consts::shader::block::cactus_top;
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec, light + (0 << 8), 16, 16);
	spec = settings::consts::shader::block::cactus_bottom;
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec, light + (5 << 8), 16, 16);
}

void Trapdoor::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	up *= 3.f * one16th;

	int spec = texture;
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec + (13 << 16), light + (3 << 8), 16, 3);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec + (13 << 16), light + (4 << 8), 16, 3);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec + (13 << 16), light + (1 << 8), 16, 3);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec + (13 << 16), light + (2 << 8), 16, 3);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec, light + (0 << 8), 16, 16);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec, light + (5 << 8), 16, 16);
}

void Button::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	pos += (front * 6.f + right * 4.f) * one16th * size;
	front *= 4.f * one16th;
	right *= 6.f * one16th;
	up *= 2.f * one16th;

	int spec = texture;
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec + (4 << 12) + (14 << 16), light + (3 << 8), 6, 2);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec + (4 << 12) + (14 << 16), light + (4 << 8), 6, 2);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec + (6 << 12) + (14 << 16), light + (1 << 8), 4, 2);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec + (6 << 12) + (14 << 16), light + (2 << 8), 4, 2);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec + (4 << 12) + (6 << 16), light + (0 << 8), 6, 4);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec + (4 << 12) + (6 << 16), light + (5 << 8), 6, 4);
}

void Fence::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	pos += (front * 6.f + right * 6.f) * one16th * size;
	front *= 4.f * one16th;
	right *= 4.f * one16th;

	int spec = texture;
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec + (6 << 12), light + (3 << 8), 4, 16);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec + (6 << 12), light + (4 << 8), 4, 16);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec + (6 << 12), light + (1 << 8), 4, 16);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec + (6 << 12), light + (2 << 8), 4, 16);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec + (6 << 12) + (6 << 16), light + (0 << 8), 4, 4);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec + (6 << 12) + (6 << 16), light + (5 << 8), 4, 4);
}

void Stairs::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	int spec = texture;
	utils::shader::addQuads(arr, {pos + up * size, pos + (right * .5f + up) * size, pos + up * .5f * size, pos + (right + up) * .5f * size}, spec, light + (3 << 8), 8, 8); // top left corner
	utils::shader::addQuads(arr, {pos + up * .5f * size, pos + (right + up * .5f) * size, pos, pos + right * size}, spec + (8 << 16), light + (3 << 8), 16, 8);

	utils::shader::addQuads(arr, {pos + (front + right * .5f + up) * size, pos + (front + up) * size, pos + (front + (right + up) * .5f) * size, pos + (front + up * .5f) * size}, spec + (8 << 12), light + (4 << 8), 8, 8); // top right corner
	utils::shader::addQuads(arr, {pos + (front + right + up * .5f) * size, pos + (front + up * .5f) * size, pos + (front + right) * size, pos + front * size}, spec + (8 << 16), light + (4 << 8), 16, 8);

	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec, light + (1 << 8), 16, 16);

	utils::shader::addQuads(arr, {pos + (right + up * .5f) * size, pos + (front + right + up * .5f) * size, pos + right * size, pos + (front + right) * size}, spec + (8 << 16), light + (2 << 8), 16, 8);

	// top of second step
	utils::shader::addQuads(arr, {pos + up * size, pos + (front + up) * size, pos + (right * .5f + up) * size, pos + (front + right * .5f + up) * size}, spec, light + (0 << 8), 16, 8);
    // top of first step
	utils::shader::addQuads(arr, {pos + (right + up) * .5f * size, pos + (front + (right + up) * .5f) * size, pos + (right + up * .5f) * size, pos + (front + right + up * .5f) * size}, spec + (8 << 16), light + (0 << 8), 16, 8);
    // front of second step
	utils::shader::addQuads(arr, {pos + (right * .5f + up) * size, pos + (front + right * .5f + up) * size, pos + (right + up) * .5f * size, pos + (front + (right + up) * .5f) * size}, spec, light + (3 << 8), 16, 8);

	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec, light + (5 << 8), 16, 16);
}

void Chest::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	int spec = settings::consts::shader::block::chest_front;
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec, light + (3 << 8), 14, 15);

	spec = settings::consts::shader::block::chest_side;
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec, light + (4 << 8), 14, 15);

	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec, light + (1 << 8), 14, 15);

	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec, light + (2 << 8), 14, 15);

	spec = settings::consts::shader::block::chest_top;
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec, light + (0 << 8), 14, 14);

	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec, light + (5 << 8), 14, 14);

	// display lock
	const float one15th = 1.f / 15.f;
	const float one14th = 1.f / 14.f;
	pos += (right * 6.f * one14th + up * 8.f * one15th) * size - front * one14th * size;
	size *= 4.f * one15th;
	spec = settings::consts::shader::block::chest_front + (14 << 12);
	utils::shader::addQuads(arr, {pos + up * size, pos + (right * .5f + up) * size, pos, pos + right * .5f * size}, spec, light + (3 << 8), 2, 4);
	utils::shader::addQuads(arr, {pos + (front * .25f + up) * size, pos + up * size, pos + front * .25f * size, pos}, spec, light + (4 << 8), 1, 4);
	utils::shader::addQuads(arr, {pos + (right * .5f + up) * size, pos + (front * .25f + right * .5f + up) * size, pos + right * .5f * size, pos + (front * .5f + right) * .5f * size}, spec + (1 << 12), light + (1 << 8), 1, 4);
	utils::shader::addQuads(arr, {pos + (front * .25f + up) * size, pos + (front * .25f + right * .5f + up) * size, pos + up * size, pos + (right * .5f + up) * size}, spec, light + (0 << 8), 2, 1);
	utils::shader::addQuads(arr, {pos, pos + right * .5f * size, pos + front * .25f * size, pos + (front * .5f + right) * .5f * size}, spec + (3 << 16), light + (5 << 8), 2, 1);
}

void Piston::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	int spec = getTex(face_dir::minus_x, face_dir::plus_z << offset::blocks::orientation);
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec, light + (3 << 8), 16, 16);

	spec = getTex(face_dir::plus_x, face_dir::plus_z << offset::blocks::orientation);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec, light + (4 << 8), 16, 16);

	spec = getTex(face_dir::minus_y, face_dir::plus_z << offset::blocks::orientation);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec, light + (1 << 8), 16, 16);

	spec = getTex(face_dir::plus_y, face_dir::plus_z << offset::blocks::orientation);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec, light + (2 << 8), 16, 16);

	spec = getTex(face_dir::plus_z, face_dir::plus_z << offset::blocks::orientation);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec, light + (0 << 8), 16, 16);

	spec = getTex(face_dir::minus_z, face_dir::plus_z << offset::blocks::orientation);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec, light + (5 << 8), 16, 16);
}

void ItemFrame::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	pos += (right + up) * 2.f * one16th;
	size *= 12.f * one16th;
	const float one12th = 1.f / 12.f;

	// draw wooden frame out of birch wood
	int spec = s_blocks[blocks::birch_planks]->getTex();
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos + up * 11.f * one12th * size, pos + (right + up * 11.f * one12th) * size}, spec + (2 << 12) + (2 << 16), light + (3 << 8), 12, 1);; // up
	utils::shader::addQuads(arr, {pos + up * one12th * size, pos + (right + up * one12th) * size, pos, pos + right * size}, spec + (2 << 12) + (13 << 16), light + (3 << 8), 12, 1);; // down
	utils::shader::addQuads(arr, {pos + up * 11.f * one12th * size, pos + (right + up * 11.f) * one12th * size, pos + up * one12th * size, pos + (right + up) * one12th * size}, spec + (2 << 12) + (3 << 16), light + (3 << 8), 1, 10);; // left
	utils::shader::addQuads(arr, {pos + (right + up) * 11.f * one12th * size, pos + (right + up * 11.f * one12th) * size, pos + (right * 11.f + up) * one12th * size, pos + (right + up * one12th) * size}, spec + (13 << 12) + (3 << 16), light + (3 << 8), 1, 10);; // right

	utils::shader::addQuads(arr, {pos + (front * one12th + right + up) * size, pos + (front * one12th + up) * size, pos + (front * one12th + right) * size, pos + front * one12th * size}, spec + (2 << 12) + (2 << 16), light + (4 << 8), 12, 12);;

	utils::shader::addQuads(arr, {pos + (front * one12th + up) * size, pos + up * size, pos + front * one12th * size, pos}, spec + (2 << 12) + (2 << 16), light + (1 << 8), 1, 12);;
	utils::shader::addQuads(arr, {pos + (front + (right + up) * 11.f) * one12th * size, pos + (right + up) * 11.f * one12th * size, pos + (front + right * 11.f + up) * one12th * size, pos + (right * 11.f + up) * one12th * size}, spec + (12 << 12) + (3 << 16), light + (1 << 8), 1, 10);;

	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front * one12th + right + up) * size, pos + right * size, pos + (front * one12th + right) * size}, spec + (13 << 12) + (2 << 16), light + (2 << 8), 1, 12);;
	utils::shader::addQuads(arr, {pos + (right + up * 11.f) * one12th * size, pos + (front + right + up * 11.f) * one12th * size, pos + (right + up) * one12th * size, pos + (front + right + up) * one12th * size}, spec + (3 << 12) + (3 << 16), light + (2 << 8), 1, 10);;

	utils::shader::addQuads(arr, {pos + (front * one12th + up) * size, pos + (front * one12th + right + up) * size, pos + up * size, pos + (right + up) * size}, spec + (2 << 12) + (2 << 16), light + (0 << 8), 12, 1);;
	utils::shader::addQuads(arr, {pos + (front + right + up) * one12th * size, pos + (front + right * 11.f + up) * one12th * size, pos + (right + up) * one12th * size, pos + (right * 11.f + up) * one12th * size}, spec + (3 << 12) + (12 << 16), light + (0 << 8), 10, 1);;

	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * one12th * size, pos + (front * one12th + right) * size}, spec + (2 << 12) + (13 << 16), light + (5 << 8), 12, 1);;
	utils::shader::addQuads(arr, {pos + (right + up * 11.f) * one12th * size, pos + (right + up) * 11.f * one12th * size, pos + (front + right + up * 11.f) * one12th * size, pos + (front + (right + up) * 11.f) * one12th * size}, spec + (3 << 12) + (3 << 16), light + (5 << 8), 10, 1);;

	// draw frame background
	pos += front * .5f * one16th;
	spec = texture;
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec + (2 << 12) + (2 << 16), light + (3 << 8), 12, 12);
}

void Lectern::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	// draw base
	up *= 2.f * one16th;
	int spec = settings::consts::shader::block::oak_planks;
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec + (14 << 16), light + (4 << 8), 16, 2);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec + (14 << 16), light + (1 << 8), 16, 2);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec + (14 << 16), light + (2 << 8), 16, 2);
	spec = settings::consts::shader::block::lectern_base;
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec + (14 << 16), light + (3 << 8), 16, 2);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec, light + (0 << 8), 16, 16);
	spec = settings::consts::shader::block::oak_planks;
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec, light + (5 << 8), 16, 16);
	// draw column
	up *= 8.f;
	pos += ((front + right) * 4.f + up) * one16th * size;
	front *= .5f;
	right *= .5f;
	spec = settings::consts::shader::block::lectern_front;
	utils::shader::addQuads(arr, {pos + up * 10.f * one16th * size, pos + (right + up * 10.f * one16th) * size, pos, pos + right * size}, spec + (4 << 16), light + (3 << 8), 8, 10);
	utils::shader::addQuads(arr, {pos + (front + right + up * 14.f * one16th) * size, pos + (front + up * 14.f * one16th) * size, pos + (front + right) * size, pos + front * size}, spec + (8 << 12) + (2 << 16), light + (4 << 8), 8, 14);
	spec = settings::consts::shader::block::lectern_sides;
	utils::shader::addQuads(arr, {pos + up * 14.f * one16th * size, pos, pos + (front + up * 14.f * one16th) * size, pos + front * size}, spec + (2 << 12) + (8 << 16), light + (1 << 8), 14, 8, false, true);
	utils::shader::addQuads(arr, {pos + (front + right + up * 14.f * one16th) * size, pos + (front + right) * size, pos + (right + up * 14.f * one16th) * size, pos + right * size}, spec + (2 << 12) + (8 << 16), light + (2 << 8), 14, 8);
	// draw top
	front *= 2.f;
	right *= 2.f;
	pos += (-front * 2.f - right * 4.f + up * 9.f) * one16th * size;
	front = glm::normalize(front + .5f * up);
	up = glm::normalize(glm::cross(right, front));
	up *= .25f;
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec, light + (3 << 8), 16, 4);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec + (4 << 16), light + (4 << 8), 16, 4);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec + (4 << 16), light + (1 << 8), 16, 4);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec + (4 << 16), light + (2 << 8), 16, 4);
	spec = settings::consts::shader::block::lectern_top;
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec, light + (0 << 8), 16, 16);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec, light + (5 << 8), 16, 16);
}

void Anvil::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	// right is anvil's long axis
	// draw base 12x12x4
	pos += (front + right) * 2.f * one16th * size;
	front *= .75f; // 12.f * one16th;
	right *= .75f; // 12.f * one16th;
	up *= .25f; // 4.f * one16th;
	int spec = settings::consts::shader::block::anvil;
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec + (2 << 12) + (12 << 16), light + (3 << 8), 12, 4);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec + (2 << 12) + (12 << 16), light + (4 << 8), 12, 4);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec + (2 << 12) + (12 << 16), light + (2 << 8), 12, 4);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec + (2 << 12) + (12 << 16), light + (2 << 8), 12, 4);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec + (2 << 12) + (2 << 16), light + (0 << 8), 12, 12);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec + (2 << 12) + (2 << 16), light + (5 << 8), 12, 12);
	front *= 4.f / 3.f;
	right *= 4.f / 3.f;
	up *= 4.f;
	// draw basebis 10x8x1
	pos += (front * 2.f + right + up * 4.f) * one16th * size;
	front *= .5f; //8.f * one16th;
	right *= 10.f * one16th;
	up *= one16th;
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec + (3 << 12) + (11 << 16), light + (3 << 8), 10, 1);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec + (3 << 12) + (11 << 16), light + (4 << 8), 10, 1);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec + (4 << 12) + (11 << 16), light + (1 << 8), 8, 1);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec + (4 << 12) + (11 << 16), light + (2 << 8), 8, 1);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec + (3 << 12) + (4 << 16), light + (0 << 8), 10, 8);
	front *= 2.f;
	right *= 1.6f;
	up *= 16.f;
	// draw column 8x4x5
	pos += (front * 2.f + right + up) * one16th * size;
	front *= .25f; // 4.f * one16th;
	right *= .5f; // 8.f * one16th;
	up *= 5.f * one16th;
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec + (4 << 12) + (6 << 16), light + (3 << 8), 8, 5);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec + (4 << 12) + (6 << 16), light + (4 << 8), 8, 5);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec + (6 << 12) + (6 << 16), light + (1 << 8), 4, 5);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec + (6 << 12) + (6 << 16), light + (2 << 8), 4, 5);
	front *= 4.f;
	right *= 2.f;
	up *= 3.2f; //16.f / 5.f;
	// draw top 16x10x6
	pos += (-front * 3.f - right * 4.f + up * 5.f) * one16th * size;
	front *= 10.f * one16th;
	up *= 6.f * one16th;
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec, light + (3 << 8), 16, 6);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec, light + (4 << 8), 16, 6);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec + (3 << 12), light + (2 << 8), 10, 6);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec + (3 << 12), light + (2 << 8), 10, 6);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec + (3 << 16), light + (5 << 8), 16, 10);
	spec = settings::consts::shader::block::anvil_top;
	utils::shader::addQuads(arr, {pos + up * size, pos + (front + up) * size, pos + (right + up) * size, pos + (front + right + up) * size}, spec + (3 << 12), light + (0 << 8), 10, 16);
}

void Carpet::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	up *= one16th;

	int spec = texture;
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec + (15 << 16), light + (3 << 8), 16, 1);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec + (15 << 16), light + (4 << 8), 16, 1);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec + (15 << 16), light + (1 << 8), 16, 1);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec + (15 << 16), light + (2 << 8), 16, 1);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec, light + (0 << 8), 16, 16);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec, light + (5 << 8), 16, 16);
}
