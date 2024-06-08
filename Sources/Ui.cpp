#include "Camera.hpp"
#include "Ui.hpp"
#include "Settings.hpp"

UI::UI( Inventory & inventory, Camera &camera )
	: _shaderProgram(0), _itemShaderProgram(0), _textures(NULL), _gui_size(4), _nb_items(0),
	_movement(false), _inventory(inventory), _camera(camera), _vaoSet(false), _hideUI(false)
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

void UI::add_inventory_elem( int index )
{
	int type = _inventory.getSlotBlock(index).type;
	if (type == blocks::air) {
		return ;
	}
	int x = (WIN_WIDTH - (182 * _gui_size)) / 2 + (20 * index * _gui_size) + _gui_size * 4;
	int y = WIN_HEIGHT - (22 * _gui_size) * 2 + _gui_size * 4;
	s_blocks[type]->addItem(this, x, y, _gui_size, 15, false, false);
}

void UI::addQuads( std::vector<std::array<int, 3>> &vertices, int atlas, int posX, int posY, int width, int height, int texX, int texY, int texWidth, int texHeight )
{
	--texWidth;--texHeight;
	vertices.push_back({texX + 0        + ((texY + 0        ) << 8) + (0 << 16) + (0 << 17) + (atlas << 18), posX,         posY});
	vertices.push_back({texX + texWidth + ((texY + 0        ) << 8) + (1 << 16) + (0 << 17) + (atlas << 18), posX + width, posY});
	vertices.push_back({texX + 0        + ((texY + texHeight) << 8) + (0 << 16) + (1 << 17) + (atlas << 18), posX,         posY + height});

	vertices.push_back({texX + texWidth + ((texY + 0        ) << 8) + (1 << 16) + (0 << 17) + (atlas << 18), posX + width, posY});
	vertices.push_back({texX + texWidth + ((texY + texHeight) << 8) + (1 << 16) + (1 << 17) + (atlas << 18), posX + width, posY + height});
	vertices.push_back({texX + 0        + ((texY + texHeight) << 8) + (0 << 16) + (1 << 17) + (atlas << 18), posX,         posY + height});
}

void UI::add_dura_value( std::vector<std::array<int, 3>> &vertices, int index )
{
	glm::ivec2 dura = _inventory.getSlotBlock(index).dura;
	if (dura.y == 0 || dura.x == dura.y) {
		return ;
	}
	// adding grey bar first
	addQuads(vertices, 0, (WIN_WIDTH - (182 * _gui_size)) / 2 + (20 * index * _gui_size) + _gui_size * 3 + _gui_size, WIN_HEIGHT - (22 * _gui_size) * 2 + _gui_size * 3 + 14 * _gui_size, 14 * _gui_size, _gui_size, 64, 0, 1, 1);
	// adding progress bar second
	float percent = 1.0f * dura.x / dura.y;
	addQuads(vertices, 0, (WIN_WIDTH - (182 * _gui_size)) / 2 + (20 * index * _gui_size) + _gui_size * 3 + _gui_size, WIN_HEIGHT - (22 * _gui_size) * 2 + _gui_size * 3 + 14 * _gui_size, static_cast<int>(14 * _gui_size * percent), _gui_size, 103 * (percent < 0.6f) - (percent < 0.3), 16 + 9 * (percent < 0.6f) - 18 * (percent < 0.3f), 1, 1);
}

void UI::add_hearts_holder( std::vector<std::array<int, 3>> &vertices, int index )
{
	addQuads(vertices, 1, (WIN_WIDTH - (182 * _gui_size)) / 2 + _gui_size + (index * 8 * _gui_size), WIN_HEIGHT - (22 * _gui_size) * 2 - (8 * _gui_size) - (2 * _gui_size), 8 * _gui_size, 8 * _gui_size, 0, 16, 9, 9);
}

void UI::add_hearts( std::vector<std::array<int, 3>> &vertices, int index )
{
	addQuads(vertices, 1, (WIN_WIDTH - (182 * _gui_size)) / 2 + _gui_size + (index * 8 * _gui_size), WIN_HEIGHT - (22 * _gui_size) * 2 - (8 * _gui_size) - (2 * _gui_size), 8 * _gui_size, 8 * _gui_size, 18 * (_camera._health_points == (1 + 2 * index)) + 9 * (_camera._health_points > (1 + 2 * index)), 16, 9, 9);
}

void UI::add_armor_holder( std::vector<std::array<int, 3>> &vertices, int index )
{
	addQuads(vertices, 1, (WIN_WIDTH - (182 * _gui_size)) / 2 + _gui_size + (index * 8 * _gui_size), WIN_HEIGHT - (22 * _gui_size) * 2 - (2 * 8 * _gui_size) - (_gui_size * 3), 8 * _gui_size, 8 * _gui_size, 27, 16, 9, 9);
}

void UI::add_armor( std::vector<std::array<int, 3>> &vertices, int index )
{
	addQuads(vertices, 1, (WIN_WIDTH - (182 * _gui_size)) / 2 + _gui_size + (index * 8 * _gui_size), WIN_HEIGHT - (22 * _gui_size) * 2 - (2 * 8 * _gui_size) - (_gui_size * 3), 8 * _gui_size, 8 * _gui_size, 36 + 9 * (index == 3), 16, 9, 9);
}

void UI::add_food_holder( std::vector<std::array<int, 3>> &vertices, int index, int saturation )
{
	addQuads(vertices, 1, (WIN_WIDTH + (182 * _gui_size)) / 2 - 10 * _gui_size - (index * 8 * _gui_size), WIN_HEIGHT - (22 * _gui_size) * 2 - (8 * _gui_size) - (2 * _gui_size), 8 * _gui_size, 8 * _gui_size, ((saturation > 2 * index) ? ((saturation == 2 * index + 1) ? 54 + 18 : 54 + 9): 54), 16, 9, 9);
}

void UI::add_food( std::vector<std::array<int, 3>> &vertices, int index )
{
	addQuads(vertices, 1, (WIN_WIDTH + (182 * _gui_size)) / 2 - 9 * _gui_size - (index * 8 * _gui_size), WIN_HEIGHT - (22 * _gui_size) * 2 - (8 * _gui_size) - (2 * _gui_size), 8 * _gui_size, 8 * _gui_size, 82 + 9 * (_camera._foodLevel == (1 + 2 * index)), 16, 9, 9);
}

void UI::add_bubbles( std::vector<std::array<int, 3>> &vertices, int index )
{
	addQuads(vertices, 1, (WIN_WIDTH + (182 * _gui_size)) / 2 - 9 * _gui_size - (index * 8 * _gui_size), WIN_HEIGHT - (22 * _gui_size) * 2 - (2 * 8 * _gui_size) - (_gui_size * 3), 8 * _gui_size, 8 * _gui_size, 99 + 9 * (_camera.getWaterStatus() == (1 + 2 * index)), 16, 9, 9);
}

void UI::setup_array_buffer( void )
{
	std::vector<std::array<int, 3>> vertices;

	addQuads(vertices, 1, WIN_WIDTH / 2 - 16, WIN_HEIGHT / 2 - 16, 32, 32, 0, 0, 16, 16); // crosshair
	addQuads(vertices, 1, (WIN_WIDTH - (182 * _gui_size)) / 2, WIN_HEIGHT - (22 * _gui_size) * 2, 182 * _gui_size, 22 * _gui_size, 0, 25, 182, 22); // hot bar
	int slotNum = _inventory.getSlotNum();
	addQuads(vertices, 1, (WIN_WIDTH - (182 * _gui_size)) / 2 + (20 * slotNum * _gui_size) - _gui_size, WIN_HEIGHT - (22 * _gui_size) * 2 - _gui_size, 24 * _gui_size, 24 * _gui_size, 0, 47, 24, 24); // slot select

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
	glBufferData(GL_ARRAY_BUFFER, _nb_points * 3 * sizeof(GLint), &vertices[0][0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(ITEM_SPECATTRIB);
	glVertexAttribIPointer(ITEM_SPECATTRIB, 1, GL_INT, 3 * sizeof(GLint), 0);

	glEnableVertexAttribArray(ITEM_POSATTRIB);
	glVertexAttribIPointer(ITEM_POSATTRIB, 2, GL_INT, 3 * sizeof(GLint), (void *)(sizeof(GLint)));

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
			_text->addText((WIN_WIDTH - (182 * _gui_size)) / 2 + ((10 + 20 * index) * _gui_size) + _gui_size * 4 - 6 * _gui_size + _gui_size, WIN_HEIGHT - (22 * _gui_size) * 2 + _gui_size * 12 + _gui_size, 8 * _gui_size, TEXT::BLACK, std::to_string(value / 10));
			_text->addText((WIN_WIDTH - (182 * _gui_size)) / 2 + ((10 + 20 * index) * _gui_size) + _gui_size * 4 - 6 * _gui_size, WIN_HEIGHT - (22 * _gui_size) * 2 + _gui_size * 12, 8 * _gui_size, TEXT::WHITE, std::to_string(value / 10));
		}
		_text->addText((WIN_WIDTH - (182 * _gui_size)) / 2 + ((10 + 20 * index) * _gui_size) + _gui_size * 4 + _gui_size, WIN_HEIGHT - (22 * _gui_size) * 2 + _gui_size * 12 + _gui_size, 8 * _gui_size, TEXT::BLACK, std::to_string(value % 10));
		_text->addText((WIN_WIDTH - (182 * _gui_size)) / 2 + ((10 + 20 * index) * _gui_size) + _gui_size * 4, WIN_HEIGHT - (22 * _gui_size) * 2 + _gui_size * 12, 8 * _gui_size, TEXT::WHITE, std::to_string(value % 10));
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

void UI::setGuiSize( int gui_size )
{
	_gui_size = gui_size;
	_vaoSet = false;
}

GLuint UI::getShaderProgram( void )
{
	return (_shaderProgram);
}

void UI::setup_shader( void )
{
	_text->setup_shader();

	// setup item shader program
	if (_itemShaderProgram) {
		glDeleteProgram(_itemShaderProgram);
	}
	_itemShaderProgram = createShaderProgram(Settings::Get()->getString(SETTINGS::STRING::ITEM_VERTEX_SHADER), "",
										Settings::Get()->getString(SETTINGS::STRING::ITEM_FRAGMENT_SHADER));

	glBindFragDataLocation(_itemShaderProgram, 0, "outColor");

	glBindAttribLocation(_itemShaderProgram, ITEM_SPECATTRIB, "specifications");
	glBindAttribLocation(_itemShaderProgram, ITEM_POSATTRIB, "position");

	glLinkProgram(_itemShaderProgram);
	glUseProgram(_itemShaderProgram);

	glUniform1i(glGetUniformLocation(_itemShaderProgram, "win_width"), WIN_WIDTH);
	glUniform1i(glGetUniformLocation(_itemShaderProgram, "win_height"), WIN_HEIGHT);

	check_glstate("Item_Shader program successfully created\n", true);

	// then setup ui shader program
	if (_shaderProgram) {
		glDeleteProgram(_shaderProgram);
	}
	_shaderProgram = createShaderProgram(Settings::Get()->getString(SETTINGS::STRING::UI_VERTEX_SHADER), "",
										Settings::Get()->getString(SETTINGS::STRING::UI_FRAGMENT_SHADER));

	glBindFragDataLocation(_shaderProgram, 0, "outColor");

	glBindAttribLocation(_shaderProgram, ITEM_SPECATTRIB, "specifications");
	glBindAttribLocation(_shaderProgram, ITEM_POSATTRIB, "position");

	glLinkProgram(_shaderProgram);
	glUseProgram(_shaderProgram);

	glUniform1i(glGetUniformLocation(_shaderProgram, "win_width"), WIN_WIDTH);
	glUniform1i(glGetUniformLocation(_shaderProgram, "win_height"), WIN_HEIGHT);

	check_glstate("UI_Shader program successfully created\n", true);
}

void UI::load_texture( void )
{
	_text->load_texture();

	glUseProgram(_itemShaderProgram);
	glUniform1i(glGetUniformLocation(_itemShaderProgram, "blockAtlas"), 0); // we reuse texture from main shader

	glUseProgram(_shaderProgram);
	glUniform1i(glGetUniformLocation(_shaderProgram, "blockAtlas"), 0);

	if (_textures) {
		glDeleteTextures(2, _textures);
		delete [] _textures;
	}
	_textures = new GLuint[2];
	glGenTextures(2, _textures);

	loadTextureShader(2, _textures[0], Settings::Get()->getString(SETTINGS::STRING::UI_ATLAS));
	glUniform1i(glGetUniformLocation(_shaderProgram, "uiAtlas"), 2);

	loadTextureShader(3, _textures[1], Settings::Get()->getString(SETTINGS::STRING::CONTAINER_ATLAS));
	glUniform1i(glGetUniformLocation(_shaderProgram, "containerAtlas"), 3);
}

void UI::updateWinSize( void )
{
	glUseProgram(_itemShaderProgram);
	glUniform1i(glGetUniformLocation(_itemShaderProgram, "win_width"), WIN_WIDTH);
	glUniform1i(glGetUniformLocation(_itemShaderProgram, "win_height"), WIN_HEIGHT);
	glUseProgram(_shaderProgram);
	glUniform1i(glGetUniformLocation(_shaderProgram, "win_width"), WIN_WIDTH);
	glUniform1i(glGetUniformLocation(_shaderProgram, "win_height"), WIN_HEIGHT);
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

void UI::drawUserInterface( std::string str, int game_mode, float deltaTime )
{
	if (_inventoryMessage.timer > 0) {
		_inventoryMessage.timer -= deltaTime;
		_text->addText((WIN_WIDTH - Utils::Text::textWidth(_gui_size * 3, _inventoryMessage.str)) / 2, WIN_HEIGHT - (35 * _gui_size) * 2, _gui_size * 3, TEXT::WHITE, _inventoryMessage.str);
	}
	_chat->blitMessages(deltaTime);
	_text->addText(12, 24, _gui_size * 3, TEXT::WHITE, str);
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
	(game_mode != GAMEMODE::CREATIVE)
		? glDrawArrays(GL_TRIANGLES, 0, _nb_points)
		: glDrawArrays(GL_TRIANGLES, 0, _nb_points_crea);
	// b.stop("drawArrays");
	// b.reset();

	for (int index = 0; index < 9; index++) {
		display_slot_value(index);
	}
	// b.stop("display numbers");
}

void UI::chatMessage( std::string str, unsigned color )
{
	_chat->chatMessage(str, color);
}

void UI::inventoryMessage( std::string str )
{
	_inventoryMessage = {str, 2};
}

/**
 * @brief display items and text, if !ingame, items are not displayed
 * @param ingame Menu::GetState() >= Menu::Pause
*/
void UI::textToScreen( bool ingame )
{
	if (!ingame || _hideUI) {
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
