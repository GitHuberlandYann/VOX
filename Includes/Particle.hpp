#ifndef PARTICLE_HPP
# define PARTICLE_HPP

namespace PARTICLES
{
	enum {
		FLAME, // torches and furnaces
		SMOKE,
		BIG_SMOKE, // campfire
		EXPLOSION, // TNT and such
		BREAKING, // breaking block
	};
}

class Particle
{
	private:
		glm::vec3 _pos, _dir;
		float _lifeTime, _shade;
		int _type, _frame, _block;
		Chunk *_chunk;

		bool updateFlame( std::vector<std::pair<int, glm::vec3>> &arr, glm::vec3 camDir );
		bool updateSmoke( std::vector<std::pair<int, glm::vec3>> &arr, glm::vec3 camDir, float deltaTime );
		bool updateExplosion( std::vector<std::pair<int, glm::vec3>> &arr, glm::vec3 camDir );
		bool updateBreaking( std::vector<std::pair<int, glm::vec3>> &arr, glm::vec3 camDir, float deltaTime );
	
	public:
		Particle( Chunk *chunk, glm::vec3 pos, int type, float shade = 1, int block = blocks::AIR );
		~Particle( void );

		bool update( std::vector<std::pair<int, glm::vec3>> &arr, glm::vec3 camPos, glm::vec3 camDir, double deltaTime );
};

#endif
