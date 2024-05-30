#include "Chunk.hpp"
#include "random.hpp"
#include "DayCycle.hpp"

Entity::Entity( Chunk *chunk, Inventory *inventory, glm::vec3 position, glm::vec3 dir, bool thrown, t_item item )
    : _thrown(thrown), _item(item), _lifeTime(0), _pos(position), _dir(dir), _chunk(chunk), _inventory(inventory)
{
    // std::cout << "new Entity at " << position.x << ", " << position.y << ", " << position.z << ": " << s_blocks[value]->name << std::endl;
	_chunk_pos = {chunk->getStartX(), chunk->getStartY()};
}

Entity::~Entity( void )
{

}

FallingBlockEntity::FallingBlockEntity( Chunk *chunk, glm::vec3 position, t_item item)
	: Entity(chunk, NULL, position, {0, 0, 0}, false, item)
{

}

TNTEntity::TNTEntity( Chunk *chunk, glm::vec3 position, glm::vec3 dir )
	: Entity(chunk, NULL, position, dir, false, {0})
{

}

ArrowEntity::ArrowEntity( Chunk *chunk, glm::vec3 position, glm::vec3 dir )
	: Entity(chunk, NULL, position, dir, false, {0}), _stuck(false)
{

}

MovingPistonEntity::MovingPistonEntity( Chunk *chunk, glm::ivec3 source, glm::ivec3 position, glm::ivec3 dir, bool piston_head, bool retraction, int value )
	: Entity(chunk, NULL, position, dir, false, {value, 1}), _piston_head(piston_head), _retraction(retraction), _softKill(false),
	_tickStart(DayCycle::Get()->getGameTicks()), _source(source), _endPos(position + dir)
{
	// std::cout << "MovingPistonEntity init movement " << ((_retraction) ? "retraction" : "extension") << " [" << _tickStart << "] from " << _chunk->getStartX() + _pos.x << ", " << _chunk->getStartY() + _pos.y << ", " << _pos.z << " to " << _chunk->getStartX() + _endPos.x << ", " << _chunk->getStartY() + _endPos.y << ", " << _endPos.z << std::endl;
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
			std::cout << "\ttrap card activated->REDSTONE::PISTON::CANCEL_RETRACTION" << std::endl;
			_chunk->setBlockAt(_item.type,  _endPos.x - _dir.x, _endPos.y - _dir.y, _endPos.z - _dir.z, false);
			return (REDSTONE::PISTON::CANCEL_RETRACTION);
		}
		if (!_piston_head) {
			_chunk->setBlockAt(_item.type, _endPos, ((_item.type & 0xFF) == blocks::OBSERVER)); // might want to add bool arg to this function, set it to false in (void)FORCE FINISH RETRACTION and to true in FORCE FINISH RETRACTION
			if (_retraction) { // TODO check if this still necessary
				_chunk->setBlockAt(blocks::AIR, _endPos.x - _dir.x, _endPos.y - _dir.y, _endPos.z - _dir.z, false);
			}
			if ((_item.type & 0xFF) == blocks::PISTON || (_item.type & 0xFF) == blocks::STICKY_PISTON) {
				_chunk->updatePiston(_endPos, _item.type);
			}
		} else {
			// _chunk->setBlockAt(((_item.type & (REDSTONE::STICKY)) ? blocks::STICKY_PISTON : blocks::PISTON) | (_item.type & (0x7 << 9)), _endPos, false);
			int front_value = _chunk->getBlockAt(_pos);
			std::cout << "BLOCK IN FRONT IS " << s_blocks[front_value & 0xFF]->name << std::endl;
			// if ((front_value & 0xFF) == blocks::MOVING_PISTON) {
			// 	_chunk->setBlockAt(blocks::AIR, _pos, true);
			// }
			_chunk->setBlockAt(_chunk->getBlockAt(_source) & (REDSTONE::ALL_BITS - REDSTONE::PISTON::MOVING), _source, false);
		}
		return (true);
	} else if (target == glm::ivec3(_endPos)) {
		return (REDSTONE::PISTON::FORCE_RETRACTION);
	}
	return (false);
}


/**
 * @brief update entity's position and push_back arr with new pos
 * @return true if entity despawns this frame
*/
bool Entity::update( std::vector<t_shaderInput> &arr,  std::vector<t_shaderInput> &partArr, glm::vec3 camPos, double deltaTime )
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
	if (_pos.x < camPos.x - CHUNK_SIZE || _pos.x > camPos.x + CHUNK_SIZE || _pos.y < camPos.y - CHUNK_SIZE || _pos.y > camPos.y + CHUNK_SIZE
		|| _pos.z < camPos.z - CHUNK_SIZE || _pos.z > camPos.z + CHUNK_SIZE) {
		return (false);
	}

    float cosRot = glm::cos(_lifeTime);
    float sinRot = glm::sin(_lifeTime);

	int shape = s_blocks[_item.type]->geometry;
	float zSize = (shape == GEOMETRY::SLAB_BOTTOM) ? 0.125f : (shape == GEOMETRY::TRAPDOOR) ? 0.046875f : 0.25f;

	glm::vec3 p0 = {_pos.x - 0.176777f * cosRot, _pos.y - 0.176777f * sinRot, _pos.z + zSize + (cosRot + 1) / 4};
	glm::vec3 p1 = {_pos.x + 0.176777f * sinRot, _pos.y - 0.176777f * cosRot, _pos.z + zSize + (cosRot + 1) / 4};
	glm::vec3 p2 = {_pos.x - 0.176777f * cosRot, _pos.y - 0.176777f * sinRot, _pos.z + (cosRot + 1) / 4};
	glm::vec3 p3 = {_pos.x + 0.176777f * sinRot, _pos.y - 0.176777f * cosRot, _pos.z + (cosRot + 1) / 4};

	glm::vec3 p4 = {_pos.x - 0.176777f * sinRot, _pos.y + 0.176777f * cosRot, _pos.z + zSize + (cosRot + 1) / 4};
	glm::vec3 p5 = {_pos.x + 0.176777f * cosRot, _pos.y + 0.176777f * sinRot, _pos.z + zSize + (cosRot + 1) / 4};
	glm::vec3 p6 = {_pos.x - 0.176777f * sinRot, _pos.y + 0.176777f * cosRot, _pos.z + (cosRot + 1) / 4};
	glm::vec3 p7 = {_pos.x + 0.176777f * cosRot, _pos.y + 0.176777f * sinRot, _pos.z + (cosRot + 1) / 4};

	if (shape == GEOMETRY::STAIRS_BOTTOM) {
	    int itemLight = _chunk->computePosLight(_pos);

	    int spec = (s_blocks[_item.type]->texX() << 4) + (s_blocks[_item.type]->texY() << 12) + (3 << 19) + (itemLight << 24);
	    t_shaderInput v0 = {spec + (8 << 8), p4 - glm::vec3(0, 0, 0.125f)};
	    t_shaderInput v1 = {spec + (8 << 8) + XTEX, p0 - glm::vec3(0, 0, 0.125f)};
	    t_shaderInput v2 = {spec + YTEX, p6};
	    t_shaderInput v3 = {spec + XTEX + YTEX, p2};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2); //-x

		spec += (1 << 19);
	    v0 = {spec, p1};
	    v1 = {spec + XTEX, p5};
	    v2 = {spec + YTEX, p3};
	    v3 = {spec + XTEX + YTEX, p7};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2); //+x

		spec -= (3 << 19);
	    v0 = {spec + 8, (p0 + p1) * 0.5f};
		v1 = {spec + XTEX, p1};
		v2 = {spec + 8 - (8 << 8) + YTEX, (p0 + p3) * 0.5f};
		v3 = {spec + XTEX - (8 << 8) + YTEX, (p1 + p3) * 0.5f};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2); //-y top right corner
	    v0 = {spec + (8 << 8), (p0 + p2) * 0.5f};
		v1 = {spec + XTEX + (8 << 8), (p1 + p3) * 0.5f};
		v2 = {spec + YTEX, p2};
		v3 = {spec + XTEX + YTEX, p3};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2); //-y bottom slice

		spec += (1 << 19);
	    v0 = {spec, p5};
	    v1 = {spec - 8 + XTEX, (p5 + p4) * 0.5f};
	    v2 = {spec - (8 << 8) + YTEX, (p5 + p7) * 0.5f};
	    v3 = {spec - 8 + XTEX - (8 << 8) + YTEX, (p5 + p6) * 0.5f};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2); //+y top left corner
	    v0 = {spec + (8 << 8), (p5 + p7) * 0.5f};
	    v1 = {spec + XTEX + (8 << 8), (p4 + p6) * 0.5f};
	    v2 = {spec + YTEX, p7};
	    v3 = {spec + XTEX + YTEX, p6};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2); //+y bottom slice

		// top of second step
		spec -= (2 << 19);
	    v0 = {spec + (8 << 8), p1};
		v1 = {spec + (8 << 8) + XTEX, p5};
		v2 = {spec + YTEX, (p0 + p1) * 0.5f};
		v3 = {spec + XTEX + YTEX, (p4 + p5) * 0.5f};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	    // top of first step
		v0 = {spec + (8 << 8), (p0 + p1) * 0.5f - glm::vec3(0, 0, 0.125f)};
		v1 = {spec + (8 << 8) + XTEX, (p4 + p5) * 0.5f - glm::vec3(0, 0, 0.125f)};
		v2 = {spec + YTEX, p0 + glm::vec3(0, 0, -0.125f)};
		v3 = {spec + XTEX + YTEX, p4 + glm::vec3(0, 0, -0.125f)};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	    // front of second step
		spec += (3 << 19);
		v0 = {spec + (8 << 8), (p0 + p1) * 0.5f};
		v1 = {spec + (8 << 8) + XTEX, (p4 + p5) * 0.5f};
		v2 = {spec + YTEX, (p0 + p1) * 0.5f - glm::vec3(0, 0, 0.125f)};
		v3 = {spec + XTEX + YTEX, (p4 + p5) * 0.5f - glm::vec3(0, 0, 0.125f)};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

		spec += (2 << 19);
	    v0 = {spec, p2};
	    v1 = {spec + XTEX, p3};
	    v2 = {spec + YTEX, p6};
	    v3 = {spec + XTEX + YTEX, p7};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	} else if (s_blocks[_item.type]->item3D) { //(_item.type < blocks::POPPY && _item.type != blocks::OAK_DOOR && _item.type != blocks::GLASS_PANE) {
		int offset = ((_item.type >= blocks::CRAFTING_TABLE && _item.type < blocks::BEDROCK) ? face_dir::MINUSX: 0);
	    int itemLight = _chunk->computePosLight(_pos);
		int slabOffset = (shape == GEOMETRY::SLAB_BOTTOM) ? (8 << 8) : (shape == GEOMETRY::TRAPDOOR) ? (13 << 8) : 0;

	    int spec = (s_blocks[_item.type]->texX(face_dir::MINUSX, offset) << 4) + (s_blocks[_item.type]->texY(face_dir::MINUSX, offset) << 12) + (3 << 19) + (itemLight << 24);
	    t_shaderInput v0 = {spec + slabOffset, p4};
	    t_shaderInput v1 = {spec + slabOffset + XTEX, p0};
	    t_shaderInput v2 = {spec + YTEX, p6};
	    t_shaderInput v3 = {spec + XTEX + YTEX, p2};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

		spec = (s_blocks[_item.type]->texX(face_dir::PLUSX, offset) << 4) + (s_blocks[_item.type]->texY(face_dir::PLUSX, offset) << 12) + (4 << 19) + (itemLight << 24);
	    v0 = {spec + slabOffset, p1};
	    v1 = {spec + slabOffset + XTEX, p5};
	    v2 = {spec + YTEX, p3};
	    v3 = {spec + XTEX + YTEX, p7};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

		spec = (s_blocks[_item.type]->texX(face_dir::MINUSY, offset) << 4) + (s_blocks[_item.type]->texY(face_dir::MINUSY, offset) << 12) + (1 << 19) + (itemLight << 24);
	    v0 = {spec + slabOffset, p0};
	    v1 = {spec + slabOffset + XTEX, p1};
	    v2 = {spec + YTEX, p2};
	    v3 = {spec + XTEX + YTEX, p3};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

		spec = (s_blocks[_item.type]->texX(face_dir::PLUSY, offset) << 4) + (s_blocks[_item.type]->texY(face_dir::PLUSY, offset) << 12) + (2 << 19) + (itemLight << 24);
	    v0 = {spec + slabOffset, p5};
	    v1 = {spec + slabOffset + XTEX, p4};
	    v2 = {spec + YTEX, p7};
	    v3 = {spec + XTEX + YTEX, p6};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

		spec = (s_blocks[_item.type]->texX(face_dir::PLUSZ, offset) << 4) + (s_blocks[_item.type]->texY(face_dir::PLUSZ, offset) << 12) + (0 << 19) + (itemLight << 24);
	    v0 = {spec, p4};
	    v1 = {spec + XTEX, p5};
	    v2 = {spec + YTEX, p0};
	    v3 = {spec + XTEX + YTEX, p1};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

		spec = (s_blocks[_item.type]->texX(face_dir::MINUSZ, offset) << 4) + (s_blocks[_item.type]->texY(face_dir::MINUSZ, offset) << 12) + (5 << 19) + (itemLight << 24);
	    v0 = {spec, p2};
	    v1 = {spec + XTEX, p3};
	    v2 = {spec + YTEX, p6};
	    v3 = {spec + XTEX + YTEX, p7};
	    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	} else { // flowers
	    int itemLight = _chunk->computePosLight(_pos);
		if (1 && EXTRUSION::drawItem3D(partArr, _item.type, itemLight, p0 + glm::vec3(0, 0, 0.25f), glm::normalize(glm::vec3(-0.176777f * sinRot, 0.176777f * cosRot, 0)), glm::normalize(glm::vec3(0.176777f * cosRot, 0.176777f * sinRot, 0)), {0, 0, 1}, 0.5f)) { // TODO replace 1 by var toggle fancy_item

		} else {
			int spec = (s_blocks[_item.type]->texX() << 4) + (s_blocks[_item.type]->texY() << 12) + (0 << 19) + (itemLight << 24);
			t_shaderInput v0 = {spec, p0};
			t_shaderInput v1 = {spec + XTEX, p5};
			t_shaderInput v2 = {spec + YTEX, p2};
			t_shaderInput v3 = {spec + XTEX + YTEX, p7};
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

bool FallingBlockEntity::update( std::vector<t_shaderInput> &arr,  std::vector<t_shaderInput> &partArr, glm::vec3 camPos, double deltaTime )
{
	(void)partArr;
	(void)camPos;
	_lifeTime += deltaTime;
    if (_lifeTime > 300 || _pos.z < 0) {
        return (true);
    }

	// std::cout << "FALLING BLOCK UPDATE" << std::endl;
	_dir.z -= 0.1f;
	_pos.z += _dir.z * deltaTime;
	int type = (_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z)) & 0xFF);
	if (type != blocks::AIR && type < blocks::WATER) {
		// std::cout << "youston, we touched ground" << std::endl;
		if (type >= blocks::POPPY) { // 'drops' as entity, but is already entity, so update state is enough
	// TODO fix this;
			// unsigned seed = type * type + type;
			// _lifeTime = 0;
			// _falling_block = false;
			// _solid = false;
			// _dir = {Random::randomFloat(seed), Random::randomFloat(seed), 2};
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

	int texture = (s_blocks[_item.type]->texX(face_dir::MINUSX) << 4) + (s_blocks[_item.type]->texY(face_dir::MINUSX) << 12);
	int faceLight = _chunk->computePosLight(_pos);
	int spec = texture + (3 << 19);
	spec += (faceLight << 24);
	t_shaderInput v0 = {spec, p4};
	t_shaderInput v1 = {spec + XTEX, p0};
	t_shaderInput v2 = {spec + YTEX, p6};
	t_shaderInput v3 = {spec + XTEX + YTEX, p2};
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

bool TNTEntity::update( std::vector<t_shaderInput> &arr,  std::vector<t_shaderInput> &partArr, glm::vec3 camPos, double deltaTime )
{
	(void)partArr;
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

	int spec = (s_blocks[_item.type]->texX(face_dir::MINUSX) << 4) + ((s_blocks[_item.type]->texY(face_dir::MINUSX) - saturation) << 12) + (3 << 19) + (itemLight << 24);
	t_shaderInput v0 = {spec, p4};
	t_shaderInput v1 = {spec + XTEX, p0};
	t_shaderInput v2 = {spec + YTEX, p6};
	t_shaderInput v3 = {spec + XTEX + YTEX, p2};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (s_blocks[_item.type]->texX(face_dir::PLUSX) << 4) + ((s_blocks[_item.type]->texY(face_dir::PLUSX) - saturation) << 12) + (4 << 19) + (itemLight << 24);
    v0 = {spec, p1};
    v1 = {spec + XTEX, p5};
    v2 = {spec + YTEX, p3};
    v3 = {spec + XTEX + YTEX, p7};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (s_blocks[_item.type]->texX(face_dir::MINUSY) << 4) + ((s_blocks[_item.type]->texY(face_dir::MINUSY) - saturation) << 12) + (1 << 19) + (itemLight << 24);
    v0 = {spec, p0};
    v1 = {spec + XTEX, p1};
    v2 = {spec + YTEX, p2};
    v3 = {spec + XTEX + YTEX, p3};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (s_blocks[_item.type]->texX(face_dir::PLUSY) << 4) + ((s_blocks[_item.type]->texY(face_dir::PLUSY) - saturation) << 12) + (2 << 19) + (itemLight << 24);
    v0 = {spec, p5};
    v1 = {spec + XTEX, p4};
    v2 = {spec + YTEX, p7};
    v3 = {spec + XTEX + YTEX, p6};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (s_blocks[_item.type]->texX(face_dir::PLUSZ) << 4) + ((s_blocks[_item.type]->texY(face_dir::PLUSZ) - saturation) << 12) + (0 << 19) + (itemLight << 24);
    v0 = {spec, p4};
    v1 = {spec + XTEX, p5};
    v2 = {spec + YTEX, p0};
    v3 = {spec + XTEX + YTEX, p1};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (s_blocks[_item.type]->texX(face_dir::MINUSZ) << 4) + ((s_blocks[_item.type]->texY(face_dir::MINUSZ) - saturation) << 12) + (5 << 19) + (itemLight << 24);
    v0 = {spec, p2};
    v1 = {spec + XTEX, p3};
    v2 = {spec + YTEX, p6};
    v3 = {spec + XTEX + YTEX, p7};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	return (false);
}

bool ArrowEntity::update( std::vector<t_shaderInput> &arr,  std::vector<t_shaderInput> &partArr, glm::vec3 camPos, double deltaTime )
{
	(void)partArr;
	(void)camPos;
	_lifeTime += deltaTime;
    if (_lifeTime > 60 || _pos.z < 0) {
        return (true);
    }

	if (air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z)), false, false, false)) {
		_stuck = true;
		// arrow explosion for fun
		_chunk->explosion(_pos - _dir * 0.25f, 10);
		return (true);
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
    int spec = (s_blocks[blocks::ARROW]->texX() << 4) + ((s_blocks[blocks::ARROW]->texY() + 1) << 12) + (0 << 19) + (itemLight << 24);
    t_shaderInput v0 = {spec, p0};
    t_shaderInput v1 = {spec + XTEX, p1};
    t_shaderInput v2 = {spec + (1 << 18) + (5 << 8), p2};
    t_shaderInput v3 = {spec + XTEX + (1 << 18) + (5 << 8), p3};
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

bool MovingPistonEntity::update( std::vector<t_shaderInput> &arr,  std::vector<t_shaderInput> &partArr, glm::vec3 camPos, double deltaTime )
{
	if (_softKill) {
		// _entities vector can be updated by pistons while looping through it from Chunk::updateEntities,
		// so we do this to ensure no segfault because Chunk::updateEntities is the only one calling _entities.erase()
		return (true);
	}

	(void)partArr;
	(void)camPos;
	_lifeTime += deltaTime;
	int lifeLimit = (_retraction) ? 2 : 3;

	int currentTick = DayCycle::Get()->getGameTicks();

	int GTMul = DayCycle::Get()->getGameTimeMultiplier();
	float percent = (GTMul == -1) ? glm::min(1.0, _lifeTime / (TICK * lifeLimit))
									: 1.0f * (currentTick - _tickStart) / lifeLimit;
	s_blocks[_item.type & 0xFF]->addMesh(_chunk, arr, {_chunk->getStartX(), _chunk->getStartY()}, _pos + _dir * percent, _item.type);

    if (currentTick - _tickStart == lifeLimit) {
		// finish extension, turn back to block
		std::cout << "MovingPistonEntity finished movement " << ((_retraction) ? "retraction" : "extension") << " [" << _tickStart << "] -> [" << currentTick << "] from " << _chunk->getStartX() + _pos.x << ", " << _chunk->getStartY() + _pos.y << ", " << _pos.z << " to " << _chunk->getStartX() + _endPos.x << ", " << _chunk->getStartY() + _endPos.y << ", " << _endPos.z << std::endl;
		if (!_piston_head) {
			_chunk->setBlockAt(_item.type, _endPos.x, _endPos.y, _endPos.z, true);
			if (_retraction) {
				_chunk->setBlockAt(blocks::AIR, _endPos.x - _dir.x, _endPos.y - _dir.y, _endPos.z - _dir.z, false);
			}
			if ((_item.type & 0xFF) == blocks::PISTON || (_item.type & 0xFF) == blocks::STICKY_PISTON) {
				_chunk->updatePiston(_endPos, _item.type);
			}
		} else {
			// _chunk->setBlockAt(((_item.type & (REDSTONE::STICKY)) ? blocks::STICKY_PISTON : blocks::PISTON) | (_item.type & (0x7 << 9)), _endPos, false);
			int front_value = _chunk->getBlockAt(_pos.x, _pos.y, _pos.z);
			std::cout << "BLOCK IN FRONT IS " << s_blocks[front_value & 0xFF]->name << std::endl;
			// if ((front_value & 0xFF) == blocks::MOVING_PISTON) {
			// 	_chunk->setBlockAt(blocks::AIR, _pos.x, _pos.y, _pos.z, true);
			// }
			std::cout << "source is " << _chunk->getStartX() + _source.x << ", " << _chunk->getStartY() + _source.y << ", " << _source.z << std::endl;
			int piston_value = _chunk->getBlockAt(_source);
			_chunk->setBlockAt(piston_value & (REDSTONE::ALL_BITS - REDSTONE::PISTON::MOVING), _source, false);
			_chunk->updatePiston(_source, piston_value);
		}
        return (true);
    }
	return (false);
}
