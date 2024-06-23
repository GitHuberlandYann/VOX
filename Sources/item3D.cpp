/* this file handles display of items floating on the ground */

#include "Blocks.hpp"
#include "utils.h"

/**
 * pos = center - front - right
 */
void Block::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	int spec = (texX(face_dir::minus_x, face_dir::minus_x << offset::blocks::orientation) << 4) + (texY(face_dir::minus_x, face_dir::minus_x << offset::blocks::orientation) << 12) + (3 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec, 16, 16, 0, 8);

	spec = (texX(face_dir::plus_x, face_dir::minus_x << offset::blocks::orientation) << 4) + (texY(face_dir::plus_x, face_dir::minus_x << offset::blocks::orientation) << 12) + (4 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec, 16, 16, 0, 8);

	spec = (texX(face_dir::minus_y, face_dir::minus_x << offset::blocks::orientation) << 4) + (texY(face_dir::minus_y, face_dir::minus_x << offset::blocks::orientation) << 12) + (1 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec, 16, 16, 0, 8);

	spec = (texX(face_dir::plus_y, face_dir::minus_x << offset::blocks::orientation) << 4) + (texY(face_dir::plus_y, face_dir::minus_x << offset::blocks::orientation) << 12) + (2 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec, 16, 16, 0, 8);

	spec = (texX(face_dir::plus_z, face_dir::minus_x << offset::blocks::orientation) << 4) + (texY(face_dir::plus_z, face_dir::minus_x << offset::blocks::orientation) << 12) + (0 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec, 16, 16, 0, 8);

	spec = (texX(face_dir::minus_z, face_dir::minus_x << offset::blocks::orientation) << 4) + (texY(face_dir::minus_z, face_dir::minus_x << offset::blocks::orientation) << 12) + (5 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec, 16, 16, 0, 8);
}

void Slab::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	up *= 0.5f;

	int spec = (textureX << 4) + (textureY << 12) + (3 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec + (8 << 8), 16, 8, 0, 8);

	spec += (1 << 19);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec + (8 << 8), 16, 8, 0, 8);

	spec -= (3 << 19);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec + (8 << 8), 16, 8, 0, 8);

	spec += (1 << 19);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec + (8 << 8), 16, 8, 0, 8);

	spec -= (2 << 19);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec, 16, 16, 0, 8);

	spec += (5 << 19);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec, 16, 16, 0, 8);
}

void Trapdoor::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	up *= 3.f * one16th;

	int spec = (textureX << 4) + (textureY << 12) + (3 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec + (13 << 8), 16, 3, 0, 8);

	spec += (1 << 19);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec + (13 << 8), 16, 3, 0, 8);

	spec -= (3 << 19);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec + (13 << 8), 16, 3, 0, 8);

	spec += (1 << 19);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec + (13 << 8), 16, 3, 0, 8);

	spec -= (2 << 19);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec, 16, 16, 0, 8);

	spec += (5 << 19);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec, 16, 16, 0, 8);
}

void Button::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	pos += (front * 6.f + right * 4.f) * one16th * size;
	front *= 4.f * one16th;
	right *= 6.f * one16th;
	up *= 2.f * one16th;

	int spec = (textureX << 4) + (textureY << 12) + (3 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec + 4 + (14 << 8), 6, 2, 0, 8);

	spec += (1 << 19);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec + 4 + (14 << 8), 6, 2, 0, 8);

	spec -= (3 << 19);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec + 6 + (14 << 8), 4, 2, 0, 8);

	spec += (1 << 19);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec + 6 + (14 << 8), 4, 2, 0, 8);

	spec -= (2 << 19);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec + 4 + (6 << 8), 6, 4, 0, 8);

	spec += (5 << 19);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec + 4 + (6 << 8), 6, 4, 0, 8);
}

void Fence::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	pos += (front * 6.f + right * 6.f) * one16th * size;
	front *= 4.f * one16th;
	right *= 4.f * one16th;

	int spec = (textureX << 4) + (textureY << 12) + (3 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec + 6, 4, 16, 0, 8);

	spec += (1 << 19);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec + 6, 4, 16, 0, 8);

	spec -= (3 << 19);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec + 6, 4, 16, 0, 8);

	spec += (1 << 19);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec + 6, 4, 16, 0, 8);

	spec -= (2 << 19);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec + 6 + (6 << 8), 4, 4, 0, 8);

	spec += (5 << 19);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec + 6 + (6 << 8), 4, 4, 0, 8);
}

void StairsBottom::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	int spec = (textureX << 4) + (textureY << 12) + (3 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + up * size, pos + (right * .5f + up) * size, pos + up * .5f * size, pos + (right + up) * .5f * size}, spec, 8, 8, 0, 8); // top left corner
	utils::shader::addQuads(arr, {pos + up * .5f * size, pos + (right + up * .5f) * size, pos, pos + right * size}, spec + (8 << 8), 16, 8, 0, 8);

	spec += (1 << 19);
	utils::shader::addQuads(arr, {pos + (front + right * .5f + up) * size, pos + (front + up) * size, pos + (front + (right + up) * .5f) * size, pos + (front + up * .5f) * size}, spec + 8, 8, 8, 0, 8); // top right corner
	utils::shader::addQuads(arr, {pos + (front + right + up * .5f) * size, pos + (front + up * .5f) * size, pos + (front + right) * size, pos + front * size}, spec + (8 << 8), 16, 8, 0, 8);

	spec -= (3 << 19);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec, 16, 16, 0, 8);

	spec += (1 << 19);
	utils::shader::addQuads(arr, {pos + (right + up * .5f) * size, pos + (front + right + up * .5f) * size, pos + right * size, pos + (front + right) * size}, spec + (8 << 8), 16, 8, 0, 8);

	// top of second step
	spec -= (2 << 19);
	utils::shader::addQuads(arr, {pos + up * size, pos + (front + up) * size, pos + (right * .5f + up) * size, pos + (front + right * .5f + up) * size}, spec, 16, 8, 0, 8);
    // top of first step
	utils::shader::addQuads(arr, {pos + (right + up) * .5f * size, pos + (front + (right + up) * .5f) * size, pos + (right + up * .5f) * size, pos + (front + right + up * .5f) * size}, spec + (8 << 8), 16, 8, 0, 8);
    // front of second step
	spec += (3 << 19);
	utils::shader::addQuads(arr, {pos + (right * .5f + up) * size, pos + (front + right * .5f + up) * size, pos + (right + up) * .5f * size, pos + (front + (right + up) * .5f) * size}, spec, 16, 8, 0, 8);

	spec += (2 << 19);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec, 16, 16, 0, 8);
}

void Chest::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	int spec = (1 << 4) + 1 + (15 << 12) + (3 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec, 14, 15, 0, 8);

	spec = spec - (1 << 4) + (1 << 19);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec, 14, 15, 0, 8);

	spec -= (3 << 19);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec, 14, 15, 0, 8);

	spec += (1 << 19);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec, 14, 15, 0, 8);

	spec = spec + (2 << 4) - (2 << 19);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec, 14, 14, 0, 8);

	spec += (5 << 19);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec, 14, 14, 0, 8);

	// display lock
	const float one15th = 1.f / 15.f;
	const float one14th = 1.f / 14.f;
	pos += (right * 6.f * one14th + up * 8.f * one15th) * size - front * one14th * size;
	size *= 4.f * one15th;
	spec = (5 << 4) + (15 << 12) + (3 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + up * size, pos + (right * .5f + up) * size, pos, pos + right * .5f * size}, spec, 2, 4, 0, 8);

	spec -= (2 << 19);
	utils::shader::addQuads(arr, {pos + (front * .25f + up) * size, pos + up * size, pos + front * .25f * size, pos}, spec, 1, 4, 0, 8);

	spec += (1 << 19);
	utils::shader::addQuads(arr, {pos + (right * .5f + up) * size, pos + (front * .25f + right * .5f + up) * size, pos + right * .5f * size, pos + (front * .5f + right) * .5f * size}, spec + 1, 1, 4, 0, 8);

	spec -= (2 << 19);
	utils::shader::addQuads(arr, {pos + (front * .25f + up) * size, pos + (front * .25f + right * .5f + up) * size, pos + up * size, pos + (right * .5f + up) * size}, spec, 2, 1, 0, 8);

	spec += (5 << 19);
	utils::shader::addQuads(arr, {pos, pos + right * .5f * size, pos + front * .25f * size, pos + (front * .5f + right) * .5f * size}, spec + (3 << 8), 2, 1, 0, 8);
}

void Piston::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	int spec = (texX(face_dir::minus_x, face_dir::plus_z << offset::blocks::orientation) << 4) + (texY(face_dir::minus_x, face_dir::plus_z << offset::blocks::orientation) << 12) + (3 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec, 16, 16, 0, 8);

	spec = (texX(face_dir::plus_x, face_dir::plus_z << offset::blocks::orientation) << 4) + (texY(face_dir::plus_x, face_dir::plus_z << offset::blocks::orientation) << 12) + (4 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec, 16, 16, 0, 8);

	spec = (texX(face_dir::minus_y, face_dir::plus_z << offset::blocks::orientation) << 4) + (texY(face_dir::minus_y, face_dir::plus_z << offset::blocks::orientation) << 12) + (1 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec, 16, 16, 0, 8);

	spec = (texX(face_dir::plus_y, face_dir::plus_z << offset::blocks::orientation) << 4) + (texY(face_dir::plus_y, face_dir::plus_z << offset::blocks::orientation) << 12) + (2 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec, 16, 16, 0, 8);

	spec = (texX(face_dir::plus_z, face_dir::plus_z << offset::blocks::orientation) << 4) + (texY(face_dir::plus_z, face_dir::plus_z << offset::blocks::orientation) << 12) + (0 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec, 16, 16, 0, 8);

	spec = (texX(face_dir::minus_z, face_dir::plus_z << offset::blocks::orientation) << 4) + (texY(face_dir::minus_z, face_dir::plus_z << offset::blocks::orientation) << 12) + (5 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec, 16, 16, 0, 8);
}

void ItemFrame::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	pos += (right + up) * 2.f * one16th;
	size *= 12.f * one16th;
	const float one12th = 1.f / 12.f;

	// draw wooden frame out of birch wood
	int spec = (s_blocks[blocks::birch_planks]->texX() << 4) + (s_blocks[blocks::birch_planks]->texY() << 12) + (3 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos + up * 11.f * one12th * size, pos + (right + up * 11.f * one12th) * size}, spec + 2 + (2 << 8), 12, 1, 0, 8); // up
	utils::shader::addQuads(arr, {pos + up * one12th * size, pos + (right + up * one12th) * size, pos, pos + right * size}, spec + 2 + (13 << 8), 12, 1, 0, 8); // down
	utils::shader::addQuads(arr, {pos + up * 11.f * one12th * size, pos + (right + up * 11.f) * one12th * size, pos + up * one12th * size, pos + (right + up) * one12th * size}, spec + 2 + (3 << 8), 1, 10, 0, 8); // left
	utils::shader::addQuads(arr, {pos + (right + up) * 11.f * one12th * size, pos + (right + up * 11.f * one12th) * size, pos + (right * 11.f + up) * one12th * size, pos + (right + up * one12th) * size}, spec + 13 + (3 << 8), 1, 10, 0, 8); // right

	spec += (1 << 19);
	utils::shader::addQuads(arr, {pos + (front * one12th + right + up) * size, pos + (front * one12th + up) * size, pos + (front * one12th + right) * size, pos + front * one12th * size}, spec + 2 + (2 << 8), 12, 12, 0, 8);

	spec -= (3 << 19);
	utils::shader::addQuads(arr, {pos + (front * one12th + up) * size, pos + up * size, pos + front * one12th * size, pos}, spec + 2 + (2 << 8), 1, 12, 0, 8);
	utils::shader::addQuads(arr, {pos + (front + (right + up) * 11.f) * one12th * size, pos + (right + up) * 11.f * one12th * size, pos + (front + right * 11.f + up) * one12th * size, pos + (right * 11.f + up) * one12th * size}, spec + 12 + (3 << 8), 1, 10, 0, 8);

	spec += (1 << 19);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front * one12th + right + up) * size, pos + right * size, pos + (front * one12th + right) * size}, spec + 13 + (2 << 8), 1, 12, 0, 8);
	utils::shader::addQuads(arr, {pos + (right + up * 11.f) * one12th * size, pos + (front + right + up * 11.f) * one12th * size, pos + (right + up) * one12th * size, pos + (front + right + up) * one12th * size}, spec + 3 + (3 << 8), 1, 10, 0, 8);

	spec -= (2 << 19);
	utils::shader::addQuads(arr, {pos + (front * one12th + up) * size, pos + (front * one12th + right + up) * size, pos + up * size, pos + (right + up) * size}, spec + 2 + (2 << 8), 12, 1, 0, 8);
	utils::shader::addQuads(arr, {pos + (front + right + up) * one12th * size, pos + (front + right * 11.f + up) * one12th * size, pos + (right + up) * one12th * size, pos + (right * 11.f + up) * one12th * size}, spec + 3 + (12 << 8), 10, 1, 0, 8);

	spec += (5 << 19);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * one12th * size, pos + (front * one12th + right) * size}, spec + 2 + (13 << 8), 12, 1, 0, 8);
	utils::shader::addQuads(arr, {pos + (right + up * 11.f) * one12th * size, pos + (right + up) * 11.f * one12th * size, pos + (front + right + up * 11.f) * one12th * size, pos + (front + (right + up) * 11.f) * one12th * size}, spec + 3 + (3 << 8), 10, 1, 0, 8);

	// draw frame background
	pos += front * .5f * one16th;
	spec = (texX() << 4) + (textureY << 12) + (3 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec + 2 + (2 << 8), 12, 12, 0, 8);
}

void Carpet::addMeshItem( std::vector<t_shaderInput>& arr, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size ) const
{
	up *= one16th;

	int spec = (textureX << 4) + (textureY << 12) + (3 << 19) + (light << 24);
	utils::shader::addQuads(arr, {pos + up * size, pos + (right + up) * size, pos, pos + right * size}, spec + (15 << 8), 16, 1, 0, 8);

	spec += (1 << 19);
	utils::shader::addQuads(arr, {pos + (front + right + up) * size, pos + (front + up) * size, pos + (front + right) * size, pos + front * size}, spec + (15 << 8), 16, 1, 0, 8);

	spec -= (3 << 19);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + up * size, pos + front * size, pos}, spec + (15 << 8), 16, 1, 0, 8);

	spec += (1 << 19);
	utils::shader::addQuads(arr, {pos + (right + up) * size, pos + (front + right + up) * size, pos + right * size, pos + (front + right) * size}, spec + (15 << 8), 16, 1, 0, 8);

	spec -= (2 << 19);
	utils::shader::addQuads(arr, {pos + (front + up) * size, pos + (front + right + up) * size, pos + up * size, pos + (right + up) * size}, spec, 16, 16, 0, 8);

	spec += (5 << 19);
	utils::shader::addQuads(arr, {pos, pos + right * size, pos + front * size, pos + (front + right) * size}, spec, 16, 16, 0, 8);
}
