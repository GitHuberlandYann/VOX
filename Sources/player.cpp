#include "Camera.hpp"
#include "utils.h"

void Camera::drawPlayer( std::vector<std::pair<int, glm::vec3>> &arr )
{
	if (_camPlacement == CAMPLACEMENT::DEFAULT || !_current_chunk_ptr) {
		return ;
	}

	// 1 model texture pxl is 0.057857142857142864 meters
	const float scale = 0.057857142857142864;

// draw head
	// draw face
	glm::vec3 pos = _position;
	pos.z += 1 + EYE_LEVEL;
	glm::vec3 p0 = pos + _right * 4.0f * scale + _front * 4.0f * scale + _up * 4.0f * scale;
	glm::vec3 p1 = p0 - _right * 8.0f * scale;
	glm::vec3 p2 = p0 - _up * 8.0f * scale;
	glm::vec3 p3 = p1 - _up * 8.0f * scale;

	int itemLight = _current_chunk_ptr->computePosLight(pos);
	int spec = (2 << 19) + 8 + (8 << 8) + (itemLight << 24);
	std::pair<int, glm::vec3> v0 = {spec, p0};
	std::pair<int, glm::vec3> v1 = {spec + 8 + (1 << 17), p1};
	std::pair<int, glm::vec3> v2 = {spec + (8 << 8) + (1 << 18), p2};
	std::pair<int, glm::vec3> v3 = {spec + 8 + (1 << 17) + (8 << 8) + (1 << 18), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	// draw neck
	glm::vec3 p4 = p1 - _front * 8.0f * scale;
	glm::vec3 p5 = p0 - _front * 8.0f * scale;
	glm::vec3 p6 = p3 - _front * 8.0f * scale;
	glm::vec3 p7 = p2 - _front * 8.0f * scale;

	spec = (2 << 19) + 24 + (8 << 8) + (itemLight << 24);
	v0 = {spec, p4};
	v1 = {spec + 8 + (1 << 17), p5};
	v2 = {spec + (8 << 8) + (1 << 18), p6};
	v3 = {spec + 8 + (1 << 17) + (8 << 8) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	// cheecks, top and bottom of head don't need to be displayed as there's no way to see them right now
	// will get a chance to look at them from inventory once implemented

// draw torso
	const glm::vec3 bodyFront = {glm::normalize(glm::vec2(_bodyFront)), 0}; // TODO use different _front than head to have offset between the two
	const glm::vec3 bodyRight = glm::normalize(glm::cross(_bodyFront, _world_up));
	pos.z -= 4.0f * scale;
	// up
	p0 = pos + bodyRight * 4.0f * scale - bodyFront * 2.0f * scale;
	p1 = p0 - bodyRight * 8.0f * scale;
	p2 = p0 + bodyFront * 4.0f * scale;
	p3 = p1 + bodyFront * 4.0f * scale;
	spec = (2 << 19) + 20 + (16 << 8) + (itemLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 8 + (1 << 17), p1};
	v2 = {spec + (4 << 8) + (1 << 18), p2};
	v3 = {spec + 8 + (1 << 17) + (4 << 8) + (1 << 18), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// down
	p4 = p2; p4.z -= 12.0f * scale;
	p5 = p3; p5.z -= 12.0f * scale;
	p6 = p0; p6.z -= 12.0f * scale;
	p7 = p1; p7.z -= 12.0f * scale;
	spec += 8;
	v0 = {spec, p4};
	v1 = {spec + 8 + (1 << 17), p5};
	v2 = {spec + (4 << 8) + (1 << 18), p6};
	v3 = {spec + 8 + (1 << 17) + (4 << 8) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// front 2345
	spec = (2 << 19) + 20 + (20 << 8) + (itemLight << 24);
	v0 = {spec, p2};
	v1 = {spec + 8 + (1 << 17), p3};
	v2 = {spec + (12 << 8) + (1 << 18), p4};
	v3 = {spec + 8 + (1 << 17) + (12 << 8) + (1 << 18), p5};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// back 1076
	spec += 12;
	v0 = {spec, p1};
	v1 = {spec + 8 + (1 << 17), p0};
	v2 = {spec + (12 << 8) + (1 << 18), p7};
	v3 = {spec + 8 + (1 << 17) + (12 << 8) + (1 << 18), p6};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// right 0264
	spec = (2 << 19) + 16 + (20 << 8) + (itemLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p2};
	v2 = {spec + (12 << 8) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (12 << 8) + (1 << 18), p4};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// left 3157
	spec += 12;
	v0 = {spec, p3};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (12 << 8) + (1 << 18), p5};
	v3 = {spec + 4 + (1 << 17) + (12 << 8) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

// draw arms and legs
	float sina = glm::sin(_walk_time * 5) * ((_sprinting) ? 1.5f : 0.5f);
	glm::vec3 armFront = glm::normalize(bodyFront + _world_up * sina);
	glm::vec3 armUp = glm::normalize(glm::cross(bodyRight, armFront));

// draw right arm
	pos += bodyRight * 6.0f * scale;
	// up
	p0 = pos + bodyRight * 2.0f * scale - armFront * 2.0f * scale;
	p1 = p0 - bodyRight * 4.0f * scale;
	p2 = p0 + armFront * 4.0f * scale;
	p3 = p1 + armFront * 4.0f * scale;
	spec = (2 << 19) + 44 + (16 << 8) + (itemLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (4 << 8) + (1 << 18), p2};
	v3 = {spec + 4 + (1 << 17) + (4 << 8) + (1 << 18), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// down
	p4 = p2 - armUp * 12.0f * scale;
	p5 = p3 - armUp * 12.0f * scale;
	p6 = p0 - armUp * 12.0f * scale;
	p7 = p1 - armUp * 12.0f * scale;
	spec += 4;
	v0 = {spec, p4};
	v1 = {spec + 4 + (1 << 17), p5};
	v2 = {spec + (4 << 8) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (4 << 8) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// front 2345
	spec = (2 << 19) + 44 + (20 << 8) + (itemLight << 24);
	v0 = {spec, p2};
	v1 = {spec + 4 + (1 << 17), p3};
	v2 = {spec + (12 << 8) + (1 << 18), p4};
	v3 = {spec + 4 + (1 << 17) + (12 << 8) + (1 << 18), p5};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// back 1076
	spec += 8;
	v0 = {spec, p1};
	v1 = {spec + 4 + (1 << 17), p0};
	v2 = {spec + (12 << 8) + (1 << 18), p7};
	v3 = {spec + 4 + (1 << 17) + (12 << 8) + (1 << 18), p6};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// right 0264
	spec = (2 << 19) + 40 + (20 << 8) + (itemLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p2};
	v2 = {spec + (12 << 8) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (12 << 8) + (1 << 18), p4};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// left 3157
	spec += 8;
	v0 = {spec, p3};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (12 << 8) + (1 << 18), p5};
	v3 = {spec + 4 + (1 << 17) + (12 << 8) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

// draw left leg // which movement is in synch with right arm
	pos -= bodyRight * 8.0f * scale;
	pos.z -= 12.0f * scale;
	// up
	p0 = pos + bodyRight * 2.0f * scale - armFront * 2.0f * scale;
	p1 = p0 - bodyRight * 4.0f * scale;
	p2 = p0 + armFront * 4.0f * scale;
	p3 = p1 + armFront * 4.0f * scale;
	spec = (2 << 19) + 20 + (48 << 8) + (itemLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (4 << 8) + (1 << 18), p2};
	v3 = {spec + 4 + (1 << 17) + (4 << 8) + (1 << 18), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// down
	p4 = p2 - armUp * 12.0f * scale;
	p5 = p3 - armUp * 12.0f * scale;
	p6 = p0 - armUp * 12.0f * scale;
	p7 = p1 - armUp * 12.0f * scale;
	spec += 4;
	v0 = {spec, p4};
	v1 = {spec + 4 + (1 << 17), p5};
	v2 = {spec + (4 << 8) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (4 << 8) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// front 2345
	spec = (2 << 19) + 20 + (52 << 8) + (itemLight << 24);
	v0 = {spec, p2};
	v1 = {spec + 4 + (1 << 17), p3};
	v2 = {spec + (12 << 8) + (1 << 18), p4};
	v3 = {spec + 4 + (1 << 17) + (12 << 8) + (1 << 18), p5};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// back 1076
	spec += 8;
	v0 = {spec, p1};
	v1 = {spec + 4 + (1 << 17), p0};
	v2 = {spec + (12 << 8) + (1 << 18), p7};
	v3 = {spec + 4 + (1 << 17) + (12 << 8) + (1 << 18), p6};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// right 0264
	spec = (2 << 19) + 16 + (52 << 8) + (itemLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p2};
	v2 = {spec + (12 << 8) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (12 << 8) + (1 << 18), p4};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// left 3157
	spec += 8;
	v0 = {spec, p3};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (12 << 8) + (1 << 18), p5};
	v3 = {spec + 4 + (1 << 17) + (12 << 8) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

// draw left arm
	sina = -sina;
	armFront = glm::normalize(bodyFront + _world_up * sina);
	armUp = glm::normalize(glm::cross(bodyRight, armFront));
	pos -= bodyRight * 4.0f * scale;
	pos.z += 12.0f * scale;
	// up
	p0 = pos + bodyRight * 2.0f * scale - armFront * 2.0f * scale;
	p1 = p0 - bodyRight * 4.0f * scale;
	p2 = p0 + armFront * 4.0f * scale;
	p3 = p1 + armFront * 4.0f * scale;
	spec = (2 << 19) + 36 + (48 << 8) + (itemLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (4 << 8) + (1 << 18), p2};
	v3 = {spec + 4 + (1 << 17) + (4 << 8) + (1 << 18), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// down
	p4 = p2 - armUp * 12.0f * scale;
	p5 = p3 - armUp * 12.0f * scale;
	p6 = p0 - armUp * 12.0f * scale;
	p7 = p1 - armUp * 12.0f * scale;
	spec += 4;
	v0 = {spec, p4};
	v1 = {spec + 4 + (1 << 17), p5};
	v2 = {spec + (4 << 8) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (4 << 8) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// front 2345
	spec = (2 << 19) + 36 + (52 << 8) + (itemLight << 24);
	v0 = {spec, p2};
	v1 = {spec + 4 + (1 << 17), p3};
	v2 = {spec + (12 << 8) + (1 << 18), p4};
	v3 = {spec + 4 + (1 << 17) + (12 << 8) + (1 << 18), p5};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// back 1076
	spec += 8;
	v0 = {spec, p1};
	v1 = {spec + 4 + (1 << 17), p0};
	v2 = {spec + (12 << 8) + (1 << 18), p7};
	v3 = {spec + 4 + (1 << 17) + (12 << 8) + (1 << 18), p6};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// right 0264
	spec = (2 << 19) + 32 + (52 << 8) + (itemLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p2};
	v2 = {spec + (12 << 8) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (12 << 8) + (1 << 18), p4};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// left 3157
	spec += 8;
	v0 = {spec, p3};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (12 << 8) + (1 << 18), p5};
	v3 = {spec + 4 + (1 << 17) + (12 << 8) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

// draw right leg
	pos += bodyRight * 8.0f * scale;
	pos.z -= 12.0f * scale;
	// up
	p0 = pos + bodyRight * 2.0f * scale - armFront * 2.0f * scale;
	p1 = p0 - bodyRight * 4.0f * scale;
	p2 = p0 + armFront * 4.0f * scale;
	p3 = p1 + armFront * 4.0f * scale;
	spec = (2 << 19) + 4 + (16 << 8) + (itemLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (4 << 8) + (1 << 18), p2};
	v3 = {spec + 4 + (1 << 17) + (4 << 8) + (1 << 18), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// down
	p4 = p2 - armUp * 12.0f * scale;
	p5 = p3 - armUp * 12.0f * scale;
	p6 = p0 - armUp * 12.0f * scale;
	p7 = p1 - armUp * 12.0f * scale;
	spec += 4;
	v0 = {spec, p4};
	v1 = {spec + 4 + (1 << 17), p5};
	v2 = {spec + (4 << 8) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (4 << 8) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// front 2345
	spec = (2 << 19) + 4 + (20 << 8) + (itemLight << 24);
	v0 = {spec, p2};
	v1 = {spec + 4 + (1 << 17), p3};
	v2 = {spec + (12 << 8) + (1 << 18), p4};
	v3 = {spec + 4 + (1 << 17) + (12 << 8) + (1 << 18), p5};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// back 1076
	spec += 8;
	v0 = {spec, p1};
	v1 = {spec + 4 + (1 << 17), p0};
	v2 = {spec + (12 << 8) + (1 << 18), p7};
	v3 = {spec + 4 + (1 << 17) + (12 << 8) + (1 << 18), p6};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// right 0264
	spec = (2 << 19) + 0 + (20 << 8) + (itemLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p2};
	v2 = {spec + (12 << 8) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (12 << 8) + (1 << 18), p4};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// left 3157
	spec += 8;
	v0 = {spec, p3};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (12 << 8) + (1 << 18), p5};
	v3 = {spec + 4 + (1 << 17) + (12 << 8) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
}
