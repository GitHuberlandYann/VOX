#include "OpenGL_Manager.hpp"
#include "Player.hpp"
#include "Server.hpp"
#include "Settings.hpp"
#include "logs.hpp"
#include <chrono>
#include <thread>
#include <set>

static void thread_server_chunk_update( Server *server )
{
	MAINLOG(LOG("thread_server_chunk_update started"));
	std::vector<glm::ivec3> playersPos; // X Y id
	t_pending_packet packet;
	int render_dist = 10;
	// Settings* settings = Settings::Get();

	while (!server->getThreadUpdate()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		if (server->getThreadStop()) {
			MAINLOG(LOG("thread_server_chunk_update stopped before loop"));
			return ;
		}
	}
	while (true) {
		server->setThreadUpdate(false);
		server->getPlayersChunkPos(playersPos);
		// render_dist = settings->getInt(settings::ints::render_dist);

		std::set<std::pair<int, int>> coords;
		for (auto pos : playersPos) {
			for (int row = -render_dist; row <= render_dist; row++) {
				for (int col = -render_dist; col <= render_dist; col++) {
					coords.insert({pos.x + (row << settings::consts::chunk_shift), pos.y + (col << settings::consts::chunk_shift)});
				}
			}
		}

		auto ite = server->_chunks.end();
		auto it = server->_chunks.begin();
		for (; it != ite; ++it) {
			bool delChunk = true;
			for (auto pos : playersPos) {
				if ((*it)->inPerimeter(pos.x, pos.y, render_dist << settings::consts::chunk_shift)) {
					delChunk = false;
					(*it)->checkFillVerticesServer();
					if (!server->isLoadedForPlayer(pos.z, (*it)->getStartVec())) {
						packet.id = pos.z;
						(*it)->serializeChunk(packet); // TODO save serialized chunk to send to next player who needs it
						server->pendPacket(packet);
					}
					coords.erase({(*it)->getStartX(), (*it)->getStartY()});
				} else if ((*it)->inPerimeter(pos.x, pos.y, (render_dist << settings::consts::chunk_shift) * 2)) {
					delChunk = false;
				} else {
					server->unloadForPlayer(pos.z, (*it)->getStartVec());
				}
			}
			if (delChunk) { // no player within 2 * render_dist of chunk
				auto tmp = it;
				--it;
				(*tmp)->setBackup(server->_backups);
				mtx_deleted_chunks.lock();
				server->_deleted_chunks.push_back(*tmp); // must be deleted from main thread because of openGL
				mtx_deleted_chunks.unlock();
				mtx.lock();
				server->_chunks.erase(tmp);
				mtx.unlock();
			}
		}

		for (auto& c: coords) {
			// create new chunk where at least one player stands
			server->setThreadUpdate(true);
			std::shared_ptr<Chunk> newChunk = std::make_shared<Chunk>(nullptr, c.first, c.second, server->_chunks);
			mtx_backup.lock();
			std::map<std::pair<int, int>, s_backup>::iterator search = server->_backups.find(std::pair<int, int>(c.first, c.second));
			if (search != server->_backups.end()) {
				newChunk->restoreBackup(search->second);
				server->_backups.erase(search);
			}
			mtx_backup.unlock();
			mtx.lock();
			server->_chunks.push_back(newChunk);
			mtx.unlock();
			newChunk->generate_chunk();
		}

		while (!server->getThreadUpdate()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			if (server->getThreadStop()) {
				MAINLOG(LOG("thread_server_chunk_update stopped"));
				return ;
			}
		}
	}
}

// ************************************************************************** //
//                                  server                                    //
// ************************************************************************** //

void Server::startThread( void )
{
	if (!_thread.joinable()) {
		_mtx.lock();
		_threadStop = false;
		_mtx.unlock();
		_thread = std::thread(thread_server_chunk_update, this);
	}
}

void Server::stopThread( void )
{
	_mtx.lock();
	_threadUpdate = false;
	_threadStop = true;
	_mtx.unlock();
	if (_thread.joinable()) {
		_thread.join();
	}
}

void Server::getPlayersChunkPos( std::vector<glm::ivec3>& pos )
{
	pos.clear();
	_mtx_plrs.lock();
	for (auto& player : _players) {
		pos.push_back({player.second->getChunkPos(), player.first});
	}
	_mtx_plrs.unlock();
}

bool Player::isLoaded( glm::ivec2 chunkPos )
{
	_mtx.lock();
	auto search = _loadedChunks.find({chunkPos.x, chunkPos.y});
	if (search == _loadedChunks.end()) {
		_loadedChunks.insert({chunkPos.x, chunkPos.y});
		_mtx.unlock();
		return (false);
	}
	_mtx.unlock();
	return (true);
}

/**
 * @brief returns whether player with given id has already loaded chunk at chunkPos
 * if no player found with given id, return true
 */
bool Server::isLoadedForPlayer( int playerId, glm::ivec2 chunkPos )
{
	bool res = true;

	_mtx_plrs.lock();
	if (_players.count(playerId)) {
		res = _players[playerId]->isLoaded(chunkPos);
	}
	_mtx_plrs.unlock();

	return (res);
}

/**
 * @return if chunk was loaded in the first place
 */
bool Player::unloadChunk( glm::ivec2 chunkPos )
{
	_mtx.lock();
	bool res = _loadedChunks.erase({chunkPos.x, chunkPos.y});
	_mtx.unlock();
	return (res);
}

void Server::unloadForPlayer( int playerId, glm::ivec2 chunkPos )
{
	_mtx_plrs.lock();
	if (_players.count(playerId)) {
		if (_players[playerId]->unloadChunk(chunkPos)) {
			t_pending_packet packet = {playerId};
			packet.packet.action = packet_id::server::unload_chunk;
			packet.size = 0;
			utils::memory::memwrite(packet.packet.data, &chunkPos.x, sizeof(int), packet.size);
			utils::memory::memwrite(packet.packet.data, &chunkPos.y, sizeof(int), packet.size);
			packet.size += sizeof(unsigned short);
			pendPacket(packet);
		}
	}
	_mtx_plrs.unlock();
}

void Server::setThreadUpdate( bool state )
{
	_mtx.lock();
	_threadUpdate = state;
	_mtx.unlock();
}

bool Server::getThreadUpdate( void )
{
	_mtx.lock();
	bool res = _threadUpdate;
	_mtx.unlock();
	return (res);
}

bool Server::getThreadStop( void )
{
	_mtx.lock();
	bool res = _threadStop;
	_mtx.unlock();
	return (res);
}
