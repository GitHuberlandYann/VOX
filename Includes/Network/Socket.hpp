#ifndef SOCKET_HPP
# define SOCKET_HPP

# include "Address.hpp"
# include <string>
# include <array>
# include <vector>
# include <list>

namespace sockets {
	enum {
		server,
		client
	};
}

namespace settings {
	namespace consts {
		namespace network {
			// id used for this program, if other id found in packet, it is discarded
			// for the program ./vox, id is "VOX!" (0x56 0x4F 0x58 0x21)
			const unsigned int protocol_id = 0x564F5821;
			const size_t packet_size_limit = 65518;
			const size_t uncompressed_packet_size_limit = 0x40000;
			const size_t client_size_limit = 8; // TODO change this
		};

		namespace math {
			const uint16_t uint16_half = 0x8000;
		};
	};
};

namespace mask {
	namespace network {
		const short action_type = 0xFF;
	};
};

namespace send_ret {
	const int success = -2;
	const int failure = -1;
	const int timeout = 0;
};

typedef struct s_packet_data {
	unsigned short action = 0;
	char data[settings::consts::network::packet_size_limit] = {};
}				t_packet_data;

typedef struct s_packet {
	unsigned int protocol_id = 0;
	uint16_t sequence = 0;
	uint16_t ack = 0; // acknoledge
	unsigned int ack_bitfield = 0;
	t_packet_data data;
}				t_packet;

namespace settings {
	namespace consts {
		namespace network {
			const size_t packet_header_size = sizeof(t_packet) - sizeof(t_packet_data);
		};
	};
};

typedef struct s_client {
	Address ip;
	unsigned int timeout = 0;
	int id = -1;
	uint16_t sequence = 0;
	uint16_t ack = -1;
	unsigned int bitfield = 0;
}				t_client;

namespace player_action {
	const unsigned char start_digging = 0;
	const unsigned char cancel_digging = 1;
	const unsigned char finish_digging = 2;
	// const unsigned char drop_item_stack = 3;
	// const unsigned char drop_item = 4;
	// const unsigned char shoot_arrow = 5;
	// const unsigned char finish_eating = shoot_arrow;
	// const unsigned char swap_item_hand = 6;
};

namespace packet_id {
	namespace client { // ids of packets sent by client
		enum {
			login,
			pong, // response to ping
			request_ping_list,
			leave,
			settings,
			player_pos,
			chat_msg,
			player_action,
		};
	};

	namespace server { // ids of packets sent by server
		enum {
			login,
			ping, // sent once per second to keep connection going
			ping_list, // id + name of all players
			kick,
			chunk_data, // 16x16x16 subChunk content
			unload_chunk,
			player_join,
			player_leave,
			players_info, // id, pos, deltas, yaw, pitch of all players
			chat_msg,
			block_destroy_stage, // set breaking anim of a block
			block_update,
			// update_section_blocks, // used when several blocks within one section are changed on same tick
			// acknowledge_block_change, // will see if I use this
			// block_action, // will be used for chests and pistons
		};
	};

	namespace step {
		enum {
			zero, one, two, three, four, five,
		};
	};
};

class Socket
{
    public:
        Socket( int type );
        ~Socket( void );

        bool open( unsigned short port = settings::consts::network::default_port );
        void close( void );
        bool isOpen( void ) const;

        int send( const Address& destination, const void* data, size_t size );
		std::vector<int> broadcast( const void* data, size_t size );
        ssize_t receive( Address& sender, void* data, size_t size );

		int getType( void );
		int getId( Address& target );
		Address* getAddress( int id );
		Address& getServerAddress( void );
		int getPing( void );
		unsigned getPacketLost( void );
		unsigned getPacketSent( void );

    private:
        int _handle, _type, _ping_us, _ping_ms;
		unsigned _sent, _lost;
		Address _server_ip;
		t_packet _packet;
		std::array<bool, settings::consts::network::client_size_limit> _occupied_ids;
		std::vector<t_client> _clients;
		// list of packets waiting for acknoledgment, with timestamps to compute ping
		std::list<std::pair<uint16_t, int64_t>> _pending_packets;
};

#endif
