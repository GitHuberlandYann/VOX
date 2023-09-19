# include "vox.h"

OpenGL_Manager::OpenGL_Manager( void )
	: _window(NULL), _textures(NULL), _background_color(0.0f, 0.0f, 0.0f),
		_key_rdist(0), _render_distance(RENDER_DISTANCE),
		_key_fill(0), _fill(FILL)//, _key_add_block(0)
{
	std::cout << "Constructor of OpenGL_Manager called" << std::endl << std::endl;
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

	glDeleteTextures(1, _textures);
	glDeleteProgram(_shaderProgram);

	glfwMakeContextCurrent(NULL);
    glfwTerminate();

	delete [] _textures;

	mtx_visible_chunks.lock();
	_visible_chunks.clear();
	mtx_visible_chunks.unlock();
	mtx.lock();
	std::list<Chunk *>::iterator it = _chunks.begin();
	for (; it != _chunks.end(); it++) {
		delete *it;
	}
	// std::cout << "chunk size upon destruction " << _chunks.size() << std::endl;
	_chunks.clear();
	mtx.unlock();
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void OpenGL_Manager::compile_shader( GLuint ptrShader, std::string name )
{
	glCompileShader(ptrShader);

    GLint status;
    glGetShaderiv(ptrShader, GL_COMPILE_STATUS, &status);
	if (status) {
		std::cout << name << " shader compiled successfully" << std::endl;
	} else {
		char buffer[512];
		glGetShaderInfoLog(ptrShader, 512, NULL, buffer);

		std::cerr << name << " shader did not compile, error log:" << std::endl << buffer << std::endl;
		exit(1);
	}
}

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

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	_window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "MineThemeGraphed", nullptr, nullptr);
	if (_window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit (1);
    }

	// activate opengl context
	glfwMakeContextCurrent(_window);

	// glew is there to use the correct version for all functions
	glewExperimental = GL_TRUE;
	glewInit();
	
	glEnable(GL_DEPTH_TEST);
	glPointSize(10);
	glfwSwapInterval(1);

	check_glstate("Window successfully created");
}

void OpenGL_Manager::initWorld( void )
{
	_current_chunk[0] = -1;
	_current_chunk[1] = -1;
	chunk_update(); //create chunks
	_current_chunk[0] = -1;
	_current_chunk[1] = -1;
	chunk_update(); //display created chunks
}

void OpenGL_Manager::create_shaders( void )
{
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

	check_glstate("Communication with shader program successfully established");
}

void OpenGL_Manager::load_texture( std::string texture_file )
{
	_textures = new GLuint[1];
	glGenTextures(1, _textures);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _textures[0]);

	// load image
	t_tex *texture = new t_tex;
	texture->content = SOIL_load_image(texture_file.c_str(), &texture->width, &texture->height, 0, SOIL_LOAD_RGB);
	if (!texture->content) {
		std::cerr << "failed to load image " << texture_file << " because:" << std::endl << SOIL_last_result() << std::endl;
		exit(1);
	}

	// load image as texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height,
		0, GL_RGB, GL_UNSIGNED_BYTE, texture->content);

	// std::string tex_str = "tex" + std::to_string(index);
	glUniform1i(glGetUniformLocation(_shaderProgram, "tex0"), 0); // sampler2D #index in fragment shader
			
	// set settings for texture wraping and size modif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (texture) {
		SOIL_free_image_data(texture->content);
	}
	delete texture;

	check_glstate("Succesfully loaded texture to shader");
}

void OpenGL_Manager::main_loop( void )
{
	check_glstate("setup done, entering main loop\n");

	// std::cout << "number of vertices: " << _number_vertices << std::endl << std::endl;
	
	glClearColor(_background_color.x, _background_color.y, _background_color.z, 1.0f);
	// glfwSetCursorPosCallback(_window, cursor_position_callback);
    // glfwSetScrollCallback(_window, scroll_callback);
    // glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// std::cout << "60fps game is 16.6666 ms/frame; 30fps game is 33.3333 ms/frame." << std::endl; 
	double lastTime = glfwGetTime();
	int nbFrames = 0;

	double previousFrame = glfwGetTime();

	// main loop cheking for inputs and rendering everything
	while (!glfwWindowShouldClose(_window))
	{
		double currentTime = glfwGetTime();
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 ){
			std::cout << 1000.0/double(nbFrames) << " ms/frame; " << nbFrames << " fps" << std::endl;
			// std::cout << "other math gives " << (currentTime - previousFrame) * 1000 << "ms/frame" << std::endl;
			nbFrames = 0;
			lastTime += 1.0;
		}

		user_inputs(currentTime - previousFrame);
		chunk_update(); //may want to call this once player goes onto new chunk to improve performance
		previousFrame = currentTime;
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLint newVaoCounter = 0;
		mtx_visible_chunks.lock();
		std::list<Chunk *>::iterator it = _visible_chunks.begin();
		for (; it != _visible_chunks.end();) {
			(*it)->drawArray(newVaoCounter);
			it++;
		}
		mtx_visible_chunks.unlock();

		// if (newVaoCounter) {
		// 	std::cout << "computed " << newVaoCounter << " new vao this frame" << std::endl;
		// }

		glfwSwapBuffers(_window);
		glfwPollEvents();
	}

	check_glstate("\nmain loop successfully exited");
}
