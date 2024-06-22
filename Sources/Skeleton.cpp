#include "Player.hpp"
#include "Skeleton.hpp"
#include "Chunk.hpp"
#include "logs.hpp"
#include "random.hpp"

Skeleton::Skeleton( Chunk* chunk, Player* player, glm::vec3 position ) : AHostileMob(player, position)
{
    _chunk = chunk;
	_type = settings::consts::mob::skeleton;

	LOG("new skeleton at " << POS(_position));
}

Skeleton::~Skeleton( void )
{

}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

bool Skeleton::update( std::vector<t_shaderInput>& modArr, float deltaTime )
{
    if (!_chunk) {
        LOGERROR("skeleton is missing a _chunk");
        return (true);
    }
	bool changeOwner = false;

    // update position
	if (!_noAI && Settings::Get()->getBool(settings::bools::mobAI) && _health > 0) {
		_deltaTime = deltaTime;
		_stateTime += deltaTime;

		if (_state == settings::state_machine::idle) {
			_walkTime = 0;
			move(0, false);
			if (_stateTime > 3.0f) {
				setState(settings::state_machine::wandle);
				unsigned seed = _position.x * 14 + _position.y * 18 + _position.z * 40 + deltaTime * 100;
				_front = glm::normalize(glm::vec3(Random::randomFloat(seed) * 2.0f - 1.0f, Random::randomFloat(seed) * 2.0f - 1.0f, 0.0f));
				if (!s_blocks[_chunk->getBlockAtAbsolute(_position + _front) & mask::blocks::type]->transparent) {
					_front = -_front;
				}
				_bodyFront = _front;
				_right = glm::normalize(glm::cross(_front, settings::consts::math::world_up));
				_up = {0, 0, 1};
			}
		} else if (_state == settings::state_machine::aim) {
			if (!_chunk->lineOfSight(getEyePos(), _player->getEyePos()) || glm::distance(_position, _player->getPos()) > 14.0f) {
				setState(settings::state_machine::chase); // lost view of player, back to chase
			} else {
				if (_stateTime > 1.0f) { // shoot arrow
					_chunk->shootArrow(getEyePos(), getDir(), 10.0f); // TODO add 0.2 per block to cover
					setState(settings::state_machine::aim); // reset timer
				}
				glm::vec3 computedHeadFront = glm::normalize(_player->getPos() - _position);
				_front = glm::normalize(_front + (computedHeadFront - _front) * 7.0f * deltaTime);
				_right = glm::normalize(glm::cross(_front, settings::consts::math::world_up));
				_up = glm::normalize(glm::cross(_right, _front));
				_bodyFront = glm::normalize(glm::vec2(_front));
			}
		} else {
			_walkTime = (_walking) ? _walkTime + deltaTime : 0;
			_blockTime += deltaTime;
			if (_blockTime > 2.5f) { // if stuck on same block for 3 sec while trying to move, revert to idle
				setState(settings::state_machine::idle);
			}

			if (_state == settings::state_machine::chase) {
				if (_path.size()) {
					// set _front according to computed path
					glm::vec3 pathDiff = (_path.size() > 1) ? _path[1] - _path[0] : _path[0] - _currentBlock;
					if (pathDiff.z > 0.1f) {
						_inJump = true;
					}
					glm::vec2 computedFront = pathDiff;
					if (!computedFront.x) { computedFront.x = (_currentBlock.x + 0.5f) - _position.x; }
					if (!computedFront.y) { computedFront.y = (_currentBlock.y + 0.5f) - _position.y; }
					computedFront = glm::normalize(computedFront);
					_bodyFront = glm::normalize(_bodyFront + (computedFront - _bodyFront) * 7.0f * deltaTime);
					glm::vec3 computedHeadFront = glm::normalize(_player->getPos() - _position);
					_front = glm::normalize(_front + (computedHeadFront - _front) * 7.0f * deltaTime);
					_right = glm::normalize(glm::cross(_front, settings::consts::math::world_up));
					_up = glm::normalize(glm::cross(_right, _front));
				}
			} else if (_stateTime > 6.0f) {
				setState(settings::state_machine::idle);
			}

			// move on X_AXIS
			glm::vec3 originalPos = _position;
			move(face_dir::plus_x, true);
			const float hitBoxHeight = settings::consts::hitbox::zombie;
			t_collision coll = _chunk->collisionBox(_position, 0.3f, hitBoxHeight, hitBoxHeight);
			if (coll.type != COLLISION::NONE) {
				// _ui->chatMessage("xcoll " + std::to_string(coll.type) + ", " + std::to_string(coll.minZ) + " ~ " + std::to_string(coll.maxZ) + " h " + std::to_string(hitBoxHeight));
				if (!customObstacle(face_dir::plus_x, coll.maxZ)
					|| _chunk->collisionBox(_position, 0.3f, hitBoxHeight, hitBoxHeight).type != COLLISION::NONE) {
					restorePos(originalPos); // if collision after movement, undo movement + setRun(false)
				}
			}
			
			// move on Y_AXIS
			originalPos = _position;
			move(face_dir::plus_y, true);
			coll = _chunk->collisionBox(_position, 0.3f, hitBoxHeight, hitBoxHeight);
			if (coll.type != COLLISION::NONE) {
				if (!customObstacle(face_dir::plus_y, coll.maxZ)
					|| _chunk->collisionBox(_position, 0.3f, hitBoxHeight, hitBoxHeight).type != COLLISION::NONE) {
					restorePos(originalPos);
				}
			}

			if (_state == settings::state_machine::chase
				&& cube_cube_intersection(_player->getPos(), {0.3f, 0.3f, settings::consts::hitbox::player},
										_position + glm::vec3(0, 0, settings::consts::hitbox::zombie * 0.5f), {0.6f, 0.6f, settings::consts::hitbox::zombie * 0.5f})) {
				_player->receiveDamage(4.5f, getDir());
			}

			changeOwner = updateCurrentBlock();
		}
		_chunk->applyGravity(this);
	}

	// update hurt, death times
	if (_hurtTime < 0) {
		_hurtTime += deltaTime;
		if (_hurtTime >= 0) {
			_invulnerable = false;
		}
	}
	if (_deathTime < 0) {
		_deathTime += deltaTime;
		_invulnerable = true;
		if (_deathTime >= 0) {
			// remove AMob
			return (true);
		}
	}

    // draw
    draw(modArr);
	return (changeOwner);
}

bool Skeleton::getHitBox( void )
{
	return (settings::consts::hitbox::skeleton);
}

glm::vec3 Skeleton::getEyePos( void )
{
	glm::vec3 res = _position;
	res.z += 1 + settings::consts::eyeLevel::skeleton;
	return (res);
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

/**
 * @brief called when Skeleton goes from one block to another, used to update its path and state
 */
void Skeleton::updatePath( void )
{
	if (_state == settings::state_machine::chase) { // update path, or take aim if close enough and clear line of sight
		if (glm::distance(_position, _player->getPos()) < 14.0f && _chunk->lineOfSight(getEyePos(), _player->getEyePos())) {
			return (setState(settings::state_machine::aim));
		}
		_path = _chunk->computePathfinding(_currentBlock, _player->getPos()).first;
		if (!_path.size()) {
			setState(settings::state_machine::idle);
		}
	} else if (glm::distance(_position, _player->getPos()) < 16.0f) {
		if (!_chunk->lineOfSight(getEyePos(), _player->getEyePos())) {
			return ; // no direct line of sight from mob to player
		}

		// mob views player
		_path = _chunk->computePathfinding(_currentBlock, _player->getPos()).first;
		setState(settings::state_machine::chase);
	}
}

void Skeleton::draw( std::vector<t_shaderInput>& arr )
{
    // 1 model texture pxl is 0.0609375 meters
	const float scale = (_deathTime < 0) ? settings::consts::hitbox::skeleton * (glm::abs(_deathTime) * 0.5f) / 32.0f
										: settings::consts::hitbox::skeleton / 32.0f;

// draw head
	// draw face
	glm::vec3 pos = getEyePos();
	// if (_smoothCam) {
	// 	pos.z = _smoothCamZ - _deltaTime * SMOOTH_CAM_SPEED;
	// }
	glm::vec3 p0 = pos + _right * 4.0f * scale + _front * 4.0f * scale + _up * 4.0f * scale;
	glm::vec3 p1 = p0 - _right * 8.0f * scale;
	glm::vec3 p2 = p0 - _up * 8.0f * scale;
	glm::vec3 p3 = p1 - _up * 8.0f * scale;

	int itemLight = _chunk->computePosLight(pos);
	int hurtColor = (_hurtTime < 0) ? (1 << 19) : 0;
	int modelAltas = (settings::consts::shader::texture::skeleton << 12);
	int spec = (2 << 19) + 8 + (8 << 6) + (itemLight << 24) + hurtColor + modelAltas;
	utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, 8, 8, 0, 6);

	// draw neck
	glm::vec3 p4 = p1 - _front * 8.0f * scale;
	glm::vec3 p5 = p0 - _front * 8.0f * scale;
	glm::vec3 p6 = p3 - _front * 8.0f * scale;
	glm::vec3 p7 = p2 - _front * 8.0f * scale;

	spec = (2 << 19) + 24 + (8 << 6) + (itemLight << 24) + hurtColor + modelAltas;
	utils::shader::addQuads(arr, {p4, p5, p6, p7}, spec, 8, 8, 0, 6);
	
	// left cheek
	spec -= 8;
	utils::shader::addQuads(arr, {p1, p4, p3, p6}, spec, 8, 8, 0, 6);
	// right cheek
	spec -= 16;
	utils::shader::addQuads(arr, {p5, p0, p7, p2}, spec, 8, 8, 0, 6);

	//top
	spec += 8 - (8 << 6);
	utils::shader::addQuads(arr, {p5, p4, p0, p1}, spec, 8, 8, 0, 6);
	//bottom
	spec += 8;
	utils::shader::addQuads(arr, {p6, p7, p3, p2}, spec, 8, 8, 0, 6);

	// cheecks, top and bottom of head don't need to be displayed as there's no way to see them right now
	// will get a chance to look at them from inventory once implemented

// draw torso
	glm::vec3 bodyFront = {glm::normalize(_bodyFront), 0};
	const glm::vec3 bodyRight = glm::normalize(glm::cross(bodyFront, settings::consts::math::world_up));
	const glm::vec3 bodyUp = settings::consts::math::world_up;
	bodyFront = glm::normalize(glm::cross(bodyUp, bodyRight));
	pos.z -= 4.0f * scale;
	// up
	p0 = pos + bodyRight * 4.0f * scale - bodyFront * 2.0f * scale;
	p1 = p0 - bodyRight * 8.0f * scale;
	p2 = p0 + bodyFront * 4.0f * scale;
	p3 = p1 + bodyFront * 4.0f * scale;
	spec = (2 << 19) + 20 + (16 << 6) + (itemLight << 24) + hurtColor + modelAltas;
	// addQuads(arr, {p0, p1, p2, p3}, spec, 8, 4, 0, 6); // not drawn
	// down
	p4 = p2 - bodyUp * 12.0f * scale;
	p5 = p3 - bodyUp * 12.0f * scale;
	p6 = p0 - bodyUp * 12.0f * scale;
	p7 = p1 - bodyUp * 12.0f * scale;
	spec += 8;
	utils::shader::addQuads(arr, {p4, p5, p6, p7}, spec, 8, 4, 0, 6);
	// front 2345
	spec = (2 << 19) + 20 + (20 << 6) + (itemLight << 24) + hurtColor + modelAltas;
	utils::shader::addQuads(arr, {p2, p3, p4, p5}, spec, 8, 12, 0, 6); // recto
	utils::shader::addQuads(arr, {p3, p2, p5, p4}, spec, 8, 12, 0, 6, true); // verso
	// back 1076
	spec += 12;
	utils::shader::addQuads(arr, {p1, p0, p7, p6}, spec, 8, 12, 0, 6); // recto
	utils::shader::addQuads(arr, {p0, p1, p6, p7}, spec, 8, 12, 0, 6, true); // verso
	// right 0264
	spec = (2 << 19) + 16 + (20 << 6) + (itemLight << 24) + hurtColor + modelAltas;
	utils::shader::addQuads(arr, {p0, p2, p6, p4}, spec, 4, 12, 0, 6);
	// left 3157
	spec += 12;
	utils::shader::addQuads(arr, {p3, p1, p5, p7}, spec, 4, 12, 0, 6);

// draw arms and legs
	float sina = glm::sin(_walkTime * 5) * 0.5f;
	glm::vec3 armFront = (_state == settings::state_machine::chase) ? glm::normalize(bodyUp + bodyFront * sina * 0.2f) : glm::normalize(bodyFront + settings::consts::math::world_up * sina);
	glm::vec3 armUp = glm::normalize(glm::cross(bodyRight, armFront));
	glm::vec3 armRight = bodyRight;

// draw right arm
	pos += armRight * 5.0f * scale;
	// up
	p0 = pos + armRight * 1.0f * scale - armFront * 1.0f * scale;
	p1 = p0 - armRight * 2.0f * scale;
	p2 = p0 + armFront * 2.0f * scale;
	p3 = p1 + armFront * 2.0f * scale;
	spec = (2 << 19) + 42 + (16 << 6) + (itemLight << 24) + hurtColor + modelAltas;
	utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, 2, 2, 0, 6);
	// down
	p4 = p2 - armUp * 12.0f * scale;
	p5 = p3 - armUp * 12.0f * scale;
	p6 = p0 - armUp * 12.0f * scale;
	p7 = p1 - armUp * 12.0f * scale;
	spec += 2;
	utils::shader::addQuads(arr, {p4, p5, p6, p7}, spec, 2, 2, 0, 6);
	// front 2345
	spec = (2 << 19) + 42 + (18 << 6) + (itemLight << 24) + hurtColor + modelAltas;
	utils::shader::addQuads(arr, {p2, p3, p4, p5}, spec, 2, 12, 0, 6);
	// back 1076
	spec += 4;
	utils::shader::addQuads(arr, {p1, p0, p7, p6}, spec, 2, 12, 0, 6);
	// right 0264
	spec = (2 << 19) + 40 + (18 << 6) + (itemLight << 24) + hurtColor + modelAltas;
	utils::shader::addQuads(arr, {p0, p2, p6, p4}, spec, 2, 12, 0, 6);
	// left 3157
	spec += 4;
	utils::shader::addQuads(arr, {p3, p1, p5, p7}, spec, 2, 12, 0, 6);

	pos -= armRight * 5.0f * scale;
// draw left leg // whose movement is in synch with right arm
	glm::vec3 legFront = glm::normalize(glm::vec3(glm::normalize(glm::vec2(_bodyFront)), 0) + settings::consts::math::world_up * sina);
	glm::vec3 legUp = glm::normalize(glm::cross(bodyRight, legFront));
	pos -= bodyRight * 2.0f * scale;
	pos -= bodyUp * 12.0f * scale;
	// up
	p0 = pos + bodyRight * 1.0f * scale - legFront * 1.0f * scale;
	p1 = p0 - bodyRight * 2.0f * scale;
	p2 = p0 + legFront * 2.0f * scale;
	p3 = p1 + legFront * 2.0f * scale;
	spec = (2 << 19) + 2 + (16 << 6) + (itemLight << 24) + hurtColor + modelAltas;
	utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, 2, 2, 0, 6);
	// down
	p4 = p2 - legUp * 12.0f * scale;
	p5 = p3 - legUp * 12.0f * scale;
	p6 = p0 - legUp * 12.0f * scale;
	p7 = p1 - legUp * 12.0f * scale;
	spec += 2;
	utils::shader::addQuads(arr, {p4, p5, p6, p7}, spec, 2, 2, 0, 6);
	// front 2345
	spec = (2 << 19) + 2 + (18 << 6) + (itemLight << 24) + hurtColor + modelAltas;
	utils::shader::addQuads(arr, {p2, p3, p4, p5}, spec, 2, 12, 0, 6);
	// back 1076
	spec += 4;
	utils::shader::addQuads(arr, {p1, p0, p7, p6}, spec, 2, 12, 0, 6);
	// right 0264
	spec = (2 << 19) + 0 + (18 << 6) + (itemLight << 24) + hurtColor + modelAltas;
	utils::shader::addQuads(arr, {p0, p2, p6, p4}, spec, 2, 12, 0, 6);
	// left 3157
	spec += 4;
	utils::shader::addQuads(arr, {p3, p1, p5, p7}, spec, 2, 12, 0, 6);

// draw left arm
	sina = -sina;
	armFront = (_state == settings::state_machine::chase) ? glm::normalize(bodyUp + bodyFront * sina * 0.2f) : glm::normalize(bodyFront + settings::consts::math::world_up * sina);
	armUp = glm::normalize(glm::cross(bodyRight, armFront));
	pos -= bodyRight * 3.0f * scale;
	pos += bodyUp * 12.0f * scale;
	// up
	p0 = pos + bodyRight * 1.0f * scale - armFront * 1.0f * scale;
	p1 = p0 - bodyRight * 2.0f * scale;
	p2 = p0 + armFront * 2.0f * scale;
	p3 = p1 + armFront * 2.0f * scale;
	spec = (2 << 19) + 42 + (16 << 6) + (itemLight << 24) + hurtColor + modelAltas;
	utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, 2, 2, 0, 6);
	// down
	p4 = p2 - armUp * 12.0f * scale;
	p5 = p3 - armUp * 12.0f * scale;
	p6 = p0 - armUp * 12.0f * scale;
	p7 = p1 - armUp * 12.0f * scale;
	spec += 2;
	utils::shader::addQuads(arr, {p4, p5, p6, p7}, spec, 2, 2, 0, 6);
	// front 2345
	spec = (2 << 19) + 42 + (18 << 6) + (itemLight << 24) + hurtColor + modelAltas;
	utils::shader::addQuads(arr, {p2, p3, p4, p5}, spec, 2, 12, 0, 6);
	// back 1076
	spec += 4;
	utils::shader::addQuads(arr, {p1, p0, p7, p6}, spec, 2, 12, 0, 6);
	// right 0264
	spec = (2 << 19) + 40 + (18 << 6) + (itemLight << 24) + hurtColor + modelAltas;
	utils::shader::addQuads(arr, {p0, p2, p6, p4}, spec, 2, 12, 0, 6);
	// left 3157
	spec += 4;
	utils::shader::addQuads(arr, {p3, p1, p5, p7}, spec, 2, 12, 0, 6);

// draw right leg
	legFront = glm::normalize(glm::vec3(glm::normalize(glm::vec2(_bodyFront)), 0) + settings::consts::math::world_up * sina);
	legUp = glm::normalize(glm::cross(bodyRight, legFront));
	pos += bodyRight * 8.0f * scale;
	pos -= bodyUp * 12.0f * scale;
	// up
	p0 = pos + bodyRight * 1.0f * scale - legFront * 1.0f * scale;
	p1 = p0 - bodyRight * 2.0f * scale;
	p2 = p0 + legFront * 2.0f * scale;
	p3 = p1 + legFront * 2.0f * scale;
	spec = (2 << 19) + 2 + (16 << 6) + (itemLight << 24) + hurtColor + modelAltas;
	utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, 2, 2, 0, 6);
	// down
	p4 = p2 - legUp * 12.0f * scale;
	p5 = p3 - legUp * 12.0f * scale;
	p6 = p0 - legUp * 12.0f * scale;
	p7 = p1 - legUp * 12.0f * scale;
	spec += 2;
	utils::shader::addQuads(arr, {p4, p5, p6, p7}, spec, 2, 2, 0, 6);
	// front 2345
	spec = (2 << 19) + 2 + (18 << 6) + (itemLight << 24) + hurtColor + modelAltas;
	utils::shader::addQuads(arr, {p2, p3, p4, p5}, spec, 2, 12, 0, 6);
	// back 1076
	spec += 4;
	utils::shader::addQuads(arr, {p1, p0, p7, p6}, spec, 2, 12, 0, 6);
	// right 0264
	spec = (2 << 19) + 0 + (18 << 6) + (itemLight << 24) + hurtColor + modelAltas;
	utils::shader::addQuads(arr, {p0, p2, p6, p4}, spec, 2, 12, 0, 6);
	// left 3157
	spec += 4;
	utils::shader::addQuads(arr, {p3, p1, p5, p7}, spec, 2, 12, 0, 6);
}
