#include "OpenGL_Manager.hpp"

AMob::AMob( glm::vec3 position )
    : _type(settings::consts::mob::unset), _deathTime(0.0f), _hurtTime(0.0f), _fallDistance(0.0f), _fallTime(0.0f), _z0(position.z), _walkTime(0.0f), _health(20.0f),
	_currentBlock(glm::floor(position)), _position(position),
    _front(0, -1, 0), _right(glm::normalize(glm::cross(_front, settings::consts::math::world_up))), _up(0.0f, 0.0f, 1.0f), _knockback(0.0f), _bodyFront(0.0f, -1.0f),
    _invulnerable(false), _touchGround(true), _walking(false), _inJump(false), _noAI(false), _persistenceRequired(false), _chunk(NULL)
{

}

AMob::~AMob( void )
{

}

// ************************************************************************** //
//                                Protected                                   //
// ************************************************************************** //

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

glm::vec3 AMob::getPos( void )
{
	return (_position);
}

glm::vec3 AMob::getDir( void )
{
	return (_front);
}

short AMob::getType( void )
{
	return (_type);
}

float AMob::getFovOffset( void )
{
	return (0.0f);
}

int AMob::getHealth( void )
{
	return (_health);
}

bool AMob::getTouchGround( void )
{
	return (_touchGround);
}

void AMob::setTouchGround( bool state )
{
	_touchGround = state;
}

void AMob::receiveDamage( const float damage, const glm::vec3 sourceDir )
{
	if (_health < 0) { // stop! it's already dead
		return ;
	} else if (_invulnerable) {
		return ;
	}

	if (_type == settings::consts::mob::player) {
		static_cast<Player*>(this)->setUIUpdate(true);
	}
	_health -= damage;
	_hurtTime = -0.5f;
	_invulnerable = true;
	if (_health < 0.0f) {
		_deathTime = -2.0f;
		_health = 0.0f;
	}
	_knockback = sourceDir;
	_knockback.z = glm::max(0.4f, _knockback.z) * 10.0f;
	_inJump = true;
}

// ************************************************************************** //
//                                Extern                                      //
// ************************************************************************** //

/**
 * @brief loop through AMob part of chunk and return ptr to mob being hit or NULL
 */
AMob* Chunk::mobHit( const t_hit blockHit )
{
	const glm::vec3 segStart = _player->getEyePos(), segEnd = segStart + _player->getDir() * settings::consts::reach::attack;

	for (auto& mob : _mobs) {
		glm::vec3 hitBoxCenter = mob->getPos(), hitBoxHalfSize = {0.3f, 0.3f, mob->getHitbox()};
		hitBoxCenter.z += hitBoxHalfSize.z;
		if (segment_cube_intersection(segStart, segEnd, hitBoxCenter, hitBoxHalfSize)) {
			if (blockHit.value & mask::blocks::type) {
				// compute which of block hit and mob is hit first
				const glm::ivec3 mobFeet = glm::floor(hitBoxCenter);
				const glm::ivec3 mobWaist = mobFeet + glm::ivec3(0, 0, 1);
				const glm::ivec3 mobHead = mobFeet + glm::ivec3(0, 0, 2);
				std::vector<glm::ivec3> ids = _player->computeRayCasting(settings::consts::reach::attack);
				for (auto block : ids) {
					if (block == blockHit.pos) {
						break ;
					} else if (block == mobFeet || block == mobWaist || block == mobHead) {
						return (mob.get());
					}
				}
			} else {
				return (mob.get());
			}
		}
	}
	Chunk* chunkEnd = getChunkAt(chunk_pos(segEnd.x), chunk_pos(segEnd.y));
	// std::cout << "chunkEnd " << chunkEnd << " vs this " << this << std::endl;
	if (chunkEnd && chunkEnd != this) {
		return (chunkEnd->mobHit(blockHit));
	}
	return (NULL);
}
