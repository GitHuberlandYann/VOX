#include "utils.h"
#include "Particle.hpp"
#include "Chunk.hpp"
#include "random.hpp"

Particle::Particle( Chunk* chunk, glm::vec3 pos, int type, float shade, int block )
	: _pos(pos), _lifeTime(0), _shade(shade), _type(type), _frame(0), _block(block), _chunk(chunk)
{
	switch (type) {
		case particles::breaking:
			_texOffset = {(pos.x - glm::floor(pos.x)) * 14, (pos.y - glm::floor(pos.y)) * 14};
		case particles::smoke:
			unsigned& seed = _chunk->getSeed();
			_dir = {Random::randomFloat(seed), Random::randomFloat(seed), 2};
			_dir = glm::normalize(_dir);
		break ;
	}
}

Particle::~Particle( void )
{

}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

bool Particle::updateFlame( std::vector<t_shaderInputPart>& arr, glm::vec3 camDir )
{
	while (_lifeTime > settings::consts::tick * 4) {
		_lifeTime -= settings::consts::tick * 4;
		++_frame;
		if (_frame == 3 || _frame == 6 || _frame == 10) {
			_chunk->addParticle({_pos.x, _pos.y, _pos.z - 0.1f}, particles::smoke, 0.05f);
		}
	}

	float size, time = _frame * settings::consts::tick * 4 + _lifeTime;
	if (_frame < 4) {
		size = utils::math::gradient(time, 0, 4 * settings::consts::tick * 4, 0.125, 0.1);
	} else if (_frame < 6) {
		size = utils::math::gradient(time, 4 * settings::consts::tick * 4, 6 * settings::consts::tick * 4, 0.1125, 0.1);
	} else {
		size = utils::math::gradient(time, 6 * settings::consts::tick * 4, 14 * settings::consts::tick * 4, 0.1125, 0);
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
	int spec = (1 << 19) + (160 << 8) + (itemLight << 24);
	utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, 8, 8);
	return (false);
}

bool Particle::updateSmoke( std::vector<t_shaderInputPart>& arr, glm::vec3 camDir, float deltaTime )
{
	_pos += _dir * deltaTime;
	while (_lifeTime > settings::consts::tick) {
		_lifeTime -= settings::consts::tick;
		++_frame;
	}
	if (_frame > 7) {
		return (true);
	}

	glm::vec3 hdir = glm::normalize(glm::cross(camDir, glm::vec3(0, 0, 1)));
	glm::vec3 up = glm::normalize(glm::cross(hdir, camDir));

	glm::vec3 p0 = {_pos.x - hdir.x * _shade + up.x * _shade, _pos.y - hdir.y * _shade + up.y * _shade, _pos.z + _shade * up.z};
	glm::vec3 p1 = {_pos.x + hdir.x * _shade + up.x * _shade, _pos.y + hdir.y * _shade + up.y * _shade, _pos.z + _shade * up.z};
	glm::vec3 p2 = {_pos.x - hdir.x * _shade - up.x * _shade, _pos.y - hdir.y * _shade - up.y * _shade, _pos.z - _shade * up.z};
	glm::vec3 p3 = {_pos.x + hdir.x * _shade - up.x * _shade, _pos.y + hdir.y * _shade - up.y * _shade, _pos.z - _shade * up.z};

	int itemLight = 0;
	int spec = (1 << 19) + (7 - _frame) * 8 + (168 << 8) + (itemLight << 24);
	utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, 8, 8);
	return (false);
}

bool Particle::updateExplosion( std::vector<t_shaderInputPart>& arr, glm::vec3 camDir )
{
	while (_lifeTime > settings::consts::tick) {
		_lifeTime -= settings::consts::tick;
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
	itemLight = (static_cast<int>((itemLight >> 4) * _shade) << 4) + (static_cast<int>((itemLight & 0xF) * _shade));
	int spec = (1 << 19) + ((_frame & 0x3) * 32) + (((_frame >> 2) * 32) << 8) + (itemLight << 24);
	utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, 32, 32);
	return (false);
}

bool Particle::updateBreaking( std::vector<t_shaderInput>& arr, glm::vec3 camDir, float deltaTime )
{
	_pos += _dir * deltaTime;
	_dir.z -= 0.1f;
	while (_lifeTime > settings::consts::tick) {
		_lifeTime -= settings::consts::tick;
		++_frame;
	}
	if (_frame > 15) {
		return (true);
	}

	glm::vec3 hdir = glm::normalize(glm::cross(camDir, glm::vec3(0, 0, 1)));
	glm::vec3 up = glm::normalize(glm::cross(hdir, camDir));

	float size = 0.033f;
	glm::vec3 p0 = {_pos.x - hdir.x * size + up.x * size, _pos.y - hdir.y * size + up.y * size, _pos.z + size * up.z};
	glm::vec3 p1 = {_pos.x + hdir.x * size + up.x * size, _pos.y + hdir.y * size + up.y * size, _pos.z + size * up.z};
	glm::vec3 p2 = {_pos.x - hdir.x * size - up.x * size, _pos.y - hdir.y * size - up.y * size, _pos.z - size * up.z};
	glm::vec3 p3 = {_pos.x + hdir.x * size - up.x * size, _pos.y + hdir.y * size - up.y * size, _pos.z - size * up.z};

	int light = _chunk->computePosLight(_pos);
	int spec = s_blocks[_block]->getTex() + (_texOffset.x << 12) + (_texOffset.y << 16);
	utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, light, 2, 2);
	return (false);
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

bool Particle::update( std::vector<t_shaderInput>& entityArr, std::vector<t_shaderInputPart>& partArr, glm::vec3 camPos, glm::vec3 camDir, double deltaTime )
{
	_lifeTime += deltaTime;

	// particles not displayed if 16 blocks away from player (TODO Entity Distance in settings)
	if (_pos.x < camPos.x - 3 * settings::consts::chunk_size || _pos.x > camPos.x + 3 * settings::consts::chunk_size || _pos.y < camPos.y - 3 * settings::consts::chunk_size || _pos.y > camPos.y + 3 * settings::consts::chunk_size
		|| _pos.z < camPos.z - 3 * settings::consts::chunk_size || _pos.z > camPos.z + 3 * settings::consts::chunk_size) {
		return (false);
	}

	switch (_type) {
		case particles::flame:
			return (updateFlame(partArr, camDir));
		case particles::smoke:
			return (updateSmoke(partArr, camDir, deltaTime));
		case particles::explosion:
			return (updateExplosion(partArr, camDir));
		case particles::breaking:
			return (updateBreaking(entityArr, camDir, deltaTime));
	}
	return (false);
}
