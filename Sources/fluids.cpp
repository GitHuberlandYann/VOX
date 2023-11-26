#include "Camera.hpp"
#include "utils.h"

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

int Chunk::exposed_water_faces( int row, int col, int level )
{
	int res = 0;
	switch (level) {
		case 0:
			res += 1;
			res += (_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1] & 0xFF) < blocks::WATER;
			break ;
		case 255:
			res += !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], false, false, true);
			res += 1;
			break ;
		default:
			res += !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], false, false, true);
			res += (_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1] & 0xFF) < blocks::WATER;
	}
	res += !air_flower(_blocks[((row - 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], false, false, true);
	res += !air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], false, false, true);
	res += !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col - 1) * WORLD_HEIGHT + level], false, false, true);
	res += !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level], false, false, true);
	return (res);
}

static int max_water_level( int v0, int v1, int v2, int v3, int u0, int u1, int u2 )
{
	int res = blocks::NOTVISIBLE;
	if (u0 >= blocks::WATER || u1 >= blocks::WATER || u2 >= blocks::WATER) {
		return (0);
	}
	if (v0 >= blocks::WATER && v0 < res) {
		res = v0;
	}
	if (v1 >= blocks::WATER && v1 < res) {
		res = v1;
	}
	if (v2 >= blocks::WATER && v2 < res) {
		res = v2;
	}
	if (v3 >= blocks::WATER && v3 < res) {
		res = v3;
	}
	if (res == blocks::NOTVISIBLE) {
		res = 0;
	} else {
		++res;
	}
	return (res - blocks::WATER);
}

std::array<int, 4> Chunk::water_heights( int value, int above, int row, int col, int level )
{
	std::array<int, 4> res;
	if ((above & 0xFF) >= blocks::WATER) {
		res = {0, 0, 0, 0};
		return (res);
	}
	std::array<int, 9> quads = {getBlockAt(row - 1, col - 1, level, true) & 0xFF,
								getBlockAt(row - 1, col, level, true) & 0xFF,
								getBlockAt(row - 1, col + 1, level, true) & 0xFF,
								getBlockAt(row, col - 1, level, true) & 0xFF,
								value & 0xFF,
								getBlockAt(row, col + 1, level, true) & 0xFF,
								getBlockAt(row + 1, col - 1, level, true) & 0xFF,
								getBlockAt(row + 1, col, level, true) & 0xFF,
								getBlockAt(row + 1, col + 1, level, true)};
	std::array<int, 8> quadsup = {getBlockAt(row - 1, col - 1, level + 1, true) & 0xFF,
								getBlockAt(row - 1, col, level + 1, true) & 0xFF,
								getBlockAt(row - 1, col + 1, level + 1, true) & 0xFF,
								getBlockAt(row, col - 1, level + 1, true) & 0xFF,
								getBlockAt(row, col + 1, level + 1, true) & 0xFF,
								getBlockAt(row + 1, col - 1, level + 1, true) & 0xFF,
								getBlockAt(row + 1, col, level + 1, true) & 0xFF,
								getBlockAt(row + 1, col + 1, level + 1, true) & 0xFF};

	res[0] = max_water_level(quads[0], quads[1], quads[3], quads[4], quadsup[0], quadsup[1], quadsup[3]);
	res[1] = max_water_level(quads[1], quads[2], quads[4], quads[5], quadsup[1], quadsup[2], quadsup[4]);
	res[2] = max_water_level(quads[3], quads[4], quads[6], quads[7], quadsup[3], quadsup[5], quadsup[6]);
	res[3] = max_water_level(quads[4], quads[5], quads[7], quads[8], quadsup[4], quadsup[6], quadsup[7]);
	return (res);
}

bool Chunk::endFlow( std::set<int> *newFluids, int &value, int posX, int posY, int posZ )
{
	if (value < blocks::WATER) {
		// std::cout << "water turned into " << s_blocks[value].name << std::endl;
		for (int index = 0; index < 6; index++) {
			const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
			int adj = getBlockAt(posX + delta[0], posY + delta[1], posZ + delta[2], true) & 0xFF;
			if (adj >= blocks::WATER) {
				// std::cout << "updating neighbour" << std::endl;
				insertFluidAt(newFluids, posX + delta[0], posY + delta[1], posZ + delta[2]);
			}
		}
		return (true);
	}
	if (value > blocks::WATER) {
		// std::cout << "update water" << value << std::endl;
		int offset = (((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + posZ;
		bool stop = true, onGround = false;
		int sourceCount = 0;
		for (int index = 0; index < 6; index++) {
			const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
			int adj = getBlockAt(posX + delta[0], posY + delta[1], posZ + delta[2], true) & 0xFF;
			if (index != face_dir::MINUSZ) { // if not block underneath
			// std::cout << posX + delta[0] << ", " << posY + delta[1] << ", " << posZ + delta[2] << " is " << s_blocks[adj].name << std::endl;
				if (adj >= blocks::WATER) {
					if (index == face_dir::PLUSZ) { // flow from above
						if (value > blocks::WATER1) {
							value = blocks::WATER1;
							_blocks[offset] = value;
							_added[offset] = value;
							_removed.erase(offset);
						}
						stop = false;
					} else if (adj < value) {
						sourceCount += (adj == blocks::WATER);
						if (adj < value - 1) {
							value = adj + 1; // update
							_blocks[offset] = value;
							_added[offset] = value;
							_removed.erase(offset);
						}
						stop = false; // still supplyed
						// std::cout << "supplyed by water" << adj << std::endl;
					}
				}
			} else if (air_flower(adj, false, false, false)) { // check block underneath
				onGround = true;
			}
		}
		if (stop) {
			// std::cout << "stop flow of " << s_blocks[value].name << std::endl;
			_blocks[offset] = blocks::AIR;
			_added.erase(offset);
			_removed.insert(offset);
			// _water_count -= exposed_water_faces(posX, posY, posZ);
			for (int index = 0; index < 6; index++) {
				const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
				int adj = getBlockAt(posX + delta[0], posY + delta[1], posZ + delta[2], true) & 0xFF;
				if (adj > blocks::WATER) {
					insertFluidAt(newFluids, posX + delta[0], posY + delta[1], posZ + delta[2]);
				}
			}
			return (true);
		} else if (onGround && sourceCount > 1) {
			// std::cout << "infinite water source" << std::endl;
			value = blocks::WATER; // infinite water baby
			_blocks[offset] = value;
			_added[offset] = value;
			_removed.erase(offset);
		}
	}
	return (false);
}

bool Chunk::addFlow( std::set<int> *newFluids, int posX, int posY, int posZ, int level )
{
	int offset = (((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + posZ;
	int type = _blocks[offset] & 0xFF;
	// std::cout << "checking blockFlow " << posX << ", " << posY << ", " << posZ << ": " << s_blocks[type].name << std::endl;
	if (!air_flower(type, false, false, true) || type > level || (type == level && level == blocks::WATER1)) {
		// std::cout << "column expension, water count before: " << _water_count << std::endl;
		_blocks[offset] = level;
		_added[offset] = level;
		_removed.erase(offset);
		if (!air_flower(type, false, false, true) && type != blocks::AIR) { // replace flower with water
			// std::cout << _startX << ", " << _startY << " type before: " << s_blocks[type].name << ". displayed: " << _displayed_faces << std::endl;
			entity_block(posX, posY, posZ, type); // drop item(s)
			_displayed_faces -= (2 << (type == blocks::TORCH));
			if (type == blocks::TORCH) {
				_lights[offset] &= 0xFF00;
				light_spread(posX, posY, posZ, false);
			}
			// std::cout << "type after: " << s_blocks[level].name << ". displayed: " << _displayed_faces << std::endl;
			delete [] static_cast<GLint*>(_vertices);
			_vertices = new GLint[_displayed_faces * 24];
			_vertex_update = true;
		}
		if (!air_flower(type, false, false, true)) {
			_hasWater = true;
		} else {
			// std::cout << '[' << _startX << ", " << _startY << "] replaced " << s_blocks[type].name << " with " << s_blocks[level].name << std::endl; 
		}
			// std::cout << '[' << _startX << ", " << _startY << "] replaced " << s_blocks[type].name << " with " << s_blocks[level].name << " at " << posX << ", " << posY << ", " << posZ << std::endl; 
		if (newFluids) {
			newFluids->insert(offset);
		} else {
			_fluids.insert(offset);
		}
		// std::cout << "column expension, water count after: " << _water_count << std::endl;
		return (true);
	} else if (type >= blocks::WATER) {
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
	// std::cout << "in sort water" << std::endl;
	if (!_water_count && !_hasWater) {
		return ;
	}
	// Bench b;
	#if 1
	// pos = glm::vec3(pos.x - _startX, pos.y - _startY, pos.z);
	std::vector<std::pair<float, std::array<int, 11>>> order;
	order.reserve(_water_count);
	for (int row = 0; row < CHUNK_SIZE; row++) {
		for (int col = 0; col < CHUNK_SIZE; col++) {
			for (int level = 1; level < 244; level++) { // TODO handle water when at level 255..
				int type = _blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] & 0xFF;
				if (type >= blocks::WATER) {
					int pX = _startX + row;
					int pY = _startY + col;
					int above = _blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level + 1];
					std::array<int, 4> heights = water_heights(type, above, row, col, level);
					if (above < blocks::WATER) {
						order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY + 0.5f, level + ((8.0f - heights[0]) / 8.0f))), {pX, pY + 1, level + 1, 1, -1, 0, heights[1], heights[3], heights[0], heights[2], 1}});
					}
					if (!air_flower(_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level - 1], false, false, true)) {
						order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY + 0.5f, level)), {pX, pY, level, 1, 1, 0, 0, 0, 0, 0, 1}});
					}
					if (!air_flower(getBlockAt(row - 1, col, level, true), false, false, true)) {
						order.push_back({dist2(pos, glm::vec3(pX, pY + 0.5f, level + 0.5f)), {pX, pY + 1, level + 1, 0, -1, -1, heights[1], heights[0], 0, 0, 1}});
					}
					if (!air_flower(getBlockAt(row + 1, col, level, true), false, false, true)) {
						order.push_back({dist2(pos, glm::vec3(pX + 1, pY + 0.5f, level + 0.5f)), {pX + 1, pY, level + 1, 0, 1, -1, heights[2], heights[3], 0, 0, 1}});
					}
					if (!air_flower(getBlockAt(row, col - 1, level, true), false, false, true)) {
						order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY, level + 0.5f)), {pX, pY, level + 1, 1, 0, -1, heights[0], heights[2], 0, 0, 1}});
					}
					if (!air_flower(getBlockAt(row, col + 1, level, true), false, false, true)) {
						order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY + 1, level + 0.5f)), {pX + 1, pY + 1, level + 1, -1, 0, -1, heights[3], heights[1], 0, 0, 1}});
					}
				} else if (type == blocks::GLASS) {
					int pX = _startX + row;
					int pY = _startY + col;
					std::array<int, 4> heights = {0, 0, 0, 0};
					if (!air_flower(_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level + 1], false, false, false)) {
						order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY + 0.5f, level + ((8.0f - heights[0]) / 8.0f))), {pX, pY + 1, level + 1, 1, -1, 0, heights[1], heights[3], heights[0], heights[2], 0}});
					}
					if (!air_flower(_blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level - 1], false, false, false)) {
						order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY + 0.5f, level)), {pX, pY, level, 1, 1, 0, 0, 0, 0, 0, 0}});
					}
					if (!air_flower(getBlockAt(row - 1, col, level, true), false, false, false)) {
						order.push_back({dist2(pos, glm::vec3(pX, pY + 0.5f, level + 0.5f)), {pX, pY + 1, level + 1, 0, -1, -1, heights[1], heights[0], 0, 0, 0}});
					}
					if (!air_flower(getBlockAt(row + 1, col, level, true), false, false, false)) {
						order.push_back({dist2(pos, glm::vec3(pX + 1, pY + 0.5f, level + 0.5f)), {pX + 1, pY, level + 1, 0, 1, -1, heights[2], heights[3], 0, 0, 0}});
					}
					if (!air_flower(getBlockAt(row, col - 1, level, true), false, false, false)) {
						order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY, level + 0.5f)), {pX, pY, level + 1, 1, 0, -1, heights[0], heights[2], 0, 0, 0}});
					}
					if (!air_flower(getBlockAt(row, col + 1, level, true), false, false, false)) {
						order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY + 1, level + 0.5f)), {pX + 1, pY + 1, level + 1, -1, 0, -1, heights[3], heights[1], 0, 0, 0}});
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
	if (order.size() != _water_count) {
		// std::cerr << '[' << _startX << ", " << _startY << "] \033[31msort_water\033[0m, order size " << order.size() << ", but _water_count " << _water_count << std::endl;
		// std::cerr << "block at 1 1 141: " << _blocks[((CHUNK_SIZE + 2) + 1) * WORLD_HEIGHT + 141] << std::endl;
		_water_count = order.size();
		_mtx_fluid.lock();
		delete [] _water_vert;
		_water_vert = new GLint[_water_count * 24];
		_mtx_fluid.unlock();
		// if (vip)b.stamp("alloc");
	}

	std::pair<float, std::array<int, 11>> tmp;
	for (size_t index = 0; index < order.size() - 1; index++) {
		float minDist = order[index].first;
		size_t minIndex = index;
		for (size_t jindex = index + 1; jindex < order.size(); jindex++) {
			if (order[jindex].first > minDist) {
				minIndex = jindex;
				minDist = order[minIndex].first;
			}
		}
		if (minIndex != index) {
			tmp = order[minIndex];
			order[minIndex] = order[index];
			order[index] = tmp;
		}
	}
	// if (vip)b.stamp("sort");

	size_t vindex = 0;
	for (auto& o: order) {
		glm::ivec4 start = {o.second[0], o.second[1], o.second[2], o.second[6]}, offset0, offset1, offset2, offset3;
		if (!o.second[5]) { // top/down faces
			std::array<int, 5> texcoord_offsets;
			if (o.second[10] == 1) {
				texcoord_offsets = compute_texcoord_offsets(o.second[6], o.second[7], o.second[8], o.second[9]);
			} else {
				texcoord_offsets = {0, 1 << 10, 1 << 11, 3 << 10, 3 << 8};
			}
			offset0 = {0, 0, 0, texcoord_offsets[0]};
			offset1 = {o.second[3], 0, 0, o.second[7] - start.w + texcoord_offsets[1]};// (1 << 10)};
			offset2 = {0, o.second[4], 0, o.second[8] - start.w + texcoord_offsets[2]};// (1 << 11)};
			offset3 = {o.second[3], o.second[4], 0, o.second[9] - start.w + texcoord_offsets[3]};//(1 << 10) + (1 << 11)};
			start.w += texcoord_offsets[4];//(1 << 8); // waterStill || waterFlow || glass
		} else {
			offset0 = {0, 0, 0, 0}; // TODO for now side faces' texture is "squished"
			offset1 = {o.second[3], o.second[4], 0, o.second[7] - start.w + (1 << 10)};
			offset2 = {0, 0, o.second[5], -start.w + (1 << 11)};
			offset3 = {o.second[3], o.second[4], o.second[5], -start.w + (1 << 10) + (1 << 11)};
			start.w += (1 << 9) + ((o.second[10] == 0) << 8); // waterFlow || glass
		}
		// std::cout << "vindex " << vindex << std::endl;
		_mtx_fluid.lock();
		face_water_vertices(_water_vert, start + offset0, start + offset1, start + offset2, start + offset3, vindex);
		_mtx_fluid.unlock();
	}
	order.clear();
	// if (vip)b.stamp("fill vertices");
	#else
	std::multimap<float, std::array<int, 10>> order;
	for (int row = 1; row < CHUNK_SIZE + 1; row++) {
		for (int col = 1; col < CHUNK_SIZE + 1; col++) {
			for (int level = 1; level < 244; level++) { // TODO handle water when at level 255..
				int value = _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level];
				if (value >= blocks::WATER) {
					int pX = _startX + row - 1;
					int pY = _startY + col - 1;
					int above = _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1];
					std::array<int, 4> heights = water_heights(value, above, row, col, level);
					if (_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1] < blocks::WATER) {
						order.insert({dist2(pos, glm::vec3(pX + 0.5f, pY + 0.5f, level + ((8.0f - heights[0]) / 8.0f))), {pX, pY + 1, level + 1, 1, -1, 0, heights[1], heights[3], heights[0], heights[2]}});
					}
					if (!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], false, false, true)) {
						order.insert({dist2(pos, glm::vec3(pX + 0.5f, pY + 0.5f, level)), {pX, pY, level, 1, 1, 0, 0, 0, 0, 0}});
					}
					if (!air_flower(_blocks[((row - 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], false, false, true)) {
						order.insert({dist2(pos, glm::vec3(pX, pY + 0.5f, level + 0.5f)), {pX, pY + 1, level + 1, 0, -1, -1, heights[1], heights[0], 0, 0}});
					}
					if (!air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], false, false, true)) {
						order.insert({dist2(pos, glm::vec3(pX + 1, pY + 0.5f, level + 0.5f)), {pX + 1, pY, level + 1, 0, 1, -1, heights[2], heights[3], 0, 0}});
					}
					if (!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col - 1) * WORLD_HEIGHT + level], false, false, true)) {
						order.insert({dist2(pos, glm::vec3(pX + 0.5f, pY, level + 0.5f)), {pX, pY, level + 1, 1, 0, -1, heights[0], heights[2], 0, 0}});
					}
					if (!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level], false, false, true)) {
						order.insert({dist2(pos, glm::vec3(pX + 0.5f, pY + 1, level + 0.5f)), {pX + 1, pY + 1, level + 1, -1, 0, -1, heights[3], heights[1], 0, 0}});
					}
				}
			}
		}
	}
	// if(vip)b.stop("order");
	if (!order.size()) {
		_water_count = 0;
		_hasWater = false;
		return ;
	}
	if (order.size() != _water_count) {
		// std::cerr << '[' << _startX << ", " << _startY << "] \033[31msort_water\033[0m, order size " << order.size() << ", but _water_count " << _water_count << std::endl;
		// std::cerr << "block at 1 1 141: " << _blocks[((CHUNK_SIZE + 2) + 1) * WORLD_HEIGHT + 141] << std::endl;
		_water_count = order.size();
		_mtx_fluid.lock();
		delete [] _water_vert;
		_water_vert = new GLint[_water_count * 24];
		_mtx_fluid.unlock();
		// if (vip)b.stamp("alloc");
	}

	size_t vindex = 0;
	for (std::multimap<float, std::array<int, 10>>::reverse_iterator it = order.rbegin(); it != order.rend(); it++) {
		glm::ivec4 start = {it->second[0], it->second[1], it->second[2], it->second[6]}, offset0, offset1, offset2, offset3;
		if (!it->second[5]) { // top/down faces
			std::array<int, 5> texcoord_offsets = compute_texcoord_offsets(it->second[6], it->second[7], it->second[8], it->second[9]);
			offset0 = {0, 0, 0, texcoord_offsets[0]};
			offset1 = {it->second[3], 0, 0, it->second[7] - start.w + texcoord_offsets[1]};// (1 << 10)};
			offset2 = {0, it->second[4], 0, it->second[8] - start.w + texcoord_offsets[2]};// (1 << 11)};
			offset3 = {it->second[3], it->second[4], 0, it->second[9] - start.w + texcoord_offsets[3]};//(1 << 10) + (1 << 11)};
			start.w += texcoord_offsets[4];//(1 << 8); // waterStill || waterFlow
		} else {
			offset0 = {0, 0, 0, 0}; // TODO for now side faces' texture is "squished"
			offset1 = {it->second[3], it->second[4], 0, it->second[7] - start.w + (1 << 10)};
			offset2 = {0, 0, it->second[5], -start.w + (1 << 11)};
			offset3 = {it->second[3], it->second[4], it->second[5], -start.w + (1 << 10) + (1 << 11)};
			start.w += (1 << 8); // waterStill
		}
		// std::cout << "vindex " << vindex << std::endl;
		_mtx_fluid.lock();
		face_water_vertices(_water_vert, start + offset0, start + offset1, start + offset2, start + offset3, vindex);
		_mtx_fluid.unlock();
	}

	#endif
	_waterVaoReset = true;
	_mtx.lock();
	if (vip) {
		_waterVaoVIP = true;
		// b.stamp("rest");
		// b.stop("fluid sorting");
	}
	_mtx.unlock();
}

void Chunk::insertFluidAt( std::set<int> *newFluids, int posX, int posY, int posZ )
{
	if (posZ < 0 || posZ > 255) {
		return ;
	}
	if (posX < 0) {
		if (_neighbours[face_dir::MINUSX]) {
			return (_neighbours[face_dir::MINUSX]->insertFluidAt(NULL, posX + CHUNK_SIZE, posY, posZ));
		}
	} else if (posX >= CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSX]) {
			return (_neighbours[face_dir::PLUSX]->insertFluidAt(NULL, posX - CHUNK_SIZE, posY, posZ));
		}
	} else if (posY < 0) {
		if (_neighbours[face_dir::MINUSY]) {
			return (_neighbours[face_dir::MINUSY]->insertFluidAt(NULL, posX, posY + CHUNK_SIZE, posZ));
		}
	} else if (posY >= CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSY]) {
			return (_neighbours[face_dir::PLUSY]->insertFluidAt(NULL, posX, posY - CHUNK_SIZE, posZ));
		}
	} else {
		if (newFluids) {
			newFluids->insert((((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + posZ);
		} else {
			_fluids.insert((((posX << CHUNK_SHIFT) + posY) << WORLD_SHIFT) + posZ);
		}
	}
}

bool Chunk::try_addFlow( std::set<int> *newFluids, int posX, int posY, int posZ, int level )
{
	if (posZ < 0 || posZ > 255) {
		return (false);
	}
	if (posX < 0) {
		if (_neighbours[face_dir::MINUSX]) {
			return (_neighbours[face_dir::MINUSX]->try_addFlow(NULL, posX + CHUNK_SIZE, posY, posZ, level));
		}
	} else if (posX >= CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSX]) {
			return (_neighbours[face_dir::PLUSX]->try_addFlow(NULL, posX - CHUNK_SIZE, posY, posZ, level));
		}
	} else if (posY < 0) {
		if (_neighbours[face_dir::MINUSY]) {
			return (_neighbours[face_dir::MINUSY]->try_addFlow(NULL, posX, posY + CHUNK_SIZE, posZ, level));
		}
	} else if (posY >= CHUNK_SIZE) {
		if (_neighbours[face_dir::PLUSY]) {
			return (_neighbours[face_dir::PLUSY]->try_addFlow(NULL, posX, posY - CHUNK_SIZE, posZ, level));
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
	for (auto f = _fluids.begin(); f != _fluids.end();) {
		fluid_modif = true;
		int posZ = *f & (WORLD_HEIGHT - 1);
		int posY = ((*f >> WORLD_SHIFT) & (CHUNK_SIZE - 1));
		int posX = ((*f >> WORLD_SHIFT) >> CHUNK_SHIFT);
		int level = _blocks[*f];
		if (endFlow(&newFluids, level, posX, posY, posZ)) {
			// std::cout << "ENDFLOW" << std::endl;
		} else if (try_addFlow(&newFluids, posX, posY, posZ - 1, blocks::WATER1) && level != blocks::WATER) { // source water spread to the side even if air below
		} else if (level == blocks::WATER7) { // stop flow
		} else {
			// std::cout << "block under fluid: " << s_blocks[_blocks[*f - 1]].name << std::endl;
			try_addFlow(&newFluids, posX - 1, posY, posZ, level + 1);
			try_addFlow(&newFluids, posX + 1, posY, posZ, level + 1);
			try_addFlow(&newFluids, posX, posY - 1, posZ, level + 1);
			try_addFlow(&newFluids, posX, posY + 1, posZ, level + 1);
		}
		// std::cout << _startX << " " << _startY << " fluid at " << posX << " (" << _startX + posX << "), " << posY << " (" << _startY + posY << "), " << posZ << ": before " << s_blocks[level].name << " after " << s_blocks[_blocks[*f]].name << std::endl;
		f = _fluids.erase(f);
	}
	_fluids = newFluids;
	newFluids.clear();
	if (fluid_modif) {
		// std::cout << "s" << std::endl;
		sort_water(_camera->getPos(), true);
	}
}
