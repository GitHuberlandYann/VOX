#ifndef ZOMBIE_HPP
# define ZOMBIE_HPP

# include "AMobs.hpp"

namespace state_machine {
    namespace zombie {
        const int idle = 0;
        const int chasing = 1;
    };
};

class Zombie : AMobs
{
    public:
        Zombie( void );
        ~Zombie( void );

    private:
        short _state;
};

#endif
