#include "OpenGL_Manager.hpp"
#include "callbacks.hpp"
#include "logs.hpp"

/**
 * @brief send packet stored at this->_packet of given size to this->_socket
 */
void OpenGL_Manager::sendPacket( size_t size )
{
	int sendRet = _socket->send(_socket->getServerAddress(), &_packet, size);
	if (sendRet >= send_ret::timeout) {
		_player = nullptr;
		_socket = nullptr;
		_paused = true;
		_menu->setState(menu::error);
		_menu->setErrorStr({"Disconnected from server", "Timeout"});
	}
}

void OpenGL_Manager::handlePacketLogin( void )
{
	if (_player) {
		return ;
	}
	_player = std::make_unique<Player>();
	_camera->setTarget(_player.get());
	_player->setCamUpdate(true);
	_player->setPos({.0f, .0f, 11.f});
		// "yaw": 61.690426,
		// "pitch": -25.588861,
	_menu->setErrorStr({"Succesfully connected to server."});
	_menu->setState(menu::pause);
	// _paused = false;
}

void OpenGL_Manager::handlePacketPing( char* data )
{
	double serverCurrentTime;
	memmove(&serverCurrentTime, data, sizeof(double));
	int server_fps, server_tps;
	memmove(&server_fps, &data[sizeof(double)], sizeof(int));
	memmove(&server_tps, &data[sizeof(double) + sizeof(int)], sizeof(int));
	MAINLOG(LOG("Client::handlePackets: time received: " << serverCurrentTime << " server fps: " << server_fps << ", server tps: " << server_tps));
	_packet.action = packet_id::client::pong;
	sendPacket(sizeof(unsigned short));
}

void OpenGL_Manager::handlePacketKick( std::string msg )
{
	_player = nullptr;
	_socket = nullptr;
	_paused = true;
	_menu->setState(menu::error);
	_menu->setErrorStr({"You have been kicked by the server.", "Server msg: " + msg});
}

void OpenGL_Manager::handlePacketChunk( t_packet_data& packet )
{
	_mtx_packets.lock();
	_chunkPackets.push_back(packet);
	_mtx_packets.unlock();
	_threadUpdate = true;
	// size_t srcOffset = 0;
	// int startX = 0, startY = 0;
	// int index = 0;
	// utils::memory::memread(&startX, packet.data, sizeof(GLint), srcOffset);
	// utils::memory::memread(&startY, packet.data, sizeof(GLint), srcOffset);
	// utils::memory::memread(&index, packet.data, sizeof(char), srcOffset);
	// LOG("\tchunk_data packet [" << startX << ", " << startY << "] index " << index);
}

void OpenGL_Manager::handlePacketChatMsg( char* data )
{
	MAINLOG(LOG("chatMessage: |" << data << "|"));
}

void OpenGL_Manager::handlePackets( void )
{
	Address sender;
	while (true) { // read incoming packets
		ssize_t bytes_read = _socket->receive(sender, &_packet, sizeof(_packet));
		
		if (bytes_read <= 0) {
			break;
		}
		PACKETLOG(LOG("received " << bytes_read << " bytes"));
		LOG("Packet received with action: " << _packet.action << " [" << bytes_read << " bytes]");

		// process packet
		switch (_packet.action & mask::network::action_type) {
			case packet_id::server::login: // server confirmed login
				handlePacketLogin();
				break ;
			case packet_id::server::ping:
				handlePacketPing(_packet.data);
				break ;
			case packet_id::server::kick:
				return (handlePacketKick(_packet.data));
			case packet_id::server::chunk_data:
				handlePacketChunk(_packet);
				break ;
			case packet_id::server::player_info:
				break ;
			case packet_id::server::chat_msg:
				handlePacketChatMsg(_packet.data);
				break ;
			default:
				MAINLOG(LOGERROR("Client::handlePackets: Unrecognised packet action: " << _packet.action << " [" << bytes_read << " bytes]" << ", sent with data: |" << _packet.data << "|"));
				break ;
		}
	}

	if (!_player) {
		_packet.action = packet_id::client::login;
		sendPacket(sizeof(unsigned short));
	}
}

void OpenGL_Manager::handleClientInputs( void )
{
	// quit program
	if (inputs::key_down(inputs::quit_program)) {
		glfwSetWindowShouldClose(_window, GL_TRUE);
		return ;
	}

	if (!_player) {
		return ;
	}

	// toggle debug mode F3
	if (inputs::key_down(inputs::debug) && inputs::key_update(inputs::debug)) {
		_debug_mode = !_debug_mode;
	}

	if (_player->getResetFovUpdate() || inputs::key_update(inputs::zoom)) {
		updateCamPerspective();
	}
	if (_player->getCamUpdate()) {
		updateCamView();
		// updateVisibleChunks();
	}
}

void OpenGL_Manager::handleClientDraw( void )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*if (_time.redTickUpdate) {
		schedule::tickUpdate(); // scheduled redstone ticks
	}*/
	
	_counter = t_counter();
	// for (auto& c: _visible_chunks) {
	mtx_perimeter.lock();
	for (auto& c: _perimeter_chunks) {
		c->drawArray(_counter.newVaos, _counter.meshFaces);
		/*if (!gamePaused) {
			if (_time.tickUpdate) {
				c->updateFurnaces(_time.currentTime);
				if (_time.fluidUpdate) {
					c->updateScheduledBlocks();
					c->updateFluids(); // fluid tick
				}
				c->tickUpdate(); // random ticks
			}
			c->updateMobs(_models, _time.deltaTime);
			c->updateEntities(_entities, _particles, _time.deltaTime);
			if (Settings::Get()->getBool(settings::bools::particles)) {
				c->updateParticles(_entities, _particles, _time.deltaTime);
			}
		} else {
			c->updateMobs(_models, 0.0);
			c->updateEntities(_entities, _particles, 0.0);
			if (Settings::Get()->getBool(settings::bools::particles)) {
				c->updateParticles(_entities, _particles, 0.0);
			}
		}*/
	}
	mtx_perimeter.unlock();

	/*if (!gamePaused || _menu->getState() >= menu::death) {
		drawParticles();
		(_camera->getCamPlacement() == CAMPLACEMENT::DEFAULT)
			? _player->drawHeldItem(_models, _entities, _hand_content, _game_mode)
			: _player->drawPlayer(_models, _entities, _hand_content);
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
	if (_time.animUpdate) {
		updateAnimFrame();
	}
	glDisable(GL_CULL_FACE);
	DayCycle::Get()->setCloudsColor(_skyShader.getUniform(settings::consts::shader::uniform::color));
	if (Settings::Get()->getInt(settings::ints::clouds) != settings::OFF) {
		for (auto& c: _visible_chunks) {
			c->drawSky(_counter.newVaos, _counter.skyFaces);
		}
	}
	glUniform3f(_skyShader.getUniform(settings::consts::shader::uniform::color), 0.24705882f, 0.4627451f, 0.89411765f); // water color
	for (auto&c: _visible_chunks) {
		c->drawWater(_counter.newVaos, _counter.waterFaces);
	}
	if (Settings::Get()->getBool(settings::bools::face_culling)) {
		glEnable(GL_CULL_FACE);
	}
#endif*/
}

void OpenGL_Manager::handleClientUI( void )
{
	if (_player && _menu->getState() >= menu::pause) {
		std::string str;
		if (_debug_mode) {
			size_t residentSize, virtualSize;
			utils::memory::getMemoryUsage(residentSize, virtualSize);
			str = "Timer: " + std::to_string(_time.currentTime)
					+ "\n\n" + DayCycle::Get()->getInfos()
					+ "\nFPS: " + std::to_string(_time.nbFramesLastSecond) + "\tframe " + std::to_string((_time.deltaTime) * 1000)
					+ "\nTPS: " + std::to_string(_time.nbTicksLastSecond)
					+ "\nMem usage: " + utils::string::toBytes(residentSize)
					+ '\n' + _player->getString(_game_mode)
					+ "\nBlock\t> " + s_blocks[_block_hit.type]->name
					+ ((_block_hit.type != blocks::air) ? "\n\t\t> x: " + std::to_string(_block_hit.pos.x) + " y: " + std::to_string(_block_hit.pos.y) + " z: " + std::to_string(_block_hit.pos.z) : "\n")
					+ ((_block_hit.type) ? "\nprev\t> x: " + std::to_string(_block_hit.prev_pos.x) + " y: " + std::to_string(_block_hit.prev_pos.y) + " z: " + std::to_string(_block_hit.prev_pos.z) : "\nprev\t> none")
					+ ((_block_hit.water_value) ? "\n\t\tWATER on the way" : "\n\t\tno water")
					+ ((_game_mode != settings::consts::gamemode::creative) ? "\nBreak time\t> " + std::to_string(_break_time) + "\nBreak frame\t> " + std::to_string(_break_frame) : "\n\n")
					+ "\n\nChunk\t> x: " + std::to_string(_current_chunk.x) + " y: " + std::to_string(_current_chunk.y)
					+ "\nDisplayed chunks\t> " + std::to_string(_visible_chunks.size());
			
			mtx_perimeter.lock();
			str += '/' + std::to_string(_perimeter_chunks.size());
			mtx_perimeter.unlock();
			mtx.lock();
			str += '/' + std::to_string(_chunks.size());
			mtx.unlock();
			str += "\nDisplayed faces\t> " + std::to_string(_counter.meshFaces)
					+ "\nSky faces\t> " + std::to_string(_counter.skyFaces)
					+ "\nWater faces\t> " + std::to_string(_counter.waterFaces)
					+ "\n\nRender Distance\t> " + std::to_string(Settings::Get()->getInt(settings::ints::render_dist))
					+ "\nGame mode\t\t> " + settings::consts::gamemode::str[_game_mode];
		} else {
			str = "\n\n\nFPS: " + std::to_string(_time.nbFramesLastSecond) + "\nTPS: " + std::to_string(_time.nbTicksLastSecond);
		}

		_ui->drawUserInterface(str, _game_mode, _time.deltaTime);
	}
}

void OpenGL_Manager::runClient( void )
{
	_ui->_hideUI = true;
	startClientThread();
	DayCycle::Get()->setTicks(12000);
	utils::shader::check_glstate("entering client loop\n", true);

	while (!glfwWindowShouldClose(_window) && _socket)
	{
		handleTime(true); // true to avoid player tickUpdate

		if (_time.tickUpdate) {
			_time.redTickUpdate = DayCycle::Get()->tickUpdate();

			handlePackets();
		}

		handleClientInputs();
		handleClientDraw();
		handleClientUI();
		if (_paused) {
			handleMenu(_time.nbTicks == 1 && _time.tickUpdate);
		} else {
			_ui->textToScreen(_menu->getState() >= menu::pause);
		}
		handleChunkDeletion();
		_time.previousFrame = _time.currentTime;
		glfwSwapBuffers(_window);
		glfwPollEvents();
	}

	utils::shader::check_glstate("exiting client loop\n", true);
	stopThread();
	_ui->_hideUI = false;
}
