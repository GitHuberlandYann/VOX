#include "Chunk.hpp"
#include "random.hpp"
#include "DayCycle.hpp"

Entity::Entity( Chunk* chunk, Inventory *inventory, glm::vec3 position, glm::vec3 dir, bool thrown, t_item item )
    : _item(item), _lifeTime(0.), _pos(position), _dir(dir), _chunk(chunk), _inventory(inventory), _thrown(thrown)
{
    // std::cout << "new Entity at " << position.x << ", " << position.y << ", " << position.z << ": " << s_blocks[value]->name << std::endl;
	_chunk_pos = {chunk->getStartX(), chunk->getStartY()};
}

Entity::~Entity( void )
{

}

FallingBlockEntity::FallingBlockEntity( Chunk* chunk, glm::vec3 position, t_item item)
	: Entity(chunk, NULL, position, {0, 0, 0}, false, item)
{

}

TNTEntity::TNTEntity( Chunk* chunk, glm::vec3 position, glm::vec3 dir )
	: Entity(chunk, NULL, position, dir, false, {0})
{

}

ArrowEntity::ArrowEntity( Chunk* chunk, glm::vec3 position, glm::vec3 dir )
	: Entity(chunk, NULL, position, dir, false, {0}), _stuck(false)
{

}

MovingPistonEntity::MovingPistonEntity( Chunk* chunk, glm::ivec3 source, glm::ivec3 position, glm::ivec3 dir, bool piston_head, bool retraction, int value )
	: Entity(chunk, NULL, position, dir, false, {value, 1}), _piston_head(piston_head), _retraction(retraction), _softKill(false),
	_tickStart(DayCycle::Get()->getGameTicks()), _source(source), _endPos(position + dir)
{
	// std::cout << "MovingPistonEntity init movement " << ((_retraction) ? "retraction" : "extension") << " [" << _tickStart << "] from " << _chunk->getStartX() + _pos.x << ", " << _chunk->getStartY() + _pos.y << ", " << _pos.z << " to " << _chunk->getStartX() + _endPos.x << ", " << _chunk->getStartY() + _endPos.y << ", " << _endPos.z << std::endl;
}

ItemFrameEntity::ItemFrameEntity( Chunk* chunk, glm::ivec3 position, int value )
	: Entity(chunk, NULL, {0, 0, 0}, {0, 0, 0}, false, {0}), _rotation(0)
{
	_pos = glm::vec3(position.x + .5f, position.y + .5f, position.z + .5f);

	int placement = (value >> offset::blocks::bitfield) & 0x3;
	switch (placement) {
		case placement::floor:
			_front = { .0f, .0f, -1.f};
			_right = { 1.f, .0f, .0f};
			_up    = { .0f, 1.f, .0f};
			break ;
		case placement::ceiling:
			_front = { .0f, .0f, 1.f};
			_right = { 1.f, .0f, .0f};
			_up    = { .0f,-1.f, .0f};
			break ;
		case placement::wall:
			int orientation = (value >> offset::blocks::orientation) & 0x7;
			switch (orientation) {
				case face_dir::minus_x:
					_front = { 1.f, .0f, .0f};
					_right = { .0f,-1.f, .0f};
					_up    = { .0f, .0f, 1.f};
					break ;
				case face_dir::plus_x:
					_front = {-1.f, .0f, .0f};
					_right = { .0f, 1.f, .0f};
					_up    = { .0f, .0f, 1.f};
					break ;
				case face_dir::minus_y:
					_front = { 0.f, 1.f, .0f};
					_right = { 1.f, .0f, .0f};
					_up    = { .0f, .0f, 1.f};
					break ;
				case face_dir::plus_y:
					_front = { .0f, -1.f, .0f};
					_right = { -1.f, .0f, .0f};
					_up    = { .0f, .0f, 1.f};
					break ;
			}
			break ;
	}
	_pos += _front * (7.5f * one16th - .125f);
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Entity::setLifetime( double lifetime )
{
	_lifeTime = lifetime;
}

bool Entity::isAt( glm::ivec3 pos )
{
	(void)pos;
	return (false);
}
bool ItemFrameEntity::isAt( glm::ivec3 pos )
{
	glm::ivec3 mpos = glm::floor(_pos);
	return (pos == mpos);
}

void Entity::getBlasted( glm::vec3 pos, float blast_radius )
{
	float dist = glm::distance(pos, _pos);
	if (dist >= blast_radius) {
		return ;
	}
	_lifeTime = 10000; // soft kill
}

void FallingBlockEntity::getBlasted( glm::vec3 pos, float blast_radius )
{
	float dist = glm::distance(pos, _pos);
	if (dist >= blast_radius) {
		return ;
	}
	_dir += (_pos - pos) * blast_radius;
}
void TNTEntity::getBlasted( glm::vec3 pos, float blast_radius )
{
	float dist = glm::distance(pos, _pos);
	if (dist >= blast_radius) {
		return ;
	}
	_dir += (_pos - pos) * blast_radius;
}
void ArrowEntity::getBlasted( glm::vec3 pos, float blast_radius )
{
	float dist = glm::distance(pos, _pos);
	if (dist >= blast_radius) {
		return ;
	}
	_dir += (_pos - pos) * blast_radius;
}

int Entity::pistonedBy( glm::ivec3 pos, glm::ivec3 target )
{
	(void)pos;(void)target;
	return (false);
}
int MovingPistonEntity::pistonedBy( glm::ivec3 pos, glm::ivec3 target )
{
	std::cout << "pistonedBy " << _chunk->getStartX() + pos.x << ", " << _chunk->getStartY() + pos.y << ", " << pos.z << ((_retraction) ? " retraction" : " extension") << " [" << _tickStart << "] from " << _chunk->getStartX() + _pos.x << ", " << _chunk->getStartY() + _pos.y << ", " << _pos.z << " to " << _chunk->getStartX() + _endPos.x << ", " << _chunk->getStartY() + _endPos.y << ", " << _endPos.z << std::endl;
	if (pos == _source) { // force place block at posEnd
		_softKill = true;
		// _chunk->setBlockAt(_item.type, _endPos.x, _endPos.y, _endPos.z, true);
		int currentTick = DayCycle::Get()->getGameTicks();
		std::cout << "MovingPistonEntity forcefinish movement " << ((_retraction) ? "retraction" : "extension") << " [" << _tickStart << "] -> [" << currentTick << "] to " << _chunk->getStartX() + _endPos.x << ", " << _chunk->getStartY() + _endPos.y << ", " << _endPos.z << std::endl;
		if (currentTick == _tickStart && _retraction) {
			std::cout << "\ttrap card activated->redstone::piston::cancel_retraction" << std::endl;
			_chunk->setBlockAt(_item.type,  _endPos.x - _dir.x, _endPos.y - _dir.y, _endPos.z - _dir.z, false);
			return (redstone::piston::cancel_retraction);
		}
		if (!_piston_head) {
			_chunk->setBlockAt(_item.type, _endPos, ((_item.type & mask::blocks::type) == blocks::observer) || ((_item.type & mask::blocks::type) == blocks::redstone_block)); // might want to add bool arg to this function, set it to false in (void)FORCE FINISH RETRACTION and to true in FORCE FINISH RETRACTION
			// _chunk->setBlockAt(_item.type, _endPos, !_retraction);
			if (_retraction) { // TODO check if this still necessary
				_chunk->setBlockAt(blocks::air, _endPos.x - _dir.x, _endPos.y - _dir.y, _endPos.z - _dir.z, false);
			}
			if ((_item.type & mask::blocks::type) == blocks::piston || (_item.type & mask::blocks::type) == blocks::sticky_piston) {
				_chunk->updatePiston(_endPos, _item.type);
			}
		} else {
			// _chunk->setBlockAt(((_item.type & (REDSTONE::STICKY)) ? blocks::sticky_piston : blocks::piston) | (_item.type & (0x7 << offset::blocks::orientation)), _endPos, false);
			int front_value = _chunk->getBlockAt(_pos);
			std::cout << "BLOCK IN FRONT IS " << s_blocks[front_value & mask::blocks::type]->name << std::endl;
			// if ((front_value & mask::blocks::type) == blocks::moving_piston) {
			// 	_chunk->setBlockAt(blocks::air, _pos, true);
			// }
			_chunk->setBlockAt(_chunk->getBlockAt(_source) & (mask::all_bits - mask::redstone::piston::moving), _source, false);
		}
		return (true);
	} else if (target == glm::ivec3(_endPos)) {
		return (redstone::piston::force_retraction);
	}
	return (false);
}


/**
 * @brief update entity's position and push_back arr with new pos
 * @return true if entity despawns this frame
*/
bool Entity::update( std::vector<t_shaderInput>& arr, glm::vec3 camPos, double deltaTime )
{
	_lifeTime += deltaTime;
    if (_lifeTime > 300 || _pos.z < 0) {
        return (true);
    }

	// if item in block, push it out of block.
	// first try on the sides, then push it upwards
	// std::cout << "pos inside chunk " << _chunk_pos.x << ", " << _chunk_pos.y << ": " << _pos.x - _chunk_pos.x << ", " << _pos.y - _chunk_pos.y << ", " << _pos.z << std::endl;
	if (air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z)), false, false, false)) {
		_pos.z += 3 * deltaTime;
		_dir = {0, 0, 1};
	} else {
		if (_dir.x && !air_flower(_chunk->getBlockAt(glm::floor(_pos.x + _dir.x * deltaTime - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z)), false, false, false)) {
			_pos.x += _dir.x * deltaTime;
		} else {
			_dir.x = 0;
		}
		if (_dir.y && !air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y + _dir.y * deltaTime - _chunk_pos.y), glm::floor(_pos.z)), false, false, false)) {
			_pos.y += _dir.y * deltaTime;
		} else {
			_dir.y = 0;
		}
		if (!air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z + _dir.z * deltaTime)), false, false, false)) {
			_pos.z += _dir.z * deltaTime;
			_dir.z -= 0.1f;
		} else {
			_dir = {_dir.x * 0.9f, _dir.y * 0.9f, -0.1f};
		}
	}

	// TODO disable this once player died
	// time to pick up: 0.5 sec if dropped, 2 sec if thrown by a player
	// hitbox to pick: + 1 on sides and + 0.5 verticaly from player's hitbox (0.3, 1.8)
	if (_lifeTime > 0.5 + 1.5 * _thrown) {
		if (_pos.x >= camPos.x - 1.3f && _pos.x <= camPos.x + 1.3f && _pos.y >= camPos.y - 1.3f && _pos.y <= camPos.y + 1.3f
			&& _pos.z >= camPos.z - 0.5f && _pos.z <= camPos.z + 2.3 && _inventory->absorbItem(_item)) {
			return (true);
		}
	}

	// items not displayed if 16 blocks away from player (TODO Entity Distance in settings)
	if (_pos.x < camPos.x - settings::consts::chunk_size || _pos.x > camPos.x + settings::consts::chunk_size
		|| _pos.y < camPos.y - settings::consts::chunk_size || _pos.y > camPos.y + settings::consts::chunk_size
		|| _pos.z < camPos.z - settings::consts::chunk_size || _pos.z > camPos.z + settings::consts::chunk_size) {
		return (false);
	}

    float cosRot = glm::cos(_lifeTime);
    float sinRot = glm::sin(_lifeTime);

	glm::vec3 front = glm::normalize(glm::vec3(-sinRot, cosRot, .0f));
	glm::vec3 right = glm::normalize(glm::vec3(cosRot, sinRot, .0f));
	int itemLight = _chunk->computePosLight(_pos);

	if (s_blocks[_item.type]->item3D) {
		glm::vec3 pos = _pos - front * .125f - right * .125f + settings::consts::math::world_up * (cosRot + 1) * 0.25f;
		s_blocks[_item.type]->addMeshItem(arr, itemLight, pos, front, right, settings::consts::math::world_up, 0.25f);
	} else {
		glm::vec3 pos = _pos - front * .25f * one16th - right * .25f + settings::consts::math::world_up * (0.5f + (cosRot + 1) * 0.25f);
		EXTRUSION::drawItem3D(arr, _item.type, itemLight, pos, front, right, settings::consts::math::world_up, 0.5f);
	}
    return (false);
}

bool FallingBlockEntity::update( std::vector<t_shaderInput>& arr, glm::vec3 camPos, double deltaTime )
{
	(void)camPos;
	_lifeTime += deltaTime;
    if (_lifeTime > 300 || _pos.z < 0) {
        return (true);
    }

	// std::cout << "FALLING BLOCK UPDATE" << std::endl;
	_dir.z -= 0.1f;
	_pos.z += _dir.z * deltaTime;
	int type = (_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z)) & mask::blocks::type);
	if (type != blocks::air && type != blocks::water) {
		// std::cout << "youston, we touched ground" << std::endl;
		if (!s_blocks[type]->isSolidForFluid) { // 'drops' as entity, but is already entity, so update state is enough
	// TODO fix this;
			// unsigned seed = type * type + type;
			// _lifeTime = 0;
			// _falling_block = false;
			// _solid = false;
			// _dir = {Random::randomFloat(seed), Random::randomFloat(seed), 2};
			return (false);
		}
		_chunk->handleHit(false, _item.type, {glm::floor(_pos.x), glm::floor(_pos.y), glm::floor(_pos.z + 1)}, Modif::add);
		return (true);
	}
	glm::vec3 p0 = {_pos.x + 0, _pos.y + 0, _pos.z + 1};
	glm::vec3 p1 = {_pos.x + 1, _pos.y + 0, _pos.z + 1};
	glm::vec3 p2 = {_pos.x + 0, _pos.y + 0, _pos.z + 0};
	glm::vec3 p3 = {_pos.x + 1, _pos.y + 0, _pos.z + 0};

	glm::vec3 p4 = {_pos.x + 0, _pos.y + 1, _pos.z + 1};
	glm::vec3 p5 = {_pos.x + 1, _pos.y + 1, _pos.z + 1};
	glm::vec3 p6 = {_pos.x + 0, _pos.y + 1, _pos.z + 0};
	glm::vec3 p7 = {_pos.x + 1, _pos.y + 1, _pos.z + 0};

	int texture = (s_blocks[_item.type]->texX(face_dir::minus_x) << 4) + (s_blocks[_item.type]->texY(face_dir::minus_x) << 12);
	int faceLight = _chunk->computePosLight(_pos);
	int spec = texture + (3 << 19) + (faceLight << 24);

	utils::shader::addQuads(arr, {p4, p0, p6, p2}, spec, 16, 16, 0, 8);

	spec += (1 << 19);
	utils::shader::addQuads(arr, {p1, p5, p3, p7}, spec, 16, 16, 0, 8);

	spec -= (3 << 19);
	utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, 16, 16, 0, 8);

	spec += (1 << 19);
	utils::shader::addQuads(arr, {p5, p4, p7, p6}, spec, 16, 16, 0, 8);

	spec -= (2 << 19);
	utils::shader::addQuads(arr, {p4, p5, p0, p1}, spec, 16, 16, 0, 8);

	spec += (5 << 19);
	utils::shader::addQuads(arr, {p2, p3, p6, p7}, spec, 16, 16, 0, 8);
	return (false);
}

bool TNTEntity::update( std::vector<t_shaderInput>& arr, glm::vec3 camPos, double deltaTime )
{
	(void)camPos;
	_lifeTime += deltaTime;

	if (_lifeTime > 4) { // explosion
		std::cout << "BOOM" << std::endl;
		_chunk->explosion(_pos + glm::vec3(0.5f, 0.5f, 0.5f), 4);
		return (true);
	}
	int frame = _lifeTime * 16;
	if ((frame & 0xF) == 0xF) {
		int prev = (_lifeTime - deltaTime) * 16;
		if (prev < frame) {
			_chunk->addParticle(new Particle(_chunk, {_pos.x + 0.5f, _pos.y + 0.5f, _pos.z + 1}, PARTICLES::SMOKE, 0.2f, 0));
		}
	}

	if (_dir.x && !air_flower(_chunk->getBlockAt(glm::floor(_pos.x + _dir.x * deltaTime - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z)), false, false, false)) {
		_pos.x += _dir.x * deltaTime;
	} else {
		_dir.x = 0;
	}
	if (_dir.y && !air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y + _dir.y * deltaTime - _chunk_pos.y), glm::floor(_pos.z)), false, false, false)) {
		_pos.y += _dir.y * deltaTime;
	} else {
		_dir.y = 0;
	}
	if (!air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z + _dir.z * deltaTime)), false, false, false)) {
		_pos.z += _dir.z * deltaTime;
		_dir.z -= 0.1f;
	} else {
		_dir = {_dir.x * 0.9f, _dir.y * 0.9f, -0.1f};
	}

	glm::vec3 p0 = {_pos.x + 0, _pos.y + 0, _pos.z + 1};
	glm::vec3 p1 = {_pos.x + 1, _pos.y + 0, _pos.z + 1};
	glm::vec3 p2 = {_pos.x + 0, _pos.y + 0, _pos.z + 0};
	glm::vec3 p3 = {_pos.x + 1, _pos.y + 0, _pos.z + 0};

	glm::vec3 p4 = {_pos.x + 0, _pos.y + 1, _pos.z + 1};
	glm::vec3 p5 = {_pos.x + 1, _pos.y + 1, _pos.z + 1};
	glm::vec3 p6 = {_pos.x + 0, _pos.y + 1, _pos.z + 0};
	glm::vec3 p7 = {_pos.x + 1, _pos.y + 1, _pos.z + 0};

	int itemLight = _chunk->computePosLight(_pos);
	int saturation = (static_cast<int>(_lifeTime * 10) / 3) & 0x2;

	int spec = (s_blocks[_item.type]->texX(face_dir::minus_x) << 4) + ((s_blocks[_item.type]->texY(face_dir::minus_x) - saturation) << 12) + (3 << 19) + (itemLight << 24);
	utils::shader::addQuads(arr, {p4, p0, p6, p2}, spec, 16, 16, 0, 8);

	spec = (s_blocks[_item.type]->texX(face_dir::plus_x) << 4) + ((s_blocks[_item.type]->texY(face_dir::plus_x) - saturation) << 12) + (4 << 19) + (itemLight << 24);
	utils::shader::addQuads(arr, {p1, p5, p3, p7}, spec, 16, 16, 0, 8);

	spec = (s_blocks[_item.type]->texX(face_dir::minus_y) << 4) + ((s_blocks[_item.type]->texY(face_dir::minus_y) - saturation) << 12) + (1 << 19) + (itemLight << 24);
	utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, 16, 16, 0, 8);

	spec = (s_blocks[_item.type]->texX(face_dir::plus_y) << 4) + ((s_blocks[_item.type]->texY(face_dir::plus_y) - saturation) << 12) + (2 << 19) + (itemLight << 24);
	utils::shader::addQuads(arr, {p5, p4, p7, p6}, spec, 16, 16, 0, 8);

	spec = (s_blocks[_item.type]->texX(face_dir::plus_z) << 4) + ((s_blocks[_item.type]->texY(face_dir::plus_z) - saturation) << 12) + (0 << 19) + (itemLight << 24);
	utils::shader::addQuads(arr, {p4, p5, p0, p1}, spec, 16, 16, 0, 8);

	spec = (s_blocks[_item.type]->texX(face_dir::minus_z) << 4) + ((s_blocks[_item.type]->texY(face_dir::minus_z) - saturation) << 12) + (5 << 19) + (itemLight << 24);
	utils::shader::addQuads(arr, {p2, p3, p6, p7}, spec, 16, 16, 0, 8);
	return (false);
}

bool ArrowEntity::update( std::vector<t_shaderInput>& arr, glm::vec3 camPos, double deltaTime )
{
	(void)camPos;
	_lifeTime += deltaTime;
    if (_lifeTime > 60 || _pos.z < 0) {
        return (true);
    }

	if (air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z)), false, false, false)) {
		_stuck = true;
		// arrow explosion for fun
		// _chunk->explosion(_pos - _dir * 0.25f, 10);
		// return (true);
	} else if (_stuck) {
		_stuck = false;
		_dir = {glm::sign(_dir.x) * 0.01f, glm::sign(_dir.y) * 0.01f, -0.1f};
	} else {
		_pos += _dir * static_cast<float>(deltaTime);
		_dir.z -= 0.1f;
	}
		
	glm::vec3 dir = glm::normalize(_dir); // might want to do this once and save it

	// first horizontal plane
	glm::vec3 hnormal = glm::normalize(glm::cross(_dir, glm::vec3(0.0, 0.0, 1.0f)));
	glm::vec3 p1 = _pos - hnormal * (1.25f / 16);
	glm::vec3 p3 = _pos + hnormal * (1.25f / 16);
	glm::vec3 p0 = _pos - dir * 0.5f - hnormal * (1.25f / 16);
	glm::vec3 p2 = _pos - dir * 0.5f + hnormal * (1.25f / 16);

	int itemLight = _chunk->computePosLight(_pos - _dir * 0.25f);
    int spec = (s_blocks[blocks::arrow]->texX() << 4) + ((s_blocks[blocks::arrow]->texY() + 1) << 12) + (0 << 19) + (itemLight << 24);
    utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, 16, 5, 0, 8); // recto
    utils::shader::addQuads(arr, {p1, p0, p3, p2}, spec, 16, 5, 0, 8, true); // verso

	// then vertical plane
	glm::vec3 vnormal = glm::normalize(glm::cross(dir, p1 - _pos));
	p1 = _pos + vnormal * (1.25f / 16);
	p3 = _pos - vnormal * (1.25f / 16);
	p0 = _pos - dir * 0.5f + vnormal * (1.25f / 16);
	p2 = _pos - dir * 0.5f - vnormal * (1.25f / 16);

    utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, 16, 5, 0, 8); // recto
    utils::shader::addQuads(arr, {p1, p0, p3, p2}, spec, 16, 5, 0, 8, true); // verso

	// then arrow's butt
	p0 = _pos - dir * 0.5f * (15.0f / 16) + vnormal * (1.25f / 16) - hnormal * (1.25f / 16);
	p1 = _pos - dir * 0.5f * (15.0f / 16) + vnormal * (1.25f / 16) + hnormal * (1.25f / 16);
	p2 = _pos - dir * 0.5f * (15.0f / 16) - vnormal * (1.25f / 16) - hnormal * (1.25f / 16);
	p3 = _pos - dir * 0.5f * (15.0f / 16) - vnormal * (1.25f / 16) + hnormal * (1.25f / 16);

    utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec + (5 << 8), 5, 5, 0, 8); // recto
    utils::shader::addQuads(arr, {p1, p0, p3, p2}, spec + (5 << 8), 5, 5, 0, 8, true); // verso
	return (false);
}

bool MovingPistonEntity::update( std::vector<t_shaderInput>& arr, glm::vec3 camPos, double deltaTime )
{
	if (_softKill) {
		// _entities vector can be updated by pistons while looping through it from Chunk::updateEntities,
		// so we do this to ensure no segfault because Chunk::updateEntities is the only one calling _entities.erase()
		return (true);
	}

	(void)camPos;
	_lifeTime += deltaTime;
	int lifeLimit = (_retraction) ? 2 : 3;

	int currentTick = DayCycle::Get()->getGameTicks();

	int GTMul = DayCycle::Get()->getGameTimeMultiplier();
	float percent = (GTMul == -1) ? glm::min(1.0, _lifeTime / (settings::consts::tick * lifeLimit))
									: 1.0f * (currentTick - _tickStart) / lifeLimit;
	s_blocks[_item.type & mask::blocks::type]->addMesh(_chunk, arr, {_chunk->getStartX(), _chunk->getStartY()}, _pos + _dir * percent, _item.type);

    if (currentTick - _tickStart == lifeLimit) {
		// finish extension, turn back to block
		std::cout << "MovingPistonEntity finished movement " << ((_retraction) ? "retraction" : "extension") << " [" << _tickStart << "] -> [" << currentTick << "] from " << _chunk->getStartX() + _pos.x << ", " << _chunk->getStartY() + _pos.y << ", " << _pos.z << " to " << _chunk->getStartX() + _endPos.x << ", " << _chunk->getStartY() + _endPos.y << ", " << _endPos.z << std::endl;
		if (!_piston_head) {
			_chunk->setBlockAt(_item.type, _endPos.x, _endPos.y, _endPos.z, true);
			if (_retraction) {
				_chunk->setBlockAt(blocks::air, _endPos.x - _dir.x, _endPos.y - _dir.y, _endPos.z - _dir.z, false);
			}
			if ((_item.type & mask::blocks::type) == blocks::piston || (_item.type & mask::blocks::type) == blocks::sticky_piston) {
				_chunk->updatePiston(_endPos, _item.type);
			}
		} else {
			// _chunk->setBlockAt(((_item.type & (REDSTONE::STICKY)) ? blocks::sticky_piston : blocks::piston) | (_item.type & (0x7 << offset::blocks::orientation)), _endPos, false);
			int front_value = _chunk->getBlockAt(_pos.x, _pos.y, _pos.z);
			std::cout << "BLOCK IN FRONT IS " << s_blocks[front_value & mask::blocks::type]->name << std::endl;
			// if ((front_value & mask::blocks::type) == blocks::moving_piston) {
			// 	_chunk->setBlockAt(blocks::air, _pos.x, _pos.y, _pos.z, true);
			// }
			std::cout << "source is " << _chunk->getStartX() + _source.x << ", " << _chunk->getStartY() + _source.y << ", " << _source.z << std::endl;
			int piston_value = _chunk->getBlockAt(_source);
			_chunk->setBlockAt(piston_value & (mask::all_bits - mask::redstone::piston::moving), _source, false);
			_chunk->updatePiston(_source, piston_value);
		}
        return (true);
    }
	return (false);
}

// ************************************************************************** //
//                              Item Frame                                    //
// ************************************************************************** //

void ItemFrameEntity::setContent( int type )
{
	_item.type = type;
}

int ItemFrameEntity::getContent( void )
{
	return (_item.type);
}

/**
 * @brief tries to rotate entity in item frame, if entity is air, place give type in item frame
 */
bool ItemFrameEntity::rotate( int type )
{
	if (_item.type == blocks::air) {
		_item.type = type;
		return (type != blocks::air);
	}

	++_rotation;
	_rotation &= 0x7;
	_up = glm::normalize(_right + _up);
	_right = glm::normalize(glm::cross(_front, _up));
	return (false);
}

int ItemFrameEntity::getRotation( void )
{
	// std::cout << "item getRotation, type is " << s_blocks[_item.type]->name << ", rotation is " << _rotation << std::endl;
	if (_item.type == blocks::air) {
		return (0);
	}
	return (_rotation + 1);
}

bool ItemFrameEntity::update( std::vector<t_shaderInput>& arr, glm::vec3 camPos, double deltaTime )
{
	(void)deltaTime;
	if (_item.type == blocks::air) {
		return (false);
	}

	// item not displayed if 16 blocks away from player (TODO Entity Distance in settings)
	if (_pos.x < camPos.x - settings::consts::chunk_size || _pos.x > camPos.x + settings::consts::chunk_size
		|| _pos.y < camPos.y - settings::consts::chunk_size || _pos.y > camPos.y + settings::consts::chunk_size
		|| _pos.z < camPos.z - settings::consts::chunk_size || _pos.z > camPos.z + settings::consts::chunk_size) {
		return (false);
	}

	int itemLight = _chunk->computePosLight(_pos);

	if (s_blocks[_item.type]->item3D) {
		glm::vec3 pos = _pos - (_right + _up) * .125f;
		s_blocks[_item.type]->addMeshItem(arr, itemLight, pos, _front, _right, _up, 0.25f);
	} else {
		glm::vec3 pos = _pos - (_right - _up) * .25f + _front * (0.125f - one16th);
		EXTRUSION::drawItem3D(arr, _item.type, itemLight, pos, _front, _right, _up, 0.5f);
	}
    return (false);
}
