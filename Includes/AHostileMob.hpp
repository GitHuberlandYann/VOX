#ifndef AHOSTILEMOB_HPP
# define AHOSTILEMOB_HPP

# include "AMob.hpp"
class Player;

namespace settings {
	namespace state_machine {
		const short idle = 0;
		const short wandle = 1;
		const short chase = 2;
	};
};

class AHostileMob : public AMob
{
	public:
		virtual ~AHostileMob( void );

	protected:
		short _state;
		float _stateTime;
		float _blockTime; // how many seconds the mob has been on the same block, used to cancel chase state and unstuck mob
		std::vector<glm::ivec3> _path;
		Player* _player;

		AHostileMob( Player* player, glm::vec3 position );

		virtual void updatePath( void ) = 0;
		virtual bool updateCurrentBlock( void );
		void setState( const short state );
};

#endif