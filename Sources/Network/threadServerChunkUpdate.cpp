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
	std::vector<glm::ivec2> playersPos;
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
		server->getPlayersChunkPos(playersPos);
		// render_dist = settings->getInt(settings::ints::render_dist);
		server->setThreadUpdate(false);

		std::set<std::pair<int, int>> coords;
		for (auto pos : playersPos) {
			for (int row = -render_dist; row <= render_dist; row++) {
				for (int col = -render_dist; col <= render_dist; col++) {
					coords.insert({pos.x + (row << settings::consts::chunk_shift), pos.y + (col << settings::consts::chunk_shift)});
				}
			}
		}

		// std::vector<std::shared_ptr<Chunk>> newperi_chunks;
		// newperi_chunks.reserve(render->_perimeter_chunks.capacity());
		auto ite = server->_chunks.end();
		auto it = server->_chunks.begin();
		for (; it != ite;) {
			int index = 0;
			for (auto pos : playersPos) {
				if ((*it)->inPerimeter(pos.x, pos.y, render_dist << settings::consts::chunk_shift)) {
					(*it)->checkFillVerticesServer();
					// newperi_chunks.push_back(*it);
					packet.id = index;
					// for (int subChunk = 0; subChunk < 16; ++subChunk) {
					// 	(*it)->serializeSubChunk(packet, subChunk);
					// 	server->pendPacket(packet);
					// }
					(*it)->serializeChunk(packet);
					server->pendPacket(packet);
					coords.erase({(*it)->getStartX(), (*it)->getStartY()});
				} /*else if (!(*it)->inPerimeter(pos.x, pos.y, (render_dist << settings::consts::chunk_shift) * 2)) {
					auto tmp = it;
					--it;
					(*tmp)->setBackup(render->_backups);
					mtx_deleted_chunks.lock();
					render->_deleted_chunks.push_back(*tmp); // must be deleted from main thread because of openGL
					mtx_deleted_chunks.unlock();
					mtx.lock();
					render->_chunks.erase(tmp);
					mtx.unlock();
				}*/
			}
			++it;
			++index;
		}
		// mtx_perimeter.lock();
		// render->_perimeter_chunks = newperi_chunks;
		// mtx_perimeter.unlock();

		for (auto& c: coords) {
			// create new chunk where at least one player stands
			server->setThreadUpdate(true);
			std::shared_ptr<Chunk> newChunk = std::make_shared<Chunk>(nullptr, nullptr, c.first, c.second, server->_chunks);
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

void Server::getPlayersChunkPos( std::vector<glm::ivec2>& pos )
{
	pos.clear();
	_mtx_plrs.lock();
	for (auto& player : _players) {
		glm::vec3 ppos = player->getPos();
		pos.push_back({utils::math::chunk_pos(ppos.x), utils::math::chunk_pos(ppos.y)});
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
