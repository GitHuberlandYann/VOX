#include "Camera.hpp"

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

/**
 * @brief compute player's hitBox height from his state
 * @return float = hitbox's height
*/
float Camera::getHitBox( void )
{
	return ((_sneaking) ? 1.5f : 1.8f);
}

// ************************************************************************** //
//                               Gravity                                      //
// ************************************************************************** //

/**
 * @brief applies gravity to player and check if he touches ground or ceiling to restrict his movements
*/
void Chunk::applyGravity( void )
{
	float saved_posZ = _camera->getPos().z;
	_camera->applyGravity();
	glm::vec3 pos = _camera->getPos();
	float distZ = saved_posZ - pos.z;
	t_collision coll;
	if (distZ < 0) { // jumping
		_camera->_touchGround = false;
		float hitboxHeight = _camera->getHitBox();
		coll = collisionBox({pos.x, pos.y, pos.z + hitboxHeight}, 0.3f, -distZ, -distZ);
		if (coll.type == COLLISION::TOTAL) {
			_camera->touchCeiling(glm::floor(pos.z) + 0.19f);
			// std::cout << "hit roof, " << pos.z << " -> " << _camera->getPos().z << std::endl;
			return ;
		} else if (coll.type == COLLISION::PARTIAL) {
			_camera->touchCeiling(coll.minZ - hitboxHeight);
			return ;
		}
	} else { // falling
		coll = collisionBox(pos, 0.3f, distZ, distZ);
		if (coll.type != COLLISION::NONE) {
			_camera->touchGround(coll.maxZ);
			if (saved_posZ != _camera->getPos().z) {
				_camera->_update = true;
			}
			return ;
		}
	}
	_camera->_update = true;
	_camera->_touchGround = false;
}


/**
 * @brief applies equation z = z0 + vt + at² / 2 to players z coordinate
 * 
*/
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

/**
 * @brief Youston, player touched ground. sets/resets player's info accordingly
*/
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

/**
 * @brief head bump, sets/resets player's info accordingly
*/
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

/**
 * @brief move player by using user inputs
 * @param v, h, z user inputs
*/
void Camera::moveFly( GLint v, GLint h, GLint z )
{
	if (!v && !h && !z) {
		return ;
	}
	_update = true;
	float speed_frame = _deltaTime * _movement_speed * ((_sprinting) ? 2 : 1);
	glm::vec3 norm = glm::normalize(glm::vec3(v * _front.x + h * _right.x,
												v * _front.y + h * _right.y,
												v * _front.z + h * _right.z + z));
	_mtx.lock();
	_position += norm * speed_frame;
	_mtx.unlock();
}

/**
 * @brief update player's position from user's inputs
 * @param direction movement's axis
 * @param v, h, z user's inputs forwards, sideways, and upwards
*/
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

/**
 * @brief called to restore position after collision in survival mode
 * @param pos absolute position to set player's pos to
*/
void Camera::unmoveHuman( glm::vec3 pos )
{
	_mtx.lock();
	_position = pos;
	_mtx.unlock();
	setRun(false);
}

/**
 * @brief if current block pos different from last recorded, we sort water and sky vertices
*/
void Camera::updateCurrentBlock( void )
{
	glm::vec3 camPos = getEyePos();
	glm::ivec3 current_block = {glm::floor(camPos.x), glm::floor(camPos.y), glm::floor(camPos.z)};
	if (current_block != _current_block) {
		_current_block = current_block;
		if (_current_chunk_ptr) {
			if (Settings::Get()->getInt(settings::ints::clouds) != settings::OFF) {
				_current_chunk_ptr->sort_sky(camPos, true);
			}
			_current_chunk_ptr->sort_water(camPos, true);
		}
	}
}

/**
 * @brief adjust position.z to given obstacle, if player can't pass obstacle return false
 * @param minZ, maxZ computed obstacle dimensions
 * @return false to cancel player movement
*/
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

/**
 * @brief called when gamemode is changed, allows to avoid taking damage upon landing from creative mode
*/
void Camera::resetFall( void )
{
	_fall_immunity = true;
	_z0 = getPos().z;
	_fall_time = 0;
	_inJump = false;
}

// ************************************************************************** //
//                               AMobs                                        //
// ************************************************************************** //

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
	_bodyFront = _front;

	const float speed_frame = _deltaTime * settings::consts::speed::zombie;
	if (direction == face_dir::plus_x) {
		_position.x += _front.x * speed_frame;
	} else if (direction == face_dir::plus_y) {
		_position.y += _front.y * speed_frame;
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
 * @param minZ, maxZ computed obstacle dimensions
 * @return false to cancel player movement
*/
bool AMob::customObstacle( float minZ, float maxZ )
{
	(void)minZ;
	if (!_touchGround) return (false);

	// std::cout << "DEBUG customObstacle " << _position.z << " vs " << maxZ << std::endl;
	if (_position.z < maxZ && _position.z + 0.6f > maxZ) {
		// set smoothcam to true to have smoother transition upon climbing stairs
		// ie player will teleport, but cam will follow smoothly to enhance user experience
		// _smoothCam = true;
		// _smoothCamZ = _position.z + 1 + ((_sneaking) ? SNEAK_EYE_LEVEL : EYE_LEVEL);
		_position.z = maxZ;
		return (true);
	}
	return (false);
}

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
 * @brief applies equation z = z0 + vt + at² / 2 to mob's z coordinate
 * 
*/
void AMob::applyGravity( void )
{
	// if (_waterFeet || _waterHead) {
	// 	return (applyGravityUnderwater());
	// }
	// std::cout << "Gravity applied" << std::endl;
	_fallTime += _deltaTime;
	const float initial_speed = ((_inJump) ? settings::consts::speed::initial_jump : settings::consts::speed::initial_fall);

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
		if (_health < 0) {
			_health = 0;
		}
	}
	_fallTime = 0;
	_z0 = _position.z;
	_touchGround = true;
	_inJump = false;
}

/**
 * @brief if current block pos different from last recorded, we update path to player
*/
void AHostileMob::updateCurrentBlock( void )
{
	glm::ivec3 currentBlock = glm::floor(_position);
	if (currentBlock != _currentBlock) {
		if (_currentBlock.z != currentBlock.z) {
			_currentBlock = currentBlock;
			return ;
		}
		_currentBlock = currentBlock;
		if (_state == settings::state_machine::chase) { // update path
			_path = _chunk->computePathfinding(_currentBlock, _player->getPos()).first;
			if (_path.size() < 2) {
				_state = settings::state_machine::idle;
				_stateTime = 0;
			}
		} else if (glm::distance(_position, _player->getPos()) < 32) {
			const std::vector<glm::ivec3> ids = voxel_traversal(_position, _player->getPos());

			for (auto i : ids) {
				int value = _chunk->getBlockAtAbsolute(i);

				if (!s_blocks[value & mask::blocks::type]->transparent) {
					return ; // no direct line of view from mob to player
				}
			}

			// mob views player
			_path = _chunk->computePathfinding(_currentBlock, _player->getPos()).first;
			_state = settings::state_machine::chase;
			_stateTime = 0;
		}
	}
}

