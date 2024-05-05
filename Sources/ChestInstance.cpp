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
			pos = {_pos.x + 0, _pos.y + 0, _pos.z + 0};
			right = {1, 0, 0};
			back = {0, 1, 0};
			break ;
		case face_dir::PLUSY:
			pos = {_pos.x + 1, _pos.y + 1, _pos.z + 0};
			right = {-1, 0, 0};
			back = {0, -1, 0};
			break ;
		case face_dir::PLUSX:
			pos = {_pos.x + 1, _pos.y + 0, _pos.z + 0};
			right = {0, 1, 0};
			back = {-1, 0, 0};
			break ;
		case face_dir::MINUSX:
			pos = {_pos.x + 0, _pos.y + 1, _pos.z + 0};
			right = {0, -1, 0};
			back = {1, 0, 0};
			break ;
		default:
			std::cout << "ERROR DISPLAY OPEN CHEST" << std::endl;
			return ;
	}
	glm::vec3 p0 = {pos.x + 0,       pos.y + 0      , pos.z + 10.0f / 16.0f};
	glm::vec3 p1 = {pos.x + right.x, pos.y + right.y, pos.z + 10.0f / 16.0f};
	glm::vec3 p2 = {pos.x + 0,       pos.y + 0,       pos.z + 0};
	glm::vec3 p3 = {pos.x + right.x, pos.y + right.y, pos.z + 0};

	glm::vec3 p4 = {pos.x + back.x + 0,       pos.y + back.y + 0      , pos.z + 10.0f / 16.0f};
	glm::vec3 p5 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 10.0f / 16.0f};
	glm::vec3 p6 = {pos.x + back.x + 0,       pos.y + back.y + 0,       pos.z + 0};
	glm::vec3 p7 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 0};

    int spec = 1 + (15 << 4) + (3 << 19) + (itemLight << 24);
    t_shaderInput v0 = {spec + (5 << 8), p0 + ONE_SIXTEENTH * back};
    t_shaderInput v1 = {spec + XTEX + (5 << 8), p1 + ONE_SIXTEENTH * back};
    t_shaderInput v2 = {spec + (1 << 18) + (15 << 8), p2 + ONE_SIXTEENTH * back};
    t_shaderInput v3 = {spec + XTEX + (1 << 18) + (15 << 8), p3 + ONE_SIXTEENTH * back};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 4) + (4 << 19) + (itemLight << 24);
    v0 = {spec + (5 << 8), p5 - ONE_SIXTEENTH * back};
    v1 = {spec + XTEX + (5 << 8), p4 - ONE_SIXTEENTH * back};
    v2 = {spec + (1 << 18) + (15 << 8), p7 - ONE_SIXTEENTH * back};
    v3 = {spec + XTEX + (1 << 18) + (15 << 8), p6 - ONE_SIXTEENTH * back};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 4) + (1 << 19) + (itemLight << 24);
    v0 = {spec + (5 << 8), p4 + ONE_SIXTEENTH * right};
    v1 = {spec + XTEX + (5 << 8), p0 + ONE_SIXTEENTH * right};
    v2 = {spec + (1 << 18) + (15 << 8), p6 + ONE_SIXTEENTH * right};
    v3 = {spec + XTEX + (1 << 18) + (15 << 8), p2 + ONE_SIXTEENTH * right};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 4) + (2 << 19) + (itemLight << 24);
    v0 = {spec + (5 << 8), p1 - ONE_SIXTEENTH * right};
    v1 = {spec + XTEX + (5 << 8), p5 - ONE_SIXTEENTH * right};
    v2 = {spec + (1 << 18) + (15 << 8), p3 - ONE_SIXTEENTH * right};
    v3 = {spec + XTEX + (1 << 18) + (15 << 8), p7 - ONE_SIXTEENTH * right};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// +z
	spec = 3 + (15 << 4) + (0 << 19) + (itemLight << 24);
    v0 = {spec, p4 - ONE_SIXTEENTH * back};
    v1 = {spec + XTEX, p5 - ONE_SIXTEENTH * back};
    v2 = {spec + (1 << 18) + (14 << 8), p0 + ONE_SIXTEENTH * back};
    v3 = {spec + XTEX + (1 << 18) + (14 << 8), p1 + ONE_SIXTEENTH * back};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// -z
	if (!air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z - 1), true), false, false, false)) {
		spec = 2 + (15 << 4) + (5 << 19) + (itemLight << 24);
		v0 = {spec, p2 + ONE_SIXTEENTH * back};
		v1 = {spec + XTEX, p3 + ONE_SIXTEENTH * back};
		v2 = {spec + (1 << 18) + (14 << 8), p6 - ONE_SIXTEENTH * back};
		v3 = {spec + XTEX + (1 << 18) + (14 << 8), p7 - ONE_SIXTEENTH * back};
		arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	}

	// top box
	pos += back * FIFTEEN_SIXTEENTH;
	back *= 5 * ONE_SIXTEENTH;
	p0 = {pos.x + 0,       pos.y + 0      , pos.z + 24.0f / 16.0f};
	p1 = {pos.x + right.x, pos.y + right.y, pos.z + 24.0f / 16.0f};
	p2 = {pos.x + 0,       pos.y + 0,       pos.z + 10.0f / 16.0f};
	p3 = {pos.x + right.x, pos.y + right.y, pos.z + 10.0f / 16.0f};

	p4 = {pos.x + back.x + 0,       pos.y + back.y + 0      , pos.z + 24.0f / 16.0f};
	p5 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 24.0f / 16.0f};
	p6 = {pos.x + back.x + 0,       pos.y + back.y + 0,       pos.z + 10.0f / 16.0f};
	p7 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 10.0f / 16.0f};

    spec = 4 + (15 << 4) + (5 << 19) + (itemLight << 24);
    v0 = {spec, p0};
    v1 = {spec + XTEX, p1};
    v2 = {spec + (1 << 18) + (14 << 8), p2};
    v3 = {spec + XTEX + (1 << 18) + (14 << 8), p3};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 2 + (15 << 4) + (0 << 19) + (itemLight << 24);
    v0 = {spec, p6};
    v1 = {spec + XTEX, p7};
    v2 = {spec + (1 << 18) + (14 << 8), p4};
    v3 = {spec + XTEX + (1 << 18) + (14 << 8), p5};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	glm::vec3 up = {0, 0, ONE_SIXTEENTH};
	spec = 0 + (15 << 4) + (1 << 19) + (itemLight << 24);
    v0 = {spec, p6 + ONE_SIXTEENTH * right - up};
    v1 = {spec + XTEX, p4 + ONE_SIXTEENTH * right + up};
    v2 = {spec + (1 << 18) + (5 << 8), p2 + ONE_SIXTEENTH * right - up};
    v3 = {spec + XTEX + (1 << 18) + (5 << 8), p0 + ONE_SIXTEENTH * right + up};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 4) + (2 << 19) + (itemLight << 24);
    v0 = {spec, p5 - ONE_SIXTEENTH * right + up};
    v1 = {spec + XTEX, p7 - ONE_SIXTEENTH * right - up};
    v2 = {spec + (1 << 18) + (5 << 8), p1 - ONE_SIXTEENTH * right + up};
    v3 = {spec + XTEX + (1 << 18) + (5 << 8), p3 - ONE_SIXTEENTH * right - up};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// +z
	spec = 1 + (15 << 4) + (3 << 19) + (itemLight << 24);
    v0 = {spec, p4};
    v1 = {spec + XTEX, p5};
    v2 = {spec + (1 << 18) + (5 << 8), p0};
    v3 = {spec + XTEX + (1 << 18) + (5 << 8), p1};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// -z
	spec = 0 + (15 << 4) + (2 << 19) + (itemLight << 24);
	v0 = {spec, p7};
	v1 = {spec + XTEX, p6};
	v2 = {spec + (1 << 18) + (5 << 8), p3};
	v3 = {spec + XTEX + (1 << 18) + (5 << 8), p2};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
}

void ChestInstance::display_moving( std::vector<t_shaderInput> &arr )
{
	int itemLight = _chunk->computePosLight(_pos);

	// bottom box -> same as display_open
	glm::vec3 pos, right, back;
	switch (_orientation) {
		case face_dir::MINUSY:
			pos = {_pos.x + 0, _pos.y + 0, _pos.z + 0};
			right = {1, 0, 0};
			back = {0, 1, 0};
			break ;
		case face_dir::PLUSY:
			pos = {_pos.x + 1, _pos.y + 1, _pos.z + 0};
			right = {-1, 0, 0};
			back = {0, -1, 0};
			break ;
		case face_dir::PLUSX:
			pos = {_pos.x + 1, _pos.y + 0, _pos.z + 0};
			right = {0, 1, 0};
			back = {-1, 0, 0};
			break ;
		case face_dir::MINUSX:
			pos = {_pos.x + 0, _pos.y + 1, _pos.z + 0};
			right = {0, -1, 0};
			back = {1, 0, 0};
			break ;
		default:
			std::cout << "ERROR DISPLAY OPEN CHEST" << std::endl;
			return ;
	}
	glm::vec3 p0 = {pos.x + 0,       pos.y + 0      , pos.z + 10.0f / 16.0f};
	glm::vec3 p1 = {pos.x + right.x, pos.y + right.y, pos.z + 10.0f / 16.0f};
	glm::vec3 p2 = {pos.x + 0,       pos.y + 0,       pos.z + 0};
	glm::vec3 p3 = {pos.x + right.x, pos.y + right.y, pos.z + 0};

	glm::vec3 p4 = {pos.x + back.x + 0,       pos.y + back.y + 0      , pos.z + 10.0f / 16.0f};
	glm::vec3 p5 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 10.0f / 16.0f};
	glm::vec3 p6 = {pos.x + back.x + 0,       pos.y + back.y + 0,       pos.z + 0};
	glm::vec3 p7 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 0};

    int spec = 1 + (15 << 4) + (3 << 19) + (itemLight << 24);
    t_shaderInput v0 = {spec + (5 << 8), p0 + ONE_SIXTEENTH * back};
    t_shaderInput v1 = {spec + XTEX + (5 << 8), p1 + ONE_SIXTEENTH * back};
    t_shaderInput v2 = {spec + (1 << 18) + (15 << 8), p2 + ONE_SIXTEENTH * back};
    t_shaderInput v3 = {spec + XTEX + (1 << 18) + (15 << 8), p3 + ONE_SIXTEENTH * back};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 4) + (4 << 19) + (itemLight << 24);
    v0 = {spec + (5 << 8), p5 - ONE_SIXTEENTH * back};
    v1 = {spec + XTEX + (5 << 8), p4 - ONE_SIXTEENTH * back};
    v2 = {spec + (1 << 18) + (15 << 8), p7 - ONE_SIXTEENTH * back};
    v3 = {spec + XTEX + (1 << 18) + (15 << 8), p6 - ONE_SIXTEENTH * back};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 4) + (1 << 19) + (itemLight << 24);
    v0 = {spec + (5 << 8), p4 + ONE_SIXTEENTH * right};
    v1 = {spec + XTEX + (5 << 8), p0 + ONE_SIXTEENTH * right};
    v2 = {spec + (1 << 18) + (15 << 8), p6 + ONE_SIXTEENTH * right};
    v3 = {spec + XTEX + (1 << 18) + (15 << 8), p2 + ONE_SIXTEENTH * right};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 4) + (2 << 19) + (itemLight << 24);
    v0 = {spec + (5 << 8), p1 - ONE_SIXTEENTH * right};
    v1 = {spec + XTEX + (5 << 8), p5 - ONE_SIXTEENTH * right};
    v2 = {spec + (1 << 18) + (15 << 8), p3 - ONE_SIXTEENTH * right};
    v3 = {spec + XTEX + (1 << 18) + (15 << 8), p7 - ONE_SIXTEENTH * right};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// +z
	spec = 3 + (15 << 4) + (0 << 19) + (itemLight << 24);
    v0 = {spec, p4 - ONE_SIXTEENTH * back};
    v1 = {spec + XTEX, p5 - ONE_SIXTEENTH * back};
    v2 = {spec + (1 << 18) + (14 << 8), p0 + ONE_SIXTEENTH * back};
    v3 = {spec + XTEX + (1 << 18) + (14 << 8), p1 + ONE_SIXTEENTH * back};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// -z
	if (!air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z - 1), true), false, false, false)) {
		spec = 2 + (15 << 4) + (5 << 19) + (itemLight << 24);
		v0 = {spec, p2 + ONE_SIXTEENTH * back};
		v1 = {spec + XTEX, p3 + ONE_SIXTEENTH * back};
		v2 = {spec + (1 << 18) + (14 << 8), p6 - ONE_SIXTEENTH * back};
		v3 = {spec + XTEX + (1 << 18) + (14 << 8), p7 - ONE_SIXTEENTH * back};
		arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	}

	// top box
	// alpha is angle between top and bottom boxes
	float alpha = _timer / CHEST_ANIM_TIME * 90;
	float sina = glm::sin(glm::radians(alpha)), cosa = glm::cos(glm::radians(alpha));
	glm::vec3 top = {-cosa * back.x * 14.0f / 16.0f, -cosa * back.y * 14.0f / 16.0f, sina * 14.0f / 16.0f};
	pos += back * FIFTEEN_SIXTEENTH;
	pos.z += 10.f / 16.0f;
	back *= 5 * ONE_SIXTEENTH * sina;
	back.z = 5 * ONE_SIXTEENTH * cosa;
	p0 = pos + top;
	p1 = pos + top + right;
	p2 = pos;
	p3 = pos + right;

	p4 = p0 + back;
	p5 = p1 + back;
	p6 = p2 + back;
	p7 = p3 + back;

    spec = 4 + (15 << 4) + (5 << 19) + (itemLight << 24);
    v0 = {spec, p0};
    v1 = {spec + XTEX, p1};
    v2 = {spec + (1 << 18) + (14 << 8), p2};
    v3 = {spec + XTEX + (1 << 18) + (14 << 8), p3};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 2 + (15 << 4) + (0 << 19) + (itemLight << 24);
    v0 = {spec, p6};
    v1 = {spec + XTEX, p7};
    v2 = {spec + (1 << 18) + (14 << 8), p4};
    v3 = {spec + XTEX + (1 << 18) + (14 << 8), p5};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	glm::vec3 up = ONE_SIXTEENTH * top;
	spec = 0 + (15 << 4) + (1 << 19) + (itemLight << 24);
    v0 = {spec, p6 + ONE_SIXTEENTH * right - up};
    v1 = {spec + XTEX, p4 + ONE_SIXTEENTH * right + up};
    v2 = {spec + (1 << 18) + (5 << 8), p2 + ONE_SIXTEENTH * right - up};
    v3 = {spec + XTEX + (1 << 18) + (5 << 8), p0 + ONE_SIXTEENTH * right + up};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 4) + (2 << 19) + (itemLight << 24);
    v0 = {spec, p5 - ONE_SIXTEENTH * right + up};
    v1 = {spec + XTEX, p7 - ONE_SIXTEENTH * right - up};
    v2 = {spec + (1 << 18) + (5 << 8), p1 - ONE_SIXTEENTH * right + up};
    v3 = {spec + XTEX + (1 << 18) + (5 << 8), p3 - ONE_SIXTEENTH * right - up};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// +z
	spec = 1 + (15 << 4) + (3 << 19) + (itemLight << 24);
    v0 = {spec, p4};
    v1 = {spec + XTEX, p5};
    v2 = {spec + (1 << 18) + (5 << 8), p0};
    v3 = {spec + XTEX + (1 << 18) + (5 << 8), p1};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// -z
	spec = 0 + (15 << 4) + (2 << 19) + (itemLight << 24);
	v0 = {spec, p7};
	v1 = {spec + XTEX, p6};
	v2 = {spec + (1 << 18) + (5 << 8), p3};
	v3 = {spec + XTEX + (1 << 18) + (5 << 8), p2};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
}

void ChestInstance::display_closed( std::vector<t_shaderInput> &arr )
{
	glm::vec3 pos, right, back;
	switch (_orientation) {
		case face_dir::MINUSY:
			pos = {_pos.x + 0, _pos.y + 0, _pos.z + 0};
			right = {1, 0, 0};
			back = {0, 1, 0};
			break ;
		case face_dir::PLUSY:
			pos = {_pos.x + 1, _pos.y + 1, _pos.z + 0};
			right = {-1, 0, 0};
			back = {0, -1, 0};
			break ;
		case face_dir::PLUSX:
			pos = {_pos.x + 1, _pos.y + 0, _pos.z + 0};
			right = {0, 1, 0};
			back = {-1, 0, 0};
			break ;
		case face_dir::MINUSX:
			pos = {_pos.x + 0, _pos.y + 1, _pos.z + 0};
			right = {0, -1, 0};
			back = {1, 0, 0};
			break ;
		default:
			std::cout << "ERROR DISPLAY CLOSED CHEST" << std::endl;
			return ;
	}
	glm::vec3 p0 = {pos.x + 0,       pos.y + 0      , pos.z + FIFTEEN_SIXTEENTH};
	glm::vec3 p1 = {pos.x + right.x, pos.y + right.y, pos.z + FIFTEEN_SIXTEENTH};
	glm::vec3 p2 = {pos.x + 0,       pos.y + 0,       pos.z + 0};
	glm::vec3 p3 = {pos.x + right.x, pos.y + right.y, pos.z + 0};

	glm::vec3 p4 = {pos.x + back.x + 0,       pos.y + back.y + 0      , pos.z + FIFTEEN_SIXTEENTH};
	glm::vec3 p5 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + FIFTEEN_SIXTEENTH};
	glm::vec3 p6 = {pos.x + back.x + 0,       pos.y + back.y + 0,       pos.z + 0};
	glm::vec3 p7 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 0};

	int itemLight = _chunk->computePosLight(_pos);

    int spec = 1 + (15 << 4) + (3 << 19) + (itemLight << 24);
    t_shaderInput v0 = {spec, p0 + ONE_SIXTEENTH * back};
    t_shaderInput v1 = {spec + XTEX, p1 + ONE_SIXTEENTH * back};
    t_shaderInput v2 = {spec + (1 << 18) + (15 << 8), p2 + ONE_SIXTEENTH * back};
    t_shaderInput v3 = {spec + XTEX + (1 << 18) + (15 << 8), p3 + ONE_SIXTEENTH * back};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 4) + (4 << 19) + (itemLight << 24);
    v0 = {spec, p5 - ONE_SIXTEENTH * back};
    v1 = {spec + XTEX, p4 - ONE_SIXTEENTH * back};
    v2 = {spec + (1 << 18) + (15 << 8), p7 - ONE_SIXTEENTH * back};
    v3 = {spec + XTEX + (1 << 18) + (15 << 8), p6 - ONE_SIXTEENTH * back};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 4) + (1 << 19) + (itemLight << 24);
    v0 = {spec, p4 + ONE_SIXTEENTH * right};
    v1 = {spec + XTEX, p0 + ONE_SIXTEENTH * right};
    v2 = {spec + (1 << 18) + (15 << 8), p6 + ONE_SIXTEENTH * right};
    v3 = {spec + XTEX + (1 << 18) + (15 << 8), p2 + ONE_SIXTEENTH * right};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = 0 + (15 << 4) + (2 << 19) + (itemLight << 24);
    v0 = {spec, p1 - ONE_SIXTEENTH * right};
    v1 = {spec + XTEX, p5 - ONE_SIXTEENTH * right};
    v2 = {spec + (1 << 18) + (15 << 8), p3 - ONE_SIXTEENTH * right};
    v3 = {spec + XTEX + (1 << 18) + (15 << 8), p7 - ONE_SIXTEENTH * right};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// +z
	spec = 2 + (15 << 4) + (0 << 19) + (itemLight << 24);
    v0 = {spec, p4 - ONE_SIXTEENTH * back};
    v1 = {spec + XTEX, p5 - ONE_SIXTEENTH * back};
    v2 = {spec + (1 << 18) + (14 << 8), p0 + ONE_SIXTEENTH * back};
    v3 = {spec + XTEX + (1 << 18) + (14 << 8), p1 + ONE_SIXTEENTH * back};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// -z
	if (!air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z - 1), true), false, false, false)) {
		spec = 2 + (15 << 4) + (5 << 19) + (itemLight << 24);
		v0 = {spec, p2 + ONE_SIXTEENTH * back};
		v1 = {spec + XTEX, p3 + ONE_SIXTEENTH * back};
		v2 = {spec + (1 << 18) + (14 << 8), p6 - ONE_SIXTEENTH * back};
		v3 = {spec + XTEX + (1 << 18) + (14 << 8), p7 - ONE_SIXTEENTH * back};
		arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	}
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
	// (void) state;
	// switch (_state) {
	// 	case chest_state::OPEN:
	// 	case chest_state::OPENING:
	// 		_state = chest_state::CLOSING;
	// 		break ;
	// 	case chest_state::CLOSED:
	// 	case chest_state::CLOSING:
	// 		_state = chest_state::OPENING;
	// 		break ;
	// }
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
