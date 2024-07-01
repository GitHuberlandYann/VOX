#ifndef ZOMBIE_HPP
# define ZOMBIE_HPP

# include "AHostileMob.hpp"

class Zombie : public AHostileMob
{
    public:
        Zombie( Chunk* chunk, Player* player, glm::vec3 position );
        ~Zombie( void );

        bool update( std::vector<t_shaderInputModel>& modArr, float deltaTime ) override;
		glm::vec3 getEyePos( void ) override;
		float getHitbox( void ) override;

    private:

		float getSpeed( void ) override;
		void updatePath( void ) override;
        void draw( std::vector<t_shaderInputModel>& arr );
};

#endif
