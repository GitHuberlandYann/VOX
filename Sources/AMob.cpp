#include "Chunk.hpp"

AMob::AMob( glm::vec3 position )
    : _air(0), _deathTime(0), _hurtTime(0), _fallDistance(0), _fallTime(0), _z0(position.z), _walkTime(0), _health(20), _position(position),
    _rotation(0, 0), _front(0, -1, 0), _right(glm::normalize(glm::cross(_front, settings::consts::math::world_up))), _up(0, 0, 1), _bodyFront(0, -1),
    _invulnerable(true), _touchGround(true), _walking(false), _inJump(false), _noAI(false), _persistenceRequired(false), _chunk(NULL)
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

void AMob::setTouchGround( bool state )
{
	_touchGround = state;
}
