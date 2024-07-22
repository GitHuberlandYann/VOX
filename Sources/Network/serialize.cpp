#include "OpenGL_Manager.hpp"
#include "Socket.hpp"
#include "Server.hpp"
#include "logs.hpp"
#include "zlib.h"

// ************************************************************************** //
//                                Palette                                     //
// ************************************************************************** //

int t_palette::idFromValue( int value )
{
	if (valueToId.count(value) == 0) {
		int id = valueToId.size();
		valueToId[value] = id;
		return (id);
	}
	return (valueToId[value]);
}

// ************************************************************************** //
//                               Serialize                                    //
// ************************************************************************** //

// gen dics used to serialize subChunk
size_t Chunk::paletteSubChunk( t_palette& palette, int index )
{
	for (int row = 0; row < settings::consts::chunk_size; row++) {
		for (int col = 0; col < settings::consts::chunk_size; col++) {
			for (int level = (index << settings::consts::chunk_shift); level < ((index + 1) << settings::consts::chunk_shift); level++) {
				int value = _blocks[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level];
				int id = palette.idFromValue(value);
				palette.idToValue[id] = value;
			}
		}
	}
	size_t res = palette.idToValue.size();
	assert((!res || res == palette.valueToId.size()));
	return (res);
}

/**
 * @brief called by server to write 16x16x256 Chunk content inside packet.
 * @param packet packet sent to client
 */
void Chunk::serializeChunk( t_pending_packet& packet )
{
	packet.packet.action = packet_id::server::chunk_data;
	waitGenDone(); // make sure chunk finished generating

	char uncompressed[settings::consts::network::uncompressed_packet_size_limit];
	size_t uncompressedSize = 0;
	utils::memory::memwrite(uncompressed, &_startX, sizeof(GLint), uncompressedSize);
	utils::memory::memwrite(uncompressed, &_startY, sizeof(GLint), uncompressedSize);

	for (int index = 0; index < 16; ++index) {
		t_palette palette;
		size_t nbIds = paletteSubChunk(palette, index);
		utils::memory::memwrite(uncompressed, &nbIds, sizeof(int), uncompressedSize); // writing palette size

		if (nbIds == 1) {
			int value = palette.idToValue[0];
			utils::memory::memwrite(uncompressed, &value, sizeof(int), uncompressedSize);
			short light = _lights[(((8 << settings::consts::chunk_shift) + 8) << settings::consts::world_shift) + (index << settings::consts::chunk_shift) + 8];
			char lightByte = ((light & 0xF) | ((light >> 4) & 0xF0));
			utils::memory::memwrite(uncompressed, &lightByte, sizeof(char), uncompressedSize);
			continue ;
		}

		for (auto& pair : palette.idToValue) { // writing palette content
			utils::memory::memwrite(uncompressed, &pair.second, sizeof(int), uncompressedSize);
		}

		int bits = (nbIds < 257) ? sizeof(char) : sizeof(short);
		for (int row = 0; row < settings::consts::chunk_size; row++) {
			for (int col = 0; col < settings::consts::chunk_size; col++) {
				for (int level = (index << settings::consts::chunk_shift); level < ((index + 1) << settings::consts::chunk_shift); level++) {
					int value = _blocks[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level];
					int id = palette.valueToId[value];
					utils::memory::memwrite(uncompressed, &id, bits, uncompressedSize); // writing block ids
				}
			}
		}

		for (int row = 0; row < settings::consts::chunk_size; row++) {
			for (int col = 0; col < settings::consts::chunk_size; col++) {
				for (int level = (index << settings::consts::chunk_shift); level < ((index + 1) << settings::consts::chunk_shift); level++) {
					short light = _lights[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level];
					char lightByte = ((light & 0xF) | ((light >> 4) & 0xF0));
					utils::memory::memwrite(uncompressed, &lightByte, sizeof(char), uncompressedSize); // writing light levels
				}
			}
		}
	}
	SERIALLOG(LOG("serializeChunk ended at " << uncompressedSize << " bytes."));

	packet.size = settings::consts::network::packet_size_limit;
	compress((Byte*)&packet.packet.data[sizeof(size_t)], &packet.size, (const Byte*)uncompressed, uncompressedSize);
	size_t dataSize = 0;
	utils::memory::memwrite(packet.packet.data, &packet.size, sizeof(size_t), dataSize); // writing compressed size // TODO might prefer to encode as int
	SERVLOG(LOGERROR("uncompressed " << uncompressedSize << " -> compressed " << packet.size));
	packet.size += dataSize;
}

// ************************************************************************** //
//                              Deserialize                                   //
// ************************************************************************** //

/**
 * @brief called by client to read 16x16x256 Chunk content from packet.
 * @param packet packet received from server
 */
void Chunk::deserializeChunk( char* packet )
{
	_genDone = true; // TODO change this temp mesure
	size_t packetOffset = sizeof(GLint) + sizeof(GLint);
	
	for (int index = 0; index < 16; ++index) {
		t_palette palette;
		int nbIds;
		utils::memory::memread(&nbIds, packet, sizeof(int), packetOffset);
		SERIALLOG(LOG("deserializeChunk index is " << index << ", nbIds " << nbIds));

		if (nbIds == 1) {
			int value;
			utils::memory::memread(&value, packet, sizeof(int), packetOffset);
			for (int row = 0; row < settings::consts::chunk_size; row++) {
				for (int col = 0; col < settings::consts::chunk_size; col++) {
					for (int level = (index << settings::consts::chunk_shift); level < ((index + 1) << settings::consts::chunk_shift); level++) {
						_blocks[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level] = value;
					}
				}
			}
			char lightLevel;
			utils::memory::memread(&lightLevel, packet, sizeof(char), packetOffset);
			short light = (lightLevel & 0xF) | ((lightLevel & 0xF0) << 4);
			for (int row = 0; row < settings::consts::chunk_size; row++) {
				for (int col = 0; col < settings::consts::chunk_size; col++) {
					for (int level = (index << settings::consts::chunk_shift); level < ((index + 1) << settings::consts::chunk_shift); level++) {
						_lights[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level] = light;
					}
				}
			}
			continue ;
		}

		for (int i = 0; i < nbIds; ++i) { // read palette content
			int value;
			utils::memory::memread(&value, packet, sizeof(int), packetOffset);
			palette.idToValue[i] = value;
		}

		int bits = (nbIds < 257) ? sizeof(char) : sizeof(short);
		for (int row = 0; row < settings::consts::chunk_size; row++) {
			for (int col = 0; col < settings::consts::chunk_size; col++) {
				for (int level = (index << settings::consts::chunk_shift); level < ((index + 1) << settings::consts::chunk_shift); level++) {
					int id = 0;
					utils::memory::memread(&id, packet, bits, packetOffset); // reading block id
					int value = palette.idToValue[id];
					_blocks[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level] = value;
				}
			}
		}

		for (int row = 0; row < settings::consts::chunk_size; row++) {
			for (int col = 0; col < settings::consts::chunk_size; col++) {
				for (int level = (index << settings::consts::chunk_shift); level < ((index + 1) << settings::consts::chunk_shift); level++) {
					char lightLevel;
					utils::memory::memread(&lightLevel, packet, sizeof(char), packetOffset);
					short light = (lightLevel & 0xF) | ((lightLevel & 0xF0) << 4);
					_lights[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level] = light;
				}
			}
		}
	}
}

// ************************************************************************** //
//                               Obsolete                                     //
// ************************************************************************** //

/**
 * @brief called by server to write 16x16x16 subChunk content inside packet.
 * @param packet packet sent to client
 * @param index index of subChunk we want to serialize [0:15]
 */
void Chunk::serializeSubChunk( t_pending_packet& packet, int index )
{
	assert((index >= 0 && index < 16));
	waitGenDone(); // make sure chunk finished generating

	packet.packet.action = packet_id::server::chunk_data;
	packet.size = 0;
	utils::memory::memwrite(packet.packet.data, &_startX, sizeof(GLint), packet.size);
	utils::memory::memwrite(packet.packet.data, &_startY, sizeof(GLint), packet.size);
	utils::memory::memwrite(packet.packet.data, &index, sizeof(char), packet.size);

	t_palette palette;
	size_t nbIds = paletteSubChunk(palette, index);
	utils::memory::memwrite(packet.packet.data, &nbIds, sizeof(int), packet.size); // writing palette size

	if (nbIds == 1) {
		int value = palette.idToValue[0];
		utils::memory::memwrite(packet.packet.data, &value, sizeof(int), packet.size);
		SERIALLOG(LOG("serializeSubChunk ended at " << packet.size << " bytes."));
		return ;
	}

	for (auto& pair : palette.idToValue) { // writing palette content
		utils::memory::memwrite(packet.packet.data, &pair.second, sizeof(int), packet.size);
	}

	int bits = (nbIds < 257) ? sizeof(char) : sizeof(short);
	for (int row = 0; row < settings::consts::chunk_size; row++) {
		for (int col = 0; col < settings::consts::chunk_size; col++) {
			for (int level = (index << settings::consts::chunk_shift); level < ((index + 1) << settings::consts::chunk_shift); level++) {
				int value = _blocks[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level];
				int id = palette.valueToId[value];
				utils::memory::memwrite(packet.packet.data, &id, bits, packet.size); // writing block ids
			}
		}
	}
	SERIALLOG(LOG("serializeSubChunk ended at " << packet.size << " bytes."));
}

/**
 * @brief called by client to read 16x16x16 subChunk content from packet.
 * @param packet packet received from server
 */
void Chunk::deserializeSubChunk( t_packet_data& packet )
{
	_genDone = true; // TODO change this temp mesure
	size_t packetOffset = sizeof(GLint) + sizeof(GLint);
	int index = 0;
	utils::memory::memread(&index, packet.data, sizeof(char), packetOffset);

	if (!index) { // first call to deserialize
		_blocks.fill(blocks::air);
	}
	
	t_palette palette;
	int nbIds;
	utils::memory::memread(&nbIds, packet.data, sizeof(int), packetOffset);
	SERIALLOG(LOG("deserializeSubChunk index is " << index << ", nbIds " << nbIds));

	if (nbIds == 1) {
		int value;
		utils::memory::memread(&value, packet.data, sizeof(int), packetOffset);
		for (int row = 0; row < settings::consts::chunk_size; row++) {
			for (int col = 0; col < settings::consts::chunk_size; col++) {
				for (int level = (index << settings::consts::chunk_shift); level < ((index + 1) << settings::consts::chunk_shift); level++) {
					_blocks[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level] = value;
				}
			}
		}
		return ;
	}

	for (int i = 0; i < nbIds; ++i) { // read palette content
		int value;
		utils::memory::memread(&value, packet.data, sizeof(int), packetOffset);
		palette.idToValue[i] = value;
	}

	int bits = (nbIds < 257) ? sizeof(char) : sizeof(short);
	for (int row = 0; row < settings::consts::chunk_size; row++) {
		for (int col = 0; col < settings::consts::chunk_size; col++) {
			for (int level = (index << settings::consts::chunk_shift); level < ((index + 1) << settings::consts::chunk_shift); level++) {
				int id = 0;
				utils::memory::memread(&id, packet.data, bits, packetOffset); // reading block id
				int value = palette.idToValue[id];
				_blocks[(((row << settings::consts::chunk_shift) + col) << settings::consts::world_shift) + level] = value;
			}
		}
	}
}

/*
// ************************************************************************** //
//                                 Debug                                      //
// ************************************************************************** //

std::mutex mtx, mtx_perimeter, mtx_deleted_chunks, mtx_backup;
siv::PerlinNoise::seed_type perlin_seed = 123456u;
int WIN_WIDTH = 0, WIN_HEIGHT = 0;

int main( void )
{
	std::list<std::shared_ptr<Chunk>> chunks;
	Chunk c0(NULL, NULL, 16, 48, chunks);
	Chunk c1(NULL, NULL, 16, 48, chunks);
	t_pending_packet packet;

	c0.generate_chunk();
	for (int index = 0; index < 16; ++index) {
		c0.serializeSubChunk(packet, index);
		c1.deserializeSubChunk(packet.packet);
	}
	for (int row = 0; row < settings::consts::chunk_size; row++) {
		for (int col = 0; col < settings::consts::chunk_size; col++) {
			for (int level = 0; level < settings::consts::world_height; level++) {
				int value0 = c0.getBlockAt(row, col, level, false);
				int value1 = c1.getBlockAt(row, col, level, false);
				if (value0 != value1) {
					LOGERROR("DIFF " << value0 << " vs " << value1);
				}
			}
		}
	}

	std::string hello = "hello world!";
	char compressed[1000];
	char dst[1000];
	size_t dstLen = 1000;

	compress((Byte*)compressed, &dstLen, (const Byte*)hello.c_str(), 13);
	LOG("compressed len: " << dstLen << " vs srcLen: " << 13);
	uncompress((Byte*)dst, &dstLen, (const Byte*)compressed, dstLen);
	LOG("uncompressed len: " << dstLen);
	LOG("src " << hello << " -> " << dst);
	return (0);
} // */
