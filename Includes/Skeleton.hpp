#ifndef SKELETON_HPP
# define SKELETON_HPP

# include "AHostileMob.hpp"

class Skeleton : public AHostileMob
{
    public:
        Skeleton( Chunk* chunk, Player* player, glm::vec3 position );
        ~Skeleton( void );

        bool update( std::vector<t_shaderInputModel>& modArr, float deltaTime ) override;
		glm::vec3 getEyePos( void ) override;
		float getHitbox( void ) override;

    private:

		float getSpeed( void ) override;
		void updatePath( void ) override;
        void draw( std::vector<t_shaderInputModel>& arr );
};

#endif
