#include "Chunk.hpp"
#include "logs.hpp"

ChestInstance::ChestInstance( Chunk* chunk, glm::ivec3 pos, int orientation )
	: _orientation(orientation), _state(chest_state::closed), _timer(0), _pos(pos), _chunk(chunk)
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

void ChestInstance::displayBottomBox( std::vector<t_shaderInput>& arr, std::array<glm::vec3, 8> pts, int light )
{
	int spec = settings::consts::shader::block::chest_front;
	light += (3 << 8);
	utils::shader::addQuads(arr, {pts[0], pts[1], pts[2], pts[3]}, spec + (5 << 16), light, 14, 10);

	spec = settings::consts::shader::block::chest_side;
	light += (1 << 8);
	utils::shader::addQuads(arr, {pts[5], pts[4], pts[7], pts[6]}, spec + (5 << 16), light, 14, 10);

	light -= (3 << 8);
	utils::shader::addQuads(arr, {pts[4], pts[0], pts[6], pts[2]}, spec + (5 << 16), light, 14, 10);

	light += (1 << 8);
	utils::shader::addQuads(arr, {pts[1], pts[5], pts[3], pts[7]}, spec + (5 << 16), light, 14, 10);
// +z
	spec = settings::consts::shader::block::chest_inner_bottom;
	light -= (2 << 8);
	utils::shader::addQuads(arr, {pts[4], pts[5], pts[0], pts[1]}, spec, light, 14, 14);
// -z
	if (!utils::block::air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z - 1)), false, false, false)) {
		spec = settings::consts::shader::block::chest_top;
		light += (5 << 8);
		utils::shader::addQuads(arr, {pts[2], pts[3], pts[6], pts[7]}, spec, light, 14, 14);
	}
}

void ChestInstance::displayTopBox( std::vector<t_shaderInput>& arr, std::array<glm::vec3, 8> pts, int light )
{
	int spec = settings::consts::shader::block::chest_inner_top;
	light += (5 << 8);
	utils::shader::addQuads(arr, {pts[0], pts[1], pts[2], pts[3]}, spec, light, 14, 14);

	spec = settings::consts::shader::block::chest_top;
	light -= (5 << 8);
	utils::shader::addQuads(arr, {pts[6], pts[7], pts[4], pts[5]}, spec, light, 14, 14);

	spec = settings::consts::shader::block::chest_side;
	light += (1 << 8);
	utils::shader::addQuads(arr, {pts[6], pts[4], pts[2], pts[0]}, spec, light, 14, 5);

	light += (1 << 8);
	utils::shader::addQuads(arr, {pts[5], pts[7], pts[1], pts[3]}, spec, light, 14, 5);
// +z
	spec = settings::consts::shader::block::chest_front;
	light += (2 << 8);
	utils::shader::addQuads(arr, {pts[4], pts[5], pts[0], pts[1]}, spec, light, 14, 5);
// -z
	spec = settings::consts::shader::block::chest_side;
	light -= (1 << 8);
	utils::shader::addQuads(arr, {pts[7], pts[6], pts[3], pts[2]}, spec, light, 14, 5);
}

void ChestInstance::displayLock( std::vector<t_shaderInput>& arr, std::array<glm::vec3, 8> pts, int light )
{
	int spec = settings::consts::shader::block::chest_front + (14 << 12);
	light += (3 << 8); // front
	utils::shader::addQuads(arr, {pts[0], pts[1], pts[2], pts[3]}, spec, light, 2, 4);

	light += (1 << 8); // back
	utils::shader::addQuads(arr, {pts[5], pts[4], pts[7], pts[6]}, spec, light, 2, 4, true);

	light -= (3 << 8); // left side
	utils::shader::addQuads(arr, {pts[4], pts[0], pts[6], pts[2]}, spec, light, 1, 4);

	light += (1 << 8); // right side
	utils::shader::addQuads(arr, {pts[1], pts[5], pts[3], pts[7]}, spec + (1 << 12), light, 1, 4);
// +z
	light -= (2 << 8);
	utils::shader::addQuads(arr, {pts[4], pts[5], pts[0], pts[1]}, spec, light, 2, 1);
// -z
	light += (5 << 8);
	utils::shader::addQuads(arr, {pts[2], pts[3], pts[6], pts[7]}, spec + (3 << 16), light, 2, 1);
}

void ChestInstance::display_open( std::vector<t_shaderInput>& arr )
{
	int itemLight = _chunk->computePosLight(_pos);

	// bottom box 
	glm::vec3 pos, right, back;
	switch (_orientation) {
		case face_dir::minus_y:
			pos   = {_pos.x + one16th,        _pos.y + one16th,        _pos.z + 0};
			right = {14.0f * one16th,        0,                        0};
			back  = {0,                      14.0f * one16th,          0};
			break ;
		case face_dir::plus_y:
			pos   = {_pos.x + 1.0f - one16th, _pos.y + 1.0f - one16th, _pos.z + 0};
			right = {-14.0f * one16th,       0,                        0};
			back  = {0,                      -14.0f * one16th,         0};
			break ;
		case face_dir::plus_x:
			pos   = {_pos.x + 1.0f - one16th, _pos.y + one16th,        _pos.z + 0};
			right = {0,                      14.0f * one16th,          0};
			back  = {-14.0f * one16th,       0,                        0};
			break ;
		case face_dir::minus_x:
			pos   = {_pos.x + one16th,       _pos.y + 1.0f - one16th,  _pos.z + 0};
			right = {0,                      -14.0f * one16th,         0};
			back  = {14.0f * one16th,        0,                        0};
			break ;
		default:
			LOGERROR("DISPLAY OPEN CHEST");
			return ;
	}
	glm::vec3 p0 = {pos.x + 0,       pos.y + 0      , pos.z + 10.0f * one16th};
	glm::vec3 p1 = {pos.x + right.x, pos.y + right.y, pos.z + 10.0f * one16th};
	glm::vec3 p2 = {pos.x + 0,       pos.y + 0,       pos.z + 0};
	glm::vec3 p3 = {pos.x + right.x, pos.y + right.y, pos.z + 0};

	glm::vec3 p4 = {pos.x + back.x + 0,       pos.y + back.y + 0      , pos.z + 10.0f * one16th};
	glm::vec3 p5 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 10.0f * one16th};
	glm::vec3 p6 = {pos.x + back.x + 0,       pos.y + back.y + 0,       pos.z + 0};
	glm::vec3 p7 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 0};

	displayBottomBox(arr, {p0, p1, p2, p3, p4, p5, p6, p7}, itemLight);

	// top box
	pos += back;
	back *= (5 / 14.0f);
	p0 = {pos.x + 0,       pos.y + 0      , pos.z + 24.0f * one16th};
	p1 = {pos.x + right.x, pos.y + right.y, pos.z + 24.0f * one16th};
	p2 = {pos.x + 0,       pos.y + 0,       pos.z + 10.0f * one16th};
	p3 = {pos.x + right.x, pos.y + right.y, pos.z + 10.0f * one16th};

	p4 = {pos.x + back.x + 0,       pos.y + back.y + 0      , pos.z + 24.0f * one16th};
	p5 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 24.0f * one16th};
	p6 = {pos.x + back.x + 0,       pos.y + back.y + 0,       pos.z + 10.0f * one16th};
	p7 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 10.0f * one16th};

	displayTopBox(arr, {p0, p1, p2, p3, p4, p5, p6, p7}, itemLight);

	const float ONE14TH = 1.0f / 14.0f;
	// adding lock
	p0 += glm::vec3( right.x * 6.0f * ONE14TH + back.x * 0.4f,  right.y * 6.0f * ONE14TH + back.y * 0.4f, one16th);
	p1 += glm::vec3(-right.x * 6.0f * ONE14TH + back.x * 0.4f, -right.y * 6.0f * ONE14TH + back.y * 0.4f, one16th);
	p2 += glm::vec3( right.x * 6.0f * ONE14TH - back.x * 0.4f,  right.y * 6.0f * ONE14TH - back.y * 0.4f, 15.0f * one16th);
	p3 += glm::vec3(-right.x * 6.0f * ONE14TH - back.x * 0.4f, -right.y * 6.0f * ONE14TH - back.y * 0.4f, 15.0f * one16th);

	p4 += glm::vec3( right.x * 6.0f * ONE14TH - back.x * 0.6f,  right.y * 6.0f * ONE14TH - back.y * 0.6f, 0);
	p5 += glm::vec3(-right.x * 6.0f * ONE14TH - back.x * 0.6f, -right.y * 6.0f * ONE14TH - back.y * 0.6f, 0);
	p6 += glm::vec3( right.x * 6.0f * ONE14TH - back.x * 1.4f,  right.y * 6.0f * ONE14TH - back.y * 1.4f, 14.0f * one16th);
	p7 += glm::vec3(-right.x * 6.0f * ONE14TH - back.x * 1.4f, -right.y * 6.0f * ONE14TH - back.y * 1.4f, 14.0f * one16th);

	displayLock(arr, {p0, p1, p2, p3, p4, p5, p6, p7}, itemLight);
}

void ChestInstance::display_moving( std::vector<t_shaderInput>& arr )
{
	int itemLight = _chunk->computePosLight(_pos);

	// bottom box -> same as display_open
	glm::vec3 pos, right, back;
	switch (_orientation) {
		case face_dir::minus_y:
			pos   = {_pos.x + one16th,        _pos.y + one16th,        _pos.z + 0};
			right = {14.0f * one16th,        0,                        0};
			back  = {0,                      14.0f * one16th,          0};
			break ;
		case face_dir::plus_y:
			pos   = {_pos.x + 1.0f - one16th, _pos.y + 1.0f - one16th, _pos.z + 0};
			right = {-14.0f * one16th,       0,                        0};
			back  = {0,                      -14.0f * one16th,         0};
			break ;
		case face_dir::plus_x:
			pos   = {_pos.x + 1.0f - one16th, _pos.y + one16th,        _pos.z + 0};
			right = {0,                      14.0f * one16th,          0};
			back  = {-14.0f * one16th,       0,                        0};
			break ;
		case face_dir::minus_x:
			pos   = {_pos.x + one16th,       _pos.y + 1.0f - one16th,  _pos.z + 0};
			right = {0,                      -14.0f * one16th,         0};
			back  = {14.0f * one16th,        0,                        0};
			break ;
		default:
			LOGERROR("DISPLAY MOVING CHEST");
			return ;
	}
	glm::vec3 p0 = {pos.x + 0,       pos.y + 0      , pos.z + 10.0f * one16th};
	glm::vec3 p1 = {pos.x + right.x, pos.y + right.y, pos.z + 10.0f * one16th};
	glm::vec3 p2 = {pos.x + 0,       pos.y + 0,       pos.z + 0};
	glm::vec3 p3 = {pos.x + right.x, pos.y + right.y, pos.z + 0};

	glm::vec3 p4 = {pos.x + back.x + 0,       pos.y + back.y + 0      , pos.z + 10.0f * one16th};
	glm::vec3 p5 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 10.0f * one16th};
	glm::vec3 p6 = {pos.x + back.x + 0,       pos.y + back.y + 0,       pos.z + 0};
	glm::vec3 p7 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 0};

	displayBottomBox(arr, {p0, p1, p2, p3, p4, p5, p6, p7}, itemLight);

	// top box
	// alpha is angle between top and bottom boxes
	float alpha = glm::radians(_timer / CHEST_ANIM_TIME * 90);
	float sina = glm::sin(alpha), cosa = glm::cos(alpha);
	glm::vec3 top = {-cosa * back.x, -cosa * back.y, sina * 14.0f * one16th};
	pos += back;
	pos.z += 10.f * one16th;
	back *= (5 / 14.0f * sina);
	back.z = 5 * one16th * cosa;
	p0 = pos + top;
	p1 = pos + top + right;
	p2 = pos;
	p3 = pos + right;

	p4 = p0 + back;
	p5 = p1 + back;
	p6 = p2 + back;
	p7 = p3 + back;

	displayTopBox(arr, {p0, p1, p2, p3, p4, p5, p6, p7}, itemLight);

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

    displayLock(arr, {p0, p1, p2, p3, p4, p5, p6, p7}, itemLight);
}

void ChestInstance::display_closed( std::vector<t_shaderInput>& arr )
{
	glm::vec3 pos, right, back;
	switch (_orientation) {
		case face_dir::minus_y:
			pos   = {_pos.x + one16th,        _pos.y + one16th,        _pos.z + 0};
			right = {14.0f * one16th,        0,                        0};
			back  = {0,                      14.0f * one16th,          0};
			break ;
		case face_dir::plus_y:
			pos   = {_pos.x + 1.0f - one16th, _pos.y + 1.0f - one16th, _pos.z + 0};
			right = {-14.0f * one16th,       0,                        0};
			back  = {0,                      -14.0f * one16th,         0};
			break ;
		case face_dir::plus_x:
			pos   = {_pos.x + 1.0f - one16th, _pos.y + one16th,        _pos.z + 0};
			right = {0,                      14.0f * one16th,          0};
			back  = {-14.0f * one16th,       0,                        0};
			break ;
		case face_dir::minus_x:
			pos   = {_pos.x + one16th,       _pos.y + 1.0f - one16th,  _pos.z + 0};
			right = {0,                      -14.0f * one16th,         0};
			back  = {14.0f * one16th,        0,                        0};
			break ;
		default:
			LOGERROR("ChestInstance::display_closed with orientation " << _orientation);
			return ;
	}
	glm::vec3 p0 = {pos.x + 0,       pos.y + 0      , pos.z + 15.0f * one16th};
	glm::vec3 p1 = {pos.x + right.x, pos.y + right.y, pos.z + 15.0f * one16th};
	glm::vec3 p2 = {pos.x + 0,       pos.y + 0,       pos.z + 0};
	glm::vec3 p3 = {pos.x + right.x, pos.y + right.y, pos.z + 0};

	glm::vec3 p4 = {pos.x + back.x + 0,       pos.y + back.y + 0      , pos.z + 15.0f * one16th};
	glm::vec3 p5 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 15.0f * one16th};
	glm::vec3 p6 = {pos.x + back.x + 0,       pos.y + back.y + 0,       pos.z + 0};
	glm::vec3 p7 = {pos.x + back.x + right.x, pos.y + back.y + right.y, pos.z + 0};

	int light = _chunk->computePosLight(_pos);

    int spec = settings::consts::shader::block::chest_front;
	light += (3 << 8);
	utils::shader::addQuads(arr, {p0, p1, p2, p3}, spec, light, 14, 15);

	spec = settings::consts::shader::block::chest_side; // back
	light += (1 << 8);
	utils::shader::addQuads(arr, {p5, p4, p7, p6}, spec, light, 14, 15);

	light -= (3 << 8); // left size
	utils::shader::addQuads(arr, {p4, p0, p6, p2}, spec, light, 14, 15);

	light += (1 << 8); // right size
	utils::shader::addQuads(arr, {p1, p5, p3, p7}, spec, light, 14, 15);
// +z
	spec = settings::consts::shader::block::chest_top;
	light -= (2 << 8);
	utils::shader::addQuads(arr, {p4, p5, p0, p1}, spec, light, 14, 14);
// -z
	if (!utils::block::air_flower(_chunk->getBlockAt(glm::floor(_pos.x - _chunk_pos.x), glm::floor(_pos.y - _chunk_pos.y), glm::floor(_pos.z - 1)), false, false, false)) {
		light += (5 << 8);
		utils::shader::addQuads(arr, {p2, p3, p6, p7}, spec, light, 14, 14);
	}

	const float ONE14TH = 1.0f / 14.0f;
	// adding lock
	p0 += glm::vec3( right.x * 6.0f * ONE14TH - back.x * ONE14TH,  right.y * 6.0f * ONE14TH - back.y * ONE14TH, -3.0f * one16th);
	p1 += glm::vec3(-right.x * 6.0f * ONE14TH - back.x * ONE14TH, -right.y * 6.0f * ONE14TH - back.y * ONE14TH, -3.0f * one16th);
	p2 += glm::vec3( right.x * 6.0f * ONE14TH - back.x * ONE14TH,  right.y * 6.0f * ONE14TH - back.y * ONE14TH,  8.0f * one16th);
	p3 += glm::vec3(-right.x * 6.0f * ONE14TH - back.x * ONE14TH, -right.y * 6.0f * ONE14TH - back.y * ONE14TH,  8.0f * one16th);

	p4 += glm::vec3( right.x * 6.0f * ONE14TH - back.x,  right.y * 6.0f * ONE14TH - back.y, -3.0f * one16th);
	p5 += glm::vec3(-right.x * 6.0f * ONE14TH - back.x, -right.y * 6.0f * ONE14TH - back.y, -3.0f * one16th);
	p6 += glm::vec3( right.x * 6.0f * ONE14TH - back.x,  right.y * 6.0f * ONE14TH - back.y,  8.0f * one16th);
	p7 += glm::vec3(-right.x * 6.0f * ONE14TH - back.x, -right.y * 6.0f * ONE14TH - back.y,  8.0f * one16th);

    displayLock(arr, {p0, p1, p2, p3, p4, p5, p6, p7}, light & 0xFF);
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void ChestInstance::setChunk( Chunk* chunk, glm::ivec3 pos )
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

void ChestInstance::update( std::vector<t_shaderInput>& arr, float deltaTime )
{
	if (!_chunk) {
		LOGERROR("chestInstance with NULL chunk");
		return ;
	}
	switch (_state) {
		case chest_state::opening:
			_timer += deltaTime;
			if (_timer > CHEST_ANIM_TIME) {
				_state = chest_state::open;
				return (display_open(arr));
			}
			display_moving(arr);
			break ;
		case chest_state::closing:
			_timer -= deltaTime;
			if (_timer < 0) {
				_state = chest_state::closed;
				return (display_closed(arr));
			}
			display_moving(arr);
			break ;
		case chest_state::open:
			return (display_open(arr));
		case chest_state::closed:
			return (display_closed(arr));
	}
}
