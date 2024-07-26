#include "OpenGL_Manager.hpp"
#include "callbacks.hpp"
#include "logs.hpp"

// ************************************************************************** //
//                                Packets                                     //
//                                  Send                                      //
// ************************************************************************** //

/**
 * @brief send packet stored at this->_packet of given size to this->_socket
 */
void OpenGL_Manager::sendPacket( size_t size )
{
	int sendRet = _socket->send(_socket->getServerAddress(), &_packet, size);
	if (sendRet >= send_ret::timeout) {
		stopThread();
		_player = nullptr;
		_socket = nullptr;
		_paused = true;
		_menu->setState(menu::error);
		_menu->setErrorStr({"Disconnected from server", "Timeout"});
	}
}

size_t Player::packetPos( t_packet_data& packet )
{
	size_t cursor = 0;
	glm::vec3 pos = getSmoothPos();
	utils::memory::memwrite(packet.data, &pos.x, sizeof(float) * 3, cursor);
	utils::memory::memwrite(packet.data, &_yaw, sizeof(float), cursor);
	utils::memory::memwrite(packet.data, &_pitch, sizeof(float), cursor);
	return (cursor + sizeof(unsigned short));
}

void OpenGL_Manager::sendPlayerPos( void )
{
	if (!_player) {
		_packet.action = packet_id::client::login;
		size_t usernameSize = 0;
		utils::memory::memwrite(_packet.data, _world_name.c_str(), _world_name.size(), usernameSize);
		return (sendPacket(sizeof(unsigned short) + usernameSize));
	}

	_packet.action = packet_id::client::player_pos;
	sendPacket(_player->packetPos(_packet));
}

bool OpenGL_Manager::sendMessageServer( void )
{
	std::string msg = inputs::getCurrentMessage();
	if (msg.empty()) {
		return (false);
	}
	inputs::resetMessage();
	_packet.action = (msg[0] == '/') ? packet_id::client::chat_command : packet_id::client::chat_msg;
	size_t cursor = 0;
	utils::memory::memwrite(_packet.data, msg.c_str(), msg.size(), cursor);
	sendPacket(cursor + sizeof(unsigned short));
	return (true);
}

// ************************************************************************** //
//                                Packets                                     //
//                                Receive                                     //
// ************************************************************************** //

void OpenGL_Manager::handlePacketLogin( void )
{
	if (_player) {
		return ;
	}
	_player = std::make_unique<Player>();
	_camera->setTarget(_player.get());
	_player->setCamUpdate(true);
	glm::ivec3 worldSpawn;
	size_t cursor = 0;
	int id;
	utils::memory::memread(&id, _packet.data, sizeof(int), cursor);
	_player->setId(id);
	utils::memory::memread(&worldSpawn.x, _packet.data, sizeof(int) * 3, cursor);
	_player->setPos(worldSpawn);
	_menu->setErrorStr({"Succesfully connected to server."});
	_menu->setState(menu::pause);
	_menu->setPtrs(_player->getInventory(), _ui.get());
	_ui->setPtrs(this, _player->getInventory(), _player.get());
	_player->setPtrs(this, _menu.get(), _ui.get());
	// _paused = false;

}

void OpenGL_Manager::handlePacketPing( void )
{
	size_t cursor = 0;
	int tick;
	utils::memory::memread(&tick, _packet.data, sizeof(int), cursor);
	DayCycle::Get()->setTicks(tick); // TODO might want to avoid forceReset
	utils::memory::memread(&_serverTime.nbFramesLastSecond, _packet.data, sizeof(int), cursor);
	utils::memory::memread(&_serverTime.nbTicksLastSecond, _packet.data, sizeof(int), cursor);
	MAINLOG(LOG("Client::handlePackets: time received: " << tick << " server fps: " << _serverTime.nbFramesLastSecond << ", server tps: " << _serverTime.nbTicksLastSecond));
	_packet.action = packet_id::client::pong;
	sendPacket(sizeof(unsigned short));
}

void OpenGL_Manager::handlePacketPingList( void )
{
	size_t cursor = 0;
	int nbPlayers;

	utils::memory::memread(&nbPlayers, _packet.data, sizeof(int), cursor);
	for (int index = 0; index < nbPlayers; ++index) {
		int id, nameSize;
		utils::memory::memread(&id, _packet.data, sizeof(int), cursor);
		utils::memory::memread(&nameSize, _packet.data, sizeof(int), cursor);
		std::string name;
		name.resize(nameSize + 1);
		utils::memory::memread(&name[0], _packet.data, nameSize, cursor);
		if (id == _player->getId()) {
		} else if (!_players.count(id)) {
			_players[id] = std::make_unique<Player>();
			_players[id]->setId(id);
			_players[id]->setName(name);
		}
	}
}

void OpenGL_Manager::handlePacketKick( std::string msg )
{
	_ui->setPtrs(this, NULL, NULL);
	stopThread();
	_player = nullptr;
	_socket = nullptr;
	_paused = true;
	_menu->setState(menu::error);
	_menu->setErrorStr({"You have been kicked by the server.", "Server msg: " + msg});
}

void OpenGL_Manager::handlePacketChunk( void )
{
	_mtx_packets.lock();
	_chunkPackets.push_back(_packet);
	_mtx_packets.unlock();
	setThreadUpdate(true);
}

void OpenGL_Manager::handlePacketChunkUnload( void )
{
	glm::ivec2 chunkPos;
	size_t cursor = 0;
	utils::memory::memread(&chunkPos.x, _packet.data, sizeof(int), cursor);
	utils::memory::memread(&chunkPos.y, _packet.data, sizeof(int), cursor);
	_player->isLoaded(chunkPos); // set chunk as 'loaded' means chunk should be deleted for client
	setThreadUpdate(true);
}

void OpenGL_Manager::handlePacketPlayerJoin( void )
{
	int id;
	size_t cursor = 0;
	utils::memory::memread(&id, _packet.data, sizeof(int), cursor);
	std::string name = &_packet.data[cursor];
	_ui->chatMessage(name + " joined the game");
	MAINLOG(LOG("Player joined with name " << name << " and id " << id));
	if (id == _player->getId()) {
		_player->setName(name);
	} else if (!_players.count(id)) {
		_players[id] = std::make_unique<Player>();
		_players[id]->setId(id);
		_players[id]->setName(name);
	}
}

void OpenGL_Manager::handlePacketPlayerLeave( void )
{
	int id;
	size_t cursor = 0;
	utils::memory::memread(&id, _packet.data, sizeof(int), cursor);
	if (_players.count(id)) {
		std::string name = &_packet.data[cursor];
		_ui->chatMessage(name + " left the game");
		MAINLOG(LOG("Player left with name " << name << " and id " << id));
		_players.erase(id);
	} else {
		MAINLOG(LOGERROR("player with id " << id << " left but doesn't exist"));
	}
}

void OpenGL_Manager::handlePacketPlayersInfo( size_t size )
{
	size_t cursor = 0;
	bool requestPingList = false;
	while (cursor < size) {
		int id;
		utils::memory::memread(&id, _packet.data, sizeof(int), cursor);
		if (_players.count(id)) {
			_players[id]->handlePacketPos(_packet, cursor, true, _player->getChunkPtr());
		} else if (id == _player->getId()) {
			cursor += sizeof(float) * 8;
		} else {
			cursor += sizeof(float) * 8;
			requestPingList = true;
		}
	}
	if (requestPingList) {
		_packet.action = packet_id::client::request_ping_list;
		sendPacket(sizeof(unsigned short));
	}
}

void OpenGL_Manager::handlePacketChatMsg( void )
{
	uint color;
	size_t cursor = 0;
	utils::memory::memread(&color, _packet.data, sizeof(uint), cursor);
	_ui->chatMessage(&_packet.data[cursor], color);
	MAINLOG(LOG("chatMessage: |" << &_packet.data[cursor] << "|"));
}

void OpenGL_Manager::handlePackets( void )
{
	if (!_socket) {
		return ;
	}

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
				handlePacketPing();
				break ;
			case packet_id::server::ping_list:
				handlePacketPingList();
				break ;
			case packet_id::server::kick:
				return (handlePacketKick(_packet.data));
			case packet_id::server::chunk_data:
				handlePacketChunk();
				break ;
			case packet_id::server::unload_chunk:
				handlePacketChunkUnload();
				break ;
			case packet_id::server::player_join:
				handlePacketPlayerJoin();
				break ;
			case packet_id::server::player_leave:
				handlePacketPlayerLeave();
				break ;
			case packet_id::server::players_info:
				handlePacketPlayersInfo(bytes_read - sizeof(unsigned short));
				break ;
			case packet_id::server::chat_msg:
				handlePacketChatMsg();
				break ;
			default:
				MAINLOG(LOGERROR("Client::handlePackets: Unrecognised packet action: " << _packet.action << " [" << bytes_read << " bytes]" << ", sent with data: |" << _packet.data << "|"));
				break ;
		}
	}
}

// ************************************************************************** //
//                                  Run                                       //
// ************************************************************************** //

void OpenGL_Manager::handleClientInputs( void )
{
	if (_paused) {
		return ;
	}

	// quit program
	if (inputs::key_down(inputs::quit_program)) {
		glfwSetWindowShouldClose(_window, GL_TRUE);
		return ;
	}

	if (!_player) {
		return ;
	}

	// open menu
	if (inputs::key_down(inputs::close) && inputs::key_update(inputs::close)) {
		_paused = true;
		_menu->setState(menu::pause);
		return ;
	}
	/*
	// open inventory
	if (inputs::key_down(inputs::inventory) && inputs::key_update(inputs::inventory)) {
		_paused = true;
		_menu->setState(menu::inventory);
		return ;
	}*/
	// toggle F3 + I == display full info about block hit
	if (inputs::key_down(inputs::info) && inputs::key_update(inputs::info) && inputs::key_down(inputs::debug)) {
		t_hit bHit = _player->getBlockHit();
		_ui->chatMessage("*******************");
		_ui->chatMessage("Info about " + s_blocks[bHit.type]->name + " at " + std::to_string(bHit.pos.x) + ", " + std::to_string(bHit.pos.y) + ", " + std::to_string(bHit.pos.z));
		_ui->chatMessage(std::string("visible: ") + ((bHit.value & mask::blocks::notVisible) ? "FALSE" : "TRUE"));
		_ui->chatMessage(std::string("adventure block: ") + ((bHit.value & mask::adventure_block) ? "TRUE" : "FALSE"));
		_ui->chatMessage(std::string("orientation: ") + std::to_string((bHit.value >> offset::blocks::orientation) & 0x7));
		_ui->chatMessage(std::string("transparent: ") + ((s_blocks[bHit.type]->isTransparent(bHit.value)) ? "TRUE" : "FALSE"));
		_ui->chatMessage(std::string("powered: ") + ((bHit.value & mask::redstone::powered) ? "TRUE" : "FALSE"));
		_ui->chatMessage(std::string("weakdy powered: ") + ((bHit.value & mask::redstone::weakdy_powered) ? "TRUE" : "FALSE"));
		_ui->chatMessage(std::string("activated: ") + ((bHit.value & mask::redstone::activated) ? "TRUE" : "FALSE"));
		_ui->chatMessage(std::string("strength: ") + std::to_string((bHit.value >> offset::redstone::strength) & 0xF));
		_ui->chatMessage(std::string("bitfield: ") + std::to_string(bHit.value >> offset::blocks::bitfield));
		switch (bHit.type) {
			case blocks::piston:
			case blocks::sticky_piston:
				_ui->chatMessage(std::string("moving piston: ") + ((bHit.value & mask::redstone::piston::moving) ? "TRUE" : "FALSE"));
			break ;
		}
		_ui->chatMessage("*******************");
	}
	// toggle hotbar F1
	if (inputs::key_down(inputs::hotbar) && inputs::key_update(inputs::hotbar)) {
		_ui->_hideUI = !_ui->_hideUI;
		Settings::Get()->setBool(settings::bools::hide_ui, _ui->_hideUI);
		_ui->chatMessage(std::string("UI ") + ((_ui->_hideUI) ? "HIDDEN" : "SHOWN"));
	}
	/*/ take screenshot F2
	if (inputs::key_down(inputs::screenshot) && inputs::key_update(inputs::screenshot)) {
		screenshot();
	}*/
	// toggle F5 mode
	if (inputs::key_down(inputs::perspective) && inputs::key_update(inputs::perspective)) {
		_camera->changeCamPlacement();
	}
	// toggle chat
	if ((inputs::key_down(inputs::chat) && inputs::key_update(inputs::chat))
		|| (inputs::key_down(inputs::enter) && inputs::key_update(inputs::enter))) {
		_paused = true;
		_menu->setState(menu::chat);
		return ;
	}
	// toggle 'command' chat, ie open chat, with '/' already written
	if (inputs::key_down(inputs::command) && inputs::key_update(inputs::command)) {
		_paused = true;
		_menu->setState(menu::command);
		return ;
	}
	// change block atlas
	if (inputs::key_down(inputs::block_atlas_clean) && inputs::key_update(inputs::block_atlas_clean)) {
		Settings::Get()->setString(settings::strings::block_atlas, "Resources/Resource_Packs/Clean/Textures/");
		loadTextures();
	} else if (inputs::key_down(inputs::block_atlas_default) && inputs::key_update(inputs::block_atlas_default)) {
		Settings::Get()->setString(settings::strings::block_atlas, "Resources/Textures/");
		loadTextures();
	}
	// toggle debug mode F3
	if (inputs::key_down(inputs::debug) && inputs::key_update(inputs::debug)) {
		_debugMode = !_debugMode;
	}
	_player->inputToggle();

	_player->setDelta(_time.deltaTime);
	_player->clientInputUpdate();
	chunkUpdate();

	if (_player->getResetFovUpdate() || inputs::key_update(inputs::zoom)) {
		updateCamPerspective();
	}
	if (_player->getCamUpdate()) {
		updateCamView();
		updateVisibleChunks();
	}
}

void OpenGL_Manager::handleClientDraw( void )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*if (_time.redTickUpdate) {
		schedule::tickUpdate(); // scheduled redstone ticks
	}*/
	
	_counter = t_counter();
	for (auto& c: _visible_chunks) {
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

	if (_menu->getState() >= menu::death) {
		drawParticles();
		(_camera->getCamPlacement() == CAMPLACEMENT::DEFAULT)
			? _player->drawHeldItem(_models, _entities)
			: _player->drawPlayer(_models, _entities);
		for (auto& player : _players) {
			player.second->setDelta(_time.deltaTime);
			player.second->moveClient();
			player.second->drawPlayer(_models, _entities);
		}
		drawModels();
		_shader.useProgram();
		_player->addBreakingAnim(_entities);
		drawEntities();
	}

	if (_menu->getState() >= menu::pause && Settings::Get()->getBool(settings::bools::skybox)) {
		_skybox->render(_camera->getCamPos());
	}

	_skyShader.useProgram();
	if (_time.animUpdate) {
		updateAnimFrame();
	}
	glDisable(GL_CULL_FACE);
	/*DayCycle::Get()->setCloudsColor(_skyShader.getUniform(settings::consts::shader::uniform::color));
	if (Settings::Get()->getInt(settings::ints::clouds) != settings::OFF) {
		for (auto& c: _visible_chunks) {
			c->drawSky(_counter.newVaos, _counter.skyFaces);
		}
	}*/
	glUniform3f(_skyShader.getUniform(settings::consts::shader::uniform::color), 0.24705882f, 0.4627451f, 0.89411765f); // water color
	for (auto&c: _visible_chunks) {
		c->drawWater(_counter.newVaos, _counter.waterFaces);
	}
	if (Settings::Get()->getBool(settings::bools::face_culling)) {
		glEnable(GL_CULL_FACE);
	}
}

void OpenGL_Manager::handleClientUI( void )
{
	if (_player && _menu->getState() >= menu::pause) {
		std::string str, rightStr;
		if (_debugMode) {
			size_t residentSize, virtualSize;
			utils::memory::getMemoryUsage(residentSize, virtualSize);
			str = "Timer: " + std::to_string(_time.currentTime)
					+ "\n\n" + DayCycle::Get()->getInfos()
					+ "\nFPS: " + std::to_string(_time.nbFramesLastSecond) + "\tframe " + std::to_string((_time.deltaTime) * 1000)
					+ "\nTPS: " + std::to_string(_time.nbTicksLastSecond)
					+ "\nMem usage: " + utils::string::toBytes(residentSize)
					+ '\n' + _player->getString()
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
					+ "\nGame mode\t\t> " + settings::consts::gamemode::str[_player->getGameMode()];
			
			rightStr = "Server ip: " + _menu->getServerIP()
					+ "\nServer rps: " + std::to_string(_serverTime.nbFramesLastSecond)
					+ "\nServer tps: " + std::to_string(_serverTime.nbTicksLastSecond)
					+ "\nPing: " + std::to_string(_socket->getPing()) + "ms"
					+ "\nPackets lost: " + std::to_string(_socket->getPacketLost()) + '/' + std::to_string(_socket->getPacketSent())
					+ "\nPlayers: " + std::to_string(_players.size() + 1);
			for (auto& player : _players) {
				rightStr += "\n\tid " + std::to_string(player.first);
			}
			_ui->addDebugStr(rightStr, true);
		} else {
			str = "\n\n\nFPS: " + std::to_string(_time.nbFramesLastSecond) + "\nTPS: " + std::to_string(_time.nbTicksLastSecond);
		}

		_ui->addDebugStr(str);
		_ui->drawUserInterface(Settings::Get()->getInt(settings::ints::game_mode), 0);//_time.deltaTime);
	}
}

void OpenGL_Manager::leaveServer( void )
{
	_packet.action = packet_id::client::leave;
	sendPacket(sizeof(unsigned short));
	_ui->setPtrs(this, NULL, NULL);
	stopThread();
	_player = nullptr;
	_socket = nullptr;
	_paused = true;
	_menu->setState(menu::main);
}

void OpenGL_Manager::runClient( void )
{
	Settings::Get()->setInt(settings::ints::session_type, settings::consts::session::client);
	startClientThread();
	DayCycle::Get()->setTicks(12000);
	utils::shader::check_glstate("entering client loop\n", true);

	while (!glfwWindowShouldClose(_window) && _socket)
	{
		handleTime(true); // true to avoid player tickUpdate

		if (_time.tickUpdate) {
			_time.redTickUpdate = DayCycle::Get()->tickUpdate();

			sendPlayerPos();
		}

		handlePackets();
		handleClientInputs();
		handleClientDraw();
		handleClientUI();
		if (_paused) {
			handleMenu(_time.nbTicks == 1 && _time.tickUpdate);
		} else if (inputs::key_down(inputs::tab)) {
			_menu->displayTabList(_player, _players);
		} else {
			_ui->textToScreen(_menu->getState() >= menu::pause);
		}
		handleChunkDeletion();
		glfwSwapBuffers(_window);
		glfwPollEvents();
	}

	utils::shader::check_glstate("exiting client loop\n", true);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	stopThread();
	_chunks.clear();
	_perimeter_chunks.clear();
	_visible_chunks.clear();
	_players.clear();
	Settings::Get()->setInt(settings::ints::session_type, settings::consts::session::singleplayer);
}
