#include "vox.h"

Menu::Menu( Inventory & inventory, Text *text ) : _state(MAIN_MENU), _vaoSet(false), _esc_released(false), _e_released(false), _inventory(inventory), _text(text)
{
	(void)_inventory;
}

Menu::~Menu( void )
{
	// delete _text; // nope, this is done in ui as they share same ptr
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

int Menu::main_menu( GLFWwindow *window )
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (_selection == 1) { //singleplayer
			_state = LOAD_MENU;
			_selection = 0;
			return (2);
		} else if (_selection == 6) { //quit game
			glfwSetWindowShouldClose(window, GL_TRUE);
			return (-1);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		_state = LOAD_MENU;
		_selection = 0;
		return (2);
	}

	setup_array_buffer_main();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	_text->displayText(WIN_WIDTH / 2 - 220, WIN_HEIGHT / 2 - 120 * 3, 24, glm::vec3(1.0f, 1.0f, 1.0f), "Press space to start");

	int mult = 3;
	// shadows
	_text->displayText(WIN_WIDTH / 2 - 110 + mult, WIN_HEIGHT / 2 - 10 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Singleplayer");
	_text->displayText(WIN_WIDTH / 2 - 100 + mult, WIN_HEIGHT / 2 + 15 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Multiplayer");
	_text->displayText(WIN_WIDTH / 2 - 150 + mult, WIN_HEIGHT / 2 + 40 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Minecraft Realms");
	_text->displayText(WIN_WIDTH / 2 - 50 * mult - 80 + mult, WIN_HEIGHT / 2 + 80 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Options...");
	_text->displayText(WIN_WIDTH / 2 + 50 * mult - 75 + mult, WIN_HEIGHT / 2 + 80 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Quit Game");
	// white
	_text->displayText(WIN_WIDTH / 2 - 110, WIN_HEIGHT / 2 - 10 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Singleplayer");
	_text->displayText(WIN_WIDTH / 2 - 100, WIN_HEIGHT / 2 + 15 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Multiplayer");
	_text->displayText(WIN_WIDTH / 2 - 150, WIN_HEIGHT / 2 + 40 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Minecraft Realms");
	_text->displayText(WIN_WIDTH / 2 - 50 * mult - 80, WIN_HEIGHT / 2 + 80 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Options...");
	_text->displayText(WIN_WIDTH / 2 + 50 * mult - 75, WIN_HEIGHT / 2 + 80 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Quit Game");
	return (0);
}

int Menu::loading_screen( std::list<Chunk *> chunks, GLint render_dist )
{
	GLint current_size = 0, newVaoCounter = 0;
	for (auto& c : chunks) {
		mtx.lock();
		current_size += c->isLoaded(newVaoCounter);
		mtx.unlock();
	}

	GLint goal = 1 + (2 * render_dist) * (2 * render_dist);
	// std::cout << "CURRENT IS " << current_size << ", GOAL IS " << goal << std::endl;
	if (current_size >= goal) {
		_state = PAUSE_MENU;
		return (1);
	}
	float percentage = current_size / static_cast<float>(goal) * 100;

	setup_array_buffer_load(percentage * 10);
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	std::string str = "LOADING.. " + std::to_string(percentage) + '%' + " (" + std::to_string(current_size) + '/' + std::to_string(goal) + ')';
	_text->displayText(WIN_WIDTH / 2 - 24 * str.length() / 2, WIN_HEIGHT / 2 - 150, 24, glm::vec3(1.0f, 1.0f, 1.0f), str);
	return (0);
}

int Menu::pause_menu( GLFWwindow* window )
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (_selection == 1) { //Back to Game
			_esc_released = false;
			_selection = 0;
			return (1);
		} else if (_selection == 8) { //Save and Quit to Title
			_state = MAIN_MENU;
			_selection = 0;
			return (3);
		}
	}
	if (_esc_released && glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		_esc_released = false;
		return (1);
	} else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
		_esc_released = true;
	}

	setup_array_buffer_pause();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);

	int mult = 3;
	// first draw text shadow
	// _text->displayText(WIN_WIDTH / 2 - 100 + mult, WIN_HEIGHT / 2 - 60 * mult - 40 + mult, 24, glm::vec3(0.0f, 0.0f, 0.0f), "Game Menu");
	_text->displayText(WIN_WIDTH / 2 - 110 + mult, WIN_HEIGHT / 2 - 60 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Back to Game");
	_text->displayText(WIN_WIDTH / 2 - 50 * mult - 120 + mult, WIN_HEIGHT / 2 - 35 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Advancements");
	_text->displayText(WIN_WIDTH / 2 - 50 * mult - 125 + mult, WIN_HEIGHT / 2 - 10 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Give Feedback");
	_text->displayText(WIN_WIDTH / 2 - 50 * mult - 80 + mult, WIN_HEIGHT / 2 + 15 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Options...");
	_text->displayText(WIN_WIDTH / 2 + 50 * mult - 70 + mult, WIN_HEIGHT / 2 - 35 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Statistics");
	_text->displayText(WIN_WIDTH / 2 + 50 * mult - 100 + mult, WIN_HEIGHT / 2 - 10 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Report Bugs");
	_text->displayText(WIN_WIDTH / 2 + 50 * mult - 100 + mult, WIN_HEIGHT / 2 + 15 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Open to LAN");
	_text->displayText(WIN_WIDTH / 2 - 200 + mult, WIN_HEIGHT / 2 + 40 * mult + 6 * mult + mult, 20, glm::vec3(0.0f, 0.0f, 0.0f), "Save and Quit to Title");

	// then draw text in white
	_text->displayText(WIN_WIDTH / 2 - 100, WIN_HEIGHT / 2 - 60 * mult - 40, 24, glm::vec3(1.0f, 1.0f, 1.0f), "Game Menu");
	_text->displayText(WIN_WIDTH / 2 - 110, WIN_HEIGHT / 2 - 60 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Back to Game");
	_text->displayText(WIN_WIDTH / 2 - 50 * mult - 120, WIN_HEIGHT / 2 - 35 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Advancements");
	_text->displayText(WIN_WIDTH / 2 - 50 * mult - 125, WIN_HEIGHT / 2 - 10 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Give Feedback");
	_text->displayText(WIN_WIDTH / 2 - 50 * mult - 80, WIN_HEIGHT / 2 + 15 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Options...");
	_text->displayText(WIN_WIDTH / 2 + 50 * mult - 70, WIN_HEIGHT / 2 - 35 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Statistics");
	_text->displayText(WIN_WIDTH / 2 + 50 * mult - 100, WIN_HEIGHT / 2 - 10 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Report Bugs");
	_text->displayText(WIN_WIDTH / 2 + 50 * mult - 100, WIN_HEIGHT / 2 + 15 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Open to LAN");
	_text->displayText(WIN_WIDTH / 2 - 200, WIN_HEIGHT / 2 + 40 * mult + 6 * mult, 20, glm::vec3(1.0f, 1.0f, 1.0f), "Save and Quit to Title");
	return (0);
}

int Menu::inventory_menu( GLFWwindow* window )
{
	if (_esc_released && glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		_state = PAUSE_MENU;
		_selection = 0;
		_esc_released = false;
		_e_released = false;
		return (3);
	} else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
		_esc_released = true;
	}
	if (_e_released && glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		_selection = 0;
		_esc_released = false;
		_e_released = false;
		return (1);
	} else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
		_e_released = true;
	}

	setup_array_buffer_inventory();
	glUseProgram(_shaderProgram);
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nb_points);
	return (0);
}

void Menu::setup_array_buffer_main( void )
{
	_nb_points = 8;
	int mult = 3;
    GLint vertices[] = { // pos: x y width height textcoord: x y width height
        1, WIN_WIDTH / 2 - 129 * mult, WIN_HEIGHT / 2 - 104 * mult, 256 * mult, 64 * mult, 0, 131, 256, 64, // MINECRAFT

        1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 10 * mult, 200 * mult, 20 * mult, 0, 91 + 20 * (_selection == 1), 200, 20, // Singleplayer
        1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 15 * mult, 200 * mult, 20 * mult, 0, 71, 200, 20, // Multiplayer
        1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 40 * mult, 200 * mult, 20 * mult, 0, 71, 200, 20, // Minecraft Realms

        1, WIN_WIDTH / 2 - 125 * mult, WIN_HEIGHT / 2 + 80 * mult, 15 * mult, 20 * mult, 0, 71, 200, 20, // Lang settings
        1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 80 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20, // Options...
        1, WIN_WIDTH / 2 + 5 * mult, WIN_HEIGHT / 2 + 80 * mult, 95 * mult, 20 * mult, 0, 91 + 20 * (_selection == 6), 200, 20, // Quit Game
        1, WIN_WIDTH / 2 + 110 * mult, WIN_HEIGHT / 2 + 80 * mult, 15 * mult, 20 * mult, 0, 71, 200, 20, // accessibility settings
    };

	setup_shader(vertices);
}

void Menu::setup_array_buffer_load( int completion )
{
	// std::cout << "current completion : " << completion << std::endl;
	_nb_points = 1 + 100 + 1; // background + 10 * 10 cubes + breaking frame
	GLint *vertices = new GLint[9 * _nb_points]; // pos: x y width height textcoord: x y width height

	vertices[0] = 1;
	vertices[1] = WIN_WIDTH / 2 - 110;
	vertices[2] = WIN_HEIGHT / 2 - 120;
	vertices[3] = 220;
	vertices[4] = 240;
	vertices[5] = 0;
	vertices[6] = 71;
	vertices[7] = 200;
	vertices[8] = 20;

	int step = 1, turnCounter = 0, numSteps = 1, stepSize = 20, state = 0, x = WIN_WIDTH / 2 - 20, y = WIN_HEIGHT / 2;
	for (; step < 1 + 100; step++) {
		vertices[step * 9] = 0;
		vertices[step * 9 + 1] = x;
		vertices[step * 9 + 2] = y;
		vertices[step * 9 + 3] = stepSize;
		vertices[step * 9 + 4] = stepSize;
		if (step < 1 + completion / 10) {
			vertices[step * 9 + 5] = 64; // cobblestone
			vertices[step * 9 + 6] = 48;
		} else {
			vertices[step * 9 + 5] = 64; // stone
			vertices[step * 9 + 6] = 32;
		}
		vertices[step * 9 + 7] = 16;
		vertices[step * 9 + 8] = 16;
		if (step == 1 + completion / 10) {
			vertices[101 * 9] = 0;
			vertices[101 * 9 + 1] = x;
			vertices[101 * 9 + 2] = y;
			vertices[101 * 9 + 3] = stepSize;
			vertices[101 * 9 + 4] = stepSize;
			vertices[101 * 9 + 5] = 240; // break frame
			vertices[101 * 9 + 6] = 16 + 16 * (completion % 10);
			vertices[101 * 9 + 7] = 16;
			vertices[101 * 9 + 8] = 16;
		}
		switch (state) {
			case 0:
				x += stepSize;
				break ;
			case 1:
				y -= stepSize;
				break ;
			case 2:
				x -= stepSize;
				break ;
			case 3:
				y += stepSize;
				break ;
		}
		if (step % numSteps == 0) {
			state = (state + 1) % 4;
			turnCounter++;
			if (turnCounter % 2 == 0) {
				numSteps++;
			}
		}
	}

	setup_shader(vertices);
	delete [] vertices;
}

void Menu::setup_array_buffer_pause( void )
{
	_nb_points = 9;
	int mult = 3;
    GLint vertices[] = { // pos: x y width height textcoord: x y width height
		1, 0, 0, WIN_WIDTH, WIN_HEIGHT, 3, 29, 1, 1, // occult window
        1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 60 * mult, 200 * mult, 20 * mult, 0, 91 + 20 * (_selection == 1), 200, 20, // Back to Game

        1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 35 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20, // Advancements
        1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 10 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20, // Give Feedback
        1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 15 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20, // Options...

        1, WIN_WIDTH / 2 + 5 * mult, WIN_HEIGHT / 2 - 35 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20, // Statistics
        1, WIN_WIDTH / 2 + 5 * mult, WIN_HEIGHT / 2 - 10 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20, // Report Bugs
        1, WIN_WIDTH / 2 + 5 * mult, WIN_HEIGHT / 2 + 15 * mult, 95 * mult, 20 * mult, 0, 71, 200, 20, // Open to LAN

        1, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 40 * mult, 200 * mult, 20 * mult, 0, 91 + 20 * (_selection == 8), 200, 20, // Save and Quit to Title
    };

	setup_shader(vertices);
}

void Menu::add_slot_value( GLint *vertices, int mult, int index )
{
	vertices[(1 + index) * 9 + 0] = 0;
	vertices[(1 + index) * 9 + 1] = (WIN_WIDTH - (166 * mult)) / 2 + (18 * index * mult) + mult * 3;
	vertices[(1 + index) * 9 + 2] = WIN_HEIGHT / 2 - 83 * mult + (142) * mult;
	vertices[(1 + index) * 9 + 3] = 16 * mult;
	vertices[(1 + index) * 9 + 4] = 16 * mult;
	mtx_inventory.lock();
	vertices[(1 + index) * 9 + 5] = blockAtlasX(_inventory.getSlotBlock(index).x);
	vertices[(1 + index) * 9 + 6] = blockAtlasY(_inventory.getSlotBlock(index).x);
	mtx_inventory.unlock();
	vertices[(1 + index) * 9 + 7] = 16;
	vertices[(1 + index) * 9 + 8] = 16;
}

void Menu::setup_array_buffer_inventory( void )
{
	_nb_points = 1 + 10;
	int mult = 3;
    GLint *vertices = new GLint[_nb_points * 9]; // pos: x y width height textcoord: x y width height

	vertices[0] = 2;
	vertices[1] = WIN_WIDTH / 2 - 88 * mult;
	vertices[2] = WIN_HEIGHT / 2 - 83 * mult;
	vertices[3] = 176 * mult;
	vertices[4] = 166 * mult;
	vertices[5] = 0;
	vertices[6] = 0;
	vertices[7] = 128; // textcoord are in 256*256
	vertices[8] = 128;

	for (int index = 0; index < 10; index++) {
		add_slot_value(vertices, mult, index);
	}

	setup_shader(vertices);
	delete [] vertices;
}

void Menu::setup_shader( GLint *vertices )
{
	glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
	_vaoSet = true;

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _nb_points * 9 * sizeof(GLint), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(UI_ATLASATTRIB);
	glVertexAttribIPointer(UI_ATLASATTRIB, 1, GL_INT, 9 * sizeof(GLint), 0);

    glEnableVertexAttribArray(UI_POSATTRIB);
	glVertexAttribIPointer(UI_POSATTRIB, 4, GL_INT, 9 * sizeof(GLint), (void *)(sizeof(GLint)));
	
	glEnableVertexAttribArray(UI_TEXATTRIB);
	glVertexAttribIPointer(UI_TEXATTRIB, 4, GL_INT, 9 * sizeof(GLint), (void *)(5 * sizeof(GLint)));

	check_glstate("NO");
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

static bool inRectangle( float posX, float posY, int rx, int ry, int width, int height )
{
	return (posX >= rx && posX <= rx + width && posY >= ry && posY <= ry + height);
}

void Menu::processMouseMovement( float posX, float posY )
{
	if (_state == MAIN_MENU) {
		int mult = 3;
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 10 * mult, 200 * mult, 20 * mult)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 + 5 * mult, WIN_HEIGHT / 2 + 80 * mult, 95 * mult, 20 * mult)) {
			_selection = 6;
		} else {
			_selection = 0;
		}
	} else if (_state == PAUSE_MENU) {
		int mult = 3;
		if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 - 60 * mult, 200 * mult, 20 * mult)) {
			_selection = 1;
		} else if (inRectangle(posX, posY, WIN_WIDTH / 2 - 100 * mult, WIN_HEIGHT / 2 + 40 * mult, 200 * mult, 20 * mult)) {
			_selection = 8;
		} else {
			_selection = 0;
		}
	}
}

void Menu::setShaderProgram( GLuint shaderProgram )
{
	_shaderProgram = shaderProgram;
}

void Menu::setState( int state )
{
	_state = state;
}

int Menu::getState( void )
{
	return (_state);
}

int Menu::run( GLFWwindow* window, std::list<Chunk *> chunks, GLint render_dist )
{
	if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
		return (-1);
	}

	switch (_state) {
		case MAIN_MENU:
			return (main_menu(window));
		case LOAD_MENU:
			return (loading_screen(chunks, render_dist));
		case PAUSE_MENU:
			return (pause_menu(window));
		case INVENTORY_MENU:
			return (inventory_menu(window));
		default:
			return (1);
	}
	return (0);
}
