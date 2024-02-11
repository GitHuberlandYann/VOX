#ifndef PARTICLE_HPP
# define PARTICLE_HPP

namespace PARTICLES
{
	enum {
		SMOKE,
		FIRE,
		EXPLOSION
	};
}

class Particle
{
	private:
		glm::vec3 _pos, _dir;
		float _lifeTime;
		int _type;
	
	public:
		Particle( glm::vec3 pos, int type );
		~Particle( void );

		void update( void );
};

#endif
