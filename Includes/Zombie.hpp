#ifndef ZOMBIE_HPP
# define ZOMBIE_HPP

# include "AHostileMob.hpp"

namespace state_machine {
    namespace zombie {
        const int idle = 0;
        const int chasing = 1;
    };
};

class Zombie : public AHostileMob
{
    public:
        Zombie( Chunk* chunk, Camera* player, glm::vec3 position );
        virtual ~Zombie( void );

        virtual void update( std::vector<t_shaderInput>& modArr, float deltaTime );
		virtual bool getHitBox( void );

    private:

        void draw( std::vector<t_shaderInput>& arr );
};

#endif
