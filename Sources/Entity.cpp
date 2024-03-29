#include "Chunk.hpp"
#include "random.hpp"

Entity::Entity( Chunk *chunk, Inventory *inventory, glm::vec3 position, glm::vec3 dir, bool solid, bool thrown, t_item item )
    : _item(item), _solid(solid), _thrown(thrown), _stuck(false), _lifeTime(0),
		_pos(position), _dir(dir), _chunk(chunk), _inventory(inventory)
{
    // std::cout << "new Entity at " << position.x << ", " << position.y << ", " << position.z << ": " << s_blocks[value]->name << std::endl;
	_chunk_pos = {chunk->getStartX(), chunk->getStartY()};
	_falling_block = (solid && (item.type != blocks::ARROW));
}

Entity::~Entity( void )
{

}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

bool Entity::updateTNT( std::vector<std::pair<int, glm::vec3>> &arr, double deltaTime )
{
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

	int spec = s_blocks[_item.type]->texX(face_dir::MINUSX) + ((s_blocks[_item.type]->texY(face_dir::MINUSX) - saturation) << 4) + (3 << 19) + (itemLight << 24);
	std::pair<int, glm::vec3> v0 = {spec, p4};
	std::pair<int, glm::vec3> v1 = {spec + XTEX, p0};
	std::pair<int, glm::vec3> v2 = {spec + YTEX, p6};
	std::pair<int, glm::vec3> v3 = {spec + XTEX + YTEX, p2};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = s_blocks[_item.type]->texX(face_dir::PLUSX) + ((s_blocks[_item.type]->texY(face_dir::PLUSX) - saturation) << 4) + (4 << 19) + (itemLight << 24);
    v0 = {spec, p1};
    v1 = {spec + XTEX, p5};
    v2 = {spec + YTEX, p3};
    v3 = {spec + XTEX + YTEX, p7};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = s_blocks[_item.type]->texX(face_dir::MINUSY) + ((s_blocks[_item.type]->texY(face_dir::MINUSY) - saturation) << 4) + (1 << 19) + (itemLight << 24);
    v0 = {spec, p0};
    v1 = {spec + XTEX, p1};
    v2 = {spec + YTEX, p2};
    v3 = {spec + XTEX + YTEX, p3};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = s_blocks[_item.type]->texX(face_dir::PLUSY) + ((s_blocks[_item.type]->texY(face_dir::PLUSY) - saturation) << 4) + (2 << 19) + (itemLight << 24);
    v0 = {spec, p5};
    v1 = {spec + XTEX, p4};
    v2 = {spec + YTEX, p7};
    v3 = {spec + XTEX + YTEX, p6};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = s_blocks[_item.type]->texX(face_dir::PLUSZ) + ((s_blocks[_item.type]->texY(face_dir::PLUSZ) - saturation) << 4) + (0 << 19) + (itemLight << 24);
    v0 = {spec, p4};
    v1 = {spec + XTEX, p5};
    v2 = {spec + YTEX, p0};
    v3 = {spec + XTEX + YTEX, p1};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = s_blocks[_item.type]->texX(face_dir::MINUSZ) + ((s_blocks[_item.type]->texY(face_dir::MINUSZ) - saturation) << 4) + (5 << 19) + (itemLight << 24);
    v0 = {spec, p2};
    v1 = {spec + XTEX, p3};
    v2 = {spec + YTEX, p6};
    v3 = {spec + XTEX + YTEX, p7};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	return (false);
}


bool Entity::updateFallingBlock( std::vector<std::pair<int, glm::vec3>> &arr, double deltaTime )
{
	// std::cout << "FALLING BLOCK UPDATE" << std::endl;
	_dir.z -= 0.1f;
	_pos.z += _dir.z * deltaTime;
	int type = (_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z), true) & 0xFF);
	if (type != blocks::AIR && type < blocks::WATER) {
		// std::cout << "youston, we touched ground" << std::endl;
		if (type >= blocks::POPPY) { // 'drops' as entity, but is already entity, so update state is enough
			unsigned seed = type * type + type;
			_lifeTime = 0;
			_falling_block = false;
			_solid = false;
			_dir = {Random::randomFloat(seed), Random::randomFloat(seed), 2};
			return (false);
		}
		_chunk->handleHit(false, _item.type, {glm::floor(_pos.x), glm::floor(_pos.y), glm::floor(_pos.z + 1)}, Modif::ADD);
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

	int texture = s_blocks[_item.type]->texX(face_dir::MINUSX) + (s_blocks[_item.type]->texY(face_dir::MINUSX) << 4);
	int faceLight = _chunk->computePosLight(_pos);
	int spec = texture + (3 << 19);
	spec += (faceLight << 24);
	std::pair<int, glm::vec3> v0 = {spec, p4};
	std::pair<int, glm::vec3> v1 = {spec + XTEX, p0};
	std::pair<int, glm::vec3> v2 = {spec + YTEX, p6};
	std::pair<int, glm::vec3> v3 = {spec + XTEX + YTEX, p2};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = texture + (4 << 19);
	spec += (faceLight << 24);
	v0 = {spec, p1};
	v1 = {spec + XTEX, p5};
	v2 = {spec + YTEX, p3};
	v3 = {spec + XTEX + YTEX, p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = texture + (1 << 19);
	spec += (faceLight << 24);
	v0 = {spec, p0};
	v1 = {spec + XTEX, p1};
	v2 = {spec + YTEX, p2};
	v3 = {spec + XTEX + YTEX, p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = texture + (2 << 19);
	spec += (faceLight << 24);
	v0 = {spec, p5};
	v1 = {spec + XTEX, p4};
	v2 = {spec + YTEX, p7};
	v3 = {spec + XTEX + YTEX, p6};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = texture;
	spec += (faceLight << 24);
	v0 = {spec, p4};
	v1 = {spec + XTEX, p5};
	v2 = {spec + YTEX, p0};
	v3 = {spec + XTEX + YTEX, p1};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = texture + (5 << 19);
	spec += (faceLight << 24);
	v0 = {spec, p2};
	v1 = {spec + XTEX, p3};
	v2 = {spec + YTEX, p6};
	v3 = {spec + XTEX + YTEX, p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	return (false);
}

bool Entity::updateArrow( std::vector<std::pair<int, glm::vec3>> &arr, float deltaTime )
{
	if (_lifeTime > 60) {
		return (true);
	}

	if (air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z), true), false, false, false)) {
		_stuck = true;
		// arrow explosion for fun
		_chunk->explosion(_pos - _dir * 0.25f, 10);
		return (true);
	} else if (_stuck) {
		_stuck = false;
		_dir = {glm::sign(_dir.x) * 0.01f, glm::sign(_dir.y) * 0.01f, -0.1f};
	} else {
		_pos += _dir * deltaTime;
		_dir.z -= 0.1f;
	}
		
	glm::vec3 dir = glm::normalize(_dir); // might want to do this once and save it

	// first horizontal plane
	glm::vec3 hnormal = glm::normalize(glm::cross(_dir, glm::vec3(0.0, 0.0, 1.0f)));
	glm::vec3 p1 = _pos - hnormal * (1.25f / 16);
	glm::vec3 p3 = _pos + hnormal * (1.25f / 16);
	glm::vec3 p0 = _pos - dir * 0.5f - hnormal * (1.25f / 16);
	glm::vec3 p2 =  _pos - dir * 0.5f + hnormal * (1.25f / 16);

	int itemLight = _chunk->computePosLight(_pos - _dir * 0.25f);
    int spec = s_blocks[_item.type]->texX() + ((s_blocks[_item.type]->texY() + 1) << 4) + (0 << 19) + (itemLight << 24);
    std::pair<int, glm::vec3> v0 = {spec, p0};
    std::pair<int, glm::vec3> v1 = {spec + XTEX, p1};
    std::pair<int, glm::vec3> v2 = {spec + (1 << 18) + (5 << 8), p2};
    std::pair<int, glm::vec3> v3 = {spec + XTEX + (1 << 18) + (5 << 8), p3};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	// then vertical plane
	glm::vec3 vnormal = glm::normalize(glm::cross(dir, p1 - _pos));
	p1 = _pos + vnormal * (1.25f / 16);
	p3 = _pos - vnormal * (1.25f / 16);
	p0 = _pos - dir * 0.5f + vnormal * (1.25f / 16);
	p2 = _pos - dir * 0.5f - vnormal * (1.25f / 16);

	v0 = {spec, p0};
    v1 = {spec + XTEX, p1};
    v2 = {spec + (1 << 18) + (5 << 8), p2};
    v3 = {spec + XTEX + (1 << 18) + (5 << 8), p3};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	// then arrow's butt
	p0 = _pos - dir * 0.5f * (15.0f / 16) + vnormal * (1.25f / 16) - hnormal * (1.25f / 16);
	p1 = _pos - dir * 0.5f * (15.0f / 16) + vnormal * (1.25f / 16) + hnormal * (6.75f / 16);
	p2 = _pos - dir * 0.5f * (15.0f / 16) - vnormal * (1.25f / 16) - hnormal * (1.25f / 16);
	p3 = _pos - dir * 0.5f * (15.0f / 16) - vnormal * (1.25f / 16) + hnormal * (6.75f / 16);

	v0 = {spec + (5 << 8), p0};
    v1 = {spec + XTEX + (5 << 8), p1};
    v2 = {spec + (1 << 18) + (10 << 8), p2};
    v3 = {spec + XTEX + (1 << 18) + (10 << 8), p3};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	return (false);
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Entity::setLifetime( double lifetime )
{
	_lifeTime = lifetime;
}

void Entity::getBlasted( glm::vec3 pos, float blast_radius )
{
	float dist = glm::distance(pos, _pos);
	if (dist >= blast_radius) {
		return ;
	}
	if (_falling_block || _solid) {
		_dir += (_pos - pos) * blast_radius;
	} else {
		_lifeTime = 10000; // soft kill
	}
}

// update entity's position and push_back arr with new pos
// returns true if entity despawns this frame
bool Entity::update( std::vector<std::pair<int, glm::vec3>> &arr,  std::vector<std::pair<int, glm::vec3>> &partArr, glm::vec3 camPos, double deltaTime )
{
	_lifeTime += deltaTime;
    if (_lifeTime > 300 || _pos.z < 0) {
        return (true);
    }

	if (_falling_block) { // sand and gravel
		if (_item.type == blocks::TNT) {
			return (updateTNT(arr, deltaTime));
		}
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
			&& _pos.z >= camPos.z - 0.5f && _pos.z <= camPos.z + 2.3 && _inventory->absorbItem(_item)) {
			return (true);
		}
	}

	// items not displayed if 16 blocks away from player (TODO Entity Distance in settings)
	if (_pos.x < camPos.x - CHUNK_SIZE || _pos.x > camPos.x + CHUNK_SIZE || _pos.y < camPos.y - CHUNK_SIZE || _pos.y > camPos.y + CHUNK_SIZE
		|| _pos.z < camPos.z - CHUNK_SIZE || _pos.z > camPos.z + CHUNK_SIZE) {
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

	if (_item.type < blocks::POPPY) {
		int offset = ((_item.type >= blocks::CRAFTING_TABLE && _item.type < blocks::BEDROCK) ? face_dir::MINUSX: 0);
	    int itemLight = _chunk->computePosLight(_pos);

	    int spec = s_blocks[_item.type]->texX(face_dir::MINUSX, offset) + (s_blocks[_item.type]->texY(face_dir::MINUSX, offset) << 4) + (3 << 19) + (itemLight << 24);
	    std::pair<int, glm::vec3> v0 = {spec, p4};
	    std::pair<int, glm::vec3> v1 = {spec + XTEX, p0};
	    std::pair<int, glm::vec3> v2 = {spec + YTEX, p6};
	    std::pair<int, glm::vec3> v3 = {spec + XTEX + YTEX, p2};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

		spec = s_blocks[_item.type]->texX(face_dir::PLUSX, offset) + (s_blocks[_item.type]->texY(face_dir::PLUSX, offset) << 4) + (4 << 19) + (itemLight << 24);
	    v0 = {spec, p1};
	    v1 = {spec + XTEX, p5};
	    v2 = {spec + YTEX, p3};
	    v3 = {spec + XTEX + YTEX, p7};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

		spec = s_blocks[_item.type]->texX(face_dir::MINUSY, offset) + (s_blocks[_item.type]->texY(face_dir::MINUSY, offset) << 4) + (1 << 19) + (itemLight << 24);
	    v0 = {spec, p0};
	    v1 = {spec + XTEX, p1};
	    v2 = {spec + YTEX, p2};
	    v3 = {spec + XTEX + YTEX, p3};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

		spec = s_blocks[_item.type]->texX(face_dir::PLUSY, offset) + (s_blocks[_item.type]->texY(face_dir::PLUSY, offset) << 4) + (2 << 19) + (itemLight << 24);
	    v0 = {spec, p5};
	    v1 = {spec + XTEX, p4};
	    v2 = {spec + YTEX, p7};
	    v3 = {spec + XTEX + YTEX, p6};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

		spec = s_blocks[_item.type]->texX(face_dir::PLUSZ, offset) + (s_blocks[_item.type]->texY(face_dir::PLUSZ, offset) << 4) + (0 << 19) + (itemLight << 24);
	    v0 = {spec, p4};
	    v1 = {spec + XTEX, p5};
	    v2 = {spec + YTEX, p0};
	    v3 = {spec + XTEX + YTEX, p1};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

		spec = s_blocks[_item.type]->texX(face_dir::MINUSZ, offset) + (s_blocks[_item.type]->texY(face_dir::MINUSZ, offset) << 4) + (5 << 19) + (itemLight << 24);
	    v0 = {spec, p2};
	    v1 = {spec + XTEX, p3};
	    v2 = {spec + YTEX, p6};
	    v3 = {spec + XTEX + YTEX, p7};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	} else { // flowers
	    int itemLight = _chunk->computePosLight(_pos);
		if (1 && EXTRUSION::drawItem3D(partArr, _item.type, itemLight, p0 + glm::vec3(0, 0, 0.25f), glm::normalize(glm::vec3(-0.176777f * sinRot, 0.176777f * cosRot, 0)), glm::normalize(glm::vec3(0.176777f * cosRot, 0.176777f * sinRot, 0)), {0, 0, 1}, 0.5f)) { // TODO replace 1 by var toggle fancy_item

		} else {
			int spec = s_blocks[_item.type]->texX() + (s_blocks[_item.type]->texY() << 4) + (0 << 19) + (itemLight << 24);
			std::pair<int, glm::vec3> v0 = {spec, p0};
			std::pair<int, glm::vec3> v1 = {spec + XTEX, p5};
			std::pair<int, glm::vec3> v2 = {spec + YTEX, p2};
			std::pair<int, glm::vec3> v3 = {spec + XTEX + YTEX, p7};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

			v0 = {spec, p1};
			v1 = {spec + XTEX, p4};
			v2 = {spec + YTEX, p3};
			v3 = {spec + XTEX + YTEX, p6};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
		}
	}
    return (false);
}
