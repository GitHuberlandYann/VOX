#include "utils.h"
#include "Settings.hpp"
#include <cassert>

Settings::Settings( void )
{
	_bools[SETTINGS::BOOL::FULLSCREEN] = true;
	_bools[SETTINGS::BOOL::SKYBOX] = true;
	_bools[SETTINGS::BOOL::PARTICLES] = true;
	_bools[SETTINGS::BOOL::FACE_CULLING] = false;

	_ints[SETTINGS::INT::RENDER_DIST] = RENDER_DISTANCE;
	_ints[SETTINGS::CLOUDS] = SETTINGS::FANCY;

	_floats[SETTINGS::FLOAT::FOV] = FOV_START;
	_floats[SETTINGS::FLOAT::BRIGHTNESS] = BRIGHTNESS_START;
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
	assert(((target >= 0 && target < SETTINGS::NBR_BOOL) && "Settings::getBool out of bound"));
	return (_bools[target]);
}

void Settings::setBool( int target, bool value )
{
	assert(((target >= 0 && target < SETTINGS::NBR_BOOL) && "Settings::setBool out of bound"));
	_bools[target] = value;
}

int Settings::getInt( int target )
{
	assert(((target >= 0 && target < SETTINGS::NBR_INT) && "Settings::getInt out of bound"));
	return (_ints[target]);
}

void Settings::setInt( int target, int value )
{
	assert(((target >= 0 && target < SETTINGS::NBR_INT) && "Settings::setInt out of bound"));
	_ints[target] = value;
}

float Settings::getFloat( int target )
{
	assert(((target >= 0 && target < SETTINGS::NBR_FLOAT) && "Settings::getFloat out of bound"));
	return (_floats[target]);
}

void Settings::setFloat( int target, float value )
{
	assert(((target >= 0 && target < SETTINGS::NBR_FLOAT) && "Settings::setFloat out of bound"));
	_floats[target] = value;
}

std::string Settings::getString( int target )
{
	assert(((target >= 0 && target < SETTINGS::NBR_STRING) && "Settings::getString out of bound"));
	return (_strings[target]);
}

void Settings::setString( int target, std::string value )
{
	assert(((target >= 0 && target < SETTINGS::NBR_STRING) && "Settings::setString out of bound"));
	_strings[target] = value;
}

void Settings::pushResourcePack( std::string pack )
{
	_packs.push_back(pack);
}

void Settings::setResourcePacks( std::vector<std::string> &packs )
{
	_packs = packs;
}

std::vector<std::string> &Settings::getResourcePacks( void )
{
	return (_packs);
}
