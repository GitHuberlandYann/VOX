#include "utils.h"
#include "DayCycle.hpp"

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

DayCycle::DayCycle( void )
	: _gameTime(0), _ticks(0), _day(0), _hour(6), _minute(0), _internal_light(15),
		_time_multiplier(1), _forceReset(false)
{

}

DayCycle::~DayCycle( void )
{
	std::cout << "Destructor of DayCycle called" << std::endl;
}

/* Internal sky light
 * level		up					down
 * 4  		to 4:18	22300						sky color is 0 0 0
 * 5  		to 4:28	22466		to 19:40	13670
 * 6  		to 4:38	22632		to 19:30	13504
 * 7  		to 4:48	22798		to 19:20	13338
 * 8  		to 4:58	22964		to 19:10	13172
 * 9  		to 5:08	23130		to 19:00	13006
 * 10 		to 5:18	23296		to 18:50	12840
 * 11 		to 5:28	23462		to 18:40	12674
 * 12 		to 5:38	23628		to 18:30	12508
 * 13 		to 5:48	23794		to 18:20	12342
 * 14 		to 5:58	23960		to 18:10	12176
 * 15 		to 18:00			12010			sky color is 120 169 255
*/

// sets _hour, _minute, _internal_light, and glClearColor form ticks
void DayCycle::setInternals( void )
{
	while (_ticks < 0) {
		if (_day) --_day;
		_ticks += 24000;
	}
	while (_ticks > 24000) {
		++_day;
		_ticks -= 24000;
	}
	_hour = (_ticks / 1000 + 6) % 24;
	_minute = (_ticks % 1000) / MINECRAFT_MINUTE;
	if (_ticks > 12000 && _ticks < 13671) {
		glClearColor(gradient(_ticks, 12010, 13670, 120 / 255.0, 0), gradient(_ticks, 12010, 13670, 169 / 255.0, 0), gradient(_ticks, 12010, 13670, 1, 0), 1.0f);
		_internal_light = static_cast<int>(gradient(_ticks, 12010, 13670, 15, 4));
	} else if (_ticks > 22299) {
		glClearColor(gradient(_ticks, 22300, 24000, 0, 120 / 255.0), gradient(_ticks, 22300, 24000, 0, 169 / 255.0), gradient(_ticks, 22300, 24000, 0, 1), 1.0f);
		_internal_light = static_cast<int>(gradient(_ticks, 22300, 24000, 4, 15));
	} else if (_forceReset) {
		(_ticks < 12500) ? glClearColor(120 / 255.0, 169 / 255.0, 1, 1.0f) : glClearColor(0, 0, 0, 1.0f);
		_internal_light = (_ticks < 12500) ? 15 : 4;
		_forceReset = false;
	} else {
		return ;
	}
	glUniform1i(_uniInternalLight, _internal_light);
	glUseProgram(_particleShaderProgram);
	glUniform1i(_uniPartInternalLight, _internal_light);
	glUseProgram(_shaderProgram);
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

DayCycle* DayCycle::_dayCycleInstance{NULL};
std::mutex DayCycle::_mtx;

DayCycle *DayCycle::Get( void )
{
	std::lock_guard<std::mutex> lock(_mtx); // multithread safety to not create several instances of DayCycle
	if (_dayCycleInstance == NULL) {
		_dayCycleInstance = new DayCycle();
	}
	return (_dayCycleInstance);
}

void DayCycle::Destroy( void )
{
	std::lock_guard<std::mutex> lock(_mtx); // multithread safety to not create several instances of DayCycle
	delete _dayCycleInstance;
	_dayCycleInstance = NULL;
}

void DayCycle::setUniInternalLight( GLuint shaderProgram, GLuint particleShaderProgram,
	GLint internal_light, GLint partice_internal_light )
{
	_shaderProgram = shaderProgram;
	_uniInternalLight = internal_light;
	glUniform1i(_uniInternalLight, _internal_light);
	_particleShaderProgram = particleShaderProgram;
	_uniPartInternalLight = partice_internal_light;
	glUseProgram(particleShaderProgram);
	glUniform1i(_uniPartInternalLight, _internal_light);
	glUseProgram(shaderProgram);

}

void DayCycle::setCloudsColor( GLint uniform_location )
{
	float color;
	if (_ticks > 12000 && _ticks < 13671) {
		color = gradient(_ticks, 12010, 13670, 1, 0);
	} else if (_ticks > 22299) {
		color = gradient(_ticks, 22300, 24000, 0, 1);
	} else {
		color = (_ticks < 12500) ? 1 : 0;
	}
	glUniform3f(uniform_location, color, color, color);
}

void DayCycle::tickUpdate( void )
{
	_ticks += _time_multiplier;
	setInternals();
}

void DayCycle::setTicks( int ticks )
{
	_ticks = ticks;
	_forceReset = true;
}

void DayCycle::addTicks( int ticks )
{
	_ticks += ticks;
	_forceReset = true;
}

void DayCycle::updateTimeMultiplier( GLint mul )
{
	if (mul == 1 && _time_multiplier < 256) {
		_time_multiplier <<= 1;
		if (!_time_multiplier) {
			_time_multiplier = 1;
		}
	} else if (mul == -1 && _time_multiplier > 0) {
			_time_multiplier >>= 1;
	}
}

std::string DayCycle::getInfos( void )
{
	return ("GameTimer\t> " + std::to_string(_ticks) + " speed " + std::to_string(_time_multiplier) + " on day " + std::to_string(_day) + " " + doubleDigits(_hour) + ":" + doubleDigits(_minute) + " (light " + doubleDigits(_internal_light) + ")");
}

std::string DayCycle::getTime( void )
{
	return (std::to_string(_day) + "D" + doubleDigits(_hour) + ":" + doubleDigits(_minute));
}
