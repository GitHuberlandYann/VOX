#include "Chunk.hpp"
#include "random.hpp"
#include "utils.h"

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

const int neighbour8[8][2] = {
	{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}
};

// make use of 1 / floor(25 / points) + 1 to update crop
void Chunk::updateCrop( int value, int offset )
{
	int posZ = offset & (WORLD_HEIGHT - 1);
	int posY = ((offset >> WORLD_SHIFT) & (CHUNK_SIZE - 1));
	int posX = ((offset >> WORLD_SHIFT) >> CHUNK_SHIFT);
	// std::cout << _startX << ", " << _startY << ": update crop at " << posX << ", " << posY << ", " << posZ << std::endl;
	int light = getLightLevel(posX, posY, posZ);
	light = maxi(light & 0xF, (light >> 8) & 0xF);
	if (light < 9) { // crop requires light level of at least 9 to grow // TODO handle night light dimming
		// std::cout << "Failure. sky light " << ((light >> 8) & 0xF) << ", block light " << (light & 0xF) << std::endl;
		if (light <= 7) { // uproot // TODO crops can't be planted if light level <= 7
			remove_block(true, {posX, posY, posZ});
			_vertex_update = true;
		}
		return ;
	}
	float points = (getBlockAt(posX, posY, posZ - 1, false) & blocks::WET_FARMLAND) ? 4 : 2;
	for (int index = 0; index < 8; ++index) {
		const int delta[2] = {neighbour8[index][0], neighbour8[index][1]};
		int adj = getBlockAt(posX + delta[0], posY + delta[1], posZ - 1, true);
		if ((adj & 0xFF) == blocks::FARMLAND) {
			points += (adj & blocks::WET_FARMLAND) ? 0.75f : 0.25f;
		}
	} // TODO might want to handle diag crop
	float growth_probability = 1.0f / (glm::floor(25 / points) + 1);
	// std::cout << "points " << points << ", proba: " << growth_probability << "%" << std::endl;
	if (Random::randomFloat(_seed) <= growth_probability) {
		_blocks[offset] = value + 1;
		_added[offset] = value + 1;
		_vertex_update = true;
		// std::cout << "Success!" << std::endl;
	} else {
		// std::cout << "Failure." << std::endl;
	}
}

bool Chunk::watered_farmland( int posX, int posY, int posZ )
{
	for (int row = posX - 4; row <= posX + 4; row++) {
		for (int col = posY - 4; col <= posY + 4; col++) {
			for (int level = posZ; level <= posZ + 1; level++) {
				if ((getBlockAt(row, col, level, true) & 0xFF) >= blocks::WATER) {
					return (true);
				}
			}
		}
	}
	return (false);
}

void Chunk::updateFarmland( int value, int offset )
{
	int posZ = offset & (WORLD_HEIGHT - 1);
	int posY = ((offset >> WORLD_SHIFT) & (CHUNK_SIZE - 1));
	int posX = ((offset >> WORLD_SHIFT) >> CHUNK_SHIFT);
	bool hydrated = watered_farmland(posX, posY, posZ);
	if (!hydrated) {
		int wetness = (value >> 10) & 0x7;
		int type_above = getBlockAt(posX, posY, posZ + 1, false) & 0xFF;
		if (wetness == 1) {
			_blocks[offset] = value & 0xFF;
			_added[offset] = value & 0xFF;
			_vertex_update = true;
			return ;
		} else if (wetness > 1) {
			_blocks[offset] = (value & 0xFFE1FF) + ((wetness - 1) << 10);
			_added[offset] = (value & 0xFFE1FF) + ((wetness - 1) << 10);
			_removed.erase(offset);
			return ;
		} else if (type_above < blocks::WHEAT_CROP || type_above > blocks::WHEAT_CROP7) {
			// std::cout << "updateFarmland replace_block " << _startX << ", " << _startY << std::endl;
			replace_block(false, {posX, posY, posZ}, blocks::DIRT, blocks::FARMLAND);
			_vertex_update = true;
			return ;
		}
	} else if (!(value & blocks::WET_FARMLAND)) {
		_blocks[offset] = (value & 0xFF) + blocks::WET_FARMLAND + (0x7 << 10);
		_added[offset] = (value & 0xFF) + blocks::WET_FARMLAND + (0x7 << 10);
		_vertex_update = true;
	}
}

const int neighbour45[44][3] = {
	{-1, -1, 2}, {0, -1, 2}, {1, -1, 2}, {-1, 0, 2}, {0, 0, 2}, {1, 0, 2}, {-1, 1, 2}, {0, 1, 2}, {1, 1, 2},
	{-1, -1, 1}, {0, -1, 1}, {1, -1, 1}, {-1, 0, 1}, {0, 0, 1}, {1, 0, 1}, {-1, 1, 1}, {0, 1, 1}, {1, 1, 1},
	{-1, -1, 0}, {0, -1, 0}, {1, -1, 0}, {-1, 0, 0}, {1, 0, 0}, {-1, 1, 0}, {0, 1, 0}, {1, 1, 0},
	{-1, -1, -1}, {0, -1, -1}, {1, -1, -1}, {-1, 0, -1}, {0, 0, -1}, {1, 0, -1}, {-1, 1, -1}, {0, 1, -1}, {1, 1, -1},
	{-1, -1, -2}, {0, -1, -2}, {1, -1, -2}, {-1, 0, -2}, {0, 0, -2}, {1, 0, -2}, {-1, 1, -2}, {0, 1, -2}, {1, 1, -2}
};

void Chunk::spreadGrassblock( int offset )
{
	int posZ = offset & (WORLD_HEIGHT - 1);
	int posY = ((offset >> WORLD_SHIFT) & (CHUNK_SIZE - 1));
	int posX = ((offset >> WORLD_SHIFT) >> CHUNK_SHIFT);
	if (posZ < 254) {
		int above = _blocks[offset + 1] & 0xFF;
		if (!s_blocks[above]->transparent || above >= blocks::WATER) {
			_blocks[offset] = blocks::DIRT;
			_added[offset] = blocks::DIRT;
			_vertex_update = true;
			return ;
		}
	}
	int light = getLightLevel(posX, posY, posZ + 1);
	light = maxi(light & 0xF, (light >> 8) & 0xF);
	if (light < 9) { // grass requires light level of at least 9 to spread // TODO handle night light dimming
		if (light < 5) {
			_blocks[offset] = blocks::DIRT;
			_added[offset] = blocks::DIRT;
			_vertex_update = true;
		}
		return ;
	}
	for (int i = 0; i < 4; i++) {
		int selected = Random::rangedNumber(_seed, 0, 3 * 3 * 5 - 1);
		const int delta[3] = {neighbour45[selected][0], neighbour45[selected][1], neighbour45[selected][2]};
		int adj = getBlockAt(posX + delta[0], posY + delta[1], posZ + delta[2], true);
		if ((adj & 0xFF) == blocks::DIRT && !(adj & blocks::NOTVISIBLE)) {
			int above_adj = getBlockAt(posX + delta[0], posY + delta[1], posZ + delta[2] + 1, true) & 0xFF;
			if (s_blocks[above_adj]->transparent && above_adj < blocks::WATER) {
				turnDirtToGrass(posX + delta[0], posY + delta[1], posZ + delta[2]);
			}
		}
	}
}

bool Chunk::findLog( int posX, int posY, int posZ, int level )
{
	if (--level < 0) {
		return (false);
	}
	int type = getBlockAt(posX, posY, posZ, true) & 0xFF;
	if (type == blocks::OAK_LOG) {
		return (true);
	} else if (type != blocks::OAK_LEAVES) {
		return (false);
	}
	return (findLog(posX + 1, posY, posZ, level)
		|| findLog(posX - 1, posY, posZ, level)
		|| findLog(posX, posY + 1, posZ, level)
		|| findLog(posX, posY - 1, posZ, level)
		|| findLog(posX, posY, posZ + 1, level)
		|| findLog(posX, posY, posZ - 1, level));
}

void Chunk::decayLeaves( int offset )
{
	int posZ = offset & (WORLD_HEIGHT - 1);
	int posY = ((offset >> WORLD_SHIFT) & (CHUNK_SIZE - 1));
	int posX = ((offset >> WORLD_SHIFT) >> CHUNK_SHIFT);

	int logConnected = findLog(posX, posY, posZ, 6);
	if (!logConnected) {
		// std::cout << "decay leave " << _startX << ", " << _startY << std::endl;
		remove_block(true, {posX, posY, posZ}); // TODO might want to update ligth
		_vertex_update = true;
	}
}

/* returns wheter there is space above position to grow an oak tree (3x3x5 needed) 
 * dirt and grass_blocks can be in that space */
bool Chunk::spaceForTree( int posX, int posY, int posZ )
{
	for (int row = -1; row < 2; row++) {
		for (int col = -1; col < 2; col++) {
			for (int level = 1; level < 6; level++) {
				int type = getBlockAt(posX + row, posY + col, posZ + level, true) & 0xFF;
				if (air_flower(type, true, false, true) && type != blocks::GRASS_BLOCK && type != blocks::DIRT) {
					return (false);
				}
			}
		}
	}
	return (true);
}

void Chunk::growTree( int value, int offset )
{
	int posZ = offset & (WORLD_HEIGHT - 1);
	int posY = ((offset >> WORLD_SHIFT) & (CHUNK_SIZE - 1));
	int posX = ((offset >> WORLD_SHIFT) >> CHUNK_SHIFT);

	int light = getLightLevel(posX, posY, posZ);
	light = maxi(light & 0xF, (light >> 8) & 0xF);
	std::cout << "growTree at " << posX << ", " << posY << ", " << posZ << std::endl;
	if (light < 9 || !spaceForTree(posX, posY, posZ)) { // requirements not met
	std::cout << "failure. light " << light << std::endl;
		return ;
	}
	if (!((value >> 9) & 0x1)) { // grow into stage 1, no visual diff
		_blocks[offset] = value + (1 << 9);
		_added[offset] = value + (1 << 9);
	std::cout << "grow stage 1!" << std::endl;
		return ;
	}
	std::cout << "oak tree!" << std::endl;
	// grow oak tree starting from offset
	_blocks[offset] = blocks::AIR;
	add_block( false, {posX, posY, posZ}, blocks::OAK_LOG, blocks::AIR );
	_vertex_update = true;
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Chunk::turnDirtToGrass( int posX, int posY, int posZ )
{
	if (!_blocks || posZ < 0 || posZ >= WORLD_HEIGHT) {
		return ;
	}
	if (posX < 0) {
		if (_neighbours[face_dir::MINUSX]) {
			_neighbours[face_dir::MINUSX]->turnDirtToGrass(posX, posY, posZ);
		}
	} else if (posX >= CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSX]) {
			_neighbours[face_dir::PLUSX]->turnDirtToGrass(posX, posY, posZ);
		}
	} else if (posY < 0) {
		if (_neighbours[face_dir::MINUSY]) {
			_neighbours[face_dir::MINUSY]->turnDirtToGrass(posX, posY, posZ);
		}
	} else if (posY >= CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSY]) {
			_neighbours[face_dir::PLUSY]->turnDirtToGrass(posX, posY, posZ);
		}
	} else {
		_blocks[(((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + posZ] = blocks::GRASS_BLOCK;
		_added[(((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + posZ] = blocks::GRASS_BLOCK;
		_vertex_update = true;
	}
}

// randomly select 3 blocks for each subchunk of 4096 blocks and update them
void Chunk::tickUpdate( void )
{
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 3; j++) {
			int selected = Random::rangedNumber(_seed, i * (1 << 12), (i + 1) * (1 << 12));
			int value = _blocks[selected], type = value & 0xFF;
				// std::cout << "updating crop in chunk " << _startX << ", " << _startY << ": " << ((selected >> WORLD_SHIFT) >> CHUNK_SHIFT) << " " << ((selected >> WORLD_SHIFT) & (CHUNK_SIZE - 1)) << ", " << (selected & (WORLD_HEIGHT - 1)) << std::endl;
			if (value & blocks::NOTVISIBLE) { // not updated
			} else if (type >= blocks::WHEAT_CROP && type <= blocks::WHEAT_CROP6) {
				updateCrop(value, selected);
			} else if (type == blocks::FARMLAND) {
				updateFarmland(value, selected);
			} else if (type == blocks::GRASS_BLOCK) {
				spreadGrassblock(selected);
			} else if (type == blocks::OAK_LEAVES && (value & blocks::NATURAL)) {
				decayLeaves(selected);
			} else if (type == blocks::OAK_SAPLING) {
				growTree(value, selected);
			}
		}
	}
}

void Chunk::updateScheduledBlocks( void )
{
	size_t size = _scheduled_to_fall.size();
	if (!size) {
		return ;
	}

	for (size_t index = 0; index < size; ++index) {
		int offset = _scheduled_to_fall[0];
		int type = _blocks[offset] & 0xFF;
		if (type != blocks::SAND && type != blocks::GRAVEL) {
			// std::cerr << _startX << ", " << _startY << " scheduled_to_fall block is " << s_blocks[type]->name << std::endl;
			_scheduled_to_fall.erase(_scheduled_to_fall.begin());
			continue ;
		}
		int posZ = offset & (WORLD_HEIGHT - 1);
		int posY = ((offset >> WORLD_SHIFT) & (CHUNK_SIZE - 1));
		int posX = ((offset >> WORLD_SHIFT) >> CHUNK_SHIFT);
		handleHit(false, type, {posX + _startX, posY + _startY, posZ}, Modif::REMOVE);
		_entities.push_back(new FallingBlockEntity(this, {posX + _startX, posY + _startY, posZ}, {type, 1, {0, 0}}));
		_scheduled_to_fall.erase(_scheduled_to_fall.begin());
	}
}
