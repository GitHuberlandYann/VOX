#include "Player.hpp"
#include "utils.h"

void Player::drawHeldItem( std::vector<t_shaderInputModel>& arr, std::vector<t_shaderInput>& entityArr )
{
	if (!_chunk || _gameMode == settings::consts::gamemode::creative || Settings::Get()->getBool(settings::bools::hide_ui)) {
		return ;
	}

	glm::vec3 pos = getEyePos();
	int light = _chunk->computePosLight(pos);
	const float scale = 0.057857142857142864 * 0.5f;

	pos += _right * 8.0f * scale - settings::consts::math::world_up * 9.0f * scale + settings::consts::math::world_up * glm::sin(_walkTime * 7) * 0.03f + _right * glm::cos(_walkTime * 4) * 0.03f;
	if (_handContent == blocks::air) { // draw arm, only the two visible faces
		const int speco = settings::consts::shader::texture::player << 12;
		glm::vec3 armFront = glm::normalize(_front + settings::consts::math::world_up * 0.3f);
		glm::vec3 armRight = glm::normalize(glm::cross(armFront, settings::consts::math::world_up) - settings::consts::math::world_up * 0.3f);
		if (_armAnimation) {
			float sinaright = glm::sin(_armAnimTime * 2.857142857142857 * 3.14159265358979323);
			armFront -= armRight * sinaright * 0.5f - settings::consts::math::world_up * sinaright * 0.3f;
			armFront = glm::normalize(armFront);
			armRight = glm::normalize(glm::cross(armFront, settings::consts::math::world_up) - settings::consts::math::world_up * 0.3f);
			pos += armFront * sinaright * 0.05f;
		}
		glm::vec3 armUp = glm::normalize(glm::cross(armRight, armFront));
		// up
		glm::vec3 p0 = pos + armFront * 12.0f * scale - armRight * 2.0f * scale;
		glm::vec3 p1 = p0 + armRight * 4.0f * scale;
		glm::vec3 p2 = p0 - armFront * 12.0f * scale;
		glm::vec3 p3 = p1 - armFront * 12.0f * scale;

		int spec = speco + 48 + (32 << 6) + (light << 24);
		// utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, {(1 << 17) + (1 << 18), -4 + (1 << 18), (1 << 17) - (12 << 6), -4 - (12 << 6)}, {1 + (1 << 6), (1 << 6), 1, 0});
		utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec - 4 - (12 << 6), 4, 12, true, true);

		// left
		p1 = p0;
		p0 -= armUp * 3.0f * scale;
		p3 = p2;
		p2 -= armUp * 3.0f * scale;
		spec = speco + 52 + (32 << 6) + (light << 24);
		// utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, {(1 << 17) + (1 << 18), -4 + (1 << 18), (1 << 17) - (12 << 6), -4 - (12 << 6)}, {1 + (1 << 6), (1 << 6), 1, 0});
		utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec - 4 - (12 << 6), 4, 12, true, true);
	} else if (s_blocks[_handContent]->item3D) { //(item < blocks::poppy && item != blocks::oak_door && item != blocks::glass_pane) { // draw block
		glm::vec3 itemFront = glm::normalize(glm::vec3(glm::vec2(_front + _right * 0.5f), 0));
		glm::vec3 itemRight = glm::normalize(glm::cross(itemFront, settings::consts::math::world_up));
		if (_armAnimation) {
			float sinaright = glm::sin(_armAnimTime * 2.857142857142857 * 3.14159265358979323);
			itemFront -= itemRight * sinaright * 0.5f - settings::consts::math::world_up * sinaright * 0.3f;
			itemFront = glm::normalize(itemFront);
			itemRight = glm::normalize(glm::cross(itemFront, settings::consts::math::world_up) - settings::consts::math::world_up * 0.3f);
			pos += itemFront * sinaright * 0.05f;
		}
		glm::vec3 itemUp = glm::normalize(glm::cross(itemRight, itemFront));

		if (s_blocks[_handContent]->geometry == geometry::stairs) {
			// up
			glm::vec3 p0 = pos + settings::consts::math::world_up * (0.1f + 0.5f * _front.z) + itemFront * 12.0f * scale - itemRight * 0.25f;
			glm::vec3 p1 = p0 + itemRight * 0.25f;
			glm::vec3 p2 = p0 - itemFront * 0.125f;
			glm::vec3 p3 = p1 - itemFront * 0.125f;

			int spec = s_blocks[_handContent]->texture;
			utils::shader::addQuads(entityArr, {p0, p1, p2, p3}, spec, light, 16, 8);

			// left
			p1 = p2;
			p2 = p0 - itemUp * 0.125f;
			p3 = p1 - itemUp * 0.125f;
			utils::shader::addQuads(entityArr, {p0, p1, p2, p3}, spec, light, 8, 8);
			p0 = p2;
			p1 = p3 - itemFront * 0.125f;
			p2 = p0 - itemUp * 0.125f;
			p3 = p1 - itemUp * 0.125f;
			utils::shader::addQuads(entityArr, {p0, p1, p2, p3}, spec + (8 << 16), light, 16, 8);

			// front
			p0 = p1;
			p1 = p0 + itemRight * 0.25f;
			p2 = p3;
			p3 = p2 + itemRight * 0.25f;
			utils::shader::addQuads(entityArr, {p0, p1, p2, p3}, spec, light, 16, 8);

			// up first stair
			p2 = p0;
			p3 = p1;
			p0 += itemFront * 0.125f;
			p1 += itemFront * 0.125f;
			utils::shader::addQuads(entityArr, {p0, p1, p2, p3}, spec, light, 16, 8);

			// front second stair
			p2 = p0;
			p3 = p1;
			p0 += itemUp * 0.125f;
			p1 += itemUp * 0.125f;
			utils::shader::addQuads(entityArr, {p0, p1, p2, p3}, spec, light, 16, 8);
			return ;
		}

		// up
		glm::vec3 p0 = pos + settings::consts::math::world_up * (0.1f + 0.5f * _front.z) + itemFront * 12.0f * scale - itemRight * 0.25f;
		glm::vec3 p1 = p0 + itemRight * 0.25f;
		glm::vec3 p2 = p0 - itemFront * 0.25f;
		glm::vec3 p3 = p1 - itemFront * 0.25f;

		int spec = s_blocks[_handContent]->getTex(face_dir::plus_z);
		utils::shader::addQuads(entityArr, {p0, p1, p2, p3}, spec, light, 16, 16);

		geometry shape = s_blocks[_handContent]->geometry;
		float height = (shape == geometry::slab) ? 0.125f : (shape == geometry::trapdoor) ? 0.046875f : 0.25f;
		int yoff = (shape == geometry::slab) ? 8 : (shape == geometry::trapdoor) ? 3 : 16;
		// left
		p1 = p2;
		p2 = p0 - itemUp * height;
		p3 = p1 - itemUp * height;
		spec = s_blocks[_handContent]->getTex(face_dir::minus_x);
		utils::shader::addQuads(entityArr, {p0, p1, p2, p3}, spec, light, 16, yoff);

		// back
		p0 = p1;
		p1 = p0 + itemRight * 0.25f;
		p2 = p3;
		p3 = p2 + itemRight * 0.25f;
		utils::shader::addQuads(entityArr, {p0, p1, p2, p3}, spec, light, 16, yoff);
	} else {
		pos += _front * 0.7f + settings::consts::math::world_up * 0.2f;
		glm::vec3 right = glm::normalize(-_front + _right * 0.3f + settings::consts::math::world_up * 0.2f);
		glm::vec3 front = glm::normalize(glm::cross(settings::consts::math::world_up, right));
		if (_armAnimation) {
			float sinaright = glm::sin(_armAnimTime * 2.857142857142857 * 3.14159265358979323);
			pos -= right * sinaright * 0.5f + front * sinaright * 0.2f;
			right += settings::consts::math::world_up * sinaright * 2.0f;
			right = glm::normalize(right);
			front = glm::normalize(glm::cross(settings::consts::math::world_up, right));
		}
		glm::vec3 up = glm::normalize(glm::cross(right, front));
		utils::extrusion::drawItem3D(entityArr, _handContent, light, pos, front, right, up, 0.5f);
	}
}

void Player::drawPlayer( std::vector<t_shaderInputModel>& arr, std::vector<t_shaderInput>& entityArr )
{
	if (!_chunk) {
		return ;
	}

	// 1 model texture pxl is [1.8 / 32 =] 0.05625 meters
	const float scale = 0.05625f;
	const int speco = settings::consts::shader::texture::player << 12;

// draw head
	// draw face
	glm::vec3 pos = getEyePos();

	glm::vec3 p0 = pos + _right * 4.0f * scale + _front * 4.0f * scale + _up * 4.0f * scale;
	glm::vec3 p1 = p0 - _right * 8.0f * scale;
	glm::vec3 p2 = p0 - _up * 8.0f * scale;
	glm::vec3 p3 = p1 - _up * 8.0f * scale;

	int light = _chunk->computePosLight(pos);
	int spec = speco + 8 + (8 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, 8, 8);

	// draw neck
	glm::vec3 p4 = p1 - _front * 8.0f * scale;
	glm::vec3 p5 = p0 - _front * 8.0f * scale;
	glm::vec3 p6 = p3 - _front * 8.0f * scale;
	glm::vec3 p7 = p2 - _front * 8.0f * scale;

	spec = speco + 24 + (8 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p4, p5, p6, p7}, spec, 8, 8);

	// draw right cheek
	spec = speco + 0 + (8 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p5, p0, p7, p2}, spec, 8, 8);
	// draw left cheek
	spec = speco + 16 + (8 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p1, p4, p3, p6}, spec, 8, 8);
	// up
	spec = speco + 8 + (0 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p5, p4, p0, p1}, spec, 8, 8);
	// down
	spec = speco + 16 + (0 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p7, p6, p2, p3}, spec, 8, 8);
	

// draw torso
	glm::vec3 bodyFront = {glm::normalize(glm::vec2(_bodyFront)), 0};
	const glm::vec3 bodyRight = glm::normalize(glm::cross(bodyFront, settings::consts::math::world_up));
	const glm::vec3 bodyUp = (_sneaking) ? glm::normalize(bodyFront * 0.2558539123988453f + settings::consts::math::world_up * 0.3f) : settings::consts::math::world_up;
	bodyFront = glm::normalize(glm::cross(bodyUp, bodyRight));
	pos.z -= 4.0f * scale;
	// up
	p0 = pos + bodyRight * 4.0f * scale - bodyFront * 2.0f * scale;
	p1 = p0 - bodyRight * 8.0f * scale;
	p2 = p0 + bodyFront * 4.0f * scale;
	p3 = p1 + bodyFront * 4.0f * scale;
	spec = speco + 20 + (16 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, 8, 4);
	// down
	p4 = p2 - bodyUp * 12.0f * scale;
	p5 = p3 - bodyUp * 12.0f * scale;
	p6 = p0 - bodyUp * 12.0f * scale;
	p7 = p1 - bodyUp * 12.0f * scale;
	spec += 8;
	utils::shader::addQuads(arr, {p4, p5, p6, p7}, spec, 8, 4);
	// front 2345
	spec = speco + 20 + (20 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p2, p3, p4, p5}, spec, 8, 12);
	// back 1076
	spec += 12;
	utils::shader::addQuads(arr, {p1, p0, p7, p6}, spec, 8, 12);
	// right 0264
	spec = speco + 16 + (20 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p0, p2, p6, p4}, spec, 4, 12);
	// left 3157
	spec += 12;
	utils::shader::addQuads(arr, {p3, p1, p5, p7}, spec, 4, 12);

// draw arms and legs
	float sina = glm::sin(_walkTime * 5) * ((_sprinting) ? 1.5f : 0.5f);
	glm::vec3 armFront = glm::normalize(bodyFront + settings::consts::math::world_up * sina);
	glm::vec3 armUp = glm::normalize(glm::cross(bodyRight, armFront));
	glm::vec3 armRight = bodyRight;
	if (_armAnimation) {
		float sinaright = glm::sin(_armAnimTime * 2.857142857142857 * 3.14159265358979323);
		armFront = glm::normalize(_front - bodyRight * 6.0f * scale + settings::consts::math::world_up * sinaright * 3.0f);
		armRight = glm::normalize(glm::cross(armFront, settings::consts::math::world_up));
		armUp = glm::normalize(glm::cross(armRight, armFront));
	}

// draw right arm
	pos += armRight * 6.0f * scale;
	// up
	p0 = pos + armRight * 2.0f * scale - armFront * 2.0f * scale;
	p1 = p0 - armRight * 4.0f * scale;
	p2 = p0 + armFront * 4.0f * scale;
	p3 = p1 + armFront * 4.0f * scale;
	spec = speco + 44 + (16 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, 4, 4);
	// down
	p4 = p2 - armUp * 12.0f * scale;
	p5 = p3 - armUp * 12.0f * scale;
	p6 = p0 - armUp * 12.0f * scale;
	p7 = p1 - armUp * 12.0f * scale;
	spec += 4;
	utils::shader::addQuads(arr, {p4, p5, p6, p7}, spec, 4, 4);
	// front 2345
	spec = speco + 44 + (20 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p2, p3, p4, p5}, spec, 4, 12);
	// back 1076
	spec += 8;
	utils::shader::addQuads(arr, {p1, p0, p7, p6}, spec, 4, 12);
	// right 0264
	spec = speco + 40 + (20 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p0, p2, p6, p4}, spec, 4, 12);
	// left 3157
	spec += 8;
	utils::shader::addQuads(arr, {p3, p1, p5, p7}, spec, 4, 12);

// draw held item
	if (_handContent != blocks::air) {
		glm::vec3 itemPos = pos - armUp * 12.0f * scale;
		p0 = itemPos + armUp * 0.25f;
		p1 = itemPos + armFront * 0.25f + armUp * 0.25f;
		p2 = itemPos;
		p3 = itemPos + armFront * 0.25f;

		p4 = itemPos - armRight * 0.25f + armUp * 0.25f;
		p5 = itemPos + armFront * 0.25f - armRight * 0.25f + armUp * 0.25f;
		p6 = itemPos - armRight * 0.25f;
		p7 = itemPos + armFront * 0.25f - armRight * 0.25f;
		if (_handContent < blocks::poppy) {
			int offset = (s_blocks[_handContent]->oriented) ? face_dir::minus_x : 0;

			spec = s_blocks[_handContent]->getTex(face_dir::minus_x, offset);
			utils::shader::addQuads(entityArr, {p4, p0, p6, p2}, spec, light + (3 << 8), 16, 16);

			spec = s_blocks[_handContent]->getTex(face_dir::plus_x, offset);
			utils::shader::addQuads(entityArr, {p1, p5, p3, p7}, spec, light + (4 << 8), 16, 16);

			spec = s_blocks[_handContent]->getTex(face_dir::minus_y, offset);
			utils::shader::addQuads(entityArr, {p0, p1, p2, p3}, spec, light + (1 << 8), 16, 16);

			spec = s_blocks[_handContent]->getTex(face_dir::plus_y, offset);
			utils::shader::addQuads(entityArr, {p5, p4, p7, p6}, spec, light + (2 << 8), 16, 16);

			spec = s_blocks[_handContent]->getTex(face_dir::plus_z, offset);
			utils::shader::addQuads(entityArr, {p4, p5, p0, p1}, spec, light + (0 << 8), 16, 16);

			spec = s_blocks[_handContent]->getTex(face_dir::minus_z, offset);
			utils::shader::addQuads(entityArr, {p2, p3, p6, p7}, spec, light + (5 << 8), 16, 16);
		} else { // flowers
			if (!utils::extrusion::drawItem3D(entityArr, _handContent, light, p1 + armFront * 0.25f, -armRight, -armUp, armFront, 0.5f)) {
				spec = s_blocks[_handContent]->getTex();
				utils::shader::addQuads(entityArr, {p0, p5, p2, p7}, spec, light, 16, 16);

				utils::shader::addQuads(entityArr, {p1, p4, p3, p6}, spec, light, 16, 16);
			}
		}
	}

	pos -= armRight * 6.0f * scale;
// draw left leg // which movement is in synch with right arm
	glm::vec3 legFront = glm::normalize(glm::vec3(glm::normalize(glm::vec2(_bodyFront)), 0) + settings::consts::math::world_up * sina);
	glm::vec3 legUp = glm::normalize(glm::cross(bodyRight, legFront));
	pos -= bodyRight * 2.0f * scale;
	pos -= bodyUp * 12.0f * scale;
	// up
	p0 = pos + bodyRight * 2.0f * scale - legFront * 2.0f * scale;
	p1 = p0 - bodyRight * 4.0f * scale;
	p2 = p0 + legFront * 4.0f * scale;
	p3 = p1 + legFront * 4.0f * scale;
	spec = speco + 20 + (48 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, 4, 4);
	// down
	p4 = p2 - legUp * 12.0f * scale;
	p5 = p3 - legUp * 12.0f * scale;
	p6 = p0 - legUp * 12.0f * scale;
	p7 = p1 - legUp * 12.0f * scale;
	spec += 4;
	utils::shader::addQuads(arr, {p4, p5, p6, p7}, spec, 4, 4);
	// front 2345
	spec = speco + 20 + (52 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p2, p3, p4, p5}, spec, 4, 12);
	// back 1076
	spec += 8;
	utils::shader::addQuads(arr, {p1, p0, p7, p6}, spec, 4, 12);
	// right 0264
	spec = speco + 16 + (52 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p0, p2, p6, p4}, spec, 4, 12);
	// left 3157
	spec += 8;
	utils::shader::addQuads(arr, {p3, p1, p5, p7}, spec, 4, 12);

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
	spec = speco + 36 + (48 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, 4, 4);
	// down
	p4 = p2 - armUp * 12.0f * scale;
	p5 = p3 - armUp * 12.0f * scale;
	p6 = p0 - armUp * 12.0f * scale;
	p7 = p1 - armUp * 12.0f * scale;
	spec += 4;
	utils::shader::addQuads(arr, {p4, p5, p6, p7}, spec, 4, 4);
	// front 2345
	spec = speco + 36 + (52 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p2, p3, p4, p5}, spec, 4, 12);
	// back 1076
	spec += 8;
	utils::shader::addQuads(arr, {p1, p0, p7, p6}, spec, 4, 12);
	// right 0264
	spec = speco + 32 + (52 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p0, p2, p6, p4}, spec, 4, 12);
	// left 3157
	spec += 8;
	utils::shader::addQuads(arr, {p3, p1, p5, p7}, spec, 4, 12);

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
	spec = speco + 4 + (16 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, 4, 4);
	// down
	p4 = p2 - legUp * 12.0f * scale;
	p5 = p3 - legUp * 12.0f * scale;
	p6 = p0 - legUp * 12.0f * scale;
	p7 = p1 - legUp * 12.0f * scale;
	spec += 4;
	utils::shader::addQuads(arr, {p4, p5, p6, p7}, spec, 4, 4);
	// front 2345
	spec = speco + 4 + (20 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p2, p3, p4, p5}, spec, 4, 12);
	// back 1076
	spec += 8;
	utils::shader::addQuads(arr, {p1, p0, p7, p6}, spec, 4, 12);
	// right 0264
	spec = speco + 0 + (20 << 6) + (light << 24);
	utils::shader::addQuads(arr, {p0, p2, p6, p4}, spec, 4, 12);
	// left 3157
	spec += 8;
	utils::shader::addQuads(arr, {p3, p1, p5, p7}, spec, 4, 12);
}
