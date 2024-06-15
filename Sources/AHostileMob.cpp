#include "AHostileMob.hpp"
#include "Settings.hpp"

AHostileMob::AHostileMob( Camera* player, glm::vec3 position )
	: AMob(position), _state(settings::state_machine::idle), _stateTime(0), _currentBlock(glm::floor(position)), _player(player)
{

}

AHostileMob::~AHostileMob( void )
{

}
