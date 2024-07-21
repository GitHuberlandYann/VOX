#include "OpenGL_Manager.hpp"
#include "Player.hpp"
#include "Server.hpp"
#include "Settings.hpp"
#include "logs.hpp"
#include "zlib.h"
#include <chrono>
#include <thread>
#include <set>

static void thread_client_chunk_update( OpenGL_Manager *render )
{
	MAINLOG(LOG("thread_client_chunk_update started"));
	std::vector<t_packet_data> packetDatas;

	while (!render->getThreadUpdate()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		if (render->getThreadStop()) {
			MAINLOG(LOG("thread_client_chunk_update stopped before loop"));
			return ;
		}
	}
	while (true) {
		LOG("thread_client_chunk_update loop start");
		render->setThreadUpdate(false);

		std::vector<std::shared_ptr<Chunk>> newperi_chunks;
		newperi_chunks.reserve(render->_perimeter_chunks.capacity());
		auto ite = render->_chunks.end();
		auto it = render->_chunks.begin();
		for (; it != ite;) {
			// if ((*it)->inPerimeter(pos.x, pos.y, render_dist << settings::consts::chunk_shift)) {
				(*it)->checkFillVertices(); // TODO add arg to checkFillVertices to avoid fill_vertex_array on server and light spread on client
				newperi_chunks.push_back(*it);
				// coords.erase({(*it)->getStartX(), (*it)->getStartY()});
			/*/ } else if (!(*it)->inPerimeter(pos.x, pos.y, (render_dist << settings::consts::chunk_shift) * 2)) {
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
			++it;
		}
		utils::math::sort_chunks(render->_player->getPos(), newperi_chunks);
		mtx_perimeter.lock();
		render->_perimeter_chunks = newperi_chunks;
		mtx_perimeter.unlock();

		packetDatas = render->getPacketDatas();
		for (auto& packet: packetDatas) {
			// create or fill chunk from server's info
			render->setThreadUpdate(true);
			size_t srcOffset = 0, uncompressedSize = settings::consts::network::packet_size_limit, compressedSize = 0;
			utils::memory::memread(&compressedSize, packet.data, sizeof(size_t), srcOffset);

			t_packet_data uncompressed;
			uncompress((Byte*)uncompressed.data, &uncompressedSize, (Byte*)&packet.data[srcOffset], compressedSize);
			LOG("uncompressing packet: " << compressedSize << " -> " << uncompressedSize);
			
			srcOffset = 0;
			int startX = 0, startY = 0;
			utils::memory::memread(&startX, uncompressed.data, sizeof(GLint), srcOffset);
			utils::memory::memread(&startY, uncompressed.data, sizeof(GLint), srcOffset);

			auto search = std::find_if(render->_chunks.begin(), render->_chunks.end(), [startX, startY](auto& chunk) { return (chunk->getStartX() == startX && chunk->getStartY() == startY); });
			if (search == render->_chunks.end()) {
				std::shared_ptr<Chunk> newChunk = std::make_shared<Chunk>(render->_player.get(), render->_inventory.get(), startX, startY, render->_chunks);
				newChunk->deserializeChunk(uncompressed);

				mtx.lock();
				render->_chunks.push_back(newChunk);
				mtx.unlock();
			} else {
				(*search)->deserializeChunk(uncompressed);
			}
			
		}

		LOG("thread_client_chunk_update loop end");
		while (!render->getThreadUpdate()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			if (render->getThreadStop()) {
				MAINLOG(LOG("thread_client_chunk_update stopped"));
				return ;
			}
		}
	}
}

// ************************************************************************** //
//                             OpenGL_Manager                                 //
// ************************************************************************** //

void OpenGL_Manager::startClientThread( void )
{
	if (!_thread.joinable()) {
		_mtx.lock();
		_threadStop = false;
		_mtx.unlock();
		_thread = std::thread(thread_client_chunk_update, this);
	}
}

std::vector<t_packet_data> OpenGL_Manager::getPacketDatas( void ) // TODO change this abomination of a function
{
	std::vector<t_packet_data> res;
	_mtx_packets.lock();
	res = _chunkPackets;
	_chunkPackets.clear();
	_mtx_packets.unlock();
	return (res);
}
