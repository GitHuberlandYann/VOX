#include "Chunk.hpp"
#include "Settings.hpp"
#include "utils.h"

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// if skySpread, _lights[offset] is bitshifted by 4 to work on sky light and not block light
// posX, posY are in [0; settings::consts::chunk_size[
void Chunk::light_spread( int posX, int posY, int posZ, bool skySpread, int recurse )
{
	if (skySpread && !recurse) { // TODO check if stackoverflow possible when torches put in a loooong row, spoiler alert: it probably is
		return ;
	}
	// if (skySpread && _startX == 256 && _startY == 320) {
	// 	static int cnt;
	// 	std::cout << POSXY(_startX, _startY) << " at " << posX << ", " << posY << ", " << posZ << " light_spread " << ((skySpread) ? "SKY" : "BLOCK") << " recurse level " << ++cnt << std::endl;
	// }
	int shift = 8 * skySpread;
	int offset = (((posX << settings::consts::chunk_shift) + posY) << settings::consts::world_shift) + posZ;
	short saveLight = _lights[offset];
	short level = ((saveLight >> shift) & 0xFF);
	// std::cout << "light_spread, level is " << (int)level << std::endl;
	if ((level & 0xF0) < 0xF0) { // not a source block or source block with light level < 15, we check if level should change
		short maxLevel = 0;
		for (int index = 0; index < 6; index++) {
			const glm::ivec3 delta = adj_blocks[index];
			if (posZ + delta.z < 0 || posZ + delta.z > 255) {
			} else {
				short adj = getLightLevel(posX + delta.x, posY + delta.y, posZ + delta.z) >> shift;
				maxLevel = maxs(maxLevel, adj & 0xF);
				if (skySpread && index == face_dir::plus_z && (adj & 0xF0)) {
					// TODO if _blocks[offset].. is not water/leaves
					// 	maxLevel = adj&0xFF - (0x10 if adj & 0xF0 else 0);
					// else
					maxLevel = (adj & 0xFF) + 1; // if sky light above is source, own sky light becomes source too
				}
			}
		}
		if ((maxLevel && maxLevel - 1 != level) || (!maxLevel && level)) {
			if (!skySpread && level >= maxLevel - 1) maxLevel = 1;
			level = (level & 0xF0) + maxs((level & 0xF0) >> 4, maxLevel - 1);
			_lights[offset] = (saveLight & (0xFF << (8 - shift))) + (level << shift);
			_light_update = true;
		} else {
			return ; // stop spread when not source and level is unchanged
		}
	} else if (skySpread && posZ + 1 < settings::consts::world_height) { // skySpread + source block
		short above = _lights[offset + 1] >> 8;
		if (!(above & 0xF0)) { // if block above is not a source anymore
			_lights[offset] &= 0xFF;
			return (light_spread(posX, posY, posZ, skySpread, settings::consts::light_recurse));
		}
	}
	// std::cout << "level is " << (int)level << std::endl;

	// spread in all 6 directions
	for (int index = 0; index < 6; index++) {
		const glm::ivec3 delta = adj_blocks[index];
		if (skySpread && index == face_dir::minus_z && level == 0xFF) {
			light_try_spread(posX + delta.x, posY + delta.y, posZ + delta.z, 0xF0, skySpread, recurse - 1);
		} else {
			light_try_spread(posX + delta.x, posY + delta.y, posZ + delta.z, level, skySpread, recurse - 1);
		}
	}
}

void Chunk::generate_lights( void )
{
	// fill sky_light with 0 for underground and 15 (- leaves and water damper) for rest
	// voxel is considered underground if there is at least 1 solid block above it
	// this also fills block_light with 0
	for (int row = 0; row < settings::consts::chunk_size; row++) {
		for (int col = 0; col < settings::consts::chunk_size; col++) {
			char light_level = 15;
			for (int level = settings::consts::world_height - 1; level >= 0; level--) {
				if (light_level) {
					int type = _blocks[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level] & mask::blocks::type;
					if (!s_blocks[type]->transparent) { // block hit
						light_level = 0;
					} else if ((type == blocks::oak_leaves || type == blocks::water) && light_level) {
						--light_level;
					}
					// if (light_level == 15) {
						_lights[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level] = (light_level + (light_level << 4)) << 8; // we consider blocks directly under sky as light source
					// } else {
					// 	_lights[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level] = light_level << 8;
					// }
				} else {
					_lights[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level] = 0;
				}
			}
		}
	}
}

// now returns 0xF0 skylight + 0x0F blocklight
// row and col are in [-1:settings::consts::chunk_size]
int Chunk::computeLight( int row, int col, int level )
{
	// (void)row;(void)col;(void)level;return (0);
	short light = getLightLevel(row, col, level);

	return ((light & 0xF) | ((light >> 4) & 0xF0));
	int blockLight = light & 0xF;
	int skyLight = (light >> 8) & 0xF;
	return (blockLight + (skyLight << 4));
}

// same as computeLight, but we handle a corner this time
// result is max of 4 light levels surrounding corner
// used to have a smooth lighting from one block to another
int Chunk::computeSmoothLight( int faceLight, int row, int col, int level, std::array<int, 9> offsets )
{
	if (!Settings::Get()->getBool(settings::bools::smooth_lighting)) {
		return (faceLight);
	}
	short light = getLightLevel(row + offsets[0], col + offsets[1], level + offsets[2]);
	faceLight = maxs(faceLight & 0xF, light & 0xF) + (maxs((faceLight >> 4) & 0xF, (light >> 8) & 0xF) << 4);
	light = getLightLevel(row + offsets[3], col + offsets[4], level + offsets[5]);
	faceLight = maxs(faceLight & 0xF, light & 0xF) + (maxs((faceLight >> 4) & 0xF, (light >> 8) & 0xF) << 4);
	light = getLightLevel(row + offsets[6], col + offsets[7], level + offsets[8]);
	faceLight = maxs(faceLight & 0xF, light & 0xF) + (maxs((faceLight >> 4) & 0xF, (light >> 8) & 0xF) << 4);
	return (faceLight);
}

// https://minecraft.fandom.com/wiki/Light#AO_texture_pattern fct returns (side1 + side2 + corner)
int Chunk::computeShade( int row, int col, int level, std::array<int, 9> offsets )
{
	// (void)row;(void)col;(void)level;(void)offsets;return (0);
	return (!s_blocks[getBlockAt(row + offsets[0], col + offsets[1], level + offsets[2]) & mask::blocks::type]->transparent
			+ !s_blocks[getBlockAt(row + offsets[3], col + offsets[4], level + offsets[5]) & mask::blocks::type]->transparent
			+ !s_blocks[getBlockAt(row + offsets[6], col + offsets[7], level + offsets[8]) & mask::blocks::type]->transparent);
}

void Chunk::fill_vertex_array( void )
{
	// std::cout << "filling " << POSXY(_startX, _startY) << "; expecting " << _displayed_faces << std::endl;
	_mtx.lock();
	_vertices.clear();
	for (int row = 0; row < settings::consts::chunk_size; row++) {
		for (int col = 0; col < settings::consts::chunk_size; col++) {
			for (int level = 0; level < settings::consts::world_height; level++) {
				GLint block_value = _blocks[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level];
				if (block_value & mask::blocks::notVisible) {
					continue ;
				}
				s_blocks[block_value & mask::blocks::type]->addMesh(this, _vertices, {_startX, _startY}, {row, col, level}, block_value);
			}
		}
	}
	_mtx.unlock();
	_light_update = false;
	_vertex_update = false;
	_vaoReset = false;
	_vaoVIP = true;
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

GLint Chunk::getCamLightLevel( glm::ivec3 location )
{
	return (getLightLevel(location.x - _startX, location.y - _startY, location.z));
}

int Chunk::computePosLight( glm::vec3 pos )
{
	return (computeLight(glm::floor(pos.x - _startX), glm::floor(pos.y - _startY), glm::floor(pos.z)));
}

short Chunk::getLightLevel( int posX, int posY, int posZ )
{
	if (posZ < 0) {
		return (0);
	} else if (posZ >= settings::consts::world_height) {
		return (0xFF00);
	}
	if (posX < 0) {
		if (_neighbours[face_dir::minus_x]) {
			return (_neighbours[face_dir::minus_x]->getLightLevel(posX + settings::consts::chunk_size, posY, posZ));
		}
	} else if (posX >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_x]) {
			return (_neighbours[face_dir::plus_x]->getLightLevel(posX - settings::consts::chunk_size, posY, posZ));
		}
	} else if (posY < 0) {
		if (_neighbours[face_dir::minus_y]) {
			return (_neighbours[face_dir::minus_y]->getLightLevel(posX, posY + settings::consts::chunk_size, posZ));
		}
	} else if (posY >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_y]) {
			return (_neighbours[face_dir::plus_y]->getLightLevel(posX, posY - settings::consts::chunk_size, posZ));
		}
	} else {
		return (_lights[(((posX << settings::consts::chunk_shift) + posY) << settings::consts::world_shift) + posZ]);
	}
	return (0xF00);
}

void Chunk::setLightLevel( short level, int posX, int posY, int posZ, bool askNeighbours )
{
	if (posZ < 0 || posZ >= settings::consts::world_height) {
		return ;
	}
	if (posX < 0) {
		if (askNeighbours && _neighbours[face_dir::minus_x]) {
			_neighbours[face_dir::minus_x]->setLightLevel(level, posX + settings::consts::chunk_size, posY, posZ, true);
			return ;
		}
	} else if (posX >= settings::consts::chunk_size) {
		if (askNeighbours && _neighbours[face_dir::plus_x]) {
			_neighbours[face_dir::plus_x]->setLightLevel(level, posX - settings::consts::chunk_size, posY, posZ, true);
			return ;
		}
	} else if (posY < 0) {
		if (askNeighbours && _neighbours[face_dir::minus_y]) {
			_neighbours[face_dir::minus_y]->setLightLevel(level, posX, posY + settings::consts::chunk_size, posZ, true);
			return ;
		}
	} else if (posY >= settings::consts::chunk_size) {
		if (askNeighbours && _neighbours[face_dir::plus_y]) {
			_neighbours[face_dir::plus_y]->setLightLevel(level, posX, posY - settings::consts::chunk_size, posZ, true);
			return ;
		}
	} else {
		_lights[(((posX << settings::consts::chunk_shift) + posY) << settings::consts::world_shift) + posZ] = level;
	}
}

void Chunk::startLightSpread( int posX, int posY, int posZ, bool skySpread )
{
	if (posZ < 0 || posZ >= settings::consts::world_height) {
		return ;
	}
	if (posX < 0) {
		if (_neighbours[face_dir::minus_x]) {
			_neighbours[face_dir::minus_x]->startLightSpread(posX + settings::consts::chunk_size, posY, posZ, skySpread);
			return ;
		}
	} else if (posX >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_x]) {
			_neighbours[face_dir::plus_x]->startLightSpread(posX - settings::consts::chunk_size, posY, posZ, skySpread);
			return ;
		}
	} else if (posY < 0) {
		if (_neighbours[face_dir::minus_y]) {
			_neighbours[face_dir::minus_y]->startLightSpread(posX, posY + settings::consts::chunk_size, posZ, skySpread);
			return ;
		}
	} else if (posY >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_y]) {
			_neighbours[face_dir::plus_y]->startLightSpread(posX, posY - settings::consts::chunk_size, posZ, skySpread);
			return ;
		}
	} else {
		light_spread(posX, posY, posZ, skySpread);
	}
}

void Chunk::light_try_spread( int posX, int posY, int posZ, short level, bool skySpread, int recurse )
{
	if (posZ < 0 || posZ > 255) {
		return ;
	}
	if (posX < 0 || posX >= settings::consts::chunk_size || posY < 0 || posY >= settings::consts::chunk_size) {
		// spread neighbour
		if (posX == -1) {
			if (_neighbours[face_dir::minus_x]) {
				_neighbours[face_dir::minus_x]->light_try_spread(settings::consts::chunk_size - 1, posY, posZ, level, skySpread, recurse);
			}
		} else if (posY == -1) {
			if (_neighbours[face_dir::minus_y]) {
				_neighbours[face_dir::minus_y]->light_try_spread(posX, settings::consts::chunk_size - 1, posZ, level, skySpread, recurse);
			}
		} else if (posX == settings::consts::chunk_size) {
			if (_neighbours[face_dir::plus_x]) {
				_neighbours[face_dir::plus_x]->light_try_spread(0, posY, posZ, level, skySpread, recurse);
			}
		} else if (posY == settings::consts::chunk_size) {
			if (_neighbours[face_dir::plus_y]) {
				_neighbours[face_dir::plus_y]->light_try_spread(posX, 0, posZ, level, skySpread, recurse);
			}
		}
	} else {
		int type = _blocks[(((posX << settings::consts::chunk_shift) + posY) << settings::consts::world_shift) + posZ] & mask::blocks::type;
		// if (air_flower(type, false, true, true) && type != blocks::OAK_SLAB_BOTTOM && type != blocks::farmland && type != blocks::dirt_PATH) {
		// 	return ;
		// }
		if (!s_blocks[type]->transparent) {
			return ;
		}
		short neighbour = (_lights[(((posX << settings::consts::chunk_shift) + posY) << settings::consts::world_shift) + posZ] >> (8 * skySpread));
		if ((!(neighbour & 0xF0) || (skySpread && !(level & 0xF0))) && ((neighbour & 0xF) != maxs(0, ((level & 0xF) - 1) & 0xF))) { // only spread to non source blocks whose value is not expected value. Also spread to source block if from non source block and skySpread
			light_spread(posX, posY, posZ, skySpread, recurse);
		}
	}
}
