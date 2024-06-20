#include "OpenGL_Manager.hpp"
#include "callbacks.hpp"
#include "Settings.hpp"
#include "WorldEdit.hpp"
// #include "Benchmark.hpp"
void thread_chunk_update( OpenGL_Manager *render );

OpenGL_Manager::OpenGL_Manager( void )
	: _player(std::make_unique<Player>()), _inventory(std::make_unique<Inventory>()),
	_window(NULL), _textures({NULL}), _fill(FILL), _debug_mode(true), _outline(true), _paused(true),
	_threadUpdate(false), _threadStop(false), _break_time(0), _eat_timer(0), _bow_timer(0),
	_game_mode(settings::consts::gamemode::creative), _break_frame(0), _world_name("default.json"),
	_block_hit({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 0, 0, 0}), _camera(std::make_unique<Camera>()),
	_ui(std::make_unique<UI>()), _menu(std::make_unique<Menu>()), _skybox(std::make_unique<Skybox>())
{
	std::cout << "Constructor of OpenGL_Manager called" << std::endl << std::endl;
	_inventory->setUIPtr(_ui.get());
	_camera->setTarget(_player.get());
	_ui->setPtrs(this, _inventory.get(), _player.get());
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
	_shader.deleteProgram();
	_skyShader.deleteProgram();
	_particleShader.deleteProgram();
	_modelShader.deleteProgram();

	_vaboEntities.deleteBuffers();
	_vaboParticles.deleteBuffers();
	_vaboModels.deleteBuffers();
	_ui->deleteBuffers();
	_menu->deleteBuffers();
	_skybox->deleteBuffers();

	set_cursor_position_callback(NULL, NULL);
	set_scroll_callback(NULL);
	_inventory->setUIPtr(NULL);
	_camera->setTarget(NULL);
	_ui->setPtrs(NULL, NULL, NULL);
	_menu->setPtrs(NULL, NULL);
	WorldEdit::Get()->setPtrs(NULL, NULL, NULL);

	mtx.lock();
	for (auto& c: _chunks) {
		c->setBackup(_backups);
		delete c;
	}
	// std::cout << "chunk size upon destruction " << _chunks.size() << std::endl;
	mtx.unlock();

	glfwMakeContextCurrent(NULL);
    glfwTerminate();

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
	if (_block_hit.type == blocks::air || (_block_hit.value & mask::blocks::notVisible)) {
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
	utils::shader::addQuads(_entities, {p4, p0, p6, p2}, spec, 16, 16, 0, 8);
	utils::shader::addQuads(_entities, {p1, p5, p3, p7}, spec, 16, 16, 0, 8);
	utils::shader::addQuads(_entities, {p0, p1, p2, p3}, spec, 16, 16, 0, 8);
	utils::shader::addQuads(_entities, {p5, p4, p7, p6}, spec, 16, 16, 0, 8);
	utils::shader::addQuads(_entities, {p4, p5, p0, p1}, spec, 16, 16, 0, 8);
	utils::shader::addQuads(_entities, {p2, p3, p6, p7}, spec, 16, 16, 0, 8);
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
		int eye = _player->getEyePos().z;
		eye &= 0xF8;
		for (int i = 0; i < 13; i += 4) {
			addLine(glm::vec3(_current_chunk.x,      _current_chunk.y,      eye + i), glm::vec3(_current_chunk.x + 16, _current_chunk.y,      eye + i));
			addLine(glm::vec3(_current_chunk.x + 16, _current_chunk.y,      eye + i), glm::vec3(_current_chunk.x + 16, _current_chunk.y + 16, eye + i));
			addLine(glm::vec3(_current_chunk.x + 16, _current_chunk.y + 16, eye + i), glm::vec3(_current_chunk.x,      _current_chunk.y + 16, eye + i));
			addLine(glm::vec3(_current_chunk.x,      _current_chunk.y + 16, eye + i), glm::vec3(_current_chunk.x,      _current_chunk.y,      eye + i));
		}
	}

	size_t bufSize = esize + ((hitBox) ? 24 : 0) + ((borders) ? 64 : 0);
	_vaboEntities.uploadData(bufSize, &(_entities[0].spec));

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

	_particleShader.useProgram();
	_vaboParticles.uploadData(psize, &(_particles[0].spec));

	check_glstate("OpenGL_Manager::drawParticles", false);

	glDrawArrays(GL_TRIANGLES, 0, psize);

	_particles.clear();
	_particles.reserve(psize);
}

void OpenGL_Manager::drawModels( void )
{
	size_t msize = _models.size();

	if (!msize) {
		return ;
	}

	_modelShader.useProgram();
	_vaboModels.uploadData(msize, &(_models[0].spec));

	check_glstate("OpenGL_Manager::drawModels", false);

	glDrawArrays(GL_TRIANGLES, 0, msize);

	_models.clear();
	_models.reserve(msize);
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
	// int width, height;
	// glfwGetWindowSize(_window, &width, &height);
	// std::cout << "window size is " << width << ", " << height << std::endl;

	// activate opengl context
	glfwMakeContextCurrent(_window);


	// glew is there to use the correct version for all functions
	glewExperimental = GL_TRUE;
	glewInit();

	_menu->setWindow(_window);
	Settings::Get()->loadResourcePacks();

	check_glstate("Window successfully created", true);
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

	// then setup the sky/water shader
	_skyShader.createProgram(Settings::Get()->getString(settings::strings::sky_vertex_shader), "",
							Settings::Get()->getString(settings::strings::sky_fragment_shader));

	_skyShader.bindFragData(settings::consts::shader::outColor, "outColor");
	_skyShader.bindAttribute(settings::consts::shader::attributes::position, "position");
	_skyShader.linkProgram();

	check_glstate("skyShader program successfully created", true);

	// then setup particles shader
	_particleShader.createProgram(Settings::Get()->getString(settings::strings::particle_vertex_shader), "",
								Settings::Get()->getString(settings::strings::particle_fragment_shader));

	_particleShader.bindFragData(settings::consts::shader::outColor, "outColor");
	_particleShader.bindAttribute(settings::consts::shader::attributes::specifications, "specifications");
	_particleShader.bindAttribute(settings::consts::shader::attributes::position, "position");
	_particleShader.linkProgram();

	check_glstate("particleShader program successfully created", true);

	// then setup model shader
	_modelShader.createProgram(Settings::Get()->getString(settings::strings::model_vertex_shader), "",
							Settings::Get()->getString(settings::strings::model_fragment_shader));

	_modelShader.bindFragData(settings::consts::shader::outColor, "outColor");
	_modelShader.bindAttribute(settings::consts::shader::attributes::specifications, "specifications");
	_modelShader.bindAttribute(settings::consts::shader::attributes::position, "position");
	_modelShader.linkProgram();

	check_glstate("modelShader program successfully created", true);

	// then setup the skybox shader
	_skybox->createShader();
	
	// then setup the main shader
	_shader.createProgram(Settings::Get()->getString(settings::strings::main_vertex_shader), "",
						Settings::Get()->getString(settings::strings::main_fragment_shader));

	_shader.bindFragData(settings::consts::shader::outColor, "outColor");
	_shader.bindAttribute(settings::consts::shader::attributes::specifications, "specifications");
	_shader.bindAttribute(settings::consts::shader::attributes::position, "position");
	_shader.linkProgram();

	check_glstate("Shader program successfully created", true);

	_vaboParticles.genBuffers();
	_vaboParticles.addAttribute(settings::consts::shader::attributes::specifications, 1, GL_INT);
	_vaboParticles.addAttribute(settings::consts::shader::attributes::position, 3, GL_FLOAT);
	_vaboEntities.genBuffers();
	_vaboEntities.addAttribute(settings::consts::shader::attributes::specifications, 1, GL_INT);
	_vaboEntities.addAttribute(settings::consts::shader::attributes::position, 3, GL_FLOAT);
	_vaboModels.genBuffers();
	_vaboModels.addAttribute(settings::consts::shader::attributes::specifications, 1, GL_INT);
	_vaboModels.addAttribute(settings::consts::shader::attributes::position, 3, GL_FLOAT);
	check_glstate("Gen and setup buffers", false);
}

void OpenGL_Manager::setupCommunicationShaders( void )
{
	_skybox->setupCommunicationShaders();

	_shader.setUniformLocation(settings::consts::shader::uniform::fog, "fogDist");
	glUniform1f(_shader.getUniform(settings::consts::shader::uniform::fog), (1 + Settings::Get()->getInt(settings::ints::render_dist)) << settings::consts::chunk_shift);
	_skyShader.setUniformLocation(settings::consts::shader::uniform::fog, "fogDist");
	glUniform1f(_skyShader.getUniform(settings::consts::shader::uniform::fog), (1 + Settings::Get()->getInt(settings::ints::render_dist)) << settings::consts::chunk_shift);

	_shader.setUniformLocation(settings::consts::shader::uniform::view, "view");
	_skyShader.setUniformLocation(settings::consts::shader::uniform::view, "view");
	_particleShader.setUniformLocation(settings::consts::shader::uniform::view, "view");
	_modelShader.setUniformLocation(settings::consts::shader::uniform::view, "view");
	updateCamView();

	_shader.setUniformLocation(settings::consts::shader::uniform::proj, "proj");
	_skyShader.setUniformLocation(settings::consts::shader::uniform::proj, "proj");
	_particleShader.setUniformLocation(settings::consts::shader::uniform::proj, "proj");
	_modelShader.setUniformLocation(settings::consts::shader::uniform::proj, "proj");
	updateCamPerspective();

	_shader.setUniformLocation(settings::consts::shader::uniform::internal_light, "internal_light");
	_particleShader.setUniformLocation(settings::consts::shader::uniform::internal_light, "internal_light");
	_modelShader.setUniformLocation(settings::consts::shader::uniform::internal_light, "internal_light");
	DayCycle::Get()->setShaderPtrs(&_shader, &_particleShader, &_modelShader);

	_modelShader.setUniformLocation(settings::consts::shader::uniform::brightness, "min_brightness");
	glUniform1f(_modelShader.getUniform(settings::consts::shader::uniform::brightness), Settings::Get()->getFloat(settings::floats::brightness));
	_particleShader.setUniformLocation(settings::consts::shader::uniform::brightness, "min_brightness");
	glUniform1f(_particleShader.getUniform(settings::consts::shader::uniform::brightness), Settings::Get()->getFloat(settings::floats::brightness));
	_shader.setUniformLocation(settings::consts::shader::uniform::brightness, "min_brightness");
	glUniform1f(_shader.getUniform(settings::consts::shader::uniform::brightness), Settings::Get()->getFloat(settings::floats::brightness));

	_skyShader.setUniformLocation(settings::consts::shader::uniform::color, "color");
	_skyShader.setUniformLocation(settings::consts::shader::uniform::animation, "animFrame");

	check_glstate("\nCommunication with shader program successfully established", true);
}

void OpenGL_Manager::loadTextures( void )
{
	_ui->loadTextures();

	if (_textures[0]) {
		glDeleteTextures(_textures.size(), &_textures[0]);
		_textures[0] = 0;
	}
	glGenTextures(_textures.size(), &_textures[0]);

	_shader.useProgram();
	loadTextureShader(0, _textures[0], Settings::Get()->getString(settings::strings::block_atlas));
	glUniform1i(glGetUniformLocation(_shader.getProgram(), "blockAtlas"), 0); // sampler2D #index in fragment shader

	_skyShader.useProgram();
	glUniform1i(glGetUniformLocation(_skyShader.getProgram(), "blockAtlas"), 0); // we reuse texture from main shader

	loadTextureShader(4, _textures[1], Settings::Get()->getString(settings::strings::water_still));
	glUniform1i(glGetUniformLocation(_skyShader.getProgram(), "waterStill"), 4);

	loadTextureShader(5, _textures[2], Settings::Get()->getString(settings::strings::water_flow));
	glUniform1i(glGetUniformLocation(_skyShader.getProgram(), "waterFlow"), 5);

	_particleShader.useProgram();
	glActiveTexture(GL_TEXTURE0 + 6);
	glBindTexture(GL_TEXTURE_2D_ARRAY, _textures[3]);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 256, 256, 2);
	loadSubTextureArray(256, 256, 0, Settings::Get()->getString(settings::strings::block_atlas));
	loadSubTextureArray(256, 256, 1, Settings::Get()->getString(settings::strings::particle_atlas));
	glUniform1i(glGetUniformLocation(_particleShader.getProgram(), "textures"), 6);
	check_glstate("Successfully loaded img[6] texture array 2D", true);

	_modelShader.useProgram();
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D_ARRAY, _textures[4]);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 64, 64, 3);
	loadSubTextureArray(64, 64, settings::consts::shader::texture::zombie, Settings::Get()->getString(settings::strings::tex_zombie));
	loadSubTextureArray(64, 64, settings::consts::shader::texture::skeleton, Settings::Get()->getString(settings::strings::tex_skeleton));
	loadSubTextureArray(64, 64, settings::consts::shader::texture::player, Settings::Get()->getString(settings::strings::tex_player));
	glUniform1i(glGetUniformLocation(_modelShader.getProgram(), "textures"), 3);
	check_glstate("Successfully loaded img[3] texture array 2D", true);
}

void OpenGL_Manager::setGamemode( int gamemode )
{
	if (gamemode < settings::consts::gamemode::survival || gamemode > settings::consts::gamemode::adventure) {
		return ;
	}
	_game_mode = gamemode;
	_player->resetFall();
	_ui->chatMessage("Gamemode set to " + settings::consts::gamemode::str[gamemode]);
}

void OpenGL_Manager::getGamemode( void )
{
	_ui->chatMessage("Current gamemode is " + settings::consts::gamemode::str[_game_mode]);
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
	glEnable(GL_DEPTH_TEST);

	// glLineWidth(2);
	glEnable(GL_LINE_SMOOTH); // anti-aliasing
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSwapInterval(1);
	glClearColor(0, 0, 0, 1.0f); // start of black, will be changed in game by DayCycle

	_menu->setState(menu::main);
	glfwSetCursorPosCallback(_window, cursor_position_callback);
	glfwSetScrollCallback(_window, scroll_callback);

	glfwSetKeyCallback(_window, inputs::key_callback);
	glfwSetMouseButtonCallback(_window, inputs::mouse_button_callback);

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
		bool gamePaused = _paused && _menu->getState() < menu::inventory && _menu->getState() != menu::death;

		++nbFrames;
		if (currentTime - lastTime >= 1.0) {
			nbFramesLastSecond = nbFrames;
			nbFrames = 0;
			nbTicksLastSecond = nbTicks;
			nbTicks = 0;
			lastTime += 1.0;
		}
		_shader.useProgram(); // must be before DayCycle tickUpdate
		if (currentTime - lastGameTick >= settings::consts::tick) {
			tickUpdate = true;
			++nbTicks;
			lastGameTick += settings::consts::tick;
			fluidUpdate = (nbTicks == 5 || nbTicks == 10 || nbTicks == 15 || nbTicks == 20);
			animUpdate = (nbTicks & 0x1);
			if (!gamePaused) {
				_player->tickUpdate();
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
		} else if (_player->getCamUpdate() && _menu->getState() >= menu::inventory) {
			// _ui->chatMessage("debug time");
			chunkUpdate();
			updateCamView();
			updateVisibleChunks();
			if (!_visible_chunks.size()) {
				_current_chunk.x += 32; // we want chunk_update to call thread on next loop
				_player->setCamUpdate(true);
			}
		}
		// b.stamp("user inputs");
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
				c->updateMobs(_models, deltaTime);
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
			(_camera->getCamPlacement() == CAMPLACEMENT::DEFAULT)
				? _player->drawHeldItem(_models, _particles, _hand_content, _game_mode)
				: _player->drawPlayer(_models, _particles, _hand_content);
			drawParticles();
			drawModels();
			_shader.useProgram();
			addBreakingAnim();
			drawEntities();
		}

		if (_menu->getState() >= menu::pause && Settings::Get()->getBool(settings::bools::skybox)) {
			_skybox->render(_camera->getCamPos());
		}

		#if 1
		_skyShader.useProgram();
		if (animUpdate) {
			updateAnimFrame();
		}
		glDisable(GL_CULL_FACE);
		DayCycle::Get()->setCloudsColor(_skyShader.getUniform(settings::consts::shader::uniform::color));
		if (Settings::Get()->getInt(settings::ints::clouds) != settings::OFF) {
			for (auto& c: _visible_chunks) {
				c->drawSky(newVaoCounter, skyFaces);
			}
		}
		glUniform3f(_skyShader.getUniform(settings::consts::shader::uniform::color), 0.24705882f, 0.4627451f, 0.89411765f); // water color
		for (auto&c: _visible_chunks) {
			c->drawWater(newVaoCounter, waterFaces);
		}
		if (Settings::Get()->getBool(settings::bools::face_culling)) {
			glEnable(GL_CULL_FACE);
		}
		// b.stamp("display water sky");
		#endif
		// Chunk *chunk_ptr = get_current_chunk_ptr();
		if (_menu->getState() >= menu::pause) {
			std::string str;
			if (_debug_mode) {
				str = "Timer: " + std::to_string(currentTime)
						+ '\n' + DayCycle::Get()->getInfos()
						+ "\nFPS: " + std::to_string(nbFramesLastSecond) + "\tframe " + std::to_string((deltaTime) * 1000)
						+ "\nTPS: " + std::to_string(nbTicksLastSecond)
						+ '\n' + _player->getString(_game_mode)
						+ "\nBlock\t> " + s_blocks[_block_hit.type]->name
						+ ((_block_hit.type != blocks::air) ? "\n\t\t> x: " + std::to_string(_block_hit.pos.x) + " y: " + std::to_string(_block_hit.pos.y) + " z: " + std::to_string(_block_hit.pos.z) : "\n")
						+ ((_block_hit.type) ? "\nprev\t> x: " + std::to_string(_block_hit.prev_pos.x) + " y: " + std::to_string(_block_hit.prev_pos.y) + " z: " + std::to_string(_block_hit.prev_pos.z) : "\nprev\t> none")
						+ ((_block_hit.water_value) ? "\n\t\tWATER on the way" : "\n\t\tno water")
						+ ((_game_mode != settings::consts::gamemode::creative) ? "\nBreak time\t> " + std::to_string(_break_time) + "\nBreak frame\t> " + std::to_string(_break_frame) : "\n\n")
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
						+ "\nGame mode\t\t> " + settings::consts::gamemode::str[_game_mode];
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
			if (_menu->getState() == menu::load) {
				mtx.lock();
				_menu->setChunks(_chunks);
				mtx.unlock();
			}
			switch (_menu->run(nbTicks == 1 && tickUpdate)) {
				case menu::ret::sign_done:
					_chunk_hit->setSignContent(_menu->getSignContent());
				case menu::ret::back_to_game: // back to game
					#if !__linux__
						glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
						if (glfwRawMouseMotionSupported()) {
							glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
						}
					#endif
					set_cursor_position_callback(_player.get(), NULL);
					set_scroll_callback(_inventory.get());
					_paused = false;
					backFromMenu = 0;
					_player->setCamUpdate(true);
					setThreadUpdate(true);
					_ui->textToScreen(_menu->getState() >= menu::pause);
					inputs::force_reset_key_update(GLFW_MOUSE_BUTTON_1);
					break ;
				case menu::ret::world_selected: // world selected, go into loading mode
					_world_name = _menu->getWorldFile();
					_shader.useProgram(); // used by dayCycle to modif internal light
					loadWorld("Worlds/" + _world_name);
					initWorld();
					break ;
				case menu::ret::world_created: // create new world, go into loading mode
					_world_name = _menu->getWorldFile();
					_player->setSpawnpoint({0, 0, 256});
					_player->respawn();
					_game_mode = settings::consts::gamemode::survival;
					DayCycle::Get()->setTicks(1000);
					initWorld();
					break ;
				case menu::ret::respawn_player: // Respawn player, init world again
					_player->respawn();
					initWorld();
					break ;
				case menu::ret::respawn_save_quit: // Respawn player, then save and quit to menu
					_player->respawn();
				case menu::ret::save_and_quit: // save and quit to menu
					resetInputsPtrs();
					saveWorld();
					break ;
				case menu::ret::fov_update: // fov change
					updateCamPerspective();
					break ;
				case menu::ret::render_dist_update: // render dist change
					// _ui->chatMessage("Render distance set to " + std::to_string(render_dist));
					glUniform1f(_skyShader.getUniform(settings::consts::shader::uniform::fog), (1 + Settings::Get()->getInt(settings::ints::render_dist)) << settings::consts::chunk_shift);
					glUniform1f(_shader.getUniform(settings::consts::shader::uniform::fog), (1 + Settings::Get()->getInt(settings::ints::render_dist)) << settings::consts::chunk_shift);
					setThreadUpdate(true);
					break ;
				case menu::ret::brightness_update: // brightness change
					glUniform1f(_particleShader.getUniform(settings::consts::shader::uniform::brightness), Settings::Get()->getFloat(settings::floats::brightness));
					glUniform1f(_shader.getUniform(settings::consts::shader::uniform::brightness), Settings::Get()->getFloat(settings::floats::brightness));
					break ;
				case menu::ret::apply_resource_packs:
					if (!Settings::Get()->loadResourcePacks()) { // if missing field in resource packs, we don't load
						createShaders();
						setupCommunicationShaders();
						loadTextures();
					}
					break ;
				default:
					break ;
			}
		} else {
			_ui->textToScreen(_menu->getState() >= menu::pause); // called from menu->run to draw in correct order
		}
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
