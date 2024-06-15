#ifndef AHOSTILEMOB_HPP
# define AHOSTILEMOB_HPP

# include "AMob.hpp"
class Camera;

class AHostileMob : public AMob
{
	public:
		AHostileMob( Camera* player, glm::vec3 position );
		virtual ~AHostileMob( void );

	protected:
		short _state;
		float _stateTime;
		glm::ivec3 _currentBlock;
		std::vector<glm::ivec3> _path;
		Camera* _player;

		void updateCurrentBlock( void );
};

#endif