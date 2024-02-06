#include "Camera.hpp"
#include "Ui.hpp"

extern std::mutex mtx_inventory;
#include "SOIL/SOIL.h"
typedef struct {
	unsigned char *content;
	int width;
	int height;
}				t_tex;

UI::UI( Inventory & inventory, Camera &camera ) : _textures(NULL), _nb_items(0), _movement(false), _inventory(inventory), _camera(camera), _vaoSet(false), _hideUI(false)
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
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, _textures[index - 2]);

	// load image
	t_tex *texture = new t_tex;
	texture->content = SOIL_load_image(texstr.c_str(), &texture->width, &texture->height, 0, SOIL_LOAD_RGBA);
	if (!texture->content) {
		std::cerr << "failed to load image " << texstr << " because:" << std::endl << SOIL_last_result() << std::endl;
		exit(1);
	}

	// load image as texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, texture->content);

	glUniform1i(glGetUniformLocation(_shaderProgram, shname.c_str()), index); // sampler2D #index in fragment shader
			
	// set settings for texture wraping and size modif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST because pixel art, otherwise GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (texture) {
		SOIL_free_image_data(texture->content);
	}
	delete texture;

	check_glstate("Succesfully loaded " + texstr + " to shader\n", true);
}

void UI::add_inventory_elem( int index )
{
	mtx_inventory.lock();
	int type = _inventory.getSlotBlock(index).x;
	mtx_inventory.unlock();
	if (type == blocks::AIR) {
		return ;
	} else if (type == blocks::OAK_SLAB) { // TODO CHANGE THIS ?
		type = blocks::OAK_PLANKS;
	}
	int mult = 4;
	int x = (WIN_WIDTH - (182 * mult)) / 2 + (20 * index * mult) + mult * 3;
	int y = WIN_HEIGHT - (22 * mult) * 2 + mult * 3;
	if (!s_blocks[type]->item3D) {
		int spec = s_blocks[type]->texX(face_dir::MINUSX, 0) + (s_blocks[type]->texY(face_dir::MINUSX, 0) << 4) + (3 << 19);
		// int faceLight = computeLight(row - 1, col, level);
		int cornerLight = 15;//computeSmoothLight(faceLight, row - 1, col, level, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		int shade = 0;//computeShade(row - 1, col, level, {0, 1, 0, 0, 1, 1, 0, 0, 1});
		// spec += (faceLight << 24);
		glm::ivec3 v0 = {spec + (cornerLight << 24) + (shade << 22), x, y};
		// cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		// shade = computeShade(row - 1, col, level, {0, -1, 0, 0, -1, 1, 0, 0, 1});
		glm::ivec3 v1 = {spec + (cornerLight << 24) + (shade << 22) + 1 + (1 << 9) + (1 << 8), x + 16 * mult, y};
		// cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		// shade = computeShade(row - 1, col, level, {0, 1, 0, 0, 1, -1, 0, 0, -1});
		glm::ivec3 v2 = {spec + (cornerLight << 24) + (shade << 22) + (1 << 4) + (1 << 10) + (1 << 12), x, y + 16 * mult};
		// cornerLight = computeSmoothLight(faceLight, row - 1, col, level, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		// shade = computeShade(row - 1, col, level, {0, -1, 0, 0, -1, -1, 0, 0, -1});
		glm::ivec3 v3 = {spec + (cornerLight << 24) + (shade << 22) + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), x + 16 * mult, y + 16 * mult};
		addFace(v0, v1, v2, v3, false);
		return ;
	}
	x += 2 * mult;
	y += mult;
	int offset = face_dir::PLUSX;
	// top face
	int spec = (15 << 24) + s_blocks[type]->texX(face_dir::PLUSZ, offset) + (s_blocks[type]->texY(face_dir::PLUSZ, offset) << 4);
	// if (type == blocks::DIRT_PATH) {
	// 	p4.z -= ONE_SIXTEENTH;
	// 	p5.z -= ONE_SIXTEENTH;
	// 	p0.z -= ONE_SIXTEENTH;
	// 	p1.z -= ONE_SIXTEENTH;
	// }
	glm::ivec3 v0 = {spec, x, y + 15 * mult * 81.25f / 362.5f};
	glm::ivec3 v1 = {spec + 1 + (1 << 9) + (1 << 8), x + 6.5f * mult, y};
	glm::ivec3 v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), x + 6.5f * mult, y + 15 * mult * 162.5f / 362.5f};
	glm::ivec3 v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), x + 13 * mult, y + 15 * mult * 81.25f / 362.5f};
	addFace(v0, v1, v2, v3, false);
	// left face
	spec = (10 << 24) + s_blocks[type]->texX(face_dir::MINUSY, offset) + (s_blocks[type]->texY(face_dir::MINUSY, offset) << 4);
	v0 = {spec, x, y + 15 * mult * 81.25f / 362.5f};
	v1 = {spec + 1 + (1 << 9) + (1 << 8), x + 6.5f * mult, y + 15 * mult * 162.5f / 362.5f};
	v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), x, y + 15 * mult * 281.25f / 362.5f};
	v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), x + 6.5f * mult, y + 15 * mult};
	addFace(v0, v1, v2, v3, false);
	// right face
	spec = (7 << 24) + s_blocks[type]->texX(face_dir::PLUSX, offset) + (s_blocks[type]->texY(face_dir::PLUSX, offset) << 4);
	v0 = {spec, x + 6.5f * mult, y + 15 * mult * 162.5f / 362.5f};
	v1 = {spec + 1 + (1 << 9) + (1 << 8), x + 13 * mult, y + 15 * mult * 81.25f / 362.5f};
	v2 = {spec + (1 << 4) + (1 << 10) + (1 << 12), x + 6.5f * mult, y + 15 * mult};
	v3 = {spec + 1 + (1 << 9) + (1 << 4) + (1 << 10) + (1 << 8) + (1 << 12), x + 13 * mult, y + 15 * mult * 281.25f / 362.5f};
	addFace(v0, v1, v2, v3, false);
}

void UI::add_dura_value( GLint *vertices, int mult, int index, int & vindex )
{
	mtx_inventory.lock();
	glm::ivec3 value = _inventory.getDuraFromIndex(index, false);
	mtx_inventory.unlock();
	if (value.y == 0) {
		return ;
	}
	// adding grey bar first
	vertices[vindex + 0] = 0;
	vertices[vindex + 1] = (WIN_WIDTH - (182 * mult)) / 2 + (20 * value.x * mult) + mult * 3 + mult;
	vertices[vindex + 2] = WIN_HEIGHT - (22 * mult) * 2 + mult * 3 + 14 * mult;
	vertices[vindex + 3] = 14 * mult;
	vertices[vindex + 4] = mult;
	vertices[vindex + 5] = 64;
	vertices[vindex + 6] = 0;
	vertices[vindex + 7] = 1;
	vertices[vindex + 8] = 1;
	vindex += 9;
	vertices[vindex + 0] = 0; // adding progress bar second
	vertices[vindex + 1] = (WIN_WIDTH - (182 * mult)) / 2 + (20 * value.x * mult) + mult * 3 + mult;
	vertices[vindex + 2] = WIN_HEIGHT - (22 * mult) * 2 + mult * 3 + 14 * mult;
	float percent = 1.0f * value.y / value.z;
	vertices[vindex + 3] = 14 * mult * percent;
	vertices[vindex + 4] = mult;
	vertices[vindex + 5] = 103 * (percent < 0.6f) - (percent < 0.3);
	vertices[vindex + 6] = 16 + 9 * (percent < 0.6f) - 18 * (percent < 0.3f);
	vertices[vindex + 7] = 1;
	vertices[vindex + 8] = 1;
	vindex += 9;
}

void UI::add_hearts_holder( GLint *vertices, int mult, int index, int & vindex )
{
	vertices[vindex + 0] = 1;
	vertices[vindex + 1] = (WIN_WIDTH - (182 * mult)) / 2 + mult + (index * 8 * mult);
	vertices[vindex + 2] = WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult);
	vertices[vindex + 3] = 8 * mult;
	vertices[vindex + 4] = 8 * mult;
	vertices[vindex + 5] = 0;
	vertices[vindex + 6] = 16;
	vertices[vindex + 7] = 9;
	vertices[vindex + 8] = 9;
	vindex += 9;
}

void UI::add_hearts( GLint *vertices, int mult, int index, int & vindex )
{
	vertices[vindex + 0] = 1;
	vertices[vindex + 1] = (WIN_WIDTH - (182 * mult)) / 2 + mult + (index * 8 * mult);
	vertices[vindex + 2] = WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult);
	vertices[vindex + 3] = 8 * mult;
	vertices[vindex + 4] = 8 * mult;
	vertices[vindex + 5] = 18 * (_camera._health_points == (1 + 2 * index)) + 9 * (_camera._health_points > (1 + 2 * index));
	vertices[vindex + 6] = 16;
	vertices[vindex + 7] = 9;
	vertices[vindex + 8] = 9;
	vindex += 9;
}

void UI::add_armor_holder( GLint *vertices, int mult, int index, int & vindex )
{
	vertices[vindex + 0] = 1;
	vertices[vindex + 1] = (WIN_WIDTH - (182 * mult)) / 2 + mult + (index * 8 * mult);
	vertices[vindex + 2] = WIN_HEIGHT - (22 * mult) * 2 - (2 * 8 * mult) - (mult * 3);
	vertices[vindex + 3] = 8 * mult;
	vertices[vindex + 4] = 8 * mult;
	vertices[vindex + 5] = 27;
	vertices[vindex + 6] = 16;
	vertices[vindex + 7] = 9;
	vertices[vindex + 8] = 9;
	vindex += 9;
}

void UI::add_armor( GLint *vertices, int mult, int index, int & vindex )
{
	vertices[vindex + 0] = 1;
	vertices[vindex + 1] = (WIN_WIDTH - (182 * mult)) / 2 + mult + (index * 8 * mult);
	vertices[vindex + 2] = WIN_HEIGHT - (22 * mult) * 2 - (2 * 8 * mult) - (mult * 3);
	vertices[vindex + 3] = 8 * mult;
	vertices[vindex + 4] = 8 * mult;
	vertices[vindex + 5] = 36 + 9 * (index == 3);
	vertices[vindex + 6] = 16;
	vertices[vindex + 7] = 9;
	vertices[vindex + 8] = 9;
	vindex += 9;
}

void UI::add_food_holder( GLint *vertices, int mult, int index, int & vindex, int saturation )
{
	vertices[vindex + 0] = 1;
	vertices[vindex + 1] = (WIN_WIDTH + (182 * mult)) / 2 - 10 * mult - (index * 8 * mult);
	vertices[vindex + 2] = WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult);
	vertices[vindex + 3] = 8 * mult;
	vertices[vindex + 4] = 8 * mult;
	vertices[vindex + 5] = ((saturation > 2 * index) ? ((saturation == 2 * index + 1) ? 54 + 18 : 54 + 9): 54);
	vertices[vindex + 6] = 16;
	vertices[vindex + 7] = 9;
	vertices[vindex + 8] = 9;
	vindex += 9;
}

void UI::add_food( GLint *vertices, int mult, int index, int & vindex )
{
	vertices[vindex + 0] = 1;
	vertices[vindex + 1] = (WIN_WIDTH + (182 * mult)) / 2 - 9 * mult - (index * 8 * mult);
	vertices[vindex + 2] = WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult);
	vertices[vindex + 3] = 8 * mult;
	vertices[vindex + 4] = 8 * mult;
	vertices[vindex + 5] = 82 + 9 * (_camera._foodLevel == (1 + 2 * index));
	vertices[vindex + 6] = 16;
	vertices[vindex + 7] = 9;
	vertices[vindex + 8] = 9;
	vindex += 9;
}

void UI::add_bubbles( GLint *vertices, int mult, int index, int & vindex )
{
	vertices[vindex + 0] = 1;
	vertices[vindex + 1] = (WIN_WIDTH + (182 * mult)) / 2 - 9 * mult - (index * 8 * mult);
	vertices[vindex + 2] = WIN_HEIGHT - (22 * mult) * 2 - (2 * 8 * mult) - (mult * 3);
	vertices[vindex + 3] = 8 * mult;
	vertices[vindex + 4] = 8 * mult;
	vertices[vindex + 5] = 99 + 9 * (_camera.getWaterStatus() == (1 + 2 * index));
	vertices[vindex + 6] = 16;
	vertices[vindex + 7] = 9;
	vertices[vindex + 8] = 9;
	vindex += 9;
}

void UI::setup_array_buffer( void )
{
	mtx_inventory.lock();
	int duras = _inventory.countDura(false);
	mtx_inventory.unlock();
    _nb_points = 3 + 2 * duras + 10 + (_camera._health_points >> 1) + (_camera._health_points & 1) + (_camera.getWaterStatus() >> 1) + (_camera.getWaterStatus() & 1) + 10 + 4 + 10 + (_camera._foodLevel >> 1) + (_camera._foodLevel & 1);
	int mult = 4;
	GLint *vertices = new GLint[_nb_points * 9];

	vertices[0] = 1; // crosshair
	vertices[1] = WIN_WIDTH / 2 - 16;
	vertices[2] = WIN_HEIGHT / 2 - 16;
	vertices[3] = 32;
	vertices[4] = 32;
	vertices[5] = 0;
	vertices[6] = 0;
	vertices[7] = 16;
	vertices[8] = 16;

	int vindex = 9;
	vertices[vindex + 0] = 1; // hot bar
	vertices[vindex + 1] = (WIN_WIDTH - (182 * mult)) / 2;
	vertices[vindex + 2] = WIN_HEIGHT - (22 * mult) * 2;
	vertices[vindex + 3] = 182 * mult;
	vertices[vindex + 4] = 22 * mult;
	vertices[vindex + 5] = 0;
	vertices[vindex + 6] = 25;
	vertices[vindex + 7] = 182;
	vertices[vindex + 8] = 22;
	vindex += 9;
	vertices[vindex + 0] = 1; // slot select
	mtx_inventory.lock();
	vertices[vindex + 1] = (WIN_WIDTH - (182 * mult)) / 2 + (20 * _inventory.getSlotNum() * mult) - mult;
	mtx_inventory.unlock();
	vertices[vindex + 2] = WIN_HEIGHT - (22 * mult) * 2 - mult;
	vertices[vindex + 3] = 24 * mult;
	vertices[vindex + 4] = 24 * mult;
	vertices[vindex + 5] = 0;
	vertices[vindex + 6] = 47;
	vertices[vindex + 7] = 24;
	vertices[vindex + 8] = 24;
	vindex += 9;

	for (int index = 0; index < duras; index++) {
		add_dura_value(vertices, mult, index, vindex);
	}
	for (int index = 0; index < 10; index++) {
		add_hearts_holder(vertices, mult, index, vindex);
	}
	for (int index = 0; index < (_camera._health_points >> 1) + (_camera._health_points & 1); index++) {
		add_hearts(vertices, mult, index, vindex);
	}
	for (int index = 0; index < 10; index++) {
		add_armor_holder(vertices, mult, index, vindex);
	}
	for (int index = 0; index < 4; index++) {
		add_armor(vertices, mult, index, vindex);
	}
	int saturation = glm::floor(_camera._foodSaturationLevel);
	for (int index = 0; index < 10; index++) {
		add_food_holder(vertices, mult, index, vindex, saturation);
	}
	for (int index = 0; index < (_camera._foodLevel >> 1) + (_camera._foodLevel & 1); index++) {
		add_food(vertices, mult, index, vindex);
	}
	for (int index = 0; index < (_camera.getWaterStatus() >> 1) + (_camera.getWaterStatus() & 1); index++) {
		add_bubbles(vertices, mult, index, vindex);
	}
	if (vindex / 9 != _nb_points) {
		std::cout << "ERROR ui nb points is " << _nb_points << std::endl;
		std::cout << "current vindex is " << vindex / 9 << std::endl << std::endl << std::endl;
	}

	glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
	_vaoSet = true;

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _nb_points * 9 * sizeof(GLint), vertices, GL_STATIC_DRAW);

	delete [] vertices;

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
	int mult = 4, value = _inventory.getSlotBlock(index).y;
	if (value > 1) {
		if (value > 9) {
			_text->addText((WIN_WIDTH - (182 * mult)) / 2 + ((10 + 20 * index) * mult) + mult * 4 - 6 * mult + mult, WIN_HEIGHT - ((22 - 4) * mult) * 2 + mult * 6 - 6 + mult, 30, false, std::to_string(value / 10));
			_text->addText((WIN_WIDTH - (182 * mult)) / 2 + ((10 + 20 * index) * mult) + mult * 4 - 6 * mult, WIN_HEIGHT - ((22 - 4) * mult) * 2 + mult * 6 - 6, 30, true, std::to_string(value / 10));
		}
		_text->addText((WIN_WIDTH - (182 * mult)) / 2 + ((10 + 20 * index) * mult) + mult * 4 + mult, WIN_HEIGHT - ((22 - 4) * mult) * 2 + mult * 6 - 6 + mult, 30, false, std::to_string(value % 10));
		_text->addText((WIN_WIDTH - (182 * mult)) / 2 + ((10 + 20 * index) * mult) + mult * 4, WIN_HEIGHT - ((22 - 4) * mult) * 2 + mult * 6 - 6, 30, true, std::to_string(value % 10));
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
	_chat->blitMessages(deltaTime);
	_text->addText(12, 24, 12, true, str);
	if (_hideUI) {
		return ;
	}
	mtx_inventory.lock();
	if (!_vaoSet || _inventory.getModif() || _camera.getModif()) {
		mtx_inventory.unlock();
		setup_array_buffer();
		setup_item_array_buffer();
		_inventory.setModif(false);
		_movement = true;
		mtx_inventory.lock();
	}
	mtx_inventory.unlock();

	// Bench b;
	glUseProgram(_shaderProgram);
    glBindVertexArray(_vao);
	mtx_inventory.lock();
	(game_mode == SURVIVAL)
		? glDrawArrays(GL_POINTS, 0, _nb_points)
		: glDrawArrays(GL_POINTS, 0, 3 + 2 * _inventory.countDura(false));
	mtx_inventory.unlock();
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
