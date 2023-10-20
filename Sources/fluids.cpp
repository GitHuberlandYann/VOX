#include "vox.h"

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

int Chunk::exposed_water_faces( int row, int col, int level )
{
	int res = 0;
	switch (level) {
		case 0:
			res += 1;
			res += _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1] < blocks::WATER;
			break ;
		case 255:
			res += !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], false, true);
			res += 1;
			break ;
		default:
			res += !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], false, true);
			res += _blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1] < blocks::WATER;
	}
	res += !air_flower(_blocks[((row - 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], false, true);
	res += !air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], false, true);
	res += !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col - 1) * WORLD_HEIGHT + level], false, true);
	res += !air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level], false, true);
	return (res);
}

static int max_water_level( int v0, int v1, int v2, int v3, int u0, int u1, int u2 )
{
	int res = 0;
	if (u0 >= blocks::WATER || u1 >= blocks::WATER || u2 >= blocks::WATER) {
		return (res);
	}
	if (v0 >= blocks::WATER && v0 > res) {
		res = v0;
	}
	if (v1 >= blocks::WATER && v1 > res) {
		res = v1;
	}
	if (v2 >= blocks::WATER && v2 > res) {
		res = v2;
	}
	if (v3 >= blocks::WATER && v3 > res) {
		res = v3;
	}
	if (res == blocks::WATER) {
		++res;
	}
	return (res - blocks::WATER);
}

std::array<int, 4> Chunk::water_heights( int value, int above, int row, int col, int level )
{
	std::array<int, 4> res;
	if (above >= blocks::WATER) {
		res = {0, 0, 0, 0};
		return (res);
	}
	std::array<int, 9> quads = {_blocks[((row - 1) * (CHUNK_SIZE + 2) + col - 1) * WORLD_HEIGHT + level],
								_blocks[((row - 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level],
								_blocks[((row - 1) * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level],
								_blocks[(row * (CHUNK_SIZE + 2) + col - 1) * WORLD_HEIGHT + level],
								value,
								_blocks[(row * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level],
								_blocks[((row + 1) * (CHUNK_SIZE + 2) + col - 1) * WORLD_HEIGHT + level],
								_blocks[((row + 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level],
								_blocks[((row + 1) * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level]};
	std::array<int, 8> quadsup = {_blocks[((row - 1) * (CHUNK_SIZE + 2) + col - 1) * WORLD_HEIGHT + level + 1],
								_blocks[((row - 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1],
								_blocks[((row - 1) * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level + 1],
								_blocks[(row * (CHUNK_SIZE + 2) + col - 1) * WORLD_HEIGHT + level + 1],
								_blocks[(row * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level + 1],
								_blocks[((row + 1) * (CHUNK_SIZE + 2) + col - 1) * WORLD_HEIGHT + level + 1],
								_blocks[((row + 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level + 1],
								_blocks[((row + 1) * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level + 1]};

	res[0] = max_water_level(quads[0], quads[1], quads[3], quads[4], quadsup[0], quadsup[1], quadsup[3]);
	res[1] = max_water_level(quads[1], quads[2], quads[4], quads[5], quadsup[1], quadsup[2], quadsup[4]);
	res[2] = max_water_level(quads[3], quads[4], quads[6], quads[7], quadsup[3], quadsup[5], quadsup[6]);
	res[3] = max_water_level(quads[4], quads[5], quads[7], quads[8], quadsup[4], quadsup[6], quadsup[7]);
	return (res);
}

bool Chunk::endFlow( std::set<int> &newFluids, int &value, int posX, int posY, int posZ )
{
	if (posX < 1 || posX > CHUNK_SIZE || posY < 1 || posY > CHUNK_SIZE) {
		return (true);
	}
	if (value < blocks::WATER) {
		// std::cout << "water turned into " << s_blocks[value].name << std::endl;
		for (int index = 0; index < 6; index++) {
			const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
			if (posX + delta[0] < 1 || posX + delta[0] > CHUNK_SIZE || posY + delta[1] < 1 || posY + delta[1] > CHUNK_SIZE) {
			} else {
				int adj = _blocks[((posX + delta[0]) * (CHUNK_SIZE + 2) + posY + delta[1]) * WORLD_HEIGHT + posZ + delta[2]];
				if (adj >= blocks::WATER) {
					// std::cout << "updating neighbour" << std::endl;
					newFluids.insert((posX + delta[0]) + ((posY + delta[1]) << 8) + ((posZ + delta[2]) << 16));
				}
			}
		}
		return (true);
	}
	if (value > blocks::WATER) {
		// std::cout << "update water" << value << std::endl;
		bool stop = true, onGround = false;
		int sourceCount = 0;
		for (int index = 0; index < 6; index++) {
			const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
			int adj = _blocks[((posX + delta[0]) * (CHUNK_SIZE + 2) + posY + delta[1]) * WORLD_HEIGHT + posZ + delta[2]];
			if (index != face_dir::MINUSZ) { // if not block underneath
				if (adj >= blocks::WATER) {
					if (index == face_dir::PLUSZ) { // flow from above
						if (value > blocks::WATER1) {
							value = blocks::WATER1;
							_blocks[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ] = value;
							_added[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ] = value;
							_removed.erase((posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ);
						}
						stop = false;
					} else if (adj < value) {
						sourceCount += (adj == blocks::WATER);
						if (adj < value - 1) {
							value = adj + 1; // update
							_blocks[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ] = value;
							_added[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ] = value;
							_removed.erase((posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ);
						}
						stop = false; // still supplyed
						// std::cout << "supplyed by water" << adj << std::endl;
					}
				}
			} else if (air_flower(adj, false, true)) { // check block underneath
				onGround = true;
			}
		}
		if (stop) {
			// std::cout << "stop flow of " << s_blocks[value].name << std::endl;
			handle_border_block({posX - 1, posY - 1, posZ}, value, false);
			_blocks[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ] = blocks::AIR;
			_added.erase((posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ);
			_removed.insert((posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ);
			// _water_count -= exposed_water_faces(posX, posY, posZ);
			for (int index = 0; index < 6; index++) {
				const GLint delta[3] = {adj_blocks[index][0], adj_blocks[index][1], adj_blocks[index][2]};
				if (posX + delta[0] < 1 || posX + delta[0] > CHUNK_SIZE || posY + delta[1] < 1 || posY + delta[1] > CHUNK_SIZE) {
				} else {
					int adj = _blocks[((posX + delta[0]) * (CHUNK_SIZE + 2) + posY + delta[1]) * WORLD_HEIGHT + posZ + delta[2]];
					if (adj > blocks::WATER) {
						newFluids.insert((posX + delta[0]) + ((posY + delta[1]) << 8) + ((posZ + delta[2]) << 16));
					}
				}
			}
			return (true);
		} else if (onGround && sourceCount > 1) {
			// std::cout << "infinite water source" << std::endl;
			value = blocks::WATER; // infinite water baby
			_blocks[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ] = value;
			_added[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ] = value;
			_removed.erase((posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ);
		}
	}
	return (false);
}

bool Chunk::addFlow( std::set<int> &newFluids, int posX, int posY, int posZ, int level )
{
	int value = _blocks[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ];
	// std::cout << "checking blockFlow " << posX << ", " << posY << ", " << posZ << ": " << s_blocks[value].name << std::endl;
	if (!air_flower(value, false, true) || value > level || (value == level && level == blocks::WATER1)) {
		// std::cout << "column expension, water count before: " << _water_count << std::endl;
		_blocks[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ] = level;
		_added[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ] = level;
		_removed.erase((posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ);
		if (!air_flower(value, false, true) && value != blocks::AIR) { // replace flower with water
			// std::cout << "value before: " << s_blocks[value].name << std::endl;
			_displayed_faces -= 2;
			delete [] _vertices;
			_vertices = new GLint[_displayed_faces * 24];
			fill_vertex_array();
			_vaoReset = false;
			_mtx.lock();
			_vaoVIP = true;
			_mtx.unlock();
		}
		if (posX < 1 || posX > CHUNK_SIZE || posY < 1 || posY > CHUNK_SIZE) {
			// std::cout << "addFlow at border" << std::endl;
			handle_border_flow(posX, posY, posZ, level, true);
		} else {
			if (!air_flower(value, false, true)) {
				_hasWater = true;
			} else {
				// std::cout << '[' << _startX << ", " << _startY << "] replaced " << s_blocks[value].name << " with " << s_blocks[level].name << std::endl; 
			}
			newFluids.insert(posX + (posY << 8) + (posZ << 16));
			handle_border_block({posX - 1, posY - 1, posZ}, level, true);
		}
		// std::cout << "column expension, water count after: " << _water_count << std::endl;
		return (true);
	} else if (value >= blocks::WATER) {
		return (true);
	}
	return (false);
}

void Chunk::handle_border_flow( int posX, int posY, int posZ, int level, bool adding )
{
	if (!posX) {
		for (auto& c : *_vis_chunks) {
			if (c->isInChunk(_startX - CHUNK_SIZE, _startY)) {
				c->update_border_flow(CHUNK_SIZE, posY, posZ, level, adding);
				break ;
			}
		}
	} else if (posX == CHUNK_SIZE + 1) {
		for (auto& c : *_vis_chunks) {
			if (c->isInChunk(_startX + CHUNK_SIZE, _startY)) {
				c->update_border_flow(1, posY, posZ, level, adding);
				break ;
			}
		}
	}
	if (!posY) {
		for (auto& c : *_vis_chunks) {
			if (c->isInChunk(_startX, _startY - CHUNK_SIZE)) {
				c->update_border_flow(posX, CHUNK_SIZE, posZ, level, adding);
				break ;
			}
		}
	} else if (posY == CHUNK_SIZE + 1) {
		for (auto& c : *_vis_chunks) {
			if (c->isInChunk(_startX, _startY + CHUNK_SIZE)) {
				c->update_border_flow(posX, 1, posZ, level, adding);
				break ;
			}
		}
	}
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Chunk::sort_water( glm::vec3 pos, bool vip )
{
	// std::cout << "in sort water" << std::endl;
	if (!_water_count && !_hasWater) {
		return ;
	}
	// pos = glm::vec3(pos.x - _startX, pos.y - _startY, pos.z);
	std::vector<std::pair<float, std::array<int, 10>>> order;
	order.reserve(_water_count);
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
						order.push_back(std::pair<float, std::array<int, 10>>(glm::distance(pos, glm::vec3(pX + 0.5f, pY + 0.5f, level + ((8.0f - heights[0]) / 8.0f))), {pX, pY + 1, level + 1, 1, -1, 0, heights[1], heights[3], heights[0], heights[2]}));
					}
					if (!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level - 1], false, true)) {
						order.push_back(std::pair<float, std::array<int, 10>>(glm::distance(pos, glm::vec3(pX + 0.5f, pY + 0.5f, level)), {pX, pY, level, 1, 1, 0, 0, 0, 0, 0}));
					}
					if (!air_flower(_blocks[((row - 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], false, true)) {
						order.push_back(std::pair<float, std::array<int, 10>>(glm::distance(pos, glm::vec3(pX, pY + 0.5f, level + 0.5f)), {pX, pY + 1, level + 1, 0, -1, -1, heights[1], heights[0], 0, 0}));
					}
					if (!air_flower(_blocks[((row + 1) * (CHUNK_SIZE + 2) + col) * WORLD_HEIGHT + level], false, true)) {
						order.push_back(std::pair<float, std::array<int, 10>>(glm::distance(pos, glm::vec3(pX + 1, pY + 0.5f, level + 0.5f)), {pX + 1, pY, level + 1, 0, 1, -1, heights[2], heights[3], 0, 0}));
					}
					if (!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col - 1) * WORLD_HEIGHT + level], false, true)) {
						order.push_back(std::pair<float, std::array<int, 10>>(glm::distance(pos, glm::vec3(pX + 0.5f, pY, level + 0.5f)), {pX, pY, level + 1, 1, 0, -1, heights[0], heights[2], 0, 0}));
					}
					if (!air_flower(_blocks[(row * (CHUNK_SIZE + 2) + col + 1) * WORLD_HEIGHT + level], false, true)) {
						order.push_back(std::pair<float, std::array<int, 10>>(glm::distance(pos, glm::vec3(pX + 0.5f, pY + 1, level + 0.5f)), {pX + 1, pY + 1, level + 1, -1, 0, -1, heights[3], heights[1], 0, 0}));
					}
				}
			}
		}
	}
	
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
	}

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
			std::pair<float, std::array<int, 10>> tmp = order[minIndex];
			order[minIndex] = order[index];
			order[index] = tmp;
		}
	}

	size_t vindex = 0;
	for (auto& o: order) {
		glm::ivec4 start = {o.second[0], o.second[1], o.second[2], o.second[6]}, offset0, offset1, offset2, offset3;
		if (!o.second[5]) { // top/down faces
			std::array<int, 5> texcoord_offsets = compute_texcoord_offsets(o.second[6], o.second[7], o.second[8], o.second[9]);
			offset0 = {0, 0, 0, texcoord_offsets[0]};
			offset1 = {o.second[3], 0, 0, o.second[7] - start.w + texcoord_offsets[1]};// (1 << 10)};
			offset2 = {0, o.second[4], 0, o.second[8] - start.w + texcoord_offsets[2]};// (1 << 11)};
			offset3 = {o.second[3], o.second[4], 0, o.second[9] - start.w + texcoord_offsets[3]};//(1 << 10) + (1 << 11)};
			start.w += texcoord_offsets[4];//(1 << 8); // waterStill || waterFlow
		} else {
			offset0 = {0, 0, 0, 0}; // TODO for now side faces' texture is "squished"
			offset1 = {o.second[3], o.second[4], 0, o.second[7] - start.w + (1 << 10)};
			offset2 = {0, 0, o.second[5], -start.w + (1 << 11)};
			offset3 = {o.second[3], o.second[4], o.second[5], -start.w + (1 << 10) + (1 << 11)};
			start.w += (1 << 8); // waterStill
		}
		// std::cout << "vindex " << vindex << std::endl;
		_mtx_fluid.lock();
		face_vertices(_water_vert, start + offset0, start + offset1, start + offset2, start + offset3, vindex);
		_mtx_fluid.unlock();
	}
	order.clear();
	_waterVaoReset = true;
	_mtx.lock();
	if (vip) {
		_waterVaoVIP = true;
	}
	_mtx.unlock();
}

void Chunk::update_border_flow( int posX, int posY, int posZ, int wlevel, bool adding )
{
	if (!(posX == 1 || posY == 1 || posX == CHUNK_SIZE || posY == CHUNK_SIZE)) {
		std::cout << "ERROR update_border_flow not border block " << posX << ", " << posY << std::endl;
		return ;
	}
	if (_thread.joinable()) {
		_thread.join();
	}
	if (adding) {
		// std::cout << "before condition" << std::endl;
		int value = _blocks[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ];
		if (!air_flower(value, false, true) || (value >= blocks::WATER && value > wlevel)) {
			// std::cout << '[' << _startX << ", " << _startY << "] flow at border " << posX << ", " << posY << ", " << posZ << " >>" << value << ", " << previous_value << ", " << wlevel << std::endl;
			_blocks[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ] = wlevel;
			_added[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ] = wlevel;
			_removed.erase((posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ);
			if (!air_flower(value, false, true)) {
				if (value != blocks::AIR) { // replace flower with water
					_displayed_faces += 2;
					delete [] _vertices;
					_vertices = new GLint[_displayed_faces * 24];
					fill_vertex_array();
					_vaoReset = false;
					_mtx.lock();
					_vaoVIP = true;
					_mtx.unlock();
				}
				_hasWater = true;
			}
			_fluids.insert(posX + (posY << 8) + (posZ << 16));
		}
	} else {
		if (_blocks[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ] >= blocks::WATER) {
			_blocks[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ] = blocks::AIR;
			_added.erase((posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ);
			_removed.insert((posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ);
			_fluids.insert(posX + (posY << 8) + (posZ << 16));
		}
	}
}

void Chunk::updateFluids( void )
{
	std::set<int> newFluids;
	bool fluid_modif = false;
	for (auto f = _fluids.begin(); f != _fluids.end();) {
		fluid_modif = true;
		int posX = *f & 0xFF;
		int posY = (*f >> 8) & 0xFF;
		int posZ = (*f >> 16) & 0xFF;
		int level = _blocks[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ];
		if (endFlow(newFluids, level, posX, posY, posZ)) {
			// std::cout << "ENDFLOW" << std::endl;
		} else if (addFlow(newFluids, posX, posY, posZ - 1, blocks::WATER1) && level != blocks::WATER) { // source water spread to the side even if air below
		} else if (level == blocks::WATER7) { // stop flow
		} else {
			// std::cout << "block under fluid: " << s_blocks[_blocks[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ - 1]].name << std::endl;
			addFlow(newFluids, posX - 1, posY, posZ, level + 1);
			addFlow(newFluids, posX + 1, posY, posZ, level + 1);
			addFlow(newFluids, posX, posY - 1, posZ, level + 1);
			addFlow(newFluids, posX, posY + 1, posZ, level + 1);
		}
		// std::cout << "fluid at " << posX << ", " << posY << ", " << posZ << ": before " << s_blocks[level].name << " after " << s_blocks[_blocks[(posX * (CHUNK_SIZE + 2) + posY) * WORLD_HEIGHT + posZ]].name << std::endl;
		f = _fluids.erase(f);
	}
	_fluids = newFluids;
	newFluids.clear();
	if (fluid_modif) {
		// std::cout << "s" << std::endl;
		sort_water(_camera->getPos(), true);
	}
}
