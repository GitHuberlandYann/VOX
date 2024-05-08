#include "Chunk.hpp"

ChestInstance::ChestInstance( Chunk *chunk, glm::ivec3 pos, int orientation )
	: _orientation(orientation), _state(chest_state::CLOSED), _timer(0), _pos(pos), _chunk(chunk)
{
	if (chunk) {
		_chunk_pos = {chunk->getStartX(), chunk->getStartY()};
	}
	_content.fill({0, 0});
}

ChestInstance::~ChestInstance( void )
{

}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void ChestInstance::display_open( std::vector<t_shaderInput> &arr )
{
	int itemLight = _chunk->computePosLight(_pos);

	// bottom box 
	glm::vec3 pos, right, back;
	switch (_orientation) {
		case face_dir::MINUSY:
			pos   = {_pos.x + ONE16TH,        _pos.y + ONE16TH,        _pos.z + 0};
			right = {14.0f * ONE16TH,        0,                        0};
			back  = {0,                      14.0f * ONE16TH,          0};
			break ;
		case face_dir::PLUSY:
			pos   = {_pos.x + 1.0f - ONE16TH, _pos.y + 1.0f - ONE16TH, _pos.z + 0};
			right = {-14.0f * ONE16TH,       0,                        0};
			back  = {0,                      -14.0f * ONE16TH,         0};
			break ;
		case face_dir::PLUSX:
			pos   = {_pos.x + 1.0f - ONE16TH, _pos.y + ONE16TH,        _pos.z + 0};
			right = {0,                      14.0f * ONE16TH,          0};
			back  = {-14.0f * ONE16TH,       0,                        0};
			break ;
		case face_dir::MINUSX:
			pos   = {_pos.x + ONE16TH,       _pos.y + 1.0f - ONE16TH,  _pos.z + 0};
			right = {0,                      -14.0f * ONE16TH,         0};
			back  = {14.0f * ONE16TH,        0,                        0};
			break ;
		default:
			std::cout << "ERROR DISPLAY OPEN CHEST" << std::endl;
			return ;
	}
	glm::vec3 p0 = {pos.x + 0,       pos.y + 0      , pos.z + 10.0f * ONE16TH};
	glm::vec3 p1 = {pos.x + right.x, pos.y + right.y, pos.z + 10.0f * ONE16TH};
	glm::vec3 p2 = {pos.x + 0,       pos.y + 0,       pos.z + 0};
	glm::vec3 p3 = {pos.x + right.x, pos.y + right.y, pos.z + 0};

	glm::vec3 p4 = {pos.x + back.x + 0,       pos.y + back.y + 0      , pos.z + 10.0f * ONE16TH};
	glm::vec3 p5 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 10.0f * ONE16TH};
	glm::vec3 p6 = {pos.x + back.x + 0,       pos.y + back.y + 0,       pos.z + 0};
	glm::vec3 p7 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 0};

    int spec = (1 << 4) + (15 << 12) + (3 << 19) + (itemLight << 24);
    t_shaderInput v0 = {spec + 1 + (5 << 8), p0};
    t_shaderInput v1 = {spec - 1 + XTEX + (5 << 8), p1};
    t_shaderInput v2 = {spec + 1 + (1 << 18) + (15 << 8), p2};
    t_shaderInput v3 = {spec - 1 + XTEX + (1 << 18) + (15 << 8), p3};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 12) + (4 << 19) + (itemLight << 24);
    v0 = {spec + 1 + (5 << 8), p5};
    v1 = {spec - 1 + XTEX + (5 << 8), p4};
    v2 = {spec + 1 + (1 << 18) + (15 << 8), p7};
    v3 = {spec - 1 + XTEX + (1 << 18) + (15 << 8), p6};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 12) + (1 << 19) + (itemLight << 24);
    v0 = {spec + 1 + (5 << 8), p4};
    v1 = {spec - 1 + XTEX + (5 << 8), p0};
    v2 = {spec + 1 + (1 << 18) + (15 << 8), p6};
    v3 = {spec - 1 + XTEX + (1 << 18) + (15 << 8), p2};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 12) + (2 << 19) + (itemLight << 24);
    v0 = {spec + 1 + (5 << 8), p1};
    v1 = {spec - 1 + XTEX + (5 << 8), p5};
    v2 = {spec + 1 + (1 << 18) + (15 << 8), p3};
    v3 = {spec - 1 + XTEX + (1 << 18) + (15 << 8), p7};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// +z
	spec = (3 << 4) + (15 << 12) + (0 << 19) + (itemLight << 24);
    v0 = {spec + 1, p4};
    v1 = {spec - 1 + XTEX, p5};
    v2 = {spec + 1 + (1 << 18) + (14 << 8), p0};
    v3 = {spec - 1 + XTEX + (1 << 18) + (14 << 8), p1};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// -z
	if (!air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z - 1), true), false, false, false)) {
		spec = (2 << 4) + (15 << 12) + (5 << 19) + (itemLight << 24);
		v0 = {spec + 1, p2};
		v1 = {spec - 1 + XTEX, p3};
		v2 = {spec + 1 + (1 << 18) + (14 << 8), p6};
		v3 = {spec - 1 + XTEX + (1 << 18) + (14 << 8), p7};
		arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	}

	// top box
	pos += back;
	back *= (5 / 14.0f);
	p0 = {pos.x + 0,       pos.y + 0      , pos.z + 24.0f * ONE16TH};
	p1 = {pos.x + right.x, pos.y + right.y, pos.z + 24.0f * ONE16TH};
	p2 = {pos.x + 0,       pos.y + 0,       pos.z + 10.0f * ONE16TH};
	p3 = {pos.x + right.x, pos.y + right.y, pos.z + 10.0f * ONE16TH};

	p4 = {pos.x + back.x + 0,       pos.y + back.y + 0      , pos.z + 24.0f * ONE16TH};
	p5 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 24.0f * ONE16TH};
	p6 = {pos.x + back.x + 0,       pos.y + back.y + 0,       pos.z + 10.0f * ONE16TH};
	p7 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 10.0f * ONE16TH};

    spec = (4 << 4) + (15 << 12) + (5 << 19) + (itemLight << 24);
    v0 = {spec + 1, p0};
    v1 = {spec - 1 + XTEX, p1};
    v2 = {spec + 1 + (1 << 18) + (14 << 8), p2};
    v3 = {spec - 1 + XTEX + (1 << 18) + (14 << 8), p3};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (2 << 4) + (15 << 12) + (0 << 19) + (itemLight << 24);
    v0 = {spec + 1, p6};
    v1 = {spec - 1 + XTEX, p7};
    v2 = {spec + 1 + (1 << 18) + (14 << 8), p4};
    v3 = {spec - 1 + XTEX + (1 << 18) + (14 << 8), p5};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 12) + (1 << 19) + (itemLight << 24);
    v0 = {spec + 1, p6};
    v1 = {spec - 1 + XTEX, p4};
    v2 = {spec + 1 + (1 << 18) + (5 << 8), p2};
    v3 = {spec - 1 + XTEX + (1 << 18) + (5 << 8), p0};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 12) + (2 << 19) + (itemLight << 24);
    v0 = {spec + 1, p5};
    v1 = {spec - 1 + XTEX, p7};
    v2 = {spec + 1 + (1 << 18) + (5 << 8), p1};
    v3 = {spec - 1 + XTEX + (1 << 18) + (5 << 8), p3};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// +z
	spec = (1 << 4) + (15 << 12) + (3 << 19) + (itemLight << 24);
    v0 = {spec + 1, p4};
    v1 = {spec - 1 + XTEX, p5};
    v2 = {spec + 1 + (1 << 18) + (5 << 8), p0};
    v3 = {spec - 1 + XTEX + (1 << 18) + (5 << 8), p1};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// -z
	spec = 0 + (15 << 12) + (2 << 19) + (itemLight << 24);
	v0 = {spec + 1, p7};
	v1 = {spec - 1 + XTEX, p6};
	v2 = {spec + 1 + (1 << 18) + (5 << 8), p3};
	v3 = {spec - 1 + XTEX + (1 << 18) + (5 << 8), p2};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	const float ONE14TH = 1.0f / 14.0f;
	// adding lock
	p0 += glm::vec3( right.x * 6.0f * ONE14TH + back.x * 0.4f,  right.y * 6.0f * ONE14TH + back.y * 0.4f, ONE16TH);
	p1 += glm::vec3(-right.x * 6.0f * ONE14TH + back.x * 0.4f, -right.y * 6.0f * ONE14TH + back.y * 0.4f, ONE16TH);
	p2 += glm::vec3( right.x * 6.0f * ONE14TH - back.x * 0.4f,  right.y * 6.0f * ONE14TH - back.y * 0.4f, 15.0f * ONE16TH);
	p3 += glm::vec3(-right.x * 6.0f * ONE14TH - back.x * 0.4f, -right.y * 6.0f * ONE14TH - back.y * 0.4f, 15.0f * ONE16TH);

	p4 += glm::vec3( right.x * 6.0f * ONE14TH - back.x * 0.6f,  right.y * 6.0f * ONE14TH - back.y * 0.6f, 0);
	p5 += glm::vec3(-right.x * 6.0f * ONE14TH - back.x * 0.6f, -right.y * 6.0f * ONE14TH - back.y * 0.6f, 0);
	p6 += glm::vec3( right.x * 6.0f * ONE14TH - back.x * 1.4f,  right.y * 6.0f * ONE14TH - back.y * 1.4f, 14.0f * ONE16TH);
	p7 += glm::vec3(-right.x * 6.0f * ONE14TH - back.x * 1.4f, -right.y * 6.0f * ONE14TH - back.y * 1.4f, 14.0f * ONE16TH);

    spec = (5 << 4) + (15 << 12) + (3 << 19) + (itemLight << 24); // front
    v0 = {spec, p0};
    v1 = {spec - 14 + XTEX, p1};
    v2 = {spec - (12 << 8) + YTEX, p2};
    v3 = {spec - 14 + XTEX - (12 << 8) + YTEX, p3};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (5 << 4) + (15 << 12) + (4 << 19) + (itemLight << 24); // back
    v0 = {spec - 14 + XTEX, p5};
    v1 = {spec, p4};
    v2 = {spec - 14 + XTEX - (12 << 8) + YTEX, p7};
    v3 = {spec - (12 << 8) + YTEX, p6};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (5 << 4) + (15 << 12) + (1 << 19) + (itemLight << 24); // left side
    v0 = {spec, p4};
    v1 = {spec - 15 + XTEX, p0};
    v2 = {spec - (12 << 8) + YTEX, p6};
    v3 = {spec - 15 + XTEX - (12 << 8) + YTEX, p2};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (5 << 4) + (15 << 12) + (2 << 19) + (itemLight << 24); // right side
    v0 = {spec + 1, p1};
    v1 = {spec - 14 + XTEX, p5};
    v2 = {spec + 1 - (12 << 8) + YTEX, p3};
    v3 = {spec - 14 + XTEX - (12 << 8) + YTEX, p7};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// +z
	spec = (5 << 4) + (15 << 12) + (0 << 19) + (itemLight << 24);
    v0 = {spec, p4};
    v1 = {spec - 14 + XTEX, p5};
    v2 = {spec - (15 << 8) + YTEX, p0};
    v3 = {spec - 14 + XTEX - (15 << 8) + YTEX, p1};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// -z
	spec = (5 << 4) + (15 << 12) + (5 << 19) + (itemLight << 24);
	v0 = {spec + (3 << 8), p2};
	v1 = {spec - 14 + XTEX + (3 << 8), p3};
	v2 = {spec - (12 << 8) + YTEX, p6};
	v3 = {spec - 14 + XTEX - (12 << 8) + YTEX, p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
}

void ChestInstance::display_moving( std::vector<t_shaderInput> &arr )
{
	int itemLight = _chunk->computePosLight(_pos);

	// bottom box -> same as display_open
	glm::vec3 pos, right, back;
	switch (_orientation) {
		case face_dir::MINUSY:
			pos   = {_pos.x + ONE16TH,        _pos.y + ONE16TH,        _pos.z + 0};
			right = {14.0f * ONE16TH,        0,                        0};
			back  = {0,                      14.0f * ONE16TH,          0};
			break ;
		case face_dir::PLUSY:
			pos   = {_pos.x + 1.0f - ONE16TH, _pos.y + 1.0f - ONE16TH, _pos.z + 0};
			right = {-14.0f * ONE16TH,       0,                        0};
			back  = {0,                      -14.0f * ONE16TH,         0};
			break ;
		case face_dir::PLUSX:
			pos   = {_pos.x + 1.0f - ONE16TH, _pos.y + ONE16TH,        _pos.z + 0};
			right = {0,                      14.0f * ONE16TH,          0};
			back  = {-14.0f * ONE16TH,       0,                        0};
			break ;
		case face_dir::MINUSX:
			pos   = {_pos.x + ONE16TH,       _pos.y + 1.0f - ONE16TH,  _pos.z + 0};
			right = {0,                      -14.0f * ONE16TH,         0};
			back  = {14.0f * ONE16TH,        0,                        0};
			break ;
		default:
			std::cout << "ERROR DISPLAY OPEN CHEST" << std::endl;
			return ;
	}
	glm::vec3 p0 = {pos.x + 0,       pos.y + 0      , pos.z + 10.0f * ONE16TH};
	glm::vec3 p1 = {pos.x + right.x, pos.y + right.y, pos.z + 10.0f * ONE16TH};
	glm::vec3 p2 = {pos.x + 0,       pos.y + 0,       pos.z + 0};
	glm::vec3 p3 = {pos.x + right.x, pos.y + right.y, pos.z + 0};

	glm::vec3 p4 = {pos.x + back.x + 0,       pos.y + back.y + 0      , pos.z + 10.0f * ONE16TH};
	glm::vec3 p5 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 10.0f * ONE16TH};
	glm::vec3 p6 = {pos.x + back.x + 0,       pos.y + back.y + 0,       pos.z + 0};
	glm::vec3 p7 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 0};

    int spec = (1 << 4) + (15 << 12) + (3 << 19) + (itemLight << 24);
    t_shaderInput v0 = {spec + 1 + (5 << 8), p0};
    t_shaderInput v1 = {spec - 1 + XTEX + (5 << 8), p1};
    t_shaderInput v2 = {spec + 1 + (1 << 18) + (15 << 8), p2};
    t_shaderInput v3 = {spec - 1 + XTEX + (1 << 18) + (15 << 8), p3};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 12) + (4 << 19) + (itemLight << 24);
    v0 = {spec + 1 + (5 << 8), p5};
    v1 = {spec - 1 + XTEX + (5 << 8), p4};
    v2 = {spec + 1 + (1 << 18) + (15 << 8), p7};
    v3 = {spec - 1 + XTEX + (1 << 18) + (15 << 8), p6};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 12) + (1 << 19) + (itemLight << 24);
    v0 = {spec + 1 + (5 << 8), p4};
    v1 = {spec - 1 + XTEX + (5 << 8), p0};
    v2 = {spec + 1 + (1 << 18) + (15 << 8), p6};
    v3 = {spec - 1 + XTEX + (1 << 18) + (15 << 8), p2};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 12) + (2 << 19) + (itemLight << 24);
    v0 = {spec + 1 + (5 << 8), p1};
    v1 = {spec - 1 + XTEX + (5 << 8), p5};
    v2 = {spec + 1 + (1 << 18) + (15 << 8), p3};
    v3 = {spec - 1 + XTEX + (1 << 18) + (15 << 8), p7};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// +z
	spec = (3 << 4) + (15 << 12) + (0 << 19) + (itemLight << 24);
    v0 = {spec + 1, p4};
    v1 = {spec - 1 + XTEX, p5};
    v2 = {spec + 1 + (1 << 18) + (14 << 8), p0};
    v3 = {spec - 1 + XTEX + (1 << 18) + (14 << 8), p1};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// -z
	if (!air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z - 1), true), false, false, false)) {
		spec = (2 << 4) + (15 << 12) + (5 << 19) + (itemLight << 24);
		v0 = {spec + 1, p2};
		v1 = {spec - 1 + XTEX, p3};
		v2 = {spec + 1 + (1 << 18) + (14 << 8), p6};
		v3 = {spec - 1 + XTEX + (1 << 18) + (14 << 8), p7};
		arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	}

	// top box
	// alpha is angle between top and bottom boxes
	float alpha = glm::radians(_timer / CHEST_ANIM_TIME * 90);
	float sina = glm::sin(alpha), cosa = glm::cos(alpha);
	glm::vec3 top = {-cosa * back.x, -cosa * back.y, sina * 14.0f * ONE16TH};
	pos += back;
	pos.z += 10.f * ONE16TH;
	back *= (5 / 14.0f * sina);
	back.z = 5 * ONE16TH * cosa;
	p0 = pos + top;
	p1 = pos + top + right;
	p2 = pos;
	p3 = pos + right;

	p4 = p0 + back;
	p5 = p1 + back;
	p6 = p2 + back;
	p7 = p3 + back;

    spec = (4 << 4) + (15 << 12) + (5 << 19) + (itemLight << 24);
    v0 = {spec + 1, p0};
    v1 = {spec - 1 + XTEX, p1};
    v2 = {spec + 1 + (1 << 18) + (14 << 8), p2};
    v3 = {spec - 1 + XTEX + (1 << 18) + (14 << 8), p3};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (2 << 4) + (15 << 12) + (0 << 19) + (itemLight << 24);
    v0 = {spec + 1, p6};
    v1 = {spec - 1 + XTEX, p7};
    v2 = {spec + 1 + (1 << 18) + (14 << 8), p4};
    v3 = {spec - 1 + XTEX + (1 << 18) + (14 << 8), p5};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 12) + (1 << 19) + (itemLight << 24);
    v0 = {spec + 1, p6};
    v1 = {spec - 1 + XTEX, p4};
    v2 = {spec + 1 + (1 << 18) + (5 << 8), p2};
    v3 = {spec - 1 + XTEX + (1 << 18) + (5 << 8), p0};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 12) + (2 << 19) + (itemLight << 24);
    v0 = {spec + 1, p5};
    v1 = {spec - 1 + XTEX, p7};
    v2 = {spec + 1 + (1 << 18) + (5 << 8), p1};
    v3 = {spec - 1 + XTEX + (1 << 18) + (5 << 8), p3};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// +z
	spec = (1 << 4) + (15 << 12) + (3 << 19) + (itemLight << 24);
    v0 = {spec + 1, p4};
    v1 = {spec - 1 + XTEX, p5};
    v2 = {spec + 1 + (1 << 18) + (5 << 8), p0};
    v3 = {spec - 1 + XTEX + (1 << 18) + (5 << 8), p1};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// -z
	spec = 0 + (15 << 12) + (2 << 19) + (itemLight << 24);
	v0 = {spec + 1, p7};
	v1 = {spec - 1 + XTEX, p6};
	v2 = {spec + 1 + (1 << 18) + (5 << 8), p3};
	v3 = {spec - 1 + XTEX + (1 << 18) + (5 << 8), p2};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	const float ONE14TH = 1.0f / 14.0f;
	// adding lock
	p0 +=  right * 6.0f * ONE14TH + back * 0.4f + top * ONE14TH;
	p1 += -right * 6.0f * ONE14TH + back * 0.4f + top * ONE14TH;
	p2 +=  right * 6.0f * ONE14TH - back * 0.4f + top * 15.0f * ONE14TH;
	p3 += -right * 6.0f * ONE14TH - back * 0.4f + top * 15.0f * ONE14TH;

	p4 +=  right * 6.0f * ONE14TH - back * 0.6f;
	p5 += -right * 6.0f * ONE14TH - back * 0.6f;
	p6 +=  right * 6.0f * ONE14TH - back * 1.4f + top * 14.0f * ONE14TH;
	p7 += -right * 6.0f * ONE14TH - back * 1.4f + top * 14.0f * ONE14TH;

    spec = (5 << 4) + (15 << 12) + (3 << 19) + (itemLight << 24); // front
    v0 = {spec, p0};
    v1 = {spec - 14 + XTEX, p1};
    v2 = {spec - (12 << 8) + YTEX, p2};
    v3 = {spec - 14 + XTEX - (12 << 8) + YTEX, p3};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (5 << 4) + (15 << 12) + (4 << 19) + (itemLight << 24); // back
    v0 = {spec - 14 + XTEX, p5};
    v1 = {spec, p4};
    v2 = {spec - 14 + XTEX - (12 << 8) + YTEX, p7};
    v3 = {spec - (12 << 8) + YTEX, p6};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (5 << 4) + (15 << 12) + (1 << 19) + (itemLight << 24); // left side
    v0 = {spec, p4};
    v1 = {spec - 15 + XTEX, p0};
    v2 = {spec - (12 << 8) + YTEX, p6};
    v3 = {spec - 15 + XTEX - (12 << 8) + YTEX, p2};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (5 << 4) + (15 << 12) + (2 << 19) + (itemLight << 24); // right side
    v0 = {spec + 1, p1};
    v1 = {spec - 14 + XTEX, p5};
    v2 = {spec + 1 - (12 << 8) + YTEX, p3};
    v3 = {spec - 14 + XTEX - (12 << 8) + YTEX, p7};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// +z
	spec = (5 << 4) + (15 << 12) + (0 << 19) + (itemLight << 24);
    v0 = {spec, p4};
    v1 = {spec - 14 + XTEX, p5};
    v2 = {spec - (15 << 8) + YTEX, p0};
    v3 = {spec - 14 + XTEX - (15 << 8) + YTEX, p1};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// -z
	spec = (5 << 4) + (15 << 12) + (5 << 19) + (itemLight << 24);
	v0 = {spec + (3 << 8), p2};
	v1 = {spec - 14 + XTEX + (3 << 8), p3};
	v2 = {spec - (12 << 8) + YTEX, p6};
	v3 = {spec - 14 + XTEX - (12 << 8) + YTEX, p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
}

void ChestInstance::display_closed( std::vector<t_shaderInput> &arr )
{
	glm::vec3 pos, right, back;
	switch (_orientation) {
		case face_dir::MINUSY:
			pos   = {_pos.x + ONE16TH,        _pos.y + ONE16TH,        _pos.z + 0};
			right = {14.0f * ONE16TH,        0,                        0};
			back  = {0,                      14.0f * ONE16TH,          0};
			break ;
		case face_dir::PLUSY:
			pos   = {_pos.x + 1.0f - ONE16TH, _pos.y + 1.0f - ONE16TH, _pos.z + 0};
			right = {-14.0f * ONE16TH,       0,                        0};
			back  = {0,                      -14.0f * ONE16TH,         0};
			break ;
		case face_dir::PLUSX:
			pos   = {_pos.x + 1.0f - ONE16TH, _pos.y + ONE16TH,        _pos.z + 0};
			right = {0,                      14.0f * ONE16TH,          0};
			back  = {-14.0f * ONE16TH,       0,                        0};
			break ;
		case face_dir::MINUSX:
			pos   = {_pos.x + ONE16TH,       _pos.y + 1.0f - ONE16TH,  _pos.z + 0};
			right = {0,                      -14.0f * ONE16TH,         0};
			back  = {14.0f * ONE16TH,        0,                        0};
			break ;
		default:
			std::cout << "ERROR DISPLAY CLOSED CHEST" << std::endl;
			return ;
	}
	glm::vec3 p0 = {pos.x + 0,       pos.y + 0      , pos.z + 15.0f * ONE16TH};
	glm::vec3 p1 = {pos.x + right.x, pos.y + right.y, pos.z + 15.0f * ONE16TH};
	glm::vec3 p2 = {pos.x + 0,       pos.y + 0,       pos.z + 0};
	glm::vec3 p3 = {pos.x + right.x, pos.y + right.y, pos.z + 0};

	glm::vec3 p4 = {pos.x + back.x + 0,       pos.y + back.y + 0      , pos.z + 15.0f * ONE16TH};
	glm::vec3 p5 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 15.0f * ONE16TH};
	glm::vec3 p6 = {pos.x + back.x + 0,       pos.y + back.y + 0,       pos.z + 0};
	glm::vec3 p7 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 0};

	int itemLight = _chunk->computePosLight(_pos);

    int spec = (1 << 4) + (15 << 12) + (3 << 19) + (itemLight << 24); // front
    t_shaderInput v0 = {spec + 1, p0};
    t_shaderInput v1 = {spec - 1 + XTEX, p1};
    t_shaderInput v2 = {spec + 1 + (1 << 18) + (15 << 8), p2};
    t_shaderInput v3 = {spec - 1 + XTEX + (1 << 18) + (15 << 8), p3};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 12) + (4 << 19) + (itemLight << 24); // back
    v0 = {spec + 1, p5};
    v1 = {spec - 1 + XTEX, p4};
    v2 = {spec + 1 + (1 << 18) + (15 << 8), p7};
    v3 = {spec - 1 + XTEX + (1 << 18) + (15 << 8), p6};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 12) + (1 << 19) + (itemLight << 24); // left size
    v0 = {spec + 1, p4};
    v1 = {spec - 1 + XTEX, p0};
    v2 = {spec + 1 + (1 << 18) + (15 << 8), p6};
    v3 = {spec - 1 + XTEX + (1 << 18) + (15 << 8), p2};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 12) + (2 << 19) + (itemLight << 24); // right size
    v0 = {spec + 1, p1};
    v1 = {spec - 1 + XTEX, p5};
    v2 = {spec + 1 + (1 << 18) + (15 << 8), p3};
    v3 = {spec - 1 + XTEX + (1 << 18) + (15 << 8), p7};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// +z
	spec = (2 << 4) + (15 << 12) + (0 << 19) + (itemLight << 24);
    v0 = {spec + 1, p4};
    v1 = {spec - 1 + XTEX, p5};
    v2 = {spec + 1 + (1 << 18) + (14 << 8), p0};
    v3 = {spec - 1 + XTEX + (1 << 18) + (14 << 8), p1};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// -z
	if (!air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z - 1), true), false, false, false)) {
		spec = (2 << 4) + (15 << 12) + (5 << 19) + (itemLight << 24);
		v0 = {spec + 1, p2};
		v1 = {spec - 1 + XTEX, p3};
		v2 = {spec + 1 + (1 << 18) + (14 << 8), p6};
		v3 = {spec - 1 + XTEX + (1 << 18) + (14 << 8), p7};
		arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	}

	const float ONE14TH = 1.0f / 14.0f;
	// adding lock
	p0 += glm::vec3( right.x * 6.0f * ONE14TH - back.x * ONE14TH,  right.y * 6.0f * ONE14TH - back.y * ONE14TH, -3.0f * ONE16TH);
	p1 += glm::vec3(-right.x * 6.0f * ONE14TH - back.x * ONE14TH, -right.y * 6.0f * ONE14TH - back.y * ONE14TH, -3.0f * ONE16TH);
	p2 += glm::vec3( right.x * 6.0f * ONE14TH - back.x * ONE14TH,  right.y * 6.0f * ONE14TH - back.y * ONE14TH,  8.0f * ONE16TH);
	p3 += glm::vec3(-right.x * 6.0f * ONE14TH - back.x * ONE14TH, -right.y * 6.0f * ONE14TH - back.y * ONE14TH,  8.0f * ONE16TH);

	p4 += glm::vec3( right.x * 6.0f * ONE14TH - back.x,  right.y * 6.0f * ONE14TH - back.y, -3.0f * ONE16TH);
	p5 += glm::vec3(-right.x * 6.0f * ONE14TH - back.x, -right.y * 6.0f * ONE14TH - back.y, -3.0f * ONE16TH);
	p6 += glm::vec3( right.x * 6.0f * ONE14TH - back.x,  right.y * 6.0f * ONE14TH - back.y,  8.0f * ONE16TH);
	p7 += glm::vec3(-right.x * 6.0f * ONE14TH - back.x, -right.y * 6.0f * ONE14TH - back.y,  8.0f * ONE16TH);

    spec = (5 << 4) + (15 << 12) + (3 << 19) + (itemLight << 24); // front
    v0 = {spec, p0};
    v1 = {spec - 14 + XTEX, p1};
    v2 = {spec - (12 << 8) + YTEX, p2};
    v3 = {spec - 14 + XTEX - (12 << 8) + YTEX, p3};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (5 << 4) + (15 << 12) + (1 << 19) + (itemLight << 24); // left side
    v0 = {spec, p4};
    v1 = {spec - 15 + XTEX, p0};
    v2 = {spec - (12 << 8) + YTEX, p6};
    v3 = {spec - 15 + XTEX - (12 << 8) + YTEX, p2};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (5 << 4) + (15 << 12) + (2 << 19) + (itemLight << 24); // right side
    v0 = {spec + 1, p1};
    v1 = {spec - 14 + XTEX, p5};
    v2 = {spec + 1 - (12 << 8) + YTEX, p3};
    v3 = {spec - 14 + XTEX - (12 << 8) + YTEX, p7};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// +z
	spec = (5 << 4) + (15 << 12) + (0 << 19) + (itemLight << 24);
    v0 = {spec, p4};
    v1 = {spec - 14 + XTEX, p5};
    v2 = {spec - (15 << 8) + YTEX, p0};
    v3 = {spec - 14 + XTEX - (15 << 8) + YTEX, p1};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// -z
	spec = (5 << 4) + (15 << 12) + (5 << 19) + (itemLight << 24);
	v0 = {spec + (3 << 8), p2};
	v1 = {spec - 14 + XTEX + (3 << 8), p3};
	v2 = {spec - (12 << 8) + YTEX, p6};
	v3 = {spec - 14 + XTEX - (12 << 8) + YTEX, p7};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void ChestInstance::setChunk( Chunk *chunk, glm::ivec3 pos )
{
	// std::cout << "CHUNK RESET IN CHESTINSANCE" << std::endl;
	_chunk = chunk;
	_chunk_pos = {chunk->getStartX(), chunk->getStartY()};
	_pos = pos;
}

void ChestInstance::setState( int state )
{
	_state = state;
}

int ChestInstance::getOrientation( void )
{
	return (_orientation);
}

t_item *ChestInstance::getItem( int index )
{
	// std::cout << "in chest, index " << index << std::endl;
	if (index < 0 || index > 26) {
		return  (NULL);
	}
	return (&_content[index]);
}

void ChestInstance::update( std::vector<t_shaderInput> &arr, float deltaTime )
{
	if (!_chunk) {
		std::cout << "chestInstance with NULL chunk" << std::endl;
		return ;
	}
	switch (_state) {
		case chest_state::OPENING:
			_timer += deltaTime;
			if (_timer > CHEST_ANIM_TIME) {
				_state = chest_state::OPEN;
				return (display_open(arr));
			}
			display_moving(arr);
			break ;
		case chest_state::CLOSING:
			_timer -= deltaTime;
			if (_timer < 0) {
				_state = chest_state::CLOSED;
				return (display_closed(arr));
			}
			display_moving(arr);
			break ;
		case chest_state::OPEN:
			return (display_open(arr));
		case chest_state::CLOSED:
			return (display_closed(arr));
	}
}
