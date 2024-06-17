#include "AHostileMob.hpp"
#include "Settings.hpp"

AHostileMob::AHostileMob( Player* player, glm::vec3 position )
	: AMob(position), _state(settings::state_machine::idle), _stateTime(0.0f), _blockTime(0.0f), _player(player)
{

}

AHostileMob::~AHostileMob( void )
{

}

void AHostileMob::setState( const short state )
{
	_state = state;
	_stateTime = 0.0f;
	_blockTime = 0.0f;
}
