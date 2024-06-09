#include "Camera.hpp"
#include "random.hpp"
#include "Menu.hpp"

extern std::mutex mtx_backup;

Chunk::Chunk( Camera *camera, Inventory *inventory, int posX, int posY, std::list<Chunk *> *chunks )
	: _vaoSet(false), _waterVaoSet(false), _waterVaoVIP(false),
	_skyVaoSet(false), _skyVaoVIP(false), _genDone(false), _light_update(false), _vertex_update(false),
	_vaoReset(false), _vaoVIP(false), _waterVaoReset(false), _skyVaoReset(false), _sortedOnce(false),
	_startX(posX), _startY(posY), _nb_neighbours(0),
	_hasWater(true), _displayed_faces(0), _water_count(0), _sky_count(0),
	_neighbours({NULL, NULL, NULL, NULL}), _camera(camera), _inventory(inventory)
{
	int cnt = 0;
// std::cout << "new chunk at " << posX << ", " << posY << std::endl;
	for (auto c : *chunks) { // TODO load backups if needed. for trees
		if (c->isInChunk(_startX - CHUNK_SIZE, _startY)) {
			_neighbours[face_dir::minus_x] = c;
			c->setNeighbour(this, face_dir::plus_x);
			++cnt;
		} else if (c->isInChunk(_startX + CHUNK_SIZE, _startY)) {
			_neighbours[face_dir::plus_x] = c;
			c->setNeighbour(this, face_dir::minus_x);
			++cnt;
		} else if (c->isInChunk(_startX, _startY - CHUNK_SIZE)) {
			_neighbours[face_dir::minus_y] = c;
			c->setNeighbour(this, face_dir::plus_y);
			++cnt;
		} else if (c->isInChunk(_startX, _startY + CHUNK_SIZE)) {
			_neighbours[face_dir::plus_y] = c;
			c->setNeighbour(this, face_dir::minus_y);
			++cnt;
		}

		if (cnt == 4) { // will never happen
			return ;
		}
	}
}

Chunk::~Chunk( void )
{
	if (_thread.joinable()) {
		_thread.join();
	}

	if (_neighbours[face_dir::minus_x]) {
		_neighbours[face_dir::minus_x]->setNeighbour(NULL, face_dir::plus_x);
		_neighbours[face_dir::minus_x] = NULL;
	}
	if (_neighbours[face_dir::plus_x]) {
		_neighbours[face_dir::plus_x]->setNeighbour(NULL, face_dir::minus_x);
		_neighbours[face_dir::plus_x] = NULL;
	}
	if (_neighbours[face_dir::minus_y]) {
		_neighbours[face_dir::minus_y]->setNeighbour(NULL, face_dir::plus_y);
		_neighbours[face_dir::minus_y] = NULL;
	}
	if (_neighbours[face_dir::plus_y]) {
		_neighbours[face_dir::plus_y]->setNeighbour(NULL, face_dir::minus_y);
		_neighbours[face_dir::plus_y] = NULL;
	}

	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_skyVbo);
	glDeleteVertexArrays(1, &_skyVao);

	// chests and furnaces are deleted in OpenGl_Manager when deleting backups
	for (auto e : _entities) {
		delete e;
	}
	for (auto p : _particles) {
		delete p;
	}
	for (auto &f : _flames) {
		delete f.second;
	}

	// std::cout << "chunk deleted " << _startX << ", " << _startY << std::endl;
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

/**
 * @brief fun overload of face_count to use glm::ivec3 instead of 3 int args
*/
GLint Chunk::face_count( int type, glm::ivec3 pos )
{
	return (face_count(type, pos.x, pos.y, pos.z));
}

GLint Chunk::face_count( int type, int row, int col, int level )
{
	type &= mask::blocks::type;
	if (!type || type >= blocks::water) {
		std::cerr << "face_count ERROR counting " << s_blocks[type]->name << std::endl;
		return (0);
	}
	if (type == blocks::glass || type == blocks::chest) {
		return (0);
	}
	if (type >= blocks::poppy) {
		return (2 << (type >= blocks::torch));
	}
	GLint res = visible_face(type, getBlockAt(row - 1, col, level), face_dir::minus_x)
				+ visible_face(type, getBlockAt(row + 1, col, level), face_dir::plus_x)
				+ visible_face(type, getBlockAt(row, col - 1, level), face_dir::minus_y)
				+ visible_face(type, getBlockAt(row, col + 1, level), face_dir::plus_y);
	switch (level) {
		case 0:
			res += visible_face(type, getBlockAt(row, col, level + 1), face_dir::plus_z);
			break ;
		case 255:
			res += visible_face(type, getBlockAt(row, col, level - 1), face_dir::minus_z);
			res += 1;
			break ;
		default:
			res += visible_face(type, getBlockAt(row, col, level - 1), face_dir::minus_z);
			res += visible_face(type, getBlockAt(row, col, level + 1), face_dir::plus_z);
	}
	if (type == blocks::oak_stairs_bottom || type == blocks::oak_stairs_top) {
		res += 2; // +3 if corner stair, but np so far
	}
	return (res);
}

void Chunk::resetDisplayedFaces( void )
{
	// hiding unseen blocks
	for (int row = 0; row < CHUNK_SIZE; row++) {
		for (int col = 0; col < CHUNK_SIZE; col++) {
			for (int level = 0; level < WORLD_HEIGHT; level++) {
				int offset = (((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level;
				GLint value = _blocks[offset], type = value & mask::blocks::type;
				bool restore = false;
				if ((value & mask::blocks::notVisible) && (!row || !col || row == CHUNK_SIZE - 1 || col == CHUNK_SIZE - 1)) {
					value -= mask::blocks::notVisible;
					restore = true;
				}
				if (type == blocks::chest) {}
				else if (type > blocks::air && type < blocks::water) {
					GLint below = ((level) ? _blocks[offset - 1] : 0) & mask::blocks::type;
					if (!air_flower(type, false, false, true) && type < blocks::torch && below != blocks::grass_block && below != blocks::dirt && below != blocks::sand) {
						_blocks[offset] = blocks::air;
					} else {
						GLint count = face_count(type, row, col, level);
						if (count) {
							// std::cout << "count is " << count << std::endl;
							if (restore) {
								_blocks[offset] = value;
							} else if (value & mask::blocks::notVisible) {
								_blocks[offset] = value - mask::blocks::notVisible;
								_added[offset] = value - mask::blocks::notVisible;
							}
						} else if (type > blocks::air) { // hide block
							_blocks[offset] = value | mask::blocks::notVisible;
						}
					}
				}
			}
		}
	}
}

// works as long as sizeof(int) == sizeof(float)
// we output 1 int and 3 floats to shader
void Chunk::setup_array_buffer( void )
{
	// if (_thread.joinable()) {
	// 	_thread.join();
	// }

	if (!_vaoSet) {
		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vbo);
		_vaoSet = true;
	}
	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	_displayed_faces = _vertices.size() / 6;
	_mtx.lock();
	glBufferData(GL_ARRAY_BUFFER, _displayed_faces * 4 * 6 * sizeof(GLint), &_vertices[0].spec, GL_STATIC_DRAW);
	_mtx.unlock();

	_vaoReset = true;
	_vaoVIP = false;

	glEnableVertexAttribArray(SPECATTRIB);
	glVertexAttribIPointer(SPECATTRIB, 1, GL_INT, 4 * sizeof(GLint), 0);

	glEnableVertexAttribArray(POSATTRIB);
	glVertexAttribPointer(POSATTRIB, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(1 * sizeof(GLint)));

	check_glstate("Chunk::setup_array_buffer", false);
}

void Chunk::setup_sky_array_buffer( void )
{
	// if (_thread.joinable()) {
	// 	_thread.join();
	// }

	// check_glstate("BEFORE Chunk::setup_sky_array_buffer", false);
	if (!_skyVaoSet) {
		glGenVertexArrays(1, &_skyVao);
		glGenBuffers(1, &_skyVbo);
		_skyVaoSet = true;
		// check_glstate("GEN Chunk::setup_sky_array_buffer", false);
	// glBindVertexArray(_skyVao);

	// glBindBuffer(GL_ARRAY_BUFFER, _skyVbo);
	// check_glstate("BIND GEN Chunk::setup_sky_array_buffer", false);
	}//else{
	glBindVertexArray(_skyVao);

	glBindBuffer(GL_ARRAY_BUFFER, _skyVbo);
	// check_glstate("BIND Chunk::setup_sky_array_buffer", false);}
	_mtx_sky.lock();
	glBufferData(GL_ARRAY_BUFFER, _sky_count * 24 * sizeof(GLint), &_sky_vert[0][0], GL_STATIC_DRAW);
	_mtx_sky.unlock();

	// check_glstate("BUFFERDATA Chunk::setup_sky_array_buffer", false);
	_skyVaoReset = false;
	_skyVaoVIP = false;

	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 4, GL_INT, 4 * sizeof(GLint), 0);

	check_glstate("Chunk::setup_sky_array_buffer", false);
}

void Chunk::setup_water_array_buffer( void )
{
	// if (_thread.joinable()) {
	// 	_thread.join();
	// }

	if (!_waterVaoSet) {
		glGenVertexArrays(1, &_waterVao);
		glGenBuffers(1, &_waterVbo);
		_waterVaoSet = true;
	}
	glBindVertexArray(_waterVao);

	glBindBuffer(GL_ARRAY_BUFFER, _waterVbo);
	_mtx_fluid.lock();
	glBufferData(GL_ARRAY_BUFFER, _water_count * 24 * sizeof(GLint), &_water_vert[0][0], GL_STATIC_DRAW);
	_mtx_fluid.unlock();

	_waterVaoReset = false;
	_waterVaoVIP = false;

	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 4, GL_INT, 4 * sizeof(GLint), 0);

	check_glstate("Chunk::setup_water_array_buffer", false);
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

GLint Chunk::getStartX( void )
{
	return (_startX);	
}

GLint Chunk::getStartY( void )
{
	return (_startY);
}

unsigned Chunk::getSeed( void )
{
	return (_seed);
}

bool Chunk::getSortedOnce( void )
{
	return (_sortedOnce);
}

void Chunk::waitGenDone( void )
{
	while (!_genDone);
}

void Chunk::setNeighbour( Chunk *neighbour, int index )
{
	if (_neighbours[index] && neighbour) {
		std::cerr << "setNeighbour ERROR DUPLICATE " << neighbour->getStartX() << ", " << neighbour->getStartY() << " vs " << _neighbours[index]->getStartX() << ", " << _neighbours[index]->getStartY() << std::endl;
		if (_neighbours[index] == neighbour) {
			std::cerr << "but they are the same.." << std::endl;
		}
	}
	_neighbours[index] = neighbour;
}

void Chunk::setBackup( std::map<std::pair<int, int>, s_backup> &backups )
{
	if (_added.size() || _removed.size()) {
		for (auto &sign : _signs) {
			sign.second->setChunk(NULL);
		}
		mtx_backup.lock();
		backups[std::pair<int, int>(_startX, _startY)] = {_added, _removed, _chests, _furnaces, _signs};
		mtx_backup.unlock();
	}
}

void Chunk::restoreBackup( s_backup &backup )
{
	_added = backup.added;
	_removed = backup.removed;
	_chests = backup.chests;
	for (auto &ch : _chests) {
		// std::cout << "chest in backup of " << _startX << " " << _startY << " at " << ch.first << std::endl;
		int posZ = ch.first & (WORLD_HEIGHT - 1);
		int posY = ((ch.first >> WORLD_SHIFT) & (CHUNK_SIZE - 1));
		int posX = ((ch.first >> WORLD_SHIFT) >> CHUNK_SHIFT);
		ch.second->setChunk(this, {posX + _startX, posY + _startY, posZ});
	}
	_furnaces = backup.furnaces;
	_signs = backup.signs;
	for (auto &sign : _signs) {
		sign.second->setChunk(this);
	}
}

void Chunk::openChest( glm::ivec3 pos )
{
	int key = ((((pos.x - _startX) << CHUNK_SHIFT) + pos.y - _startY) << WORLD_SHIFT) + pos.z;
	auto search = _chests.find(key);
	if (search != _chests.end()) {
		search->second->setState(chest_state::OPENING);
	}
}

ChestInstance *Chunk::getChestInstance( glm::ivec3 pos )
{
	int key = ((((pos.x - _startX) << CHUNK_SHIFT) + pos.y - _startY) << WORLD_SHIFT) + pos.z;
	std::map<int, ChestInstance*>::iterator search = _chests.find(key);
	if (search != _chests.end()) {
		return (search->second);
	}
	std::cout << _startX << ", " << _startY << " failed to find chest at " << key << " from " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
	std::cout << "chests values are " << std::endl;
	for (auto& ch: _chests) {
		std::cout << ch.first << std::endl;
	}
	return (NULL);
}

FurnaceInstance *Chunk::getFurnaceInstance( glm::ivec3 pos )
{
	int key = ((((pos.x - _startX) << CHUNK_SHIFT) + pos.y - _startY) << WORLD_SHIFT) + pos.z;
	std::map<int, FurnaceInstance*>::iterator search = _furnaces.find(key);
	if (search != _furnaces.end()) {
		return (search->second);
	}
	std::cout << _startX << ", " << _startY << " failed to find furnace at " << key << " from " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
	std::cout << "furnaces values are " << std::endl;
	for (auto& fur: _furnaces) {
		std::cout << fur.first << std::endl;
	}
	return (NULL);
}

/**
 * @brief find signInstance at given pos and set its content to given content
 * @param sign {pos, content} used to set content of sign at pos
*/
void Chunk::setSignContent( t_sign_info sign )
{
	int key = ((((sign.pos.x - _startX) << CHUNK_SHIFT) + sign.pos.y - _startY) << WORLD_SHIFT) + sign.pos.z;;
	auto search = _signs.find(key);
	if (search == _signs.end()) {
		std::cout << "Chunk::setSignContent [" << _startX << ", " << _startY << "]failed to find sign at pos " << sign.pos.x << ", " << sign.pos.y << ", " << sign.pos.z << std::endl;
		return ;
	}
	search->second->setContent(sign.content);
}

// fill_vertices uses light emited by neighbours, so had to make sure they are loaded before we fill our own vertices in
void Chunk::checkFillVertices( void )
{
	int cnt = 0;
	for (int i = 0; i < 4; i++) {
		if (_neighbours[i]) {
			_neighbours[i]->waitGenDone();
			++cnt;
		}
	}
	if (cnt > _nb_neighbours) {
		if (cnt == 4) {
			for (auto a: _added) { // update torches == block_light spreading, we do it here because before neighbours might not be ready to accept spread
				switch (a.second & mask::blocks::type) {
				case blocks::redstone_lamp:
					if (!(a.second & mask::redstone::activated)) {
						continue ;
					}
				case blocks::torch:
				case blocks::redstone_torch:
					if ((a.second & mask::blocks::type) == blocks::redstone_torch && (a.second & mask::redstone::powered)) {
						break ; // red torch is turned off
					}
					int level = a.first & (WORLD_HEIGHT - 1);
					int col = ((a.first >> WORLD_SHIFT) & (CHUNK_SIZE - 1));
					int row = ((a.first >> WORLD_SHIFT) >> CHUNK_SHIFT);
					_lights[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] &= 0xFF00; // discard previous light value TODO change this if different light source level implemented
					_lights[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] += s_blocks[a.second & mask::blocks::type]->light_level + (s_blocks[a.second & mask::blocks::type]->light_level << 4); // 0xF0 = light source. & 0xF = light level
					light_spread(row, col, level, false);
					break ;
				}
			}
			// this time spread sky_light underground
			for (int row = 0; row < CHUNK_SIZE; row++) {
				for (int col = 0; col < CHUNK_SIZE; col++) {
					for (int level = WORLD_HEIGHT - 1; level > 0; level--) {
						if (!(_lights[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level] & 0xF000)) {
							int value = _blocks[(((row << CHUNK_SHIFT) + col) << WORLD_SHIFT) + level];
							if (s_blocks[value & mask::blocks::type]->transparent) { // underground hole
								// spread_light TODO watch out for leaves and water light damping..
								light_spread(row, col, level, true);
							}
						}
					}
				}
			}
		}
		resetDisplayedFaces();
		// std::cout << "chunk " << _startX << ", " << _startY << ": " << _displayed_faces << std::endl;
		fill_vertex_array();
		_vaoVIP = false;
		_nb_neighbours = cnt; // do this at end to make sure resetDisplayedFaces is called before any light_update shinanigans
	} else if (cnt < _nb_neighbours) {
		_nb_neighbours = cnt;
	} else if (!cnt) {
		std::cerr << "ERROR chunk has no neighbours" << std::endl;
	}
}

void Chunk::dropEntity( glm::vec3 dir, t_item item )
{
	glm::vec3 camPos = _camera->getPos();
	camPos.z += 1;
	camPos += dir;
	_entities.push_back(new Entity(this, _inventory, camPos, dir, true, item));
}

void Chunk::addParticle( Particle *particle )
{
	_particles.push_back(particle);
}

void Chunk::sort_sky( glm::vec3 &pos, bool vip )
{
	// std::cout << "in sort sky" << std::endl;
	if (!_sky_count) {
		return ;
	}
	// pos = glm::vec3(pos.x - _startX, pos.y - _startY, pos.z);
	_mtx_sky.lock();
	std::vector<std::pair<float, std::array<int, 6>>> order;
	order.reserve(_sky_count);
	for (int row = 1; row < CHUNK_SIZE + 1; row++) {
		for (int col = 1; col < CHUNK_SIZE + 1; col++) {
			if (_sky[row * (CHUNK_SIZE + 2) + col]) {
				int pX = _startX + row - 1;
				int pY = _startY + col - 1;
				order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY + 0.5f, 196.0f)), {pX, pY + 1, 196, 1, -1, 0, }});
				order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY + 0.5f, 195.0f)), {pX, pY, 195, 1, 1, 0, }});
				if (!_sky[(row - 1) * (CHUNK_SIZE + 2) + col]) {
					order.push_back({dist2(pos, glm::vec3(pX, pY + 0.5f, 195.5f)), {pX, pY + 1, 196, 0, -1, -1, }});
				}
				if (!_sky[(row + 1) * (CHUNK_SIZE + 2) + col]) {
					order.push_back({dist2(pos, glm::vec3(pX + 1, pY + 0.5f, 195.5f)), {pX + 1, pY, 196, 0, 1, -1, }});
				}
				if (!_sky[row * (CHUNK_SIZE + 2) + col - 1]) {
					order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY, 195.5f)), {pX, pY, 196, 1, 0, -1, }});
				}
				if (!_sky[row * (CHUNK_SIZE + 2) + col + 1]) {
					order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY + 1, 195.5f)), {pX + 1, pY + 1, 196, -1, 0, -1, }});
				}
			}
		}
	}
	_mtx_sky.unlock();
	// std::cout << "before sort" << std::endl;
	// 	std::cout << "_sky_vert malloc " << _sky_count * 24 << std::endl;
	// 	std::cout << "order size " << order.size() << std::endl;
	
	if (order.size() != _sky_count) {
		std::cerr << "sort_sky, order size " << order.size() << ", but _sky_count " << _sky_count << std::endl;
		return ;
	}

	std::sort(order.begin(), order.end(), []( std::pair<float, std::array<int, 6>> a, std::pair<float, std::array<int, 6>> b )
                                  {
                                      return (a.first > b.first);
                                  });
	_sky_vert.clear();
	for (auto& o: order) {
		glm::ivec4 start = {o.second[0], o.second[1], o.second[2], 0}, offset0, offset1, offset2;
		if (!o.second[5]) {
			offset0 = start + glm::ivec4(o.second[3], 0, 0, 0);
			offset1 = start + glm::ivec4(0, o.second[4], 0, 0);
			offset2 = start + glm::ivec4(o.second[3], o.second[4], 0, 0);
		} else {
			offset0 = start + glm::ivec4(o.second[3], o.second[4], 0, 0);
			offset1 = start + glm::ivec4(0, 0, o.second[5], 0);
			offset2 = start + glm::ivec4(o.second[3], o.second[4], o.second[5], 0);
		}
		// std::cout << "vindex " << vindex << std::endl;
		_mtx_sky.lock();
		face_water_vertices(_sky_vert, start, offset0, offset1, offset2);
		_mtx_sky.unlock();
	}
	order.clear();
	_skyVaoReset = true;
	if (vip) {
		_mtx.lock();
		_skyVaoVIP = true;
		_mtx.unlock();
	}
}

bool Chunk::inPerimeter( int posX, int posY, GLint render_dist )
{
	while (!_genDone); // wait for generation taking place in another thread to be done
	return (_startX >= posX - render_dist && _startX <= posX + render_dist
			&& _startY >= posY - render_dist && _startY <= posY + render_dist);
}

int Chunk::manhattanDist( int posX, int posY )
{
	int distX = (_startX > posX) ? _startX - posX : posX - _startX;
	int distY = (_startY > posY) ? _startY - posY : posY - _startY;
	return (distX + distY);
}

bool Chunk::isInChunk( int posX, int posY )
{
	return (posX == _startX && posY == _startY);
}

int Chunk::isHit( glm::ivec3 pos )
{
	// std::cout << "current_chunk is " << _startX << ", " << _startY << std::endl;
	glm::ivec3 chunk_pos = glm::ivec3(pos.x - _startX, pos.y - _startY, pos.z);
	if (chunk_pos.z < 0 || chunk_pos.z > 255) {
		return (blocks::air);
	}
	if (chunk_pos.x < 0 || chunk_pos.x >= CHUNK_SIZE || chunk_pos.y < 0 || chunk_pos.y >= CHUNK_SIZE) {
		std::cout << "ERROR block out of chunk " << chunk_pos.x << ", " << chunk_pos.y << ", " << chunk_pos.z << std::endl;
		return (blocks::air);
	}
	// if (_thread.joinable()) {
	// 	_thread.join();
	// }
	int type = _blocks[(((chunk_pos.x << CHUNK_SHIFT) + chunk_pos.y) << WORLD_SHIFT) + chunk_pos.z];
	if ((type & mask::blocks::type) > blocks::water) {
		return (blocks::air);
	}
	return (type);
}

void Chunk::handleBlast( glm::vec3 pos, int blast_radius )
{
	// update current entities
	// items get destroyed if in range
	// falling blocks and tnts get propelled if in ranges
	for (auto e : _entities) {
		e->getBlasted(pos, blast_radius);
	}

	glm::vec3 relative = {pos.x - _startX, pos.y - _startY, pos.z};
	if (relative.x > 0 && relative.x - blast_radius < 0 && _neighbours[face_dir::minus_x]) {
		_neighbours[face_dir::minus_x]->handleBlast(pos, blast_radius);
	}
	if (relative.x <= CHUNK_SIZE && relative.x + blast_radius >= CHUNK_SIZE && _neighbours[face_dir::plus_x]) {
		_neighbours[face_dir::plus_x]->handleBlast(pos, blast_radius);
	}

	if (relative.y > 0 && relative.y - blast_radius < 0 && _neighbours[face_dir::minus_y]) {
		_neighbours[face_dir::minus_y]->handleBlast(pos, blast_radius);
	}
	if (relative.y <= CHUNK_SIZE && relative.y + blast_radius >= CHUNK_SIZE && _neighbours[face_dir::plus_y]) {
		_neighbours[face_dir::plus_y]->handleBlast(pos, blast_radius);
	}
}

// explosions from TNT cause a 100% drop rate, otherwise 1/power drop rate
void Chunk::explosion( glm::vec3 pos, int power )
{
	handleBlast(pos, 2 * power);

	// std::cout << "EXPLOSION POWER " << power << std::endl;
	// we launch a ray in each OUTER intersection of a 16x16x16 grid == 1352 rays
	for (int row = 0; row < 16; ++row) {
		for (int col = 0; col < 16; ++col) {
			for (int level = 0; level < 16; ++level) {
				if (!row || !col || !level || row == 15 || col == 15 || level == 15) {
					float intensity = (0.7f + 0.6f * Random::randomFloat(_seed)) * power; // [0.7:1.3] * power
					glm::vec3 end = pos + glm::normalize(glm::vec3(-7.5f + row, -7.5f + col, -7.5f + level)) * intensity;
					std::vector<glm::ivec3> vt = voxel_traversal(pos, end);
					intensity += 0.75f;
					for (auto &p : vt) {
						int type = getBlockAt(p.x - _startX, p.y - _startY, p.z) & mask::blocks::type;
						intensity -= 0.75f + s_blocks[type]->blast_resistance;
						if (intensity < 0) {
							break ;
						} else if (type == blocks::tnt) {
							handleHit(false, type, p, Modif::LITNT);
							_entities.back()->setLifetime(3.5 - Random::randomFloat(_seed)); // tnt lit by explosion has lifetime in [0.5;1.5] sec
							// TODO better particles spawning
							_particles.push_back(new Particle(this, {p.x + Random::randomFloat(_seed), p.y + Random::randomFloat(_seed), p.z + Random::randomFloat(_seed)}, PARTICLES::EXPLOSION, Random::randomFloat(_seed)));
						} else if (type != blocks::air) {
							// std::cout << "block " << s_blocks[type]->name << " removed" << std::endl;
							handleHit(true, type, p, Modif::REMOVE);
							// handleHit(false, type, p, Modif::REMOVE);
							_particles.push_back(new Particle(this, {p.x + Random::randomFloat(_seed), p.y + Random::randomFloat(_seed), p.z + Random::randomFloat(_seed)}, PARTICLES::EXPLOSION, Random::randomFloat(_seed)));
						}
					}
				}
			}
		}
	}
	// std::cout << "EXPLOSION OVER" << std::endl;
}

void Chunk::shootArrow( float timer )
{
	timer = (timer > 1.0f) ? 20.0f : timer * 20.0f;
	glm::vec3 camPos = _camera->getEyePos(), camDir = _camera->getDir();
	camPos += camDir;
	_entities.push_back(new ArrowEntity(this, camPos, camDir * timer));
}

void Chunk::updateBreak( glm::ivec4 block_hit )
{
	if (block_hit.w == blocks::air || !_vaoReset) {
		return ;
	}
	glm::ivec3 chunk_pos = {block_hit.x - _startX, block_hit.y - _startY, block_hit.z};
	if (chunk_pos.x < 0 || chunk_pos.x >= CHUNK_SIZE || chunk_pos.y < 0 || chunk_pos.y >= CHUNK_SIZE || chunk_pos.z < 0 || chunk_pos.z > 255) {
		std::cout << "ERROR block hit out of chunk " << chunk_pos.x << ", " << chunk_pos.y << ", " << chunk_pos.z << std::endl;
		return ;
	}
	for (int index = 0; index < 6; ++index) {
		const glm::ivec3 delta = adj_blocks[index];
		_particles.push_back(new Particle(this, {block_hit.x + 0.5f + (Random::randomFloat(_seed) - 0.5f) * !delta.x + 0.55f * delta.x, block_hit.y + 0.5f + (Random::randomFloat(_seed) - 0.5f) * !delta.y + 0.55f * delta.y, block_hit.z + 0.5f + (Random::randomFloat(_seed) - 0.5f) * !delta.z + 0.55f * delta.z}, PARTICLES::BREAKING, 0, block_hit.w));
	}
}

int Chunk::isLoaded( GLint &counter )
{
	_mtx.lock();
	if (!_vaoSet) {
		_mtx.unlock();
		++counter;
		if ((counter % 50) <= 5) {
			setup_array_buffer();
		}
		return (0);
	}
	_mtx.unlock();
	return (1);
}

void Chunk::drawArray( GLint & counter, GLint &face_counter )
{
	if (_light_update && _nb_neighbours == 4) {
		std::cout << _startX << ", " << _startY << " light update" << std::endl;
		fill_vertex_array();
		// std::cout << "over." << std::endl;
	} else if (_vertex_update && _nb_neighbours == 4) {
		// std::cout << _startX << ", " << _startY << " vertex update (prob crop)" << std::endl;
		fill_vertex_array();
	}
	if (!_vaoReset) { // TODO change vaoReset logic (swap true and false)
		++counter;
		if (!_vaoVIP && (counter & 63) > 6) { // we don't load more than 5 new chunks per 50 new chunks per frame
		// std::cout << "skip one" << std::endl;
			return ;
		}
		// std::cout << "chunk reset " << _startX << ", " << _startY << std::endl;
		setup_array_buffer();
	}
    glBindVertexArray(_vao); // this is the costly operation, chunk_size up == fps down
	glDrawArrays(GL_TRIANGLES, 0, _displayed_faces * 6);
	face_counter += _displayed_faces;
}

void Chunk::updateFurnaces( double currentTime )
{
	for (auto& fur: _furnaces) {
		int state = fur.second->updateTimes(currentTime);
		if (state != furnace_state::NOCHANGE) {
			// std::cout << "FURNACE STATE CHANGE TO " << state << std::endl;
			int value = _blocks[fur.first];
			_blocks[fur.first] = (value & (mask::all_bits - mask::redstone::activated)) + ((state == furnace_state::ON) << offset::redstone::activated);
			// set/unset furnace position as light source of 13 using state
			int posZ = fur.first & (WORLD_HEIGHT - 1);
			int posY = ((fur.first >> WORLD_SHIFT) & (CHUNK_SIZE - 1));
			int posX = ((fur.first >> WORLD_SHIFT) >> CHUNK_SHIFT);
			// std::cout << "furnace at " << _startX + posX << ", " << _startY + posY << ", " << posZ << std::endl;
			if (state == furnace_state::ON) {
				_lights[fur.first] = 13 + (13 << 4);
				light_spread(posX, posY, posZ, false); // spread block light
			} else {
				_lights[fur.first] = 0;
				for (int index = 0; index < 6; index++) {
					const glm::ivec3 delta = adj_blocks[index];
					light_try_spread(posX + delta.x, posY + delta.y, posZ + delta.z, 0, false, LIGHT_RECURSE);
				}
			}
			_light_update = true;
		}
	}
}

void Chunk::updateEntities( std::vector<t_shaderInput> &arr,  std::vector<t_shaderInput> &partArr, double deltaTime )
{
	// TODO merge identical close(3/4 of a block) stackable items together
	// 			on merge, item timer set to longest of 2
	// 			double for loop, use bool given as parameter to do this once per second at most

	glm::vec3 camPos;
	size_t eSize = _entities.size();
	if (!eSize) {
		goto CHESTS;
	}

	camPos = _camera->getPos();
	for (size_t index = 0, delCount = 0; index < eSize; ++index) {
		if (_entities[index - delCount]->update(arr, partArr, camPos, deltaTime)) {
			delete _entities[index - delCount];
			_entities.erase(_entities.begin() + index - delCount);
			++delCount;
		}
	}

	CHESTS:
	for (auto &c : _chests) {
		c.second->update(arr, deltaTime);
	}

	for (auto &s : _signs) {
		s.second->displayText(partArr);
	}
}

size_t Chunk::clearEntities( void )
{
	size_t res = _entities.size();
	for (auto e : _entities) {
		delete e;
	}
	_entities.clear();
	return (res);
}

void Chunk::updateParticles( std::vector<t_shaderInput> &arr, double deltaTime )
{
	glm::vec3 camPos = _camera->getPos(), camDir = _camera->getDir();
	size_t size = _particles.size();
	if (!size) {
		goto FLAMES;
	}

	for (int index = size - 1; index >= 0; --index) {
		if (_particles[index]->update(arr, camPos, camDir, deltaTime)) {
			delete _particles[index];
			_particles.erase(_particles.begin() + index);
		}
	}

	FLAMES:
	for (auto &f : _flames) {
		f.second->update(arr, camPos, camDir, deltaTime);
	}
}

size_t Chunk::clearParticles( void )
{
	size_t res = _particles.size();
	for (auto p : _particles) {
		delete p;
	}
	_particles.clear();
	return (res);
}

void Chunk::drawSky( GLint & counter, GLint &face_counter )
{
	if (!_sky_count) {
		return ;
	}
	if (_skyVaoReset) {
		// std::cout << "chunk reset " << _startX << ", " << _startY << std::endl;
		++counter;
		if (!_skyVaoVIP && !_skyVaoSet && counter > 5) { // we don't load more than 5 new chunks per frame
			return ;
		} else if (_skyVaoVIP || counter < 6) {
			setup_sky_array_buffer();
		}
	}
    glBindVertexArray(_skyVao);
	glDrawArrays(GL_TRIANGLES, 0, _sky_count * 6); // 6 points/face
	// std::cout << "draw sky" << std::endl;
	face_counter += _sky_count;
}

void Chunk::drawWater( GLint & counter, GLint &face_counter )
{
	if (!_water_count) {
		return ;
	}
	if (_waterVaoReset) {
		// std::cout << "chunk reset " << _startX << ", " << _startY << std::endl;
		++counter;
		if (!_waterVaoVIP && !_waterVaoSet && counter > 5) { // we don't load more than 5 new chunks per frame
			return ;
		} else if (_waterVaoVIP || counter < 6) {
			setup_water_array_buffer();
		}
	}
    glBindVertexArray(_waterVao);
	glDrawArrays(GL_TRIANGLES, 0, _water_count * 6); // 6 points/face
	// std::cout << "draw water" << std::endl;
	face_counter += _water_count;
}

std::string Chunk::getAddsRmsString( void )
{
	std::string res = "\nADDED\t> " + std::to_string(_added.size()) + "\n";
	for (auto& add: _added) {
		res += "\t\t\t" + std::to_string(add.first) + ": " + std::to_string(add.second) + '\n';
	}
	res += "\nRMED\t> " + std::to_string(_removed.size()) + "\n";
	for (auto& rm: _removed) {
		res += "\t\t\t" + std::to_string(rm) + '\n';
	}
	return (res);
}
