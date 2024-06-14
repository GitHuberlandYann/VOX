#ifndef ZOMBIE_HPP
# define ZOMBIE_HPP

# include "AMob.hpp"

namespace state_machine {
    namespace zombie {
        const int idle = 0;
        const int chasing = 1;
    };
};

class Zombie : AMob
{
    public:
        Zombie( Chunk* chunk, glm::ivec3 position );
        virtual ~Zombie( void );

        virtual void update( std::vector<t_shaderInput>& modArr, double deltaTime );

    private:
        short _state;

        void draw( std::vector<t_shaderInput>& modArr, double deltaTime );
};

#endif
