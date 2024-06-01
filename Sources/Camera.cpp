#include "Camera.hpp"
#include "Settings.hpp"
#include "utils.h"
extern std::mutex mtx;

Camera::Camera( glm::vec3 position )
	: _fall_time(0), _walk_time(0), _breathTime(0), _armAnimTime(0), _fov_offset(0), _fall_distance(0),
	_foodTickTimer(0), _camPlacement(CAMPLACEMENT::DEFAULT), _foodExhaustionLevel(0), _smoothCamZ(0), _z0(position.z), _fall_immunity(true),
	_walking(false), _sprinting(false), _sneaking(false), _smoothCam(false), _healthUpdate(false),
	_waterHead(false), _waterFeet(false), _armAnimation(false), _hideUI(false), _current_chunk_ptr(NULL), _movement_speed(FLY_SPEED), _health_points(20), _foodLevel(20),
	_inJump(false), _touchGround(false), _foodSaturationLevel(20)
{
	_position = position;
	_spawnpoint = position;
	_lastTp = {0, 0, 0};
	_world_up = glm::vec3(0.0f, 0.0f, 1.0f);
	_yaw = YAW;
	_pitch = PITCH;
	_update = false;
	_fovUpdate = false;
	updateCameraVectors();
	_bodyFront = _front;
}

Camera::~Camera( void )
{
	std::cout << "Destructor of Camera called" << std::endl;
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
	_right2 = glm::vec2(glm::cos(glm::radians(Settings::Get()->getFloat(SETTINGS::FLOAT::FOV) + _fov_offset))) * glm::normalize(glm::vec2(_right));
}

void Camera::moveHumanUnderwater( Camera_Movement direction, GLint v, GLint h, GLint z )
{
	if (!v && !h) {
		if (direction == Z_AXIS) {
			_inJump = z == 1;
			_sneaking = z == -1;
		}
		return ;
	}
	_update = true;

	float speed_frame = _deltaTime * ((_sprinting) ?  SPRINT_SPEED : SWIM_SPEED);
	float movement = 0;
	_mtx.lock();
	if (direction == X_AXIS) {
		movement = glm::normalize(glm::vec3(v * _front.x + h * _right.x, v * _front.y + h * _right.y, v * _front.z + h * _right.z)).x * speed_frame;
		_position.x += movement;
	}
	else if (direction == Y_AXIS) {
		movement = glm::normalize(glm::vec3(v * _front.x + h * _right.x, v * _front.y + h * _right.y, v * _front.z + h * _right.z)).y * speed_frame;
		_position.y += movement;
	} else {
		_inJump = z == 1;
		_sneaking = z == -1;
		movement = glm::normalize(glm::vec3(v * _front.x + h * _right.x, v * _front.y + h * _right.y, v * _front.z + h * _right.z)).z * speed_frame;
		_position.z += movement;
		mtx.lock();
		if (_current_chunk_ptr->collisionBox(_position, 0.3f, getHitBox(), getHitBox()).type == COLLISION::TOTAL) { // TODO handle partial collision underwater
			_position.z -= movement;
		}
		mtx.unlock();
	}
	_mtx.unlock();
	updateExhaustion(glm::abs(movement * EXHAUSTION_SWIMMING));
	if (_sprinting && movement != 0) {
		updateExhaustion(glm::abs(movement * EXHAUSTION_SPRINTING));
	}
}

void Camera::applyGravityUnderwater( void )
{
	float speed_frame = _deltaTime * ((_inJump) ? ((_waterHead) ? SWIM_UP_SPEED : SWIM_DOWN_SPEED) : ((_sneaking) ? -SWIM_DOWN_SPEED : -SWIM_UP_SPEED));

	_mtx.lock();
	_position.z += speed_frame;
	_mtx.unlock();
	_z0 = _position.z;
	_fall_time = 0;
	// std::cout << "gravity underwater " << speed_frame << ", " << _inJump << ", " << _sneaking << std::endl;
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

glm::mat4 Camera::getViewMatrix( void )
{
	_update = false;
	glm::vec3 pos = getEyePos();
	if (_smoothCam) {
		if (_smoothCamZ < pos.z) {
			pos.z = _smoothCamZ;
			_smoothCamZ += _deltaTime * SMOOTH_CAM_SPEED;
			_update = true;
		} else {
			_smoothCam = false;
		}
	}
	switch (_camPlacement) {
	case CAMPLACEMENT::DEFAULT:
		return (glm::lookAt(pos, pos + _front, _up));
	case CAMPLACEMENT::BEHIND:
		return (glm::lookAt(pos - _front * 5.0f, pos, _up));
	case CAMPLACEMENT::FRONT:
		return (glm::lookAt(pos + _front * 5.0f, pos, _up));
	}
	return (glm::lookAt(pos, pos + _front, _up));
}

glm::mat4 Camera::getPerspectiveMatrix( void )
{
	return (glm::perspective(glm::radians(Settings::Get()->getFloat(SETTINGS::FLOAT::FOV) + _fov_offset), (GLfloat)WIN_WIDTH / (GLfloat)WIN_HEIGHT, 0.1f, 1000.0f));
}

bool Camera::chunkInFront( glm::ivec2 current_chunk, int posX, int posY )
{
	if (posX >= current_chunk.x - CHUNK_SIZE && posX <= current_chunk.x + CHUNK_SIZE
		&& posY >= current_chunk.y - CHUNK_SIZE && posY <= current_chunk.y + CHUNK_SIZE) {
		return (true);
	}
	if (_camPlacement == CAMPLACEMENT::FRONT) {
		return !(glm::dot(glm::vec2(posX + CHUNK_SIZE * (-_front2.x + _right2.x) - _position.x, posY + CHUNK_SIZE * (-_front2.y + _right2.y) - _position.y), glm::vec2((-_front2.x + _right2.x), (-_front2.y + _right2.y))) < 0
			|| glm::dot(glm::vec2(posX + CHUNK_SIZE * (-_front2.x - _right2.x) - _position.x, posY + CHUNK_SIZE * (-_front2.y - _right2.y) - _position.y), glm::vec2((-_front2.x - _right2.x), (-_front2.y - _right2.y))) < 0);
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

/**
 * @brief same as ::getOrientation, but also return PLUSZ/MINUSZ
*/
int Camera::getOrientation6( void )
{
	if (glm::abs(_front.x) > glm::abs(_front.y)) {
		if (glm::abs(_front.z) > glm::abs(_front.x)) {
			return ((_front.z < 0) ? face_dir::PLUSZ : face_dir::MINUSZ);
		}
		return ((_front.x < 0) ? face_dir::PLUSX : face_dir::MINUSX);
	}
	if (glm::abs(_front.z) > glm::abs(_front.y)) {
		return ((_front.z < 0) ? face_dir::PLUSZ : face_dir::MINUSZ);
	}
	return ((_front.y < 0) ? face_dir::PLUSY : face_dir::MINUSY);
}

glm::vec3 Camera::getPos( void )
{
	_mtx.lock();
	glm::vec3 res = _position;
	_mtx.unlock();
	return (res);
}

glm::vec3 Camera::getEyePos( void )
{
	_mtx.lock();
	glm::vec3 res = _position;
	_mtx.unlock();
	res.z += 1 + ((_sneaking) ? SNEAK_EYE_LEVEL : EYE_LEVEL);
	return (res);
}

glm::vec3 Camera::getCamPos( void )
{
	glm::vec3 pos = getEyePos();
	if (_smoothCam) {
		pos.z = _smoothCamZ - _deltaTime * SMOOTH_CAM_SPEED;
	}
	switch (_camPlacement) {
	case CAMPLACEMENT::DEFAULT:
		return (pos);
	case CAMPLACEMENT::BEHIND:
		return (pos - _front * 5.0f);
	case CAMPLACEMENT::FRONT:
		return (pos + _front * 5.0f);
	}
	return (pos);
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

void Camera::setHideUI( bool value )
{
	_hideUI = value;
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

bool Camera::isUnderwater( void )
{
	return (_waterHead);
}

void Camera::setCurrentChunkPtr( Chunk *ptr )
{
	_current_chunk_ptr = ptr;
}

void Camera::changeCamPlacement( void )
{
	const int next[3] = {CAMPLACEMENT::BEHIND, CAMPLACEMENT::FRONT, CAMPLACEMENT::DEFAULT};
	_camPlacement = next[_camPlacement];
	_update = true;
}

void Camera::setRun( bool value )
{
	_sprinting = value;
	if (_foodLevel <= 6) { // can't sprint when out of energy
		_sprinting = false;
	}
	if (_sprinting) {
		_fov_offset += 1.0f;
		if (_fov_offset > 2) {
			_fov_offset = 2;
		} else {
			_fovUpdate = true;
		}
	} else {
		_fov_offset -= 1.0f;
		if (_fov_offset < 0) {
			_fov_offset = 0;
		} else {
			_fovUpdate = true;
		}
	}
}

void Camera::setSneak( bool value )
{
	if (value != _sneaking) {
		_sneaking = value;
		if (value == false) {
			mtx.lock();
			if (_current_chunk_ptr->collisionBox(_position, 0.3f, getHitBox(), getHitBox()).type != COLLISION::NONE) {
				_sneaking = true;
				mtx.unlock();
				return ;
			}
			mtx.unlock();
		}
		_update = true;
	}
}

void Camera::setJump( bool value )
{
	if (value && _touchGround) {
		_update = true;
		_inJump = true;
		updateExhaustion((_sprinting) ? EXHAUSTION_SPRINT_JUMP : EXHAUSTION_JUMP);
	}
}

void Camera::setArmAnimation( bool state )
{
	if (state) {
		_armAnimation = true;
		_armAnimTime += _deltaTime;
		if (_armAnimTime > 0.35) {
			_armAnimTime -= 0.35;
		}
	} else if (_armAnimTime > 0.35) {
		_armAnimation = 0;
		_armAnimTime = 0;
	} else if (_armAnimTime > 0) {
		_armAnimTime += _deltaTime;
	}
}

void Camera::setDelta( float deltaTime )
{
	// _speed_frame = _movement_speed * deltaTime;
	_deltaTime = deltaTime;
	if (_walking) {
		_walk_time += deltaTime;
	} else {
		_walk_time = 0;
	}
}

void Camera::update_movement_speed( GLint key_cam_speed )
{
	if (_movement_speed + key_cam_speed >= 0) {
		_movement_speed += key_cam_speed;
	}
}

void Camera::tickUpdate( void )
{
	++_foodTickTimer;
	if (_health_points < 20 && (_foodTickTimer == 10 || _foodTickTimer == 20 || _foodTickTimer == 30 || _foodTickTimer == 40
		|| _foodTickTimer == 50 || _foodTickTimer == 60 || _foodTickTimer == 70)
		&& _foodLevel == 20 && _foodSaturationLevel) {
		++_health_points;
		updateExhaustion(EXHAUSTION_REGEN);
		_healthUpdate = true;
	}
	if (_foodTickTimer == 80) {
		if (_health_points < 20 && _foodLevel >= 18) {
			++_health_points;
			updateExhaustion(EXHAUSTION_REGEN);
			_healthUpdate = true;
		} else if (_foodLevel == 0) {
			if (--_health_points < 0) {
				_health_points = 0;
			} else {
				_healthUpdate = true;
			}
		}
		_foodTickTimer = 0;
	}
}

void Camera::updateExhaustion( float level )
{
	_foodExhaustionLevel += level;
	if (_foodExhaustionLevel >= 4) {
		if (_foodSaturationLevel) {
			--_foodSaturationLevel;
		} else if (_foodLevel) {
			--_foodLevel;
		}
		_healthUpdate = true;
		_foodExhaustionLevel -= 4;
	}
}

bool Camera::canEatFood( int type )
{
	if (_foodLevel == 20 && _foodSaturationLevel == 20) {
		return (false);
	}
	_foodLevel = glm::min(20, _foodLevel + s_blocks[type]->hunger_restauration);
	_foodSaturationLevel = glm::min(static_cast<float>(_foodLevel), _foodSaturationLevel + s_blocks[type]->saturation_restauration);
	_healthUpdate = true;
	return (true);
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
	glm::vec3 pos = getEyePos();
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
	if (_pitch > 89.9f || _pitch < -89.9f) {
		_pitch -= y_offset;
	}

	updateCameraVectors();
}

std::string Camera::getCamString( int game_mode )
{
	const std::string strtrue = "TRUE";
	const std::string strfalse = "FALSE";
	_mtx.lock();
	int light = (_current_chunk_ptr) ? _current_chunk_ptr->getCamLightLevel(_current_block) : 0;
	std::string str =  ((game_mode == GAMEMODE::CREATIVE)
		? "\nPos\t\t> x: " + std::to_string(_position.x)
				+ " y: " + std::to_string(_position.y) + " z: " + std::to_string(_position.z)
				+ "\niPos\t> x:" + std::to_string(_current_block.x) // TODO might sometimes want to use current_block instea of _position when dealing with chunks
					+ " y: " + std::to_string(_current_block.y) + " z: " + std::to_string(_current_block.z)
				+ "\nDir\t\t> x: " + std::to_string(_front.x)
					+ " y: " + std::to_string(_front.y) + " z: " + std::to_string(_front.z)
				+ "\nSpeed\t> " + ((_sprinting) ? std::to_string(_movement_speed * 2) : std::to_string(_movement_speed))
				+ ((_current_chunk_ptr) 
					? "\nSky Light\t> " + std::to_string(light >> 8)
						+ "\nBlock Light\t> " + std::to_string(light & 0xFF) : "\n\n")
				+ "\n\n\n\n\n\n\n\n\n\n\n"
		: "\nPos\t\t> x: " + std::to_string(_position.x)
			+ " y: " + std::to_string(_position.y) + " z: " + std::to_string(_position.z)
			+ "\niPos\t> x:" + std::to_string(_current_block.x)
				+ " y: " + std::to_string(_current_block.y) + " z: " + std::to_string(_current_block.z)
			+ "\nDir\t\t> x: " + std::to_string(_front.x)
				+ " y: " + std::to_string(_front.y) + " z: " + std::to_string(_front.z)
			+ "\nSpeed\t> " + ((_sprinting) ?  ((_touchGround) ? std::to_string(SPRINT_SPEED) : std::to_string(SPRINT_JUMP_SPEED) ) : std::to_string(WALK_SPEED))
			+ ((_current_chunk_ptr) 
				? "\nSky Light\t> " + std::to_string(light >> 8)
					+ "\nBlock Light\t> " + std::to_string(light & 0xFF) : "")
			+ "\nFall Time\t> " + std::to_string(_fall_time)
			+ "\nFall Distance\t> " + std::to_string(_fall_distance)
			+ "\nWater head\t> " + ((_waterHead) ? strtrue : strfalse)
			+ ((_breathTime > 0) ? " - " + std::to_string(_breathTime) + ": " + std::to_string(getWaterStatus()) : "")
			+ "\nWater feet\t> " + ((_waterFeet) ? strtrue : strfalse)
			+ "\nHealth\t> " + std::to_string(_health_points)
			+ "\nFood\tLevel\t> " + std::to_string(_foodLevel)
			+ "\n\t\tSaturation\t> " + std::to_string(_foodSaturationLevel)
			+ "\n\t\tExhaustion\t> " + std::to_string(_foodExhaustionLevel)
			+ "\nGounded\t> " + ((_touchGround) ? strtrue : strfalse)
			+ "\nJumping\t> " + ((_inJump) ? strtrue : strfalse)
			+ "\nSneaking\t> " + ((_sneaking) ? strtrue : strfalse));
	_mtx.unlock();
	return (str);
}

// called upon /tp
void Camera::setPos( glm::vec3 pos )
{
	_mtx.lock();
	_lastTp = _position;
	_position = pos;
	_mtx.unlock();
	_fall_immunity = true;
	_z0 = pos.z;
	_fall_time = 0;
	_update = true;
	_smoothCam = false;
	_current_chunk_ptr = NULL;
}

void Camera::setSpawnpoint( glm::vec3 spawnpoint )
{
	_spawnpoint = spawnpoint;
}

glm::vec3 Camera::getSpawnpoint( void )
{
	return (_spawnpoint);
}

glm::vec3 Camera::getLastTp( void )
{
	return (_lastTp);
}

void Camera::respawn( void )
{
	_fall_time = 0;
	_breathTime = 0;
	_z0 = _spawnpoint.z;
	_sprinting = false;
	_sneaking = false;
	_smoothCam = false;
	_healthUpdate = true;
	_waterHead = false;
	_waterFeet = false;
	_current_chunk_ptr = NULL;
	_movement_speed = FLY_SPEED;
	_health_points = 20;
	_foodLevel = 20;
	_foodSaturationLevel = 20;
	_foodTickTimer = 0;
	_foodExhaustionLevel = 0;
	_inJump = false;
	_touchGround = false;
	_fall_immunity = true;
	_fall_distance = 0;

	_mtx.lock();
	_position = _spawnpoint; // {1.0f, -2.0f, 66.0f};
	_mtx.unlock();
	_yaw = YAW;
	_pitch = PITCH;
	_update = false;
	_fovUpdate = false;
	updateCameraVectors();
}
