#include "vox.h"

Camera::Camera( glm::vec3 position ) : _fov(FOV), _fall_speed(0), _movement_speed(SPEED)
{
	_position = position;
	_world_up = glm::vec3(0.0f, 0.0f, 1.0f);
	_yaw = YAW;
	_pitch = PITCH;
	_update = false;
	updateCameraVectors();
}

Camera::~Camera( void )
{
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void Camera::updateCameraVectors( void )
{
	_update = true;
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
	_update = false;
	return (glm::lookAt(_position, _position + _front, _up));
}

glm::mat4 Camera::getPerspectiveMatrix( void )
{
	return (glm::perspective(glm::radians(_fov), (GLfloat)WIN_WIDTH / (GLfloat)WIN_HEIGHT, 0.1f, 1000.0f));
}

void Camera::setDelta( float deltaTime )
{
	// _speed_frame = _movement_speed * deltaTime;
	_deltaTime = deltaTime;
}

void Camera::update_movement_speed( GLint key_cam_speed )
{
	if (_movement_speed + key_cam_speed >= 0) {
		_movement_speed += key_cam_speed;
	}
}

void Camera::processKeyboard( Camera_Movement direction, bool game_mode )
{
	_update = true;
	if (game_mode == CREATIVE) {
		float speed_frame = _deltaTime * _movement_speed;
		if (direction == FORWARD)
			_position += _front * speed_frame;
		else if (direction == BACKWARD)
			_position -= _front * speed_frame;
		else if (direction == LEFT)
			_position -= _right * speed_frame;
		else if (direction == RIGHT)
			_position += _right * speed_frame;
		else if (direction == UP)
			_position += _up * speed_frame;
		else if (direction == DOWN)
			_position -= _up * speed_frame;
	} else {
		float speed_frame = _deltaTime * WALK_SPEED;
		if (direction == FORWARD)
			_position += glm::vec3(_front.x, _front.y, 0.0f) * speed_frame;
		else if (direction == BACKWARD)
			_position -= glm::vec3(_front.x, _front.y, 0.0f) * speed_frame;
		else if (direction == LEFT)
			_position -= glm::vec3(_right.x, _right.y, 0.0f) * speed_frame;
		else if (direction == RIGHT)
			_position += glm::vec3(_right.x, _right.y, 0.0f) * speed_frame;
	}
}

void Camera::fall( bool real_fall )
{
	if (_fall_speed + 1 < FALL_SPEED) {
		_fall_speed++;
	}
	// std::cout << "fall " << real_fall << std::endl;
	if (real_fall) { // empty block bellow
		_position.z -= _deltaTime * _fall_speed;
		_update = true;
	} else {//if (_position.z - int(_position.z) > 0.62f) {
		float new_z = _position.z - _deltaTime * _fall_speed;
		if (new_z - int(_position.z) > 0.62f) {
			_position.z = new_z;
			_update = true;
		} else {
			_position.z = int(_position.z) + 0.62f;
			_update = true;
			touchGround();
		}
	}
}

void Camera::touchGround( void )
{
	// std::cout << "TOUCH GROUND AT " << _fall_speed << std::endl;
	_fall_speed = 1;
}

void Camera::processPitch( GLint offset )
{
	_pitch += offset * _deltaTime * _movement_speed * 1.5f;
	if (_pitch > 85.0f || _pitch < -85.0f) {
		_pitch -= offset * _deltaTime * _movement_speed * 1.5f;
	}

	updateCameraVectors();
}

void Camera::processYaw( GLint offset )
{
	_yaw += offset * _deltaTime * _movement_speed * 1.5f;
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


void Camera::processMouseMovement( float x_offset, float y_offset )
{
	// std::cout << "callback with offset " << x_offset << ", " << y_offset << std::endl;
	_yaw += x_offset;
	_pitch += y_offset;

	if (_yaw > 180.0f) {
		_yaw -= 360.0f;
	} else if (_yaw < -180.0f) {
		_yaw += 360.0f;
	}
	if (_pitch > 85.0f || _pitch < -85.0f) {
		_pitch -= y_offset;
	}

	updateCameraVectors();
}
/*
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

std::string Camera::getCamString( void )
{
	return ("\nPos > x: " + std::to_string(_position.x)
				+ " y: " + std::to_string(_position.y) + " z: " + std::to_string(_position.z)
				+ "\nSpeed > " + std::to_string(_movement_speed)
				+ "\nFall > " + std::to_string(_fall_speed));
}
