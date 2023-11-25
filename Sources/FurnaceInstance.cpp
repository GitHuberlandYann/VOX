#include <string>
#include "FurnaceInstance.hpp"
#include "Blocks.hpp"

FurnaceInstance::FurnaceInstance( void )
	: _composant(blocks::AIR, 0), _fuel(blocks::AIR, 0), _production(blocks::AIR, 0),
		_fuel_time(0), _composant_time(0), _current_fuel_time(0)
{

}

FurnaceInstance::~FurnaceInstance( void )
{

}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

int FurnaceInstance::count( void )
{
	return ((_composant.x != blocks::AIR) + ( _fuel.x != blocks::AIR) + (_production.x != blocks::AIR) + (_fuel_time > 0) + (_composant_time > 0));
}

float FurnaceInstance::getComposantTime( void )
{
	if (!_composant_time) {
		return (0);
	}
	return ((_current_time - _composant_time) / 10);
}

float FurnaceInstance::getFuelTime( void )
{
	if (_current_fuel_time) {
		return ((_current_time - _fuel_time) / _current_fuel_time);
	}
	return (0);
}

glm::ivec2 FurnaceInstance::getComposant( void )
{
	return (_composant);
}

glm::ivec2 FurnaceInstance::getFuel( void )
{
	return (_fuel);
}

glm::ivec2 FurnaceInstance::getProduction( void )
{
	return (_production);
}

void FurnaceInstance::setComposant( glm::ivec2 value )
{
	_composant = value;
}

void FurnaceInstance::setFuel( glm::ivec2 value )
{
	_fuel = value;
}

void FurnaceInstance::setProduction( glm::ivec2 value )
{
	_production = value;
}


void FurnaceInstance::removeComposant( void )
{
	if (_composant.x != blocks::AIR && --_composant.y <= 0) {
		_composant = glm::ivec2(blocks::AIR, 0);
	}
}

void FurnaceInstance::removeFuel( void )
{
	if (_fuel.x != blocks::AIR && --_fuel.y <= 0) {
		_fuel = glm::ivec2(blocks::AIR, 0);
	}
}

glm::ivec2 FurnaceInstance::pickProduction( void )
{
	glm::ivec2 res = _production;
	_production = glm::ivec2(blocks::AIR, 0);
	return (res);
}

glm::ivec2 *FurnaceInstance::pickCompoFuel( bool fuel )
{
	if (fuel) {
		return (&_fuel);
	}
	return (&_composant);
}

int FurnaceInstance::updateTimes( float currentTime )
{
	_current_time = currentTime; // used for display
	float deltaTime;
	if (_current_fuel_time && s_blocks[_composant.x].isComposant) {
		if (_composant_time) {
			deltaTime = currentTime - _composant_time;
			if (deltaTime >= 10) { // A furnace smelts items at a speed of one item every 200 game ticks (10 seconds)
				if (_production.x == blocks::AIR) {
					_production = glm::ivec2(s_blocks[_composant.x].getProduction, 0);
				}
				if (++_production.y == 64) {
					_composant_time = 0;
				} else {
					_composant_time += 10;
					if (--_composant.y == 0) {
						_composant.x = blocks::AIR;
						_composant_time = 0;
					}
				}
			}
		} else {
			if (_production.x == blocks::AIR || (_production.y < 64 && _production.x == s_blocks[_composant.x].getProduction)) {
				_composant_time = currentTime;
			}
		}
	} else {
		_composant_time = 0;
	}

	if (_current_fuel_time) {
		deltaTime = currentTime - _fuel_time;
		if (deltaTime >= _current_fuel_time) {
			_fuel_time += _current_fuel_time;
			_current_fuel_time = s_blocks[_fuel.x].fuel_time;
			if (_current_fuel_time) {
				if (--_fuel.y == 0) {
					_fuel.x = blocks::AIR;
				}
			} else {
				_fuel_time = 0;
				return (furnace_state::OFF);
			}
		}
	} else if (s_blocks[_fuel.x].isFuel && s_blocks[_composant.x].isComposant
		&& (_production.x == blocks::AIR || (_production.y < 64 && _production.x == s_blocks[_composant.x].getProduction))) {
		_current_fuel_time = s_blocks[_fuel.x].fuel_time;
		if (_current_fuel_time) {
			_fuel_time = currentTime;
			if (--_fuel.y == 0) {
				_fuel.x = blocks::AIR;
			}
			return (furnace_state::ON);
		}
	}
	return (furnace_state::NOCHANGE);
}

// coal 80 seconds
// log planks craftingTable 15 seconds
// woodenTools 10 seconds
// stick 5 seconds

std::string FurnaceInstance::getInfoString( void )
{
	std::string res = "\n\nComposant\t> " + s_blocks[_composant.x].name + ((s_blocks[_composant.x].isComposant) ? " smelts into " + s_blocks[s_blocks[_composant.x].getProduction].name : " is not composant")
						+ "\nFuel\t\t> " + s_blocks[_fuel.x].name + ((s_blocks[_fuel.x].isFuel) ? " lasts " + std::to_string(s_blocks[_fuel.x].fuel_time) : " is not fuel")
						+ "\nTimes\t\t> " + std::to_string((_current_time - _composant_time) * (_composant_time > 0)) + ", " + std::to_string((_current_time - _fuel_time) * (_fuel_time > 0));

	return (res);
}
