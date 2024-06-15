#include "Zombie.hpp"
#include "Chunk.hpp"
#include "logs.hpp"
#include "random.hpp"

Zombie::Zombie( Chunk* chunk, Camera* player, glm::vec3 position ) : AHostileMob(player, position)
{
    _chunk = chunk;

	LOG("new zombie at " << POS(_position));
}

Zombie::~Zombie( void )
{

}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Zombie::update( std::vector<t_shaderInput>& modArr, float deltaTime )
{
    if (!_chunk) {
        std::cout << "zombie is missing a _chunk" << std::endl;
        return ;
    }
    // update position
	if (!_noAI && Settings::Get()->getBool(settings::bools::mobAI)) {
		_deltaTime = deltaTime;
		_stateTime += deltaTime;

		if (_state == settings::state_machine::idle) {
			_walkTime = 0;
			move(0, false);
			if (_stateTime > 3) {
				_state = settings::state_machine::wandle;
				unsigned seed = _position.x * 14 + _position.y * 18 + _position.z * 40 + deltaTime * 100;
				_front = glm::normalize(glm::vec3(Random::randomFloat(seed) * 2.0f - 1.0f, Random::randomFloat(seed) * 2.0f - 1.0f, 0.0f));
				_right = glm::normalize(glm::cross(_front, settings::consts::math::world_up));
				_up = {0, 0, 1};
				_stateTime = 0;
			}
		} else {
			_walkTime = (_walking) ? _walkTime + deltaTime : 0;

			if (_state == settings::state_machine::chase) {
				if (_path.size() > 1) {
					// set _front according to computed path
					_front = _path[1] - _path[0];
					if (!_front.x) { _front.x = (_currentBlock.x + 0.5f) - _position.x; }
					if (!_front.y) { _front.y = (_currentBlock.y + 0.5f) - _position.y; }
					_front = glm::normalize(_front);
					_right = glm::normalize(glm::cross(_front, settings::consts::math::world_up));
					_up = glm::normalize(glm::cross(_right, _front));
					if (_front.z != 0) {
						_inJump = true;
					}
				}
			} else if (_stateTime > 6) {
				_state = settings::state_machine::idle;
				_stateTime = 0;
			}

			// move on X_AXIS
			glm::vec3 originalPos = _position;
			move(face_dir::plus_x, true);
			const float hitBoxHeight = settings::consts::hitbox::zombie;
			t_collision coll = _chunk->collisionBox(_position, 0.3f, hitBoxHeight, hitBoxHeight);
			if (coll.type != COLLISION::NONE) {
				// _ui->chatMessage("xcoll " + std::to_string(coll.type) + ", " + std::to_string(coll.minZ) + " ~ " + std::to_string(coll.maxZ) + " h " + std::to_string(hitBoxHeight));
				if (!customObstacle(coll.minZ, coll.maxZ)
					|| _chunk->collisionBox(_position, 0.3f, hitBoxHeight, hitBoxHeight).type != COLLISION::NONE) {
					restorePos(originalPos); // if collision after movement, undo movement + setRun(false)
				}
			}
			
			// move on Y_AXIS
			originalPos = _position;
			move(face_dir::plus_y, true);
			coll = _chunk->collisionBox(_position, 0.3f, hitBoxHeight, hitBoxHeight);
			if (coll.type != COLLISION::NONE) {
				if (!customObstacle(coll.minZ, coll.maxZ)
					|| _chunk->collisionBox(_position, 0.3f, hitBoxHeight, hitBoxHeight).type != COLLISION::NONE) {
					restorePos(originalPos);
				}
			}

			_chunk->applyGravity(this);

			updateCurrentBlock();
		}
	}

    // draw
    draw(modArr);
}

bool Zombie::getHitBox( void )
{
	return (settings::consts::hitbox::zombie);
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void Zombie::draw( std::vector<t_shaderInput>& arr )
{
    // 1 model texture pxl is 0.057857142857142864 meters
	const float scale = 0.057857142857142864f;

// draw head
	// draw face
	glm::vec3 pos = _position; // getEyePos();
	pos.z += 1.62f;
	// if (_smoothCam) {
	// 	pos.z = _smoothCamZ - _deltaTime * SMOOTH_CAM_SPEED;
	// }
	glm::vec3 p0 = pos + _right * 4.0f * scale + _front * 4.0f * scale + _up * 4.0f * scale;
	glm::vec3 p1 = p0 - _right * 8.0f * scale;
	glm::vec3 p2 = p0 - _up * 8.0f * scale;
	glm::vec3 p3 = p1 - _up * 8.0f * scale;

	int itemLight = _chunk->computePosLight(pos);
	int spec = (2 << 19) + 8 + (8 << 6) + (itemLight << 24);
	addQuads(arr, {p0, p1, p2, p3}, spec, 8, 8);

	// draw neck
	glm::vec3 p4 = p1 - _front * 8.0f * scale;
	glm::vec3 p5 = p0 - _front * 8.0f * scale;
	glm::vec3 p6 = p3 - _front * 8.0f * scale;
	glm::vec3 p7 = p2 - _front * 8.0f * scale;

	spec = (2 << 19) + 24 + (8 << 6) + (itemLight << 24);
	addQuads(arr, {p4, p5, p6, p7}, spec, 8, 8);
	
	// left cheek
	spec -= 8;
	addQuads(arr, {p1, p4, p3, p6}, spec, 8, 8);
	// right cheek
	spec -= 16;
	addQuads(arr, {p5, p0, p7, p2}, spec, 8, 8);

	//top
	spec += 8 - (8 << 6);
	addQuads(arr, {p5, p4, p0, p1}, spec, 8, 8);
	//bottom
	spec += 8;
	addQuads(arr, {p6, p7, p3, p2}, spec, 8, 8);

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
	spec = (2 << 19) + 20 + (16 << 6) + (itemLight << 24);
	addQuads(arr, {p0, p1, p2, p3}, spec, 8, 4);
	// down
	p4 = p2 - bodyUp * 12.0f * scale;
	p5 = p3 - bodyUp * 12.0f * scale;
	p6 = p0 - bodyUp * 12.0f * scale;
	p7 = p1 - bodyUp * 12.0f * scale;
	spec += 8;
	addQuads(arr, {p4, p5, p6, p7}, spec, 8, 4);
	// front 2345
	spec = (2 << 19) + 20 + (20 << 6) + (itemLight << 24);
	addQuads(arr, {p2, p3, p4, p5}, spec, 8, 12);
	// back 1076
	spec += 12;
	addQuads(arr, {p1, p0, p7, p6}, spec, 8, 12);
	// right 0264
	spec = (2 << 19) + 16 + (20 << 6) + (itemLight << 24);
	addQuads(arr, {p0, p2, p6, p4}, spec, 4, 12);
	// left 3157
	spec += 12;
	addQuads(arr, {p3, p1, p5, p7}, spec, 4, 12);

// draw arms and legs
	float sina = glm::sin(_walkTime * 5) * 0.5f;
	glm::vec3 armFront = glm::normalize(bodyFront + settings::consts::math::world_up * sina);
	glm::vec3 armUp = glm::normalize(glm::cross(bodyRight, armFront));
	glm::vec3 armRight = bodyRight;

// draw right arm
	pos += armRight * 6.0f * scale;
	// up
	p0 = pos + armRight * 2.0f * scale - armFront * 2.0f * scale;
	p1 = p0 - armRight * 4.0f * scale;
	p2 = p0 + armFront * 4.0f * scale;
	p3 = p1 + armFront * 4.0f * scale;
	spec = (2 << 19) + 44 + (16 << 6) + (itemLight << 24);
	addQuads(arr, {p0, p1, p2, p3}, spec, 4, 4);
	// down
	p4 = p2 - armUp * 12.0f * scale;
	p5 = p3 - armUp * 12.0f * scale;
	p6 = p0 - armUp * 12.0f * scale;
	p7 = p1 - armUp * 12.0f * scale;
	spec += 4;
	addQuads(arr, {p4, p5, p6, p7}, spec, 4, 4);
	// front 2345
	spec = (2 << 19) + 44 + (20 << 6) + (itemLight << 24);
	addQuads(arr, {p2, p3, p4, p5}, spec, 4, 12);
	// back 1076
	spec += 8;
	addQuads(arr, {p1, p0, p7, p6}, spec, 4, 12);
	// right 0264
	spec = (2 << 19) + 40 + (20 << 6) + (itemLight << 24);
	addQuads(arr, {p0, p2, p6, p4}, spec, 4, 12);
	// left 3157
	spec += 8;
	addQuads(arr, {p3, p1, p5, p7}, spec, 4, 12);

	pos -= armRight * 6.0f * scale;
// draw left leg // whose movement is in synch with right arm
	glm::vec3 legFront = glm::normalize(glm::vec3(glm::normalize(glm::vec2(_bodyFront)), 0) + settings::consts::math::world_up * sina);
	glm::vec3 legUp = glm::normalize(glm::cross(bodyRight, legFront));
	pos -= bodyRight * 2.0f * scale;
	pos -= bodyUp * 12.0f * scale;
	// up
	p0 = pos + bodyRight * 2.0f * scale - legFront * 2.0f * scale;
	p1 = p0 - bodyRight * 4.0f * scale;
	p2 = p0 + legFront * 4.0f * scale;
	p3 = p1 + legFront * 4.0f * scale;
	spec = (2 << 19) + 4 + (16 << 6) + (itemLight << 24);
	addQuads(arr, {p0, p1, p2, p3}, spec, 4, 4);
	// down
	p4 = p2 - legUp * 12.0f * scale;
	p5 = p3 - legUp * 12.0f * scale;
	p6 = p0 - legUp * 12.0f * scale;
	p7 = p1 - legUp * 12.0f * scale;
	spec += 4;
	addQuads(arr, {p4, p5, p6, p7}, spec, 4, 4);
	// front 2345
	spec = (2 << 19) + 4 + (20 << 6) + (itemLight << 24);
	addQuads(arr, {p2, p3, p4, p5}, spec, 4, 12);
	// back 1076
	spec += 8;
	addQuads(arr, {p1, p0, p7, p6}, spec, 4, 12);
	// right 0264
	spec = (2 << 19) + 0 + (20 << 6) + (itemLight << 24);
	addQuads(arr, {p0, p2, p6, p4}, spec, 4, 12);
	// left 3157
	spec += 8;
	addQuads(arr, {p3, p1, p5, p7}, spec, 4, 12);

// draw left arm
	sina = -sina;
	armFront = glm::normalize(bodyFront + settings::consts::math::world_up * sina);
	armUp = glm::normalize(glm::cross(bodyRight, armFront));
	pos -= bodyRight * 4.0f * scale;
	pos += bodyUp * 12.0f * scale;
	// up
	p0 = pos + bodyRight * 2.0f * scale - armFront * 2.0f * scale;
	p1 = p0 - bodyRight * 4.0f * scale;
	p2 = p0 + armFront * 4.0f * scale;
	p3 = p1 + armFront * 4.0f * scale;
	spec = (2 << 19) + 44 + (16 << 6) + (itemLight << 24);
	addQuads(arr, {p0, p1, p2, p3}, spec, 4, 4);
	// down
	p4 = p2 - armUp * 12.0f * scale;
	p5 = p3 - armUp * 12.0f * scale;
	p6 = p0 - armUp * 12.0f * scale;
	p7 = p1 - armUp * 12.0f * scale;
	spec += 4;
	addQuads(arr, {p4, p5, p6, p7}, spec, 4, 4);
	// front 2345
	spec = (2 << 19) + 44 + (20 << 6) + (itemLight << 24);
	addQuads(arr, {p2, p3, p4, p5}, spec, 4, 12);
	// back 1076
	spec += 8;
	addQuads(arr, {p1, p0, p7, p6}, spec, 4, 12);
	// right 0264
	spec = (2 << 19) + 40 + (20 << 6) + (itemLight << 24);
	addQuads(arr, {p0, p2, p6, p4}, spec, 4, 12);
	// left 3157
	spec += 8;
	addQuads(arr, {p3, p1, p5, p7}, spec, 4, 12);

// draw right leg
	legFront = glm::normalize(glm::vec3(glm::normalize(glm::vec2(_bodyFront)), 0) + settings::consts::math::world_up * sina);
	legUp = glm::normalize(glm::cross(bodyRight, legFront));
	pos += bodyRight * 8.0f * scale;
	pos -= bodyUp * 12.0f * scale;
	// up
	p0 = pos + bodyRight * 2.0f * scale - legFront * 2.0f * scale;
	p1 = p0 - bodyRight * 4.0f * scale;
	p2 = p0 + legFront * 4.0f * scale;
	p3 = p1 + legFront * 4.0f * scale;
	spec = (2 << 19) + 4 + (16 << 6) + (itemLight << 24);
	addQuads(arr, {p0, p1, p2, p3}, spec, 4, 4);
	// down
	p4 = p2 - legUp * 12.0f * scale;
	p5 = p3 - legUp * 12.0f * scale;
	p6 = p0 - legUp * 12.0f * scale;
	p7 = p1 - legUp * 12.0f * scale;
	spec += 4;
	addQuads(arr, {p4, p5, p6, p7}, spec, 4, 4);
	// front 2345
	spec = (2 << 19) + 4 + (20 << 6) + (itemLight << 24);
	addQuads(arr, {p2, p3, p4, p5}, spec, 4, 12);
	// back 1076
	spec += 8;
	addQuads(arr, {p1, p0, p7, p6}, spec, 4, 12);
	// right 0264
	spec = (2 << 19) + 0 + (20 << 6) + (itemLight << 24);
	addQuads(arr, {p0, p2, p6, p4}, spec, 4, 12);
	// left 3157
	spec += 8;
	addQuads(arr, {p3, p1, p5, p7}, spec, 4, 12);
}
