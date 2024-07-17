#ifndef SERVER_HPP
# define SERVER_HPP

class Server
{
	public:
		Server( t_time time, std::map<std::pair<int, int>, s_backup>& backups, std::shared_ptr<Socket> socket );
		~Server( void );

		void run( void );

	private:
		t_time _time;
		std::map<std::pair<int, int>, s_backup> _backups;
		std::shared_ptr<Socket> _socket;
		std::vector<std::unique_ptr<Player>> _players;

		void handleTime( void );
		void handlePackets( void );
		void broadcastPlayersInfo( void );
};

#endif
