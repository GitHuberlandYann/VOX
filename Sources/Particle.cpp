#include "utils.h"
#include "Particle.hpp"
#include "Chunk.hpp"

Particle::Particle( Chunk *chunk, glm::vec3 pos, int type )
	: _pos(pos), _lifeTime(0), _type(type), _frame(0), _chunk(chunk)
{
	(void)_dir;
	if (type == PARTICLES::FLAME) {
		std::cout << "new flame part at " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
	}
}

Particle::~Particle( void )
{

}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

bool Particle::updateFlame( std::vector<std::pair<int, glm::vec3>> &arr, glm::vec3 camDir )
{
	while (_lifeTime > TICK * 4) {
		_lifeTime -= TICK * 4;
		++_frame;
	}

	float size, time = _frame * TICK * 4 + _lifeTime;
	if (_frame < 4) {
		size = gradient(time, 0, 4 * TICK * 4, 0.125, 0.1);
	} else if (_frame < 6) {
		size = gradient(time, 4 * TICK * 4, 6 * TICK * 4, 0.1125, 0.1);
	} else {
		size = gradient(time, 6 * TICK * 4, 14 * TICK * 4, 0.1125, 0);
		if (_frame > 15) {
			_frame = 0;
		}
	}

	glm::vec3 hdir = glm::normalize(glm::cross(camDir, glm::vec3(0, 0, 1)));
	glm::vec3 up = glm::normalize(glm::cross(hdir, camDir));

	glm::vec3 p0 = {_pos.x - hdir.x * size + up.x * size, _pos.y - hdir.y * size + up.y * size, _pos.z + size * up.z};
	glm::vec3 p1 = {_pos.x + hdir.x * size + up.x * size, _pos.y + hdir.y * size + up.y * size, _pos.z + size * up.z};
	glm::vec3 p2 = {_pos.x - hdir.x * size - up.x * size, _pos.y - hdir.y * size - up.y * size, _pos.z - size * up.z};
	glm::vec3 p3 = {_pos.x + hdir.x * size - up.x * size, _pos.y + hdir.y * size - up.y * size, _pos.z - size * up.z};

	int itemLight = 15;
	int spec = (1 << 16) + (96 << 8) + (itemLight << 24);
	std::pair<int, glm::vec3> v0 = {spec, p0};
	std::pair<int, glm::vec3> v1 = {spec + 8 + (1 << 17), p1};
	std::pair<int, glm::vec3> v2 = {spec + (8 << 8) + (1 << 18), p2};
	std::pair<int, glm::vec3> v3 = {spec + 8 + (1 << 17) + (8 << 8) + (1 << 18), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	return (false);
}

bool Particle::updateExplosion( std::vector<std::pair<int, glm::vec3>> &arr, glm::vec3 camDir )
{
	while (_lifeTime > TICK) {
		_lifeTime -= TICK;
		++_frame;
	}
	if (_frame > 15) {
		return (true);
	}

	glm::vec3 hdir = glm::normalize(glm::cross(camDir, glm::vec3(0, 0, 1)));
	glm::vec3 up = glm::normalize(glm::cross(hdir, camDir));

	glm::vec3 p0 = {_pos.x - hdir.x * 0.5f + up.x * 0.5f, _pos.y - hdir.y * 0.5f + up.y * 0.5f, _pos.z + 0.5f * up.z};
	glm::vec3 p1 = {_pos.x + hdir.x * 0.5f + up.x * 0.5f, _pos.y + hdir.y * 0.5f + up.y * 0.5f, _pos.z + 0.5f * up.z};
	glm::vec3 p2 = {_pos.x - hdir.x * 0.5f - up.x * 0.5f, _pos.y - hdir.y * 0.5f - up.y * 0.5f, _pos.z - 0.5f * up.z};
	glm::vec3 p3 = {_pos.x + hdir.x * 0.5f - up.x * 0.5f, _pos.y + hdir.y * 0.5f - up.y * 0.5f, _pos.z - 0.5f * up.z};

	int itemLight = _chunk->computePosLight(_pos);
// std::cout << "for frame " << _frame << ": " << ((_frame & 0x7) * 32) << ", " << (((_frame > 7) ? 32 : 0)) << std::endl;
	int spec = (1 << 16) + ((_frame & 0x7) * 32) + (((_frame > 7) ? 32 : 0) << 8) + (itemLight << 24);
	std::pair<int, glm::vec3> v0 = {spec, p0};
	std::pair<int, glm::vec3> v1 = {spec + 31 + (1 << 17), p1}; // TODO change texture and put 32 here
	std::pair<int, glm::vec3> v2 = {spec + (32 << 8) + (1 << 18), p2};
	std::pair<int, glm::vec3> v3 = {spec + 31 + (1 << 17) + (32 << 8) + (1 << 18), p3};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	return (false);
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

bool Particle::update( std::vector<std::pair<int, glm::vec3>> &arr, glm::vec3 camPos, glm::vec3 camDir, double deltaTime )
{
	_lifeTime += deltaTime;

	// particles not displayed if 16 blocks away from player (TODO Entity Distance in settings)
	if (_pos.x < camPos.x - 3 * CHUNK_SIZE || _pos.x > camPos.x + 3 * CHUNK_SIZE || _pos.y < camPos.y - 3 * CHUNK_SIZE || _pos.y > camPos.y + 3 * CHUNK_SIZE
		|| _pos.z < camPos.z - 3 * CHUNK_SIZE || _pos.z > camPos.z + 3 * CHUNK_SIZE) {
		return (false);
	}

	if (_type == PARTICLES::FLAME) {
		return (updateFlame(arr, camDir));
	} else if (_type == PARTICLES::EXPLOSION) {
		return (updateExplosion(arr, camDir));
	}
	return (false);
}
