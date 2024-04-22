#include "utils.h"
#include "Settings.hpp"
#include <cassert>

Settings::Settings( void )
	: _render_distance(RENDER_DISTANCE), _fov(FOV_START)
{

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

int Settings::getInt( int target )
{
	switch (target) {
		case SETTINGS::RENDER_DIST:
			return (_render_distance);
	}
	assert((0 == 1 && "Settings::getInt reached end of function"));
}

void Settings::setInt( int target, int value )
{
	switch (target) {
		case SETTINGS::RENDER_DIST:
			_render_distance = value;
			return ;
	}
	assert((0 == 1 && "Settings::setInt reached end of function"));
}

float Settings::getFloat( int target )
{
	switch (target) {
		case SETTINGS::FOV:
			return (_fov);
	}
	assert((0 == 1 && "Settings::getFloat reached end of function"));
}

void Settings::setFloat( int target, float value )
{
	switch (target) {
		case SETTINGS::FOV:
			_fov = value;
			return ;
	}
	assert((0 == 1 && "Settings::setFloat reached end of function"));
}
