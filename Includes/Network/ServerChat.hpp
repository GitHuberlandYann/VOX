#ifndef SERVERCHAT_HPP
# define SERVERCHAT_HPP

# include "utils.h"
# include "Chat.hpp"
class Server;
class Address;
class Player;
typedef s_pending_packet t_pending_packet;

class ServerChat
{
	public:
		ServerChat( Server* server );
		~ServerChat( void );

		void parseCommand( std::unique_ptr<Player>& player, Address& sender, std::string cmd );

	private:
		t_pending_packet _packet;
		Server* _server;

		/*void handle_help( int argc, std::vector<std::string>& argv );
		void handle_gamemode( int argc, std::vector<std::string>& argv );
		void handle_gamerule( int argc, std::vector<std::string>& argv );*/
		void handleTime( std::unique_ptr<Player>& player, Address& sender, int argc, std::vector<std::string>& argv );
		/*void handle_clear( int argc, std::vector<std::string>& argv );
		void handle_teleport( int argc, std::vector<std::string>& argv );
		void handle_spawnpoint( int argc, std::vector<std::string>& argv );
		void handle_give( int argc, std::vector<std::string>& argv );
		bool handle_freeze( int argc, std::vector<std::string>& argv );
		bool handle_frame( int argc, std::vector<std::string>& argv );*/
		void handleAdmin( std::unique_ptr<Player>& player, Address& sender, int argc, std::vector<std::string>& argv );

		/*float parseLocation( std::string &str, int coord );*/

		void sendPacket( Address& sender, uint color, std::string msg );
};

#endif
