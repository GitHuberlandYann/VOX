#include "utils.h"
#include "Particle.hpp"

Particle::Particle( glm::vec3 pos, int type )
	: _pos(pos), _lifeTime(0), _type(type)
{
	(void)_pos;
	(void)_dir;
	(void)_lifeTime;
	(void)_type;
}

Particle::~Particle( void )
{

}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Particle::update( void )
{

}
