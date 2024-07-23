#include "utils.h"
#include "Settings.hpp"
#include "logs.hpp"
#include <cassert>

Settings::Settings( void )
{
	_bools.fill(true);
	_bools[settings::bools::face_culling] = false;
	_bools[settings::bools::visible_chunk_border] = false;
	_bools[settings::bools::flat_world] = false;
	_bools[settings::bools::hide_ui] = false;

	_ints[settings::ints::render_dist] = settings::defaults::render_distance;
	_ints[settings::ints::clouds] = settings::FANCY;
	_ints[settings::ints::flat_world_block] = settings::fw_types[0];
	_ints[settings::ints::game_mode] = settings::defaults::game_mode;
	_ints[settings::ints::world_spawn_x] = 0;
	_ints[settings::ints::world_spawn_y] = 0;
	_ints[settings::ints::world_spawn_z] = 256;

	_floats[settings::floats::fov] = settings::defaults::fov;
	_floats[settings::floats::brightness] = settings::defaults::brightness;
}

Settings::~Settings( void )
{
	MAINLOG(LOG("Destructor of Settings called"));
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
	assert(((target >= 0 && target < settings::bools::size) && "Settings::getBool out of bound"));
	return (_bools[target]);
}

void Settings::setBool( int target, bool value )
{
	assert(((target >= 0 && target < settings::bools::size) && "Settings::setBool out of bound"));
	_bools[target] = value;
}

int Settings::getInt( int target )
{
	assert(((target >= 0 && target < settings::ints::size) && "Settings::getInt out of bound"));
	return (_ints[target]);
}

void Settings::setInt( int target, int value )
{
	assert(((target >= 0 && target < settings::ints::size) && "Settings::setInt out of bound"));
	_ints[target] = value;
}

float Settings::getFloat( int target )
{
	assert(((target >= 0 && target < settings::floats::size) && "Settings::getFloat out of bound"));
	return (_floats[target]);
}

void Settings::setFloat( int target, float value )
{
	assert(((target >= 0 && target < settings::floats::size) && "Settings::setFloat out of bound"));
	_floats[target] = value;
}

std::string Settings::getString( int target )
{
	assert(((target >= 0 && target < settings::strings::size) && "Settings::getString out of bound"));
	return (_strings[target]);
}

void Settings::setString( int target, std::string value )
{
	assert(((target >= 0 && target < settings::strings::size) && "Settings::setString out of bound"));
	_strings[target] = value;
}

void Settings::pushResourcePack( std::string pack )
{
	_packs.push_back(pack);
}

void Settings::setResourcePacks( std::vector<std::string>& packs )
{
	_packs = packs;
}

std::vector<std::string> &Settings::getResourcePacks( void )
{
	return (_packs);
}
