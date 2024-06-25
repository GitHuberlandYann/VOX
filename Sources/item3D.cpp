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
