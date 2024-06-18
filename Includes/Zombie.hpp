#ifndef ZOMBIE_HPP
# define ZOMBIE_HPP

# include "AHostileMob.hpp"

class Zombie : public AHostileMob
{
    public:
        Zombie( Chunk* chunk, Player* player, glm::vec3 position );
        virtual ~Zombie( void );

        virtual bool update( std::vector<t_shaderInput>& modArr, float deltaTime );
		virtual glm::vec3 getEyePos( void );
		virtual bool getHitBox( void );

    private:

		virtual float getSpeed( void );
		virtual void updatePath( void );
        void draw( std::vector<t_shaderInput>& arr );
};

#endif
