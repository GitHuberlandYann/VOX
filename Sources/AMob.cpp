#include "OpenGL_Manager.hpp"

AMob::AMob( glm::vec3 position )
    : _type(0), _deathTime(0.0f), _hurtTime(0.0f), _fallDistance(0.0f), _fallTime(0.0f), _z0(position.z), _walkTime(0.0f), _health(20.0f), _currentBlock(glm::floor(position)), _position(position),
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

/**
 * @brief push back rectangle make of 4 given pts to arr
 * @param arr vector containing vertices
 * @param pts 4 corners of rectangle, in clockwise order
 * @param spec specifications of top left corner
 * @param dx offset in x coord of texture for right pts
 * @param dy offset in y coord of texture for bottom pts
 */
void AMob::addQuads( std::vector<t_shaderInput>& arr, std::array<glm::vec3, 4> pts, int spec, int dx, int dy )
{
	t_shaderInput v0 = {spec, pts[0]};
	t_shaderInput v1 = {spec + dx + (1 << 17), pts[1]};
	t_shaderInput v2 = {spec + (dy << 6) + (1 << 18), pts[2]};
	t_shaderInput v3 = {spec + dx + (1 << 17) + (dy << 6) + (1 << 18), pts[3]};
	
	arr.push_back(v0);
	arr.push_back(v1);
	arr.push_back(v2);

	arr.push_back(v1);
	arr.push_back(v3);
	arr.push_back(v2);
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

glm::vec3 AMob::getPos( void )
{
	return (_position);
}

glm::vec3 AMob::getCamPos( bool update )
{
	(void)update;
	return (getEyePos());
}

glm::vec3 AMob::getDir( void )
{
	return (_front);
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

	_health -= damage;
	_hurtTime = -0.5f;
	_invulnerable = true;
	if (_health < 0) {
		_deathTime = -2.0f;
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
AMob* Chunk::mobHit( t_hit blockHit )
{
	if (!_mobs.size()) {
		return (NULL);
	}

	const glm::vec3 segStart = _player->getEyePos(), segEnd = segStart + _player->getDir() * settings::consts::reach::attack;
	for (auto& mob : _mobs) {
		glm::vec3 hitBoxCenter = mob->getPos(), hitBoxHalfSize = {0.3f, 0.3f, mob->getHitBox()};
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
	return (NULL);
}
