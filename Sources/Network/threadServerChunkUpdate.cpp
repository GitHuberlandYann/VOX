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
	/*glm::ivec2 pos;
	int render_dist;
	Settings* settings = Settings::Get();*/

	while (!server->getThreadUpdate()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		if (server->getThreadStop()) {
			MAINLOG(LOG("thread_server_chunk_update stopped before loop"));
			return ;
		}
	}
	while (true) {
		/*pos = render->getCurrentChunkPos();
		render_dist = settings->getInt(settings::ints::render_dist);
		render->setThreadUpdate(false);

		std::set<std::pair<int, int>> coords;
		for (int row = -render_dist; row <= render_dist; row++) {
			for (int col = -render_dist; col <= render_dist; col++) {
				coords.insert({pos.x + (row << settings::consts::chunk_shift), pos.y + (col << settings::consts::chunk_shift)});
			}
		}

		std::vector<std::shared_ptr<Chunk>> newperi_chunks;
		newperi_chunks.reserve(render->_perimeter_chunks.capacity());
		auto ite = render->_chunks.end();
		auto it = render->_chunks.begin();
		for (; it != ite;) {
			if ((*it)->inPerimeter(pos.x, pos.y, render_dist << settings::consts::chunk_shift)) {
				(*it)->checkFillVertices();
				newperi_chunks.push_back(*it);
				coords.erase({(*it)->getStartX(), (*it)->getStartY()});
			} else if (!(*it)->inPerimeter(pos.x, pos.y, (render_dist << settings::consts::chunk_shift) * 2)) {
				auto tmp = it;
				--it;
				(*tmp)->setBackup(render->_backups);
				mtx_deleted_chunks.lock();
				render->_deleted_chunks.push_back(*tmp); // must be deleted from main thread because of openGL
				mtx_deleted_chunks.unlock();
				mtx.lock();
				render->_chunks.erase(tmp);
				mtx.unlock();
			}
			++it;
		}
		utils::math::sort_chunks(render->_player->getPos(), newperi_chunks);
		mtx_perimeter.lock();
		render->_perimeter_chunks = newperi_chunks;
		mtx_perimeter.unlock();

		for (auto& c: coords) {
			//create new chunk where player stands
			std::shared_ptr<Chunk> newChunk = std::make_shared<Chunk>(render->_player.get(), render->_inventory.get(), c.first, c.second, render->_chunks);
			mtx_backup.lock();
			std::map<std::pair<int, int>, s_backup>::iterator search = render->_backups.find(std::pair<int, int>(c.first, c.second));
			if (search != render->_backups.end()) {
				newChunk->restoreBackup(search->second);
				render->_backups.erase(search);
			}
			mtx_backup.unlock();
			mtx.lock();
			render->_chunks.push_back(newChunk);
			mtx.unlock();
			newChunk->generate_chunk(); // TODO remove this from thread because it launches its own thread and there's data races..
		}*/

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
//                             OpenGL-Manager                                 //
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

// glm::ivec2 Server::getCurrentChunkPos( void )
// {
// 	_mtx.lock();
// 	glm::ivec2 res = _current_chunk;
// 	_mtx.unlock();
// 	return (res);
// }

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
