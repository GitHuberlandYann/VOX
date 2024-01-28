#include "OpenGL_Manager.hpp"

#include "SOIL/SOIL.h"
typedef struct {
	unsigned char *content;
	int width;
	int height;
}				t_tex;

OpenGL_Manager::OpenGL_Manager( void )
	: _window(NULL), _textures(NULL),
		_key_rdist(0), _render_distance(RENDER_DISTANCE),
		_key_fill(0), _fill(FILL), _key_add_block(0), _key_rm_block(0), _key_pick_block(0), _key_screenshot(0),
		_key_h(0), _key_g(0), _key_j(0), _key_o(0), _key_time_mul(0), _key_jump(0), _key_1(0), _key_2(0), _key_3(0),
		_key_4(0), _key_5(0), _key_6(0), _key_7(0), _key_8(0), _key_9(0),
		_debug_mode(true), _game_mode(CREATIVE), _f5_mode(false), _outline(true), _paused(true),
		_esc_released(true), _e_released(true),
		_break_time(0), _eat_timer(0), _break_frame(0), _block_hit(glm::ivec4(0, 0, 0, blocks::AIR))
{
	std::cout << "Constructor of OpenGL_Manager called" << std::endl << std::endl;
	_world_name = "default.json";
	_camera = new Camera(glm::vec3(1.0f, -2.0f, 66.0f));
	_inventory = new Inventory();
	_ui = new UI(*_inventory, *_camera);
	_menu = new Menu(*_inventory, _ui->getTextPtr());
}

OpenGL_Manager::~OpenGL_Manager( void )
{
	std::cout << "Destructor of OpenGL_Manager called" << std::endl;

	if (_thread.joinable()) {
		_thread.join();
	}

	if (_textures) {
		glDeleteTextures(3, _textures);
		delete [] _textures;
	}
	glDeleteProgram(_shaderProgram);
	glDeleteProgram(_skyShaderProgram);

	glDeleteBuffers(1, &_vboEntities);
	glDeleteVertexArrays(1, &_vaoEntities);

	set_cursor_position_callback(NULL, NULL);
	set_scroll_callback(NULL);
	delete _camera;
	delete _inventory;
	delete _ui;
	delete _menu;

	glfwMakeContextCurrent(NULL);
    glfwTerminate();

	_visible_chunks.clear();
	mtx_perimeter.lock();
	_perimeter_chunks.clear();
	mtx_perimeter.unlock();
	mtx_deleted_chunks.lock();
	_deleted_chunks.clear();
	mtx_deleted_chunks.unlock();
	mtx.lock();
	for (auto& c: _chunks) {
		delete c;
	}
	// std::cout << "chunk size upon destruction " << _chunks.size() << std::endl;
	_chunks.clear();
	mtx.unlock();
	mtx_backup.lock();
	_backups.clear();
	mtx_backup.unlock();

	check_glstate("OpengGL_Manager destructed", true);
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

static void addLine( GLint *vertInt, GLfloat *vertFloat, glm::vec3 a, glm::vec3 b, size_t &index )
{
	vertInt[index] = 11;
	vertFloat[index + 1] = a.x;
	vertFloat[index + 2] = a.y;
	vertFloat[index + 3] = a.z;
	index += 4;
	vertInt[index] = 12 + (1 << 9);
	vertFloat[index + 1] = b.x;
	vertFloat[index + 2] = b.y;
	vertFloat[index + 3] = b.z;
	index += 4;
}

void OpenGL_Manager::drawEntities( int size )
{
	void *vertices = new GLint[(24 + size) * 4];
	GLint *vertInt = static_cast<GLint *>(vertices);
	GLfloat *vertFloat = static_cast<GLfloat *>(vertices);

	size_t index = 0;
	bool hitBox = _block_hit.w != blocks::AIR;
	if (!hitBox) {
		index = 24 * 4;
	} else if (s_blocks[_block_hit.w]->hasHitbox) {
		glm::vec3 hitCenter = s_blocks[_block_hit.w]->hitboxCenter, hitHalfSize = s_blocks[_block_hit.w]->hitboxHalfSize;
		glm::vec3 pos = {_block_hit.x + hitCenter.x - hitHalfSize.x, _block_hit.y + hitCenter.y - hitHalfSize.y, _block_hit.z + hitCenter.z - hitHalfSize.z};
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(2 * hitHalfSize.x, 0, 0), index);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(0, 2 * hitHalfSize.y, 0), index);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(0, 0, 2 * hitHalfSize.z), index);
		pos += glm::vec3(2 * hitHalfSize.x, 2 * hitHalfSize.y, 2 * hitHalfSize.z);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(-2 * hitHalfSize.x, 0, 0), index);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(0, -2 * hitHalfSize.y, 0), index);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(0, 0, -2 * hitHalfSize.z), index);
		pos.x -= 2 * hitHalfSize.x;
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(0, 0, -2 * hitHalfSize.z), index);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(0, -2 * hitHalfSize.y, 0), index);
		pos += glm::vec3(2 * hitHalfSize.x, 0, -2 * hitHalfSize.z);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(0, -2 * hitHalfSize.y, 0), index);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(-2 * hitHalfSize.x, 0, 0), index);
		pos += glm::vec3(0, -2 * hitHalfSize.y, 2 * hitHalfSize.z);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(0, 0, -2 * hitHalfSize.z), index);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(-2 * hitHalfSize.x, 0, 0), index);
	} else {
		glm::vec3 pos = {_block_hit.x - 0.001f, _block_hit.y - 0.001f, _block_hit.z - 0.001f};
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(1.0002f, 0, 0), index);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(0, 1.0002f, 0), index);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(0, 0, 1.0002f), index);
		pos += glm::vec3(1.0002f, 1.0002f, 1.0002f);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(-1.0002f, 0, 0), index);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(0, -1.0002f, 0), index);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(0, 0, -1.0002f), index);
		pos.x -= 1.0002f;
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(0, 0, -1.0002f), index);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(0, -1.0002f, 0), index);
		pos += glm::vec3(1.0002f, 0, -1.0002f);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(0, -1.0002f, 0), index);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(-1.0002f, 0, 0), index);
		pos += glm::vec3(0, -1.0002f, 1.0002f);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(0, 0, -1.0002f), index);
		addLine(vertInt, vertFloat, pos, pos + glm::vec3(-1.0002f, 0, 0), index);
	}

	for (auto e: _entities) {
		vertInt[index] = e.first;
		vertFloat[index + 1] = e.second.x;
		vertFloat[index + 2] = e.second.y;
		vertFloat[index + 3] = e.second.z;
		index += 4;
	}

	glBindVertexArray(_vaoEntities);

	glBindBuffer(GL_ARRAY_BUFFER, _vboEntities);
	glBufferData(GL_ARRAY_BUFFER, (24 + size) * 4 * sizeof(GLint), vertices, GL_STATIC_DRAW);
	delete [] static_cast<GLint*>(vertices);

	glEnableVertexAttribArray(SPECATTRIB);
	glVertexAttribIPointer(SPECATTRIB, 1, GL_INT, 4 * sizeof(GLint), 0);

	glEnableVertexAttribArray(POSATTRIB);
	glVertexAttribPointer(POSATTRIB, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(1 * sizeof(GLint)));

	check_glstate("OpenGL_Manager::drawEntities", false);

	if (hitBox) {
		glDrawArrays(GL_LINES, 0, 24);
	}
	glDrawArrays(GL_TRIANGLES, 24, size);
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void error_callback( int error, const char *msg ) {
    std::string s;
    s = " [" + std::to_string(error) + "] " + msg + '\n';
    std::cerr << s << std::endl;
}

void OpenGL_Manager::setup_window( void )
{
	glfwSetErrorCallback( error_callback );
	if (!glfwInit()) {
    	std::cerr << "glfwInit failure" << std::endl;
        exit(1);
    }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE );
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE); // doesn't seem to work in full sreen mode
	glfwWindowHint(GLFW_CENTER_CURSOR, GL_TRUE); // doesn't seem to work in windowed mode

	// std::cout << "win size is set to " << WIN_WIDTH << ", " << WIN_HEIGHT << std::endl;
	// (IS_LINUX)
	// 	? _window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "MineGraphed", nullptr, nullptr)
	// 	: _window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "MineGraphed", glfwGetPrimaryMonitor(), nullptr);
	_window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "MineGraphed", glfwGetPrimaryMonitor(), nullptr);
	if (_window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit (1);
    }
	_menu->setWindow(_window);

	// int width, height;
	// glfwGetWindowSize(_window, &width, &height);
	// std::cout << "window size is " << width << ", " << height << std::endl;

	// activate opengl context
	glfwMakeContextCurrent(_window);


	// glew is there to use the correct version for all functions
	glewExperimental = GL_TRUE;
	glewInit();

	check_glstate("Window successfully created", true);
}

void OpenGL_Manager::initWorld( void )
{
	_current_chunk = glm::ivec2(-10000, -10000);
	chunk_update();
	_current_chunk = glm::ivec2(-10000, -10000);
	// chunk_update();
}

void OpenGL_Manager::create_shaders( void )
{
	// first setup the ui and text shaders
	_ui->setup_shader();
	_menu->setShaderProgram(_ui->getShaderProgram());

	// then setup the sky/water shader
	std::string sky_vertex_shader_data = get_file_content("Sources/Shaders/sky_vertex.glsl");
	char *sky_vertexSource = &sky_vertex_shader_data[0];

	GLuint skyVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(skyVertexShader, 1, &sky_vertexSource, NULL);
	compile_shader(skyVertexShader, "sky_vertex");

	std::string sky_fragment_shader_data = get_file_content("Sources/Shaders/sky_fragment.glsl");
	char *sky_fragmentSource = &sky_fragment_shader_data[0];

	GLuint skyFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(skyFragmentShader, 1, &sky_fragmentSource, NULL);
	compile_shader(skyFragmentShader, "sky_fragment");

	// Combining shaders into a program
	_skyShaderProgram = glCreateProgram();
	glAttachShader(_skyShaderProgram, skyVertexShader);
	glAttachShader(_skyShaderProgram, skyFragmentShader);

	glBindFragDataLocation(_skyShaderProgram, 0, "outColor");

	glBindAttribLocation(_skyShaderProgram, 0, "position");

	glLinkProgram(_skyShaderProgram);
	glUseProgram(_skyShaderProgram);

	glDeleteShader(skyFragmentShader);
    glDeleteShader(skyVertexShader);

	check_glstate("skyShader program successfully created", true);
	
	// then setup the main shader
	std::string vertex_shader_data = get_file_content("Sources/Shaders/vertex.glsl");
	char *vertexSource = &vertex_shader_data[0];

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	compile_shader(vertexShader, "vertex");

	std::string fragment_shader_data = get_file_content("Sources/Shaders/fragment.glsl");
	char *fragmentSource = &fragment_shader_data[0];

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	compile_shader(fragmentShader, "fragment");

	// Combining shaders into a program
	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, vertexShader);
	glAttachShader(_shaderProgram, fragmentShader);

	glBindFragDataLocation(_shaderProgram, 0, "outColor");

	glBindAttribLocation(_shaderProgram, SPECATTRIB, "specifications");
	glBindAttribLocation(_shaderProgram, POSATTRIB, "position");

	glLinkProgram(_shaderProgram);
	glUseProgram(_shaderProgram);

	glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

	check_glstate("Shader program successfully created", true);
}

void OpenGL_Manager::setup_communication_shaders( void )
{
	_uniFog = glGetUniformLocation(_shaderProgram, "fogDist");
	glUniform1f(_uniFog, (1 + _render_distance) << CHUNK_SHIFT);
	_skyUniFog = glGetUniformLocation(_skyShaderProgram, "fogDist");
	glUseProgram(_skyShaderProgram);
	glUniform1f(_skyUniFog, (1 + _render_distance) << CHUNK_SHIFT);
	glUseProgram(_shaderProgram);

	_uniView = glGetUniformLocation(_shaderProgram, "view");
	_skyUniView = glGetUniformLocation(_skyShaderProgram, "view");
	update_cam_view();

	_uniProj = glGetUniformLocation(_shaderProgram, "proj");
	_skyUniProj = glGetUniformLocation(_skyShaderProgram, "proj");
	update_cam_perspective();

	DayCycle::Get()->setUniInternalLight(glGetUniformLocation(_shaderProgram, "internal_light"));

	_skyUniColor = glGetUniformLocation(_skyShaderProgram, "color");
	_skyUniAnim = glGetUniformLocation(_skyShaderProgram, "animFrame");

	check_glstate("\nCommunication with shader program successfully established", true);

	glGenVertexArrays(1, &_vaoEntities);
	glGenBuffers(1, &_vboEntities);
	check_glstate("VAO and VBO for entities", false);
}

void OpenGL_Manager::load_texture( std::string texture_file )
{
	_textures = new GLuint[3];
	glGenTextures(3, _textures);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _textures[0]);

	// load image
	t_tex texture;
	texture.content = SOIL_load_image(texture_file.c_str(), &texture.width, &texture.height, 0, SOIL_LOAD_RGBA);
	if (!texture.content) {
		std::cerr << "failed to load image " << texture_file << " because:" << std::endl << SOIL_last_result() << std::endl;
		exit(1);
	}

	// load image as texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, texture.content);

	glUniform1i(glGetUniformLocation(_shaderProgram, "blockAtlas"), 0); // sampler2D #index in fragment shader
			
	// set settings for texture wraping and size modif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST because pixel art, otherwise GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SOIL_free_image_data(texture.content);

	check_glstate("Succesfully loaded " + texture_file + " to shader", true);



	glUseProgram(_skyShaderProgram);
	glUniform1i(glGetUniformLocation(_skyShaderProgram, "blockAtlas"), 0); // we reuse texture from main shader
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, _textures[1]);

	// load image
	texture.content = SOIL_load_image("Resources/waterStill.png", &texture.width, &texture.height, 0, SOIL_LOAD_RGBA);
	if (!texture.content) {
		std::cerr << "failed to load image Resources/waterStill.png because:" << std::endl << SOIL_last_result() << std::endl;
		exit(1);
	}

	// load image as texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, texture.content);

	glUniform1i(glGetUniformLocation(_skyShaderProgram, "waterStill"), 4); // sampler2D #index in fragment shader
			
	// set settings for texture wraping and size modif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST because pixel art, otherwise GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SOIL_free_image_data(texture.content);

	check_glstate("Succesfully loaded Resources/waterStill.png to shader", true);

	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_2D, _textures[2]);

	// load image
	texture.content = SOIL_load_image("Resources/waterFlow.png", &texture.width, &texture.height, 0, SOIL_LOAD_RGBA);
	if (!texture.content) {
		std::cerr << "failed to load image Resources/waterFlow.png because:" << std::endl << SOIL_last_result() << std::endl;
		exit(1);
	}

	// load image as texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, texture.content);

	glUniform1i(glGetUniformLocation(_skyShaderProgram, "waterFlow"), 5); // sampler2D #index in fragment shader
			
	// set settings for texture wraping and size modif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST because pixel art, otherwise GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SOIL_free_image_data(texture.content);

	check_glstate("Succesfully loaded Resources/waterFlow.png to shader", true);
}

void OpenGL_Manager::main_loop( void )
{
	// glEnable(GL_DEPTH_TEST); // culling messes up with flower visual and doesn't seem to gain fps
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_FRONT);
	// 	// glFrontFace(GL_CW);
	// (glIsEnabled(GL_CULL_FACE))
	// 	? std::cout << "culling enabled" << std::endl
	// 	: std::cout << "culling disabled" << std::endl;

	// glLineWidth(2);
	glEnable(GL_LINE_SMOOTH); // anti-aliasing
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSwapInterval(1);
	glClearColor(0, 0, 0, 1.0f); // start of black, will be changed in game by DayCycle

	set_cursor_position_callback( NULL, _menu );
	set_scroll_callback(NULL);
	glfwSetCursorPosCallback(_window, cursor_position_callback);
	glfwSetScrollCallback(_window, scroll_callback);

	check_glstate("setup done, entering main loop\n", true);

	// std::cout << "60fps game is 16.6666 ms/frame; 30fps game is 33.3333 ms/frame." << std::endl; 
	double lastTime = glfwGetTime(), lastGameTick = lastTime;
	int nbFrames = 0, nbFramesLastSecond = 0, nbTicks = 0, nbTicksLastSecond = 0;

	double previousFrame = lastTime;
	int backFromMenu = 0; // TODO check this var's shinanigans

	bool fluidUpdate = false, animUpdate = false, tickUpdate = false;

	// main loop cheking for inputs and rendering everything
	while (!glfwWindowShouldClose(_window))
	{
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - previousFrame;
		// if (deltaTime > 0.03) {
		// 	std::cerr << "on frame " << nbFrames << ": " << deltaTime * 1000 << " ms" << std::endl;
		// }
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 ){
			// if (0 && !_debug_mode) {
			// 	std::cout << 1000.0/double(nbFrames) << " ms/frame; " << nbFrames << " fps" << std::endl;
			// 	// std::cout << "other math gives " << (deltaTime) * 1000 << "ms/frame" << std::endl;
			// }
			nbFramesLastSecond = nbFrames;
			nbFrames = 0;
			nbTicksLastSecond = nbTicks;
			nbTicks = 0;
			lastTime += 1.0;
		}
		if (currentTime - lastGameTick >= 0.05) {
			tickUpdate = true;
			++nbTicks;
			lastGameTick += 0.05;
			fluidUpdate = (nbTicks == 5 || nbTicks == 10 || nbTicks == 15 || nbTicks == 20);
			animUpdate = (nbTicks & 0x1);
			if (!_paused || _menu->getState() >= INVENTORY_MENU) {
				_camera->tickUpdate();
			}
		} else {
			tickUpdate = false;
			fluidUpdate = false;
			animUpdate = false;
		}

		glUseProgram(_shaderProgram);
		// Bench b;
		if (!_paused) {
			if (++backFromMenu != 1) {
				user_inputs(deltaTime, ++backFromMenu > 3);
			}
			chunk_update();
			DayCycle::Get()->update(deltaTime);
		} else if (_menu->getState() >= INVENTORY_MENU) {
			DayCycle::Get()->update(deltaTime);
		}
		// b.stamp("user inputs");
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLint newVaoCounter = 0, faceCounter = 0, waterFaces = 0, skyFaces = 0;
		for (auto& c: _visible_chunks) {
			c->drawArray(newVaoCounter, faceCounter);
			if (!_paused || _menu->getState() >= INVENTORY_MENU || _menu->getState() == DEATH_MENU) {
				c->updateFurnaces(currentTime);
				if (fluidUpdate) {
					c->updateFluids();
				}
				if (tickUpdate) {
					c->updateTick();
				}
				c->updateEntities(_entities, deltaTime);
			}
		}
		// if (newVaoCounter) {
		// 	std::cout << "new vao counter: " << newVaoCounter << std::endl;
		// }
		// b.stamp("solids");

		int size = _entities.size();
		drawEntities(size);
		_entities.clear();
		_entities.reserve(size);

		#if 1
		glUseProgram(_skyShaderProgram);
		if (animUpdate) {
			update_anim_frame();
		}
		DayCycle::Get()->setCloudsColor(_skyUniColor);
		for (auto& c: _visible_chunks) {
			c->drawSky(newVaoCounter, skyFaces);
		}
		glUniform3f(_skyUniColor, 0.24705882f, 0.4627451f, 0.89411765f); // water color
		for (auto&c: _visible_chunks) {
			c->drawWater(newVaoCounter, waterFaces);
		}
		// b.stamp("display water sky");
		#endif
		glDisable(GL_DEPTH_TEST);
		// Chunk *chunk_ptr = get_current_chunk_ptr();
		mtx.lock();
		mtx_perimeter.lock();
		mtx_backup.lock();
		std::string str = (_debug_mode)
			? "Timer: " + std::to_string(currentTime)
				+ '\n' + DayCycle::Get()->getInfos()
				+ "\nFPS: " + std::to_string(nbFramesLastSecond) + "\tframe " + std::to_string((deltaTime) * 1000)
				+ "\nTPS: " + std::to_string(nbTicksLastSecond)
				+ '\n' + _camera->getCamString(_game_mode)
				+ "\nBlock\t> " + s_blocks[_block_hit.w]->name
				+ ((_block_hit.w != blocks::AIR) ? "\n\t\t> x: " + std::to_string(_block_hit.x) + " y: " + std::to_string(_block_hit.y) + " z: " + std::to_string(_block_hit.z) : "\n")
				+ ((_game_mode == SURVIVAL) ? "\nBreak time\t> " + std::to_string(_break_time) + "\nBreak frame\t> " + std::to_string(_break_frame) : "")
				+ "\n\nChunk\t> x: " + std::to_string(_current_chunk.x) + " y: " + std::to_string(_current_chunk.y)
				// + ((chunk_ptr) ? chunk_ptr->getAddsRmsString() : "")
				+ "\nDisplayed chunks\t> " + std::to_string(_visible_chunks.size())
				+ '/' + std::to_string(_perimeter_chunks.size()) + '/' + std::to_string(_chunks.size())
				+ "\nDisplayed faces\t> " + std::to_string(faceCounter)
				+ "\nSky faces\t> " + std::to_string(skyFaces)
				+ "\nWater faces\t> " + std::to_string(waterFaces)
				+ "\n\nRender Distance\t> " + std::to_string(_render_distance)
				+ "\nGame mode\t\t> " + ((_game_mode) ? "SURVIVAL" : "CREATIVE")
				+ "\nBackups\t> " + std::to_string(_backups.size())
				+ _inventory->getSlotString()
				+ _menu->getFurnaceString()
				// + _inventory->getDuraString()
				// + _inventory->getInventoryString()
			: "\n\nFPS: " + std::to_string(nbFramesLastSecond) + "\nTPS: " + std::to_string(nbTicksLastSecond);
		mtx_backup.unlock();
		mtx_perimeter.unlock();
		mtx.unlock();
		// b.stamp("stringing");
		if (_menu->getState() >= PAUSE_MENU) {
			_ui->drawUserInterface(str, _game_mode, _f5_mode);
		}
		// b.stamp("UI");
		if (_paused) {
			mtx.lock();
			_menu->setChunks(_chunks);
			mtx.unlock();
			switch (_menu->run(_render_distance)) {
				case (1): // back to game
					if (!IS_LINUX) {
						glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
						if (glfwRawMouseMotionSupported()) {
							glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
						}
					}
					set_cursor_position_callback( _camera, NULL );
					set_scroll_callback(_inventory);
					_paused = false;
					backFromMenu = 0;
					_camera->_update = true;
					break ;
				case (2): // world selected, go into loading mode
					_world_name = _menu->getWorldFile();
					glUseProgram(_shaderProgram); // used by dayCycle to modif internal light
					loadWorld("Worlds/" + _world_name);
					initWorld();
					break ;
				case (3): // save and quit to menu
					resetInputsPtrs();
					saveWorld();
					break ;
				case (4): // skip world selection and play with default seed of 123456
					initWorld();
					break ;
				case (5): // Respawn player, init world again
					_camera->respawn();
					initWorld();
					break ;
				case (6): // Respawn player, then save and quit to menu
					_camera->respawn();
					resetInputsPtrs();
					saveWorld();
					break ;
			}
		}
		_ui->textToScreen();
		// b.stamp("textoscreen");
		mtx_deleted_chunks.lock();
		for (auto& todel: _deleted_chunks) {
			delete todel;
		}
		_deleted_chunks.clear();
		mtx_deleted_chunks.unlock();
		// b.stamp("chunk deletion");

		previousFrame = currentTime;
		glfwSwapBuffers(_window);
		// b.stamp("swap buffer");
		glfwPollEvents();
		// b.stamp("poll events");
		// b.stop("frame");
	}

	check_glstate("\nmain loop successfully exited", true);
}
