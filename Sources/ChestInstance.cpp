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

void ChestInstance::display_open( std::vector<std::pair<int, glm::vec3>> &arr )
{
	int itemLight = _chunk->computePosLight(_pos);

	// bottom box 
	glm::vec3 p0 = {_pos.x + 0, _pos.y + 0, _pos.z + 10 / 16.0f};
	glm::vec3 p1 = {_pos.x + 1, _pos.y + 0, _pos.z + 10 / 16.0f};
	glm::vec3 p2 = {_pos.x + 0, _pos.y + 0, _pos.z + 0};
	glm::vec3 p3 = {_pos.x + 1, _pos.y + 0, _pos.z + 0};

	glm::vec3 p4 = {_pos.x + 0, _pos.y + 1, _pos.z + 10 / 16.0f};
	glm::vec3 p5 = {_pos.x + 1, _pos.y + 1, _pos.z + 10 / 16.0f};
	glm::vec3 p6 = {_pos.x + 0, _pos.y + 1, _pos.z + 0};
	glm::vec3 p7 = {_pos.x + 1, _pos.y + 1, _pos.z + 0};

    int spec = (_orientation == face_dir::MINUSX) + (15 << 4) + (3 << 19) + (itemLight << 24);
    std::pair<int, glm::vec3> v0 = {spec + (5 << 8), p4 + glm::vec3(ONE_SIXTEENTH, 0, 0)};
    std::pair<int, glm::vec3> v1 = {spec + XTEX + (5 << 8), p0 + glm::vec3(ONE_SIXTEENTH, 0, 0)};
    std::pair<int, glm::vec3> v2 = {spec + (1 << 18) + (15 << 8), p6 + glm::vec3(ONE_SIXTEENTH, 0, 0)};
    std::pair<int, glm::vec3> v3 = {spec + XTEX + (1 << 18) + (15 << 8), p2 + glm::vec3(ONE_SIXTEENTH, 0, 0)};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (_orientation == face_dir::PLUSX) + (15 << 4) + (4 << 19) + (itemLight << 24);
    v0 = {spec + (5 << 8), p1 - glm::vec3(ONE_SIXTEENTH, 0, 0)};
    v1 = {spec + XTEX + (5 << 8), p5 - glm::vec3(ONE_SIXTEENTH, 0, 0)};
    v2 = {spec + (1 << 18) + (15 << 8), p3 - glm::vec3(ONE_SIXTEENTH, 0, 0)};
    v3 = {spec + XTEX + (1 << 18) + (15 << 8), p7 - glm::vec3(ONE_SIXTEENTH, 0, 0)};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (_orientation == face_dir::MINUSY) + (15 << 4) + (1 << 19) + (itemLight << 24);
    v0 = {spec + (5 << 8), p0 + glm::vec3(0, ONE_SIXTEENTH, 0)};
    v1 = {spec + XTEX + (5 << 8), p1 + glm::vec3(0, ONE_SIXTEENTH, 0)};
    v2 = {spec + (1 << 18) + (15 << 8), p2 + glm::vec3(0, ONE_SIXTEENTH, 0)};
    v3 = {spec + XTEX + (1 << 18) + (15 << 8), p3 + glm::vec3(0, ONE_SIXTEENTH, 0)};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (_orientation == face_dir::PLUSY) + (15 << 4) + (2 << 19) + (itemLight << 24);
    v0 = {spec + (5 << 8), p5 - glm::vec3(0, ONE_SIXTEENTH, 0)};
    v1 = {spec + XTEX + (5 << 8), p4 - glm::vec3(0, ONE_SIXTEENTH, 0)};
    v2 = {spec + (1 << 18) + (15 << 8), p7 - glm::vec3(0, ONE_SIXTEENTH, 0)};
    v3 = {spec + XTEX + (1 << 18) + (15 << 8), p6 - glm::vec3(0, ONE_SIXTEENTH, 0)};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// +z
	spec = 3 + (15 << 4) + (0 << 19) + (itemLight << 24);
    v0 = {spec, p4 - glm::vec3(0, ONE_SIXTEENTH, 0)};
    v1 = {spec + XTEX, p5 - glm::vec3(0, ONE_SIXTEENTH, 0)};
    v2 = {spec + (1 << 18) + (14 << 8), p0 + glm::vec3(0, ONE_SIXTEENTH, 0)};
    v3 = {spec + XTEX + (1 << 18) + (14 << 8), p1 + glm::vec3(0, ONE_SIXTEENTH, 0)};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// -z
	if (!air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z - 1), true), false, false, false)) {
		spec = 2 + (15 << 4) + (5 << 19) + (itemLight << 24);
		v0 = {spec, p2 + glm::vec3(0, ONE_SIXTEENTH, 0)};
		v1 = {spec + XTEX, p3 + glm::vec3(0, ONE_SIXTEENTH, 0)};
		v2 = {spec + (1 << 18) + (14 << 8), p6 - glm::vec3(0, ONE_SIXTEENTH, 0)};
		v3 = {spec + XTEX + (1 << 18) + (14 << 8), p7 - glm::vec3(0, ONE_SIXTEENTH, 0)};
		arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	}

	// top box
	float mxo = 0, pxo = 1, myo = 0, pyo = 1;
	switch (_orientation) {
		case face_dir::MINUSX:
			mxo = FIFTEEN_SIXTEENTH;
			pxo = 20 / 16.0f;
			break ;
		case face_dir::PLUSX:
			mxo = -4 / 16.0f;
			pxo = ONE_SIXTEENTH;
			break ;
		case face_dir::MINUSY:
			myo = FIFTEEN_SIXTEENTH;
			pyo = 20 / 16.0f;
			break ;
		case face_dir::PLUSY:
			myo = -4 / 16.0f;
			pyo = ONE_SIXTEENTH;
			break ;
	}
	p0 = {_pos.x + mxo, _pos.y + myo, _pos.z + 24 / 16.0f};
	p1 = {_pos.x + pxo, _pos.y + myo, _pos.z + 24 / 16.0f};
	p2 = {_pos.x + mxo, _pos.y + myo, _pos.z + 10 / 16.0f};
	p3 = {_pos.x + pxo, _pos.y + myo, _pos.z + 10 / 16.0f};

	p4 = {_pos.x + mxo, _pos.y + pyo, _pos.z + 24 / 16.0f};
	p5 = {_pos.x + pxo, _pos.y + pyo, _pos.z + 24 / 16.0f};
	p6 = {_pos.x + mxo, _pos.y + pyo, _pos.z + 10 / 16.0f};
	p7 = {_pos.x + pxo, _pos.y + pyo, _pos.z + 10 / 16.0f};

	int offset = (_orientation == face_dir::MINUSX) * 4 + (_orientation == face_dir::PLUSX) * 2;
    spec = offset + (15 << 4) + (3 << 19) + (itemLight << 24);
    v0 = {spec, p6 + glm::vec3(ONE_SIXTEENTH * !offset, -ONE_SIXTEENTH * !!offset, -ONE_SIXTEENTH)};
    v1 = {spec + XTEX, p4 + glm::vec3(ONE_SIXTEENTH * !offset, -ONE_SIXTEENTH * !!offset, ONE_SIXTEENTH)};
    v2 = {spec + (1 << 18) + ((5 + 9 * !!offset) << 8), p2 + glm::vec3(ONE_SIXTEENTH * !offset, ONE_SIXTEENTH * !!offset, -ONE_SIXTEENTH)};
    v3 = {spec + XTEX + (1 << 18) + ((5 + 9 * !!offset) << 8), p0 + glm::vec3(ONE_SIXTEENTH * !offset, ONE_SIXTEENTH * !!offset, ONE_SIXTEENTH)};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	offset = (_orientation == face_dir::MINUSX) * 2 + (_orientation == face_dir::PLUSX) * 4;
	spec = offset + (15 << 4) + (4 << 19) + (itemLight << 24);
    v0 = {spec, p3 + glm::vec3(0, 0, -ONE_SIXTEENTH)};
    v1 = {spec + XTEX, p1 + glm::vec3(0, 0, ONE_SIXTEENTH)};
    v2 = {spec + (1 << 18) + (5 << 8), p7 + glm::vec3(0, 0, -ONE_SIXTEENTH)};
    v3 = {spec + XTEX + (1 << 18) + (5 << 8), p5 + glm::vec3(0, 0, ONE_SIXTEENTH)};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (_orientation == face_dir::MINUSY) + (15 << 4) + (1 << 19) + (itemLight << 24);
    v0 = {spec, p0 + glm::vec3(0, ONE_SIXTEENTH, 0)};
    v1 = {spec + XTEX, p1 + glm::vec3(0, ONE_SIXTEENTH, 0)};
    v2 = {spec + (1 << 18) + (5 << 8), p2 + glm::vec3(0, ONE_SIXTEENTH, 0)};
    v3 = {spec + XTEX + (1 << 18) + (5 << 8), p3 + glm::vec3(0, ONE_SIXTEENTH, 0)};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (_orientation == face_dir::PLUSY) + (15 << 4) + (2 << 19) + (itemLight << 24);
    v0 = {spec, p5 - glm::vec3(0, ONE_SIXTEENTH, 0)};
    v1 = {spec + XTEX, p4 - glm::vec3(0, ONE_SIXTEENTH, 0)};
    v2 = {spec + (1 << 18) + (5 << 8), p7 - glm::vec3(0, ONE_SIXTEENTH, 0)};
    v3 = {spec + XTEX + (1 << 18) + (5 << 8), p6 - glm::vec3(0, ONE_SIXTEENTH, 0)};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// +z
	spec = 1 + (15 << 4) + (0 << 19) + (itemLight << 24);
    v0 = {spec, p4 - glm::vec3(0, ONE_SIXTEENTH, 0)};
    v1 = {spec + XTEX, p5 - glm::vec3(0, ONE_SIXTEENTH, 0)};
    v2 = {spec + (1 << 18) + (5 << 8), p0 + glm::vec3(0, ONE_SIXTEENTH, 0)};
    v3 = {spec + XTEX + (1 << 18) + (5 << 8), p1 + glm::vec3(0, ONE_SIXTEENTH, 0)};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// -z
	spec = 0 + (15 << 4) + (5 << 19) + (itemLight << 24);
	v0 = {spec, p2 + glm::vec3(0, ONE_SIXTEENTH, 0)};
	v1 = {spec + XTEX, p3 + glm::vec3(0, ONE_SIXTEENTH, 0)};
	v2 = {spec + (1 << 18) + (5 << 8), p6 - glm::vec3(0, ONE_SIXTEENTH, 0)};
	v3 = {spec + XTEX + (1 << 18) + (5 << 8), p7 - glm::vec3(0, ONE_SIXTEENTH, 0)};
	arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
}

void ChestInstance::display_closed( std::vector<std::pair<int, glm::vec3>> &arr )
{
	glm::vec3 p0 = {_pos.x + 0, _pos.y + 0, _pos.z + FIFTEEN_SIXTEENTH};
	glm::vec3 p1 = {_pos.x + 1, _pos.y + 0, _pos.z + FIFTEEN_SIXTEENTH};
	glm::vec3 p2 = {_pos.x + 0, _pos.y + 0, _pos.z + 0};
	glm::vec3 p3 = {_pos.x + 1, _pos.y + 0, _pos.z + 0};

	glm::vec3 p4 = {_pos.x + 0, _pos.y + 1, _pos.z + FIFTEEN_SIXTEENTH};
	glm::vec3 p5 = {_pos.x + 1, _pos.y + 1, _pos.z + FIFTEEN_SIXTEENTH};
	glm::vec3 p6 = {_pos.x + 0, _pos.y + 1, _pos.z + 0};
	glm::vec3 p7 = {_pos.x + 1, _pos.y + 1, _pos.z + 0};

	int itemLight = _chunk->computePosLight(_pos);

    int spec = (_orientation == face_dir::MINUSX) + (15 << 4) + (3 << 19) + (itemLight << 24);
    std::pair<int, glm::vec3> v0 = {spec, p4 + glm::vec3(ONE_SIXTEENTH, 0, 0)};
    std::pair<int, glm::vec3> v1 = {spec + XTEX, p0 + glm::vec3(ONE_SIXTEENTH, 0, 0)};
    std::pair<int, glm::vec3> v2 = {spec + (1 << 18) + (15 << 8), p6 + glm::vec3(ONE_SIXTEENTH, 0, 0)};
    std::pair<int, glm::vec3> v3 = {spec + XTEX + (1 << 18) + (15 << 8), p2 + glm::vec3(ONE_SIXTEENTH, 0, 0)};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (_orientation == face_dir::PLUSX) + (15 << 4) + (4 << 19) + (itemLight << 24);
    v0 = {spec, p1 - glm::vec3(ONE_SIXTEENTH, 0, 0)};
    v1 = {spec + XTEX, p5 - glm::vec3(ONE_SIXTEENTH, 0, 0)};
    v2 = {spec + (1 << 18) + (15 << 8), p3 - glm::vec3(ONE_SIXTEENTH, 0, 0)};
    v3 = {spec + XTEX + (1 << 18) + (15 << 8), p7 - glm::vec3(ONE_SIXTEENTH, 0, 0)};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (_orientation == face_dir::MINUSY) + (15 << 4) + (1 << 19) + (itemLight << 24);
    v0 = {spec, p0 + glm::vec3(0, ONE_SIXTEENTH, 0)};
    v1 = {spec + XTEX, p1 + glm::vec3(0, ONE_SIXTEENTH, 0)};
    v2 = {spec + (1 << 18) + (15 << 8), p2 + glm::vec3(0, ONE_SIXTEENTH, 0)};
    v3 = {spec + XTEX + (1 << 18) + (15 << 8), p3 + glm::vec3(0, ONE_SIXTEENTH, 0)};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);

	spec = (_orientation == face_dir::PLUSY) + (15 << 4) + (2 << 19) + (itemLight << 24);
    v0 = {spec, p5 - glm::vec3(0, ONE_SIXTEENTH, 0)};
    v1 = {spec + XTEX, p4 - glm::vec3(0, ONE_SIXTEENTH, 0)};
    v2 = {spec + (1 << 18) + (15 << 8), p7 - glm::vec3(0, ONE_SIXTEENTH, 0)};
    v3 = {spec + XTEX + (1 << 18) + (15 << 8), p6 - glm::vec3(0, ONE_SIXTEENTH, 0)};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// +z
	spec = 2 + (15 << 4) + (0 << 19) + (itemLight << 24);
    v0 = {spec, p4 - glm::vec3(0, ONE_SIXTEENTH, 0)};
    v1 = {spec + XTEX, p5 - glm::vec3(0, ONE_SIXTEENTH, 0)};
    v2 = {spec + (1 << 18) + (14 << 8), p0 + glm::vec3(0, ONE_SIXTEENTH, 0)};
    v3 = {spec + XTEX + (1 << 18) + (14 << 8), p1 + glm::vec3(0, ONE_SIXTEENTH, 0)};
    arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
// -z
	if (!air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z - 1), true), false, false, false)) {
		spec = 2 + (15 << 4) + (5 << 19) + (itemLight << 24);
		v0 = {spec, p2 + glm::vec3(0, ONE_SIXTEENTH, 0)};
		v1 = {spec + XTEX, p3 + glm::vec3(0, ONE_SIXTEENTH, 0)};
		v2 = {spec + (1 << 18) + (14 << 8), p6 - glm::vec3(0, ONE_SIXTEENTH, 0)};
		v3 = {spec + XTEX + (1 << 18) + (14 << 8), p7 - glm::vec3(0, ONE_SIXTEENTH, 0)};
		arr.push_back(v0);arr.push_back(v1);arr.push_back(v2);arr.push_back(v1);arr.push_back(v3);arr.push_back(v2);
	}
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void ChestInstance::setChunk( Chunk *chunk, glm::ivec3 pos )
{
	std::cout << "CHUNK RESET IN CHESTINSANCE" << std::endl;
	_chunk = chunk;
	_chunk_pos = {chunk->getStartX(), chunk->getStartY()};
	_pos = pos;
}

void ChestInstance::setState( int state )
{
	// _state = state; // this will be used later
	(void) state;
	switch (_state) {
		case chest_state::OPEN:
		case chest_state::OPENING:
			_state = chest_state::CLOSING;
			break ;
		case chest_state::CLOSED:
		case chest_state::CLOSING:
			_state = chest_state::OPENING;
			break ;
	}
}

int ChestInstance::getOrientation( void )
{
	return (_orientation);
}

void ChestInstance::update( std::vector<std::pair<int, glm::vec3>> &arr, float deltaTime )
{
	if (!_chunk) {
		std::cout << "chestInstance with NULL chunk" << std::endl;
		return ;
	}
	switch (_state) {
		case chest_state::OPENING:
			_timer += deltaTime;
			if (_timer > 1) {
				_state = chest_state::OPEN;
				return (display_open(arr));
			}
			break ;
		case chest_state::CLOSING:
			_timer -= deltaTime;
			if (_timer < 0) {
				_state = chest_state::CLOSED;
				return (display_closed(arr));
			}
			break ;
		case chest_state::OPEN:
			return (display_open(arr));
		case chest_state::CLOSED:
			return (display_closed(arr));
	}
}
