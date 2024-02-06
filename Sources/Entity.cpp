#include "Chunk.hpp"

Entity::Entity( Chunk *chunk, Inventory *inventory, glm::vec3 position, glm::vec3 dir, bool solid, bool thrown, int value, int amount, int dura )
    : _value(value), _amount(amount), _dura(dura), _solid(solid), _thrown(thrown), _lifeTime(0),
		_pos(position), _dir(dir), _chunk(chunk), _inventory(inventory)
{
    // std::cout << "new Entity at " << position.x << ", " << position.y << ", " << position.z << ": " << s_blocks[value]->name << std::endl;
	_chunk_pos = {chunk->getStartX(), chunk->getStartY()};
	_falling_block = (solid && (value == blocks::SAND || value == blocks::GRAVEL));
}

Entity::~Entity( void )
{

}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

bool Entity::updateFallingBlock( std::vector<std::pair<int, glm::vec3>> &arr, double deltaTime )
{
	// std::cout << "FALLING BLOCK UPDATE" << std::endl;
	_dir.z -= 0.1f;
	_pos.z += _dir.z * deltaTime;
	int type = (_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z), true) & 0xFF);
	if (type != blocks::AIR && type < blocks::WATER) {
		// std::cout << "youston, we touched ground" << std::endl;
		if (type >= blocks::POPPY) {
			// _chunk->addEntity(, ); // TODO add entity to chunk
		} else {
			_chunk->handleHit(false, _value, {glm::floor(_pos.x), glm::floor(_pos.y), glm::floor(_pos.z + 1)}, Modif::ADD);
		}
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

	int texture = s_blocks[_value]->texX(face_dir::MINUSX, 0) + (s_blocks[_value]->texY(face_dir::MINUSX, 0) << 4);
	int faceLight = _chunk->computeLight(_pos.x - _chunk_pos.x, _pos.y - _chunk_pos.y, _pos.z);
	int spec = texture + (3 << 19);
	spec += (faceLight << 24);
	std::pair<int, glm::vec3> v0 = {spec, p4};
	std::pair<int, glm::vec3> v1 = {spec + 1 + (1 << 9) + (1 << 8), p0};
	std::pair<int, glm::vec3> v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p6};
	std::pair<int, glm::vec3> v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p2};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = texture + (4 << 19);
	spec += (faceLight << 24);
	v0 = {spec, p1};
	v1 = {spec + 1 + (1 << 9) + (1 << 8), p5};
	v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p3};
	v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = texture + (1 << 19);
	spec += (faceLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 1 + (1 << 9) + (1 << 8), p1};
	v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p2};
	v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = texture + (2 << 19);
	spec += (faceLight << 24);
	v0 = {spec, p5};
	v1 = {spec + 1 + (1 << 9) + (1 << 8), p4};
	v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p7};
	v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p6};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = texture;
	spec += (faceLight << 24);
	v0 = {spec, p4};
	v1 = {spec + 1 + (1 << 9) + (1 << 8), p5};
	v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p0};
	v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p1};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = texture + (5 << 19);
	spec += (faceLight << 24);
	v0 = {spec, p2};
	v1 = {spec + 1 + (1 << 9) + (1 << 8), p3};
	v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p6};
	v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	return (false);
}

bool Entity::updateArrow( std::vector<std::pair<int, glm::vec3>> &arr, double deltaTime )
{
	if (_dir.x && !air_flower(_chunk->getBlockAt(glm::floor(_pos.x + _dir.x * deltaTime - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z), true), false, false, false)) {
		_pos.x += _dir.x * deltaTime;
		if (_dir.y && !air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y + _dir.y * deltaTime - _chunk_pos.y), glm::floor(_pos.z), true), false, false, false)) {
			_pos.y += _dir.y * deltaTime;
			if (!air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z + _dir.z * deltaTime), true), false, false, false)) {
				_pos.z += _dir.z * deltaTime;
				_dir.z -= 0.1f;
			} else {
				_pos.y -= _dir.y * deltaTime;
				_pos.x -= _dir.x * deltaTime;
			}
		} else {
			_pos.x -= _dir.x * deltaTime;
		}
	}
		
	glm::vec3 dir = glm::normalize(_dir); // might want to do this once and save it

	// first horizontal plane
	glm::vec3 normal = glm::normalize(glm::cross(_dir, glm::vec3(0.0, 0.0, 1.0f)));
	glm::vec3 p1 = _pos - normal * (1.25f / 16);
	glm::vec3 p3 = _pos + normal * (6.75f / 16);
	glm::vec3 p0 = _pos - dir * 0.5f - normal * (1.25f / 16);
	glm::vec3 p2 =  _pos - dir * 0.5f + normal * (6.75f / 16);

	int itemLight = _chunk->computePosLight(_pos);
    int spec = s_blocks[_value]->texX() - 1 + (s_blocks[_value]->texY() << 4) + (0 << 19) + (itemLight << 24);
    std::pair<int, glm::vec3> v0 = {spec, p0};
    std::pair<int, glm::vec3> v1 = {spec + 1 + (1 << 9) + (1 << 8), p1};
    std::pair<int, glm::vec3> v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p2};
    std::pair<int, glm::vec3> v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p3};
    arr.push_back(v0);
    arr.push_back(v1);
    arr.push_back(v2);
    arr.push_back(v1);
    arr.push_back(v3);
    arr.push_back(v2);

	// then vertical plane
	normal = glm::normalize(glm::cross(dir, p1 - _pos));
	p1 = _pos + normal * (1.25f / 16);
	p3 = _pos - normal * (6.75f / 16);
	p0 = _pos - dir * 0.5f + normal * (1.25f / 16);
	p2 = _pos - dir * 0.5f - normal * (6.75f / 16);

	v0 = {spec, p0};
    v1 = {spec + 1 + (1 << 9) + (1 << 8), p1};
    v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p2};
    v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p3};
    arr.push_back(v0);
    arr.push_back(v1);
    arr.push_back(v2);
    arr.push_back(v1);
    arr.push_back(v3);
    arr.push_back(v2);
	return (false);
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

// update entity's position and push_back arr with new pos
// returns true if entity despawns this frame
bool Entity::update( std::vector<std::pair<int, glm::vec3>> &arr, glm::vec3 camPos, double deltaTime )
{
	_lifeTime += deltaTime;
    if (_lifeTime > 300 || _pos.z < 0) {
        return (true);
    }

	if (_falling_block) {
		return (updateFallingBlock(arr, deltaTime));
	}

	if (_solid) { // fow now only arrows
		return (updateArrow(arr, deltaTime));
	}

	// if item in block, push it out of block.
	// first try on the sides, then push it upwards
	// std::cout << "pos inside chunk " << _chunk_pos.x << ", " << _chunk_pos.y << ": " << _pos.x - _chunk_pos.x << ", " << _pos.y - _chunk_pos.y << ", " << _pos.z << std::endl;
	if (air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z), true), false, false, false)) {
		_pos.z += 3 * deltaTime;
		_dir = {0, 0, 1};
	} else {
		if (_dir.x && !air_flower(_chunk->getBlockAt(glm::floor(_pos.x + _dir.x * deltaTime - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z), true), false, false, false)) {
			_pos.x += _dir.x * deltaTime;
		} else {
			_dir.x = 0;
		}
		if (_dir.y && !air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y + _dir.y * deltaTime - _chunk_pos.y), glm::floor(_pos.z), true), false, false, false)) {
			_pos.y += _dir.y * deltaTime;
		} else {
			_dir.y = 0;
		}
		if (!air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z + _dir.z * deltaTime), true), false, false, false)) {
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
			&& _pos.z >= camPos.z - 0.5f && _pos.z <= camPos.z + 2.3 && _inventory->absorbItem({_value, _amount}, _dura)) {
			return (true);
		}
	}

	// items not displayed if 16 blocks away from player (Entity Distance in settings)
	if (_pos.x < camPos.x - 16 || _pos.x > camPos.x + 16 || _pos.y < camPos.y - 16 || _pos.y > camPos.y + 16
		|| _pos.z < camPos.z - 16 || _pos.z > camPos.z + 16) {
		return (false);
	}

    float cosRot = glm::cos(_lifeTime);
    float sinRot = glm::sin(_lifeTime);

	glm::vec3 p0 = {_pos.x - 0.176777f * cosRot, _pos.y - 0.176777f * sinRot, _pos.z + 0.25f + (cosRot + 1) / 4};
	glm::vec3 p1 = {_pos.x + 0.176777f * sinRot, _pos.y - 0.176777f * cosRot, _pos.z + 0.25f + (cosRot + 1) / 4};
	glm::vec3 p2 = {_pos.x - 0.176777f * cosRot, _pos.y - 0.176777f * sinRot, _pos.z + (cosRot + 1) / 4};
	glm::vec3 p3 = {_pos.x + 0.176777f * sinRot, _pos.y - 0.176777f * cosRot, _pos.z + (cosRot + 1) / 4};

	glm::vec3 p4 = {_pos.x - 0.176777f * sinRot, _pos.y + 0.176777f * cosRot, _pos.z + 0.25f + (cosRot + 1) / 4};
	glm::vec3 p5 = {_pos.x + 0.176777f * cosRot, _pos.y + 0.176777f * sinRot, _pos.z + 0.25f + (cosRot + 1) / 4};
	glm::vec3 p6 = {_pos.x - 0.176777f * sinRot, _pos.y + 0.176777f * cosRot, _pos.z + (cosRot + 1) / 4};
	glm::vec3 p7 = {_pos.x + 0.176777f * cosRot, _pos.y + 0.176777f * sinRot, _pos.z + (cosRot + 1) / 4};

	if (_value < blocks::POPPY) {
		int offset = ((_value >= blocks::CRAFTING_TABLE && _value < blocks::BEDROCK) ? face_dir::MINUSX + (1 << 4): 0);
	    int itemLight = _chunk->computePosLight(_pos);

	    int spec = s_blocks[_value]->texX(face_dir::MINUSX, offset) + (s_blocks[_value]->texY(face_dir::MINUSX, offset) << 4) + (3 << 19) + (itemLight << 24);
	    std::pair<int, glm::vec3> v0 = {spec, p4};
	    std::pair<int, glm::vec3> v1 = {spec + 1 + (1 << 9) + (1 << 8), p0};
	    std::pair<int, glm::vec3> v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p6};
	    std::pair<int, glm::vec3> v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p2};
	    arr.push_back(v0);
	    arr.push_back(v1);
	    arr.push_back(v2);
	    arr.push_back(v1);
	    arr.push_back(v3);
	    arr.push_back(v2);
	    spec = s_blocks[_value]->texX(face_dir::PLUSX, offset) + (s_blocks[_value]->texY(face_dir::PLUSX, offset) << 4) + (4 << 19) + (itemLight << 24);
	    v0 = {spec, p1};
	    v1 = {spec + 1 + (1 << 9) + (1 << 8), p5};
	    v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p3};
	    v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p7};
	    arr.push_back(v0);
	    arr.push_back(v1);
	    arr.push_back(v2);
	    arr.push_back(v1);
	    arr.push_back(v3);
	    arr.push_back(v2);
	    spec = s_blocks[_value]->texX(face_dir::MINUSY, offset) + (s_blocks[_value]->texY(face_dir::MINUSY, offset) << 4) + (1 << 19) + (itemLight << 24);
	    v0 = {spec, p0};
	    v1 = {spec + 1 + (1 << 9) + (1 << 8), p1};
	    v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p2};
	    v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p3};
	    arr.push_back(v0);
	    arr.push_back(v1);
	    arr.push_back(v2);
	    arr.push_back(v1);
	    arr.push_back(v3);
	    arr.push_back(v2);
	    spec = s_blocks[_value]->texX(face_dir::PLUSY, offset) + (s_blocks[_value]->texY(face_dir::PLUSY, offset) << 4) + (2 << 19) + (itemLight << 24);
	    v0 = {spec, p5};
	    v1 = {spec + 1 + (1 << 9) + (1 << 8), p4};
	    v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p7};
	    v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p6};
	    arr.push_back(v0);
	    arr.push_back(v1);
	    arr.push_back(v2);
	    arr.push_back(v1);
	    arr.push_back(v3);
	    arr.push_back(v2);
	    spec = s_blocks[_value]->texX(face_dir::PLUSZ, offset) + (s_blocks[_value]->texY(face_dir::PLUSZ, offset) << 4) + (0 << 19) + (itemLight << 24);
	    v0 = {spec, p4};
	    v1 = {spec + 1 + (1 << 9) + (1 << 8), p5};
	    v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p0};
	    v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p1};
	    arr.push_back(v0);
	    arr.push_back(v1);
	    arr.push_back(v2);
	    arr.push_back(v1);
	    arr.push_back(v3);
	    arr.push_back(v2);
	    spec = s_blocks[_value]->texX(face_dir::MINUSZ, offset) + (s_blocks[_value]->texY(face_dir::MINUSZ, offset) << 4) + (5 << 19) + (itemLight << 24);
	    v0 = {spec, p2};
	    v1 = {spec + 1 + (1 << 9) + (1 << 8), p3};
	    v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p6};
	    v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p7};
	    arr.push_back(v0);
	    arr.push_back(v1);
	    arr.push_back(v2);
	    arr.push_back(v1);
	    arr.push_back(v3);
	    arr.push_back(v2);
	} else { // flowers
	    int itemLight = _chunk->computePosLight(_pos);
	    int spec = s_blocks[_value]->texX() + (s_blocks[_value]->texY() << 4) + (0 << 19) + (itemLight << 24);
	    std::pair<int, glm::vec3> v0 = {spec, p0};
	    std::pair<int, glm::vec3> v1 = {spec + 1 + (1 << 9) + (1 << 8), p5};
	    std::pair<int, glm::vec3> v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p2};
	    std::pair<int, glm::vec3> v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p7};
	    arr.push_back(v0);
	    arr.push_back(v1);
	    arr.push_back(v2);
	    arr.push_back(v1);
	    arr.push_back(v3);
	    arr.push_back(v2);
	    v0 = {spec, p1};
	    v1 = {spec + 1 + (1 << 9) + (1 << 8), p4};
	    v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p3};
	    v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p6};
	    arr.push_back(v0);
	    arr.push_back(v1);
	    arr.push_back(v2);
	    arr.push_back(v1);
	    arr.push_back(v3);
	    arr.push_back(v2);
	}
    return (false);
}
