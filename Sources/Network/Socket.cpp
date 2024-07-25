#include "Socket.hpp"
#include "utils.h" // GLuint used in DayCycle.hpp included by logs.hpp
#include "logs.hpp"
#include <unistd.h> // close
#include <algorithm> // std::find_if
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h> // strcpy
#include <chrono>
#include <array>

Socket::Socket( int type )
	: _handle(-1), _type(type), _ping_us(0), _ping_ms(0), _sent(0), _lost(0), _occupied_ids({false})
{

}

Socket::~Socket( void )
{
	MAINLOG(LOG("Destructor of " << ((_type == sockets::server) ? "Server" : "Client") << " socket " << _server_ip << " called"));
	close();
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

bool Socket::open( unsigned short port )
{
	if (isOpen()) {
        MAINLOG(LOGERROR("Socket::open: socket is already open"));
        return (false);
	}

	// UDP socket
	_handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (_handle <= 0) {
        MAINLOG(LOGERROR("Socket::open: failed to create socket"));
        return (false);
    }

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = (_type == sockets::server) ? htons(port) : 0;

	if (bind(_handle, (const sockaddr*) &addr, sizeof(sockaddr_in)) < 0) {
		MAINLOG(LOGERROR("Socket::open: failed to bind socket"));
		close();
		return (false);
	}

	int nonBlocking = 1;
    if (fcntl(_handle, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
		MAINLOG(LOGERROR("Socket::open: failed to set socket as non-blocking"));
		close();
        return (false);
	}
	return (true);
}

void Socket::close( void )
{
	if (_handle == -1) {
		return ;
	}
	::close(_handle);
	_handle = -1;
}

bool Socket::isOpen( void ) const
{
	return (_handle != -1);
}

/**
 * @return -2 for success, -1 for failure, (>=0) for timeout -> res is id of client who timed out
 */
int Socket::send( const Address& destination, const void* data, size_t size )
{
	if (size > settings::consts::network::packet_size_limit) {
		MAINLOG(LOGERROR("Socket::send: packet too big to be sent (" << size << " vs " << settings::consts::network::packet_size_limit << ')'));
		return (send_ret::failure);
	}

	sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(destination.getAddress());
    addr.sin_port = htons(destination.getPort());

	t_client* dst = NULL;
	size_t index = 0;
	switch (_type) {
		case sockets::server:
			for (auto& c : _clients) {
				if (c.ip == destination) {
					dst = &c;
					break ;
				}
				++index;
			}
			break ;
		case sockets::client:
			if (_clients.empty()) {
				_clients.push_back({destination, });
				_clients[0].timeout = DayCycle::Get()->getGameTicks();
				_clients[0].id = 0;
			} else if (_clients[0].ip != destination) {
				MAINLOG(LOGERROR("Socket::send: address does not match: " << _clients[0].ip << " vs " << destination));
				return (send_ret::failure);
			}
			dst = &_clients[0];
			break ;
		default:
			MAINLOG(LOGERROR("Socket::send: unrecognised type: " << _type));
			return (send_ret::failure);
	}

	if (!dst) {
		MAINLOG(LOGERROR("Socket::send: failed to send packet, client not found"));
		return (send_ret::failure);
	}

	_packet.protocol_id = settings::consts::network::protocol_id;
	_packet.sequence = dst->sequence;
	_packet.ack = dst->ack;
	_packet.ack_bitfield = dst->bitfield;

	if (DayCycle::Get()->getGameTicks() - dst->timeout > 5 * 20) { // timeout after 5 seconds
		// disconnection
		if (_type == sockets::server) {
			MAINLOG(LOG("disconnection from " << dst->ip << ", id is " << dst->id << " | timeout was " << dst->timeout << " vs " << DayCycle::Get()->getGameTicks()));
			_packet.data.action = packet_id::server::kick;
			memmove(&_packet.data.data, "Timeout", 7);
			sendto(_handle, static_cast<const void*>(&_packet.protocol_id), 9 + settings::consts::network::packet_header_size, 0, (sockaddr*)&addr, sizeof(sockaddr_in));
		} else {
			MAINLOG(LOG("disconnected from server"));
		}
		int id = dst->id;
		_occupied_ids[id] = false; // free up id
		_clients.erase(_clients.begin() + index);
		return (send_ret::timeout + id);
	}

	memmove(&_packet.data, data, size);
	size += settings::consts::network::packet_header_size;

	PACKETLOG(LOG("sending packet " << _packet.protocol_id << " - " << _packet.sequence << " - " << _packet.ack << " - " << _packet.ack_bitfield << ": " << _packet.data));
	size_t sent_bytes = sendto(_handle, static_cast<const void*>(&_packet.protocol_id), size, 0, (sockaddr*)&addr, sizeof(sockaddr_in));

    if (sent_bytes != size) {
        MAINLOG(LOGERROR("failed to send packet"));
        return (send_ret::failure);
    }

	PACKETLOG(LOG("sent packet of size " << sent_bytes << " to " << destination));
	if (_type == sockets::client) {
		// update pending packets
		auto timestamp = std::chrono::high_resolution_clock::now();
		int64_t ms = std::chrono::time_point_cast<std::chrono::microseconds>(timestamp).time_since_epoch().count();
		_pending_packets.push_back({dst->sequence, ms});
		++_sent;
		if (_pending_packets.size() > 32) {
			_pending_packets.pop_front();
			++_lost;
		}
	}
	++dst->sequence;
	return (send_ret::success);
}

/**
 * @brief broadcast data of given size to all clients
 * @return ids of clients who timed out
 */
std::vector<int> Socket::broadcast( const void* data, size_t size )
{
	std::vector<int> res;

	if (_type == sockets::client) { // only server is allowed to broadcast
		return (res);
	}

	size_t delCount = 0;
	size_t clientSize = _clients.size();
	for (size_t index = 0; index < clientSize; ++index) {
		int sendRet = send(_clients[index - delCount].ip, data, size);
		if (sendRet >= send_ret::timeout) {
			res.push_back(sendRet - send_ret::timeout);
			++delCount;
		}
	}
	return (res);
}

static inline bool sequence_greater_than( uint16_t s1, uint16_t s2 )
{
	return (s1 > s2 && s1 - s2 <= settings::consts::math::uint16_half)
		|| (s1 < s2 && s2 - s1 > settings::consts::math::uint16_half);
}

static inline int diff_wrap_around( uint16_t s1, uint16_t s2 )
{
	if (s1 > s2 && s1 - s2 <= settings::consts::math::uint16_half) {
		return (s1 - s2);
	} else if (s1 < s2 && s2 - s1 > settings::consts::math::uint16_half) { // wrap_around
		return (s1 + 0x10000 - s2);
	}
	return (-1); // s2 > s1
}

ssize_t Socket::receive( Address& sender, void* data, size_t size )
{
	sockaddr_in from;
	socklen_t fromLength = sizeof(from);

	size_t bytes = recvfrom(_handle, static_cast<void*>(&_packet), settings::consts::network::packet_header_size + size, 0, (sockaddr*)&from, &fromLength);

	// discard wrong recv
	if (bytes == std::string::npos || bytes < settings::consts::network::packet_header_size) {
	    return (-1);
	}
	bytes -= settings::consts::network::packet_header_size;
	if (bytes >= settings::consts::network::packet_size_limit) {
		MAINLOG(LOGERROR("Socket::receive: packet discarded because too big ("
			<< ((static_cast<ssize_t>(bytes + settings::consts::network::packet_header_size) == -1) ? -1 : bytes)
			<< " vs " << settings::consts::network::packet_size_limit));
		return (0);
	}
	if (_packet.protocol_id != settings::consts::network::protocol_id) {
		MAINLOG(LOGERROR("Socket::receive: packet discarded becausee wrong protocol id used: " << _packet.protocol_id));
		return (0);
	}

	sender = Address(ntohl(from.sin_addr.s_addr), ntohs(from.sin_port));
	t_client* src = NULL;
	for (auto& c : _clients) {
		if (c.ip == sender) {
			src = &c;
			break ;
		}
	}
	if (!src) {
		if (_type == sockets::client) {
			MAINLOG(LOGERROR("Socket::receive: packet discarded because source is not server"));
			return (0);
		}
		if (_clients.size() == settings::consts::network::client_size_limit) {
			MAINLOG(LOGERROR("Socket::receive: server max capacity reached, can't accept new connection"));
			return (0);
		}
		_clients.push_back({sender});
		src = &_clients.back();
		for (size_t i = 0; i < settings::consts::network::client_size_limit; ++i) {
			if (!_occupied_ids[i]) {
				src->id = i;
				_occupied_ids[i] = true;
				break ;
			}
		}
		MAINLOG(LOG("new connection from " << src->ip << ", id is " << src->id));
	}

	src->timeout = DayCycle::Get()->getGameTicks(); // reset timeout
	if (!sequence_greater_than(_packet.sequence, src->ack)) {
		PACKETLOG(LOGERROR("Socket::receive: packet discarded because sequence smaller than remote"));
		if (_type == sockets::server) { // update ack_bitfield
			int diff = diff_wrap_around(src->ack, _packet.sequence);
			src->bitfield |= (1 << (diff - 1));
		}
		return (0);
	}

	if (_type == sockets::client) {
		// update own packet loss
		for (auto& pack : _pending_packets) {
			if (pack.first == _packet.ack) {
				auto endstamp = std::chrono::high_resolution_clock::now();
				int64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(endstamp).time_since_epoch().count();
				int ping = end - pack.second;
				_ping_us += 0.1 * (ping - _ping_us); // exponentially smoothed moving average
				_ping_ms = _ping_us * 0.001f;
				PACKETLOG(LOG("received packet " << _packet.ack << " | " << pack.second << " -> " << end << ", packet's ping is " << ping << "us. Smoothed ping is " << _ping_us));
				_pending_packets.remove(pack);
				break ;
			}
		}
		std::list<std::pair<uint16_t, int64_t>>::iterator it = _pending_packets.begin();
		PENDINGLOG(LOG("still in pending packets: "));
		while (it != _pending_packets.end()) {
			int diff = diff_wrap_around(_packet.ack, it->first);
			if (diff > 32) { // packet lost
				it = _pending_packets.erase(it);
				PENDINGLOG(LOG("<LOSS diff " << diff << ", ack " << _packet.ack << " it " << it->first  << '>'));
				++_lost;
			} else if (diff < 0) {
				PENDINGLOG(LOG(it->first << ' '));
				++it;
			} else if (_packet.ack_bitfield & (1 << (diff - 1))) { // paquet has been aknoledged
				PENDINGLOG(LOG("<diff " << diff << ", ack " << _packet.ack << " bitfield rmed " << it->first << '>'));
				it = _pending_packets.erase(it);
			} else {
				PENDINGLOG(LOG(it->first << ' '));
				++it;
			}
		}
		PENDINGLOG(LOG(std::endl << "packet lost: " << _lost << ", pending " << _pending_packets.size() << ", packet sent: " << _sent));
	} else if (_type == sockets::server) {
		// update ack_bitfield for client's packet loss
		int diff = diff_wrap_around(_packet.sequence, src->ack);
		src->bitfield <<= diff;
		PENDINGLOG(LOG("diff is " << diff));
		src->bitfield |= (1 << (diff - 1));
		PENDINGLOG(LOG("bitfield is " << src->bitfield));
	}

	src->ack = _packet.sequence;
	_packet.data.data[bytes - sizeof(unsigned short)] = '\0';
	memmove(data, &_packet.data.action, bytes + 1); // copy the \0 too
	PACKETLOG(LOG("received packet with header " << _packet.protocol_id << " - " << _packet.sequence << " - " << _packet.ack << " - " << _packet.ack_bitfield));
	return (bytes);
}

void Socket::rmClient( int id )
{
	auto search = std::find_if(_clients.begin(), _clients.end(), [id](auto& client) { return (client.id == id); });
	if (search != _clients.end()) {
		_clients.erase(search);
	}
}

int Socket::getType( void )
{
	return (_type);
}

int Socket::getId( Address& target )
{
	for (auto &c : _clients) {
		if (c.ip == target) {
			return (c.id);
		}
	}
	return (-1);
}

Address* Socket::getAddress( int id )
{
	for (auto &c : _clients) {
		if (c.id == id) {
			return (&c.ip);
		}
	}
	return (NULL);
}

Address& Socket::getServerAddress( void )
{
	return (_server_ip);
}

// return ping in ms
int Socket::getPing( void )
{
	return (_ping_ms);
}

unsigned Socket::getPacketLost( void )
{
	return (_lost);
}

unsigned Socket::getPacketSent( void )
{
	return (_sent);
}
