#include "AMob.hpp"
#include "utils.h"

AMob::AMob( void )
    : _air(0), _deathTime(0), _hurtTime(0), _fallDistance(0), _walkTime(0), _health(20), _position(0, 0, 0),
    _rotation(0, 0), _front(0, -1, 0), _right(glm::normalize(glm::cross(_front, settings::consts::math::world_up))), _up(0, 0, 1), _bodyFront(0, -1),
    _invulnerable(true), _touchGround(true), _noAI(false), _persistenceRequired(false), _chunk(NULL)
{

}

AMob::~AMob( void )
{

}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //
// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //
