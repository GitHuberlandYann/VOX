#include "vox.h"

Camera::Camera( glm::vec3 position )
	: _fall_time(0), _breathTime(0), _fov(FOV), _fall_speed(0), _isRunning(false), _healthUpdate(false),
	_waterHead(false), _waterFeet(false), _current_chunk_ptr(NULL), _movement_speed(SPEED), _health_points(20),
	_inJump(false), _touchGround(false), _fall_immunity(true), _fall_distance(0)
{
	_position = position;
	_world_up = glm::vec3(0.0f, 0.0f, 1.0f);
	_yaw = YAW;
	_pitch = PITCH;
	_update = false;
	_fovUpdate = false;
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
	_front2 = glm::normalize(glm::vec2(_front)); // used in chunkInFront
	_right2 = glm::vec2(glm::cos(glm::radians(_fov))) * glm::normalize(glm::vec2(_right));
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

 glm::mat4 Camera::getViewMatrix( void )
{
	_update = false;
	glm::vec3 pos = _position;
	pos.z += 1.0f + EYE_LEVEL;
	return (glm::lookAt(pos, pos + _front, _up));
}

glm::mat4 Camera::getPerspectiveMatrix( void )
{
	return (glm::perspective(glm::radians(_fov), (GLfloat)WIN_WIDTH / (GLfloat)WIN_HEIGHT, 0.1f, 1000.0f));
}

bool Camera::chunkInFront( glm::ivec2 current_chunk, int posX, int posY )
{
	if (posX >= current_chunk.x - CHUNK_SIZE && posX <= current_chunk.x + CHUNK_SIZE
		&& posY >= current_chunk.y - CHUNK_SIZE && posY <= current_chunk.y + CHUNK_SIZE) {
		return (true);
	}
	return !(glm::dot(glm::vec2(posX + CHUNK_SIZE * (_front2.x + _right2.x) - _position.x, posY + CHUNK_SIZE * (_front2.y + _right2.y) - _position.y), glm::vec2((_front2.x + _right2.x), (_front2.y + _right2.y))) < 0
		|| glm::dot(glm::vec2(posX + CHUNK_SIZE * (_front2.x - _right2.x) - _position.x, posY + CHUNK_SIZE * (_front2.y - _right2.y) - _position.y), glm::vec2((_front2.x - _right2.x), (_front2.y - _right2.y))) < 0);
}

// return orientation of block we just placed 0 = -y, 1 = +y, 2 = -x, 3 = +x (it is opposite of cam dir)
int Camera::getOrientation( void ) // if change this, then change face_dir in Chunk.hpp
{
	if (glm::abs(_front.x) > glm::abs(_front.y)) {
		return (2 + (_front.x < 0));
	}
	return (_front.y < 0);
}

glm::vec3 Camera::getPos( void )
{
	_mtx.lock();
	glm::vec3 res = _position;
	_mtx.unlock();
	return (res);
}

glm::vec3 Camera::getDir( void )
{
	return (_front);
}

void Camera::setPosZ( float value )
{
	_mtx.lock();
	_position.z = value;
	_mtx.unlock();
}

void Camera::setWaterStatus( bool head, bool underwater )
{
	int statusSave = getWaterStatus();
	if (head) {
		if (underwater) {
			_breathTime += _deltaTime;
			if (_breathTime > 16.0) {
				_breathTime -= 1;
				_health_points = glm::max(0, _health_points - 2);
				_healthUpdate = true;
			}
		} else {
			_breathTime -= _deltaTime * 2;
			if (_breathTime < 0) {
				_breathTime = 0;
			}
		}
		_waterHead = underwater;
	} else {
		_waterFeet = underwater;
	}
	if (getWaterStatus() != statusSave) {
		_healthUpdate = true;
	}
}

// computes value [0:20] representing amount of bubbles to display on ui
int Camera::getWaterStatus( void )
{
	if (_breathTime == 0 || _breathTime >= 15.0f) {
		return (0);
	}
	float percentage = _breathTime / 15.0f;
	int res = 20 - glm::floor(percentage * 20);
	if (res & 1) {
		res += (percentage * 10 - glm::floor(percentage * 10) < 0.9f);
	}
	return (res);
}

void Camera::setCurrentChunkPtr( Chunk *ptr )
{
	_current_chunk_ptr = ptr;
}

void Camera::setRun( bool value )
{
	_isRunning = value;
	float save_fov = _fov;
	if (_isRunning) {
		_fov += 1.0f;
		if (_fov > FOV + 2) {
			_fov = FOV + 2;
		}
	} else {
		_fov -= 1.0f;
		if (_fov < FOV) {
			_fov = FOV;
		}
	}
	_fovUpdate = save_fov != _fov;
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

void Camera::moveFly( GLint v, GLint h, GLint z )
{
	if (!v && !h && !z) {
		return ;
	}
	_update = true;
	float speed_frame = _deltaTime * _movement_speed * ((_isRunning) ? 2 : 1);
	glm::vec3 norm = glm::normalize(glm::vec3(v * _front.x + h * _right.x + z * _up.x,
												v * _front.y + h * _right.y + z * _up.y,
												v * _front.z + h * _right.z + z * _up.z));
	_mtx.lock();
	_position.x += norm.x * speed_frame;
	_position.y += norm.y * speed_frame;
	_position.z += norm.z * speed_frame;
	_mtx.unlock();
}

void Camera::moveHuman( Camera_Movement direction, GLint v, GLint h )
{
	if (direction == UP && _touchGround) {
		_update = true;
		_inJump = true;
		return ;
	} else if (direction == UP || direction == DOWN || (!v && !h)) {
		return ;
	}
	_update = true;

	float speed_frame = _deltaTime * ((_isRunning) ?  ((_touchGround) ? RUN_SPEED : RUN_JUMP_SPEED ) : WALK_SPEED);
	_mtx.lock();
	if (direction == X_AXIS)
		_position.x += glm::normalize(glm::vec2(v * _front.x + h * _right.x, v * _front.y + h * _right.y)).x * speed_frame;
	else if (direction == Y_AXIS)
		_position.y += glm::normalize(glm::vec2(v * _front.x + h * _right.x, v * _front.y + h * _right.y)).y * speed_frame;
	_mtx.unlock();
}

void Camera::applyGravity( void )
{
	// std::cout << "Gravity applied" << std::endl;
	_fall_time += _deltaTime;
	if (_inJump) {
		if (INITIAL_JUMP + STANDARD_GRAVITY * PLAYER_MASS * _fall_time * _fall_time < FALL_SPEED) {
			_fall_speed = INITIAL_JUMP + STANDARD_GRAVITY * PLAYER_MASS * _fall_time * _fall_time;
		}
	} else if (INITIAL_FALL + STANDARD_GRAVITY * PLAYER_MASS * _fall_time * _fall_time < FALL_SPEED) {
		_fall_speed = INITIAL_FALL + STANDARD_GRAVITY * PLAYER_MASS * _fall_time * _fall_time;
	}

	_mtx.lock();
	_position.z -= _deltaTime * _fall_speed;
	_mtx.unlock();
	_fall_distance += _deltaTime * _fall_speed;
}

void Camera::touchGround( void )
{
	// std::cout << "TOUCH GROUND AT " << _fall_speed << std::endl;
	if (_inJump && _fall_speed < 0) {
		return ;
	}
	if (!_touchGround) {
		_update = true;
	}
	if (_fall_immunity) {
		_fall_immunity = false;
	} else {
		_healthUpdate = (glm::max(0.0f, _fall_distance - 3) != 0);
		_health_points -= glm::max(0.0f, _fall_distance - 3);
		if (_health_points < 0) {
			_health_points = 0;
		}
	}
	_fall_time = 0;
	_fall_distance = 0;
	_touchGround = true;
	_inJump = false;
}

bool Camera::getModif( void )
{
	bool res = _healthUpdate;
	_healthUpdate = false;
	return (res);
}

void Camera::processPitch( GLint offset )
{
	_pitch += offset * _deltaTime * _movement_speed * 1.5f;
	if (_pitch > 89.0f || _pitch < -89.0f) {
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
	_mtx.lock();
	glm::vec3 pos = _position;
	_mtx.unlock();
	pos.z += 1.0f + EYE_LEVEL;
	return (voxel_traversal(pos, pos + _front * radius));
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
	if (_pitch > 89.0f || _pitch < -89.0f) {
		_pitch -= y_offset;
	}

	updateCameraVectors();
}

std::string Camera::getCamString( bool game_mode )
{
	const std::string strtrue = "TRUE";
	const std::string strfalse = "FALSE";
	_mtx.lock();
	std::string str =  ((game_mode == CREATIVE)
		? ("\nPos\t\t> x: " + std::to_string(_position.x)
				+ " y: " + std::to_string(_position.y) + " z: " + std::to_string(_position.z)
				+ "\niPos\t> x:" + std::to_string(_current_block.x) // TODO might sometimes want to use current_block instea of _position when dealing with chunks
				+ " y: " + std::to_string(_current_block.y) + " z: " + std::to_string(_current_block.z)
				+ "\nDir\t\t> x: " + std::to_string(_front.x)
				+ " y: " + std::to_string(_front.y) + " z: " + std::to_string(_front.z)
				+ "\nSpeed\t> " + ((_isRunning) ? std::to_string(_movement_speed * 2) : std::to_string(_movement_speed)))
				+ ((_current_chunk_ptr) 
					? "\nSky Light\t> " + std::to_string(_current_chunk_ptr->getSkyLightLevel(_current_block))
						+ "\nBlock Light\t> " + std::to_string(_current_chunk_ptr->getBlockLightLevel(_current_block)) : "")
		: ("\nPos\t\t> x: " + std::to_string(_position.x)
			+ " y: " + std::to_string(_position.y) + " z: " + std::to_string(_position.z)
			+ "\niPos\t> x:" + std::to_string(_current_block.x)
				+ " y: " + std::to_string(_current_block.y) + " z: " + std::to_string(_current_block.z)
			+ "\nDir\t\t> x: " + std::to_string(_front.x)
			+ " y: " + std::to_string(_front.y) + " z: " + std::to_string(_front.z)
			+ "\nSpeed\t> " + ((_isRunning) ?  ((_touchGround) ? std::to_string(RUN_SPEED) : std::to_string(RUN_JUMP_SPEED) ) : std::to_string(WALK_SPEED))
			+ ((_current_chunk_ptr) 
				? "\nSky Light\t> " + std::to_string(_current_chunk_ptr->getSkyLightLevel(_current_block))
					+ "\nBlock Light\t> " + std::to_string(_current_chunk_ptr->getBlockLightLevel(_current_block)) : "")
			+ "\nFall\t> " + std::to_string(_fall_speed)
			+ "\nFall Distance\t> " + std::to_string(_fall_distance)
			+ "\nWater head\t> " + ((_waterHead) ? strtrue : strfalse)
			+ ((_breathTime > 0) ? " - " + std::to_string(_breathTime) + ": " + std::to_string(getWaterStatus()) : "")
			+ "\nWater feet\t> " + ((_waterFeet) ? strtrue : strfalse)
			+ "\nHealth\t> " + std::to_string(_health_points)
			+ "\nGounded\t> " + ((_touchGround) ? strtrue : strfalse)
			+ "\nJumping\t> " + ((_inJump) ? strtrue : strfalse)));
	_mtx.unlock();
	return (str);
}

void Camera::respawn( void )
{
	_fall_time = 0;
	_breathTime = 0;
	// _fov(FOV)
	_fall_speed = 0;
	_isRunning = false;
	_healthUpdate = true;
	_waterHead = false;
	_waterFeet = false;
	_current_chunk_ptr = NULL;
	_movement_speed = SPEED;
	_health_points = 20;
	_inJump = false;
	_touchGround = false;
	_fall_immunity = true;
	_fall_distance = 0;

	_position = {1.0f, -2.0f, 66.0f}; // TODO respawn in bed or at default position
	_yaw = YAW;
	_pitch = PITCH;
	_update = false;
	_fovUpdate = false;
	updateCameraVectors();
}
