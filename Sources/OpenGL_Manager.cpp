# include "vox.h"

extern Camera camera;
// OpenGL_Manager *render = NULL;

OpenGL_Manager::OpenGL_Manager( void )
	: _window(NULL), _textures(NULL), _background_color(0.0f, 0.0f, 0.0f),
		_key_rdist(0), _render_distance(RENDER_DISTANCE),
		_key_fill(0), _fill(FILL), _key_add_block(0), _key_rm_block(0),
		_key_h(0), _key_g(0), _key_1(0), _key_2(0), _key_3(0),
		_key_4(0), _key_5(0), _key_6(0), _key_7(0), _key_8(0), _key_9(0),
		_debug_mode(true), _game_mode(CREATIVE)
{
	std::cout << "Constructor of OpenGL_Manager called" << std::endl << std::endl;
	_inventory = new Inventory();
	_ui = new UI();
	// render = this;
}

OpenGL_Manager::~OpenGL_Manager( void )
{
	std::cout << "Destructor of OpenGL_Manager called" << std::endl;
	// if (_thread_block.joinable()) {
	// 	_thread_block.join();
	// }
	if (_thread.joinable()) {
		_thread.join();
	}

	if (_textures) {
		glDeleteTextures(1, _textures);
		delete [] _textures;
	}
	glDeleteProgram(_shaderProgram);

	delete _inventory;
	delete _ui;

	glfwMakeContextCurrent(NULL);
    glfwTerminate();


	mtx_visible_chunks.lock();
	_visible_chunks.clear();
	mtx_visible_chunks.unlock();
	mtx_delete_chunks.lock();
	_delete_chunks.clear();
	mtx_delete_chunks.unlock();
	mtx.lock();
	std::list<Chunk *>::iterator it = _chunks.begin();
	for (; it != _chunks.end(); it++) {
		delete *it;
	}
	// std::cout << "chunk size upon destruction " << _chunks.size() << std::endl;
	_chunks.clear();
	mtx.unlock();

	check_glstate("OpengGL_Manager destructed");
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void OpenGL_Manager::setup_window( void )
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // doesn't seem to work in full sreen mode
	// glfwWindowHint(GLFW_CENTER_CURSOR, GL_TRUE); // doesn't seem to work in windowed mode

	_window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "MineThemeGraphed", glfwGetPrimaryMonitor(), nullptr);
	if (_window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit (1);
    }

	// int width, height;
	// glfwGetWindowSize(_window, &width, &height);
	// std::cout << "size is " << width << ", " << height << std::endl;

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
	chunk_update(); //create chunks
	_current_chunk = glm::ivec2(-10000, -10000);
	chunk_update(); //display created chunks
}

void OpenGL_Manager::create_shaders( void )
{
	// first setup the ui shader
	_ui->setup_shader();
	
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

	glUniform1i(glGetUniformLocation(_shaderProgram, "tex0"), 0); // sampler2D #index in fragment shader
			
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
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported()) {
		glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
	glEnable(GL_DEPTH_TEST);
	glfwSwapInterval(1);
	glClearColor(_background_color.x, _background_color.y, _background_color.z, 1.0f);
	glfwSetCursorPosCallback(_window, cursor_position_callback);
    // glfwSetScrollCallback(_window, scroll_callback);

	check_glstate("setup done, entering main loop\n");

	// std::cout << "60fps game is 16.6666 ms/frame; 30fps game is 33.3333 ms/frame." << std::endl; 
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	int nbFramesLastSecond = 0;

	double previousFrame = glfwGetTime();

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

		user_inputs(currentTime - previousFrame);
		chunk_update();
		previousFrame = currentTime;
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLint newVaoCounter = 0;
		mtx_visible_chunks.lock();
		std::list<Chunk *>::iterator it = _visible_chunks.begin();
		for (; it != _visible_chunks.end(); it++) {
			(*it)->drawArray(newVaoCounter);
		}
		mtx_visible_chunks.unlock();

		glClear(GL_DEPTH_BUFFER_BIT);
		std::string str = (_debug_mode)
			? "FPS: " + std::to_string(nbFramesLastSecond) + "\nPos > x: " + std::to_string(camera._position.x)
				+ " y: " + std::to_string(camera._position.y) + " z: " + std::to_string(camera._position.z)
				+ "\nSpeed > " + std::to_string(camera._movement_speed)
				+ "\nChunk > x: " + std::to_string(_current_chunk.x) + " y: " + std::to_string(_current_chunk.y)
				+ "\nRender Distance > " + std::to_string(_render_distance)
				+ "\nGame mode > " + ((_game_mode) ? "SURVIVAL" : "CREATIVE")
				+ _inventory->getInventoryString()
			: "";
		_ui->drawUserInterface(str);
		glUseProgram(_shaderProgram);

		mtx_delete_chunks.lock();
		std::list<Chunk *>::iterator d_it = _delete_chunks.begin();
		for (; d_it != _delete_chunks.end(); d_it++) {
			delete *d_it;
		}
		_delete_chunks.clear();
		mtx_delete_chunks.unlock();
		

		glfwSwapBuffers(_window);
		glfwPollEvents();
	}

	check_glstate("\nmain loop successfully exited");
}
