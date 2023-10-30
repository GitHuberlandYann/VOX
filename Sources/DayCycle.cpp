#include "vox.h"

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

DayCycle::DayCycle( void )
	: _gameTime(0), _day(0), _hour(6), _minute(0), _internal_light(15),
		_state(dayCycle_state::DAYTIME)
{

}

DayCycle::~DayCycle( void )
{

}

/* Internal sky light
 * level		up					down
 * 4  		to 4:18
 * 5  		to 4:28				to 19:40
 * 6  		to 4:38				to 19:30
 * 7  		to 4:48				to 19:20
 * 8  		to 4:58				to 19:10
 * 9  		to 4:08				to 19:00
 * 10 		to 5:18				to 18:50
 * 11 		to 5:28				to 18:40
 * 12 		to 5:38				to 18:30
 * 13 		to 5:48				to 18:20
 * 14 		to 5:58				to 18:10
 * 15 		to 18:00
*/
void DayCycle::updateInternalLight( void )
{
	switch (_internal_light) {
		case 4:
			if (_hour == 4 && _minute == 18)
				++_internal_light;
			break ;
		case 5:
			if (_hour == 4 && _minute == 28)
				++_internal_light;
			else if (_hour == 19 && _minute == 40)
				--_internal_light;
			break ;
		case 6:
			if (_hour == 4 && _minute == 38)
				++_internal_light;
			else if (_hour == 19 && _minute == 30)
				--_internal_light;
			break ;
		case 7:
			if (_hour == 4 && _minute == 48)
				++_internal_light;
			else if (_hour == 19 && _minute == 20)
				--_internal_light;
			break ;
		case 8:
			if (_hour == 4 && _minute == 58)
				++_internal_light;
			else if (_hour == 19 && _minute == 10)
				--_internal_light;
			break ;
		case 9:
			if (_hour == 5 && _minute == 8)
				++_internal_light;
			else if (_hour == 19 && _minute == 0)
				--_internal_light;
			break ;
		case 10:
			if (_hour == 5 && _minute == 18)
				++_internal_light;
			else if (_hour == 18 && _minute == 50)
				--_internal_light;
			break ;
		case 11:
			if (_hour == 5 && _minute == 28)
				++_internal_light;
			else if (_hour == 18 && _minute == 40)
				--_internal_light;
			break ;
		case 12:
			if (_hour == 5 && _minute == 38)
				++_internal_light;
			else if (_hour == 18 && _minute == 30)
				--_internal_light;
			break ;
		case 13:
			if (_hour == 5 && _minute == 48)
				++_internal_light;
			else if (_hour == 18 && _minute == 20)
				--_internal_light;
			break ;
		case 14:
			if (_hour == 5 && _minute == 58)
				++_internal_light;
			else if (_hour == 18 && _minute == 10)
				--_internal_light;
			break ;
		case 15:
			if (_hour == 18 && _minute == 0)
				--_internal_light;
			break ;
	}
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

DayCycle* DayCycle::_dayCycleInstance{NULL};
std::mutex DayCycle::_mtx;

DayCycle *DayCycle::Get( void )
{
	std::lock_guard<std::mutex> lock(_mtx); // multithread safety to not create several instances of Time
	if (_dayCycleInstance == NULL) {
		_dayCycleInstance = new DayCycle();
	}
	return (_dayCycleInstance);
}

void DayCycle::update( double deltaTime )
{
	_gameTime += deltaTime;
	if (_gameTime < MINECRAFT_MINUTE) {
		return ;
	}
	_gameTime -= MINECRAFT_MINUTE;
	++_minute;
	if (_minute == 60) {
		_minute = 0;
		++_hour;
		switch (_hour) {
			case 6:
				++_day;
				_state = dayCycle_state::DAYTIME;
				break ;
			case 18:
				_state = dayCycle_state::SUNSET;
				break ;
			case 19:
				_state = dayCycle_state::NIGHTTIME;
				break ;
			case 24:
				_hour = 0;
				break ;
			case 5:
				_state = dayCycle_state::SUNRISE;
		}
	}
	updateInternalLight();
}

std::string DayCycle::getInfos( void )
{
	const std::string dayCycle_states[4] = {"DAYTIME", "SUNSET", "NIGHTTIME", "SUNRISE"};
	return ("GameTimer\t> " + dayCycle_states[_state] + " on day " + std::to_string(_day) + " " + doubleDigits(_hour) + ":" + doubleDigits(_minute) + " (light " + doubleDigits(_internal_light) + ")");
}
