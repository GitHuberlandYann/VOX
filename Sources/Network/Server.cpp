#include "OpenGL_Manager.hpp"
#include "Player.hpp"
#include "Server.hpp"
#include "utils.h"
#include "logs.hpp"

#include <unistd.h> // usleep

Server::Server( t_time time, std::map<std::pair<int, int>, s_backup>& backups, std::shared_ptr<Socket> socket )
	: _backups(backups), _threadUpdate(false), _threadStop(false), _time(time), _socket(socket)
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
}

// ************************************************************************** //
//                                Packets                                     //
// ************************************************************************** //

void Server::broadcastPlayersInfo( void )
{
	for (auto& player : _players) {
		std::string content = player->getString(false); // TODO custom function instead of this one
		_packet = {pendings::broadcast, sizeof(unsigned short) + content.size()};
		_packet.packet.action = packet_id::server::player_info;
		memmove(_packet.packet.data, content.c_str(), content.size());
		pendPacket();
	}
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
		_players.erase(std::remove_if(_players.begin(), _players.end(), [id](auto& player) { return (player->getId() == id); }));
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

void Server::handlePacketLogin( Address& sender, std::string name )
{
	_packet = {pendings::useAddr, sizeof(unsigned short), sender, {packet_id::server::login}};
	pendPacket();
	int id = _socket->getId(sender);
	auto search = std::find_if(_players.begin(), _players.end(), [id](auto& player) { return (player->getId() == id); });
	if (search == _players.end()) {
		_mtx_plrs.lock();
		_players.push_back(std::make_unique<Player>());
		_mtx_plrs.unlock();
		_players.back()->setId(id);
		_players.back()->setName(name);
		setThreadUpdate(true);
		name = name + " joined the game. [id:" + std::to_string(id) + "]";
		_packet = {pendings::broadcast, sizeof(unsigned short) + name.size()};
		_packet.packet.action = packet_id::server::chat_msg;
		memmove(_packet.packet.data, name.c_str(), name.size());
		pendPacket();
	}
}

// ************************************************************************** //
//                                  Run                                       //
// ************************************************************************** //

void Server::handleTime( void )
{
	_time.currentTime = glfwGetTime();
	_time.deltaTime = _time.currentTime - _time.previousFrame;

	++_time.nbFrames;
	if (_time.currentTime - _time.lastSecondRecorded >= 1.0) {
		_time.nbFramesLastSecond = _time.nbFrames;
		_time.nbFrames = 0;
		_time.nbTicksLastSecond = _time.nbTicks;
		_time.nbTicks = 0;
		_time.lastSecondRecorded += 1.0;
		_packet = {pendings::broadcast, sizeof(unsigned short) + sizeof(double) + sizeof(int) + sizeof(int)};
		_packet.packet.action = packet_id::server::ping;
		memmove(_packet.packet.data, &_time.currentTime, sizeof(double));
		memmove(&_packet.packet.data[sizeof(double)], &_time.nbFramesLastSecond, sizeof(int));
		memmove(&_packet.packet.data[sizeof(double) + sizeof(int)], &_time.nbTicksLastSecond, sizeof(int));
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
	for (auto& pending : _pendingPackets) {
		sendPacket(pending);
	}
	_pendingPackets.clear();
	_mtx_pend.unlock();

	// read incoming packets
	Address sender;
	while (true) {
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
			case packet_id::client::leave:
				break ;
			default:
				MAINLOG(LOGERROR("Server::handlePackets: Unrecognised packet action: " << _packet.packet.action << " [" << bytes_read << " bytes]" << ", sent with data: |" << _packet.packet.data << "|"));
				break ;
		}
	}
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

			// broadcastPlayersInfo();
		}
		handlePackets();

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