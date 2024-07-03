#include <string>
#include "FurnaceInstance.hpp"
#include "Blocks.hpp"
#include "Inventory.hpp"

FurnaceInstance::FurnaceInstance( void )
	: _fuel_time(0), _composant_time(0), _current_fuel_time(0)
{

}

FurnaceInstance::~FurnaceInstance( void )
{

}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

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

t_item FurnaceInstance::getComposant( void )
{
	return (_composant);
}

t_item FurnaceInstance::getFuel( void )
{
	return (_fuel);
}

t_item FurnaceInstance::getProduction( void )
{
	return (_production);
}

void FurnaceInstance::setComposant( t_item value )
{
	_composant = value;
}

void FurnaceInstance::setFuel( t_item value )
{
	_fuel = value;
}

void FurnaceInstance::setProduction( t_item value )
{
	_production = value;
}


void FurnaceInstance::removeComposant( void )
{
	if (_composant.type != blocks::air && --_composant.amount <= 0) {
		_composant = {0, 0};
	}
}

void FurnaceInstance::removeFuel( void )
{
	if (_fuel.type != blocks::air && --_fuel.amount <= 0) {
		_fuel = {0};
	}
}

t_item FurnaceInstance::pickProduction( void )
{
	t_item res = _production;
	_production = {0, 0};
	return (res);
}

t_item *FurnaceInstance::pickCompoFuel( bool fuel )
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
	if (_current_fuel_time && s_blocks[_composant.type]->isComposant) {
		if (_composant_time) {
			deltaTime = currentTime - _composant_time;
			if (deltaTime >= 10) { // A furnace smelts items at a speed of one item every 200 game ticks (10 seconds)
				if (_production.type == blocks::air) {
					_production = {s_blocks[_composant.type]->getProduction, 0};
				}
				if (++_production.amount == 64) {
					_composant_time = 0;
				} else {
					_composant_time += 10;
				}
				if (--_composant.amount == 0) {
					_composant = {0};
					_composant_time = 0;
				}
			}
		} else {
			if (_production.type == blocks::air || (_production.amount < 64 && _production.type == s_blocks[_composant.type]->getProduction)) {
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
			_current_fuel_time = s_blocks[_fuel.type]->fuel_time;
			if (_current_fuel_time) {
				if (--_fuel.amount == 0) {
					_fuel = {0};
				}
			} else {
				_fuel_time = 0;
				return (furnace_state::OFF);
			}
		}
	} else if (s_blocks[_fuel.type]->isFuel && s_blocks[_composant.type]->isComposant
		&& (_production.type == blocks::air || (_production.type < 64 && _production.type == s_blocks[_composant.type]->getProduction))) {
		_current_fuel_time = s_blocks[_fuel.type]->fuel_time;
		if (_current_fuel_time) {
			_fuel_time = currentTime;
			if (--_fuel.amount == 0) {
				_fuel = {0};
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
	std::string res = "\n\nComposant\t> " + s_blocks[_composant.type]->name + ((s_blocks[_composant.type]->isComposant) ? " smelts into " + s_blocks[s_blocks[_composant.type]->getProduction]->name : " is not composant")
						+ "\nFuel\t\t> " + s_blocks[_fuel.type]->name + ((s_blocks[_fuel.type]->isFuel) ? " lasts " + std::to_string(s_blocks[_fuel.type]->fuel_time) : " is not fuel")
						+ "\nTimes\t\t> " + std::to_string((_current_time - _composant_time) * (_composant_time > 0)) + ", " + std::to_string((_current_time - _fuel_time) * (_fuel_time > 0));

	return (res);
}
