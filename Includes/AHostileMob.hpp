#ifndef AHOSTILEMOB_HPP
# define AHOSTILEMOB_HPP

# include "AMob.hpp"
class Player;

class AHostileMob : public AMob
{
	public:
		AHostileMob( Player* player, glm::vec3 position );
		virtual ~AHostileMob( void );

	protected:
		short _state;
		float _stateTime;
		float _blockTime; // how many seconds the mob has been on the same block, used to cancel chase state and unstuck mob
		std::vector<glm::ivec3> _path;
		Player* _player;

		virtual bool updateCurrentBlock( void );
		void setState( const short state );
};

#endif