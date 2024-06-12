/* this file handles display of items in inventory */

#include "Blocks.hpp"
#include "Ui.hpp"

void Block::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    int spec = (texX(face_dir::minus_x, blocks::item) << 4) + (texY(face_dir::minus_x, blocks::item) << 12) + (3 << 19) + (0xF << 24) + (depth << 16);
	glm::ivec3 v0 = {spec, x, y};
	glm::ivec3 v1 = {spec + (1 << 4), x + width * gui_size, y};
	glm::ivec3 v2 = {spec + (1 << 12), x, y + width * gui_size};
	glm::ivec3 v3 = {spec + (1 << 4) + (1 << 12), x + width * gui_size, y + width * gui_size};
	ui->addFace(v0, v1, v2, v3, alien, movement);
}

void Cube::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    // top face
	int spec = (15 << 24) + (texX(face_dir::plus_z, face_dir::plus_x) << 4) + (texY(face_dir::plus_z, face_dir::plus_x) << 12) + (depth << 16);
	glm::ivec3 v0 = {spec, x, y + width * gui_size * 81.25f / 362.5f};
	glm::ivec3 v1 = {spec + (1 << 4), x + (width - 2) * 0.5f * gui_size, y};
	glm::ivec3 v2 = {spec + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f};
	glm::ivec3 v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// left face
	spec = (10 << 24) + (texX(face_dir::minus_y, face_dir::plus_x) << 4) + (texY(face_dir::minus_y, face_dir::plus_x) << 12) + (depth << 16);
	v0 = {spec, x, y + width * gui_size * 81.25f / 362.5f};
	v1 = {spec + (1 << 4), x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f};
	v2 = {spec + (1 << 12), x, y + width * gui_size * 281.25f / 362.5f};
	v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// right face
	spec = (7 << 24) + (texX(face_dir::plus_x, face_dir::plus_x) << 4) + (texY(face_dir::plus_x, face_dir::plus_x) << 12) + (depth << 16);
	v0 = {spec, x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f};
	v1 = {spec + (1 << 4), x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f};
	v2 = {spec + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size};
	v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * gui_size, y + width * gui_size * 281.25f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
}

void Glass::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    // top face
	int spec = (15 << 24) + (texX(face_dir::plus_z, face_dir::plus_x) << 4) + (texY(face_dir::plus_z, face_dir::plus_x) << 12) + (depth << 16);
	glm::ivec3 v0 = {spec, x, y + width * gui_size * 81.25f / 362.5f};
	glm::ivec3 v1 = {spec + (1 << 4), x + (width - 2) * 0.5f * gui_size, y};
	glm::ivec3 v2 = {spec + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f};
	glm::ivec3 v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// left face
	spec = (10 << 24) + (texX(face_dir::minus_y, face_dir::plus_x) << 4) + (texY(face_dir::minus_y, face_dir::plus_x) << 12) + (depth << 16);
	v0 = {spec, x, y + width * gui_size * 81.25f / 362.5f};
	v1 = {spec + (1 << 4), x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f};
	v2 = {spec + (1 << 12), x, y + width * gui_size * 281.25f / 362.5f};
	v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// right face
	spec = (7 << 24) + (texX(face_dir::plus_x, face_dir::plus_x) << 4) + (texY(face_dir::plus_x, face_dir::plus_x) << 12) + (depth << 16);
	v0 = {spec, x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f};
	v1 = {spec + (1 << 4), x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f};
	v2 = {spec + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size};
	v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * gui_size, y + width * gui_size * 281.25f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
}

void OakLog::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    // top face
	int spec = (15 << 24) + (texX(face_dir::plus_z, face_dir::plus_z << offset::blocks::orientation) << 4) + (texY(face_dir::plus_z, face_dir::plus_z << offset::blocks::orientation) << 12) + (depth << 16);
    glm::ivec3 v0 = {spec, x, y + width * gui_size * 81.25f / 362.5f};
	glm::ivec3 v1 = {spec + (1 << 4), x + (width - 2) * 0.5f * gui_size, y};
	glm::ivec3 v2 = {spec + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f};
	glm::ivec3 v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// left face
	spec = (10 << 24) + (texX(face_dir::minus_y, face_dir::plus_z << offset::blocks::orientation) << 4) + (texY(face_dir::minus_y, face_dir::plus_z << offset::blocks::orientation) << 12) + (depth << 16);
	v0 = {spec, x, y + width * gui_size * 81.25f / 362.5f};
	v1 = {spec + (1 << 4), x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f};
	v2 = {spec + (1 << 12), x, y + width * gui_size * 281.25f / 362.5f};
	v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// right face
	spec = (7 << 24) + (texX(face_dir::plus_x, face_dir::plus_z << offset::blocks::orientation) << 4) + (texY(face_dir::plus_x, face_dir::plus_z << offset::blocks::orientation) << 12) + (depth << 16);
	v0 = {spec, x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f};
	v1 = {spec + (1 << 4), x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f};
	v2 = {spec + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size};
	v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * gui_size, y + width * gui_size * 281.25f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
}

void SlabBottom::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    int ytop = y + width * gui_size * 100.0f / 362.5f;
	// top face
	int spec = (15 << 24) + (texX(face_dir::plus_z, face_dir::plus_x) << 4) + (texY(face_dir::plus_z, face_dir::plus_x) << 12) + (depth << 16);
	glm::ivec3 v0 = {spec, x, ytop + width * gui_size * 81.25f / 362.5f};
	glm::ivec3 v1 = {spec + (1 << 4), x + (width - 2) * 0.5f * gui_size, ytop};
	glm::ivec3 v2 = {spec + (1 << 12), x + (width - 2) * 0.5f * gui_size, ytop + width * gui_size * 162.5f / 362.5f};
	glm::ivec3 v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * gui_size, ytop + width * gui_size * 81.25f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// left face
	spec = (10 << 24) + (texX(face_dir::minus_y, face_dir::plus_x) << 4) + (texY(face_dir::minus_y, face_dir::plus_x) << 12) + (depth << 16);
	v0 = {spec + (8 << 8), x, ytop + width * gui_size * 81.25f / 362.5f};
	v1 = {spec + (8 << 8) + (1 << 4), x + (width - 2) * 0.5f * gui_size, ytop + width * gui_size * 162.5f / 362.5f};
	v2 = {spec + (1 << 12), x, y + width * gui_size * 281.25f / 362.5f};
	v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// right face
	spec = (7 << 24) + (texX(face_dir::plus_x, face_dir::plus_x) << 4) + (texY(face_dir::plus_x, face_dir::plus_x) << 12) + (depth << 16);
	v0 = {spec + (8 << 8), x + (width - 2) * 0.5f * gui_size, ytop + width * gui_size * 162.5f / 362.5f};
	v1 = {spec + (8 << 8) + (1 << 4), x + (width - 2) * gui_size, ytop + width * gui_size * 81.25f / 362.5f};
	v2 = {spec + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size};
	v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * gui_size, y + width * gui_size * 281.25f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
}

void StairsBottom::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    int spec = (15 << 24) + (texX() << 4) + (texY() << 12) + (depth << 16);
	// top of second step
	glm::ivec3 v0 = {spec, x, y + width * gui_size * 81.25f / 362.5f};
	glm::ivec3 v1 = {spec + (1 << 4), x + (width - 2) * 0.5f * gui_size, y};
	glm::ivec3 v2 = {spec + (1 << 12), x + (width - 2) * 0.25f * gui_size, y + width * gui_size * 121.875f / 362.5f};
	glm::ivec3 v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * 0.75f * gui_size, y + width * gui_size * 40.625f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// top of first step
	v0 = {spec, x + (width - 2) * 0.25f * gui_size, y + width * gui_size * 221.875f / 362.5f};
	v1 = {spec + (1 << 4), x + (width - 2) * 0.75f * gui_size, y + width * gui_size * 140.624f / 362.5f};
	v2 = {spec + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 262.5f / 362.5f};
	v3 = {spec + (1 << 4) + (1 << 12), x + 13 * gui_size, y + width * gui_size * 181.25f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// front of second step
	spec -= (8 << 24);
	v0 = {spec, x + (width - 2) * 0.25f * gui_size, y + width * gui_size * 121.875f / 362.5f};
	v1 = {spec + (1 << 4), x + (width - 2) * 0.75f * gui_size, y + width * gui_size * 40.625f / 362.5f};
	v2 = {spec + (1 << 12), x + (width - 2) * 0.25f * gui_size, y + width * gui_size * 221.875f / 362.5f};
	v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * 0.75f * gui_size, y + width * gui_size * 140.624f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// front of first step
	v0 = {spec, x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 262.5f / 362.5f};
	v1 = {spec + (1 << 4), x + 13 * gui_size, y + width * gui_size * 181.25f / 362.5f};
	v2 = {spec + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size};
	v3 = {spec + (1 << 4) + (1 << 12), x + 13 * gui_size, y + width * gui_size * 281.25f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// left face (top corner + bottom horizontal)
	spec += (3 << 24);
	v0 = {spec, x, y + width * gui_size * 81.25f / 362.5f};
	v1 = {spec + 8, x + (width - 2) * 0.25f * gui_size, y + width * gui_size * 121.875f / 362.5f};
	v2 = {spec + (8 << 8), x, y + width * gui_size * 181.25f / 362.5f};
	v3 = {spec + 8 + (8 << 8), x + (width - 2) * 0.25f * gui_size, y + width * gui_size * 221.875f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	v0 = {spec + (8 << 8), x, y + width * gui_size * 181.25f / 362.5f};
	v1 = {spec + (1 << 4) + (8 << 8), x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 262.5f / 362.5f};
	v2 = {spec + (1 << 12), x, y + width * gui_size * 281.25f / 362.5f};
	v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size};
	ui->addFace(v0, v1, v2, v3, alien, movement);
}

void Fence::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    int spec = (15 << 24) + (s_blocks[blocks::oak_planks]->texX() << 4) + (s_blocks[blocks::oak_planks]->texY() << 12) + (depth << 16);
	// top of left pole
	glm::ivec3 v0 = {spec + 6, x + width * gui_size * 3.0f * one16th, y + width * gui_size * 3.66f * one16th};
	glm::ivec3 v1 = {spec + 10, x + width * gui_size * 5.0f * one16th, y + width * gui_size * 3.0f * one16th};
	glm::ivec3 v2 = {spec + 6 + (4 << 8), x + width * gui_size * 5.0f * one16th, y + width * gui_size * 4.33f * one16th};
	glm::ivec3 v3 = {spec + 10 + (4 << 8), x + width * gui_size * 7.0f * one16th, y + width * gui_size * 3.66f * one16th};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// top of right pole
	v0 = {spec + 6, x + width * gui_size * 9.0f * one16th, y + width * gui_size * 1.66f * one16th};
	v1 = {spec + 10, x + width * gui_size * 11.0f * one16th, y + width * gui_size * 1.0f * one16th};
	v2 = {spec + 6 + (4 << 8), x + width * gui_size * 11.0f * one16th, y + width * gui_size * 2.33f * one16th};
	v3 = {spec + 10 + (4 << 8), x + width * gui_size * 13.0f * one16th, y + width * gui_size * 1.66f * one16th};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// left of left pole
	spec -= (5 << 24);
	v0 = {spec + 6, x + width * gui_size * 3.0f * one16th, y + width * gui_size * 3.66f * one16th};
	v1 = {spec + 10, x + width * gui_size * 5.0f * one16th, y + width * gui_size * 4.33f * one16th};
	v2 = {spec + 6 + (1 << 12), x + width * gui_size * 3.0f * one16th, y + width * gui_size * 14.33f * one16th};
	v3 = {spec + 10 + (1 << 12), x + width * gui_size * 5.0f * one16th, y + width * gui_size * 15.0f * one16th};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// left of right pole
	v0 = {spec + 6, x + width * gui_size * 9.0f * one16th, y + width * gui_size * 1.66f * one16th};
	v1 = {spec + 10, x + width * gui_size * 11.0f * one16th, y + width * gui_size * 2.33f * one16th};
	v2 = {spec + 6 + (1 << 12), x + width * gui_size * 9.0f * one16th, y + width * gui_size * 12.33f * one16th};
	v3 = {spec + 10 + (1 << 12), x + width * gui_size * 11.0f * one16th, y + width * gui_size * 13.0f * one16th};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// right of left pole
	spec -= (3 << 24);
	v0 = {spec + 6, x + width * gui_size * 5.0f * one16th, y + width * gui_size * 4.33f * one16th};
	v1 = {spec + 10, x + width * gui_size * 7.0f * one16th, y + width * gui_size * 3.66f * one16th};
	v2 = {spec + 6 + (1 << 12), x + width * gui_size * 5.0f * one16th, y + width * gui_size * 15.0f * one16th};
	v3 = {spec + 10 + (1 << 12), x + width * gui_size * 7.0f * one16th, y + width * gui_size * 14.33f * one16th};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// right of right pole
	v0 = {spec + 6, x + width * gui_size * 11.0f * one16th, y + width * gui_size * 2.33f * one16th};
	v1 = {spec + 10, x + width * gui_size * 13.0f * one16th, y + width * gui_size * 1.66f * one16th};
	v2 = {spec + 6 + (1 << 12), x + width * gui_size * 11.0f * one16th, y + width * gui_size * 13.0f * one16th};
	v3 = {spec + 10 + (1 << 12), x + width * gui_size * 13.0f * one16th, y + width * gui_size * 12.33f * one16th};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// top of top bar
	spec += (8 << 24);
	v0 = {spec + (1 << 8), x + width * gui_size * 6.5f * one16th, y + width * gui_size * 3.66f * one16th};
	v1 = {spec + (1 << 4) + (1 << 8), x + width * gui_size * 9.5f * one16th, y + width * gui_size * 2.66f * one16th};
	v2 = {spec + (3 << 8), x + width * gui_size * 7.5f * one16th, y + width * gui_size * 4.0f * one16th};
	v3 = {spec + (1 << 4) + (3 << 8), x + width * gui_size * 10.5f * one16th, y + width * gui_size * 3.0f * one16th};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// top of bottom bar
	v0 = {spec + (1 << 8), x + width * gui_size * 7.0f * one16th, y + width * gui_size * 7.5f * one16th};
	v1 = {spec + (1 << 4) + (1 << 8), x + width * gui_size * 9.5f * one16th, y + width * gui_size * 6.66f * one16th};
	v2 = {spec + (3 << 8), x + width * gui_size * 7.5f * one16th, y + width * gui_size * 8.0f * one16th};
	v3 = {spec + (1 << 4) + (3 << 8), x + width * gui_size * 10.5f * one16th, y + width * gui_size * 7.0f * one16th};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// right of top bar
	spec -= (8 << 24);
	v0 = {spec + (1 << 8), x + width * gui_size * 7.0f * one16th, y + width * gui_size * 4.16f * one16th};
	v1 = {spec + (1 << 4) + (1 << 8), x + width * gui_size * 10.5f * one16th, y + width * gui_size * 3.0f * one16th};
	v2 = {spec + (4 << 8), x + width * gui_size * 7.0f * one16th, y + width * gui_size * 6.16f * one16th};
	v3 = {spec + (1 << 4) + (4 << 8), x + width * gui_size * 10.5f * one16th, y + width * gui_size * 5.0f * one16th};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// right of bottom bar
	v0 = {spec + (1 << 8), x + width * gui_size * 7.0f * one16th, y + width * gui_size * 8.16f * one16th};
	v1 = {spec + (1 << 4) + (1 << 8), x + width * gui_size * 10.5f * one16th, y + width * gui_size * 7.0f * one16th};
	v2 = {spec + (4 << 8), x + width * gui_size * 7.0f * one16th, y + width * gui_size * 10.16f * one16th};
	v3 = {spec + (1 << 4) + (4 << 8), x + width * gui_size * 10.5f * one16th, y + width * gui_size * 9.0f * one16th};
	ui->addFace(v0, v1, v2, v3, alien, movement);
}

void Trapdoor::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    int ytop = y + width * gui_size * 162.5f / 362.5f;
	// top face
	int spec = (15 << 24) + (texX(face_dir::plus_z, face_dir::plus_x) << 4) + (texY(face_dir::plus_z, face_dir::plus_x) << 12) + (depth << 16);
	glm::ivec3 v0 = {spec, x, ytop + width * gui_size * 81.25f / 362.5f};
	glm::ivec3 v1 = {spec + (1 << 4), x + (width - 2) * 0.5f * gui_size, ytop};
	glm::ivec3 v2 = {spec + (1 << 12), x + (width - 2) * 0.5f * gui_size, ytop + width * gui_size * 162.5f / 362.5f};
	glm::ivec3 v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * gui_size, ytop + width * gui_size * 81.25f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// left face
	spec = (10 << 24) + (texX(face_dir::minus_y, face_dir::plus_x) << 4) + (texY(face_dir::minus_y, face_dir::plus_x) << 12) + (depth << 16);
	v0 = {spec + (13 << 8), x, ytop + width * gui_size * 81.25f / 362.5f};
	v1 = {spec + (13 << 8) + (1 << 4), x + (width - 2) * 0.5f * gui_size, ytop + width * gui_size * 162.5f / 362.5f};
	v2 = {spec + (1 << 12), x, y + width * gui_size * 281.25f / 362.5f};
	v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// right face
	spec = (7 << 24) + (texX(face_dir::plus_x, face_dir::plus_x) << 4) + (texY(face_dir::plus_x, face_dir::plus_x) << 12) + (depth << 16);
	v0 = {spec + (13 << 8), x + (width - 2) * 0.5f * gui_size, ytop + width * gui_size * 162.5f / 362.5f};
	v1 = {spec + (13 << 8) + (1 << 4), x + (width - 2) * gui_size, ytop + width * gui_size * 81.25f / 362.5f};
	v2 = {spec + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size};
	v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * gui_size, y + width * gui_size * 281.25f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
}

void Button::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += width + (width - 2) * 122.0f / 362.5f; 
	y += width * 122.0f / 362.5f; 
	// top face
	int spec = (15 << 24) + (texX() << 4) + (texY() << 12) + (depth << 16);
	glm::ivec3 v0 = {spec, x, y + width * gui_size * 81.25f / 362.5f};
	glm::ivec3 v1 = {spec + (1 << 4), x + (width - 2) * 0.5f * gui_size, y};
	glm::ivec3 v2 = {spec + (1 << 12), x + (width - 2) * gui_size * 40.5125f / 362.5f, y + width * gui_size * 121.7125f / 362.5f};
	glm::ivec3 v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * gui_size * 203.0125f / 362.5f, y + width * gui_size * 40.5125f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// left face
	spec -= (5 << 24);
	v0 = {spec, x, y + width * gui_size * 81.25f / 362.5f};
	v1 = {spec + (1 << 4), x + (width - 2) * gui_size * 40.5125f / 362.5f, y + width * gui_size * 121.7125f / 362.5f};
	v2 = {spec + (1 << 12), x, y + width * gui_size * 206.7125f / 362.5f};
	v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * gui_size * 40.5125f / 362.5f, y + width * gui_size * 247.225f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// right face
	spec -= (3 << 24);
	v0 = {spec, x + (width - 2) * gui_size * 40.5125f / 362.5f, y + width * gui_size * 121.7125f / 362.5f};
	v1 = {spec + (1 << 4), x + (width - 2) * gui_size * 203.0125f / 362.5f, y + width * gui_size * 40.5125f / 362.5f};
	v2 = {spec + (1 << 12), x + (width - 2) * gui_size * 40.5125f / 362.5f, y + width * gui_size * 246.7125f / 362.5f};
	v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * gui_size * 203.0125f / 362.5f, y + width * gui_size * 165.5125f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
}

void Piston::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    // top face
	int spec = (15 << 24) + (texX(face_dir::plus_z, face_dir::plus_z << offset::blocks::orientation) << 4) + (texY(face_dir::plus_z, face_dir::plus_z << offset::blocks::orientation) << 12) + (depth << 16);
	glm::ivec3 v0 = {spec, x, y + width * gui_size * 81.25f / 362.5f};
	glm::ivec3 v1 = {spec + (1 << 4), x + (width - 2) * 0.5f * gui_size, y};
	glm::ivec3 v2 = {spec + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f};
	glm::ivec3 v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// left face
	spec = (10 << 24) + (texX(face_dir::minus_y, face_dir::plus_z << offset::blocks::orientation) << 4) + (texY(face_dir::minus_y, face_dir::plus_z << offset::blocks::orientation) << 12) + (depth << 16);
	v0 = {spec, x, y + width * gui_size * 81.25f / 362.5f};
	v1 = {spec + (1 << 4), x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f};
	v2 = {spec + (1 << 12), x, y + width * gui_size * 281.25f / 362.5f};
	v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// right face
	spec = (7 << 24) + (texX(face_dir::plus_x, face_dir::plus_z << offset::blocks::orientation) << 4) + (texY(face_dir::plus_x, face_dir::plus_z << offset::blocks::orientation) << 12) + (depth << 16);
	v0 = {spec, x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f};
	v1 = {spec + (1 << 4), x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f};
	v2 = {spec + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size};
	v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * gui_size, y + width * gui_size * 281.25f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
}

void Observer::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    // top face
	int spec = (15 << 24) + (texX(face_dir::plus_z, face_dir::plus_x << offset::blocks::orientation) << 4) + (texY(face_dir::plus_z, face_dir::plus_x << offset::blocks::orientation) << 12) + (depth << 16);
	glm::ivec3 v0 = {spec, x, y + width * gui_size * 81.25f / 362.5f};
	glm::ivec3 v1 = {spec + (1 << 4), x + (width - 2) * 0.5f * gui_size, y};
	glm::ivec3 v2 = {spec + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f};
	glm::ivec3 v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// left face
	spec = (10 << 24) + (texX(face_dir::minus_y, face_dir::plus_x << offset::blocks::orientation) << 4) + (texY(face_dir::minus_y, face_dir::plus_x << offset::blocks::orientation) << 12) + (depth << 16);
	v0 = {spec, x, y + width * gui_size * 81.25f / 362.5f};
	v1 = {spec + (1 << 4), x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f};
	v2 = {spec + (1 << 12), x, y + width * gui_size * 281.25f / 362.5f};
	v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size};
	ui->addFace(v0, v1, v2, v3, alien, movement);
	// right face
	spec = (7 << 24) + (texX(face_dir::plus_x, face_dir::plus_x << offset::blocks::orientation) << 4) + (texY(face_dir::plus_x, face_dir::plus_x << offset::blocks::orientation) << 12) + (depth << 16);
	v0 = {spec, x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f};
	v1 = {spec + (1 << 4), x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f};
	v2 = {spec + (1 << 12), x + (width - 2) * 0.5f * gui_size, y + width * gui_size};
	v3 = {spec + (1 << 4) + (1 << 12), x + (width - 2) * gui_size, y + width * gui_size * 281.25f / 362.5f};
	ui->addFace(v0, v1, v2, v3, alien, movement);
}

void Chest::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    // top face
    int spec = (15 << 24) + ((texX() + 2) << 4) + (texY() << 12) + (depth << 16);
    glm::ivec3 v0 = {spec + 1, x, y + width * gui_size * 81.25f / 362.5f};
    glm::ivec3 v1 = {spec + width, x + (width - 2) * 0.5f * gui_size, y};
    glm::ivec3 v2 = {spec + 1 + (14 << 8), x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f};
    glm::ivec3 v3 = {spec + width + (14 << 8), x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f};
    ui->addFace(v0, v1, v2, v3, alien, movement);
    // left face
    spec -= (5 << 24) + (2 << 4);
    v0 = {spec + 1, x, y + 15 * gui_size * 81.25f / 362.5f};
    v1 = {spec + 15, x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f};
    v2 = {spec + 1 + (15 << 8), x, y + width * gui_size * 281.25f / 362.5f};
    v3 = {spec + 15 + (15 << 8), x + (width - 2) * 0.5f * gui_size, y + width * gui_size};
    ui->addFace(v0, v1, v2, v3, alien, movement);
    // right face
    spec -= (3 << 24) - (1 << 4);
    v0 = {spec + 1, x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f};
    v1 = {spec + 15, x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f};
    v2 = {spec + 1 + (15 << 8), x + (width - 2) * 0.5f * gui_size, y + width * gui_size};
    v3 = {spec + 15 + (15 << 8), x + (width - 2) * gui_size, y + width * gui_size * 281.25f / 362.5f};
    ui->addFace(v0, v1, v2, v3, alien, movement);
}
