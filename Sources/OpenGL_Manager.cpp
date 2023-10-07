# include "vox.h"

OpenGL_Manager::OpenGL_Manager( void )
	: _window(NULL), _textures(NULL), _background_color(0.0f, 0.0f, 0.0f),
		_key_rdist(0), _render_distance(RENDER_DISTANCE),
		_key_fill(0), _fill(FILL), _key_add_block(0), _key_rm_block(0), _key_pick_block(0),
		_key_h(0), _key_g(0), _key_j(0), _key_o(0), _key_jump(0), _key_1(0), _key_2(0), _key_3(0),
		_key_4(0), _key_5(0), _key_6(0), _key_7(0), _key_8(0), _key_9(0),
		_debug_mode(true), _game_mode(CREATIVE), _f5_mode(false), _outline(true), _paused(true),
		_esc_released(true), _e_released(true),
		_break_time(0), _break_frame(0), _block_hit(glm::ivec4(0, 0, 0, blocks::AIR))
{
	std::cout << "Constructor of OpenGL_Manager called" << std::endl << std::endl;
	_world_name = "default.json";
	_camera = new Camera(glm::vec3(1.0f, -2.0f, 66.0f));
	_inventory = new Inventory();
	_ui = new UI(*_inventory, *_camera);
	_menu = new Menu(*_inventory, _ui->getTextPtr());
	// render = this;
}

OpenGL_Manager::~OpenGL_Manager( void )
{
	std::cout << "Destructor of OpenGL_Manager called" << std::endl;

	if (_thread.joinable()) {
		_thread.join();
	}

	if (_textures) {
		glDeleteTextures(1, _textures);
		delete [] _textures;
	}
	glDeleteProgram(_shaderProgram);

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
	_backups.clear();

	check_glstate("OpengGL_Manager destructed");
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

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

	check_glstate("Window successfully created");
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
	
	// then setup the main shader
	std::string vertex_shader_data = get_file_content("Sources/Shaders/vertex.glsl");
	char *vertexSource = &vertex_shader_data[0];

	_vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(_vertexShader, 1, &vertexSource, NULL);
	compile_shader(_vertexShader, "vertex");

	std::string geometry_shader_data = get_file_content("Sources/Shaders/geometry.glsl");
	char *geometrySource = &geometry_shader_data[0];

	_geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(_geometryShader, 1, &geometrySource, NULL);
	compile_shader(_geometryShader, "geometry");

	std::string fragment_shader_data = get_file_content("Sources/Shaders/fragment.glsl");
	char *fragmentSource = &fragment_shader_data[0];

	_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(_fragmentShader, 1, &fragmentSource, NULL);
	compile_shader(_fragmentShader, "fragment");

	// Combining shaders into a program
	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, _vertexShader);
	glAttachShader(_shaderProgram, _geometryShader);
	glAttachShader(_shaderProgram, _fragmentShader);

	glBindFragDataLocation(_shaderProgram, 0, "outColor");

	glBindAttribLocation(_shaderProgram, BLOCKATTRIB, "block_type");
	glBindAttribLocation(_shaderProgram, BREAKATTRIB, "break_frame");
	glBindAttribLocation(_shaderProgram, ADJATTRIB, "adj_blocks");
	glBindAttribLocation(_shaderProgram, POSATTRIB, "position");

	glLinkProgram(_shaderProgram);
	glUseProgram(_shaderProgram);

	glDeleteShader(_fragmentShader);
    glDeleteShader(_geometryShader);
    glDeleteShader(_vertexShader);

	check_glstate("Shader program successfully created");
}

void OpenGL_Manager::setup_communication_shaders( void )
{
	_uniView = glGetUniformLocation(_shaderProgram, "view");
	update_cam_view();

	_uniProj = glGetUniformLocation(_shaderProgram, "proj");
	update_cam_perspective();

	// _uniPV = glGetUniformLocation(_shaderProgram, "pv");
	// update_cam_matrix();

	check_glstate("\nCommunication with shader program successfully established");
}

void OpenGL_Manager::load_texture( std::string texture_file )
{
	_textures = new GLuint[1];
	glGenTextures(1, _textures);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _textures[0]);

	// load image
	t_tex *texture = new t_tex;
	texture->content = SOIL_load_image(texture_file.c_str(), &texture->width, &texture->height, 0, SOIL_LOAD_RGBA);
	if (!texture->content) {
		std::cerr << "failed to load image " << texture_file << " because:" << std::endl << SOIL_last_result() << std::endl;
		exit(1);
	}

	// load image as texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, texture->content);

	glUniform1i(glGetUniformLocation(_shaderProgram, "blockAtlas"), 0); // sampler2D #index in fragment shader
			
	// set settings for texture wraping and size modif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST because pixel art, otherwise GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (texture) {
		SOIL_free_image_data(texture->content);
	}
	delete texture;

	check_glstate("Succesfully loaded texture to shader");
}

void OpenGL_Manager::main_loop( void )
{
	// if (!IS_LINUX) { // face culling seems to only work on mac, but breaks flowers and gains 0 fps anyway ..
	// 	glEnable(GL_DEPTH_TEST);
	// 	glEnable(GL_CULL_FACE);
	// 	glCullFace(GL_FRONT);
	// 	// glFrontFace(GL_CW);

	//	(glIsEnabled(GL_CULL_FACE))
	// 		? std::cout << "culling enabled" << std::endl
	//		: std::cout << "culling disabled" << std::endl;
	// }


	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSwapInterval(1);
	glClearColor(_background_color.x, _background_color.y, _background_color.z, 1.0f);

	set_cursor_position_callback( NULL, _menu );
	set_scroll_callback(NULL);
	glfwSetCursorPosCallback(_window, cursor_position_callback);
	glfwSetScrollCallback(_window, scroll_callback);

	check_glstate("setup done, entering main loop\n");

	// std::cout << "60fps game is 16.6666 ms/frame; 30fps game is 33.3333 ms/frame." << std::endl; 
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	int nbFramesLastSecond = 0;

	double previousFrame = lastTime;
	int backFromMenu = 0;

	// main loop cheking for inputs and rendering everything
	while (!glfwWindowShouldClose(_window))
	{
		double currentTime = glfwGetTime();
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 ){
			if (!_debug_mode) {
				std::cout << 1000.0/double(nbFrames) << " ms/frame; " << nbFrames << " fps" << std::endl;
				// std::cout << "other math gives " << (currentTime - previousFrame) * 1000 << "ms/frame" << std::endl;
			}
			nbFramesLastSecond = nbFrames;
			nbFrames = 0;
			lastTime += 1.0;
		}

		glEnable(GL_DEPTH_TEST);
		glUseProgram(_shaderProgram);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (!_paused) {
			if (++backFromMenu != 1) {
				user_inputs(currentTime - previousFrame, ++backFromMenu > 3);
			}
			chunk_update();
		}
		previousFrame = currentTime;

		GLint newVaoCounter = 0, blockCounter = 0;
		for (auto& c: _visible_chunks) {
			c->drawArray(newVaoCounter, blockCounter);
			c->updateFurnaces(currentTime);
		}

		// glClear(GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		// Chunk *chunk_ptr = get_current_chunk_ptr();
		mtx.lock();
		mtx_perimeter.lock();
		std::string str = (_debug_mode)
			? "Timer: " + std::to_string(currentTime)
				+ "\nFPS: " + std::to_string(nbFramesLastSecond) + '\n' + _camera->getCamString(_game_mode)
				+ "\nBlock\t> " + ((_block_hit.w >= blocks::AIR) ? s_blocks[_block_hit.w].name : s_blocks[_block_hit.w + blocks::NOTVISIBLE].name)
				+ ((_block_hit.w != blocks::AIR) ? "\n\t\t> x: " + std::to_string(_block_hit.x) + " y: " + std::to_string(_block_hit.y) + " z: " + std::to_string(_block_hit.z) : "\n")
				+ ((_game_mode == SURVIVAL) ? "\nBreak time\t> " + std::to_string(_break_time) + "\nBreak frame\t> " + std::to_string(_break_frame) : "")
				+ "\n\nChunk\t> x: " + std::to_string(_current_chunk.x) + " y: " + std::to_string(_current_chunk.y)
				// + ((chunk_ptr) ? chunk_ptr->getAddsRmsString() : "")
				+ "\nDisplayed chunks\t> " + std::to_string(_visible_chunks.size())
				+ '/' + std::to_string(_perimeter_chunks.size()) + '/' + std::to_string(_chunks.size())
				+ "\nDisplayed blocks\t> " + std::to_string(blockCounter)
				+ "\n\nRender Distance\t> " + std::to_string(_render_distance)
				+ "\nGame mode\t\t> " + ((_game_mode) ? "SURVIVAL" : "CREATIVE")
				+ "\nBackups\t> " + std::to_string(_backups.size())
				+ _inventory->getSlotString()
				+ _menu->getFurnaceString()
				// + _inventory->getDuraString()
				// + _inventory->getInventoryString()
			: "";
		mtx_perimeter.unlock();
		mtx.unlock();
		if (_menu->getState() >= PAUSE_MENU) {
			_ui->drawUserInterface(str, _game_mode, _f5_mode);
		}
		if (_paused) {
			mtx.lock();
			_menu->setChunks(_chunks);
			mtx.unlock();
			int menu_ret = _menu->run(_render_distance);
			if (menu_ret == 2) {
				_world_name = _menu->getWorldFile();
				loadWorld("Worlds/" + _world_name);
				initWorld();
			} else if (menu_ret == 1) { // back to game
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
			} else if (menu_ret == 3) { // save and quit to menu
				resetInputsPtrs();
				saveWorld();
			} else if (menu_ret == 4) { // skip world selection and play with default seed of 123456
				initWorld();
			}
		}

		mtx_deleted_chunks.lock();
		for (auto& todel: _deleted_chunks) {
			delete todel;
		}
		_deleted_chunks.clear();
		mtx_deleted_chunks.unlock();

		glfwSwapBuffers(_window);
		glfwPollEvents();
	}

	check_glstate("\nmain loop successfully exited");
}
