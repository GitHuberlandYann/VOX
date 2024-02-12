#ifndef PARTICLE_HPP
# define PARTICLE_HPP

namespace PARTICLES
{
	enum {
		FLAME, // torches and furnaces
		BIG_SMOKE,
		EXPLOSION
	};
}

class Particle
{
	private:
		glm::vec3 _pos, _dir;
		float _lifeTime;
		int _type, _frame;
		Chunk *_chunk;

		bool updateFlame( std::vector<std::pair<int, glm::vec3>> &arr, glm::vec3 camDir );
		bool updateExplosion( std::vector<std::pair<int, glm::vec3>> &arr, glm::vec3 camDir );
	
	public:
		Particle( Chunk *chunk, glm::vec3 pos, int type );
		~Particle( void );

		bool update( std::vector<std::pair<int, glm::vec3>> &arr, glm::vec3 camPos, glm::vec3 camDir, double deltaTime );
};

#endif
