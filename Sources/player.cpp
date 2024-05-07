#include "Camera.hpp"
#include "utils.h"

void Camera::drawHeldItem( std::vector<t_shaderInput> &arr, int item, bool game_mode )
{
	if (_hideUI || game_mode == CREATIVE) {
		return ;
	}
	glm::vec3 pos = getCamPos();
	int itemLight = _current_chunk_ptr->computePosLight(pos) << 24;
	const float scale = 0.057857142857142864 * 0.5f;

	pos += _right * 8.0f * scale - _world_up * 9.0f * scale + _world_up * glm::sin(_walk_time * 7) * 0.03f + _right * glm::cos(_walk_time * 4) * 0.03f;
	if (item == blocks::AIR) { // draw arm, only the two visible faces
		const int speco = 0; // 64 for second skin
		glm::vec3 armFront = glm::normalize(_front + _world_up * 0.3f);
		glm::vec3 armRight = glm::normalize(glm::cross(armFront, _world_up) - _world_up * 0.3f);
		if (_armAnimation) {
			float sinaright = glm::sin(_armAnimTime * 2.857142857142857 * 3.14159265358979323);
			armFront -= armRight * sinaright * 0.5f - _world_up * sinaright * 0.3f;
			armFront = glm::normalize(armFront);
			armRight = glm::normalize(glm::cross(armFront, _world_up) - _world_up * 0.3f);
			pos += armFront * sinaright * 0.05f;
		}
		glm::vec3 armUp = glm::normalize(glm::cross(armRight, armFront));
		// up
		glm::vec3 p0 = pos + armFront * 12.0f * scale - armRight * 2.0f * scale;
		glm::vec3 p1 = p0 + armRight * 4.0f * scale;
		glm::vec3 p2 = p0 - armFront * 12.0f * scale;
		glm::vec3 p3 = p1 - armFront * 12.0f * scale;

		int spec = speco + (2 << 19) + 48 + (32 << 8) + itemLight;
		t_shaderInput v0 = {spec + (1 << 17) + (1 << 18), p0};
		t_shaderInput v1 = {spec - 4 + (1 << 18), p1};
		t_shaderInput v2 = {spec + (1 << 17) - (12 << 8), p2};
		t_shaderInput v3 = {spec - 4 - (12 << 8), p3};
		arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

		// left
		p1 = p0;
		p0 -= armUp * 3.0f * scale;
		p3 = p2;
		p2 -= armUp * 3.0f * scale;
		spec = speco + (2 << 19) + 52 + (32 << 8) + itemLight;
		v0 = {spec + (1 << 17) + (1 << 18), p0};
		v1 = {spec - 4 + (1 << 18), p1};
		v2 = {spec + (1 << 17) - (12 << 8), p2};
		v3 = {spec - 4 - (12 << 8), p3};
		arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	} else if (s_blocks[item]->item3D) { //(item < blocks::POPPY && item != blocks::OAK_DOOR && item != blocks::GLASS_PANE) { // draw block
		glm::vec3 itemFront = glm::normalize(glm::vec3(glm::vec2(_front + _right * 0.5f), 0));
		glm::vec3 itemRight = glm::normalize(glm::cross(itemFront, _world_up));
		if (_armAnimation) {
			float sinaright = glm::sin(_armAnimTime * 2.857142857142857 * 3.14159265358979323);
			itemFront -= itemRight * sinaright * 0.5f - _world_up * sinaright * 0.3f;
			itemFront = glm::normalize(itemFront);
			itemRight = glm::normalize(glm::cross(itemFront, _world_up) - _world_up * 0.3f);
			pos += itemFront * sinaright * 0.05f;
		}
		glm::vec3 itemUp = glm::normalize(glm::cross(itemRight, itemFront));

		if (item == blocks::OAK_STAIRS) {
			// up
			glm::vec3 p0 = pos + _world_up * (0.1f + 0.5f * _front.z) + itemFront * 12.0f * scale - itemRight * 0.25f;
			glm::vec3 p1 = p0 + itemRight * 0.25f;
			glm::vec3 p2 = p0 - itemFront * 0.125f;
			glm::vec3 p3 = p1 - itemFront * 0.125f;

			int spec = s_blocks[item]->texX() * 16 + ((s_blocks[item]->texY() * 16) << 8) + itemLight;
			t_shaderInput v0 = {spec, p0};
			t_shaderInput v1 = {spec + 16 + (1 << 17), p1};
			t_shaderInput v2 = {spec + (8 << 8) + (1 << 18), p2};
			t_shaderInput v3 = {spec + 16 + (1 << 17) + (8 << 8) + (1 << 18), p3};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

			// left
			p1 = p2;
			p2 = p0 - itemUp * 0.125f;
			p3 = p1 - itemUp * 0.125f;
			v0 = {spec, p0};
			v1 = {spec + 8 + (1 << 17), p1};
			v2 = {spec + (8 << 8) + (1 << 18), p2};
			v3 = {spec + 8 + (1 << 17) + (8 << 8) + (1 << 18), p3};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
			p0 = p2;
			p1 = p3 - itemFront * 0.125f;
			p2 = p0 - itemUp * 0.125f;
			p3 = p1 - itemUp * 0.125f;
			v0 = {spec + (8 << 8), p0};
			v1 = {spec + 16 + (1 << 17) + (8 << 8), p1};
			v2 = {spec + (16 << 8) + (1 << 18), p2};
			v3 = {spec + 16 + (1 << 17) + (16 << 8) + (1 << 18), p3};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

			// front
			p0 = p1;
			p1 = p0 + itemRight * 0.25f;
			p2 = p3;
			p3 = p2 + itemRight * 0.25f;
			v0 = {spec, p0};
			v1 = {spec + 16 + (1 << 17), p1};
			v2 = {spec + (8 << 8) + (1 << 18), p2};
			v3 = {spec + 16 + (1 << 17) + (8 << 8) + (1 << 18), p3};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

			// up first stair
			p2 = p0;
			p3 = p1;
			p0 += itemFront * 0.125f;
			p1 += itemFront * 0.125f;
			v0 = {spec, p0};
			v1 = {spec + 16 + (1 << 17), p1};
			v2 = {spec + (8 << 8) + (1 << 18), p2};
			v3 = {spec + 16 + (1 << 17) + (8 << 8) + (1 << 18), p3};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

			// front second stair
			p2 = p0;
			p3 = p1;
			p0 += itemUp * 0.125f;
			p1 += itemUp * 0.125f;
			v0 = {spec, p0};
			v1 = {spec + 16 + (1 << 17), p1};
			v2 = {spec + (8 << 8) + (1 << 18), p2};
			v3 = {spec + 16 + (1 << 17) + (8 << 8) + (1 << 18), p3};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
			return ;
		}

		// up
		glm::vec3 p0 = pos + _world_up * (0.1f + 0.5f * _front.z) + itemFront * 12.0f * scale - itemRight * 0.25f;
		glm::vec3 p1 = p0 + itemRight * 0.25f;
		glm::vec3 p2 = p0 - itemFront * 0.25f;
		glm::vec3 p3 = p1 - itemFront * 0.25f;

		int spec = s_blocks[item]->texX(face_dir::PLUSZ) * 16 + ((s_blocks[item]->texY(face_dir::PLUSZ) * 16) << 8) + itemLight;
		t_shaderInput v0 = {spec, p0};
		t_shaderInput v1 = {spec + 16 + (1 << 17), p1};
		t_shaderInput v2 = {spec + (16 << 8) + (1 << 18), p2};
		t_shaderInput v3 = {spec + 16 + (1 << 17) + (16 << 8) + (1 << 18), p3};
		arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
		
		float height = (item == blocks::OAK_SLAB) ? 0.125f : (item == blocks::OAK_TRAPDOOR) ? 0.046875f : 0.25f;
		int yoff = (item == blocks::OAK_SLAB) ? 8 : (item == blocks::OAK_TRAPDOOR) ? 3 : 16;
		// left
		p1 = p2;
		p2 = p0 - itemUp * height;
		p3 = p1 - itemUp * height;
		spec = s_blocks[item]->texX(face_dir::MINUSX) * 16 + ((s_blocks[item]->texY(face_dir::MINUSX) * 16) << 8) + itemLight;
		v0 = {spec, p0};
		v1 = {spec + 16 + (1 << 17), p1};
		v2 = {spec + (yoff << 8) + (1 << 18), p2};
		v3 = {spec + 16 + (1 << 17) + (yoff << 8) + (1 << 18), p3};
		arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

		// back
		p0 = p1;
		p1 = p0 + itemRight * 0.25f;
		p2 = p3;
		p3 = p2 + itemRight * 0.25f;
		v0 = {spec, p0};
		v1 = {spec + 16 + (1 << 17), p1};
		v2 = {spec + (yoff << 8) + (1 << 18), p2};
		v3 = {spec + 16 + (1 << 17) + (yoff << 8) + (1 << 18), p3};
		arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	} else {
		pos += _front * 0.7f + _world_up * 0.2f;
		glm::vec3 right = glm::normalize(-_front + _right * 0.3f + _world_up * 0.2f);
		glm::vec3 front = glm::normalize(glm::cross(_world_up, right));
		if (_armAnimation) {
			float sinaright = glm::sin(_armAnimTime * 2.857142857142857 * 3.14159265358979323);
			pos -= right * sinaright * 0.5f + front * sinaright * 0.2f;
			right += _world_up * sinaright * 2.0f;
			right = glm::normalize(right);
			front = glm::normalize(glm::cross(_world_up, right));
		}
		glm::vec3 up = glm::normalize(glm::cross(right, front));
		EXTRUSION::drawItem3D(arr, item, itemLight >> 24, pos, front, right, up, 0.5f);
	}
}

void Camera::drawPlayer( std::vector<t_shaderInput> &arr, int item, bool game_mode )
{
	if (!_current_chunk_ptr) {
		return ;
	}
	if (_camPlacement == CAMPLACEMENT::DEFAULT) {
		return (drawHeldItem(arr, item, game_mode));
	}

	// 1 model texture pxl is 0.057857142857142864 meters
	const float scale = 0.057857142857142864;
	const int speco = 0; // 64 for second skin

// draw head
	// draw face
	glm::vec3 pos = getEyePos();
	if (_smoothCam) {
		pos.z = _smoothCamZ - _deltaTime * SMOOTH_CAM_SPEED;
	}
	glm::vec3 p0 = pos + _right * 4.0f * scale + _front * 4.0f * scale + _up * 4.0f * scale;
	glm::vec3 p1 = p0 - _right * 8.0f * scale;
	glm::vec3 p2 = p0 - _up * 8.0f * scale;
	glm::vec3 p3 = p1 - _up * 8.0f * scale;

	int itemLight = _current_chunk_ptr->computePosLight(pos);
	int spec = speco + (2 << 19) + 8 + (8 << 8) + (itemLight << 24);
	t_shaderInput v0 = {spec, p0};
	t_shaderInput v1 = {spec + 8 + (1 << 17), p1};
	t_shaderInput v2 = {spec + (8 << 8) + (1 << 18), p2};
	t_shaderInput v3 = {spec + 8 + (1 << 17) + (8 << 8) + (1 << 18), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	// draw neck
	glm::vec3 p4 = p1 - _front * 8.0f * scale;
	glm::vec3 p5 = p0 - _front * 8.0f * scale;
	glm::vec3 p6 = p3 - _front * 8.0f * scale;
	glm::vec3 p7 = p2 - _front * 8.0f * scale;

	spec = speco + (2 << 19) + 24 + (8 << 8) + (itemLight << 24);
	v0 = {spec, p4};
	v1 = {spec + 8 + (1 << 17), p5};
	v2 = {spec + (8 << 8) + (1 << 18), p6};
	v3 = {spec + 8 + (1 << 17) + (8 << 8) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	// cheecks, top and bottom of head don't need to be displayed as there's no way to see them right now
	// will get a chance to look at them from inventory once implemented

// draw torso
	glm::vec3 bodyFront = {glm::normalize(glm::vec2(_bodyFront)), 0};
	const glm::vec3 bodyRight = glm::normalize(glm::cross(_bodyFront, _world_up));
	const glm::vec3 bodyUp = (_sneaking) ? glm::normalize(bodyFront * 0.2558539123988453f + _world_up * 0.3f) : _world_up;
	bodyFront = glm::normalize(glm::cross(bodyUp, bodyRight));
	pos.z -= 4.0f * scale;
	// up
	p0 = pos + bodyRight * 4.0f * scale - bodyFront * 2.0f * scale;
	p1 = p0 - bodyRight * 8.0f * scale;
	p2 = p0 + bodyFront * 4.0f * scale;
	p3 = p1 + bodyFront * 4.0f * scale;
	spec = speco + (2 << 19) + 20 + (16 << 8) + (itemLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 8 + (1 << 17), p1};
	v2 = {spec + (4 << 8) + (1 << 18), p2};
	v3 = {spec + 8 + (1 << 17) + (4 << 8) + (1 << 18), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// down
	p4 = p2 - bodyUp * 12.0f * scale;
	p5 = p3 - bodyUp * 12.0f * scale;
	p6 = p0 - bodyUp * 12.0f * scale;
	p7 = p1 - bodyUp * 12.0f * scale;
	spec += 8;
	v0 = {spec, p4};
	v1 = {spec + 8 + (1 << 17), p5};
	v2 = {spec + (4 << 8) + (1 << 18), p6};
	v3 = {spec + 8 + (1 << 17) + (4 << 8) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// front 2345
	spec = speco + (2 << 19) + 20 + (20 << 8) + (itemLight << 24);
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
	spec = speco + (2 << 19) + 16 + (20 << 8) + (itemLight << 24);
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
	glm::vec3 armRight = bodyRight;
	if (_armAnimation) {
		float sinaright = glm::sin(_armAnimTime * 2.857142857142857 * 3.14159265358979323);
		armFront = glm::normalize(_front - bodyRight * 6.0f * scale + _world_up * sinaright * 3.0f);
		armRight = glm::normalize(glm::cross(armFront, _world_up));
		armUp = glm::normalize(glm::cross(armRight, armFront));
	}

// draw right arm
	pos += armRight * 6.0f * scale;
	// up
	p0 = pos + armRight * 2.0f * scale - armFront * 2.0f * scale;
	p1 = p0 - armRight * 4.0f * scale;
	p2 = p0 + armFront * 4.0f * scale;
	p3 = p1 + armFront * 4.0f * scale;
	spec = speco + (2 << 19) + 44 + (16 << 8) + (itemLight << 24);
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
	spec = speco + (2 << 19) + 44 + (20 << 8) + (itemLight << 24);
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
	spec = speco + (2 << 19) + 40 + (20 << 8) + (itemLight << 24);
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

// draw held item
	if (item != blocks::AIR) {
		glm::vec3 itemPos = pos - armUp * 12.0f * scale;
		p0 = itemPos + armUp * 0.25f;
		p1 = itemPos + armFront * 0.25f + armUp * 0.25f;
		p2 = itemPos;
		p3 = itemPos + armFront * 0.25f;

		p4 = itemPos - armRight * 0.25f + armUp * 0.25f;
		p5 = itemPos + armFront * 0.25f - armRight * 0.25f + armUp * 0.25f;
		p6 = itemPos - armRight * 0.25f;
		p7 = itemPos + armFront * 0.25f - armRight * 0.25f;
		if (item < blocks::POPPY) {
			int offset = ((item >= blocks::CRAFTING_TABLE && item < blocks::BEDROCK) ? face_dir::MINUSX: 0);

			spec = 16 * s_blocks[item]->texX(face_dir::MINUSX, offset) + (16 * s_blocks[item]->texY(face_dir::MINUSX, offset) << 8) + (itemLight << 24);
			v0 = {spec, p4};
			v1 = {spec + 16 + (1 << 17), p0};
			v2 = {spec + (16 << 8) + (1 << 18), p6};
			v3 = {spec + 16 + (1 << 17) + (16 << 8) + (1 << 18), p2};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

			spec = 16 * s_blocks[item]->texX(face_dir::PLUSX, offset) + (16 * s_blocks[item]->texY(face_dir::PLUSX, offset) << 8) + (itemLight << 24);
			v0 = {spec, p1};
			v1 = {spec + 16 + (1 << 17), p5};
			v2 = {spec + (16 << 8) + (1 << 18), p3};
			v3 = {spec + 16 + (1 << 17) + (16 << 8) + (1 << 18), p7};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

			spec = 16 * s_blocks[item]->texX(face_dir::MINUSY, offset) + (16 * s_blocks[item]->texY(face_dir::MINUSY, offset) << 8) + (itemLight << 24);
			v0 = {spec, p0};
			v1 = {spec + 16 + (1 << 17), p1};
			v2 = {spec + (16 << 8) + (1 << 18), p2};
			v3 = {spec + 16 + (1 << 17) + (16 << 8) + (1 << 18), p3};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

			spec = 16 * s_blocks[item]->texX(face_dir::PLUSY, offset) + (16 * s_blocks[item]->texY(face_dir::PLUSY, offset) << 8) + (itemLight << 24);
			v0 = {spec, p5};
			v1 = {spec + 16 + (1 << 17), p4};
			v2 = {spec + (16 << 8) + (1 << 18), p7};
			v3 = {spec + 16 + (1 << 17) + (16 << 8) + (1 << 18), p6};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

			spec = 16 * s_blocks[item]->texX(face_dir::PLUSZ, offset) + (16 * s_blocks[item]->texY(face_dir::PLUSZ, offset) << 8) + (itemLight << 24);
			v0 = {spec, p4};
			v1 = {spec + 16 + (1 << 17), p5};
			v2 = {spec + (16 << 8) + (1 << 18), p0};
			v3 = {spec + 16 + (1 << 17) + (16 << 8) + (1 << 18), p1};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

			spec = 16 * s_blocks[item]->texX(face_dir::MINUSZ, offset) + (16 * s_blocks[item]->texY(face_dir::MINUSZ, offset) << 8) + (itemLight << 24);
			v0 = {spec, p2};
			v1 = {spec + 16 + (1 << 17), p3};
			v2 = {spec + (16 << 8) + (1 << 18), p6};
			v3 = {spec + 16 + (1 << 17) + (16 << 8) + (1 << 18), p7};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
		} else { // flowers
			if (1 && EXTRUSION::drawItem3D(arr, item, itemLight, p1 + armFront * 0.25f, -armRight, -armUp, armFront, 0.5f)) { // TODO replace 1 by var toggle fancy_item

			} else {
				spec = 16 * s_blocks[item]->texX() + (16 * s_blocks[item]->texY() << 8) + (itemLight << 24);
				v0 = {spec, p0};
				v1 = {spec + 16 + (1 << 17), p5};
				v2 = {spec + (16 << 8) + (1 << 18), p2};
				v3 = {spec + 16 + (1 << 17) + (16 << 8) + (1 << 18), p7};
				arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

				v0 = {spec, p1};
				v1 = {spec + 16 + (1 << 17), p4};
				v2 = {spec + (16 << 8) + (1 << 18), p3};
				v3 = {spec + 16 + (1 << 17) + (16 << 8) + (1 << 18), p6};
				arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
			}
		}
	}

	pos -= armRight * 6.0f * scale;
// draw left leg // which movement is in synch with right arm
	glm::vec3 legFront = glm::normalize(glm::vec3(glm::normalize(glm::vec2(_bodyFront)), 0) + _world_up * sina);
	glm::vec3 legUp = glm::normalize(glm::cross(bodyRight, legFront));
	pos -= bodyRight * 2.0f * scale;
	pos -= bodyUp * 12.0f * scale;
	// up
	p0 = pos + bodyRight * 2.0f * scale - legFront * 2.0f * scale;
	p1 = p0 - bodyRight * 4.0f * scale;
	p2 = p0 + legFront * 4.0f * scale;
	p3 = p1 + legFront * 4.0f * scale;
	spec = speco + (2 << 19) + 20 + (48 << 8) + (itemLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (4 << 8) + (1 << 18), p2};
	v3 = {spec + 4 + (1 << 17) + (4 << 8) + (1 << 18), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// down
	p4 = p2 - legUp * 12.0f * scale;
	p5 = p3 - legUp * 12.0f * scale;
	p6 = p0 - legUp * 12.0f * scale;
	p7 = p1 - legUp * 12.0f * scale;
	spec += 4;
	v0 = {spec, p4};
	v1 = {spec + 4 + (1 << 17), p5};
	v2 = {spec + (4 << 8) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (4 << 8) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// front 2345
	spec = speco + (2 << 19) + 20 + (52 << 8) + (itemLight << 24);
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
	spec = speco + (2 << 19) + 16 + (52 << 8) + (itemLight << 24);
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
	pos += bodyUp * 12.0f * scale;
	// up
	p0 = pos + bodyRight * 2.0f * scale - armFront * 2.0f * scale;
	p1 = p0 - bodyRight * 4.0f * scale;
	p2 = p0 + armFront * 4.0f * scale;
	p3 = p1 + armFront * 4.0f * scale;
	spec = speco + (2 << 19) + 36 + (48 << 8) + (itemLight << 24);
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
	spec = speco + (2 << 19) + 36 + (52 << 8) + (itemLight << 24);
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
	spec = speco + (2 << 19) + 32 + (52 << 8) + (itemLight << 24);
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
	legFront = glm::normalize(glm::vec3(glm::normalize(glm::vec2(_bodyFront)), 0) + _world_up * sina);
	legUp = glm::normalize(glm::cross(bodyRight, legFront));
	pos += bodyRight * 8.0f * scale;
	pos -= bodyUp * 12.0f * scale;
	// up
	p0 = pos + bodyRight * 2.0f * scale - legFront * 2.0f * scale;
	p1 = p0 - bodyRight * 4.0f * scale;
	p2 = p0 + legFront * 4.0f * scale;
	p3 = p1 + legFront * 4.0f * scale;
	spec = speco + (2 << 19) + 4 + (16 << 8) + (itemLight << 24);
	v0 = {spec, p0};
	v1 = {spec + 4 + (1 << 17), p1};
	v2 = {spec + (4 << 8) + (1 << 18), p2};
	v3 = {spec + 4 + (1 << 17) + (4 << 8) + (1 << 18), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// down
	p4 = p2 - legUp * 12.0f * scale;
	p5 = p3 - legUp * 12.0f * scale;
	p6 = p0 - legUp * 12.0f * scale;
	p7 = p1 - legUp * 12.0f * scale;
	spec += 4;
	v0 = {spec, p4};
	v1 = {spec + 4 + (1 << 17), p5};
	v2 = {spec + (4 << 8) + (1 << 18), p6};
	v3 = {spec + 4 + (1 << 17) + (4 << 8) + (1 << 18), p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	// front 2345
	spec = speco + (2 << 19) + 4 + (20 << 8) + (itemLight << 24);
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
	spec = speco + (2 << 19) + 0 + (20 << 8) + (itemLight << 24);
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
