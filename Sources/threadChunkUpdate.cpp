#include "OpenGL_Manager.hpp"
#include "Settings.hpp"
#include <chrono>
#include <thread>

void thread_chunk_update( OpenGL_Manager *render )
{
	std::cout << "thread_chunk_update started" << std::endl;
	glm::ivec2 pos;
	int render_dist;
	Settings *settings = Settings::Get();

	while (!render->getThreadUpdate()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		if (render->getThreadStop()) {
			std::cout << "thread_chunk_update stopped before loop" << std::endl;
			return ;
		}
	}
	while (true) {
		// std::cout << "thread chunk update" << std::endl;
		pos = render->getCurrentChunk();
		render_dist = settings->getInt(SETTINGS::RENDER_DIST);
		render->setThreadUpdate(false);
		// Bench b;
		std::set<std::pair<int, int>> coords;
		for (int row = -render_dist; row <= render_dist; row++) {
			for (int col = -render_dist; col <= render_dist; col++) {
				coords.insert({pos.x + (row << CHUNK_SHIFT), pos.y + (col << CHUNK_SHIFT)});
			}
		}
		// b.stamp("gen coordinates set");

		std::vector<Chunk *> newperi_chunks;
		newperi_chunks.reserve(render->_perimeter_chunks.capacity());
		std::vector<Chunk *> newdel_chunks;
		newdel_chunks.reserve(render->_deleted_chunks.capacity());
		std::list<Chunk *>::iterator ite = render->_chunks.end();
		std::list<Chunk *>::iterator it = render->_chunks.begin();
		// std::cout << "IN THREAD UPDATE, nb chunks: " << render->_chunks.size() << std::endl;
		for (; it != ite;) {
			if ((*it)->inPerimeter(pos.x, pos.y, render_dist << CHUNK_SHIFT)) {
				// std::cout << "IN PERIMETER" << std::endl;
				(*it)->checkFillVertices();
				newperi_chunks.push_back(*it);
				coords.erase({(*it)->getStartX(), (*it)->getStartY()});
			} else if (!(*it)->inPerimeter(pos.x, pos.y, (render_dist << CHUNK_SHIFT) * 2)) {
				std::list<Chunk *>::iterator tmp = it;
				--it;
				(*tmp)->setBackup(render->_backups);
				newdel_chunks.push_back(*tmp);
				mtx.lock();
				render->_chunks.erase(tmp);
				mtx.unlock();
			}
			++it;
		}
		// b.stamp("delperi");
		sort_chunks(render->_camera->getPos(), newperi_chunks);
		// b.stamp("sort chunks");
		mtx_perimeter.lock();
		render->_perimeter_chunks = newperi_chunks;
		mtx_perimeter.unlock();
		mtx_deleted_chunks.lock();
		render->_deleted_chunks = newdel_chunks;
		mtx_deleted_chunks.unlock();

		// b.stamp("NO");
		for (auto& c: coords) {
			//create new chunk where player stands
			Chunk *newChunk = new Chunk(render->_camera, render->_inventory, c.first, c.second, &render->_chunks);
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
		}
		// b.stamp("loop and create new chunks");
		// b.stop("chunk update");
		// std::cout << "for now " << count << " new chunks, computed " << coords.size() << std::endl;

		while (!render->getThreadUpdate()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			if (render->getThreadStop()) {
				std::cout << "thread_chunk_update stopped" << std::endl;
				return ;
			}
		}
	}
}

// ************************************************************************** //
//                             OpenGL-Manager                                 //
// ************************************************************************** //

void OpenGL_Manager::startThread( void )
{
	if (!_thread.joinable()) {
		_mtx.lock();
		_threadStop = false;
		_mtx.unlock();
		_thread = std::thread(thread_chunk_update, this);
	}
}

void OpenGL_Manager::stopThread( void )
{
	_mtx.lock();
	_threadUpdate = false;
	_threadStop = true;
	_mtx.unlock();
	if (_thread.joinable()) {
		_thread.join();
	}
}

glm::ivec2 OpenGL_Manager::getCurrentChunk( void )
{
	_mtx.lock();
	glm::ivec2 res = _current_chunk;
	_mtx.unlock();
	return (res);
}

void OpenGL_Manager::setThreadUpdate( bool state )
{
	_mtx.lock();
	_threadUpdate = state;
	_mtx.unlock();
}

bool OpenGL_Manager::getThreadUpdate( void )
{
	_mtx.lock();
	bool res = _threadUpdate;
	_mtx.unlock();
	return (res);
}

bool OpenGL_Manager::getThreadStop( void )
{
	_mtx.lock();
	bool res = _threadStop;
	_mtx.unlock();
	return (res);
}
