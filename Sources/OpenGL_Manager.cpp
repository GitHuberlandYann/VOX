#include "OpenGL_Manager.hpp"
#include "callbacks.hpp"
// #include "Benchmark.hpp"
void thread_chunk_update( OpenGL_Manager *render );

OpenGL_Manager::OpenGL_Manager( void )
	: _window(NULL), _textures(NULL),
		_render_distance(RENDER_DISTANCE), _fill(FILL),
		_debug_mode(true), _game_mode(CREATIVE), _outline(true), _paused(true),
		_threadUpdate(false), _threadStop(false),
		_break_time(0), _eat_timer(0), _bow_timer(0), _break_frame(0), _block_hit({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 0, 0})
{
	std::cout << "Constructor of OpenGL_Manager called" << std::endl << std::endl;
	_world_name = "default.json";
	_camera = new Camera(glm::vec3(1.0f, -2.0f, 66.0f));
	_inventory = new Inventory();
	_ui = new UI(*_inventory, *_camera);
	_ui->getChatPtr()->setOGLManPtr(this);
	_menu = new Menu(*_inventory, _ui);

	startThread();
}

OpenGL_Manager::~OpenGL_Manager( void )
{
	std::cout << "Destructor of OpenGL_Manager called" << std::endl;

	stopThread();

	if (_textures) {
		glDeleteTextures(5, _textures);
		delete [] _textures;
	}
	glDeleteProgram(_shaderProgram);
	glDeleteProgram(_skyShaderProgram);
	glDeleteProgram(_particleShaderProgram);

	glDeleteBuffers(1, &_vboEntities);
	glDeleteVertexArrays(1, &_vaoEntities);

	glDeleteBuffers(1, &_vboParticles);
	glDeleteVertexArrays(1, &_vaoParticles);

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
		c->setBackup(_backups);
		delete c;
	}
	// std::cout << "chunk size upon destruction " << _chunks.size() << std::endl;
	_chunks.clear();
	mtx.unlock();
	mtx_backup.lock();
	for (auto &b : _backups) {
		for (auto ch : b.second.chests) {
			delete ch.second;
		}
		for (auto fur : b.second.furnaces) {
			delete fur.second;
		}
	}
	_backups.clear();
	mtx_backup.unlock();

	DayCycle::Destroy();
	check_glstate("openGL_Manager destructed", true);
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void OpenGL_Manager::addLine( glm::vec3 a, glm::vec3 b )
{
	_entities.push_back({11, a});
	_entities.push_back({12 + (1 << 9), b});
}

void OpenGL_Manager::drawEntities( void )
{
	// TODO update hand entity
	// _hand->update(deltaTime);
	size_t esize = _entities.size();

	bool hitBox = (_block_hit.value != blocks::AIR) && (_block_hit.value != blocks::CHEST);
	if (!hitBox) {
	} else if (s_blocks[_block_hit.value]->hasHitbox) {
		glm::vec3 hitCenter = s_blocks[_block_hit.value]->hitboxCenter, hitHalfSize = s_blocks[_block_hit.value]->hitboxHalfSize;
		glm::vec3 pos = {_block_hit.pos.x + hitCenter.x - hitHalfSize.x, _block_hit.pos.y + hitCenter.y - hitHalfSize.y, _block_hit.pos.z + hitCenter.z - hitHalfSize.z};
		addLine(pos, pos + glm::vec3(2 * hitHalfSize.x, 0, 0));
		addLine(pos, pos + glm::vec3(0, 2 * hitHalfSize.y, 0));
		addLine(pos, pos + glm::vec3(0, 0, 2 * hitHalfSize.z));
		pos += glm::vec3(2 * hitHalfSize.x, 2 * hitHalfSize.y, 2 * hitHalfSize.z);
		addLine(pos, pos + glm::vec3(-2 * hitHalfSize.x, 0, 0));
		addLine(pos, pos + glm::vec3(0, -2 * hitHalfSize.y, 0));
		addLine(pos, pos + glm::vec3(0, 0, -2 * hitHalfSize.z));
		pos.x -= 2 * hitHalfSize.x;
		addLine(pos, pos + glm::vec3(0, 0, -2 * hitHalfSize.z));
		addLine(pos, pos + glm::vec3(0, -2 * hitHalfSize.y, 0));
		pos += glm::vec3(2 * hitHalfSize.x, 0, -2 * hitHalfSize.z);
		addLine(pos, pos + glm::vec3(0, -2 * hitHalfSize.y, 0));
		addLine(pos, pos + glm::vec3(-2 * hitHalfSize.x, 0, 0));
		pos += glm::vec3(0, -2 * hitHalfSize.y, 2 * hitHalfSize.z);
		addLine(pos, pos + glm::vec3(0, 0, -2 * hitHalfSize.z));
		addLine(pos, pos + glm::vec3(-2 * hitHalfSize.x, 0, 0));
	} else {
		glm::vec3 pos = {_block_hit.pos.x - 0.001f, _block_hit.pos.y - 0.001f, _block_hit.pos.z - 0.001f};
		addLine(pos, pos + glm::vec3(1.0002f, 0, 0));
		addLine(pos, pos + glm::vec3(0, 1.0002f, 0));
		addLine(pos, pos + glm::vec3(0, 0, 1.0002f));
		pos += glm::vec3(1.0002f, 1.0002f, 1.0002f);
		addLine(pos, pos + glm::vec3(-1.0002f, 0, 0));
		addLine(pos, pos + glm::vec3(0, -1.0002f, 0));
		addLine(pos, pos + glm::vec3(0, 0, -1.0002f));
		pos.x -= 1.0002f;
		addLine(pos, pos + glm::vec3(0, 0, -1.0002f));
		addLine(pos, pos + glm::vec3(0, -1.0002f, 0));
		pos += glm::vec3(1.0002f, 0, -1.0002f);
		addLine(pos, pos + glm::vec3(0, -1.0002f, 0));
		addLine(pos, pos + glm::vec3(-1.0002f, 0, 0));
		pos += glm::vec3(0, -1.0002f, 1.0002f);
		addLine(pos, pos + glm::vec3(0, 0, -1.0002f));
		addLine(pos, pos + glm::vec3(-1.0002f, 0, 0));
	}

	glBindVertexArray(_vaoEntities);

	glBindBuffer(GL_ARRAY_BUFFER, _vboEntities);
	glBufferData(GL_ARRAY_BUFFER, ((hitBox) ? esize + 24 : esize) * 4 * sizeof(GLint), &(_entities[0].first), GL_STATIC_DRAW);

	glEnableVertexAttribArray(SPECATTRIB);
	glVertexAttribIPointer(SPECATTRIB, 1, GL_INT, 4 * sizeof(GLint), 0);

	glEnableVertexAttribArray(POSATTRIB);
	glVertexAttribPointer(POSATTRIB, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(1 * sizeof(GLint)));

	check_glstate("OpenGL_Manager::drawEntities", false);

	glDrawArrays(GL_TRIANGLES, 0, esize);
	if (hitBox) {
		glDrawArrays(GL_LINES, esize, 24);
	}

	_entities.clear();
	_entities.reserve(esize);
}

void OpenGL_Manager::drawParticles( void )
{
	_camera->drawPlayer(_particles, _hand_content, _game_mode);
	size_t psize = _particles.size();

	if (!psize) {
		return ;
	}

	glUseProgram(_particleShaderProgram);
	glBindVertexArray(_vaoParticles);

	glBindBuffer(GL_ARRAY_BUFFER, _vboParticles);
	glBufferData(GL_ARRAY_BUFFER, psize * 4 * sizeof(GLint), &(_particles[0].first), GL_STATIC_DRAW);

	glEnableVertexAttribArray(SPECATTRIB);
	glVertexAttribIPointer(SPECATTRIB, 1, GL_INT, 4 * sizeof(GLint), 0);

	glEnableVertexAttribArray(POSATTRIB);
	glVertexAttribPointer(POSATTRIB, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(1 * sizeof(GLint)));

	check_glstate("OpenGL_Manager::drawParticles", false);

	glDrawArrays(GL_TRIANGLES, 0, psize);

	_particles.clear();
	_particles.reserve(psize);
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
	chunk_update();
	setThreadUpdate(true);
}

void OpenGL_Manager::create_shaders( void )
{
	// first setup the ui and text shaders
	_ui->setup_shader();
	_menu->setShaderProgram(_ui->getShaderProgram());

	// then setup the sky/water shader
	_skyShaderProgram = createShaderProgram("sky_vertex", "", "sky_fragment");

	glBindFragDataLocation(_skyShaderProgram, 0, "outColor");

	glBindAttribLocation(_skyShaderProgram, 0, "position");

	glLinkProgram(_skyShaderProgram);
	glUseProgram(_skyShaderProgram);

	check_glstate("skyShader program successfully created", true);

	// then setup particles shader
	_particleShaderProgram = createShaderProgram("particle_vertex", "", "particle_fragment");

	glBindFragDataLocation(_particleShaderProgram, 0, "outColor");

	glBindAttribLocation(_particleShaderProgram, SPECATTRIB, "specifications");
	glBindAttribLocation(_particleShaderProgram, POSATTRIB, "position");

	glLinkProgram(_particleShaderProgram);
	glUseProgram(_particleShaderProgram);

	check_glstate("particleShader program successfully created", true);
	
	// then setup the main shader
	_shaderProgram = createShaderProgram("vertex", "", "fragment");

	glBindFragDataLocation(_shaderProgram, 0, "outColor");

	glBindAttribLocation(_shaderProgram, SPECATTRIB, "specifications");
	glBindAttribLocation(_shaderProgram, POSATTRIB, "position");

	glLinkProgram(_shaderProgram);
	glUseProgram(_shaderProgram);

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
	_partUniView = glGetUniformLocation(_particleShaderProgram, "view");
	update_cam_view();

	_uniProj = glGetUniformLocation(_shaderProgram, "proj");
	_skyUniProj = glGetUniformLocation(_skyShaderProgram, "proj");
	_partUniProj = glGetUniformLocation(_particleShaderProgram, "proj");
	update_cam_perspective();

	DayCycle::Get()->setUniInternalLight(_shaderProgram, _particleShaderProgram,
		glGetUniformLocation(_shaderProgram, "internal_light"), glGetUniformLocation(_particleShaderProgram, "internal_light"));

	_skyUniColor = glGetUniformLocation(_skyShaderProgram, "color");
	_skyUniAnim = glGetUniformLocation(_skyShaderProgram, "animFrame");

	check_glstate("\nCommunication with shader program successfully established", true);

	glGenVertexArrays(1, &_vaoEntities);
	glGenBuffers(1, &_vboEntities);
	check_glstate("VAO and VBO for entities", false);
	glGenVertexArrays(1, &_vaoParticles);
	glGenBuffers(1, &_vboParticles);
	check_glstate("VAO and VBO for particles", false);
}

void OpenGL_Manager::load_texture( std::string texture_file )
{
	_textures = new GLuint[5];
	glGenTextures(5, _textures);

	loadTextureShader(0, _textures[0], texture_file);
	glUniform1i(glGetUniformLocation(_shaderProgram, "blockAtlas"), 0); // sampler2D #index in fragment shader

	glUseProgram(_skyShaderProgram);
	glUniform1i(glGetUniformLocation(_skyShaderProgram, "blockAtlas"), 0); // we reuse texture from main shader

	loadTextureShader(4, _textures[1], "Resources/waterStill.png");
	glUniform1i(glGetUniformLocation(_skyShaderProgram, "waterStill"), 4);

	loadTextureShader(5, _textures[2], "Resources/waterFlow.png");
	glUniform1i(glGetUniformLocation(_skyShaderProgram, "waterFlow"), 5);

	glUseProgram(_particleShaderProgram);
	loadTextureShader(6, _textures[3], "Resources/particleAtlas.png");
	glUniform1i(glGetUniformLocation(_particleShaderProgram, "blockAtlas"), 0); // we reuse texture from main shader
	glUniform1i(glGetUniformLocation(_particleShaderProgram, "particleAtlas"), 6);
	loadTextureShader(7, _textures[4], "Resources/modelAtlas.png");
	glUniform1i(glGetUniformLocation(_particleShaderProgram, "modelAtlas"), 7);
}

void OpenGL_Manager::setGamemode( bool gamemode )
{
	_game_mode = gamemode;
	_camera->resetFall();
	_ui->chatMessage(std::string("Gamemode set to ") + ((gamemode) ? "SURVIVAL" : "CREATIVE"));
}

void OpenGL_Manager::getGamemode( void )
{
	_ui->chatMessage(std::string("Current gamemode is ") + ((_game_mode) ? "SURVIVAL" : "CREATIVE"));
}

size_t OpenGL_Manager::clearEntities( void )
{
	size_t res = 0;
	for (auto c : _perimeter_chunks) {
		res += c->clearEntities();
	}
	return (res);
}

size_t OpenGL_Manager::clearParticles( void )
{
	size_t res = 0;
	for (auto c : _perimeter_chunks) {
		res += c->clearParticles();
	}
	return (res);
}

void OpenGL_Manager::main_loop( void )
{
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_FRONT);
	// (glIsEnabled(GL_CULL_FACE))
	// 	? std::cout << "culling enabled" << std::endl
	// 	: std::cout << "culling disabled" << std::endl;

	// glLineWidth(2);
	glEnable(GL_LINE_SMOOTH); // anti-aliasing
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSwapInterval(1);
	glClearColor(0, 0, 0, 1.0f); // start of black, will be changed in game by DayCycle

	_menu->setState(MENU::MAIN);
	glfwSetCursorPosCallback(_window, cursor_position_callback);
	glfwSetScrollCallback(_window, scroll_callback);

	glfwSetKeyCallback(_window, INPUT::key_callback);
	glfwSetMouseButtonCallback(_window, INPUT::mouse_button_callback);

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
		// if (deltaTime > 0.1f) std::cout << "\t\tPREVIOUS FRAME AT " << deltaTime << std::endl;
		bool gamePaused = _paused && _menu->getState() < MENU::INVENTORY && _menu->getState() != MENU::DEATH;

		++nbFrames;
		if (currentTime - lastTime >= 1.0) {
			nbFramesLastSecond = nbFrames;
			nbFrames = 0;
			nbTicksLastSecond = nbTicks;
			nbTicks = 0;
			lastTime += 1.0;
		}
		glUseProgram(_shaderProgram); // must be before DayCycle tickUpdate
		if (currentTime - lastGameTick >= TICK) {
			tickUpdate = true;
			++nbTicks;
			lastGameTick += TICK;
			fluidUpdate = (nbTicks == 5 || nbTicks == 10 || nbTicks == 15 || nbTicks == 20);
			animUpdate = (nbTicks & 0x1);
			if (!gamePaused) {
				_camera->tickUpdate();
				DayCycle::Get()->tickUpdate();
			}
		} else {
			tickUpdate = false;
			fluidUpdate = false;
			animUpdate = false;
		}

		// Bench b;
		if (!_paused) {
			if (++backFromMenu != 1) {
				user_inputs(deltaTime, ++backFromMenu > 3);
			}
			chunk_update();
		} else if (_camera->_update && _menu->getState() >= MENU::INVENTORY) {
			// _ui->chatMessage("debug time");
			chunk_update();
			update_cam_view();
			update_visible_chunks();
			if (!_visible_chunks.size()) {
				_current_chunk.x += 32; // we want chunk_update to call thread on next loop
				_camera->_update = true;
			}
		}
		// b.stamp("user inputs");
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLint newVaoCounter = 0, faceCounter = 0, waterFaces = 0, skyFaces = 0;
		for (auto& c: _visible_chunks) {
			c->drawArray(newVaoCounter, faceCounter);
			if (!gamePaused) {
				if (fluidUpdate) {
					c->updateFluids();
					c->updateScheduledBlocks();
				}
				if (tickUpdate) {
					c->updateFurnaces(currentTime);
					c->tickUpdate();
				}
				c->updateEntities(_entities, _particles, deltaTime);
				c->updateParticles(_particles, deltaTime);
			}
		}
		// if (newVaoCounter) {
		// 	std::cout << "new vao counter: " << newVaoCounter << std::endl;
		// }
		// b.stamp("solids");

		if (!gamePaused) {
			drawParticles();
			glUseProgram(_shaderProgram);
			drawEntities();
		}

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
		if (_menu->getState() >= MENU::PAUSE) {
			std::string str;
			if (_debug_mode) {
				str = "Timer: " + std::to_string(currentTime)
						+ '\n' + DayCycle::Get()->getInfos()
						+ "\nFPS: " + std::to_string(nbFramesLastSecond) + "\tframe " + std::to_string((deltaTime) * 1000)
						+ "\nTPS: " + std::to_string(nbTicksLastSecond)
						+ '\n' + _camera->getCamString(_game_mode)
						+ "\nBlock\t> " + s_blocks[_block_hit.value]->name
						+ ((_block_hit.value != blocks::AIR) ? "\n\t\t> x: " + std::to_string(_block_hit.pos.x) + " y: " + std::to_string(_block_hit.pos.y) + " z: " + std::to_string(_block_hit.pos.z) : "\n")
						+ ((_block_hit.value) ? "\nprev\t> x: " + std::to_string(_block_hit.prev_pos.x) + " y: " + std::to_string(_block_hit.prev_pos.y) + " z: " + std::to_string(_block_hit.prev_pos.z) : "\nprev\t> none")
						+ ((_block_hit.water_value) ? "\n\t\tWATER on the way" : "\n\t\tno water")
						+ ((_game_mode == SURVIVAL) ? "\nBreak time\t> " + std::to_string(_break_time) + "\nBreak frame\t> " + std::to_string(_break_frame) : "\n\n")
						+ "\n\nChunk\t> x: " + std::to_string(_current_chunk.x) + " y: " + std::to_string(_current_chunk.y)
						// + ((chunk_ptr) ? chunk_ptr->getAddsRmsString() : "")
						+ "\nDisplayed chunks\t> " + std::to_string(_visible_chunks.size());
				
				mtx_perimeter.lock();
				str += '/' + std::to_string(_perimeter_chunks.size());
				mtx_perimeter.unlock();
				mtx.lock();
				str += '/' + std::to_string(_chunks.size());
				mtx.unlock();
				str += "\nDisplayed faces\t> " + std::to_string(faceCounter)
						+ "\nSky faces\t> " + std::to_string(skyFaces)
						+ "\nWater faces\t> " + std::to_string(waterFaces)
						+ "\n\nRender Distance\t> " + std::to_string(_render_distance)
						+ "\nGame mode\t\t> " + ((_game_mode) ? "SURVIVAL" : "CREATIVE");
				mtx_backup.lock();
				str += "\nBackups\t> " + std::to_string(_backups.size());
				mtx_backup.unlock();
				str += _inventory->getSlotString()
						+ _menu->getInfoString();
						// + _inventory->getDuraString()
						// + _inventory->getInventoryString();
			} else {
				str = "\n\nFPS: " + std::to_string(nbFramesLastSecond) + "\nTPS: " + std::to_string(nbTicksLastSecond);
			}
			// b.stamp("stringing");
			_ui->drawUserInterface(str, _game_mode, deltaTime);
		}
		// b.stamp("UI");
		if (_paused) {
			if (_menu->getState() == MENU::LOAD) {
				mtx.lock();
				_menu->setChunks(_chunks);
				mtx.unlock();
			}
			switch (_menu->run(_render_distance, nbTicks == 1 && tickUpdate)) {
				case (1): // back to game
					if (!IS_LINUX) {
						glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
						if (glfwRawMouseMotionSupported()) {
							glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
						}
					}
					set_cursor_position_callback(_camera, NULL);
					set_scroll_callback(_inventory);
					_paused = false;
					backFromMenu = 0;
					_camera->_update = true;
					setThreadUpdate(true);
					break ;
				case (2): // world selected, go into loading mode
					_world_name = _menu->getWorldFile();
					glUseProgram(_shaderProgram); // used by dayCycle to modif internal light
					loadWorld("Worlds/" + _world_name);
					initWorld();
					break ;
				case (4): // skip world selection and play with default seed of 123456
					DayCycle::Get()->addTicks(0); // used to forceReset sky color
					initWorld();
					break ;
				case (5): // Respawn player, init world again
					_camera->respawn();
					initWorld();
					break ;
				case (6): // Respawn player, then save and quit to menu
					_camera->respawn();
				case (3): // save and quit to menu
					resetInputsPtrs();
					saveWorld();
					_ui->_hideUI = true;
					break ;
				case (7): // fov change
					update_cam_perspective();
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
