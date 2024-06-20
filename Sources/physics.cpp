#include "Player.hpp"
#include "callbacks.hpp"
#include "logs.hpp"

// ************************************************************************** //
//                          HitBox Collisions                                 //
// ************************************************************************** //

/**
 * @brief called by Chunk::collisionBox, checks if given box collides with given custom hitboxed block
 * @param res current state of collision check
 * @param target ptr to element in s_blocks array
 * @param value block being checked for collision
 * @param pos absolute position of 3D point taken from player hitbox
 * @param width, height dimensions of the player's hitbox
 * @param bX, bY, bZ block's position
*/
void Chunk::collisionWHitbox( t_collision& res, const Block* target, int value, glm::vec3 pos, float width, float height, int bX, int bY, int bZ )
{
	if (cube_cube_intersection(pos, {width, width, height},
		{bX + target->hitboxCenter.x + _startX, bY + target->hitboxCenter.y + _startY, bZ + target->hitboxCenter.z},
		target->hitboxHalfSize)) {
		if (res.type == COLLISION::NONE) {
			res = {COLLISION::PARTIAL, bZ + target->hitboxCenter.z - target->hitboxHalfSize.z, bZ + target->hitboxCenter.z + target->hitboxHalfSize.z};
		} else {
			res.minZ = glm::min(res.minZ, bZ + target->hitboxCenter.z - target->hitboxHalfSize.z);
			res.maxZ = glm::max(res.maxZ, bZ + target->hitboxCenter.z + target->hitboxHalfSize.z);
		}
	}
	if (target->orientedCollisionHitbox) {
		glm::vec3 hitbox[2];
		target->getSecondaryHitbox(hitbox, (value >> offset::blocks::orientation) & 0x7, value >> offset::blocks::bitfield);
		if (cube_cube_intersection(pos, {width, width, height},
			{bX + hitbox[0].x + _startX, bY + hitbox[0].y + _startY, bZ + hitbox[0].z},
			hitbox[1])) {
			if (res.type == COLLISION::NONE) {
				res = {COLLISION::PARTIAL, bZ + hitbox[0].z - hitbox[1].z, bZ + hitbox[0].z + hitbox[1].z};
			} else {
				res.minZ = glm::min(res.minZ, bZ + hitbox[0].z - hitbox[1].z);
				res.maxZ = glm::max(res.maxZ, bZ + hitbox[0].z + hitbox[1].z);
			}
		}
	}
}

/**
 * @brief takes feet position of object, dimension of its hitbox and returns wether object is inside block or not.
 * This is done recursively, originalHeight holds hitbox height throughout recursion
 * WATCHOUT if width > 0.5 problemos because we check block left and right but not middle
 * @param pos object absolute position
 * @param width, height, originalHeight
 * @return t_collision struct containing info about collision
*/
t_collision Chunk::collisionBox( glm::vec3 pos, float width, float height, float originalHeight )
{
	t_collision res = {COLLISION::NONE}, recurse_res = {COLLISION::NONE};
	int minX = glm::floor(pos.x - width - _startX);
	int maxX = glm::floor(pos.x + width - _startX);
	int minY = glm::floor(pos.y - width - _startY);
	int maxY = glm::floor(pos.y + width - _startY);
	int top  = glm::floor(pos.z + height);
	int value = getBlockAt(minX, minY, top);
	const Block* target = s_blocks[value & mask::blocks::type].get();
	if (target->collisionHitbox_1x1x1) {
		return {COLLISION::TOTAL, 0, static_cast<float>(top + 1)};
	} else if (target->collisionHitbox) {
		collisionWHitbox(res, target, value, pos, width, originalHeight, minX, minY, top);
	}
	if (minX != maxX) {
		value = getBlockAt(maxX, minY, top);
		target = s_blocks[value & mask::blocks::type].get();
		if (target->collisionHitbox_1x1x1) {
			return {COLLISION::TOTAL, 0, static_cast<float>(top + 1)};
		} else if (target->collisionHitbox) {
			collisionWHitbox(res, target, value, pos, width, originalHeight, maxX, minY, top);
		}
	}
	if (minY != maxY) {
		value = getBlockAt(minX, maxY, top);
		target = s_blocks[value & mask::blocks::type].get();
		if (target->collisionHitbox_1x1x1) {
			return {COLLISION::TOTAL, 0, static_cast<float>(top + 1)};
		} else if (target->collisionHitbox) {
			collisionWHitbox(res, target, value, pos, width, originalHeight, minX, maxY, top);
		}
	}
	if (minX != maxX && minY != maxY) {
		value = getBlockAt(maxX, maxY, top);
		target = s_blocks[value & mask::blocks::type].get();
		if (target->collisionHitbox_1x1x1) {
			return {COLLISION::TOTAL, 0, static_cast<float>(top + 1)};
		} else if (target->collisionHitbox) {
			collisionWHitbox(res, target, value, pos, width, originalHeight, maxX, maxY, top);
		}
	}
	if (height > 1) {
		recurse_res = collisionBox(pos, width, height - 1, originalHeight);
	} else if (top != glm::floor(pos.z)) {
		recurse_res = collisionBox(pos, width, 0, originalHeight);
	}
	if (recurse_res.type == COLLISION::TOTAL) {
		recurse_res.maxZ = glm::max(res.maxZ, recurse_res.maxZ);
		return (recurse_res);
	} else if (recurse_res.type == COLLISION::PARTIAL) {
		if (res.type == COLLISION::NONE) {
			return (recurse_res);
		} else {
			res.minZ = glm::min(res.minZ, recurse_res.minZ);
			res.maxZ = glm::max(res.maxZ, recurse_res.maxZ);
		}
	}
	return (res);
}

/**
 * @brief collisionBoxWater takes feet position of object, dimension of its hitbox and returns wether object is inside water or not
 * WATCHOUT if width > 0.5 problemos because we check block left and right but not middle
 * @param pos absolute pos of entity being checked
 * @param width, height dimensions of entity's hitbox
 * @return boolean: collision with water
*/
bool Chunk::collisionBoxWater( glm::vec3 pos, float width, float height )
{
	glm::ivec3 top0 = {glm::floor(pos.x - width - _startX), glm::floor(pos.y - width - _startY), glm::floor(pos.z + height)};
	if ((getBlockAt(top0.x, top0.y, top0.z) & mask::blocks::type) == blocks::water) {
		return (true);
	}
	glm::ivec3 top1 = {glm::floor(pos.x + width - _startX), glm::floor(pos.y - width - _startY), glm::floor(pos.z + height)};
	if (top1 != top0) {
		if ((getBlockAt(top1.x, top1.y, top1.z) & mask::blocks::type) == blocks::water) {
			return (true);
		}
	}
	glm::ivec3 top2 = {glm::floor(pos.x + width - _startX), glm::floor(pos.y + width - _startY), glm::floor(pos.z + height)};
	if (top2 != top0) {
		if ((getBlockAt(top2.x, top2.y, top2.z) & mask::blocks::type) == blocks::water) {
			return (true);
		}
	}
	glm::ivec3 top3 = {glm::floor(pos.x - width - _startX), glm::floor(pos.y + width - _startY), glm::floor(pos.z + height)};
	if (top3 != top0) {
		if ((getBlockAt(top3.x, top3.y, top3.z) & mask::blocks::type) == blocks::water) {
			return (true);
		}
	}
	if (height > 1) {
		return (collisionBoxWater(pos, width, height - 1));
	} else if (top0.z != glm::floor(pos.z)) {
		return (collisionBoxWater(pos, width, 0));
	}
	return (false);
}

// ************************************************************************** //
//                               Gravity                                      //
// ************************************************************************** //

/**
 * @brief applies gravity to mob and check if he touches ground or ceiling to restrict his movements
*/
void Chunk::applyGravity( AMob* mob )
{
	float saved_posZ = mob->getPos().z;
	mob->applyGravity();
	glm::vec3 pos = mob->getPos();
	float distZ = saved_posZ - pos.z;
	t_collision coll;
	if (distZ < 0) { // jumping
		mob->setTouchGround(false);
		float hitboxHeight = mob->getHitBox();
		coll = collisionBox({pos.x, pos.y, pos.z + hitboxHeight}, 0.3f, -distZ, -distZ);
		if (coll.type == COLLISION::TOTAL) {
			mob->touchCeiling(glm::floor(pos.z) + 2.0f - hitboxHeight);
			// std::cout << "hit roof, " << pos.z << " -> " << error->getPos().z << std::endl;
			return ;
		} else if (coll.type == COLLISION::PARTIAL) {
			mob->touchCeiling(coll.minZ - hitboxHeight);
			return ;
		}
	} else { // falling
		coll = collisionBox(pos, 0.3f, distZ, distZ);
		if (coll.type != COLLISION::NONE) {
			mob->touchGround(coll.maxZ);
			return ;
		}
	}
	mob->setTouchGround(false);
}

/**
 * @brief applies equation z = z0 + vt + at² / 2 to players z coordinate
 * 
*/
void Player::applyGravity( void )
{
	if (_waterFeet || _waterHead) {
		return (applyGravityUnderwater());
	}
	// std::cout << "Gravity applied" << std::endl;
	_fallTime += _deltaTime;
	// const float initial_speed = ((_inJump) ? (_knockback.z > 0.1f) ? _knockback.z : settings::consts::speed::initial_jump : settings::consts::speed::initial_fall);
	const float initial_speed = ((_inJump) ? settings::consts::speed::initial_jump : settings::consts::speed::initial_fall);

	_position.z = _z0 + initial_speed * _fallTime + 3 * settings::consts::math::standard_gravity * _fallTime * _fallTime * 0.5f; // TODO this '3' is a bit random ...
}

void Player::applyGravityUnderwater( void )
{
	float speed_frame = _deltaTime * ((_inJump) ? ((_waterHead) ? settings::consts::speed::swim_up : settings::consts::speed::swim_down)
												: ((_sneaking) ? -settings::consts::speed::swim_down : -settings::consts::speed::swim_up));

	_position.z += speed_frame;
	_z0 = _position.z;
	_fallTime = 0;
	// std::cout << "gravity underwater " << speed_frame << ", " << _inJump << ", " << _sneaking << std::endl;
}

/**
 * @brief Youston, player touched ground. sets/resets player's info accordingly
*/
void Player::touchGround( float value )
{
	_position.z = value;
	_fallDistance = _z0 - _position.z;
	// std::cout << "TOUCH GROUND AT " << _fallDistance << std::endl;
	// if (_inJump && _fallDistance < 0) {
	// 	return ;
	// }
	if (!_touchGround) {
		_updateCam = true;
	}
	if (_invulnerable) {
	} else if (_fallImmunity) {
		_fallImmunity = false;
	} else if (_waterFeet || _waterHead) {
	} else if (_fallDistance > 3) { // TODO call this function AFTER setting waterFeet
		_updateUI = (glm::max(0.0f, _fallDistance - 3) != 0);
		_health -= glm::max(0.0f, _fallDistance - 3);
		_invulnerable = true;
		_hurtTime = -0.5f;
		if (_health < 0) {
			_health = 0;
		}
	}
	_fallTime = 0;
	_z0 = _position.z;
	_knockback = {0.0f, 0.0f, 0.0f};
	_touchGround = true;
	_inJump = false;
}

/**
 * @brief head bump, sets/resets player's info accordingly
*/
void Player::touchCeiling( float value )
{
	_position.z = value;
	_z0 = value;
	_fallTime = 0;
	_updateCam = true;
	_inJump = false;
}

/**
 * @brief called when gamemode is changed, allows to avoid taking damage upon landing from creative mode
*/
void Player::resetFall( void )
{
	_fallImmunity = true;
	_z0 = _position.z;
	_fallTime = 0;
	_inJump = false;
}

// ************************************************************************** //
//                               AMobs                                        //
// ************************************************************************** //

float Zombie::getSpeed( void )
{
	return (settings::consts::speed::zombie);
}

float Skeleton::getSpeed( void )
{
	return (settings::consts::speed::skeleton);
}

/**
 * @brief update mobs's position
 * @param direction movement's axis
*/
void AMob::move( int direction, bool move )
{
	// if (_waterFeet || _waterHead) {
	// 	return (moveHumanUnderwater(direction, v, h, z));
	if (!move) {
		_walking = false;
		return ;
	}
	_walking = true;

	const float speedFrame = _deltaTime * getSpeed();
	if (direction == face_dir::plus_x) {
		if (glm::abs(_knockback.x) + glm::abs(_knockback.y) > 0.01f) {
			_position.x += _knockback.x * speedFrame;
			_knockback.x = glm::sign(_knockback.x) * (glm::abs(_knockback.x) - _deltaTime);
		} else {
			_position.x += _bodyFront.x * speedFrame;
			_knockback.z = 0.0f;
		}
	} else if (direction == face_dir::plus_y) {
		if (glm::abs(_knockback.x) + glm::abs(_knockback.y) > 0.01f) {
			_position.y += _knockback.y * speedFrame;
			_knockback.y = glm::sign(_knockback.y) * (glm::abs(_knockback.y) - _deltaTime);
		} else {
			_position.y += _bodyFront.y * speedFrame;
		}
	}
}

/**
 * @brief called to undo movement after collision detected
 */
void AMob::restorePos( glm::vec3 position )
{
	_position = position;
}

/**
 * @brief adjust position.z to given obstacle, if player can't pass obstacle return false
 * @param dir one of face_dir::plus_x and face_dir::plus_y representing movement mob was trying to do
 * @param maxZ computed obstacle max z, used to jump it if possible
 * @return false to cancel player movement
*/
bool AHostileMob::customObstacle( int dir, float maxZ )
{
	if (!_touchGround) return (false);

	// std::cout << "DEBUG customObstacle " << _position.z << " vs " << maxZ << std::endl;
	if (_position.z < maxZ) {
		if (_position.z + 0.6f > maxZ) {
			_position.z = maxZ;
			return (true);
		} else if (_position.z + 1.1f > maxZ) {
			_inJump = true;
		} else if (_state == settings::state_machine::wandle) {
			(dir == face_dir::plus_x)
				? _bodyFront.y += 5.0f * ((_bodyFront.y > 0) ? _deltaTime : -_deltaTime)
				: _bodyFront.x += 5.0f * ((_bodyFront.x > 0) ? _deltaTime : -_deltaTime);
			_bodyFront = glm::normalize(_bodyFront);
			_stateTime += _deltaTime;		}
	}
	return (false);
}

/**
 * @brief applies equation z = z0 + vt + at² / 2 to mob's z coordinate
 * 
*/
void AMob::applyGravity( void )
{
	_fallTime += _deltaTime;
	const float initial_speed = ((_inJump) ? (_knockback.z > 0.1f) ? _knockback.z : settings::consts::speed::initial_jump : settings::consts::speed::initial_fall);

	_position.z = _z0 + initial_speed * _fallTime + 3 * settings::consts::math::standard_gravity * _fallTime * _fallTime * 0.5f; // TODO this '3' is a bit random ...
}

/**
 * @brief head bump, sets/resets mob's info accordingly
*/
void AMob::touchCeiling( float value )
{
	_position.z = value;
	_z0 = value;
	_fallTime = 0;
	_inJump = false;
}

/**
 * @brief Youston, mob touched ground. sets/resets mob's info accordingly
*/
void AMob::touchGround( float value )
{
	_position.z = value;
	_fallDistance = _z0 - _position.z;
	// std::cout << "TOUCH GROUND AT " << _fall_distance << std::endl;
	// if (_inJump && _fall_distance < 0) {
	// 	return ;
	// }
	if (_invulnerable) {
	// } else if (_waterFeet || _waterHead) {
	} else if (_fallDistance > 3) { // TODO call this function AFTER setting waterFeet
		_health -= glm::max(0.0f, _fallDistance - 3);
		_invulnerable = true;
		_hurtTime = -0.5f;
		if (_health < 0) {
			_health = 0;
		}
	}
	_fallTime = 0;
	_z0 = _position.z;
	_knockback = {0.0f, 0.0f, 0.0f};
	_touchGround = true;
	_inJump = false;
}

/**
 * @brief if current block pos different from last recorded, we update path to player
 * @return true if mob went from one chunk to another
*/
bool AHostileMob::updateCurrentBlock( void )
{
	bool changeOwner = false;
	glm::ivec3 currentBlock = glm::floor(_position);
	if (currentBlock != _currentBlock) {
		if (_currentBlock.z != currentBlock.z) {
			_currentBlock = currentBlock;
			return (false);
		}
		
		if (chunk_pos(currentBlock.x) != chunk_pos(_currentBlock.x) || chunk_pos(currentBlock.y) != chunk_pos(_currentBlock.y)) {
			Chunk* newOwner = _chunk->getChunkAt(chunk_pos(currentBlock.x), chunk_pos(currentBlock.y));
			if (!newOwner || newOwner == _chunk) { return (true); } // just kill mob if out of generated chunks
			_chunk = newOwner;
			_blockTime = 0.0f;
			_currentBlock = currentBlock;
			_chunk->addMob(*this, _type);
			changeOwner = true;
		} else {
			_blockTime = 0.0f;
			_currentBlock = currentBlock;
		}

		updatePath();
	}
	return (changeOwner);
}

// ************************************************************************** //
//                               AMobs                                        //
// ************************************************************************** //

/**
 * @brief move player by using user inputs
 * @param v, h, z user inputs
*/
void Player::moveFly( GLint v, GLint h, GLint z )
{
	if (!v && !h && !z) {
		return ;
	}
	_updateCam = true;
	float speed_frame = _deltaTime * _flySpeed * ((_sprinting) ? 2 : 1);
	glm::vec3 norm = glm::normalize(glm::vec3(v * _front.x + h * _right.x,
												v * _front.y + h * _right.y,
												v * _front.z + h * _right.z + z));
	_position += norm * speed_frame;
}

void Player::moveUnderwater( int direction, GLint v, GLint h, GLint z )
{
	if (!v && !h) {
		if (direction == face_dir::plus_z) {
			_inJump = (z == 1);
			_sneaking = (z == -1);
		}
		return ;
	}
	_updateCam = true;

	float speed_frame = _deltaTime * ((_sprinting) ?  settings::consts::speed::sprint : settings::consts::speed::swim);
	float movement = 0;
	if (direction == face_dir::plus_x) {
		movement = glm::normalize(glm::vec3(v * _front.x + h * _right.x, v * _front.y + h * _right.y, v * _front.z + h * _right.z)).x * speed_frame;
		_position.x += movement;
	}
	else if (direction == face_dir::plus_y) {
		movement = glm::normalize(glm::vec3(v * _front.x + h * _right.x, v * _front.y + h * _right.y, v * _front.z + h * _right.z)).y * speed_frame;
		_position.y += movement;
	} else {
		_inJump = (z == 1);
		_sneaking = (z == -1);
		movement = glm::normalize(glm::vec3(v * _front.x + h * _right.x, v * _front.y + h * _right.y, v * _front.z + h * _right.z)).z * speed_frame;
		_position.z += movement;
		if (_chunk->collisionBox(_position, 0.3f, getHitBox(), getHitBox()).type == COLLISION::TOTAL) { // TODO handle partial collision underwater
			_position.z -= movement;
		}
	}
	updateExhaustion(glm::abs(movement * settings::consts::exhaustion::swim));
	if (_sprinting && movement != 0) {
		updateExhaustion(glm::abs(movement * settings::consts::exhaustion::sprint));
	}
}

float Player::getSpeed( void )
{
	return ((_sneaking)
				? ((_sprinting) ? settings::consts::speed::sneak_sprint
								: settings::consts::speed::sneak)
				: ((_sprinting) ?  ((_touchGround) ? settings::consts::speed::sprint
													: settings::consts::speed::sprint_jump )
								: settings::consts::speed::walk));
}

/**
 * @brief update player's on x OR y axis
 * @param direction movement's axis
 * @param v, h, z user's inputs forwards, sideways, and upwards
*/
void Player::move( int direction, GLint v, GLint h, GLint z )
{
	if (_waterFeet || _waterHead) {
		return (moveUnderwater(direction, v, h, z));
	} else if (direction == face_dir::plus_z) {
		return ;
	} else if (!v && !h) {
		_walking = false;
		// return ;
	} else {
		_updateCam = true;
		_walking = true;
		_bodyFront = _front;
	}

	const float speedFrame = _deltaTime * getSpeed();
	float movement = 0;
	if (direction == face_dir::plus_x) {
		if (glm::abs(_knockback.x) + glm::abs(_knockback.y) > 0.01f) {
			_position.x += _knockback.x * speedFrame;
			_knockback.x = glm::sign(_knockback.x) * (glm::max(0.0f, glm::abs(_knockback.x) - _deltaTime));
		} else {
			movement = glm::normalize(glm::vec2(v * _front.x + h * _right.x, v * _front.y + h * _right.y)).x * speedFrame;
			_position.x += movement;
			_knockback.z = 0.0f;
		}
	}
	else if (direction == face_dir::plus_y) {
		if (glm::abs(_knockback.x) + glm::abs(_knockback.y) > 0.01f) {
			_position.y += _knockback.y * speedFrame;
			_knockback.y = glm::sign(_knockback.y) * (glm::max(0.0f, glm::abs(_knockback.y) - _deltaTime));
		} else {
			movement = glm::normalize(glm::vec2(v * _front.x + h * _right.x, v * _front.y + h * _right.y)).y * speedFrame;
			_position.y += movement;
		}
	}
	if (_sprinting && movement != 0) {
		updateExhaustion(glm::abs(movement * settings::consts::exhaustion::sprint));
	}
}

/**
 * @brief adjust position.z to given obstacle, if player can't pass obstacle return false
 * @param maxZ computed obstacle dimensions
 * @return false to cancel player movement
*/
bool Player::customObstacle( int dir, float maxZ )
{
	(void)dir;
	if (!_touchGround) return (false);

	// std::cout << "DEBUG customObstacle " << _position.z << " vs " << maxZ << std::endl;
	if (_position.z < maxZ && _position.z + 0.6f > maxZ) {
		// set smoothcam to true to have smoother transition upon climbing stairs
		// ie player will teleport, but cam will follow smoothly to enhance user experience
		_smoothCam = true;
		_smoothCamZ = _position.z;
		_position.z = maxZ;
		return (true);
	}
	return (false);
}

/**
 * @brief called to restore position after collision in survival mode
 * @param pos absolute position to set player's pos to
*/
void Player::restorePos( glm::vec3 pos )
{
	_position = pos;
	setRun(false);
}

/**
 * @brief if current block pos different from last recorded, we sort water and sky vertices
*/
bool Player::updateCurrentBlock( void )
{
	glm::ivec3 current_block = glm::floor(_position);
	if (current_block != _currentBlock) {
		_currentBlock = current_block;
		if (_chunk) {
			glm::vec3 camPos = getEyePos();
			if (Settings::Get()->getInt(settings::ints::clouds) != settings::OFF) {
				_chunk->sort_sky(camPos, true);
			}
			_chunk->sort_water(camPos, true);
		}
		return (true);
	}
	return (false);
}

bool Player::update( std::vector<t_shaderInput>& modArr, float deltaTime )
{
	(void)modArr, (void)deltaTime;
	std::cout << "ERROR Player::update called." << std::endl;
	return (false);
}

/**
 * @brief use user input to update player's position and handle collision
 */
void Player::inputUpdate( bool rayCast, int gameMode )
{
	GLint key_cam_v = inputs::key_down(inputs::move_forwards) - inputs::key_down(inputs::move_backwards);
	GLint key_cam_h = inputs::key_down(inputs::move_right) - inputs::key_down(inputs::move_left);
	bool key_cam_zup = inputs::key_down(inputs::jump);
	bool key_cam_zdown = inputs::key_down(inputs::sneak);

	// this will be commented at some point
	GLint key_cam_yaw = inputs::key_down(inputs::look_left) - inputs::key_down(inputs::look_right);
	if (key_cam_yaw) {
		processYaw(key_cam_yaw * 5);
	}
	GLint key_cam_pitch = inputs::key_down(inputs::look_up) - inputs::key_down(inputs::look_down);
	if (key_cam_pitch) {
		processPitch(key_cam_pitch * 5);
	}

	if (!key_cam_v && !key_cam_h) {
		setRun(false);
	} else {
		setRun(inputs::key_down(inputs::run));
	}

	if (gameMode == settings::consts::gamemode::creative) { // no collision check, free to move however you want
		moveFly(key_cam_v, key_cam_h, key_cam_zup - key_cam_zdown);
	}

	if (rayCast) {
		if (gameMode != settings::consts::gamemode::creative && _chunk) { // on first frame -> no _chunk
			if (_smoothCam) {
				if (_smoothCamZ < _position.z) {
					_smoothCamZ += _deltaTime * settings::consts::speed::smooth_cam;
					_updateCam = true;
				} else {
					_smoothCam = false;
				}
			}
			if (_hurtTime < 0) {
				_hurtTime += _deltaTime;
				if (_hurtTime >= 0) {
					_invulnerable = false;
				}
			}

			setSneak(key_cam_zdown);
			setJump(key_cam_zup && inputs::key_update(inputs::jump));
			move(face_dir::plus_z, key_cam_v, key_cam_h, key_cam_zup - key_cam_zdown); // used for underwater movement
			glm::vec3 originalPos = _position;
			move(face_dir::plus_x, key_cam_v, key_cam_h, 0); // move on X_AXIS
			float hitBoxHeight = getHitBox();
			t_collision coll = _chunk->collisionBox(_position, 0.3f, hitBoxHeight, hitBoxHeight);
			if (coll.type != COLLISION::NONE) {
				// _ui->chatMessage("xcoll " + std::to_string(coll.type) + ", " + std::to_string(coll.minZ) + " ~ " + std::to_string(coll.maxZ) + " h " + std::to_string(hitBoxHeight));
				if (!customObstacle(face_dir::plus_x, coll.maxZ)
					|| _chunk->collisionBox(_position, 0.3f, hitBoxHeight, hitBoxHeight).type != COLLISION::NONE) {
					restorePos(originalPos); // if collision after movement, undo movement + setRun(false)
				}
			} else if (key_cam_zdown && _touchGround && _chunk->collisionBox(_position - glm::vec3(0, 0, 0.6f), 0.3f, 0.7f, 0.7f).type == COLLISION::NONE) {
				// if sneaking and touch ground and after move we have gap of more than 0.6 under our feet, undo movement
				restorePos(originalPos);
			}
			originalPos = _position;
			move(face_dir::plus_y, key_cam_v, key_cam_h, 0); // move on Y_AXIS
			coll = _chunk->collisionBox(_position, 0.3f, hitBoxHeight, hitBoxHeight);
			if (coll.type != COLLISION::NONE) {
				if (!customObstacle(face_dir::plus_y, coll.maxZ)
					|| _chunk->collisionBox(_position, 0.3f, hitBoxHeight, hitBoxHeight).type != COLLISION::NONE) {
					restorePos(originalPos);
				}
			} else if (key_cam_zdown && _touchGround && _chunk->collisionBox(_position - glm::vec3(0, 0, 0.6f), 0.3f, 0.7f, 0.7f).type == COLLISION::NONE) {
				restorePos(originalPos);
			}
			_chunk->applyGravity(this); // move on Z_AXIS
			setWaterStatus(false, _chunk->collisionBoxWater(_position, 0.3f, 0));
			setWaterStatus(true, _chunk->collisionBoxWater(getEyePos(), 0.05f, 0));
			if (!_touchGround) {
				_updateCam = true;
			}
		}
	}

	updateCurrentBlock();

	GLint key_cam_speed = inputs::key_down(inputs::fly_speed_up) - inputs::key_down(inputs::fly_speed_down);
	if (key_cam_speed) {
		updateFlySpeed(key_cam_speed);
	}
}
