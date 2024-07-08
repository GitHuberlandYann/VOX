#include "Chunk.hpp"
#include "random.hpp"
#include "utils.h"
#include "logs.hpp"

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

const int neighbour8[8][2] = {
	{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}
};

// make use of 1 / floor(25 / points) + 1 to update crop
void Chunk::updateCrop( int value, int offset )
{
	int posZ = offset & (settings::consts::world_height - 1);
	int posY = ((offset >> settings::consts::world_shift) & (settings::consts::chunk_size - 1));
	int posX = ((offset >> settings::consts::world_shift) >> settings::consts::chunk_shift);
	TICKUPLOG(LOG(_startX << ", " << _startY << ": update crop at " << posX << ", " << posY << ", " << posZ));
	int light = getLightLevel(posX, posY, posZ);
	light = glm::max(light & 0xF, (light >> 8) & 0xF);
	if (light < 9) { // crop requires light level of at least 9 to grow // TODO handle night light dimming
		TICKUPLOG(LOG("Failure. sky light " << ((light >> 8) & 0xF) << ", block light " << (light & 0xF)));
		if (light <= 7) { // uproot // TODO crops can't be planted if light level <= 7
			remove_block(true, {posX, posY, posZ});
			_vertex_update = true;
		}
		return ;
	}
	int growth_stage = (value >> offset::blocks::bitfield) & 0x7;
	if (growth_stage == 7) {
		return ;
	}
	float points = (getBlockAt(posX, posY, posZ - 1) & mask::farmland::wet) ? 4 : 2;
	for (int index = 0; index < 8; ++index) {
		const int delta[2] = {neighbour8[index][0], neighbour8[index][1]};
		int adj = getBlockAt(posX + delta[0], posY + delta[1], posZ - 1);
		if (TYPE(adj) == blocks::farmland) {
			points += (adj & mask::farmland::wet) ? 0.75f : 0.25f;
		}
	} // TODO might want to handle diag crop
	float growth_probability = 1.0f / (glm::floor(25 / points) + 1);
	TICKUPLOG(LOG("points " << points << ", proba: " << growth_probability << "%"));
	if (Random::randomFloat(_seed) <= growth_probability) {
		_blocks[offset] = value + (1 << offset::blocks::bitfield);
		_added[offset] = value + (1 << offset::blocks::bitfield);
		_vertex_update = true;
		TICKUPLOG(LOG("Success!"));
	} else {
		TICKUPLOG(LOG("Failure."));
	}
}

bool Chunk::watered_farmland( int posX, int posY, int posZ )
{
	for (int row = posX - 4; row <= posX + 4; row++) {
		for (int col = posY - 4; col <= posY + 4; col++) {
			for (int level = posZ; level <= posZ + 1; level++) {
				if (TYPE(getBlockAt(row, col, level)) == blocks::water) {
					return (true);
				}
			}
		}
	}
	return (false);
}

void Chunk::updateFarmland( int value, int offset )
{
	int posZ = offset & (settings::consts::world_height - 1);
	int posY = ((offset >> settings::consts::world_shift) & (settings::consts::chunk_size - 1));
	int posX = ((offset >> settings::consts::world_shift) >> settings::consts::chunk_shift);
	bool hydrated = watered_farmland(posX, posY, posZ);
	if (!hydrated) {
		int wetness = (value >> 10) & 0x7;
		int type_above = TYPE(getBlockAt(posX, posY, posZ + 1));
		if (wetness == 1) {
			_blocks[offset] = TYPE(value);
			_added[offset] = TYPE(value);
			_vertex_update = true;
			return ;
		} else if (wetness > 1) {
			_blocks[offset] = (value & (mask::all_bits - mask::farmland::moisture)) + ((wetness - 1) << offset::farmland::moisture);
			_added[offset] = (value & (mask::all_bits - mask::farmland::moisture)) + ((wetness - 1) << offset::farmland::moisture);
			_removed.erase(offset);
			return ;
		} else if (type_above != blocks::wheat_crop) { // TODO once other crops implemented, use ->geometry != GEO::CROP instead
			TICKUPLOG(LOG("updateFarmland replace_block " << _startX << ", " << _startY));
			setBlockAt(blocks::dirt, {posX, posY, posZ}, true);
			_vertex_update = true;
			return ;
		}
	} else if (!(value & mask::farmland::wet)) {
		_blocks[offset] = TYPE(value) | mask::farmland::wet | (0x7 << offset::farmland::moisture);
		_added[offset] = TYPE(value) | mask::farmland::wet | (0x7 << offset::farmland::moisture);
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
	int posZ = offset & (settings::consts::world_height - 1);
	int posY = ((offset >> settings::consts::world_shift) & (settings::consts::chunk_size - 1));
	int posX = ((offset >> settings::consts::world_shift) >> settings::consts::chunk_shift);
	if (posZ < 254) {
		int above = TYPE(_blocks[offset + 1]);
		if (!s_blocks[above]->transparent || above == blocks::water) {
			_blocks[offset] = blocks::dirt;
			_added[offset] = blocks::dirt;
			_vertex_update = true;
			return ;
		}
	}
	int light = getLightLevel(posX, posY, posZ + 1);
	light = glm::max(light & 0xF, (light >> 8) & 0xF);
	if (light < 9) { // grass requires light level of at least 9 to spread // TODO handle night light dimming
		if (light < 5) {
			_blocks[offset] = blocks::dirt;
			_added[offset] = blocks::dirt;
			_vertex_update = true;
		}
		return ;
	}
	for (int i = 0; i < 4; i++) {
		int selected = Random::rangedNumber(_seed, 0, 3 * 3 * 5 - 1);
		const int delta[3] = {neighbour45[selected][0], neighbour45[selected][1], neighbour45[selected][2]};
		int adj = getBlockAt(posX + delta[0], posY + delta[1], posZ + delta[2]);
		if (TYPE(adj) == blocks::dirt && !(adj & mask::blocks::notVisible)) {
			int above_adj = TYPE(getBlockAt(posX + delta[0], posY + delta[1], posZ + delta[2] + 1));
			if (s_blocks[above_adj]->transparent && above_adj != blocks::water) {
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
	int type = TYPE(getBlockAt(posX, posY, posZ));
	if (type == blocks::oak_log) {
		return (true);
	} else if (type != blocks::oak_leaves) {
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
	int posZ = offset & (settings::consts::world_height - 1);
	int posY = ((offset >> settings::consts::world_shift) & (settings::consts::chunk_size - 1));
	int posX = ((offset >> settings::consts::world_shift) >> settings::consts::chunk_shift);

	int logConnected = findLog(posX, posY, posZ, 6);
	if (!logConnected) {
		TICKUPLOG(LOG("decay leave " << _startX << ", " << _startY));
		remove_block(true, {posX, posY, posZ}); // TODO might want to update light
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
				int type = TYPE(getBlockAt(posX + row, posY + col, posZ + level));
				if (utils::block::air_flower(type, true, false, true) && type != blocks::grass_block && type != blocks::dirt) {
					return (false);
				}
			}
		}
	}
	return (true);
}

void Chunk::growTree( int value, int offset )
{
	int posZ = offset & (settings::consts::world_height - 1);
	int posY = ((offset >> settings::consts::world_shift) & (settings::consts::chunk_size - 1));
	int posX = ((offset >> settings::consts::world_shift) >> settings::consts::chunk_shift);

	int light = getLightLevel(posX, posY, posZ);
	light = glm::max(light & 0xF, (light >> 8) & 0xF);
	TICKUPLOG(LOG("growTree at " << posX << ", " << posY << ", " << posZ));
	if (light < 9 || !spaceForTree(posX, posY, posZ)) { // requirements not met
	TICKUPLOG(LOG("failure. light " << light));
		return ;
	}
	if (!(value & mask::sapling::growth)) { // grow into stage 1, no visual diff
		_blocks[offset] = value | mask::sapling::growth;
		_added[offset] = value | mask::sapling::growth;
	TICKUPLOG(LOG("grow stage 1!"));
		return ;
	}
	TICKUPLOG(LOG("oak tree!"));
	// grow oak tree starting from offset
	_blocks[offset] = blocks::air;
	add_block( false, {posX, posY, posZ}, blocks::oak_log, blocks::air );
	_vertex_update = true;
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Chunk::turnDirtToGrass( int posX, int posY, int posZ )
{
	if (posZ < 0 || posZ >= settings::consts::world_height) {
		return ;
	}
	if (posX < 0) {
		if (_neighbours[face_dir::minus_x]) {
			_neighbours[face_dir::minus_x]->turnDirtToGrass(posX, posY, posZ);
		}
	} else if (posX >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_x]) {
			_neighbours[face_dir::plus_x]->turnDirtToGrass(posX, posY, posZ);
		}
	} else if (posY < 0) {
		if (_neighbours[face_dir::minus_y]) {
			_neighbours[face_dir::minus_y]->turnDirtToGrass(posX, posY, posZ);
		}
	} else if (posY >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_y]) {
			_neighbours[face_dir::plus_y]->turnDirtToGrass(posX, posY, posZ);
		}
	} else {
		_blocks[(((posX << settings::consts::chunk_shift) + posY) << settings::consts::world_shift) + posZ] = blocks::grass_block;
		_added[(((posX << settings::consts::chunk_shift) + posY) << settings::consts::world_shift) + posZ] = blocks::grass_block;
		_vertex_update = true;
	}
}

// randomly select 3 blocks for each subchunk of 4096 blocks and update them
void Chunk::tickUpdate( void )
{
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 3; j++) {
			// int selected = Random::rangedNumber(_seed, i * (1 << 12), (i + 1) * (1 << 12));
			int selected = Random::rangedNumber(_seed, (i << 12), (i + 1) << 12);
			int value = _blocks[selected], type = TYPE(value);
				TICKUPLOG(LOG("updating crop in chunk " << POSXY(_startX, _startY) << ": " << ((selected >> settings::consts::world_shift) >> settings::consts::chunk_shift) << " " << ((selected >> settings::consts::world_shift) & (settings::consts::chunk_size - 1)) << ", " << (selected & (settings::consts::world_height - 1))));
			if (value & mask::blocks::notVisible || !type) { // not updated
				continue ;
			}
			switch (type) {
				case blocks::wheat_crop:
					updateCrop(value, selected);
					break ;
				case blocks::farmland:
					updateFarmland(value, selected);
					break ;
				case blocks::grass_block:
					spreadGrassblock(selected);
					break ;
				case blocks::oak_leaves:
					if (value & mask::leaves::natural) {
						decayLeaves(selected);
					}
					break ;
				case blocks::oak_sapling:
					growTree(value, selected);
					break ;
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
		int type = TYPE(_blocks[offset]);
		if (type != blocks::sand && type != blocks::gravel) {
			LOGERROR(_startX << ", " << _startY << " scheduled_to_fall block is " << s_blocks[type]->name);
			_scheduled_to_fall.erase(_scheduled_to_fall.begin());
			continue ;
		}
		int posZ = offset & (settings::consts::world_height - 1);
		int posY = ((offset >> settings::consts::world_shift) & (settings::consts::chunk_size - 1));
		int posX = ((offset >> settings::consts::world_shift) >> settings::consts::chunk_shift);
		handleHit(false, type, {posX + _startX, posY + _startY, posZ}, Modif::rm);
		_entities.push_back(std::make_shared<FallingBlockEntity>(this, glm::vec3(posX + _startX, posY + _startY, posZ), t_item(type, 1)));
		_scheduled_to_fall.erase(_scheduled_to_fall.begin());
	}
}
