#include "Zombie.hpp"
#include "Chunk.hpp"

Zombie::Zombie( Chunk* chunk, glm::ivec3 position )
{
    _position = position;
    _chunk = chunk;
}

Zombie::~Zombie( void )
{

}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Zombie::update( std::vector<t_shaderInput>& modArr, double deltaTime )
{
    if (!_chunk) {
        std::cout << "zombie is missing a _chunk" << std::endl;
        return ;
    }
    // update position
	_walkTime += deltaTime;

    // draw
    draw(modArr);
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void Zombie::draw( std::vector<t_shaderInput>& arr )
{
    // 1 model texture pxl is 0.057857142857142864 meters
	const float scale = 0.057857142857142864;

// draw head
	// draw face
	glm::vec3 pos = _position + 1.62f; // getEyePos();
	// if (_smoothCam) {
	// 	pos.z = _smoothCamZ - _deltaTime * SMOOTH_CAM_SPEED;
	// }
	glm::vec3 p0 = pos + _right * 4.0f * scale + _front * 4.0f * scale + _up * 4.0f * scale;
	glm::vec3 p1 = p0 - _right * 8.0f * scale;
	glm::vec3 p2 = p0 - _up * 8.0f * scale;
	glm::vec3 p3 = p1 - _up * 8.0f * scale;

	int itemLight = _chunk->computePosLight(pos);
	int spec = (2 << 19) + 8 + (8 << 6) + (itemLight << 24);
	t_shaderInput v0 = {spec, p0};
	t_shaderInput v1 = {spec + 8 + (1 << 17), p1};
	t_shaderInput v2 = {spec + (8 << 6) + (1 << 18), p2};
	t_shaderInput v3 = {spec + 8 + (1 << 17) + (8 << 6) + (1 << 18), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	// draw neck
	glm::vec3 p4 = p1 - _front * 8.0f * scale;
	glm::vec3 p5 = p0 - _front * 8.0f * scale;
	glm::vec3 p6 = p3 - _front * 8.0f * scale;
	glm::vec3 p7 = p2 - _front * 8.0f * scale;

	spec = (2 << 19) + 24 + (8 << 6) + (itemLight << 24);
	v0 = {spec, p4};
	v1 = {spec + 8 + (1 << 17), p5};
	v2 = {spec + (8 << 6) + (1 << 18), p6};
	v3 = {spec + 8 + (1 << 17) + (8 << 6) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	
	// left cheek
	spec -= 8;
	v0 = {spec, p1};
	v1 = {spec + 8 + (1 << 17), p4};
	v2 = {spec + (8 << 6) + (1 << 18), p3};
	v3 = {spec + 8 + (1 << 17) + (8 << 6) + (1 << 18), p6};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// right cheek
	spec -= 16;
	v0 = {spec, p5};
	v1 = {spec + 8 + (1 << 17), p0};
	v2 = {spec + (8 << 6) + (1 << 18), p7};
	v3 = {spec + 8 + (1 << 17) + (8 << 6) + (1 << 18), p2};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	//top
	spec += 8 - (8 << 6);
	v0 = {spec, p5};
	v1 = {spec + 8 + (1 << 17), p4};
	v2 = {spec + (8 << 6) + (1 << 18), p0};
	v3 = {spec + 8 + (1 << 17) + (8 << 6) + (1 << 18), p1};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	//bottom
	spec += 8;
	v0 = {spec, p6};
	v1 = {spec + 8 + (1 << 17), p7};
	v2 = {spec + (8 << 6) + (1 << 18), p3};
	v3 = {spec + 8 + (1 << 17) + (8 << 6) + (1 << 18), p2};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

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
	v0 = {spec, p0};
	v1 = {spec + 8 + (1 << 17), p1};
	v2 = {spec + (4 << 6) + (1 << 18), p2};
	v3 = {spec + 8 + (1 << 17) + (4 << 6) + (1 << 18), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// down
	p4 = p2 - bodyUp * 12.0f * scale;
	p5 = p3 - bodyUp * 12.0f * scale;
	p6 = p0 - bodyUp * 12.0f * scale;
	p7 = p1 - bodyUp * 12.0f * scale;
	spec += 8;
	v0 = {spec, p4};
	v1 = {spec + 8 + (1 << 17), p5};
	v2 = {spec + (4 << 6) + (1 << 18), p6};
	v3 = {spec + 8 + (1 << 17) + (4 << 6) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// front 2345
	spec = (2 << 19) + 20 + (20 << 6) + (itemLight << 24);
	v0 = {spec, p2};
	v1 = {spec + 8 + (1 << 17), p3};
	v2 = {spec + (12 << 6) + (1 << 18), p4};
	v3 = {spec + 8 + (1 << 17) + (12 << 6) + (1 << 18), p5};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// back 1076
	spec += 12;
	v0 = {spec, p1};
	v1 = {spec + 8 + (1 << 17), p0};
	v2 = {spec + (12 << 6) + (1 << 18), p7};
	v3 = {spec + 8 + (1 << 17) + (12 << 6) + (1 << 18), p6};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// right 0264
	spec = (2 << 19) + 16 + (20 << 6) + (itemLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p2};
	v2 = {spec + (12 << 6) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (12 << 6) + (1 << 18), p4};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// left 3157
	spec += 12;
	v0 = {spec, p3};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (12 << 6) + (1 << 18), p5};
	v3 = {spec + 4 + (1 << 17) + (12 << 6) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

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
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (4 << 6) + (1 << 18), p2};
	v3 = {spec + 4 + (1 << 17) + (4 << 6) + (1 << 18), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// down
	p4 = p2 - armUp * 12.0f * scale;
	p5 = p3 - armUp * 12.0f * scale;
	p6 = p0 - armUp * 12.0f * scale;
	p7 = p1 - armUp * 12.0f * scale;
	spec += 4;
	v0 = {spec, p4};
	v1 = {spec + 4 + (1 << 17), p5};
	v2 = {spec + (4 << 6) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (4 << 6) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// front 2345
	spec = (2 << 19) + 44 + (20 << 6) + (itemLight << 24);
	v0 = {spec, p2};
	v1 = {spec + 4 + (1 << 17), p3};
	v2 = {spec + (12 << 6) + (1 << 18), p4};
	v3 = {spec + 4 + (1 << 17) + (12 << 6) + (1 << 18), p5};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// back 1076
	spec += 8;
	v0 = {spec, p1};
	v1 = {spec + 4 + (1 << 17), p0};
	v2 = {spec + (12 << 6) + (1 << 18), p7};
	v3 = {spec + 4 + (1 << 17) + (12 << 6) + (1 << 18), p6};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// right 0264
	spec = (2 << 19) + 40 + (20 << 6) + (itemLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p2};
	v2 = {spec + (12 << 6) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (12 << 6) + (1 << 18), p4};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// left 3157
	spec += 8;
	v0 = {spec, p3};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (12 << 6) + (1 << 18), p5};
	v3 = {spec + 4 + (1 << 17) + (12 << 6) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

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
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (4 << 6) + (1 << 18), p2};
	v3 = {spec + 4 + (1 << 17) + (4 << 6) + (1 << 18), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// down
	p4 = p2 - legUp * 12.0f * scale;
	p5 = p3 - legUp * 12.0f * scale;
	p6 = p0 - legUp * 12.0f * scale;
	p7 = p1 - legUp * 12.0f * scale;
	spec += 4;
	v0 = {spec, p4};
	v1 = {spec + 4 + (1 << 17), p5};
	v2 = {spec + (4 << 6) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (4 << 6) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// front 2345
	spec = (2 << 19) + 4 + (20 << 6) + (itemLight << 24);
	v0 = {spec, p2};
	v1 = {spec + 4 + (1 << 17), p3};
	v2 = {spec + (12 << 6) + (1 << 18), p4};
	v3 = {spec + 4 + (1 << 17) + (12 << 6) + (1 << 18), p5};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// back 1076
	spec += 8;
	v0 = {spec, p1};
	v1 = {spec + 4 + (1 << 17), p0};
	v2 = {spec + (12 << 6) + (1 << 18), p7};
	v3 = {spec + 4 + (1 << 17) + (12 << 6) + (1 << 18), p6};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// right 0264
	spec = (2 << 19) + 0 + (20 << 6) + (itemLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p2};
	v2 = {spec + (12 << 6) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (12 << 6) + (1 << 18), p4};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// left 3157
	spec += 8;
	v0 = {spec, p3};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (12 << 6) + (1 << 18), p5};
	v3 = {spec + 4 + (1 << 17) + (12 << 6) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

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
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (4 << 6) + (1 << 18), p2};
	v3 = {spec + 4 + (1 << 17) + (4 << 6) + (1 << 18), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// down
	p4 = p2 - armUp * 12.0f * scale;
	p5 = p3 - armUp * 12.0f * scale;
	p6 = p0 - armUp * 12.0f * scale;
	p7 = p1 - armUp * 12.0f * scale;
	spec += 4;
	v0 = {spec, p4};
	v1 = {spec + 4 + (1 << 17), p5};
	v2 = {spec + (4 << 6) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (4 << 6) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// front 2345
	spec = (2 << 19) + 44 + (20 << 6) + (itemLight << 24);
	v0 = {spec, p2};
	v1 = {spec + 4 + (1 << 17), p3};
	v2 = {spec + (12 << 6) + (1 << 18), p4};
	v3 = {spec + 4 + (1 << 17) + (12 << 6) + (1 << 18), p5};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// back 1076
	spec += 8;
	v0 = {spec, p1};
	v1 = {spec + 4 + (1 << 17), p0};
	v2 = {spec + (12 << 6) + (1 << 18), p7};
	v3 = {spec + 4 + (1 << 17) + (12 << 6) + (1 << 18), p6};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// right 0264
	spec = (2 << 19) + 40 + (20 << 6) + (itemLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p2};
	v2 = {spec + (12 << 6) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (12 << 6) + (1 << 18), p4};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// left 3157
	spec += 8;
	v0 = {spec, p3};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (12 << 6) + (1 << 18), p5};
	v3 = {spec + 4 + (1 << 17) + (12 << 6) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

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
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (4 << 6) + (1 << 18), p2};
	v3 = {spec + 4 + (1 << 17) + (4 << 6) + (1 << 18), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// down
	p4 = p2 - legUp * 12.0f * scale;
	p5 = p3 - legUp * 12.0f * scale;
	p6 = p0 - legUp * 12.0f * scale;
	p7 = p1 - legUp * 12.0f * scale;
	spec += 4;
	v0 = {spec, p4};
	v1 = {spec + 4 + (1 << 17), p5};
	v2 = {spec + (4 << 6) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (4 << 6) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// front 2345
	spec = (2 << 19) + 4 + (20 << 6) + (itemLight << 24);
	v0 = {spec, p2};
	v1 = {spec + 4 + (1 << 17), p3};
	v2 = {spec + (12 << 6) + (1 << 18), p4};
	v3 = {spec + 4 + (1 << 17) + (12 << 6) + (1 << 18), p5};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// back 1076
	spec += 8;
	v0 = {spec, p1};
	v1 = {spec + 4 + (1 << 17), p0};
	v2 = {spec + (12 << 6) + (1 << 18), p7};
	v3 = {spec + 4 + (1 << 17) + (12 << 6) + (1 << 18), p6};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// right 0264
	spec = (2 << 19) + 0 + (20 << 6) + (itemLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p2};
	v2 = {spec + (12 << 6) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (12 << 6) + (1 << 18), p4};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// left 3157
	spec += 8;
	v0 = {spec, p3};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (12 << 6) + (1 << 18), p5};
	v3 = {spec + 4 + (1 << 17) + (12 << 6) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
}
