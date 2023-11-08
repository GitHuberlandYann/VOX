#include "vox.h"

Entity::Entity( Chunk *chunk, Inventory *inventory, glm::vec3 position, glm::vec2 dir, bool thrown, int value, int amount, int dura )
    : _value(value), _amount(amount), _dura(dura), _thrown(thrown), _lifeTime(0),
		_pos(position), _dir(dir, 1.0f), _chunk(chunk), _inventory(inventory)
{
    // std::cout << "new Entity at " << position.x << ", " << position.y << ", " << position.z << ": " << s_blocks[value].name << std::endl;
	_chunk_pos = {chunk->getStartX(), chunk->getStartY()};
}

Entity::~Entity( void )
{

}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

// update entity's position and push_back arr with new pos
// returns true if entity dispawns this frame
bool Entity::update( std::vector<std::pair<int, glm::vec3>> &arr, glm::vec3 camPos, double deltaTime )
{
	_lifeTime += deltaTime;
    if (_lifeTime > 300 || _pos.z < 0) {
        return (true);
    }

	// if item in block, push it out of block.
	// first try on the sides, then push it upwards
	// std::cout << "pos inside chunk " << _chunk_pos.x << ", " << _chunk_pos.y << ": " << _pos.x - _chunk_pos.x << ", " << _pos.y - _chunk_pos.y << ", " << _pos.z << std::endl;
	if (air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z), true), false, true)) {
		_pos.z += 3 * deltaTime;
		_dir = {0, 0, 1};
	} else {
		if (_dir.x && !air_flower(_chunk->getBlockAt(glm::floor(_pos.x + _dir.x * deltaTime - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z), true), false, true)) {
			_pos.x += _dir.x * deltaTime;
		} else {
			_dir.x = 0;
		}
		if (_dir.y && !air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y + _dir.y * deltaTime - _chunk_pos.y), glm::floor(_pos.z), true), false, true)) {
			_pos.y += _dir.y * deltaTime;
		} else {
			_dir.y = 0;
		}
		if (!air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z + _dir.z * deltaTime), true), false, true)) {
			_pos.z += _dir.z * deltaTime;
			_dir.z -= 0.1f;
		} else {
			_dir = {_dir.x * 0.9f, _dir.y * 0.9f, -0.1f};
		}
	}


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
	    bool orientedItem = 2 * (_value >= blocks::CRAFTING_TABLE && _value < blocks::BEDROCK);
	    int itemLight = _chunk->computePosLight(_pos);

	    int spec = blockGridX(_value, 0) + (blockGridY(_value) << 4) + (3 << 19) + (itemLight << 24);
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
	    spec = blockGridX(_value, 0) + (blockGridY(_value) << 4) + (4 << 19) + (itemLight << 24);
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
	    spec = blockGridX(_value, orientedItem) + (blockGridY(_value) << 4) + (1 << 19) + (itemLight << 24);
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
	    spec = blockGridX(_value, 0) + (blockGridY(_value) << 4) + (2 << 19) + (itemLight << 24);
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
	    spec = blockGridX(_value, 1) + (blockGridY(_value) << 4) + (0 << 19) + (itemLight << 24);
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
	    spec = blockGridX(_value, 1 + (_value == blocks::GRASS_BLOCK)) + (blockGridY(_value) << 4) + (5 << 19) + (itemLight << 24);
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
	/*} else if (_value == blocks::TORCH) {
	    // TODO for now torches only in default middle-of-block configuration
	    int spec = blockGridX(_value, 0) + (blockGridY(_value) << 4) + (0 << 19) + (15 << 24);
	    p4 = {_pos.x + 7.0f / 16 - 0.17f * sinRot, _pos.y + 0.17f * cosRot, _pos.z + 0.125f + cosRot / 4};
	    std::pair<int, glm::vec3> v0 = {spec, p4};
	    p0 = {_pos.x + 7.0f / 16 - 0.17f * cosRot, _pos.y - 0.17f * sinRot, _pos.z + 0.125f + cosRot / 4};
	    std::pair<int, glm::vec3> v1 = {spec + 1 + (1 << 8), p0};
	    p6 = {_pos.x + 7.0f / 16 - 0.17f * sinRot, _pos.y + 0.17f * cosRot, _pos.z - 0.125f + cosRot / 4};
	    std::pair<int, glm::vec3> v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p6};
	    p2 = {_pos.x + 7.0f / 16 - 0.17f * cosRot, _pos.y - 0.17f * sinRot, _pos.z - 0.125f + cosRot / 4};
	    std::pair<int, glm::vec3> v3 = {spec + 1 + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p2};
	    arr.push_back(v0);
	    arr.push_back(v1);
	    arr.push_back(v2);
	    arr.push_back(v1);
	    arr.push_back(v3);
	    arr.push_back(v2);
	    p0 = {_pos.x + 9.0f / 16 - 0.17f * cosRot, _pos.y - 0.17f * sinRot, _pos.z + 0.125f + cosRot / 4};
	    v0 = {spec, p0};
	    p4 = {_pos.x + 9.0f / 16 - 0.17f * sinRot, _pos.y + 0.17f * cosRot, _pos.z + 0.125f + cosRot / 4};
	    v1 = {spec + 1 + (1 << 8), {p4.x + 9.0 / 16, p4.y, p4.z}};
	    p2 = {_pos.x + 9.0f / 16 - 0.17f * cosRot, _pos.y - 0.17f * sinRot, _pos.z - 0.125f + cosRot / 4};
	    v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p2};
	    p6 = {_pos.x + 9.0f / 16 - 0.17f * sinRot, _pos.y + 0.17f * cosRot, _pos.z - 0.125f + cosRot / 4};
	    v3 = {spec + 1 + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), {p6.x + 9.0 / 16, p6.y, p6.z}};
	    arr.push_back(v0);
	    arr.push_back(v1);
	    arr.push_back(v2);
	    arr.push_back(v1);
	    arr.push_back(v3);
	    arr.push_back(v2);
	    p0 = {_pos.x - 0.17f * cosRot, _pos.y + 7.0f / 16 - 0.17f * sinRot, _pos.z + 0.125f + cosRot / 4};
	    v0 = {spec, p0};
	    p1 = {_pos.x + 0.17f * sinRot, _pos.y + 7.0f / 16 - 0.17f * cosRot, _pos.z + 0.125f + cosRot / 4};
	    v1 = {spec + 1 + (1 << 8), p1};
	    p2 = {_pos.x - 0.17f * cosRot, _pos.y + 7.0f / 16 - 0.17f * sinRot, _pos.z - 0.125f + cosRot / 4};
	    v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p2};
	    p3 = {_pos.x + 0.17f * sinRot, _pos.y + 7.0f / 16 - 0.17f * cosRot, _pos.z - 0.125f + cosRot / 4};
	    v3 = {spec + 1 + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p3};
	    arr.push_back(v0);
	    arr.push_back(v1);
	    arr.push_back(v2);
	    arr.push_back(v1);
	    arr.push_back(v3);
	    arr.push_back(v2);
	    p1 = {_pos.x + 0.17f * sinRot, _pos.y + 9.0f / 16 - 0.17f * cosRot, _pos.z + 0.125f + cosRot / 4};
	    v0 = {spec, p1};
	    p0 = {_pos.x - 0.17f * cosRot, _pos.y + 9.0f / 16 - 0.17f * sinRot, _pos.z + 0.125f + cosRot / 4};
	    v1 = {spec + 1 + (1 << 8), p0};
	    p3 = {_pos.x + 0.17f * sinRot, _pos.y + 9.0f / 16 - 0.17f * cosRot, _pos.z - 0.125f + cosRot / 4};
	    v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), p3};
	    p2 = {_pos.x - 0.17f * cosRot, _pos.y + 9.0f / 16 - 0.17f * sinRot, _pos.z - 0.125f + cosRot / 4};
	    v3 = {spec + 1 + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), p2};
	    arr.push_back(v0);
	    arr.push_back(v1);
	    arr.push_back(v2);
	    arr.push_back(v1);
	    arr.push_back(v3);
	    arr.push_back(v2);*/
	} else { // flowers
	    int itemLight = _chunk->computePosLight(_pos);
	    int spec = blockGridX(_value, 0) + (blockGridY(_value) << 4) + (0 << 19) + (itemLight << 24);
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
