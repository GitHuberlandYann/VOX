#include "OpenGL_Manager.hpp"
#include "Player.hpp"
#include "Server.hpp"
#include "ServerChat.hpp"
#include "utils.h"
#include "logs.hpp"

#include <unistd.h> // usleep

Server::Server( t_time time, std::map<std::pair<int, int>, s_backup>& backups, std::shared_ptr<Socket> socket )
	: _backups(backups), _threadUpdate(false), _threadStop(false), _time(time),
	_socket(socket), _chat(std::make_unique<ServerChat>(this))
{
	startThread();
}

Server::~Server( void )
{
	MAINLOG(LOG("Destructor of Server called"));
	stopThread();

	glfwTerminate();

	DayCycle::Destroy();
	Settings::Destroy();
}

void OpenGL_Manager::createServer( std::unique_ptr<Server>& server )
{
	server = std::make_unique<Server>(_time, _backups, _socket);
	Settings::Get()->setInt(settings::ints::session_type, settings::consts::session::server);
}

// ************************************************************************** //
//                                Packets                                     //
// ************************************************************************** //

void Player::appendPacketInfo( t_pending_packet& packet )
{
	utils::memory::memwrite(packet.packet.data, &_id, sizeof(int), packet.size);
	glm::vec3 pos = getSmoothPos();
	utils::memory::memwrite(packet.packet.data, &pos.x, sizeof(float) * 3, packet.size);
	utils::memory::memwrite(packet.packet.data, &_knockback, sizeof(float) * 3, packet.size);
	utils::memory::memwrite(packet.packet.data, &_yaw, sizeof(float), packet.size);
	utils::memory::memwrite(packet.packet.data, &_pitch, sizeof(float), packet.size);
}

void Server::broadcastPlayersInfo( void )
{
	_packet = {pendings::broadcast, 0};
	_packet.packet.action = packet_id::server::players_info;
	for (auto& player: _players) {
		player.second->appendPacketInfo(_packet);
	}
	_packet.size += sizeof(unsigned short);
	pendPacket();
}

/**
 * @return true if server must pend packet
 */
bool Player::appendPacketBreakingFrame( t_pending_packet& packet )
{
	if (_blockHit.type == blocks::air) {
		return (false);
	}

	_breakTime += _deltaTime;
	bool can_collect = s_blocks[_blockHit.type]->canCollect(_handContent);
	float break_time = getBreakTime(can_collect);
	if (_breakTime >= break_time) {
		// _breakTime = (break_time > 0) ? -0.3f : 0;
		_breakFrame = 0;
		// TODO implement serverHandleBlockModif(false, can_collect);
		// updateExhaustion(settings::consts::exhaustion::breaking_block);
		// _inventory->decrementDurabitilty();
		_blockHit.type = blocks::air;
	} else {
		int breakFrame = (_breakTime < .0f) ? 0 : static_cast<int>(10 * _breakTime / break_time);
		if (_breakFrame != breakFrame) {
			packet.size = 0;
			utils::memory::memwrite(packet.packet.data, &_id, sizeof(int), packet.size);
			utils::memory::memwrite(packet.packet.data, &_blockHit.pos.x, sizeof(int) * 3, packet.size);
			utils::memory::memwrite(packet.packet.data, &breakFrame, sizeof(unsigned char), packet.size);
			_breakFrame = breakFrame;
			return (true);
		}
	}
	return (false);
}

void Server::updateBreakingFrames( void )
{
	_packet.id = pendings::broadcast;
	_packet.packet.action = packet_id::server::block_destroy_stage;
	for (auto& player: _players) {
		if (player.second->appendPacketBreakingFrame(_packet)) {
			pendPacket();
		}
	}
}

void Server::sendPingList( Address& sender )
{
	_packet = {pendings::useAddr, 0, sender, {packet_id::server::ping_list}};
	size_t nbPlayers = _players.size();
	utils::memory::memwrite(_packet.packet.data, &nbPlayers, sizeof(int), _packet.size);
	for (auto& player: _players) {
		utils::memory::memwrite(_packet.packet.data, &player.first, sizeof(int), _packet.size);
		std::string name = player.second->getName();
		size_t nameSize = name.size();
		utils::memory::memwrite(_packet.packet.data, &nameSize, sizeof(int), _packet.size);
		utils::memory::memwrite(_packet.packet.data, name.c_str(), nameSize, _packet.size);
	}
	_packet.size += sizeof(unsigned short);
	pendPacket();
}

/**
 * @brief send packet stored at this->_packet of given size to given target
 */
void Server::sendPacket( t_pending_packet& pending )
{
	// LOG("Sending packet with action: " << pending.packet.action << " [" << pending.size << " bytes]");
	std::vector<int> timedout_ids;
	int sendRet;
	switch (pending.id) {
		case -2: // broadcast
			timedout_ids = _socket->broadcast(&pending.packet, pending.size);
			break ;
		case -1: // send to given addr
			sendRet = _socket->send(pending.addr, &pending.packet, pending.size);
			if (sendRet >= send_ret::timeout) {
				timedout_ids.push_back(sendRet - send_ret::timeout);
			}
			break ;
		default:
			Address* target = _socket->getAddress(pending.id);
			if (target) {
				sendRet = _socket->send(*target, &pending.packet, pending.size);
				if (sendRet >= send_ret::timeout) {
					timedout_ids.push_back(sendRet - send_ret::timeout);
				}
			}
			break ;
	}
	for (auto id : timedout_ids) {
		_mtx_plrs.lock();
		if (_players.count(id)) {
			_packet = {pendings::broadcast, 0};
			_packet.packet.action = packet_id::server::player_leave;
			utils::memory::memwrite(_packet.packet.data, &id, sizeof(int), _packet.size);
			std::string name = _players[id]->getName();
			utils::memory::memwrite(_packet.packet.data, name.c_str(), name.size(), _packet.size);
			_packet.size += sizeof(unsigned short);
			_pendingPackets.push_back(_packet); // we can't use pendPacket because of double lock 
			_players.erase(id);
		}
		_mtx_plrs.unlock();
	}
}

void Server::pendPacket( void )
{
	_mtx_pend.lock();
	_pendingPackets.push_back(_packet);
	_mtx_pend.unlock();
}

void Server::pendPacket( t_pending_packet& packet )
{
	_mtx_pend.lock();
	_pendingPackets.push_back(packet);
	_mtx_pend.unlock();
}

// ************************************************************************** //
//                                Packets                                     //
//                                Receive                                     //
// ************************************************************************** //

void Server::handlePacketLogin( Address& sender, std::string name )
{
	int id = _socket->getId(sender);
	if (!_players.count(id)) {
		if (nameOnServer(name)) {
			_packet = {pendings::useAddr, 0, sender, {packet_id::server::kick}};
			utils::memory::memwrite(_packet.packet.data, "Name already taken", 18, _packet.size);
			_packet.size += sizeof(unsigned short);
			return (pendPacket());
		}
		_mtx_plrs.lock();
		_players[id] = std::make_unique<Player>();
		_mtx_plrs.unlock();
		_players[id]->setId(id);
		_players[id]->setName(name);
		glm::ivec3 worldSpawn = {Settings::Get()->getInt(settings::ints::world_spawn_x),
								Settings::Get()->getInt(settings::ints::world_spawn_y),
								Settings::Get()->getInt(settings::ints::world_spawn_z)};
		_players[id]->setPos(worldSpawn);
		setThreadUpdate(true);

		_packet = {pendings::useAddr, 0, sender, {packet_id::server::login}};
		utils::memory::memwrite(_packet.packet.data, &id, sizeof(int), _packet.size);
		utils::memory::memwrite(_packet.packet.data, &worldSpawn.x, sizeof(int) * 3, _packet.size);
		_packet.size += sizeof(unsigned short);
		pendPacket();

		sendPingList(sender);

		_packet = {pendings::broadcast, 0};
		_packet.packet.action = packet_id::server::player_join;
		utils::memory::memwrite(_packet.packet.data, &id, sizeof(int), _packet.size);
		utils::memory::memwrite(_packet.packet.data, name.c_str(), name.size(), _packet.size);
		_packet.size += sizeof(unsigned short);
		pendPacket();
	}
}

void Server::handlePacketLeave( Address& sender )
{
	int id = _socket->getId(sender);
	_socket->rmClient(id);
	if (_players.count(id)) {
		_packet = {pendings::broadcast, 0};
		_packet.packet.action = packet_id::server::player_leave;
		utils::memory::memwrite(_packet.packet.data, &id, sizeof(int), _packet.size);
		std::string name = _players[id]->getName();
		utils::memory::memwrite(_packet.packet.data, name.c_str(), name.size(), _packet.size);
		_packet.size += sizeof(unsigned short);
		_pendingPackets.push_back(_packet); // we can't use pendPacket because of double lock 
		_players.erase(id);
	}
}

bool Player::handlePacketPos( t_packet_data& packet, size_t& cursor, bool client, Chunk* chunk )
{
	if (client) {
		utils::memory::memread(&_position, packet.data, sizeof(float) * 3, cursor);
		utils::memory::memread(&_knockback, packet.data, sizeof(float) * 3, cursor);
		utils::memory::memread(&_yaw, packet.data, sizeof(float), cursor);
		utils::memory::memread(&_pitch, packet.data, sizeof(float), cursor);
		updateVectors();
		_chunk = chunk;
	} else {
		glm::vec3 pos;
		utils::memory::memread(&pos, packet.data, sizeof(float) * 3, cursor);
		_knockback = (pos - _position) * 20.f; // _knockback is blocks per seconds, packetPos sent 20/s
		_position = pos;
		utils::memory::memread(&_yaw, packet.data, sizeof(float), cursor);
		utils::memory::memread(&_pitch, packet.data, sizeof(float), cursor);
	}
	return (updateCurrentBlock() && updateCurrentChunk());
}

void Server::handlePacketPosition( Address& sender )
{
	int id = _socket->getId(sender);
	if (_players.count(id)) {
		size_t cursor = 0;
		if (_players[id]->handlePacketPos(_packet.packet, cursor, false)) {
			setThreadUpdate(true);
		}
	}
}

void Server::handlePacketChatMsg( Address& sender, std::string msg )
{
	int id = _socket->getId(sender);
	if (_players.count(id)) {
		_packet = {pendings::broadcast, 0};
		_packet.packet.action = packet_id::server::chat_msg;
		utils::memory::memwrite(_packet.packet.data, &argb::white, sizeof(uint), _packet.size);
		msg = "<" + _players[id]->getName() + "> " + msg;
		if (_players[id]->getPermissionLevel() == settings::consts::permission::admin) {
			msg = "[ADMIN]" + msg;
		}
		utils::memory::memwrite(_packet.packet.data, msg.c_str(), msg.size(), _packet.size);
		_packet.size += sizeof(unsigned short);
		pendPacket();
	}
}

void Server::handlePacketChatCommand( Address& sender, std::string msg )
{
	int id = _socket->getId(sender);
	if (_players.count(id)) {
		_chat->parseCommand(_players[id], sender, msg);
	}
}

void Player::updateBlockHit( bool destroy, glm::ivec3 pos )
{
	if (!destroy && _blockHit.pos == pos) {
		_blockHit.type = blocks::air;
		_breakTime = 0.f;
	} else if (destroy) {
		_breakTime = (_blockHit.type == blocks::air) ? 0.f : -.3f;
		_blockHit.pos = pos;
		_blockHit.type = (_chunk) ? _chunk->getBlockAtAbsolute(pos) : blocks::air;
		_breakFrame = 0;
	}
}

void Server::handlePacketPlayerAction( Address& sender )
{
	int id = _socket->getId(sender);
	if (_players.count(id)) {
		size_t cursor = 0;
		unsigned char action;
		utils::memory::memread(&action, _packet.packet.data, sizeof(unsigned char), cursor);
		glm::ivec3 pos;
		utils::memory::memread(&pos.x, _packet.packet.data, sizeof(int) * 3, cursor);
		switch (action) {
			case player_action::start_digging:
				_players[id]->updateBlockHit(true, pos);
				break ;
			case player_action::cancel_digging:
				_players[id]->updateBlockHit(false, pos);
				break ;
			case player_action::finish_digging: // unused for now, used to synch up block destruction
				break ;
			default:
				MAINLOG(SERVLOG(LOGERROR("handlePacketPlayerAction unrecognised action: " << action)));
				break ;
		}
	}
}

// ************************************************************************** //
//                                Players                                     //
// ************************************************************************** //

/**
 * @return true if a player on the server has the given name
 */
bool Server::nameOnServer( std::string name )
{
	for (auto& player: _players) {
		if (player.second->getName() == name) {
			return (true);
		}
	}
	return (false);
}

/**
 * @return true if at least one player on the server is admin
 */
bool Server::adminedServer( void )
{
	for (auto& player: _players) {
		if (player.second->getPermissionLevel() == settings::consts::permission::admin) {
			return (true);
		}
	}
	return (false);
}

// ************************************************************************** //
//                                  Run                                       //
// ************************************************************************** //

void Server::handleTime( void )
{
	_time.currentTime = glfwGetTime();
	_time.deltaTime = _time.currentTime - _time.previousFrame;
	_time.previousFrame = _time.currentTime;

	++_time.nbFrames;
	if (_time.currentTime - _time.lastSecondRecorded >= 1.0) {
		_time.nbFramesLastSecond = _time.nbFrames;
		_time.nbFrames = 0;
		_time.nbTicksLastSecond = _time.nbTicks;
		_time.nbTicks = 0;
		_time.lastSecondRecorded += 1.0;
		_packet = {pendings::broadcast, 0};
		_packet.packet.action = packet_id::server::ping;
		int tick = DayCycle::Get()->getTicks();
		utils::memory::memwrite(_packet.packet.data, &tick, sizeof(int), _packet.size);
		utils::memory::memwrite(_packet.packet.data, &_time.nbFramesLastSecond, sizeof(int), _packet.size);
		utils::memory::memwrite(_packet.packet.data, &_time.nbTicksLastSecond, sizeof(int), _packet.size);
		_packet.size += sizeof(unsigned short);
		pendPacket();
		MAINLOG(SERVLOG(LOG("FPS: " << _time.nbFramesLastSecond << ", TPS: " << _time.nbTicksLastSecond << ", players: " << _players.size())));
	}
	if (_time.currentTime - _time.lastGameTick >= settings::consts::tick) {
		_time.tickUpdate = true;
		++_time.nbTicks;
		_time.lastGameTick += settings::consts::tick;
		_time.fluidUpdate = (_time.nbTicks == 5 || _time.nbTicks == 10 || _time.nbTicks == 15 || _time.nbTicks == 20);
		_time.animUpdate = (_time.nbTicks & 0x1);
	} else {
		_time.tickUpdate = false;
		_time.redTickUpdate = false;
		_time.fluidUpdate = false;
		_time.animUpdate = false;
	}
}

/**
 * @brief Send queued packets and handle incoming packets for each players
 */
void Server::handlePackets( void )
{
	// send pending packets
	_mtx_pend.lock();
	size_t pendSize = _pendingPackets.size();
	for (size_t index = 0; index < pendSize; ++index) {
		sendPacket(_pendingPackets[0]);
		_pendingPackets.erase(_pendingPackets.begin());
	}
	_mtx_pend.unlock();

	// read incoming packets
	Address sender;
	while (true) {
		_packet.size = 0;
		ssize_t bytes_read = _socket->receive(sender, &_packet.packet, sizeof(_packet.packet));
		
		if (bytes_read <= 0) {
			break;
		}
		PACKETLOG(LOG("received " << bytes_read << " bytes"));
		SERVLOG(LOG("Packet received with action: " << _packet.packet.action << " [" << bytes_read << " bytes]"));

		// process packet
		switch (_packet.packet.action & mask::network::action_type) {
			case packet_id::client::login:
				handlePacketLogin(sender, _packet.packet.data);
				break ;
			case packet_id::client::pong:
				break ;
			case packet_id::client::request_ping_list:
				sendPingList(sender);
				break ;
			case packet_id::client::leave:
				handlePacketLeave(sender);
				break ;
			case packet_id::client::player_pos:
				handlePacketPosition(sender);
				break ;
			case packet_id::client::chat_msg:
				handlePacketChatMsg(sender, _packet.packet.data);
				break ;
			case packet_id::client::chat_command:
				handlePacketChatCommand(sender, _packet.packet.data);
				break ;
			case packet_id::client::player_action:
				handlePacketPlayerAction(sender);
				break ;
			default:
				MAINLOG(LOGERROR("Server::handlePackets: Unrecognised packet action: " << _packet.packet.action << " [" << bytes_read << " bytes]" << ", sent with data: |" << _packet.packet.data << "|"));
				break ;
		}
	}
}

void Server::handleChunkDeletion( void )
{
	mtx_deleted_chunks.lock();
	schedule::deleteChunkSchedule(_deleted_chunks);
	_deleted_chunks.clear();
	mtx_deleted_chunks.unlock();
}

void Server::run( void )
{
	if (!glfwInit()) {
    	LOGERROR("glfwInit failure");
        exit(1);
    }

	// glew is there to use the correct version for all functions
	glewExperimental = GL_TRUE;
	glewInit();

	glfwSetTime(_time.currentTime);

	MAINLOG(LOG("\nServer::run with socket " << _socket->getServerAddress()));
	MAINLOG(LOG("Server ip is " << utils::network::getEth0()));

	while (true)
	{
		handleTime();

		if (_time.tickUpdate) {
			// _player->tickUpdate();
			_time.redTickUpdate = DayCycle::Get()->tickUpdate();

			if (!_players.empty()) {
				broadcastPlayersInfo();
				updateBreakingFrames();
			}
		}
		handlePackets();
		handleChunkDeletion();

		if (_time.nbFramesLastSecond > 60) {
			std::this_thread::sleep_for(std::chrono::microseconds(50));
		}
	}
}

/*
from https://minecraft.fandom.com/wiki/Tick?so=search#Game_process:

on each tick:
	for each dimension:
	    Send time every 20 gameticks to players
		Update world border bounds
		Advance weather cycle
		Update Daylight cycle time
		Player sleeping logic
		Run scheduled commands
		Scheduled block ticks
		Scheduled fluid ticks
		Raid logic
		Update chunks load level
		For all chunks in random order:
			Spawn mobs
			Tick ice and snow
			Random ticks
		Send block changes to players
		Update points of interest
		Unloads chunks
		Execute block events
		Tick dragon fight
		For all non-passenger entities:
			Check if it can despawn
			Tick entity
			Tick passengers
		Tick block entities
		Handle game events

	Send queued packets and handle incoming packets for each players
	Send player info to players
	Autosave every 6000 ticks
	Run pending tasks and wait for next tick
*/