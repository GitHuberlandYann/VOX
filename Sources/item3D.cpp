#include "utils.h"
#include <map>

namespace EXTRUSION {

	typedef struct s_extrusion {
		std::vector<glm::ivec3> up;
		std::vector<glm::ivec3> down;
		std::vector<glm::ivec3> left;
		std::vector<glm::ivec3> right;
	}		t_extrusion;

	std::map<int, t_extrusion> exList;

	void init( void )
	{
		{t_extrusion tmp;
		tmp.up.push_back({8, 5, 2}); tmp.up.push_back({6, 6, 2}); tmp.up.push_back({10, 6, 1}); tmp.up.push_back({5, 7, 1}); tmp.up.push_back({5, 13, 2}); tmp.up.push_back({10, 13, 2}); tmp.up.push_back({9, 14, 1}); tmp.up.push_back({7, 15, 1});
		tmp.down.push_back({10, 7, 1}); tmp.down.push_back({5, 8, 1}); tmp.down.push_back({9, 8, 1}); tmp.down.push_back({6, 9, 2}); tmp.down.push_back({5, 13, 1}); tmp.down.push_back({10, 13, 2}); tmp.down.push_back({6, 14, 1}); tmp.down.push_back({7, 15, 3});
		tmp.left.push_back({5, 7, 2}); tmp.left.push_back({5, 13, 1}); tmp.left.push_back({6, 6, 1}); tmp.left.push_back({6, 9, 1}); tmp.left.push_back({6, 14, 1}); tmp.left.push_back({7, 15, 1}); tmp.left.push_back({8, 5, 1}); tmp.left.push_back({8, 10, 5}); tmp.left.push_back({10, 13, 1});
		tmp.right.push_back({6, 13, 2});tmp.right.push_back({8, 9, 5});tmp.right.push_back({9, 5, 1});tmp.right.push_back({9, 8, 1});tmp.right.push_back({9, 14, 2});tmp.right.push_back({10, 6, 2});tmp.right.push_back({11, 13, 1});
		exList.emplace(blocks::POPPY, tmp);}
		{t_extrusion tmp;
		tmp.up.push_back({7, 6, 2});
		tmp.down.push_back({7, 15, 2});
		tmp.left.push_back({7, 6, 10});
		tmp.right.push_back({8, 6, 10});
		exList.emplace(blocks::TORCH, tmp);}
		
		std::cout << "item3D extrusion successfully initialized" << std::endl;
	}

	/* return false if item not in map
	 * drawItem3D uses particle shader
	 * front, right, up should be normalized
	 */
	bool drawItem3D( std::vector<std::pair<int, glm::vec3>> &arr, int type, int light, glm::vec3 pos, glm::vec3 front, glm::vec3 right, glm::vec3 up, float size )
	{
		auto search = exList.find(type);
		if (search == exList.end()) {
			return (false);
		}
		light <<= 24;

		int texStartX = s_blocks[type]->texX() * 16, texStartY = s_blocks[type]->texY() * 16;
		// front face
		int spec = texStartX + (texStartY << 8) + light;
		std::pair<int, glm::vec3> v0 = {spec, pos};
		std::pair<int, glm::vec3> v1 = {spec + 16 + (1 << 17), pos + right * size};
		std::pair<int, glm::vec3> v2 = {spec + (16 << 8) + (1 << 18), pos - up * size};
		std::pair<int, glm::vec3> v3 = {spec + 16 + (1 << 17) + (16 << 8) + (1 << 18), pos + right * size - up * size};
		arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
		// back face
		spec += 16;
		v0 = {spec + (1 << 17), pos + front * size / 16.0f + right * size};
		v1 = {spec - 16, pos + front * size / 16.0f};
		v2 = {spec + (1 << 17) + (16 << 8) + (1 << 18), pos + front * size / 16.0f + right * size - up * size};
		v3 = {spec - 16 + (16 << 8) + (1 << 18), pos + front * size / 16.0f - up * size};
		arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
		// up faces
		for (auto u : search->second.up) {
			spec = texStartX + u.x + ((texStartY + u.y) << 8) + light;
			v0 = {spec, pos + front * size / 16.0f + right * size * static_cast<float>(u.x) / 16.0f - up * size * static_cast<float>(u.y) / 16.0f};
			v1 = {spec + u.z + (1 << 17), pos + front * size / 16.0f + right * size * static_cast<float>(u.x + u.z) / 16.0f - up * size * static_cast<float>(u.y) / 16.0f};
			v2 = {spec + (1 << 8) + (1 << 18), pos + right * size * static_cast<float>(u.x) / 16.0f - up * size * static_cast<float>(u.y) / 16.0f};
			v3 = {spec + u.z + (1 << 17) + (1 << 8) + (1 << 18), pos + right * size * static_cast<float>(u.x + u.z) / 16.0f - up * size * static_cast<float>(u.y) / 16.0f};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
		}
		// down faces
		for (auto d : search->second.down) {
			spec = texStartX + d.x + ((texStartY + d.y) << 8) + light;
			v0 = {spec, pos + right * size * static_cast<float>(d.x) / 16.0f - up * size * static_cast<float>(d.y + 1) / 16.0f};
			v1 = {spec + d.z + (1 << 17), pos + right * size * static_cast<float>(d.x + d.z) / 16.0f - up * size * static_cast<float>(d.y + 1) / 16.0f};
			v2 = {spec + (1 << 8) + (1 << 18), pos + front * size / 16.0f + right * size * static_cast<float>(d.x) / 16.0f - up * size * static_cast<float>(d.y + 1) / 16.0f};
			v3 = {spec + d.z + (1 << 17) + (1 << 8) + (1 << 18),  pos + front * size / 16.0f + right * size * static_cast<float>(d.x + d.z) / 16.0f - up * size * static_cast<float>(d.y + 1) / 16.0f};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
		}
		// left faces
		for (auto l : search->second.left) {
			spec = texStartX + l.x + ((texStartY + l.y) << 8) + light;
			v0 = {spec, pos + front * size / 16.0f + right * size * static_cast<float>(l.x) / 16.0f - up * size * static_cast<float>(l.y) / 16.0f};
			v1 = {spec + 1 + (1 << 17), pos + right * size * static_cast<float>(l.x) / 16.0f - up * size * static_cast<float>(l.y) / 16.0f};
			v2 = {spec + (l.z << 8) + (1 << 18), pos + front * size / 16.0f + right * size * static_cast<float>(l.x) / 16.0f - up * size * static_cast<float>(l.y + l.z) / 16.0f};
			v3 = {spec + 1 + (1 << 17) + (l.z << 8) + (1 << 18), pos + right * size * static_cast<float>(l.x) / 16.0f - up * size * static_cast<float>(l.y + l.z) / 16.0f};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
		}
		// right faces
		for (auto r : search->second.right) {
			spec = texStartX + r.x + ((texStartY + r.y) << 8) + light;
			v0 = {spec, pos + right * size * static_cast<float>(r.x + 1) / 16.0f - up * size * static_cast<float>(r.y) / 16.0f};
			v1 = {spec + 1 + (1 << 17), pos + front * size / 16.0f + right * size * static_cast<float>(r.x + 1) / 16.0f - up * size * static_cast<float>(r.y) / 16.0f};
			v2 = {spec + (r.z << 8) + (1 << 18), pos + right * size * static_cast<float>(r.x + 1) / 16.0f - up * size * static_cast<float>(r.y + r.z) / 16.0f};
			v3 = {spec + 1 + (1 << 17) + (r.z << 8) + (1 << 18), pos + front * size / 16.0f + right * size * static_cast<float>(r.x + 1) / 16.0f - up * size * static_cast<float>(r.y + r.z) / 16.0f};
			arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
		}
		return (true);
	}

}
