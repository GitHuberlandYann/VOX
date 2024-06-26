#include "utils.h"
#include "DayCycle.hpp"
#include "Chat.hpp"
#include "logs.hpp"

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

DayCycle::DayCycle( void )
	: _gameTime(0), _ticks(0), _gameTicks(0), _day(0), _hour(6), _minute(0), _internalLight(15),
		_timeMultiplier(1), _gameTimeMultiplier(-1), _forceReset(false)
{

}

DayCycle::~DayCycle( void )
{
	MAINLOG(LOG("Destructor of DayCycle called"));
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

// sets _hour, _minute, _internalLight, and glClearColor form ticks
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
		_internalLight = static_cast<int>(gradient(_ticks, 12010, 13670, 15, 4));
	} else if (_ticks > 22299) {
		glClearColor(gradient(_ticks, 22300, 24000, 0, 120 / 255.0), gradient(_ticks, 22300, 24000, 0, 169 / 255.0), gradient(_ticks, 22300, 24000, 0, 1), 1.0f);
		_internalLight = static_cast<int>(gradient(_ticks, 22300, 24000, 4, 15));
	} else if (_forceReset) {
		(_ticks < 12500) ? glClearColor(120 / 255.0, 169 / 255.0, 1, 1.0f) : glClearColor(0, 0, 0, 1.0f);
		_internalLight = (_ticks < 12500) ? 15 : 4;
		_forceReset = false;
	} else {
		return ;
	}
	glUniform1i(_modelShader->getUniform(settings::consts::shader::uniform::internal_light), _internalLight);
	glUniform1i(_particleShader->getUniform(settings::consts::shader::uniform::internal_light), _internalLight);
	glUniform1i(_shader->getUniform(settings::consts::shader::uniform::internal_light), _internalLight);
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

void DayCycle::setShaderPtrs( Shader* shader, Shader* particleShader, Shader* modelShader )
{
	_shader = shader;
	glUniform1i(_shader->getUniform(settings::consts::shader::uniform::internal_light), _internalLight);
	_particleShader = particleShader;
	glUniform1i(_particleShader->getUniform(settings::consts::shader::uniform::internal_light), _internalLight);
	_modelShader = modelShader;
	glUniform1i(_modelShader->getUniform(settings::consts::shader::uniform::internal_light), _internalLight);
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

bool DayCycle::tickUpdate( void )
{
	_ticks += _timeMultiplier;
	if (_timeMultiplier || _forceReset) {
		setInternals();
	}
	if (_gameTimeMultiplier == -1) { // default
		++_gameTicks;
		return (true);
	} else if (_gameTimeMultiplier > 0) { // game was '//freeze'd and then //step was called
		++_gameTicks;
		--_gameTimeMultiplier;
		return (true);
	}
	return (false); // game is '//freeze'd
}

int DayCycle::getTicks( void )
{
	return (_ticks);
}

int DayCycle::getGameTicks( void )
{
	return (_gameTicks);
}

int DayCycle::getGameTimeMultiplier( void )
{
	return (_gameTimeMultiplier);
}

void DayCycle::setTicks( int ticks )
{
	_ticks = ticks;
	_gameTicks = ticks;
	_forceReset = true;
}

void DayCycle::addTicks( int ticks )
{
	_ticks += ticks;
	_forceReset = true;
}

void DayCycle::setTimeMultiplier( GLint mul )
{
	_timeMultiplier = mul;
}

/**
 * @brief toggle game_timeMultiplier between -1 and 0
 * @param chat used to display message to user
*/
void DayCycle::freeze( Chat *chat )
{
	if (_gameTimeMultiplier == -1) {
		_gameTimeMultiplier = 0;
		return (chat->chatMessage("Game frozen."));
	}
	_gameTimeMultiplier = -1;
	return (chat->chatMessage("Game unfrozen."));
}

/**
 * @brief if game frozen, allows for steps to be taken
 * @param chat used to display message to user
 * @param steps nbr of ticks to step through
*/
void DayCycle::step( Chat *chat, int steps )
{
	if (_gameTimeMultiplier == -1) {
		return (chat->chatMessage("You can't step when game is not frozen.", TEXT::RED));
	}
	_gameTimeMultiplier = steps;
	return (chat->chatMessage("Taking " + std::to_string(steps) + ((steps > 1) ? " steps." : " step.")));
}

std::string DayCycle::getInfos( void )
{
	return ("GameTimer\t> " + std::to_string(_ticks) + " speed " + std::to_string(_timeMultiplier) + " on day " + std::to_string(_day) + " " + doubleDigits(_hour) + ":" + doubleDigits(_minute) + " (light " + doubleDigits(_internalLight) + ")" + " GT " + std::to_string(_gameTicks) + " [" + std::to_string(_gameTimeMultiplier) + "]");
}

std::string DayCycle::getTime( void )
{
	return (std::to_string(_day) + "D" + doubleDigits(_hour) + ":" + doubleDigits(_minute));
}
