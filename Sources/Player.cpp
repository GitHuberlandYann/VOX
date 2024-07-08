#include "Player.hpp"
#include "Camera.hpp"
#include "logs.hpp"

Player::Player( void )
    : AMob({0.0f, 0.0f, 0.0f}), _flySpeed(settings::consts::speed::fly), _foodLevel(20), _foodTickTimer(0),
    _yaw(settings::defaults::yaw), _pitch(settings::defaults::pitch),
    _smoothCamZ(0.0f), _fovOffset(0.0f), _armAnimTime(0.0f), _breathTime(0.0f), _foodSaturationLevel(20.0f), _foodExhaustionLevel(0.0f),
    _spawnpoint(0.0f, 0.0f, 0.0f), _lastTp(0.0f, 0.0f, 0.0f),
    _smoothCam(false), _armAnimation(false), _fallImmunity(false), _sprinting(false), _sneaking(false), _waterHead(false), _waterFeet(false),
    _updateCam(false), _updateFov(false), _updateUI(false)
{
	_type = settings::consts::mob::player;
}

Player::~Player( void )
{

}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

bool Player::chunkInFront( int camPlacement, glm::ivec2 current_chunk, int posX, int posY )
{
	if (posX >= current_chunk.x - settings::consts::chunk_size && posX <= current_chunk.x + settings::consts::chunk_size
		&& posY >= current_chunk.y - settings::consts::chunk_size && posY <= current_chunk.y + settings::consts::chunk_size) {
		return (true);
	}
	if (camPlacement == CAMPLACEMENT::FRONT) {
		return !(glm::dot(glm::vec2(posX + settings::consts::chunk_size * (-_front2.x + _right2.x) - _position.x, posY + settings::consts::chunk_size * (-_front2.y + _right2.y) - _position.y), glm::vec2((-_front2.x + _right2.x), (-_front2.y + _right2.y))) < 0
			|| glm::dot(glm::vec2(posX + settings::consts::chunk_size * (-_front2.x - _right2.x) - _position.x, posY + settings::consts::chunk_size * (-_front2.y - _right2.y) - _position.y), glm::vec2((-_front2.x - _right2.x), (-_front2.y - _right2.y))) < 0);
	}
	return !(glm::dot(glm::vec2(posX + settings::consts::chunk_size * (_front2.x + _right2.x) - _position.x, posY + settings::consts::chunk_size * (_front2.y + _right2.y) - _position.y), glm::vec2((_front2.x + _right2.x), (_front2.y + _right2.y))) < 0
		|| glm::dot(glm::vec2(posX + settings::consts::chunk_size * (_front2.x - _right2.x) - _position.x, posY + settings::consts::chunk_size * (_front2.y - _right2.y) - _position.y), glm::vec2((_front2.x - _right2.x), (_front2.y - _right2.y))) < 0);
}

std::vector<glm::ivec3> Player::computeRayCasting( GLfloat radius )
{
	glm::vec3 pos = getEyePos();
	return (utils::math::voxel_traversal(pos, pos + _front * radius));
}

glm::vec3 Player::getEyePos( void )
{
	glm::vec3 res = _position;
    if (_smoothCam) {
		res.z = _smoothCamZ;
	}
	res.z += 1 + ((_sneaking) ? settings::consts::eyeLevel::player_sneak : settings::consts::eyeLevel::player);
	return (res);
}

float Player::getHitbox( void )
{
	return ((_sneaking) ? settings::consts::hitbox::player_sneak : settings::consts::hitbox::player);
}


// return orientation of block we just placed 0 = -y, 1 = +y, 2 = -x, 3 = +x (it is opposite of player dir)
int Player::getOrientation( void ) // if change this, then change face_dir in Chunk.hpp
{
	if (glm::abs(_front.x) > glm::abs(_front.y)) {
		return (2 + (_front.x < 0));
	}
	return (_front.y < 0);
}

/**
 * @brief same as ::getOrientation, but also return plus_z/minus_z
*/
int Player::getOrientation6( void )
{
	if (glm::abs(_front.x) > glm::abs(_front.y)) {
		if (glm::abs(_front.z) > glm::abs(_front.x)) {
			return ((_front.z < 0) ? face_dir::plus_z : face_dir::minus_z);
		}
		return ((_front.x < 0) ? face_dir::plus_x : face_dir::minus_x);
	}
	if (glm::abs(_front.z) > glm::abs(_front.y)) {
		return ((_front.z < 0) ? face_dir::plus_z : face_dir::minus_z);
	}
	return ((_front.y < 0) ? face_dir::plus_y : face_dir::minus_y);
}

void Player::setWaterStatus( bool head, bool underwater )
{
	int statusSave = getWaterStatus();
	if (head) {
		if (underwater) {
			_breathTime += _deltaTime;
			if (_breathTime > 16.0) {
				_breathTime -= 1;
				_health = glm::max(0.0f, _health - 2.0f);
				_updateUI = true;
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
		_updateUI = true;
	}
}

// computes value [0:20] representing amount of bubbles to display on ui
int Player::getWaterStatus( void )
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

bool Player::isUnderwater( void )
{
	return (_waterHead);
}

bool Player::canEatFood( int type )
{
	if (_foodLevel == 20 && _foodSaturationLevel == 20) {
		return (false);
	}
	_foodLevel = glm::min(20, _foodLevel + s_blocks[type]->hunger_restauration);
	_foodSaturationLevel = glm::min(static_cast<float>(_foodLevel), _foodSaturationLevel + s_blocks[type]->saturation_restauration);
	_updateUI = true;
	return (true);
}

void Player::setRun( bool state )
{
	_sprinting = state;
	if (_foodLevel <= 6) { // can't sprint when out of energy
		_sprinting = false;
	}
	if (_sprinting) {
		_fovOffset += 1.0f;
		if (_fovOffset > 2) {
			_fovOffset = 2;
		} else {
			_updateFov = true;
		}
	} else {
		_fovOffset -= 1.0f;
		if (_fovOffset < 0) {
			_fovOffset = 0;
		} else {
			_updateFov = true;
		}
	}
}

void Player::setSneak( bool state )
{
	if (state != _sneaking) {
		_sneaking = state;
		if (state == false) {
			if (_chunk->collisionBox(_position, 0.3f, getHitbox(), getHitbox()).type != COLLISION::NONE) { // keep sneaking no place to stand up
				_sneaking = true;
				return ;
			}
		}
		_updateCam = true;
	}
}

void Player::setJump( bool state )
{
	if (state && _touchGround) {
		_updateCam = true;
		_inJump = true;
		updateExhaustion((_sprinting) ? settings::consts::exhaustion::sprint_jump : settings::consts::exhaustion::jump);
	}
}

void Player::setDelta( float deltaTime )
{
	_deltaTime = deltaTime;
	if (_walking) {
		_walkTime += deltaTime;
	} else {
		_walkTime = 0;
	}
}

void Player::setArmAnimation( bool state )
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

void Player::setCamUpdate( bool state )
{
    _updateCam = state;
}

void Player::setUIUpdate( bool state )
{
	_updateUI = state;
}

float Player::getFovOffset( void )
{
    return (_fovOffset);
}

int Player::getFoodLevel( void )
{
    return (_foodLevel);
}

float Player::getFoodSaturationLevel( void )
{
    return (_foodSaturationLevel);
}

bool Player::getCamUpdate( void )
{
    return (_updateCam);
}

bool Player::getResetFovUpdate( void )
{
    bool res = _updateFov;
    _updateFov = false;
    return (res);
}

bool Player::getResetUIUpdate( void )
{
	bool res = _updateUI;
	_updateUI = false;
	return (res);
}

void Player::updateFlySpeed( GLint key_cam_speed )
{
	if (_flySpeed + key_cam_speed >= 0) {
		_flySpeed += key_cam_speed;
	}
}

void Player::updateExhaustion( float level )
{
	_foodExhaustionLevel += level;
	if (_foodExhaustionLevel >= 4) {
		if (_foodSaturationLevel) {
			--_foodSaturationLevel;
		} else if (_foodLevel) {
			--_foodLevel;
		}
		_updateUI = true;
		_foodExhaustionLevel -= 4;
	}
}

void Player::tickUpdate( void )
{
	++_foodTickTimer;
	if (_health < 20 && (_foodTickTimer == 10 || _foodTickTimer == 20 || _foodTickTimer == 30 || _foodTickTimer == 40
		|| _foodTickTimer == 50 || _foodTickTimer == 60 || _foodTickTimer == 70)
		&& _foodLevel == 20 && _foodSaturationLevel) {
		++_health;
		updateExhaustion(settings::consts::exhaustion::regen);
		_updateUI = true;
	}
	if (_foodTickTimer == 80) {
		if (_health < 20 && _foodLevel >= 18) {
			++_health;
			updateExhaustion(settings::consts::exhaustion::regen);
			_updateUI = true;
		} else if (_foodLevel == 0) {
			if (--_health < 0) {
				_health = 0;
			} else {
				_updateUI = true;
			}
		}
		_foodTickTimer = 0;
	}
}

void Player::processMouseMovement( float x_offset, float y_offset )
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

	updateVectors();
}

void Player::setChunkPtr( std::shared_ptr<Chunk> chunk )
{
    _chunk = chunk;
}

std::string Player::getString( int game_mode )
{
	const std::string strtrue = "TRUE";
	const std::string strfalse = "FALSE";
	int light = (_chunk) ? _chunk->getCamLightLevel(_currentBlock) : 0;
	std::string str =  ((game_mode == settings::consts::gamemode::creative)
		? "\nPos\t\t> x: " + std::to_string(_position.x)
				+ " y: " + std::to_string(_position.y) + " z: " + std::to_string(_position.z)
				+ "\niPos\t> x:" + std::to_string(_currentBlock.x) // TODO might sometimes want to use current_block instea of _position when dealing with chunks
					+ " y: " + std::to_string(_currentBlock.y) + " z: " + std::to_string(_currentBlock.z)
				+ "\nDir\t\t> x: " + std::to_string(_front.x)
					+ " y: " + std::to_string(_front.y) + " z: " + std::to_string(_front.z)
				+ "\nSpeed\t> " + ((_sprinting) ? std::to_string(_flySpeed * 2) : std::to_string(_flySpeed))
				+ ((_chunk) 
					? "\nSky Light\t> " + std::to_string((light >> 12) & 0xF) + " $ " + std::to_string((light >> 8) & 0xF)
						+ "\nBlock Light\t> " + std::to_string((light >> 4) & 0xF) + " $ " + std::to_string(light & 0xF) : "\n\n")
				+ "\n\n\n\n\n\n\n\n\n\n\n"
		: "\nPos\t\t> x: " + std::to_string(_position.x)
			+ " y: " + std::to_string(_position.y) + " z: " + std::to_string(_position.z)
			+ "\niPos\t> x:" + std::to_string(_currentBlock.x)
				+ " y: " + std::to_string(_currentBlock.y) + " z: " + std::to_string(_currentBlock.z)
			+ "\nDir\t\t> x: " + std::to_string(_front.x)
				+ " y: " + std::to_string(_front.y) + " z: " + std::to_string(_front.z)
			+ "\nSpeed\t> " + ((_sprinting) ?  ((_touchGround) ? std::to_string(settings::consts::speed::sprint) : std::to_string(settings::consts::speed::sprint_jump) ) : std::to_string(settings::consts::speed::walk))
			+ ((_chunk) 
				? "\nSky Light\t> " + std::to_string((light >> 12) & 0xF) + " $ " + std::to_string((light >> 8) & 0xF)
					+ "\nBlock Light\t> " + std::to_string((light >> 4) & 0xF) + " $ " + std::to_string(light & 0xF) : "")
			+ "\nFall Time\t> " + std::to_string(_fallTime)
			+ "\nFall Distance\t> " + std::to_string(_fallDistance)
			+ "\nWater head\t> " + ((_waterHead) ? strtrue : strfalse)
			+ ((_breathTime > 0) ? " - " + std::to_string(_breathTime) + ": " + std::to_string(getWaterStatus()) : "")
			+ "\nWater feet\t> " + ((_waterFeet) ? strtrue : strfalse)
			+ "\nHealth\t> " + std::to_string(_health)
			+ "\nKnockback\t> " + std::to_string(_knockback.x) + ", " + std::to_string(_knockback.y) + ", " + std::to_string(_knockback.z)
			+ "\nFood\tLevel\t> " + std::to_string(_foodLevel)
			+ "\n\t\tSaturation\t> " + std::to_string(_foodSaturationLevel)
			+ "\n\t\tExhaustion\t> " + std::to_string(_foodExhaustionLevel)
			+ "\nGounded\t> " + ((_touchGround) ? strtrue : strfalse)
			+ "\nJumping\t> " + ((_inJump) ? strtrue : strfalse)
			+ "\nSneaking\t> " + ((_sneaking) ? strtrue : strfalse));
	return (str);
}

// called upon /tp
void Player::setPos( glm::vec3 pos )
{
	_lastTp = _position;
	_position = pos;
	_fallImmunity = true;
	_z0 = pos.z;
	_fallTime = 0;
	_updateCam = true;
	_smoothCam = false;
	_chunk = NULL;
}

void Player::setSpawnpoint( glm::vec3 spawnpoint )
{
	_spawnpoint = spawnpoint;
}

glm::vec3 Player::getSpawnpoint( void )
{
	return (_spawnpoint);
}

glm::vec3 Player::getLastTp( void )
{
	return (_lastTp);
}

void Player::respawn( void )
{
	_fallTime = 0;
	_breathTime = 0;
	_z0 = _spawnpoint.z;
	_sprinting = false;
	_sneaking = false;
	_smoothCam = false;
	_updateUI = true;
	_waterHead = false;
	_waterFeet = false;
	_chunk = NULL;
	_flySpeed = settings::consts::speed::fly;
	_health = 20;
	_foodLevel = 20;
	_foodSaturationLevel = 20;
	_foodTickTimer = 0;
	_foodExhaustionLevel = 0;
	_inJump = false;
	_touchGround = false;
	_fallImmunity = true;
	_fallDistance = 0;

	_position = _spawnpoint; // {1.0f, -2.0f, 66.0f};
	_yaw = settings::defaults::yaw;
	_pitch = settings::defaults::pitch;
	_updateCam = false;
	_updateFov = false;
	updateVectors();
}


// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void Player::updateVectors( void )
{
	_updateCam = true;
	glm::vec3 front;
	front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	front.y = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	front.z = sin(glm::radians(_pitch));
	_front = glm::normalize(front);
	_right = glm::normalize(glm::cross(_front, settings::consts::math::world_up));
	_up    = glm::normalize(glm::cross(_right, _front));
	_front2 = glm::normalize(glm::vec2(_front)); // used in chunkInFront
	_right2 = glm::vec2(glm::cos(glm::radians(Settings::Get()->getFloat(settings::floats::fov) + _fovOffset))) * glm::normalize(glm::vec2(_right));
}

void Player::processPitch( GLint offset )
{
	_pitch += offset * _deltaTime * _flySpeed * 1.5f;
	if (_pitch > 89.0f || _pitch < -89.0f) {
		_pitch -= offset * _deltaTime * _flySpeed * 1.5f;
	}

	updateVectors();
}

void Player::processYaw( GLint offset )
{
	_yaw += offset * _deltaTime * _flySpeed * 1.5f;
	if (_yaw > 180.0f) {
		_yaw -= 360.0f;
	} else if (_yaw < -180.0f) {
		_yaw += 360.0f;
	}

	updateVectors();
}
