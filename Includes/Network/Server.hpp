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
		std::list<std::shared_ptr<Chunk>> _chunks;
		std::vector<std::shared_ptr<Chunk>> _deleted_chunks;
		std::map<std::pair<int, int>, s_backup> _backups;

		Server( t_time time, std::map<std::pair<int, int>, s_backup>& backups, std::shared_ptr<Socket> socket );
		~Server( void );

		void pendPacket( t_pending_packet& packet );
		void run( void );

		/** @category threading */
		void getPlayersChunkPos( std::vector<glm::ivec3>& pos );
		bool isLoadedForPlayer( int playerId, glm::ivec2 chunkPos );
		void unloadForPlayer( int playerId, glm::ivec2 chunkPos );
		void setThreadUpdate( bool state );
		bool getThreadUpdate( void );
		bool getThreadStop( void );

	private:
		bool _threadUpdate, _threadStop;
		std::thread _thread;
		std::mutex _mtx, _mtx_pend, _mtx_plrs;
		t_time _time;
		t_pending_packet _packet;
		std::shared_ptr<Socket> _socket;
		std::vector<t_pending_packet> _pendingPackets;
		std::vector<std::unique_ptr<Player>> _players;

		void broadcastPlayersInfo( void );
		void sendPacket( t_pending_packet& pending );
		void pendPacket( void );

		void handlePacketLogin( Address& sender, std::string name );
		void handlePacketPosition( Address& sender );

		void handleTime( void );
		void handlePackets( void );
		void handleChunkDeletion( void );
		
		/** @category threading */
		void startThread( void );
		void stopThread( void );
};

#endif
