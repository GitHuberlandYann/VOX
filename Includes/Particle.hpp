#ifndef PARTICLE_HPP
# define PARTICLE_HPP

namespace particles
{
	enum {
		flame, // torches and furnaces
		smoke,
		big_smoke, // campfire
		explosion, // TNT and such
		breaking, // breaking block
	};
}

typedef struct s_shaderInputPart t_shaderInputPart;

class Particle
{
	public:
		Particle( Chunk* chunk, glm::vec3 pos, int type, float shade = 1, int block = blocks::air );
		~Particle( void );

		bool update( std::vector<t_shaderInput>& entityArr, std::vector<t_shaderInputPart>& partArr, glm::vec3 camPos, glm::vec3 camDir, double deltaTime );

	private:
		glm::vec3 _pos, _dir;
		glm::ivec2 _texOffset;
		float _lifeTime, _shade;
		int _type, _frame, _block;
		Chunk *_chunk;

		bool updateFlame( std::vector<t_shaderInputPart>& arr, glm::vec3 camDir );
		bool updateSmoke( std::vector<t_shaderInputPart>& arr, glm::vec3 camDir, float deltaTime );
		bool updateExplosion( std::vector<t_shaderInputPart>& arr, glm::vec3 camDir );
		bool updateBreaking( std::vector<t_shaderInput>& arr, glm::vec3 camDir, float deltaTime );
};

#endif
