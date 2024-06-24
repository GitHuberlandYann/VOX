#include "Player.hpp"
#include "random.hpp"
#include "Menu.hpp"
#include "logs.hpp"

extern std::mutex mtx_backup;

Chunk::Chunk( Player *player, Inventory *inventory, int posX, int posY, std::list<Chunk *> *chunks )
	: _vaoSet(false), _waterVaoSet(false), _waterVaoVIP(false),
	_skyVaoSet(false), _skyVaoVIP(false), _genDone(false), _light_update(false), _vertex_update(false),
	_vaoReset(false), _vaoVIP(false), _waterVaoReset(false), _skyVaoReset(false), _sortedOnce(false),
	_startX(posX), _startY(posY), _nb_neighbours(0),
	_hasWater(true), _displayed_faces(0), _water_count(0), _sky_count(0),
	_neighbours({NULL, NULL, NULL, NULL}), _player(player), _inventory(inventory)
{
	int cnt = 0;
// std::cout << "new chunk at " << posX << ", " << posY << std::endl;
	for (auto c : *chunks) { // TODO load backups if needed. for trees
		if (c->isInChunk(_startX - settings::consts::chunk_size, _startY)) {
			_neighbours[face_dir::minus_x] = c;
			c->setNeighbour(this, face_dir::plus_x);
			++cnt;
		} else if (c->isInChunk(_startX + settings::consts::chunk_size, _startY)) {
			_neighbours[face_dir::plus_x] = c;
			c->setNeighbour(this, face_dir::minus_x);
			++cnt;
		} else if (c->isInChunk(_startX, _startY - settings::consts::chunk_size)) {
			_neighbours[face_dir::minus_y] = c;
			c->setNeighbour(this, face_dir::plus_y);
			++cnt;
		} else if (c->isInChunk(_startX, _startY + settings::consts::chunk_size)) {
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

	_vabo.deleteBuffers();
	_vaboSky.deleteBuffers();
	_vaboWater.deleteBuffers();

	// chests and furnaces are deleted in OpenGl_Manager when deleting backups
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
	if (!type || type == blocks::water) {
		LOGERROR("face_count ERROR counting " << s_blocks[type]->name);
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
	if (s_blocks[type]->geometry == geometry::stairs) {
		res += 2; // +3 if corner stair, but np so far
	}
	return (res);
}

void Chunk::resetDisplayedFaces( void )
{
	// hiding unseen blocks
	for (int row = 0; row < settings::consts::chunk_size; row++) {
		for (int col = 0; col < settings::consts::chunk_size; col++) {
			for (int level = 0; level < settings::consts::world_height; level++) {
				int offset = (((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level;
				GLint value = _blocks[offset], type = value & mask::blocks::type;
				bool restore = false;
				if ((value & mask::blocks::notVisible) && (!row || !col || row == settings::consts::chunk_size - 1 || col == settings::consts::chunk_size - 1)) {
					value -= mask::blocks::notVisible;
					restore = true;
				}
				if (type != blocks::air && type != blocks::chest && type != blocks::water) {
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
						} else { // hide block
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
		_vabo.genBuffers();
		_vabo.addAttribute(settings::consts::shader::attributes::specifications, 1, GL_INT);
		_vabo.addAttribute(settings::consts::shader::attributes::position, 3, GL_FLOAT);
		_vaoSet = true;
	}

	_displayed_faces = _vertices.size() / 6;
	_mtx.lock();
	_vabo.uploadData(_vertices.size(), &_vertices[0].spec);
	_mtx.unlock();

	_vaoReset = true;
	_vaoVIP = false;

	check_glstate("Chunk::setup_array_buffer", false);
}

void Chunk::setup_sky_array_buffer( void )
{
	// if (_thread.joinable()) {
	// 	_thread.join();
	// }

	// check_glstate("BEFORE Chunk::setup_sky_array_buffer", false);
	if (!_skyVaoSet) {
		_vaboSky.genBuffers();
		_vaboSky.addAttribute(settings::consts::shader::attributes::position, 4, GL_INT);
		_skyVaoSet = true;
		// check_glstate("GEN Chunk::setup_sky_array_buffer", false);
	// glBindVertexArray(_skyVao);

	// glBindBuffer(GL_ARRAY_BUFFER, _skyVbo);
	// check_glstate("BIND GEN Chunk::setup_sky_array_buffer", false);
	}//else{

	// check_glstate("BIND Chunk::setup_sky_array_buffer", false);}
	_mtx_sky.lock();
	_vaboSky.uploadData(_sky_count * 6, &_sky_vert[0][0]);
	// glBufferData(GL_ARRAY_BUFFER, _sky_count * 24 * sizeof(GLint), &_sky_vert[0][0], GL_STATIC_DRAW);
	_mtx_sky.unlock();

	// check_glstate("BUFFERDATA Chunk::setup_sky_array_buffer", false);
	_skyVaoReset = false;
	_skyVaoVIP = false;

	check_glstate("Chunk::setup_sky_array_buffer", false);
}

void Chunk::setup_water_array_buffer( void )
{
	// if (_thread.joinable()) {
	// 	_thread.join();
	// }

	if (!_waterVaoSet) {
		_vaboWater.genBuffers();
		_vaboWater.addAttribute(settings::consts::shader::attributes::position, 4, GL_INT);
		_waterVaoSet = true;
	}

	_mtx_fluid.lock();
	_vaboWater.uploadData(_water_count * 6, &_water_vert[0][0]);
	// glBufferData(GL_ARRAY_BUFFER, _water_count * 24 * sizeof(GLint), &_water_vert[0][0], GL_STATIC_DRAW);
	_mtx_fluid.unlock();

	_waterVaoReset = false;
	_waterVaoVIP = false;

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

/**
 * @brief get ptr to neighbour whose _startX and _startY are those given
 */
Chunk* Chunk::getChunkAt( int startX, int startY )
{
	if (startX < _startX) {
		return (_neighbours[face_dir::minus_x]);
	} else if (startX > _startX) {
		return (_neighbours[face_dir::plus_x]);
	} else if (startY < _startY) {
		return (_neighbours[face_dir::minus_y]);
	} else if (startY > _startY) {
		return (_neighbours[face_dir::plus_y]);
	}
	return (this);
}

unsigned& Chunk::getSeed( void )
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

void Chunk::setNeighbour( Chunk* neighbour, int index )
{
	if (_neighbours[index] && neighbour) {
		LOGERROR("setNeighbour ERROR DUPLICATE " << neighbour->getStartX() << ", " << neighbour->getStartY() << " vs " << _neighbours[index]->getStartX() << ", " << _neighbours[index]->getStartY());
		if (_neighbours[index] == neighbour) {
			LOGERROR("but they are the same..");
		}
	}
	_neighbours[index] = neighbour;
}

/**
 * @brief load own information on backup before being destroyed
 */
void Chunk::setBackup( std::map<std::pair<int, int>, s_backup>& backups )
{
	if (_added.size() || _removed.size()) {
		for (auto &sign : _signs) {
			sign.second->setChunk(NULL);
		}
		std::remove_if(_entities.begin(), _entities.end(),
			[]( auto e ) { return (dynamic_cast<ItemFrameEntity*>(e.get()) == nullptr); }); // rm all entities which are not ItemFrameEntities
		mtx_backup.lock();
		backups[std::pair<int, int>(_startX, _startY)] = {_added, _removed, _chests, _furnaces, _signs, _entities};
		mtx_backup.unlock();
	}
}

void Chunk::restoreBackup( s_backup& backup )
{
	_added = backup.added;
	_removed = backup.removed;
	_chests = backup.chests;
	for (auto &ch : _chests) {
		// std::cout << "chest in backup of " << _startX << " " << _startY << " at " << ch.first << std::endl;
		int posZ = ch.first & (settings::consts::world_height - 1);
		int posY = ((ch.first >> settings::consts::world_shift) & (settings::consts::chunk_size - 1));
		int posX = ((ch.first >> settings::consts::world_shift) >> settings::consts::chunk_shift);
		ch.second->setChunk(this, {posX + _startX, posY + _startY, posZ});
	}
	_furnaces = backup.furnaces;
	_signs = backup.signs;
	for (auto &sign : _signs) {
		sign.second->setChunk(this);
	}
	_entities = backup.item_frames;
	for (auto &e : _entities) {
		e->setChunk(this);
	}
}

void Chunk::openChest( glm::ivec3 pos )
{
	int key = ((((pos.x - _startX) << settings::consts::chunk_shift) + pos.y - _startY) << settings::consts::world_shift) + pos.z;
	auto search = _chests.find(key);
	if (search != _chests.end()) {
		search->second->setState(chest_state::OPENING);
	}
}

ChestInstance *Chunk::getChestInstance( glm::ivec3 pos )
{
	int key = ((((pos.x - _startX) << settings::consts::chunk_shift) + pos.y - _startY) << settings::consts::world_shift) + pos.z;
	std::map<int, ChestInstance*>::iterator search = _chests.find(key);
	if (search != _chests.end()) {
		return (search->second);
	}
	LOGERROR(_startX << ", " << _startY << " failed to find chest at " << key << " from " << POS(pos));
	LOGERROR("chests values are ");
	for (auto& ch: _chests) {
		LOGERROR(ch.first);
	}
	return (NULL);
}

FurnaceInstance *Chunk::getFurnaceInstance( glm::ivec3 pos )
{
	int key = ((((pos.x - _startX) << settings::consts::chunk_shift) + pos.y - _startY) << settings::consts::world_shift) + pos.z;
	std::map<int, FurnaceInstance*>::iterator search = _furnaces.find(key);
	if (search != _furnaces.end()) {
		return (search->second);
	}
	LOGERROR(_startX << ", " << _startY << " failed to find furnace at " << key << " from " << POS(pos));
	LOGERROR("furnaces values are ");
	for (auto& fur: _furnaces) {
		LOGERROR(fur.first);
	}
	return (NULL);
}

/**
 * @brief find signInstance at given pos and set its content to given content
 * @param sign {pos, content} used to set content of sign at pos
*/
void Chunk::setSignContent( t_sign_info sign )
{
	int key = ((((sign.pos.x - _startX) << settings::consts::chunk_shift) + sign.pos.y - _startY) << settings::consts::world_shift) + sign.pos.z;;
	auto search = _signs.find(key);
	if (search == _signs.end()) {
		LOGERROR("Chunk::setSignContent [" << _startX << ", " << _startY << "]failed to find sign at pos " << POS(sign.pos));
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
					int level = a.first & (settings::consts::world_height - 1);
					int col = ((a.first >> settings::consts::world_shift) & (settings::consts::chunk_size - 1));
					int row = ((a.first >> settings::consts::world_shift) >> settings::consts::chunk_shift);
					_lights[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level] &= 0xFF00; // discard previous light value TODO change this if different light source level implemented
					_lights[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level] += s_blocks[a.second & mask::blocks::type]->light_level + (s_blocks[a.second & mask::blocks::type]->light_level << 4); // 0xF0 = light source. & 0xF = light level
					light_spread(row, col, level, false);
					break ;
				}
			}
			// this time spread sky_light underground
			for (int row = 0; row < settings::consts::chunk_size; row++) {
				for (int col = 0; col < settings::consts::chunk_size; col++) {
					for (int level = settings::consts::world_height - 1; level > 0; level--) {
						if (!(_lights[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level] & 0xF000)) {
							int value = _blocks[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level];
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
		fill_vertex_array();
		_vaoVIP = false;
		_nb_neighbours = cnt; // do this at end to make sure resetDisplayedFaces is called before any light_update shinanigans
	} else if (cnt < _nb_neighbours) {
		_nb_neighbours = cnt;
	} else if (!cnt) {
		LOGERROR("ERROR chunk has no neighbours");
	}
}

void Chunk::dropEntity( glm::vec3 dir, t_item item )
{
	glm::vec3 camPos = _player->getPos();
	camPos.z += 1;
	camPos += dir;
	_entities.push_back(std::make_shared<Entity>(this, _inventory, camPos, dir, true, item));
}

void Chunk::addParticle( Particle* particle )
{
	_particles.push_back(particle);
}

void Chunk::sort_sky( glm::vec3& pos, bool vip )
{
	// std::cout << "in sort sky" << std::endl;
	if (!_sky_count) {
		return ;
	}
	// pos = glm::vec3(pos.x - _startX, pos.y - _startY, pos.z);
	_mtx_sky.lock();
	std::vector<std::pair<float, std::array<int, 6>>> order;
	order.reserve(_sky_count);
	for (int row = 1; row < settings::consts::chunk_size + 1; row++) {
		for (int col = 1; col < settings::consts::chunk_size + 1; col++) {
			if (_sky[row * (settings::consts::chunk_size + 2) + col]) {
				int pX = _startX + row - 1;
				int pY = _startY + col - 1;
				order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY + 0.5f, 196.0f)), {pX, pY + 1, 196, 1, -1, 0, }});
				order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY + 0.5f, 195.0f)), {pX, pY, 195, 1, 1, 0, }});
				if (!_sky[(row - 1) * (settings::consts::chunk_size + 2) + col]) {
					order.push_back({dist2(pos, glm::vec3(pX, pY + 0.5f, 195.5f)), {pX, pY + 1, 196, 0, -1, -1, }});
				}
				if (!_sky[(row + 1) * (settings::consts::chunk_size + 2) + col]) {
					order.push_back({dist2(pos, glm::vec3(pX + 1, pY + 0.5f, 195.5f)), {pX + 1, pY, 196, 0, 1, -1, }});
				}
				if (!_sky[row * (settings::consts::chunk_size + 2) + col - 1]) {
					order.push_back({dist2(pos, glm::vec3(pX + 0.5f, pY, 195.5f)), {pX, pY, 196, 1, 0, -1, }});
				}
				if (!_sky[row * (settings::consts::chunk_size + 2) + col + 1]) {
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
		LOGERROR("sort_sky, order size " << order.size() << ", but _sky_count " << _sky_count);
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

/**
 * @brief return whether there is a clear line of sight between to 3d points
 * A line of sight is 'clear' as long as there are no solid blocks between the points
 * @param src start of line segment
 * @param dst end of line segment
 */
bool Chunk::lineOfSight( const glm::vec3 src, const glm::vec3 dst )
{
	const std::vector<glm::ivec3> ids = voxel_traversal(src, dst);

	for (auto i : ids) {
		int value = getBlockAtAbsolute(i);

		if (!s_blocks[value & mask::blocks::type]->transparent) {
			return (false);
		}
	}

	return (true);
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
	if (chunk_pos.x < 0 || chunk_pos.x >= settings::consts::chunk_size || chunk_pos.y < 0 || chunk_pos.y >= settings::consts::chunk_size) {
		LOGERROR("ERROR block out of chunk " << POS(chunk_pos));
		return (blocks::air);
	}
	// if (_thread.joinable()) {
	// 	_thread.join();
	// }
	int value = _blocks[(((chunk_pos.x << settings::consts::chunk_shift) + chunk_pos.y) << settings::consts::world_shift) + chunk_pos.z];
	if ((value & mask::blocks::type) == blocks::water && ((value >> offset::blocks::bitfield) & 0x7) > 0) {
		return (blocks::air);
	}
	return (value);
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
	if (relative.x <= settings::consts::chunk_size && relative.x + blast_radius >= settings::consts::chunk_size && _neighbours[face_dir::plus_x]) {
		_neighbours[face_dir::plus_x]->handleBlast(pos, blast_radius);
	}

	if (relative.y > 0 && relative.y - blast_radius < 0 && _neighbours[face_dir::minus_y]) {
		_neighbours[face_dir::minus_y]->handleBlast(pos, blast_radius);
	}
	if (relative.y <= settings::consts::chunk_size && relative.y + blast_radius >= settings::consts::chunk_size && _neighbours[face_dir::plus_y]) {
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
							handleHit(false, type, p, Modif::litnt);
							_entities.back()->setLifetime(3.5 - Random::randomFloat(_seed)); // tnt lit by explosion has lifetime in [0.5;1.5] sec
							// TODO better particles spawning
							_particles.push_back(new Particle(this, {p.x + Random::randomFloat(_seed), p.y + Random::randomFloat(_seed), p.z + Random::randomFloat(_seed)}, PARTICLES::EXPLOSION, Random::randomFloat(_seed)));
						} else if (type != blocks::air) {
							// std::cout << "block " << s_blocks[type]->name << " removed" << std::endl;
							handleHit(true, type, p, Modif::rm);
							// handleHit(false, type, p, Modif::rm);
							_particles.push_back(new Particle(this, {p.x + Random::randomFloat(_seed), p.y + Random::randomFloat(_seed), p.z + Random::randomFloat(_seed)}, PARTICLES::EXPLOSION, Random::randomFloat(_seed)));
						}
					}
				}
			}
		}
	}
	// std::cout << "EXPLOSION OVER" << std::endl;
}

void Chunk::shootArrow( const glm::vec3 start, const glm::vec3 dir, float timer )
{
	timer = (timer > 1.0f) ? 20.0f : timer * 20.0f;
	_entities.push_back(std::make_shared<ArrowEntity>(this, start + dir, dir * timer));
}

void Chunk::updateBreak( glm::ivec4 block_hit )
{
	if (block_hit.w == blocks::air || !_vaoReset) {
		return ;
	}
	glm::ivec3 chunk_pos = {block_hit.x - _startX, block_hit.y - _startY, block_hit.z};
	if (chunk_pos.x < 0 || chunk_pos.x >= settings::consts::chunk_size || chunk_pos.y < 0 || chunk_pos.y >= settings::consts::chunk_size || chunk_pos.z < 0 || chunk_pos.z > 255) {
		LOGERROR("ERROR block hit out of chunk " << POS(chunk_pos));
		return ;
	}
	for (int index = 0; index < 6; ++index) {
		const glm::ivec3 delta = adj_blocks[index];
		_particles.push_back(new Particle(this, {block_hit.x + 0.5f + (Random::randomFloat(_seed) - 0.5f) * !delta.x + 0.55f * delta.x, block_hit.y + 0.5f + (Random::randomFloat(_seed) - 0.5f) * !delta.y + 0.55f * delta.y, block_hit.z + 0.5f + (Random::randomFloat(_seed) - 0.5f) * !delta.z + 0.55f * delta.z}, PARTICLES::BREAKING, 0, block_hit.w));
	}
}

int Chunk::isLoaded( GLint& counter )
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

void Chunk::drawArray( GLint& counter, GLint& face_counter )
{
	if (_light_update && _nb_neighbours == 4) {
		MAINLOG(LOG(_startX << ", " << _startY << " light update"));
		fill_vertex_array();
	} else if (_vertex_update && _nb_neighbours == 4) {
		fill_vertex_array();
	}
	if (!_vaoReset) { // TODO change vaoReset logic (swap true and false)
		++counter;
		if (!_vaoVIP && (counter & 63) > 6) { // we don't load more than 5 new chunks per 50 new chunks per frame
			return ;
		}
		setup_array_buffer();
	}
    _vabo.bindVertexArray(); // this is the costly operation, chunk_size up == fps down
	glDrawArrays(GL_TRIANGLES, 0, _displayed_faces * 6);
	face_counter += _displayed_faces;
}

void Chunk::updateFurnaces( double currentTime )
{
	for (auto& fur: _furnaces) {
		int state = fur.second->updateTimes(currentTime);
		if (state != furnace_state::NOCHANGE) {
			FURNACELOG(LOG("FURNACE STATE CHANGE TO " << state));
			int value = _blocks[fur.first];
			_blocks[fur.first] = (value & (mask::all_bits - mask::redstone::activated)) + ((state == furnace_state::ON) << offset::redstone::activated);
			// set/unset furnace position as light source of 13 using state
			int posZ = fur.first & (settings::consts::world_height - 1);
			int posY = ((fur.first >> settings::consts::world_shift) & (settings::consts::chunk_size - 1));
			int posX = ((fur.first >> settings::consts::world_shift) >> settings::consts::chunk_shift);
			FURNACELOG(LOG("furnace at " << _startX + posX << ", " << _startY + posY << ", " << posZ));
			if (state == furnace_state::ON) {
				_lights[fur.first] = 13 + (13 << 4);
				light_spread(posX, posY, posZ, false); // spread block light
			} else {
				_lights[fur.first] = 0;
				for (int index = 0; index < 6; index++) {
					const glm::ivec3 delta = adj_blocks[index];
					light_try_spread(posX + delta.x, posY + delta.y, posZ + delta.z, 0, false, settings::consts::light_recurse);
				}
			}
			_light_update = true;
		}
	}
}

void Chunk::addMob( const AMob& mob, int mobType )
{
	switch (mobType) {
		case settings::consts::mob::zombie:
			_mobs.push_back(std::make_shared<Zombie>(static_cast<const Zombie&>(mob)));
			// std::cout << _startX << ", " << _startY << ": new zombie added from neighbouring chunk." << std::endl;
			break ;
		case settings::consts::mob::skeleton:
			_mobs.push_back(std::make_shared<Skeleton>(static_cast<const Skeleton&>(mob)));
			break ;
		default:
			LOGERROR("ERROR Chunk::addMob invalid mobType " << mobType);
			break ;
	}
}

/**
 * @brief loops through all mobs, update and draw them
 */
void Chunk::updateMobs( std::vector<t_shaderInput>& modArr, double deltaTime )
{
	size_t mSize = _mobs.size();
	if (!mSize) {
		return ;
	}

	for (size_t index = 0, delCount = 0; index < mSize; ++index) {
		if (_mobs[index - delCount]->update(modArr, deltaTime)) {
			_mobs.erase(_mobs.begin() + index - delCount);
			++delCount;
		}
	}
}

void Chunk::updateEntities( std::vector<t_shaderInput>& arr, std::vector<t_shaderInput>& partArr, double deltaTime )
{
	// TODO merge identical close(3/4 of a block) stackable items together
	// 			on merge, item timer set to longest of 2
	// 			double for loop, use bool given as parameter to do this once per second at most

	glm::vec3 camPos;
	size_t eSize = _entities.size();
	if (!eSize) {
		goto CHESTS;
	}

	camPos = _player->getPos();
	for (size_t index = 0, delCount = 0; index < eSize; ++index) {
		if (_entities[index - delCount]->update(arr, camPos, deltaTime)) {
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
	_entities.clear();
	return (res);
}

void Chunk::updateParticles( std::vector<t_shaderInput>& entityArr, std::vector<t_shaderInput>& partArr, double deltaTime )
{
	glm::vec3 camPos = _player->getPos(), camDir = _player->getDir();
	size_t size = _particles.size();
	if (!size) {
		goto FLAMES;
	}

	for (int index = size - 1; index >= 0; --index) {
		if (_particles[index]->update(entityArr, partArr, camPos, camDir, deltaTime)) {
			delete _particles[index];
			_particles.erase(_particles.begin() + index);
		}
	}

	FLAMES:
	for (auto &f : _flames) {
		f.second->update(entityArr, partArr, camPos, camDir, deltaTime);
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

void Chunk::drawSky( GLint& counter, GLint& face_counter )
{
	if (!_sky_count) {
		return ;
	}
	if (_skyVaoReset) {
		++counter;
		if (!_skyVaoVIP && !_skyVaoSet && counter > 5) { // we don't load more than 5 new chunks per frame
			return ;
		} else if (_skyVaoVIP || counter < 6) {
			setup_sky_array_buffer();
		}
	}
    _vaboSky.bindVertexArray();
	glDrawArrays(GL_TRIANGLES, 0, _sky_count * 6); // 6 points/face
	face_counter += _sky_count;
}

void Chunk::drawWater( GLint& counter, GLint& face_counter )
{
	if (!_water_count) {
		return ;
	}
	if (_waterVaoReset) {
		++counter;
		if (!_waterVaoVIP && !_waterVaoSet && counter > 5) { // we don't load more than 5 new chunks per frame
			return ;
		} else if (_waterVaoVIP || counter < 6) {
			setup_water_array_buffer();
		}
	}
    _vaboWater.bindVertexArray();
	glDrawArrays(GL_TRIANGLES, 0, _water_count * 6); // 6 points/face
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
