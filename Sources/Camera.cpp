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

float Camera::getHitBox( void )
{
	return ((_sneaking) ? 1.5f : 1.8f);
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

void Camera::moveFly( GLint v, GLint h, GLint z )
{
	if (!v && !h && !z) {
		return ;
	}
	_update = true;
	float speed_frame = _deltaTime * _movement_speed * ((_sprinting) ? 2 : 1);
	glm::vec3 norm = glm::normalize(glm::vec3(v * _front.x + h * _right.x + z * _up.x,
												v * _front.y + h * _right.y + z * _up.y,
												v * _front.z + h * _right.z + z * _up.z));
	_mtx.lock();
	_position.x += norm.x * speed_frame;
	_position.y += norm.y * speed_frame;
	_position.z += norm.z * speed_frame;
	_mtx.unlock();
}

void Camera::moveHuman( Camera_Movement direction, GLint v, GLint h, GLint z )
{
	if (_waterFeet || _waterHead) {
		return (moveHumanUnderwater(direction, v, h, z));
	} else if (direction == Z_AXIS) {
		return ;
	} else if (!v && !h) {
		_walking = false;
		return ;
	}
	_update = true;
	_walking = true;
	_bodyFront = _front;

	float speed_frame = _deltaTime * ((_sneaking) ? ((_sprinting) ? SNEAK_SPRINT_SPEED : SNEAK_SPEED) : ((_sprinting) ?  ((_touchGround) ? SPRINT_SPEED : SPRINT_JUMP_SPEED ) : WALK_SPEED));
	float movement = 0;
	_mtx.lock();
	if (direction == X_AXIS) {
		movement = glm::normalize(glm::vec2(v * _front.x + h * _right.x, v * _front.y + h * _right.y)).x * speed_frame;
		_position.x += movement;
	}
	else if (direction == Y_AXIS) {
		movement = glm::normalize(glm::vec2(v * _front.x + h * _right.x, v * _front.y + h * _right.y)).y * speed_frame;
		_position.y += movement;
	}
	_mtx.unlock();
	if (_sprinting && movement != 0) {
		updateExhaustion(glm::abs(movement * EXHAUSTION_SPRINTING));
	}
}

// if collision after movement, restore position
void Camera::unmoveHuman( glm::vec3 pos )
{
	_mtx.lock();
	_position = pos;
	_mtx.unlock();
	setRun(false);
}

// adjust position.z to given obstacle, if player can't pass obstacle return false
bool Camera::customObstacle( float minZ, float maxZ )
{
	(void)minZ;
	if (!_touchGround) return (false);

	_mtx.lock();
	// std::cout << "DEBUG customObstacle " << _position.z << " vs " << maxZ << std::endl;
	if (_position.z < maxZ && _position.z + 0.6f > maxZ) {
		// set smoothcam to true to have smoother transition upon climbing stairs
		// ie player will teleport, but cam will follow smoothly to enhance user experience
		_smoothCam = true;
		_smoothCamZ = _position.z + 1 + ((_sneaking) ? SNEAK_EYE_LEVEL : EYE_LEVEL);
		_position.z = maxZ;
		_mtx.unlock();
		return (true);
	}
	_mtx.unlock();
	return (false);
}

void Camera::resetFall( void )
{
	_fall_immunity = true;
	_z0 = getPos().z;
	_fall_time = 0;
	_inJump = false;
}

// z = z0 + vt + at² / 2
void Camera::applyGravity( void )
{
	if (_waterFeet || _waterHead) {
		return (applyGravityUnderwater());
	}
	// std::cout << "Gravity applied" << std::endl;
	_fall_time += _deltaTime;
	float initial_speed = ((_inJump) ? INITIAL_JUMP : INITIAL_FALL);

	_mtx.lock();
	_position.z = _z0 + initial_speed * _fall_time + 3 * STANDARD_GRAVITY * _fall_time * _fall_time * 0.5f; // TODO this '3' is a bit random ...
	_mtx.unlock();
}

void Camera::touchGround( float value )
{
	_mtx.lock();
	_position.z = value;
	_mtx.unlock();
	_fall_distance = _z0 - _position.z;
	// std::cout << "TOUCH GROUND AT " << _fall_distance << std::endl;
	// if (_inJump && _fall_distance < 0) {
	// 	return ;
	// }
	if (!_touchGround) {
		_update = true;
	}
	if (_fall_immunity) {
		_fall_immunity = false;
	} else if (_waterFeet || _waterHead) {
	} else if (_fall_distance > 3) { // TODO call this function AFTER setting waterFeet
		_healthUpdate = (glm::max(0.0f, _fall_distance - 3) != 0);
		_health_points -= glm::max(0.0f, _fall_distance - 3);
		if (_health_points < 0) {
			_health_points = 0;
		}
	}
	_fall_time = 0;
	_z0 = _position.z;
	_touchGround = true;
	_inJump = false;
}

void Camera::touchCeiling( float value )
{
	_mtx.lock();
	_position.z = value;
	_mtx.unlock();
	_z0 = value;
	_fall_time = 0;
	_update = true;
	_inJump = false;
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

std::string Camera::getCamString( bool game_mode )
{
	const std::string strtrue = "TRUE";
	const std::string strfalse = "FALSE";
	_mtx.lock();
	int light = (_current_chunk_ptr) ? _current_chunk_ptr->getCamLightLevel(_current_block) : 0;
	std::string str =  ((game_mode == CREATIVE)
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
