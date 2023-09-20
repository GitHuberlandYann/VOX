#include "vox.h"

Camera::Camera( glm::vec3 position ) : _speed_frame(0), _fov(FOV), _movement_speed(SPEED)
{
	_position = position;
	_world_up = glm::vec3(0.0f, 0.0f, 1.0f);
	_yaw = YAW;
	_pitch = PITCH;
	_mouse_update = false;
	_scroll_update = false;
	updateCameraVectors();
	// (void)_mouse_sensitivity;
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
	return (glm::perspective(glm::radians(_fov), (GLfloat)WIN_WIDTH / (GLfloat)WIN_HEIGHT, 0.05f, 1000.0f));
}

void Camera::setDelta( float deltaTime )
{
	_speed_frame = _movement_speed * deltaTime;
}

void Camera::update_movement_speed( GLint key_cam_speed )
{
	if (_movement_speed + key_cam_speed >= 0) {
		_movement_speed += key_cam_speed;
	}
}

void Camera::processKeyboard( Camera_Movement direction )
{
	if (direction == FORWARD)
		_position += _front * _speed_frame;
	else if (direction == BACKWARD)
		_position -= _front * _speed_frame;
	else if (direction == LEFT)
		_position -= _right * _speed_frame;
	else if (direction == RIGHT)
		_position += _right * _speed_frame;
	else if (direction == UP)
		_position += _up * _speed_frame;
	else if (direction == DOWN)
		_position -= _up * _speed_frame;
}

void Camera::processPitch( GLint offset )
{
	_pitch += offset * _speed_frame * 1.5f;
	if (_pitch > 85.0f || _pitch < -85.0f) {
		_pitch -= offset * _speed_frame * 1.5f;
	}

	updateCameraVectors();
}

void Camera::processYaw( GLint offset )
{
	_yaw += offset * _speed_frame * 1.5f;
	if (_yaw > 180.0f) {
		_yaw -= 360.0f;
	} else if (_yaw < -180.0f) {
		_yaw += 360.0f;
	}

	updateCameraVectors();
}

std::vector<glm::ivec3> Camera::get_ray_casting( GLfloat radius )
{
	return (voxel_traversal(_position, _position + _front * radius));
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