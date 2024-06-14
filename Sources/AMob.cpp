#include "AMob.hpp"

AMob::Amob( void )
    : _air(0), _deathTime(0), _hurtTime(0), _fall_distance(0), _health(20), _position({0}),
    _rotation({0}), _right(1, 0, 0), _front(0, -1, 0), _up(0, 0, 1),
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
