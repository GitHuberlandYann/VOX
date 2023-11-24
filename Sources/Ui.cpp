#include "vox.h"

UI::UI( Inventory & inventory, Camera &camera ) : _textures(NULL), _inventory(inventory), _camera(camera), _vaoSet(false)
{
	_text = new Text();
}

UI::~UI( void )
{
    glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
	
	if (_textures) {
		glDeleteTextures(2, _textures);
		delete [] _textures;
	}
	
	glDeleteProgram(_shaderProgram);

	delete _text;
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

void UI::add_inventory_elem( GLint *vertices, int mult, int index, int & vindex )
{
	mtx_inventory.lock();
	int value = _inventory.getSlotBlock(index).x;
	mtx_inventory.unlock();
	if (value == blocks::AIR) {
		return ;
	} else if (value == blocks::OAK_SLAB) { // TODO CHANGE THIS ?
		value = blocks::OAK_PLANKS;
	}
	vertices[vindex + 0] = 0;
	vertices[vindex + 1] = (WIN_WIDTH - (182 * mult)) / 2 + (20 * index * mult) + mult * 3;
	vertices[vindex + 2] = WIN_HEIGHT - (22 * mult) * 2 + mult * 3;
	vertices[vindex + 3] = 16 * mult;
	vertices[vindex + 4] = 16 * mult;
	vertices[vindex + 5] = blockAtlasX(value);
	vertices[vindex + 6] = blockAtlasY(value);
	vertices[vindex + 7] = 16;
	vertices[vindex + 8] = 16;
	vindex += 9;
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

void UI::add_food_holder( GLint *vertices, int mult, int index, int & vindex )
{
	vertices[vindex + 0] = 1;
	vertices[vindex + 1] = (WIN_WIDTH + (182 * mult)) / 2 - 9 * mult - (index * 8 * mult);
	vertices[vindex + 2] = WIN_HEIGHT - (22 * mult) * 2 - (8 * mult) - (2 * mult);
	vertices[vindex + 3] = 8 * mult;
	vertices[vindex + 4] = 8 * mult;
	vertices[vindex + 5] = 54;
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
	vertices[vindex + 5] = 63 + 9 * (_camera._foodLevel == (1 + 2 * index));
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
	vertices[vindex + 5] = 81 + 9 * (_camera.getWaterStatus() == (1 + 2 * index));
	vertices[vindex + 6] = 16;
	vertices[vindex + 7] = 9;
	vertices[vindex + 8] = 9;
	vindex += 9;
}

void UI::setup_array_buffer( void )
{
	mtx_inventory.lock();
	int countSlot = _inventory.countSlots();
	int duras = _inventory.countDura(false);
	mtx_inventory.unlock();
    _nb_points = 3 + countSlot + 2 * duras + 10 + (_camera._health_points >> 1) + (_camera._health_points & 1) + (_camera.getWaterStatus() >> 1) + (_camera.getWaterStatus() & 1) + 10 + 4 + 10 + (_camera._foodLevel >> 1) + (_camera._foodLevel & 1);
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

	for (int index = 0; index < 9; index++) {
		add_inventory_elem(vertices, mult, index, vindex);
	}
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
	for (int index = 0; index < 10; index++) {
		add_food_holder(vertices, mult, index, vindex);
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

GLuint UI::getShaderProgram( void )
{
	return (_shaderProgram);
}

void UI::setup_shader( void )
{
	_text->setup_shader();
	_text->load_texture();

	std::string vertex_shader_data = get_file_content("Sources/Shaders/ui_vertex.glsl");
	char *vertexSource = &vertex_shader_data[0];

	_vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(_vertexShader, 1, &vertexSource, NULL);
	compile_shader(_vertexShader, "ui_vertex");

	std::string geometry_shader_data = get_file_content("Sources/Shaders/ui_geometry.glsl");
	char *geometrySource = &geometry_shader_data[0];

	_geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(_geometryShader, 1, &geometrySource, NULL);
	compile_shader(_geometryShader, "ui_geometry");

	std::string fragment_shader_data = get_file_content("Sources/Shaders/ui_fragment.glsl");
	char *fragmentSource = &fragment_shader_data[0];

	_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(_fragmentShader, 1, &fragmentSource, NULL);
	compile_shader(_fragmentShader, "ui_fragment");

	// Combining shaders into a program
	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, _vertexShader);
	glAttachShader(_shaderProgram, _geometryShader);
	glAttachShader(_shaderProgram, _fragmentShader);

	glBindFragDataLocation(_shaderProgram, 0, "outColor");

	glBindAttribLocation(_shaderProgram, UI_ATLASATTRIB, "atlas");
	glBindAttribLocation(_shaderProgram, UI_POSATTRIB, "pos");
	glBindAttribLocation(_shaderProgram, UI_TEXATTRIB, "textcoord");

	glLinkProgram(_shaderProgram);
	glUseProgram(_shaderProgram);

	glDeleteShader(_fragmentShader);
	glDeleteShader(_geometryShader);
    glDeleteShader(_vertexShader);

	check_glstate("UI_Shader program successfully created\n", true);

	glUniform1i(glGetUniformLocation(_shaderProgram, "window_width"), WIN_WIDTH);
	glUniform1i(glGetUniformLocation(_shaderProgram, "window_height"), WIN_HEIGHT);

	glUniform1i(glGetUniformLocation(_shaderProgram, "blockAtlas"), 0); // we reuse texture from main shader
	_textures = new GLuint[2];
	glGenTextures(2, _textures);
	load_texture("Resources/uiAtlas.png", "uiAtlas", 2);
	load_texture("Resources/containersAtlas.png", "containerAtlas", 3);
}

void UI::drawUserInterface( std::string str, bool game_mode, bool f5_mode )
{
	if (f5_mode) {
		return (_text->addText(12, 24, 12, true, str));
	}
	mtx_inventory.lock();
	if (!_vaoSet || _inventory.getModif() || _camera.getModif()) {
		mtx_inventory.unlock();
		setup_array_buffer();
		_inventory.setModif(false);
		mtx_inventory.lock();
	}
	mtx_inventory.unlock();
	// Bench b;
	glUseProgram(_shaderProgram);
    glBindVertexArray(_vao);
	mtx_inventory.lock();
	(game_mode == SURVIVAL)
		? glDrawArrays(GL_POINTS, 0, _nb_points)
		: glDrawArrays(GL_POINTS, 0, 3 + _inventory.countSlots() + 2 * _inventory.countDura(false));
	mtx_inventory.unlock();
	// b.stop("drawArrays");
	// b.reset();
	_text->addText(12, 24, 12, true, str);
	// b.stop("display text");
	// b.reset();
	for (int index = 0; index < 9; index++) {
		display_slot_value(index);
	}
	// b.stop("display numbers");
}

void UI::textToScreen( void )
{
	// Bench b;
	_text->toScreen();
	// b.stop("text to screen");
}
