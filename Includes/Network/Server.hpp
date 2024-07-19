#ifndef SERVER_HPP
# define SERVER_HPP

namespace pendings {
	const int broadcast = -2;
	const int useAddr = -1; // send packet to Address in t_pending_packet
};

typedef struct s_pending_packet {
	int id = pendings::broadcast;
	size_t size = 0;
	Address addr = {};
	t_packet_data packet = {};
}				t_pending_packet;

class Server
{
	public:
		Server( t_time time, std::map<std::pair<int, int>, s_backup>& backups, std::shared_ptr<Socket> socket );
		~Server( void );

		void run( void );

		// threading
		void setThreadUpdate( bool state );
		bool getThreadUpdate( void );
		bool getThreadStop( void );

	private:
		bool _threadUpdate, _threadStop;
		std::thread _thread;
		std::mutex _mtx, _mtx_pend;
		t_time _time;
		t_pending_packet _packet;
		std::map<std::pair<int, int>, s_backup> _backups;
		std::shared_ptr<Socket> _socket;
		std::vector<t_pending_packet> _pendingPackets;
		std::vector<std::unique_ptr<Player>> _players;

		void broadcastPlayersInfo( void );
		void sendPacket( t_pending_packet& pending );
		void pendPacket( void );

		void handlePacketLogin( Address& sender, std::string name );

		void handleTime( void );
		void handlePackets( void );
		
		// threading
		void startThread( void );
		void stopThread( void );
};

#endif
