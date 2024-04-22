#include "Camera.hpp"
#include "Ui.hpp"


UI::UI( Inventory & inventory, Camera &camera ) : _textures(NULL), _gui_size(4), _nb_items(0), _movement(false), _inventory(inventory), _camera(camera), _vaoSet(false), _hideUI(false)
{
	_text = new Text();
	_chat = new Chat(_text);
}

UI::~UI( void )
{
	std::cout << "Destructor of UI called" << std::endl;
    glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
	
	if (_textures) {
		glDeleteTextures(2, _textures);
		delete [] _textures;
	}
	
	glDeleteProgram(_shaderProgram);

	delete _text;
	delete _chat;
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void UI::load_texture( std::string texstr, std::string shname, int index )
{
	loadTextureShader(index, _textures[index - 2], texstr);
	glUniform1i(glGetUniformLocation(_shaderProgram, shname.c_str()), index); // sampler2D #index in fragment shader
}

void UI::add_inventory_elem( int index )
{
	int type = _inventory.getSlotBlock(index).type;
	if (type == blocks::AIR) {
		return ;
	}
	int x = (WIN_WIDTH - (182 * _gui_size)) / 2 + (20 * index * _gui_size) + _gui_size * 3;
	int y = WIN_HEIGHT - (22 * _gui_size) * 2 + _gui_size * 3;
	if (!s_blocks[type]->item3D) {
		int spec = s_blocks[type]->texX(face_dir::MINUSX, 2) + (s_blocks[type]->texY(face_dir::MINUSX, 2) << 4) + (3 << 19);
		// int faceLight = computeLight(row - 1, col, level);
		int cornerLight = 15;//computeSmoothLight(faceLight, row - 1, col, level, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		int shade = 0;//computeShade(row - 1, col, level, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		// spec += (faceLight << 24);
		glm::ivec3 v0 = {spec + (cornerLight << 24) + (shade << 22), x, y};
		// cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		// shade = computeShade(row - 1, col, level, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		glm::ivec3 v1 = {spec + (cornerLight << 24) + (shade << 22) + 1, x + 16 * _gui_size, y};
		// cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		// shade = computeShade(row - 1, col, level, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		glm::ivec3 v2 = {spec + (cornerLight << 24) + (shade << 22) + (1 << 4), x, y + 16 * _gui_size};
		// cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		// shade = computeShade(row - 1, col, level, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		glm::ivec3 v3 = {spec + (cornerLight << 24) + (shade << 22) + 1 + (1 << 4), x + 16 * _gui_size, y + 16 * _gui_size};
		addFace(v0, v1, v2, v3, false);
		return ;
	}
	x += 2 * _gui_size;
	y += _gui_size;
	int offset = (type == blocks::OAK_LOG) ? AXIS::Z : face_dir::PLUSX;
	if (type == blocks::OAK_STAIRS) {
		int spec = (15 << 24) + s_blocks[type]->texX(face_dir::PLUSZ, offset) + (s_blocks[type]->texY(face_dir::PLUSZ, offset) << 4);
		// top of second step
		glm::ivec3 v0 = {spec, x, y + 15 * _gui_size * 81.25f / 362.5f};
		glm::ivec3 v1 = {spec + 1, x + 6.5f * _gui_size, y};
		glm::ivec3 v2 = {spec + (1 << 4), x + 3.25f * _gui_size, y + 15 * _gui_size * 121.875f / 362.5f};
		glm::ivec3 v3 = {spec + 1 + (1 << 4), x + 9.75f * _gui_size, y + 15 * _gui_size * 40.625f / 362.5f};
		addFace(v0, v1, v2, v3, false);
		// top of first step
		v0 = {spec, x + 3.25f * _gui_size, y + 15 * _gui_size * 221.875f / 362.5f};
		v1 = {spec + 1, x + 9.75f * _gui_size, y + 15 * _gui_size * 140.624f / 362.5f};
		v2 = {spec + (1 << 4), x + 6.5f * _gui_size, y + 15 * _gui_size * 262.5f / 362.5f};
		v3 = {spec + 1 + (1 << 4), x + 13 * _gui_size, y + 15 * _gui_size * 181.25f / 362.5f};
		addFace(v0, v1, v2, v3, false);
		// front of second step
		spec -= (8 << 24);
		v0 = {spec, x + 3.25f * _gui_size, y + 15 * _gui_size * 121.875f / 362.5f};
		v1 = {spec + 1, x + 9.75f * _gui_size, y + 15 * _gui_size * 40.625f / 362.5f};
		v2 = {spec + (1 << 4), x + 3.25f * _gui_size, y + 15 * _gui_size * 221.875f / 362.5f};
		v3 = {spec + 1 + (1 << 4), x + 9.75f * _gui_size, y + 15 * _gui_size * 140.624f / 362.5f};
		addFace(v0, v1, v2, v3, false);
		// front of first step
		v0 = {spec, x + 6.5f * _gui_size, y + 15 * _gui_size * 262.5f / 362.5f};
		v1 = {spec + 1, x + 13 * _gui_size, y + 15 * _gui_size * 181.25f / 362.5f};
		v2 = {spec + (1 << 4), x + 6.5f * _gui_size, y + 15 * _gui_size};
		v3 = {spec + 1 + (1 << 4), x + 13 * _gui_size, y + 15 * _gui_size * 281.25f / 362.5f};
		addFace(v0, v1, v2, v3, false);
		// left face
		spec = (10 << 24) + s_blocks[type]->texX(face_dir::MINUSY, offset) + (s_blocks[type]->texY(face_dir::MINUSY, offset) << 4);
		v0 = {spec, x, y + 15 * _gui_size * 81.25f / 362.5f};
		v1 = {spec + 1, x + 6.5f * _gui_size, y + 15 * _gui_size * 162.5f / 362.5f};
		v2 = {spec + (1 << 4), x, y + 15 * _gui_size * 281.25f / 362.5f};
		v3 = {spec + 1 + (1 << 4), x + 6.5f * _gui_size, y + 15 * _gui_size};
		addFace(v0, v1, v2, v3, false);
		return ;
	}
	int yTop = (type == blocks::OAK_SLAB) ? y + 16 * _gui_size * 100.0f / 362.5f
											: (type == blocks::OAK_TRAPDOOR) ? y + 16 * _gui_size * 162.5f / 362.5f : y;
	int slabOffset = (type == blocks::OAK_SLAB) ? (8 << 8)
											: (type == blocks::OAK_TRAPDOOR) ? (13 << 8) : 0;
	// top face
	int spec = (15 << 24) + s_blocks[type]->texX(face_dir::PLUSZ, offset) + (s_blocks[type]->texY(face_dir::PLUSZ, offset) << 4);
	glm::ivec3 v0 = {spec, x, yTop + 15 * _gui_size * 81.25f / 362.5f};
	glm::ivec3 v1 = {spec + 1, x + 6.5f * _gui_size, yTop};
	glm::ivec3 v2 = {spec + (1 << 4), x + 6.5f * _gui_size, yTop + 15 * _gui_size * 162.5f / 362.5f};
	glm::ivec3 v3 = {spec + 1 + (1 << 4), x + 13 * _gui_size, yTop + 15 * _gui_size * 81.25f / 362.5f};
	addFace(v0, v1, v2, v3, false);
	// left face
	spec = (10 << 24) + s_blocks[type]->texX(face_dir::MINUSY, offset) + (s_blocks[type]->texY(face_dir::MINUSY, offset) << 4);
	v0 = {spec + slabOffset, x, yTop + 15 * _gui_size * 81.25f / 362.5f};
	v1 = {spec + slabOffset + 1, x + 6.5f * _gui_size, yTop + 15 * _gui_size * 162.5f / 362.5f};
	v2 = {spec + (1 << 4), x, y + 15 * _gui_size * 281.25f / 362.5f};
	v3 = {spec + 1 + (1 << 4), x + 6.5f * _gui_size, y + 15 * _gui_size};
	addFace(v0, v1, v2, v3, false);
	// right face
	spec = (7 << 24) + s_blocks[type]->texX(face_dir::PLUSX, offset) + (s_blocks[type]->texY(face_dir::PLUSX, offset) << 4);
	v0 = {spec + slabOffset, x + 6.5f * _gui_size, yTop + 15 * _gui_size * 162.5f / 362.5f};
	v1 = {spec + slabOffset + 1, x + 13 * _gui_size, yTop + 15 * _gui_size * 81.25f / 362.5f};
	v2 = {spec + (1 << 4), x + 6.5f * _gui_size, y + 15 * _gui_size};
	v3 = {spec + 1 + (1 << 4), x + 13 * _gui_size, y + 15 * _gui_size * 281.25f / 362.5f};
	addFace(v0, v1, v2, v3, false);
}

void UI::add_dura_value( std::vector<std::array<int, 9>> &vertices, int index )
{
	glm::ivec2 dura = _inventory.getSlotBlock(index).dura;
	if (dura.y == 0 || dura.x == dura.y) {
		return ;
	}
	// adding grey bar first
	vertices.push_back({0, (WIN_WIDTH - (182 * _gui_size)) / 2 + (20 * index * _gui_size) + _gui_size * 3 + _gui_size, WIN_HEIGHT - (22 * _gui_size) * 2 + _gui_size * 3 + 14 * _gui_size, 14 * _gui_size, _gui_size, 64, 0, 1, 1});
	// adding progress bar second
	float percent = 1.0f * dura.x / dura.y;
	vertices.push_back({0, (WIN_WIDTH - (182 * _gui_size)) / 2 + (20 * index * _gui_size) + _gui_size * 3 + _gui_size, WIN_HEIGHT - (22 * _gui_size) * 2 + _gui_size * 3 + 14 * _gui_size, static_cast<int>(14 * _gui_size * percent), _gui_size, 103 * (percent < 0.6f) - (percent < 0.3), 16 + 9 * (percent < 0.6f) - 18 * (percent < 0.3f), 1, 1});
}

void UI::add_hearts_holder( std::vector<std::array<int, 9>> &vertices, int index )
{
	vertices.push_back({1, (WIN_WIDTH - (182 * _gui_size)) / 2 + _gui_size + (index * 8 * _gui_size), WIN_HEIGHT - (22 * _gui_size) * 2 - (8 * _gui_size) - (2 * _gui_size), 8 * _gui_size, 8 * _gui_size, 0, 16, 9, 9});
}

void UI::add_hearts( std::vector<std::array<int, 9>> &vertices, int index )
{
	vertices.push_back({1, (WIN_WIDTH - (182 * _gui_size)) / 2 + _gui_size + (index * 8 * _gui_size), WIN_HEIGHT - (22 * _gui_size) * 2 - (8 * _gui_size) - (2 * _gui_size), 8 * _gui_size, 8 * _gui_size, 18 * (_camera._health_points == (1 + 2 * index)) + 9 * (_camera._health_points > (1 + 2 * index)), 16, 9, 9});
}

void UI::add_armor_holder( std::vector<std::array<int, 9>> &vertices, int index )
{
	vertices.push_back({1, (WIN_WIDTH - (182 * _gui_size)) / 2 + _gui_size + (index * 8 * _gui_size), WIN_HEIGHT - (22 * _gui_size) * 2 - (2 * 8 * _gui_size) - (_gui_size * 3), 8 * _gui_size, 8 * _gui_size, 27, 16, 9, 9});
}

void UI::add_armor( std::vector<std::array<int, 9>> &vertices, int index )
{
	vertices.push_back({1, (WIN_WIDTH - (182 * _gui_size)) / 2 + _gui_size + (index * 8 * _gui_size), WIN_HEIGHT - (22 * _gui_size) * 2 - (2 * 8 * _gui_size) - (_gui_size * 3), 8 * _gui_size, 8 * _gui_size, 36 + 9 * (index == 3), 16, 9, 9});
}

void UI::add_food_holder( std::vector<std::array<int, 9>> &vertices, int index, int saturation )
{
	vertices.push_back({1, (WIN_WIDTH + (182 * _gui_size)) / 2 - 10 * _gui_size - (index * 8 * _gui_size), WIN_HEIGHT - (22 * _gui_size) * 2 - (8 * _gui_size) - (2 * _gui_size), 8 * _gui_size, 8 * _gui_size, ((saturation > 2 * index) ? ((saturation == 2 * index + 1) ? 54 + 18 : 54 + 9): 54), 16, 9, 9});
}

void UI::add_food( std::vector<std::array<int, 9>> &vertices, int index )
{
	vertices.push_back({1, (WIN_WIDTH + (182 * _gui_size)) / 2 - 9 * _gui_size - (index * 8 * _gui_size), WIN_HEIGHT - (22 * _gui_size) * 2 - (8 * _gui_size) - (2 * _gui_size), 8 * _gui_size, 8 * _gui_size, 82 + 9 * (_camera._foodLevel == (1 + 2 * index)), 16, 9, 9});
}

void UI::add_bubbles( std::vector<std::array<int, 9>> &vertices, int index )
{
	vertices.push_back({1, (WIN_WIDTH + (182 * _gui_size)) / 2 - 9 * _gui_size - (index * 8 * _gui_size), WIN_HEIGHT - (22 * _gui_size) * 2 - (2 * 8 * _gui_size) - (_gui_size * 3), 8 * _gui_size, 8 * _gui_size, 99 + 9 * (_camera.getWaterStatus() == (1 + 2 * index)), 16, 9, 9});
}

void UI::setup_array_buffer( void )
{
	std::vector<std::array<int, 9>> vertices;

	vertices.push_back({1, WIN_WIDTH / 2 - 16, WIN_HEIGHT / 2 - 16, 32, 32, 0, 0, 16, 16}); // crosshair
	vertices.push_back({1, (WIN_WIDTH - (182 * _gui_size)) / 2, WIN_HEIGHT - (22 * _gui_size) * 2, 182 * _gui_size, 22 * _gui_size, 0, 25, 182, 22}); // hot bar
	int slotNum = _inventory.getSlotNum();
	vertices.push_back({1, (WIN_WIDTH - (182 * _gui_size)) / 2 + (20 * slotNum * _gui_size) - _gui_size, WIN_HEIGHT - (22 * _gui_size) * 2 - _gui_size, 24 * _gui_size, 24 * _gui_size, 0, 47, 24, 24}); // slot select

	for (int index = 0; index < 9; index++) {
		add_dura_value(vertices, index);
	}
	_nb_points_crea = vertices.size();
	for (int index = 0; index < 10; index++) {
		add_hearts_holder(vertices, index);
	}
	for (int index = 0; index < (_camera._health_points >> 1) + (_camera._health_points & 1); index++) {
		add_hearts(vertices, index);
	}
	for (int index = 0; index < 10; index++) {
		add_armor_holder(vertices, index);
	}
	for (int index = 0; index < 4; index++) {
		add_armor(vertices, index);
	}
	int saturation = glm::floor(_camera._foodSaturationLevel);
	for (int index = 0; index < 10; index++) {
		add_food_holder(vertices, index, saturation);
	}
	for (int index = 0; index < (_camera._foodLevel >> 1) + (_camera._foodLevel & 1); index++) {
		add_food(vertices, index);
	}
	for (int index = 0; index < (_camera.getWaterStatus() >> 1) + (_camera.getWaterStatus() & 1); index++) {
		add_bubbles(vertices, index);
	}

	_nb_points = vertices.size();
	if (!_nb_points) {
		return ;
	}

	glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
	_vaoSet = true;

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _nb_points * 9 * sizeof(GLint), &vertices[0][0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(UI_ATLASATTRIB);
	glVertexAttribIPointer(UI_ATLASATTRIB, 1, GL_INT, 9 * sizeof(GLint), 0);

    glEnableVertexAttribArray(UI_POSATTRIB);
	glVertexAttribIPointer(UI_POSATTRIB, 4, GL_INT, 9 * sizeof(GLint), (void *)(sizeof(GLint)));
	
	glEnableVertexAttribArray(UI_TEXATTRIB);
	glVertexAttribIPointer(UI_TEXATTRIB, 4, GL_INT, 9 * sizeof(GLint), (void *)(5 * sizeof(GLint)));

	check_glstate("UI::setup_array_buffer", false);
}

void UI::setup_item_array_buffer( void )
{
	_items.clear();

	for (int index = 0; index < 9; index++) {
		add_inventory_elem(index);
	}
}

void UI::display_slot_value( int index )
{
	if (index < 0 || index >= 9) {
		return ;
	}
	int value = _inventory.getSlotBlock(index).amount;
	if (value > 1) {
		if (value > 9) {
			_text->addText((WIN_WIDTH - (182 * _gui_size)) / 2 + ((10 + 20 * index) * _gui_size) + _gui_size * 4 - 6 * _gui_size + _gui_size, WIN_HEIGHT - (22 * _gui_size) * 2 + _gui_size * 12 + _gui_size, 8 * _gui_size, false, std::to_string(value / 10));
			_text->addText((WIN_WIDTH - (182 * _gui_size)) / 2 + ((10 + 20 * index) * _gui_size) + _gui_size * 4 - 6 * _gui_size, WIN_HEIGHT - (22 * _gui_size) * 2 + _gui_size * 12, 8 * _gui_size, true, std::to_string(value / 10));
		}
		_text->addText((WIN_WIDTH - (182 * _gui_size)) / 2 + ((10 + 20 * index) * _gui_size) + _gui_size * 4 + _gui_size, WIN_HEIGHT - (22 * _gui_size) * 2 + _gui_size * 12 + _gui_size, 8 * _gui_size, false, std::to_string(value % 10));
		_text->addText((WIN_WIDTH - (182 * _gui_size)) / 2 + ((10 + 20 * index) * _gui_size) + _gui_size * 4, WIN_HEIGHT - (22 * _gui_size) * 2 + _gui_size * 12, 8 * _gui_size, true, std::to_string(value % 10));
	}
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

Text *UI::getTextPtr( void )
{
	return (_text);
}

Chat *UI::getChatPtr( void )
{
	return (_chat);
}

void UI::changeGuiSize( int offset )
{
	_gui_size += offset;
	if (_gui_size < 2) _gui_size = 2;
	else if (_gui_size > 7) _gui_size = 7;
	else _vaoSet = false;
}

GLuint UI::getShaderProgram( void )
{
	return (_shaderProgram);
}

void UI::setup_shader( void )
{
	_text->setup_shader();
	_text->load_texture();

	// setup item shader program
	_itemShaderProgram =createShaderProgram("item_vertex", "", "item_fragment");

	glBindFragDataLocation(_itemShaderProgram, 0, "outColor");

	glBindAttribLocation(_itemShaderProgram, ITEM_SPECATTRIB, "specifications");
	glBindAttribLocation(_itemShaderProgram, ITEM_POSATTRIB, "position");

	glLinkProgram(_itemShaderProgram);
	glUseProgram(_itemShaderProgram);

	glUniform1i(glGetUniformLocation(_itemShaderProgram, "window_width"), WIN_WIDTH);
	glUniform1i(glGetUniformLocation(_itemShaderProgram, "window_height"), WIN_HEIGHT);

	glUniform1i(glGetUniformLocation(_itemShaderProgram, "blockAtlas"), 0); // we reuse texture from main shader

	check_glstate("Item_Shader program successfully created\n", true);

	// then setup ui shader program
	_shaderProgram = createShaderProgram("ui_vertex", "ui_geometry", "ui_fragment");

	glBindFragDataLocation(_shaderProgram, 0, "outColor");

	glBindAttribLocation(_shaderProgram, UI_ATLASATTRIB, "atlas");
	glBindAttribLocation(_shaderProgram, UI_POSATTRIB, "pos");
	glBindAttribLocation(_shaderProgram, UI_TEXATTRIB, "textcoord");

	glLinkProgram(_shaderProgram);
	glUseProgram(_shaderProgram);

	glUniform1i(glGetUniformLocation(_shaderProgram, "window_width"), WIN_WIDTH);
	glUniform1i(glGetUniformLocation(_shaderProgram, "window_height"), WIN_HEIGHT);

	glUniform1i(glGetUniformLocation(_shaderProgram, "blockAtlas"), 0); // TODO get rid of this because we use itemShaderProgram for it now

	check_glstate("UI_Shader program successfully created\n", true);

	_textures = new GLuint[2];
	glGenTextures(2, _textures);
	load_texture("Resources/uiAtlas.png", "uiAtlas", 2);
	load_texture("Resources/containersAtlas.png", "containerAtlas", 3);
}

void UI::addFace( glm::ivec3 v0, glm::ivec3 v1, glm::ivec3 v2, glm::ivec3 v3, bool alien, bool movement )
{
	if (alien) {
		_vaoSet = false;
	}
	if (movement) {
		_movement = true;
		if (!alien) return ;
	}

	_items.push_back(v0[0]);
	_items.push_back(v0[1]);
	_items.push_back(v0[2]);
	_items.push_back(v1[0]);
	_items.push_back(v1[1]);
	_items.push_back(v1[2]);
	_items.push_back(v2[0]);
	_items.push_back(v2[1]);
	_items.push_back(v2[2]);

	_items.push_back(v1[0]);
	_items.push_back(v1[1]);
	_items.push_back(v1[2]);
	_items.push_back(v3[0]);
	_items.push_back(v3[1]);
	_items.push_back(v3[2]);
	_items.push_back(v2[0]);
	_items.push_back(v2[1]);
	_items.push_back(v2[2]);
}

void UI::drawUserInterface( std::string str, bool game_mode, float deltaTime )
{
	if (_inventoryMessage.timer > 0) {
		_inventoryMessage.timer -= deltaTime;
		_text->addText((WIN_WIDTH - _text->textWidth(_gui_size * 3, _inventoryMessage.str)) / 2, WIN_HEIGHT - (35 * _gui_size) * 2, _gui_size * 3, true, _inventoryMessage.str);
	}
	_chat->blitMessages(deltaTime);
	_text->addText(12, 24, _gui_size * 3, true, str);
	if (_hideUI) {
		return ;
	}
	if (!_vaoSet || _inventory.getModif() || _camera.getModif()) {
		setup_array_buffer();
		setup_item_array_buffer();
		_inventory.setModif(false);
		_movement = true;
	}

	// Bench b;
	glUseProgram(_shaderProgram);
    glBindVertexArray(_vao);
	(game_mode == SURVIVAL)
		? glDrawArrays(GL_POINTS, 0, _nb_points)
		: glDrawArrays(GL_POINTS, 0, _nb_points_crea);
	// b.stop("drawArrays");
	// b.reset();

	for (int index = 0; index < 9; index++) {
		display_slot_value(index);
	}
	// b.stop("display numbers");
}

void UI::chatMessage( std::string str )
{
	_chat->chatMessage(str);
}

void UI::inventoryMessage( std::string str )
{
	_inventoryMessage = {str, 2};
}

void UI::textToScreen( void )
{
	if (_hideUI) {
		return (_text->toScreen());
	}

	int nb_items = _items.size();
	if (nb_items) {
		if (_nb_items != nb_items || _movement) {
			_nb_items = nb_items;
			_movement = false;
			// std::cout << "debug: iSize is " << nb_items << std::endl;

			glGenVertexArrays(1, &_item_vao);
			glBindVertexArray(_item_vao);

			glGenBuffers(1, &_item_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, _item_vbo);
			glBufferData(GL_ARRAY_BUFFER, nb_items * sizeof(GLint), &_items[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(ITEM_SPECATTRIB);
			glVertexAttribIPointer(ITEM_SPECATTRIB, 1, GL_INT, 3 * sizeof(GLint), 0);

			glEnableVertexAttribArray(ITEM_POSATTRIB);
			glVertexAttribIPointer(ITEM_POSATTRIB, 2, GL_INT, 3 * sizeof(GLint), (void *)(sizeof(GLint)));
			
			check_glstate("UI::setup_item_array_buffer", false);
		}

		glUseProgram(_itemShaderProgram);
		glBindVertexArray(_item_vao);
		glDrawArrays(GL_TRIANGLES, 0, nb_items / 3);
	}

	// Bench b;
	_text->toScreen();
	// b.stop("text to screen");
}
