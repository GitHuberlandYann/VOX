#include "vox.h"

Camera::Camera( glm::vec3 position ) : _movement_speed(SPEED), _mouse_sensitivity(SENSITIVITY), _fov(FOV)
{
	_position = position;
	_world_up = glm::vec3(0.0f, 0.0f, 1.0f);
	_yaw = YAW;
	_pitch = PITCH;
	_mouse_update = false;
	_scroll_update = false;
	updateCameraVectors();
	(void)_movement_speed;
	(void)_mouse_sensitivity;
}

Camera::~Camera( void )
{
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void Camera::updateCameraVectors( void )
{
	glm::vec3 front;
	front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	front.y = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	front.z = sin(glm::radians(_pitch));
	_front = glm::normalize(front);
	_right = glm::normalize(glm::cross(_front, _world_up));
	_up    = glm::normalize(glm::cross(_right, _front));
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

 glm::mat4 Camera::getViewMatrix( void )
{
	return (glm::lookAt(_position, _position + _front, _up));
}

glm::mat4 Camera::getPerspectiveMatrix( void )
{
	return (glm::perspective(glm::radians(_fov), (GLfloat)WIN_WIDTH / (GLfloat)WIN_HEIGHT, 0.00001f, 100.0f));
}

void Camera::processKeyboard( Camera_Movement direction )
{
	if (direction == FORWARD)
		_position += _front * _movement_speed;
	else if (direction == BACKWARD)
		_position -= _front * _movement_speed;
	else if (direction == LEFT)
		_position -= _right * _movement_speed;
	else if (direction == RIGHT)
		_position += _right * _movement_speed;
	else if (direction == UP)
		_position += _up * _movement_speed;
	else if (direction == DOWN)
		_position -= _up * _movement_speed;
}
/*
void Camera::processMouseMovement( float x_offset, float y_offset )
{
	_yaw += x_offset * _mouse_sensitivity;
	_pitch += y_offset * _mouse_sensitivity;

	if (_pitch > 89.0f)
		_pitch = 89.0f;
	if (_pitch < -89.0f)
		_pitch = -89.0f;

	updateCameraVectors();
	_mouse_update = true;
}

void Camera::processMouseScroll( float offset )
{
	_zoom -= offset;
	if (_zoom < 1.0f)
		_zoom = 1.0f;
	if (_zoom > 45.0f)
		_zoom = 45.0f;
	_scroll_update = true;
}
*/