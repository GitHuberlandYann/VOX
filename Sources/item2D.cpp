/* this file handles display of items in inventory */

#include "Blocks.hpp"
#include "Ui.hpp"

void Block::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    int spec = getTex(face_dir::minus_x, blocks::item) + (12 << 24) + (depth << 22);
	ui->addFace({glm::ivec2(x, y), {x + width * gui_size, y}, {x, y + width * gui_size},
		{x + width * gui_size, y + width * gui_size}}, spec, 16, 16, alien, movement);
}

void Cube::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    // top face
	int spec = (15 << 24) + getTex(face_dir::plus_z, face_dir::plus_x) + (depth << 22);
	ui->addFace({glm::ivec2(x, y + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, y},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f},
		{x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f}},
		spec, 16, 16, alien, movement);
	// left face
	spec = (10 << 24) + getTex(face_dir::minus_y, face_dir::plus_x) + (depth << 22);
	ui->addFace({glm::ivec2(x, y + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f},
		{x, y + width * gui_size * 281.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size}},
		spec, 16, 16, alien, movement);
	// right face
	spec = (7 << 24) + getTex(face_dir::plus_x, face_dir::plus_x) + (depth << 22);
	ui->addFace({glm::ivec2(x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f),
		{x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size},
		{x + (width - 2) * gui_size, y + width * gui_size * 281.25f / 362.5f}},
		spec, 16, 16, alien, movement);
}

void Glass::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    // top face
	int spec = (15 << 24) + settings::consts::shader::block::glass + (depth << 22);
	ui->addFace({glm::ivec2(x, y + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, y},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f},
		{x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f}},
		spec, 16, 16, alien, movement);
	// left face
	spec = (10 << 24) + settings::consts::shader::block::glass + (depth << 22);
	ui->addFace({glm::ivec2(x, y + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f},
		{x, y + width * gui_size * 281.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size}},
		spec, 16, 16, alien, movement);
	// right face
	spec = (7 << 24) + settings::consts::shader::block::glass + (depth << 22);
	ui->addFace({glm::ivec2(x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f),
		{x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size},
		{x + (width - 2) * gui_size, y + width * gui_size * 281.25f / 362.5f}},
		spec, 16, 16, alien, movement);
}

void OakLog::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    // top face
	int spec = (15 << 24) + settings::consts::shader::block::oak_log_top + (depth << 22);
	ui->addFace({glm::ivec2(x, y + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, y},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f},
		{x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f}},
		spec, 16, 16, alien, movement);
	// left face
	spec = (10 << 24) + settings::consts::shader::block::oak_log + (depth << 22);
	ui->addFace({glm::ivec2(x, y + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f},
		{x, y + width * gui_size * 281.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size}},
		spec, 16, 16, alien, movement);
	// right face
	spec = (7 << 24) + settings::consts::shader::block::oak_log + (depth << 22);
	ui->addFace({glm::ivec2(x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f),
		{x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size},
		{x + (width - 2) * gui_size, y + width * gui_size * 281.25f / 362.5f}},
		spec, 16, 16, alien, movement);
}

void Slab::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    int ytop = y + width * gui_size * 100.0f / 362.5f;
    // top face
	int spec = (15 << 24) + texture + (depth << 22);
	ui->addFace({glm::ivec2(x, ytop + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, ytop},
		{x + (width - 2) * 0.5f * gui_size, ytop + width * gui_size * 162.5f / 362.5f},
		{x + (width - 2) * gui_size, ytop + width * gui_size * 81.25f / 362.5f}},
		spec, 16, 16, alien, movement);
	// left face
	spec = (10 << 24) + texture + (depth << 22);
	ui->addFace({glm::ivec2(x, ytop + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, ytop + width * gui_size * 162.5f / 362.5f},
		{x, y + width * gui_size * 281.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size}},
		spec + (8 << 16), 16, 8, alien, movement);
	// right face
	spec = (7 << 24) + texture + (depth << 22);
	ui->addFace({glm::ivec2(x + (width - 2) * 0.5f * gui_size, ytop + width * gui_size * 162.5f / 362.5f),
		{x + (width - 2) * gui_size, ytop + width * gui_size * 81.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size},
		{x + (width - 2) * gui_size, y + width * gui_size * 281.25f / 362.5f}},
		spec + (8 << 16), 16, 8, alien, movement);
}

void Stairs::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    int spec = (15 << 24) + texture + (depth << 22);
	// top of second step
	ui->addFace({glm::ivec2(x, y + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, y},
		{x + (width - 2) * 0.25f * gui_size, y + width * gui_size * 121.875f / 362.5f},
		{x + (width - 2) * 0.75f * gui_size, y + width * gui_size * 40.625f / 362.5f}},
		spec, 16, 16, alien, movement);
	// top of first step
	ui->addFace({glm::ivec2(x + (width - 2) * 0.25f * gui_size, y + width * gui_size * 221.875f / 362.5f),
		{x + (width - 2) * 0.75f * gui_size, y + width * gui_size * 140.624f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 262.5f / 362.5f},
		{x + 13 * gui_size, y + width * gui_size * 181.25f / 362.5f}},
		spec, 16, 16, alien, movement);
	// front of second step
	spec -= (8 << 24);
	ui->addFace({glm::ivec2(x + (width - 2) * 0.25f * gui_size, y + width * gui_size * 121.875f / 362.5f),
		{x + (width - 2) * 0.75f * gui_size, y + width * gui_size * 40.625f / 362.5f},
		{x + (width - 2) * 0.25f * gui_size, y + width * gui_size * 221.875f / 362.5f},
		{x + (width - 2) * 0.75f * gui_size, y + width * gui_size * 140.624f / 362.5f}},
		spec, 16, 16, alien, movement);
	// front of first step
	ui->addFace({glm::ivec2(x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 262.5f / 362.5f),
		{x + 13 * gui_size, y + width * gui_size * 181.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size},
		{x + 13 * gui_size, y + width * gui_size * 281.25f / 362.5f}},
		spec, 16, 16, alien, movement);
	// left face (top corner + bottom horizontal)
	spec += (3 << 24);
	ui->addFace({glm::ivec2(x, y + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.25f * gui_size, y + width * gui_size * 121.875f / 362.5f},
		{x, y + width * gui_size * 181.25f / 362.5f},
		{x + (width - 2) * 0.25f * gui_size, y + width * gui_size * 221.875f / 362.5f}},
		spec, 8, 8, alien, movement);
	ui->addFace({glm::ivec2(x, y + width * gui_size * 181.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 262.5f / 362.5f},
		{x, y + width * gui_size * 281.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size}},
		spec + (8 << 16), 16, 8, alien, movement);
}

void Fence::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    int spec = (15 << 24) + settings::consts::shader::block::oak_planks + (depth << 22);
	// top of left pole
	ui->addFace({glm::ivec2(x + width * gui_size * 3.0f * one16th, y + width * gui_size * 3.66f * one16th),
		{x + width * gui_size * 5.0f * one16th, y + width * gui_size * 3.0f * one16th},
		{x + width * gui_size * 5.0f * one16th, y + width * gui_size * 4.33f * one16th},
		{x + width * gui_size * 7.0f * one16th, y + width * gui_size * 3.66f * one16th}},
		spec + (6 << 12), 4, 4, alien, movement);
	// top of right pole
	ui->addFace({glm::ivec2(x + width * gui_size * 9.0f * one16th, y + width * gui_size * 1.66f * one16th),
		{x + width * gui_size * 11.0f * one16th, y + width * gui_size * 1.0f * one16th},
		{x + width * gui_size * 11.0f * one16th, y + width * gui_size * 2.33f * one16th},
		{x + width * gui_size * 13.0f * one16th, y + width * gui_size * 1.66f * one16th}},
		spec + (6 << 12), 4, 4, alien, movement);
	// left of left pole
	spec -= (5 << 24);
	ui->addFace({glm::ivec2(x + width * gui_size * 3.0f * one16th, y + width * gui_size * 3.66f * one16th),
		{x + width * gui_size * 5.0f * one16th, y + width * gui_size * 4.33f * one16th},
		{x + width * gui_size * 3.0f * one16th, y + width * gui_size * 14.33f * one16th},
		{x + width * gui_size * 5.0f * one16th, y + width * gui_size * 15.0f * one16th}},
		spec + (6 << 12), 4, 16, alien, movement);
	// left of right pole
	ui->addFace({glm::ivec2(x + width * gui_size * 9.0f * one16th, y + width * gui_size * 1.66f * one16th),
		{x + width * gui_size * 11.0f * one16th, y + width * gui_size * 2.33f * one16th},
		{x + width * gui_size * 9.0f * one16th, y + width * gui_size * 12.33f * one16th},
		{x + width * gui_size * 11.0f * one16th, y + width * gui_size * 13.0f * one16th}},
		spec + (6 << 12), 4, 16, alien, movement);
	// right of left pole
	ui->addFace({glm::ivec2(x + width * gui_size * 5.0f * one16th, y + width * gui_size * 4.33f * one16th),
		{x + width * gui_size * 7.0f * one16th, y + width * gui_size * 3.66f * one16th},
		{x + width * gui_size * 5.0f * one16th, y + width * gui_size * 15.0f * one16th},
		{x + width * gui_size * 7.0f * one16th, y + width * gui_size * 14.33f * one16th}},
		spec + (6 << 12), 4, 16, alien, movement);
	spec -= (3 << 24);
	// right of right pole
	ui->addFace({glm::ivec2(x + width * gui_size * 11.0f * one16th, y + width * gui_size * 2.33f * one16th),
		{x + width * gui_size * 13.0f * one16th, y + width * gui_size * 1.66f * one16th},
		{x + width * gui_size * 11.0f * one16th, y + width * gui_size * 13.0f * one16th},
		{x + width * gui_size * 13.0f * one16th, y + width * gui_size * 12.33f * one16th}},
		spec + (6 << 12), 4, 16, alien, movement);
	// top of top bar
	spec += (8 << 24);
	ui->addFace({glm::ivec2(x + width * gui_size * 6.5f * one16th, y + width * gui_size * 3.66f * one16th),
		{x + width * gui_size * 9.5f * one16th, y + width * gui_size * 2.66f * one16th},
		{x + width * gui_size * 7.5f * one16th, y + width * gui_size * 4.0f * one16th},
		{x + width * gui_size * 10.5f * one16th, y + width * gui_size * 3.0f * one16th}},
		spec + (1 << 16), 16, 2, alien, movement);
	// top of bottom bar
	ui->addFace({glm::ivec2(x + width * gui_size * 7.0f * one16th, y + width * gui_size * 7.5f * one16th),
		{x + width * gui_size * 9.5f * one16th, y + width * gui_size * 6.66f * one16th},
		{x + width * gui_size * 7.5f * one16th, y + width * gui_size * 8.0f * one16th},
		{x + width * gui_size * 10.5f * one16th, y + width * gui_size * 7.0f * one16th}},
		spec + (1 << 16), 16, 2, alien, movement);
	// right of top bar
	spec -= (8 << 24);
	ui->addFace({glm::ivec2(x + width * gui_size * 7.0f * one16th, y + width * gui_size * 4.16f * one16th),
		{x + width * gui_size * 10.5f * one16th, y + width * gui_size * 3.0f * one16th},
		{x + width * gui_size * 7.0f * one16th, y + width * gui_size * 6.16f * one16th},
		{x + width * gui_size * 10.5f * one16th, y + width * gui_size * 5.0f * one16th}},
		spec + (1 << 16), 16, 3, alien, movement);
	// right of bottom bar
	ui->addFace({glm::ivec2(x + width * gui_size * 7.0f * one16th, y + width * gui_size * 8.16f * one16th),
		{x + width * gui_size * 10.5f * one16th, y + width * gui_size * 7.0f * one16th},
		{x + width * gui_size * 7.0f * one16th, y + width * gui_size * 10.16f * one16th},
		{x + width * gui_size * 10.5f * one16th, y + width * gui_size * 9.0f * one16th}},
		spec + (1 << 16), 16, 3, alien, movement);
}

void Trapdoor::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    int ytop = y + width * gui_size * 162.5f / 362.5f;
	// top face
	int spec = (15 << 24) + texture + (depth << 22);
	ui->addFace({glm::ivec2(x, ytop + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, ytop},
		{x + (width - 2) * 0.5f * gui_size, ytop + width * gui_size * 162.5f / 362.5f},
		{x + (width - 2) * gui_size, ytop + width * gui_size * 81.25f / 362.5f}},
		spec, 16, 16, alien, movement);
	// left face
	spec = (10 << 24) + texture + (depth << 22);
	ui->addFace({glm::ivec2(x, ytop + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, ytop + width * gui_size * 162.5f / 362.5f},
		{x, y + width * gui_size * 281.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size}},
		spec + (13 << 16), 16, 3, alien, movement);
	// right face
	spec = (7 << 24) + texture + (depth << 22);
	ui->addFace({glm::ivec2(x + (width - 2) * 0.5f * gui_size, ytop + width * gui_size * 162.5f / 362.5f),
		{x + (width - 2) * gui_size, ytop + width * gui_size * 81.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size},
		{x + (width - 2) * gui_size, y + width * gui_size * 281.25f / 362.5f}},
		spec + (13 << 16), 16, 3, alien, movement);
}

void Button::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size + (width - 2) * gui_size * 122.0f / 362.5f; 
	y += width * .5f * gui_size * 122.0f / 362.5f; 
	// top face
	int spec = (15 << 24) + texture + (depth << 22);
	ui->addFace({glm::ivec2(x, y + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, y},
		{x + (width - 2) * gui_size * 40.5125f / 362.5f, y + width * gui_size * 121.7125f / 362.5f},
		{x + (width - 2) * gui_size * 203.0125f / 362.5f, y + width * gui_size * 40.5125f / 362.5f}},
		spec + (4 << 12) + (14 << 16), 6, 2, alien, movement);
	// left face
	spec -= (5 << 24);
	ui->addFace({glm::ivec2(x, y + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * gui_size * 40.5125f / 362.5f, y + width * gui_size * 121.7125f / 362.5f},
		{x, y + width * gui_size * 206.7125f / 362.5f},
		{x + (width - 2) * gui_size * 40.5125f / 362.5f, y + width * gui_size * 247.225f / 362.5f}},
		spec + (6 << 12) + (14 << 16), 4, 2, alien, movement);
	// right face
	spec -= (3 << 24);
	ui->addFace({glm::ivec2(x + (width - 2) * gui_size * 40.5125f / 362.5f, y + width * gui_size * 121.7125f / 362.5f),
		{x + (width - 2) * gui_size * 203.0125f / 362.5f, y + width * gui_size * 40.5125f / 362.5f},
		{x + (width - 2) * gui_size * 40.5125f / 362.5f, y + width * gui_size * 246.7125f / 362.5f},
		{x + (width - 2) * gui_size * 203.0125f / 362.5f, y + width * gui_size * 165.5125f / 362.5f}},
		spec + (4 << 12) + (6 << 16), 6, 4, alien, movement);
}

void Piston::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    // top face
	int spec = (15 << 24) + getTex(face_dir::plus_z, face_dir::plus_z << offset::blocks::orientation) + (depth << 22);
	ui->addFace({glm::ivec2(x, y + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, y},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f},
		{x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f}},
		spec, 16, 16, alien, movement);
	// left face
	spec = (10 << 24) + getTex(face_dir::minus_y, face_dir::plus_z << offset::blocks::orientation) + (depth << 22);
	ui->addFace({glm::ivec2(x, y + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f},
		{x, y + width * gui_size * 281.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size}},
		spec, 16, 16, alien, movement);
	// right face
	spec = (7 << 24) + getTex(face_dir::plus_x, face_dir::plus_z << offset::blocks::orientation) + (depth << 22);
	ui->addFace({glm::ivec2(x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f),
		{x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size},
		{x + (width - 2) * gui_size, y + width * gui_size * 281.25f / 362.5f}},
		spec, 16, 16, alien, movement);
}

void Observer::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    // top face
	int spec = (15 << 24) + getTex(face_dir::plus_z, face_dir::plus_x << offset::blocks::orientation) + (depth << 22);
	ui->addFace({glm::ivec2(x, y + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, y},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f},
		{x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f}},
		spec, 16, 16, alien, movement);
	// left face
	spec = (10 << 24) + getTex(face_dir::minus_y, face_dir::plus_x << offset::blocks::orientation) + (depth << 22);
	ui->addFace({glm::ivec2(x, y + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f},
		{x, y + width * gui_size * 281.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size}},
		spec, 16, 16, alien, movement);
	// right face
	spec = (7 << 24) + getTex(face_dir::plus_x, face_dir::plus_x << offset::blocks::orientation) + (depth << 22);
	ui->addFace({glm::ivec2(x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f),
		{x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size},
		{x + (width - 2) * gui_size, y + width * gui_size * 281.25f / 362.5f}},
		spec, 16, 16, alien, movement);
}

void Chest::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    // top face
    int spec = (15 << 24) + settings::consts::shader::block::chest_top + (depth << 22);
	ui->addFace({glm::ivec2(x, y + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, y},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f},
		{x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f}},
		spec, 14, 14, alien, movement);
    // left face
    spec = (10 << 24) + settings::consts::shader::block::chest_side + (depth << 22);
	ui->addFace({glm::ivec2(x, y + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f},
		{x, y + width * gui_size * 281.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size}},
		spec, 14, 15, alien, movement);
    // right face
    spec = (13 << 24) + settings::consts::shader::block::chest_front + (depth << 22);
	ui->addFace({glm::ivec2(x + (width - 2) * 0.5f * gui_size, y + width * gui_size * 162.5f / 362.5f),
		{x + (width - 2) * gui_size, y + width * gui_size * 81.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size},
		{x + (width - 2) * gui_size, y + width * gui_size * 281.25f / 362.5f}},
		spec, 14, 15, alien, movement);
}

void Carpet::addItem( UI *ui, int x, int y, int gui_size, int width, int depth, bool alien, bool movement ) const
{
    x += gui_size;
    int ytop = y + width * gui_size * 175.0f / 362.5f;
	// top face
	int spec = (15 << 24) + texture + (depth << 22);
	ui->addFace({glm::ivec2(x, ytop + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, ytop},
		{x + (width - 2) * 0.5f * gui_size, ytop + width * gui_size * 162.5f / 362.5f},
		{x + (width - 2) * gui_size, ytop + width * gui_size * 81.25f / 362.5f}},
		spec, 16, 16, alien, movement);
	// left face
	spec = (10 << 24) + texture + (depth << 22);
	ui->addFace({glm::ivec2(x, ytop + width * gui_size * 81.25f / 362.5f),
		{x + (width - 2) * 0.5f * gui_size, ytop + width * gui_size * 162.5f / 362.5f},
		{x, y + width * gui_size * 281.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size}},
		spec + (15 << 16), 16, 1, alien, movement);
	// right face
	spec = (7 << 24) + texture + (depth << 22);
	ui->addFace({glm::ivec2(x + (width - 2) * 0.5f * gui_size, ytop + width * gui_size * 162.5f / 362.5f),
		{x + (width - 2) * gui_size, ytop + width * gui_size * 81.25f / 362.5f},
		{x + (width - 2) * 0.5f * gui_size, y + width * gui_size},
		{x + (width - 2) * gui_size, y + width * gui_size * 281.25f / 362.5f}},
		spec + (15 << 16), 16, 1, alien, movement);
}
