#include "OpenGL_Manager.hpp"
#include "callbacks.hpp"
#include "Settings.hpp"
#include "WorldEdit.hpp"
// #include "Benchmark.hpp"
void thread_chunk_update( OpenGL_Manager *render );

OpenGL_Manager::OpenGL_Manager( void )
	: _camera(std::make_unique<Camera>()), _inventory(std::make_unique<Inventory>()),
	_window(NULL), _shaderProgram(0), _skyShaderProgram(0), _particleShaderProgram(0),
	_textures({NULL}), _fill(FILL), _debug_mode(true), _outline(true), _paused(true),
	_threadUpdate(false), _threadStop(false), _break_time(0), _eat_timer(0), _bow_timer(0),
	_game_mode(GAMEMODE::CREATIVE), _break_frame(0), _world_name("default.json"),
	_block_hit({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 0, 0, 0}),
	_ui(std::make_unique<UI>()), _menu(std::make_unique<Menu>()), _skybox(std::make_unique<Skybox>())
{
	std::cout << "Constructor of OpenGL_Manager called" << std::endl << std::endl;
	_inventory->setUIPtr(_ui.get());
	_ui->setPtrs(this, _inventory.get(), _camera.get());
	_menu->setPtrs(_inventory.get(), _ui.get());
	WorldEdit::Get()->setPtrs(this, _inventory.get(), _ui->getChatPtr().get());

	startThread();
}

OpenGL_Manager::~OpenGL_Manager( void )
{
	std::cout << "Destructor of OpenGL_Manager called" << std::endl;

	stopThread();

	if (_textures[0]) {
		glDeleteTextures(_textures.size(), &_textures[0]);
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
	_inventory->setUIPtr(NULL);
	_ui->setPtrs(NULL, NULL, NULL);
	_menu->setPtrs(NULL, NULL);
	WorldEdit::Get()->setPtrs(NULL, NULL, NULL);

	glfwMakeContextCurrent(NULL);
    glfwTerminate();

	mtx.lock();
	for (auto& c: _chunks) {
		c->setBackup(_backups);
		delete c;
	}
	// std::cout << "chunk size upon destruction " << _chunks.size() << std::endl;
	mtx.unlock();
	mtx_backup.lock();
	for (auto &b : _backups) {
		for (auto ch : b.second.chests) {
			delete ch.second;
		}
		for (auto fur : b.second.furnaces) {
			delete fur.second;
		}
		for (auto s : b.second.signs) {
			delete s.second;
		}
	}
	mtx_backup.unlock();

	DayCycle::Destroy();
	Settings::Destroy();
	WorldEdit::Destroy();
	check_glstate("openGL_Manager destructed", true);
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void OpenGL_Manager::addBreakingAnim( void )
{
	if (_block_hit.type == blocks::air) {
		return ;
	}

	glm::vec3 p0 = {_block_hit.pos.x - 0.001f, _block_hit.pos.y - 0.001f, _block_hit.pos.z + 1.001f};
	glm::vec3 p1 = {_block_hit.pos.x + 1.001f, _block_hit.pos.y - 0.001f, _block_hit.pos.z + 1.001f};
	glm::vec3 p2 = {_block_hit.pos.x - 0.001f, _block_hit.pos.y - 0.001f, _block_hit.pos.z - 0.001f};
	glm::vec3 p3 = {_block_hit.pos.x + 1.001f, _block_hit.pos.y - 0.001f, _block_hit.pos.z - 0.001f};

	glm::vec3 p4 = {_block_hit.pos.x - 0.001f, _block_hit.pos.y + 1.001f, _block_hit.pos.z + 1.001f};
	glm::vec3 p5 = {_block_hit.pos.x + 1.001f, _block_hit.pos.y + 1.001f, _block_hit.pos.z + 1.001f};
	glm::vec3 p6 = {_block_hit.pos.x - 0.001f, _block_hit.pos.y + 1.001f, _block_hit.pos.z - 0.001f};
	glm::vec3 p7 = {_block_hit.pos.x + 1.001f, _block_hit.pos.y + 1.001f, _block_hit.pos.z - 0.001f};
	// if (s_blocks[_block_hit.type]->hasHitbox) {
	// 	glm::vec3 hitCenter = s_blocks[_block_hit.type]->hitboxCenter, hitHalfSize = s_blocks[_block_hit.type]->hitboxHalfSize;
	// 	p0 += glm::vec3(     hitCenter.x - hitHalfSize.x,      hitCenter.y - hitHalfSize.y, -1 + hitCenter.z + hitHalfSize.z);
	// 	p1 += glm::vec3(-1 + hitCenter.x + hitHalfSize.x,      hitCenter.y - hitHalfSize.y, -1 + hitCenter.z + hitHalfSize.z);
	// 	p2 += glm::vec3(     hitCenter.x - hitHalfSize.x,      hitCenter.y - hitHalfSize.y,      hitCenter.z - hitHalfSize.z);
	// 	p3 += glm::vec3(-1 + hitCenter.x + hitHalfSize.x,      hitCenter.y - hitHalfSize.y,      hitCenter.z - hitHalfSize.z);

	// 	p4 += glm::vec3(     hitCenter.x - hitHalfSize.x, -1 + hitCenter.y + hitHalfSize.y, -1 + hitCenter.z + hitHalfSize.z);
	// 	p5 += glm::vec3(-1 + hitCenter.x + hitHalfSize.x, -1 + hitCenter.y + hitHalfSize.y, -1 + hitCenter.z + hitHalfSize.z);
	// 	p6 += glm::vec3(     hitCenter.x - hitHalfSize.x, -1 + hitCenter.y + hitHalfSize.y,      hitCenter.z - hitHalfSize.z);
	// 	p7 += glm::vec3(-1 + hitCenter.x + hitHalfSize.x, -1 + hitCenter.y + hitHalfSize.y,      hitCenter.z - hitHalfSize.z);
	// }

	int spec = (14 << 4) + ((_block_hit.type == blocks::glass && _break_frame == 1) ? 0 : (_break_frame << 12));
	t_shaderInput v0 = {spec, p4};
	t_shaderInput v1 = {spec + XTEX, p0};
	t_shaderInput v2 = {spec + YTEX, p6};
	t_shaderInput v3 = {spec + XTEX + YTEX, p2};
	face_vertices(_entities, v0, v1, v2, v3);
	v0 = {spec, p1};
	v1 = {spec + XTEX, p5};
	v2 = {spec + YTEX, p3};
	v3 = {spec + XTEX + YTEX, p7};
	face_vertices(_entities, v0, v1, v2, v3);
	v0 = {spec, p0};
	v1 = {spec + XTEX, p1};
	v2 = {spec + YTEX, p2};
	v3 = {spec + XTEX + YTEX, p3};
	face_vertices(_entities, v0, v1, v2, v3);
	v0 = {spec, p5};
	v1 = {spec + XTEX, p4};
	v2 = {spec + YTEX, p7};
	v3 = {spec + XTEX + YTEX, p6};
	face_vertices(_entities, v0, v1, v2, v3);
	v0 = {spec, p4};
	v1 = {spec + XTEX, p5};
	v2 = {spec + YTEX, p0};
	v3 = {spec + XTEX + YTEX, p1};
	face_vertices(_entities, v0, v1, v2, v3);
	v0 = {spec, p2};
	v1 = {spec + XTEX, p3};
	v2 = {spec + YTEX, p6};
	v3 = {spec + XTEX + YTEX, p7};
	face_vertices(_entities, v0, v1, v2, v3);
}

void OpenGL_Manager::addLine( glm::vec3 a, glm::vec3 b )
{
	// _entities.push_back({(4 << 4) + 1, a}); // black line
	// _entities.push_back({(4 << 4) + 2 + (1 << 17) + (1 << 8) + (1 << 18), b});
	int spec = (6 << 12) + (0xFF << 24);
	_entities.push_back({spec, a}); // red line
	_entities.push_back({spec + 1 + (1 << 17) + (1 << 8) + (1 << 18), b});
}

void OpenGL_Manager::drawEntities( void )
{
	// TODO update hand entity
	// _hand->update(deltaTime);
	size_t esize = _entities.size();

	bool hitBox = false;/*/(_block_hit.type != blocks::air) && (_block_hit.type != blocks::chest);
	if (!hitBox) {
	} else if (s_blocks[_block_hit.type]->hasHitbox) {
		glm::vec3 hitCenter = s_blocks[_block_hit.type]->hitboxCenter, hitHalfSize = s_blocks[_block_hit.type]->hitboxHalfSize;
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
	}//*/

	bool borders = false;
	if (Settings::Get()->getBool(settings::bools::visible_chunk_border)) {
		borders = true;
		for (int i = 0; i < 13; i += 4) {
			addLine(glm::vec3(_current_chunk.x + i,  _current_chunk.y,      0), glm::vec3(_current_chunk.x + i,  _current_chunk.y,      256));
			addLine(glm::vec3(_current_chunk.x + 16, _current_chunk.y + i,  0), glm::vec3(_current_chunk.x + 16, _current_chunk.y + i,  256));
			addLine(glm::vec3(_current_chunk.x + i,  _current_chunk.y + 16, 0), glm::vec3(_current_chunk.x + i,  _current_chunk.y + 16, 256));
			addLine(glm::vec3(_current_chunk.x,      _current_chunk.y + i,  0), glm::vec3(_current_chunk.x,      _current_chunk.y + i, 256));
		}
		int eye = _camera->getEyePos().z;
		eye &= 0xF8;
		for (int i = 0; i < 13; i += 4) {
			addLine(glm::vec3(_current_chunk.x,      _current_chunk.y,      eye + i), glm::vec3(_current_chunk.x + 16, _current_chunk.y,      eye + i));
			addLine(glm::vec3(_current_chunk.x + 16, _current_chunk.y,      eye + i), glm::vec3(_current_chunk.x + 16, _current_chunk.y + 16, eye + i));
			addLine(glm::vec3(_current_chunk.x + 16, _current_chunk.y + 16, eye + i), glm::vec3(_current_chunk.x,      _current_chunk.y + 16, eye + i));
			addLine(glm::vec3(_current_chunk.x,      _current_chunk.y + 16, eye + i), glm::vec3(_current_chunk.x,      _current_chunk.y,      eye + i));
		}
	}

	glBindVertexArray(_vaoEntities);

	glBindBuffer(GL_ARRAY_BUFFER, _vboEntities);
	size_t bufSize = esize + ((hitBox) ? 24 : 0) + ((borders) ? 64 : 0);
	glBufferData(GL_ARRAY_BUFFER, bufSize * 4 * sizeof(GLint), &(_entities[0].spec), GL_STATIC_DRAW);

	glEnableVertexAttribArray(SPECATTRIB);
	glVertexAttribIPointer(SPECATTRIB, 1, GL_INT, 4 * sizeof(GLint), 0);

	glEnableVertexAttribArray(POSATTRIB);
	glVertexAttribPointer(POSATTRIB, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(1 * sizeof(GLint)));

	check_glstate("OpenGL_Manager::drawEntities", false);

	if (hitBox) {
		glDrawArrays(GL_LINES, esize, 24);
	}
	glDrawArrays(GL_TRIANGLES, 0, esize);
	if (borders) {
		glDrawArrays(GL_LINES, esize + ((hitBox) ? 24 : 0), 64);
	}

	_entities.clear();
	_entities.reserve(esize);
}

void OpenGL_Manager::drawParticles( void )
{
	size_t psize = _particles.size();

	if (!psize) {
		return ;
	}

	glUseProgram(_particleShaderProgram);
	glBindVertexArray(_vaoParticles);

	glBindBuffer(GL_ARRAY_BUFFER, _vboParticles);
	glBufferData(GL_ARRAY_BUFFER, psize * 4 * sizeof(GLint), &(_particles[0].spec), GL_STATIC_DRAW);

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

void OpenGL_Manager::setupWindow( void )
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

	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);
	GLFWmonitor *monitor = (count > 1) ? monitors[1] : monitors[0];
	// std::cout << count << " monitors detected, sizes are:" << std::endl;
	// for (int i = 0; i < count; ++i) {
	// 	const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
	// 	std::cout << "\t" << i << " -> " << mode->width << ", " << mode->height << std::endl;
	// }

	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	WIN_WIDTH = mode->width;
	WIN_HEIGHT = mode->height;
	// std::cout << "win size is set to " << WIN_WIDTH << ", " << WIN_HEIGHT << ", refresh rate is " << mode->refreshRate << std::endl;

	// (Settings::Get()->getBool(settings::bools::fullscreen))
	// 	? _window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "MineGraphed", glfwGetPrimaryMonitor(), nullptr)
	// 	: _window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "MineGraphed", nullptr, nullptr);
	_window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "MineGraphed", monitor, nullptr);
	if (_window == NULL) {
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

	Settings::Get()->loadResourcePacks();

	check_glstate("Window successfully created", true);

	glGenVertexArrays(1, &_vaoEntities);
	glGenBuffers(1, &_vboEntities);
	check_glstate("VAO and VBO for entities", false);
	glGenVertexArrays(1, &_vaoParticles);
	glGenBuffers(1, &_vboParticles);
	check_glstate("VAO and VBO for particles", false);
}

void OpenGL_Manager::initWorld( void )
{
	chunkUpdate();
	setThreadUpdate(true);
}

void OpenGL_Manager::createShaders( void )
{
	// first setup the ui and text shaders
	_ui->setupShader();
	_menu->setShaderProgram(_ui->getShaderProgram());

	// then setup the sky/water shader
	if (_skyShaderProgram) {
		glDeleteProgram(_skyShaderProgram);
	}
	_skyShaderProgram = createShaderProgram(Settings::Get()->getString(settings::strings::sky_vertex_shader), "",
										Settings::Get()->getString(settings::strings::sky_fragment_shader));

	glBindFragDataLocation(_skyShaderProgram, 0, "outColor");

	glBindAttribLocation(_skyShaderProgram, 0, "position");

	glLinkProgram(_skyShaderProgram);
	glUseProgram(_skyShaderProgram);

	check_glstate("skyShader program successfully created", true);

	// then setup particles shader
	if (_particleShaderProgram) {
		glDeleteProgram(_particleShaderProgram);
	}
	_particleShaderProgram = createShaderProgram(Settings::Get()->getString(settings::strings::particle_vertex_shader), "",
										Settings::Get()->getString(settings::strings::particle_fragment_shader));

	glBindFragDataLocation(_particleShaderProgram, 0, "outColor");

	glBindAttribLocation(_particleShaderProgram, SPECATTRIB, "specifications");
	glBindAttribLocation(_particleShaderProgram, POSATTRIB, "position");

	glLinkProgram(_particleShaderProgram);
	glUseProgram(_particleShaderProgram);

	check_glstate("particleShader program successfully created", true);

	// then setup the skybox shader
	_skybox->createShader();
	
	// then setup the main shader
	if (_shaderProgram) {
		glDeleteProgram(_shaderProgram);
	}
	_shaderProgram = createShaderProgram(Settings::Get()->getString(settings::strings::main_vertex_shader), "",
										Settings::Get()->getString(settings::strings::main_fragment_shader));

	glBindFragDataLocation(_shaderProgram, 0, "outColor");

	glBindAttribLocation(_shaderProgram, SPECATTRIB, "specifications");
	glBindAttribLocation(_shaderProgram, POSATTRIB, "position");

	glLinkProgram(_shaderProgram);
	glUseProgram(_shaderProgram);

	check_glstate("Shader program successfully created", true);
}

void OpenGL_Manager::setupCommunicationShaders( void )
{
	_skybox->setupCommunicationShaders();

	glUseProgram(_shaderProgram);
	_uniFog = glGetUniformLocation(_shaderProgram, "fogDist");
	glUniform1f(_uniFog, (1 + Settings::Get()->getInt(settings::ints::render_dist)) << CHUNK_SHIFT);
	_skyUniFog = glGetUniformLocation(_skyShaderProgram, "fogDist");
	glUseProgram(_skyShaderProgram);
	glUniform1f(_skyUniFog, (1 + Settings::Get()->getInt(settings::ints::render_dist)) << CHUNK_SHIFT);
	glUseProgram(_shaderProgram);

	_uniView = glGetUniformLocation(_shaderProgram, "view");
	_skyUniView = glGetUniformLocation(_skyShaderProgram, "view");
	_partUniView = glGetUniformLocation(_particleShaderProgram, "view");
	updateCamView();

	_uniProj = glGetUniformLocation(_shaderProgram, "proj");
	_skyUniProj = glGetUniformLocation(_skyShaderProgram, "proj");
	_partUniProj = glGetUniformLocation(_particleShaderProgram, "proj");
	updateCamPerspective();

	DayCycle::Get()->setUniInternalLight(_shaderProgram, _particleShaderProgram,
		glGetUniformLocation(_shaderProgram, "internal_light"), glGetUniformLocation(_particleShaderProgram, "internal_light"));

	_uniBrightness = glGetUniformLocation(_shaderProgram, "min_brightness");
	_partUniBrightness = glGetUniformLocation(_particleShaderProgram, "min_brightness");
	glUseProgram(_particleShaderProgram);
	glUniform1f(_partUniBrightness, Settings::Get()->getFloat(settings::floats::brightness));
	glUseProgram(_shaderProgram);
	glUniform1f(_uniBrightness, Settings::Get()->getFloat(settings::floats::brightness));

	_skyUniColor = glGetUniformLocation(_skyShaderProgram, "color");
	_skyUniAnim = glGetUniformLocation(_skyShaderProgram, "animFrame");

	check_glstate("\nCommunication with shader program successfully established", true);
}

void OpenGL_Manager::loadTextures( void )
{
	_ui->loadTextures();

	if (_textures[0]) {
		glDeleteTextures(_textures.size(), &_textures[0]);
		_textures[0] = 0;
	}
	glGenTextures(4, &_textures[0]);

	glUseProgram(_shaderProgram);
	loadTextureShader(0, _textures[0], Settings::Get()->getString(settings::strings::block_atlas));
	glUniform1i(glGetUniformLocation(_shaderProgram, "blockAtlas"), 0); // sampler2D #index in fragment shader

	glUseProgram(_skyShaderProgram);
	glUniform1i(glGetUniformLocation(_skyShaderProgram, "blockAtlas"), 0); // we reuse texture from main shader

	loadTextureShader(4, _textures[1], Settings::Get()->getString(settings::strings::water_still));
	glUniform1i(glGetUniformLocation(_skyShaderProgram, "waterStill"), 4);

	loadTextureShader(5, _textures[2], Settings::Get()->getString(settings::strings::water_flow));
	glUniform1i(glGetUniformLocation(_skyShaderProgram, "waterFlow"), 5);

	glUseProgram(_particleShaderProgram);
	glActiveTexture(GL_TEXTURE0 + 6);
	glBindTexture(GL_TEXTURE_2D_ARRAY, _textures[3]);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 256, 256, 3);
	loadSubTextureArray(0, Settings::Get()->getString(settings::strings::block_atlas));
	loadSubTextureArray(1, Settings::Get()->getString(settings::strings::particle_atlas));
	loadSubTextureArray(2, Settings::Get()->getString(settings::strings::model_atlas));
	glUniform1i(glGetUniformLocation(_particleShaderProgram, "textures"), 6);
}

void OpenGL_Manager::setGamemode( int gamemode )
{
	if (gamemode < GAMEMODE::SURVIVAL || gamemode > GAMEMODE::ADVENTURE) {
		return ;
	}
	_game_mode = gamemode;
	_camera->resetFall();
	_ui->chatMessage("Gamemode set to " + GAMEMODE::str[gamemode]);
}

void OpenGL_Manager::getGamemode( void )
{
	_ui->chatMessage("Current gamemode is " + GAMEMODE::str[_game_mode]);
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
	if (Settings::Get()->getBool(settings::bools::face_culling)) {
		glEnable(GL_CULL_FACE);
	}
	glCullFace(GL_FRONT);

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

	bool fluidUpdate = false, animUpdate = false, tickUpdate = false, redTickUpdate = false;

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
				redTickUpdate = DayCycle::Get()->tickUpdate();
			}
		} else {
			tickUpdate = false;
			redTickUpdate = false;
			fluidUpdate = false;
			animUpdate = false;
		}

		// Bench b;
		if (!_paused) {
			if (++backFromMenu != 1) {
				userInputs(deltaTime, ++backFromMenu > 3);
			}
			chunkUpdate();
		} else if (_camera->_update && _menu->getState() >= MENU::INVENTORY) {
			// _ui->chatMessage("debug time");
			chunkUpdate();
			updateCamView();
			updateVisibleChunks();
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
				if (tickUpdate) {
					if (redTickUpdate) {
						c->updateRedstone(); // scheduled ticks
					}
					c->updateFurnaces(currentTime);
					if (fluidUpdate) {
						c->updateScheduledBlocks();
						c->updateFluids(); // fluid tick
					}
					c->tickUpdate(); // random ticks
				}
				c->updateEntities(_entities, _particles, deltaTime);
				if (Settings::Get()->getBool(settings::bools::particles)) {
					c->updateParticles(_particles, deltaTime);
				}
			}
		}
		// if (newVaoCounter) {
		// 	std::cout << "new vao counter: " << newVaoCounter << std::endl;
		// }
		// b.stamp("solids");

		if (!gamePaused) {
			_camera->drawPlayer(_particles, _hand_content, _game_mode);
			drawParticles();
			glUseProgram(_shaderProgram);
			addBreakingAnim();
			drawEntities();
		}

		if (_menu->getState() >= MENU::PAUSE && Settings::Get()->getBool(settings::bools::skybox)) {
			_skybox->render(_camera->getCamPos());
		}

		#if 1
		glUseProgram(_skyShaderProgram);
		if (animUpdate) {
			updateAnimFrame();
		}
		glDisable(GL_CULL_FACE);
		DayCycle::Get()->setCloudsColor(_skyUniColor);
		if (Settings::Get()->getInt(settings::ints::clouds) != settings::OFF) {
			for (auto& c: _visible_chunks) {
				c->drawSky(newVaoCounter, skyFaces);
			}
		}
		glUniform3f(_skyUniColor, 0.24705882f, 0.4627451f, 0.89411765f); // water color
		for (auto&c: _visible_chunks) {
			c->drawWater(newVaoCounter, waterFaces);
		}
		if (Settings::Get()->getBool(settings::bools::face_culling)) {
			glEnable(GL_CULL_FACE);
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
						+ "\nBlock\t> " + s_blocks[_block_hit.type]->name
						+ ((_block_hit.type != blocks::air) ? "\n\t\t> x: " + std::to_string(_block_hit.pos.x) + " y: " + std::to_string(_block_hit.pos.y) + " z: " + std::to_string(_block_hit.pos.z) : "\n")
						+ ((_block_hit.type) ? "\nprev\t> x: " + std::to_string(_block_hit.prev_pos.x) + " y: " + std::to_string(_block_hit.prev_pos.y) + " z: " + std::to_string(_block_hit.prev_pos.z) : "\nprev\t> none")
						+ ((_block_hit.water_value) ? "\n\t\tWATER on the way" : "\n\t\tno water")
						+ ((_game_mode != GAMEMODE::CREATIVE) ? "\nBreak time\t> " + std::to_string(_break_time) + "\nBreak frame\t> " + std::to_string(_break_frame) : "\n\n")
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
						+ "\n\nRender Distance\t> " + std::to_string(Settings::Get()->getInt(settings::ints::render_dist))
						+ "\nGame mode\t\t> " + GAMEMODE::str[_game_mode];
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
			switch (_menu->run(nbTicks == 1 && tickUpdate)) {
				case MENU::RET::SIGN_DONE:
					_chunk_hit->setSignContent(_menu->getSignContent());
				case MENU::RET::BACK_TO_GAME: // back to game
					#if !__linux__
						glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
						if (glfwRawMouseMotionSupported()) {
							glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
						}
					#endif
					set_cursor_position_callback(_camera.get(), NULL);
					set_scroll_callback(_inventory.get());
					_paused = false;
					backFromMenu = 0;
					_camera->_update = true;
					setThreadUpdate(true);
					break ;
				case MENU::RET::WORLD_SELECTED: // world selected, go into loading mode
					_world_name = _menu->getWorldFile();
					glUseProgram(_shaderProgram); // used by dayCycle to modif internal light
					loadWorld("Worlds/" + _world_name);
					initWorld();
					break ;
				case MENU::RET::WORLD_CREATED: // create new world, go into loading mode
					_world_name = _menu->getWorldFile();
					_camera->setSpawnpoint({0, 0, 256});
					_camera->respawn();
					_game_mode = GAMEMODE::SURVIVAL;
					DayCycle::Get()->setTicks(1000);
					initWorld();
					break ;
				case MENU::RET::RESPAWN_PLAYER: // Respawn player, init world again
					_camera->respawn();
					initWorld();
					break ;
				case MENU::RET::RESPAWN_SAVE_QUIT: // Respawn player, then save and quit to menu
					_camera->respawn();
				case MENU::RET::SAVE_AND_QUIT: // save and quit to menu
					resetInputsPtrs();
					saveWorld();
					break ;
				case MENU::RET::FOV_UPDATE: // fov change
					updateCamPerspective();
					break ;
				case MENU::RET::RENDER_DIST_UPDATE: // render dist change
					// _ui->chatMessage("Render distance set to " + std::to_string(render_dist));
					glUseProgram(_skyShaderProgram);
					glUniform1f(_skyUniFog, (1 + Settings::Get()->getInt(settings::ints::render_dist)) << CHUNK_SHIFT);
					glUseProgram(_shaderProgram);
					glUniform1f(_uniFog, (1 + Settings::Get()->getInt(settings::ints::render_dist)) << CHUNK_SHIFT);
					setThreadUpdate(true);
					break ;
				case MENU::RET::BRIGHTNESS_UPDATE: // brightness change
					glUseProgram(_particleShaderProgram);
					glUniform1f(_partUniBrightness, Settings::Get()->getFloat(settings::floats::brightness));
					glUseProgram(_shaderProgram);
					glUniform1f(_uniBrightness, Settings::Get()->getFloat(settings::floats::brightness));
					break ;
				case MENU::RET::APPLY_RESOURCE_PACKS:
					if (!Settings::Get()->loadResourcePacks()) { // if missing field in resource packs, we don't load
						createShaders();
						setupCommunicationShaders();
						loadTextures();
					}
					break ;
				default:
					break ;
			}
		}
		_ui->textToScreen(_menu->getState() >= MENU::PAUSE);
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
