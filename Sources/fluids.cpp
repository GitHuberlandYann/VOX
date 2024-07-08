#include "Player.hpp"
#include "utils.h"
#include "logs.hpp"

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

int Chunk::exposed_water_faces( int row, int col, int level )
{
	int res = 0;
	switch (level) {
		case 0:
			res += 1;
			res += TYPE(_blocks[(row * (settings::consts::chunk_size + 2) + col) * settings::consts::world_height + level + 1]) != blocks::water;
			break ;
		case 255:
			res += !utils::block::air_flower(_blocks[(row * (settings::consts::chunk_size + 2) + col) * settings::consts::world_height + level - 1], false, false, true);
			res += 1;
			break ;
		default:
			res += !utils::block::air_flower(_blocks[(row * (settings::consts::chunk_size + 2) + col) * settings::consts::world_height + level - 1], false, false, true);
			res += TYPE(_blocks[(row * (settings::consts::chunk_size + 2) + col) * settings::consts::world_height + level + 1]) != blocks::water;
	}
	res += !utils::block::air_flower(_blocks[((row - 1) * (settings::consts::chunk_size + 2) + col) * settings::consts::world_height + level], false, false, true);
	res += !utils::block::air_flower(_blocks[((row + 1) * (settings::consts::chunk_size + 2) + col) * settings::consts::world_height + level], false, false, true);
	res += !utils::block::air_flower(_blocks[(row * (settings::consts::chunk_size + 2) + col - 1) * settings::consts::world_height + level], false, false, true);
	res += !utils::block::air_flower(_blocks[(row * (settings::consts::chunk_size + 2) + col + 1) * settings::consts::world_height + level], false, false, true);
	return (res);
}

static int max_water_level( int v0, int v1, int v2, int v3, int u0, int u1, int u2 )
{
	int res = mask::blocks::notVisible;
	if (u0 == blocks::water || u1 == blocks::water || u2 == blocks::water) {
		return (0);
	}
	if (TYPE(v0) == blocks::water && ((v0 >> offset::blocks::bitfield) & 0x7) < res) {
		res = (v0 >> offset::blocks::bitfield) & 0x7;
	}
	if (TYPE(v1) == blocks::water && ((v1 >> offset::blocks::bitfield) & 0x7) < res) {
		res = (v1 >> offset::blocks::bitfield) & 0x7;
	}
	if (TYPE(v2) == blocks::water && ((v2 >> offset::blocks::bitfield) & 0x7) < res) {
		res = (v2 >> offset::blocks::bitfield) & 0x7;
	}
	if (TYPE(v3) == blocks::water && ((v3 >> offset::blocks::bitfield) & 0x7) < res) {
		res = (v3 >> offset::blocks::bitfield) & 0x7;
	}
	if (res == mask::blocks::notVisible) {
		res = 0;
	} else {
		++res;
	}
	return (res);
}

std::array<int, 4> Chunk::water_heights( int value, int above, int row, int col, int level )
{
	std::array<int, 4> res;
	if (above == blocks::water) {
		res = {0, 0, 0, 0};
		return (res);
	}
	std::array<int, 9> quads = {getBlockAt(row - 1, col - 1, level),
								getBlockAt(row - 1, col, level),
								getBlockAt(row - 1, col + 1, level),
								getBlockAt(row, col - 1, level),
								value,
								getBlockAt(row, col + 1, level),
								getBlockAt(row + 1, col - 1, level),
								getBlockAt(row + 1, col, level),
								getBlockAt(row + 1, col + 1, level)};
	std::array<int, 8> quadsup = {TYPE(getBlockAt(row - 1, col - 1, level + 1)),
								TYPE(getBlockAt(row - 1, col, level + 1)),
								TYPE(getBlockAt(row - 1, col + 1, level + 1)),
								TYPE(getBlockAt(row, col - 1, level + 1)),
								TYPE(getBlockAt(row, col + 1, level + 1)),
								TYPE(getBlockAt(row + 1, col - 1, level + 1)),
								TYPE(getBlockAt(row + 1, col, level + 1)),
								TYPE(getBlockAt(row + 1, col + 1, level + 1))};

	res[0] = max_water_level(quads[0], quads[1], quads[3], quads[4], quadsup[0], quadsup[1], quadsup[3]);
	res[1] = max_water_level(quads[1], quads[2], quads[4], quads[5], quadsup[1], quadsup[2], quadsup[4]);
	res[2] = max_water_level(quads[3], quads[4], quads[6], quads[7], quadsup[3], quadsup[5], quadsup[6]);
	res[3] = max_water_level(quads[4], quads[5], quads[7], quads[8], quadsup[4], quadsup[6], quadsup[7]);
	return (res);
}

bool Chunk::endFlow( std::set<int> *newFluids, int &value, int posX, int posY, int posZ )
{
	int type = TYPE(value), wLevel = (value >> offset::blocks::bitfield) & 0x7;
	if (type != blocks::water) {
		FLUIDLOG(LOG("water turned into " << s_blocks[type]->name));
		for (int index = 0; index < 6; index++) {
			const glm::ivec3 delta = adj_blocks[index];
			int adj = TYPE(getBlockAt(posX + delta.x, posY + delta.y, posZ + delta.z));
			if (adj == blocks::water) {
				FLUIDLOG(LOG("updating neighbour"));
				insertFluidAt(newFluids, posX + delta.x, posY + delta.y, posZ + delta.z);
			}
		}
		return (true);
	}
	if (wLevel > 0) {  // not a water source
		FLUIDLOG(LOG("update water" << wLevel));
		int offset = (((posX << settings::consts::chunk_shift) + posY) << settings::consts::world_shift) + posZ;
		bool stop = true, onGround = false;
		int sourceCount = 0;
		for (int index = 0; index < 6; index++) {
			const glm::ivec3 delta = adj_blocks[index];
			int adj = getBlockAt(posX + delta.x, posY + delta.y, posZ + delta.z);
			if (index != face_dir::minus_z) { // if not block underneath
			FLUIDLOG(LOG(posX + delta.x << ", " << posY + delta.y << ", " << posZ + delta.z << " is " << s_blocks[adj]->name));
				if (TYPE(adj) == blocks::water) {
					int adjWLevel = ((adj >> offset::blocks::bitfield) & 0x7);
					if (index == face_dir::plus_z) { // flow from above
						if (wLevel > 1) {
							value = blocks::water + (1 << offset::blocks::bitfield);
							_blocks[offset] = value;
							_added[offset] = value;
							_removed.erase(offset);
						}
						stop = false;
					} else if (adjWLevel < wLevel) {
						sourceCount += (adjWLevel == 0);
						if (adjWLevel < wLevel - 1) {
							value = adj + (1 << offset::blocks::bitfield); // update
							_blocks[offset] = value;
							_added[offset] = value;
							_removed.erase(offset);
						}
						stop = false; // still supplyed
						FLUIDLOG(LOG("supplyed by water" << adj));
					}
				}
			} else if (utils::block::air_flower(adj, false, false, false)) { // check block underneath
				onGround = true;
			}
		}
		if (stop) {
			FLUIDLOG(LOG("stop flow of " << s_blocks[value]->name));
			_blocks[offset] = blocks::air;
			_added.erase(offset);
			_removed.insert(offset);
			// _water_count -= exposed_water_faces(posX, posY, posZ);
			for (int index = 0; index < 6; index++) {
				const glm::ivec3 delta = adj_blocks[index];
				int adj = TYPE(getBlockAt(posX + delta.x, posY + delta.y, posZ + delta.z));
				if (adj == blocks::water) {
					insertFluidAt(newFluids, posX + delta.x, posY + delta.y, posZ + delta.z);
				}
			}
			return (true);
		} else if (onGround && sourceCount > 1) {
			FLUIDLOG(LOG("infinite water source"));
			value = blocks::water; // infinite water baby
			_blocks[offset] = value;
			_added[offset] = value;
			_removed.erase(offset);
		}
	}
	return (false);
}

bool Chunk::addFlow( std::set<int> *newFluids, int posX, int posY, int posZ, int srcWLevel )
{
	int offset = (((posX << settings::consts::chunk_shift) + posY) << settings::consts::world_shift) + posZ;
	int value = _blocks[offset], type = TYPE(value), wLevel = (value >> offset::blocks::bitfield) & 0x7;
	FLUIDLOG(LOG("checking blockFlow " << posX << ", " << posY << ", " << posZ << ": " << s_blocks[type]->name));
	// if (!utils::block::air_flower(type, false, false, true) || type > level || (type == level && level == blocks::water1)) {
	if (!s_blocks[type]->isSolidForFluid || (type == blocks::water && (wLevel > srcWLevel || (wLevel == srcWLevel && wLevel == 1)))) {
		FLUIDLOG(LOG("column expension, water count before: " << _water_count));
		_blocks[offset] = blocks::water + (srcWLevel << offset::blocks::bitfield);
		_added[offset] = blocks::water + (srcWLevel << offset::blocks::bitfield);
		_removed.erase(offset);
		if (!s_blocks[type]->isSolidForFluid) {
			_hasWater = true;
			if (type != blocks::air) { // replace flower with water
				FLUIDLOG(LOG(_startX << ", " << _startY << " type before: " << s_blocks[type]->name << ". displayed: " << _displayed_faces));
				entity_block(posX, posY, posZ, type); // drop item(s)
				if (type == blocks::torch || type == blocks::redstone_torch) {
					_lights[offset] &= 0xFF00;
					light_spread(posX, posY, posZ, false);
					if (type == blocks::torch) {
						auto search = _flames.find(offset);
						if (search != _flames.end()) {
							_flames.erase(offset);
						}
					}
				}
				FLUIDLOG(LOG("type after: " << s_blocks[level]->name << ". displayed: " << _displayed_faces));
				_vertex_update = true;
			}
		} else {
			FLUIDLOG(LOG('[' << POSXY(_startX, _startY) << "] replaced " << s_blocks[type]->name << " with " << s_blocks[level]->name)); 
		}
			FLUIDLOG(LOG('[' << POSXY(_startX, _startY) << "] replaced " << s_blocks[type]->name << " with " << s_blocks[level]->name << " at " << posX << ", " << posY << ", " << posZ)); 
		if (newFluids) {
			newFluids->insert(offset);
		} else {
			_fluids.insert(offset);
		}
		FLUIDLOG(LOG("column expension, water count after: " << _water_count));
		return (true);
	} else if (type == blocks::water) {
		return (true);
	}
	return (false);
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Chunk::sort_water( glm::vec3 pos, bool vip )
{
	_sortedOnce = true;
	FLUIDLOG(LOG("in sort water"));
	if (!_water_count && !_hasWater) {
		return ;
	}
	// Bench b;
	// pos = glm::vec3(pos.x - _startX, pos.y - _startY, pos.z);
	std::vector<std::pair<float, std::array<int, 11>>> order;
	order.reserve(_water_count);
	for (int row = 0; row < settings::consts::chunk_size; row++) {
		for (int col = 0; col < settings::consts::chunk_size; col++) {
			for (int level = 1; level < 244; level++) { // TODO handle water when at level 255..
				int value = _blocks[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level], type = TYPE(value);
				if (type == blocks::water) {
					int pX = _startX + row;
					int pY = _startY + col;
					int above = TYPE(_blocks[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level + 1]);
					std::array<int, 4> heights = water_heights(value, above, row, col, level);
					if (above != blocks::water) {
						order.push_back({utils::math::dist2(pos, glm::vec3(pX + 0.5f, pY + 0.5f, level + ((8.0f - heights[0]) / 8.0f))), {pX, pY + 1, level + 1, 1, -1, 0, heights[1], heights[3], heights[0], heights[2], 1}});
					}
					if (!utils::block::air_flower(_blocks[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level - 1], false, false, true)) {
						order.push_back({utils::math::dist2(pos, glm::vec3(pX + 0.5f, pY + 0.5f, level)), {pX, pY, level, 1, 1, 0, 0, 0, 0, 0, 1}});
					}
					if (!utils::block::air_flower(getBlockAt(row - 1, col, level), false, false, true)) {
						order.push_back({utils::math::dist2(pos, glm::vec3(pX, pY + 0.5f, level + 0.5f)), {pX, pY + 1, level + 1, 0, -1, -1, heights[1], heights[0], 0, 0, 1}});
					}
					if (!utils::block::air_flower(getBlockAt(row + 1, col, level), false, false, true)) {
						order.push_back({utils::math::dist2(pos, glm::vec3(pX + 1, pY + 0.5f, level + 0.5f)), {pX + 1, pY, level + 1, 0, 1, -1, heights[2], heights[3], 0, 0, 1}});
					}
					if (!utils::block::air_flower(getBlockAt(row, col - 1, level), false, false, true)) {
						order.push_back({utils::math::dist2(pos, glm::vec3(pX + 0.5f, pY, level + 0.5f)), {pX, pY, level + 1, 1, 0, -1, heights[0], heights[2], 0, 0, 1}});
					}
					if (!utils::block::air_flower(getBlockAt(row, col + 1, level), false, false, true)) {
						order.push_back({utils::math::dist2(pos, glm::vec3(pX + 0.5f, pY + 1, level + 0.5f)), {pX + 1, pY + 1, level + 1, -1, 0, -1, heights[3], heights[1], 0, 0, 1}});
					}
				} else if (type == blocks::glass) {
					int pX = _startX + row;
					int pY = _startY + col;
					std::array<int, 4> heights = {0, 0, 0, 0};
					if (!utils::block::air_flower(_blocks[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level + 1], false, false, false)) {
						order.push_back({utils::math::dist2(pos, glm::vec3(pX + 0.5f, pY + 0.5f, level + ((8.0f - heights[0]) / 8.0f))), {pX, pY + 1, level + 1, 1, -1, 0, heights[1], heights[3], heights[0], heights[2], 0}});
					}
					if (!utils::block::air_flower(_blocks[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level - 1], false, false, false)) {
						order.push_back({utils::math::dist2(pos, glm::vec3(pX + 0.5f, pY + 0.5f, level)), {pX, pY, level, 1, 1, 0, 0, 0, 0, 0, 0}});
					}
					if (!utils::block::air_flower(getBlockAt(row - 1, col, level), false, false, false)) {
						order.push_back({utils::math::dist2(pos, glm::vec3(pX, pY + 0.5f, level + 0.5f)), {pX, pY + 1, level + 1, 0, -1, -1, heights[1], heights[0], 0, 0, 0}});
					}
					if (!utils::block::air_flower(getBlockAt(row + 1, col, level), false, false, false)) {
						order.push_back({utils::math::dist2(pos, glm::vec3(pX + 1, pY + 0.5f, level + 0.5f)), {pX + 1, pY, level + 1, 0, 1, -1, heights[2], heights[3], 0, 0, 0}});
					}
					if (!utils::block::air_flower(getBlockAt(row, col - 1, level), false, false, false)) {
						order.push_back({utils::math::dist2(pos, glm::vec3(pX + 0.5f, pY, level + 0.5f)), {pX, pY, level + 1, 1, 0, -1, heights[0], heights[2], 0, 0, 0}});
					}
					if (!utils::block::air_flower(getBlockAt(row, col + 1, level), false, false, false)) {
						order.push_back({utils::math::dist2(pos, glm::vec3(pX + 0.5f, pY + 1, level + 0.5f)), {pX + 1, pY + 1, level + 1, -1, 0, -1, heights[3], heights[1], 0, 0, 0}});
					}
				}
			}
		}
	}
	// if (vip)b.stamp("order");
	if (!order.size()) {
		_water_count = 0;
		_hasWater = false;
		return ;
	}
	_water_count = order.size();

	std::sort(order.begin(), order.end(), []( std::pair<float, std::array<int, 11>> a, std::pair<float, std::array<int, 11>> b )
                                  {
                                      return (a.first > b.first);
                                  });
	// if (vip)b.stamp("sort");

	_water_vert.clear();
	for (auto& o: order) {
		glm::ivec4 start = {o.second[0], o.second[1], o.second[2], 0}, offset0, offset1, offset2, offset3;
		if (!o.second[5]) { // top/down faces
			std::array<int, 5> texcoord_offsets;
			if (o.second[10] == 1) {
				texcoord_offsets = utils::fluid::compute_texcoord_offsets(o.second[6], o.second[7], o.second[8], o.second[9]);
			} else {
				texcoord_offsets = {0, 1 << 10, 1 << 11, 3 << 10, 3 << 8};
			}
			start.w += texcoord_offsets[4];//(1 << 8); // waterStill || waterFlow || glass
			offset0 = start + glm::ivec4(0, 0, 0, texcoord_offsets[0] + o.second[6]);
			offset1 = start + glm::ivec4(o.second[3], 0, 0, o.second[7] + texcoord_offsets[1]);// (1 << 10)};
			offset2 = start + glm::ivec4(0, o.second[4], 0, o.second[8] + texcoord_offsets[2]);// (1 << 11)};
			offset3 = start + glm::ivec4(o.second[3], o.second[4], 0, o.second[9] + texcoord_offsets[3]);//(1 << 10) + (1 << 11)};
		} else {
			start.w += (1 << 9) + ((o.second[10] == 0) << 8); // waterFlow || glass
			offset0 = start + glm::ivec4(0, 0, 0, o.second[6]); // TODO for now side faces' texture is "squished"
			offset1 = start + glm::ivec4(o.second[3], o.second[4], 0, o.second[7] + (1 << 10));
			offset2 = start + glm::ivec4(0, 0, o.second[5], (1 << 11));
			offset3 = start + glm::ivec4(o.second[3], o.second[4], o.second[5], (1 << 10) + (1 << 11));
		}
		_mtx_fluid.lock();
		utils::fluid::face_water_vertices(_water_vert, offset0, offset1, offset2, offset3);
		_mtx_fluid.unlock();
	}
	order.clear();
	// if (vip)b.stamp("fill vertices");

	_waterVaoReset = true;
	if (vip) {
		_mtx.lock();
		_waterVaoVIP = true;
		// b.stamp("rest");
		// b.stop("fluid sorting");
		_mtx.unlock();
	}
}

void Chunk::insertFluidAt( std::set<int> *newFluids, int posX, int posY, int posZ )
{
	if (posZ < 0 || posZ > 255) {
		return ;
	}
	if (posX < 0) {
		if (_neighbours[face_dir::minus_x]) {
			return (_neighbours[face_dir::minus_x]->insertFluidAt(NULL, posX + settings::consts::chunk_size, posY, posZ));
		}
	} else if (posX >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_x]) {
			return (_neighbours[face_dir::plus_x]->insertFluidAt(NULL, posX - settings::consts::chunk_size, posY, posZ));
		}
	} else if (posY < 0) {
		if (_neighbours[face_dir::minus_y]) {
			return (_neighbours[face_dir::minus_y]->insertFluidAt(NULL, posX, posY + settings::consts::chunk_size, posZ));
		}
	} else if (posY >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_y]) {
			return (_neighbours[face_dir::plus_y]->insertFluidAt(NULL, posX, posY - settings::consts::chunk_size, posZ));
		}
	} else {
		(newFluids) ? newFluids->insert((((posX << settings::consts::chunk_shift) + posY) << settings::consts::world_shift) + posZ)
					: _fluids.insert((((posX << settings::consts::chunk_shift) + posY) << settings::consts::world_shift) + posZ);
	}
}

bool Chunk::try_addFlow( std::set<int> *newFluids, int posX, int posY, int posZ, int level )
{
	if (posZ < 0 || posZ > 255) {
		return (false);
	}
	if (posX < 0) {
		if (_neighbours[face_dir::minus_x]) {
			return (_neighbours[face_dir::minus_x]->try_addFlow(NULL, posX + settings::consts::chunk_size, posY, posZ, level));
		}
	} else if (posX >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_x]) {
			return (_neighbours[face_dir::plus_x]->try_addFlow(NULL, posX - settings::consts::chunk_size, posY, posZ, level));
		}
	} else if (posY < 0) {
		if (_neighbours[face_dir::minus_y]) {
			return (_neighbours[face_dir::minus_y]->try_addFlow(NULL, posX, posY + settings::consts::chunk_size, posZ, level));
		}
	} else if (posY >= settings::consts::chunk_size) {
		if (_neighbours[face_dir::plus_y]) {
			return (_neighbours[face_dir::plus_y]->try_addFlow(NULL, posX, posY - settings::consts::chunk_size, posZ, level));
		}
	} else {
		return (addFlow(newFluids, posX, posY, posZ, level));
	}
	return (false);
}

void Chunk::updateFluids( void )
{
	std::set<int> newFluids;
	bool fluid_modif = false;
	for (auto f : _fluids) {
		fluid_modif = true;
		int posZ = f & (settings::consts::world_height - 1);
		int posY = ((f >> settings::consts::world_shift) & (settings::consts::chunk_size - 1));
		int posX = ((f >> settings::consts::world_shift) >> settings::consts::chunk_shift);
		int value = _blocks[f];
		if (endFlow(&newFluids, value, posX, posY, posZ)) {
			FLUIDLOG(LOG("ENDFLOW"));
			continue ;
		}
		int wLevel = (value >> offset::blocks::bitfield) & 0x7;
		if (try_addFlow(&newFluids, posX, posY, posZ - 1, 1) && wLevel != 0) { // source water spread to the side even if air below
		} else if (wLevel == 7) { // stop flow
		} else {
			FLUIDLOG(LOG("block under fluid: " << s_blocks[_blocks[*f - 1]].name));
			try_addFlow(&newFluids, posX - 1, posY, posZ, wLevel + 1);
			try_addFlow(&newFluids, posX + 1, posY, posZ, wLevel + 1);
			try_addFlow(&newFluids, posX, posY - 1, posZ, wLevel + 1);
			try_addFlow(&newFluids, posX, posY + 1, posZ, wLevel + 1);
		}
		FLUIDLOG(LOG(_startX << " " << _startY << " fluid at " << posX << " (" << _startX + posX << "), " << posY << " (" << _startY + posY << "), " << posZ << ": before " << s_blocks[level]->name << " after " << s_blocks[_blocks[*f]].name));
	}
	if (fluid_modif) {
		_fluids = newFluids;
		FLUIDLOG(LOG("sort water after fluid modif"));
		sort_water(_player->getEyePos(), true);
	}
}
