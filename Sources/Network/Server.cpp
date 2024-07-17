#include "OpenGL_Manager.hpp"
#include "Player.hpp"
#include "Server.hpp"
#include "logs.hpp"

Server::Server( t_time time, std::map<std::pair<int, int>, s_backup>& backups, std::shared_ptr<Socket> socket )
	: _time(time), _backups(backups), _socket(socket)
{

}

Server::~Server( void )
{
	MAINLOG(LOG("Destructor of Server called"));
	glfwTerminate();

	DayCycle::Destroy();
	Settings::Destroy();
}

void OpenGL_Manager::createServer( std::unique_ptr<Server>& server )
{
	server = std::make_unique<Server>(_time, _backups, _socket);
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
		t_packet_data data = {packet_id::server::time};
		memmove(data.data, &_time.currentTime, sizeof(double));
		_socket->broadcast(&data, sizeof(data.action) + sizeof(double)); // send time every 20 game ticks (== every second) to players
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
	while (true) { // read incoming packets
		Address sender;
		t_packet_data buffer;

		ssize_t bytes_read = _socket->receive(sender, &buffer, sizeof(buffer));
		
		if (bytes_read <= 0) {
			break;
		}
		PACKETLOG(LOG("received " << bytes_read << " bytes"));

		// process packet
		switch (buffer.action & mask::network::action_type) {
			case packet_id::client::login:
				break ;
			default:
				MAINLOG(LOGERROR("Unrecognised packet action: " << buffer.action << ", sent with data: |" << buffer.data << "|"));
		}
	}
}

void Server::broadcastPlayersInfo( void )
{
	for (auto& player : _players) {
		t_packet_data data = {packet_id::server::player_info};
		std::string content = player->getString(false); // TODO custom function instead of this one
		memmove(data.data, content.c_str(), content.size());
		_socket->broadcast(&data, sizeof(data.action) + content.size());
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

	while (true)
	{
		handleTime();

		if (_time.tickUpdate) {
			// _player->tickUpdate();
			_time.redTickUpdate = DayCycle::Get()->tickUpdate();

			handlePackets();
			broadcastPlayersInfo();
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