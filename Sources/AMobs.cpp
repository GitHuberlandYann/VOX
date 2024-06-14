#include "AMobs.hpp"

AMobs::Amobs( void )
    : _air(0), _deathTime(0), _hurtTime(0), _fall_distance(0), _health(20), _position({0}),
    _rotation({0}), _invulnerable(true), _touchGround(true), _noAI(false), _persistenceRequired(false)
{

}

AMobs::~AMobs( void )
{

}
