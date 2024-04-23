#include "utils.h"
#include "Settings.hpp"
#include <cassert>

Settings::Settings( void )
{
	_bools[SETTINGS::SKYBOX] = true;

	_ints[SETTINGS::RENDER_DIST] = RENDER_DISTANCE;

	_floats[SETTINGS::FOV] = FOV_START;
}

Settings::~Settings( void )
{
	std::cout << "Destructor of Settings called" << std::endl;
}

// ************************************************************************** //
//                             Get / Destroy                                  //
// ************************************************************************** //

Settings* Settings::_settingsInstance{NULL};
std::mutex Settings::_mtx;

Settings *Settings::Get( void )
{
	std::lock_guard<std::mutex> lock(_mtx); // multithread safety to not create several instances of Settings
	if (_settingsInstance == NULL) {
		_settingsInstance = new Settings();
	}
	return (_settingsInstance);
}

void Settings::Destroy( void )
{
	std::lock_guard<std::mutex> lock(_mtx); // multithread safety to not create several instances of Settings
	delete _settingsInstance;
	_settingsInstance = NULL;
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

bool Settings::getBool( int target )
{
	if (target < 0 || target >= SETTINGS::NBR_BOOL) {
		assert((0 == 1 && "Settings::getBool out of bound"));
	}
	return (_bools[target]);
}

void Settings::setBool( int target, bool value )
{
	if (target < 0 || target >= SETTINGS::NBR_BOOL) {
		assert((0 == 1 && "Settings::setBool out of bound"));
	}
	_bools[target] = value;
}

int Settings::getInt( int target )
{
	if (target < 0 || target >= SETTINGS::NBR_INT) {
		assert((0 == 1 && "Settings::getInt out of bound"));
	}
	return (_ints[target]);
}

void Settings::setInt( int target, int value )
{
	if (target < 0 || target >= SETTINGS::NBR_INT) {
		assert((0 == 1 && "Settings::setInt out of bound"));
	}
	_ints[target] = value;
}

float Settings::getFloat( int target )
{
	if (target < 0 || target >= SETTINGS::NBR_FLOAT) {
		assert((0 == 1 && "Settings::getFloat out of bound"));
	}
	return (_floats[target]);
}

void Settings::setFloat( int target, float value )
{
	if (target < 0 || target >= SETTINGS::NBR_FLOAT) {
		assert((0 == 1 && "Settings::setFloat out of bound"));
	}
	_floats[target] = value;
}
