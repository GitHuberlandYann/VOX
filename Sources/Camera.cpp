#include "Camera.hpp"
#include "Settings.hpp"
#include "utils.h"

Camera::Camera( void )
	: _camPlacement(CAMPLACEMENT::DEFAULT)
{

}

Camera::~Camera( void )
{
	std::cout << "Destructor of Camera called" << std::endl;
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

glm::mat4 Camera::getViewMatrix( void )
{
	if (!_target) {
		return (glm::lookAt(glm::vec3(0), glm::vec3(1), settings::consts::math::world_up));
	}
	glm::vec3 pos = _target->getEyePos();
	switch (_camPlacement) {
	case CAMPLACEMENT::DEFAULT:
		return (glm::lookAt(pos, pos + _target->getDir(), settings::consts::math::world_up));
	case CAMPLACEMENT::BEHIND:
		return (glm::lookAt(pos - _target->getDir() * 5.0f, pos, settings::consts::math::world_up));
	case CAMPLACEMENT::FRONT:
		return (glm::lookAt(pos + _target->getDir() * 5.0f, pos, settings::consts::math::world_up));
	}
	return (glm::lookAt(pos, pos + _target->getDir(), settings::consts::math::world_up));
}

glm::mat4 Camera::getPerspectiveMatrix( void )
{
	return (glm::perspective(glm::radians(Settings::Get()->getFloat(settings::floats::fov) + ((_target) ? _target->getFovOffset() : 0)), (GLfloat)WIN_WIDTH / (GLfloat)WIN_HEIGHT, 0.1f, 1000.0f));
}

void Camera::setTarget( AMob* mob )
{
	_target = mob;
}

glm::vec3 Camera::getCamPos( void )
{
	if (!_target) {
		return {0.0f, 0.0f, 0.0f};
	}
	glm::vec3 pos = _target->getEyePos();
	switch (_camPlacement) {
	case CAMPLACEMENT::DEFAULT:
		return (pos);
	case CAMPLACEMENT::BEHIND:
		return (pos - _target->getDir() * 5.0f);
	case CAMPLACEMENT::FRONT:
		return (pos + _target->getDir() * 5.0f);
	}
	return (pos);
}

int Camera::getCamPlacement( void )
{
	return (_camPlacement);
}

void Camera::changeCamPlacement( void )
{
	const int next[3] = {CAMPLACEMENT::BEHIND, CAMPLACEMENT::FRONT, CAMPLACEMENT::DEFAULT};
	_camPlacement = next[_camPlacement];
	if (_target) {
		_target->setCamUpdate(true);
	}
}
