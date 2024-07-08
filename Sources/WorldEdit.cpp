#include "utils.h"
#include "WorldEdit.hpp"
#include "logs.hpp"
#include <cassert>

WorldEdit::WorldEdit( void )
	: _running(false), _brushSize(4), _selectStart(1000), _selectEnd(1000), _openGL_Manager(NULL), _inventory(NULL), _chat(NULL)
{

}

WorldEdit::~WorldEdit( void )
{
	MAINLOG(LOG("Destructor of WorldEdit called"));
}

// ************************************************************************** //
//                             Get / Destroy                                  //
// ************************************************************************** //

WorldEdit* WorldEdit::_worldEditInstance{NULL};
std::mutex WorldEdit::_mtx;

WorldEdit *WorldEdit::Get( void )
{
	std::lock_guard<std::mutex> lock(_mtx); // multithread safety to not create several instances of WorldEdit
	if (_worldEditInstance == NULL) {
		_worldEditInstance = new WorldEdit();
	}
	return (_worldEditInstance);
}

void WorldEdit::Destroy( void )
{
	std::lock_guard<std::mutex> lock(_mtx); // multithread safety to not create several instances of WorldEdit
	delete _worldEditInstance;
	_worldEditInstance = NULL;
}

// ************************************************************************** //
//                                 Sets                                       //
// ************************************************************************** //

/**
 * @brief sets respective pointers used by WorldEdit instance
*/
void WorldEdit::setPtrs( OpenGL_Manager* openGL_Manager, Inventory* inventory, Chat* chat )
{
	_openGL_Manager = openGL_Manager;
	_inventory = inventory;
	_chat = chat;
}

/**
 * @brief set _selectStart used by WordEdit commands to pos
 * @param pos world pos of block selected
*/
void WorldEdit::setSelectionStart( glm::ivec3 pos )
{
	_selectStart = pos;
	_clipEnd = _clipEnd - _clipStart + pos;
	_clipStart = pos;
	_absoluteClipboard = false;
	_chat->chatMessage("Selection Start set to " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z), argb::green);
}

/**
 * @brief set _selectEnd used by WordEdit commands to pos
 * @param pos world pos of block selected
*/
void WorldEdit::setSelectionEnd( glm::ivec3 pos )
{
	_selectEnd = pos;
	_clipStart = _clipStart - _clipEnd + pos;
	_clipEnd = pos;
	_absoluteClipboard = false;
	_chat->chatMessage("Selection End set to " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z), argb::red);
}

/**
 * @brief generate sphere at given pos filled with blocks or with air
 */
void WorldEdit::useBrush( glm::ivec3 pos, bool adding )
{
	Chunk * chunk = _openGL_Manager->getCurrentChunkPtr();
	glm::ivec3 delta;
	for (delta.x = -_brushSize; delta.x < _brushSize + 1; ++delta.x) {
		for (delta.y = -_brushSize; delta.y < _brushSize + 1; ++delta.y) {
			for (delta.z = -_brushSize; delta.z < _brushSize + 1; ++delta.z) {
				float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
				if (dist - .1f > _brushSize) { // || dist < _brushSize - 1) {
					continue ;
				}
				int previous = TYPE(chunk->getBlockAtAbsolute(pos + delta));
				if (adding && previous == blocks::air) {
					chunk->setLightLevelAbsolute(0, pos + delta, true);
					chunk->setBlockAtAbsolute(blocks::stone, pos + delta, dist + .9f > _brushSize);
				} else if (!adding && previous != blocks::air && previous != blocks::bedrock) {
					chunk->setBlockAtAbsolute(blocks::air, pos + delta, true);
				}
			}
		}
	}
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

/**
 * @brief sets blocks from selectStart to selectEnd to given type
 * if type is not a solid block, we set to stone by default
 * will later accept -a to not set air blocks
*/
void WorldEdit::handleCmdSet( int type )
{
	Chunk * chunk = _openGL_Manager->getCurrentChunkPtr();
	if (!chunk) {
		return ;
	}
	if (type && (s_blocks[type]->isItemOnly || s_blocks[type]->transparent)) {
		type = blocks::stone;
	}

	glm::ivec3 start = {(_selectStart.x < _selectEnd.x) ? _selectStart.x : _selectEnd.x, (_selectStart.y < _selectEnd.y) ? _selectStart.y : _selectEnd.y, (_selectStart.z < _selectEnd.z) ? _selectStart.z : _selectEnd.z};
	glm::ivec3 end = {(_selectStart.x > _selectEnd.x) ? _selectStart.x : _selectEnd.x, (_selectStart.y > _selectEnd.y) ? _selectStart.y : _selectEnd.y, (_selectStart.z > _selectEnd.z) ? _selectStart.z : _selectEnd.z};

	_chat->chatMessage("Settings [" + std::to_string(start.x) + ", " + std::to_string(start.y) + ", " + std::to_string(start.z) + "] to [" + std::to_string(end.x) + ", " + std::to_string(end.y) + ", " + std::to_string(end.z) + "] to " + s_blocks[type]->name);
	for (int posX = start.x; posX <= end.x; ++posX) {
		for (int posY = start.y; posY <= end.y; ++posY) {
			for (int posZ = start.z; posZ <= end.z; ++posZ) {
				chunk->setBlockAtAbsolute(type, posX, posY, posZ, false);
			}
		}
	}
}

/**
 * @brief copy blocks from selectStart to selectEnd to clipboard
*/
void WorldEdit::handleCmdCopy( void )
{
	Chunk * chunk = _openGL_Manager->getCurrentChunkPtr();
	if (!chunk) {
		return ;
	}

	_clipStart = {(_selectStart.x < _selectEnd.x) ? _selectStart.x : _selectEnd.x, (_selectStart.y < _selectEnd.y) ? _selectStart.y : _selectEnd.y, (_selectStart.z < _selectEnd.z) ? _selectStart.z : _selectEnd.z};
	_clipEnd = {(_selectStart.x > _selectEnd.x) ? _selectStart.x : _selectEnd.x, (_selectStart.y > _selectEnd.y) ? _selectStart.y : _selectEnd.y, (_selectStart.z > _selectEnd.z) ? _selectStart.z : _selectEnd.z};

	_chat->chatMessage("Copying [" + std::to_string(_clipStart.x) + ", " + std::to_string(_clipStart.y) + ", " + std::to_string(_clipStart.z) + "] to [" + std::to_string(_clipEnd.x) + ", " + std::to_string(_clipEnd.y) + ", " + std::to_string(_clipEnd.z) + "] to clipboard.");
	_clipboard.clear();
	for (int posX = _clipStart.x; posX <= _clipEnd.x; ++posX) {
		for (int posY = _clipStart.y; posY <= _clipEnd.y; ++posY) {
			for (int posZ = _clipStart.z; posZ <= _clipEnd.z; ++posZ) {
				int value = chunk->getBlockAtAbsolute(posX, posY, posZ);
				_clipboard.push_back(value);
			}
		}
	}
	// rm player pos from clipStart and End to have relative position of clipped area in memory
	glm::ivec3 pos = _openGL_Manager->_player->getPos();
	_clipStart -= pos;
	_clipEnd -= pos;
	_absoluteClipboard = true;
}

/**
 * @brief pasting content of clipboard to world
 * will later accept -a to not set air blocks
*/
void WorldEdit::handleCmdPaste( bool notAirBlocks )
{
	if (!_clipboard.size()) {
		_chat->chatMessage("Clipboard is empty.", argb::red);
		return ;
	}

	Chunk * chunk = _openGL_Manager->getCurrentChunkPtr();
	if (!chunk) {
		return ;
	}

	glm::ivec3 start = _clipStart;
	glm::ivec3 end = _clipEnd;
	// add player pos to clipStart and End to have absolute position of paste area
	if (_absoluteClipboard) {
		glm::ivec3 pos = _openGL_Manager->_player->getPos();
		start += pos;
		end += pos;
	}

	_chat->chatMessage("Pasting clipboard [" + std::to_string(start.x) + ", " + std::to_string(start.y) + ", " + std::to_string(start.z) + "] to [" + std::to_string(end.x) + ", " + std::to_string(end.y) + ", " + std::to_string(end.z) + "]");
	int index = 0;
	for (int posX = start.x; posX <= end.x; ++posX) {
		for (int posY = start.y; posY <= end.y; ++posY) {
			for (int posZ = start.z; posZ <= end.z; ++posZ) {
				int value = _clipboard[index++];
				if (notAirBlocks && value == blocks::air) {
					continue ;
				}
				chunk->setBlockAtAbsolute(value, posX, posY, posZ, false);
			}
		}
	}
}

/**
 * @brief move selection [n] blocks towards cam direction
 * if -a is specified, air blocks are not moved
*/
void WorldEdit::handleCmdMove( std::vector<std::string>& argv )
{
	int argc = argv.size();
	if (argc < 2 || argc > 3 || (argc == 3 && argv[2] != "-a")) {
		_chat->chatMessage("Wrong usage of WorldEdit command //move <distance> [-a]", argb::red);
		return ;
	}

	int distance = 0;
	for (int i = (argv[1][0] == '-'); argv[1][i]; ++i) {
		if (!std::isdigit(argv[1][i])) {
			_chat->chatMessage("Wrong usage of WorldEdit command //move <distance> [-a]", argb::red);
			return ;
		}
		distance = distance * 10 + argv[1][i] - '0';
	}
	if (!distance) {
		_chat->chatMessage("Wrong usage of WorldEdit command //move <distance> [-a]", argb::red);
		return ;
	}

	const glm::ivec3 dir = ((argv[1][0] == '-') ? 1 : -1) * adj_blocks[_openGL_Manager->_player->getOrientation6()];
	handleCmdCopy();
	handleCmdSet(blocks::air);
	_clipStart += distance * dir;
	_clipEnd += distance * dir;
	handleCmdPaste(argc == 3);
}

/**
 * @brief check if path possible between 2 selected blocks and transform
 * found path to diamond blocks
*/
void WorldEdit::handleCmdPathfind( void )
{
	Chunk* chunk = _openGL_Manager->getCurrentChunkPtr();
	if (!chunk) {
		return (_chat->chatMessage("Missing current_chunk_ptr, abort.", argb::red));
	}
	
	std::pair<std::vector<glm::ivec3>, int> path = chunk->computePathfinding(_selectStart + glm::ivec3(0, 0, 1), _selectEnd + glm::ivec3(0, 0, 1));
	if (!path.first.size()) {
		return (_chat->chatMessage("Couldn't find path, nbr iter: " + std::to_string(path.second) + ".", argb::red));
	}

	for (auto pos : path.first) {
		chunk->setBlockAtAbsolute(blocks::diamond_block, pos, false);
	}
	return (_chat->chatMessage("Path of size " + std::to_string(path.first.size()) + " found in " + std::to_string(path.second) + " iterations."));
}

void WorldEdit::handleCmdBrushSize( std::vector<std::string>& argv )
{
	if (argv.size() != 2) {
		return (_chat->chatMessage("Wrong usage of command //brushSize <size>", argb::red));
	}
	int value = 0;
	size_t index = 0;
	for (; argv[1][index]; ++index) {
		if (!isdigit(argv[1][index])) {
			return (_chat->chatMessage("Wrong usage of //brushSize <size>, size invalid"));
		}
		value = value * 10 + argv[1][index] - '0';
	}
	_brushSize = value;
	_chat->chatMessage("Brush size set to " + std::to_string(value));
}

/**
 * @brief compute next generation of game of life inside selection
 */
void WorldEdit::handleCmdGameOfLife( void )
{
	Chunk * chunk = _openGL_Manager->getCurrentChunkPtr();
	if (!chunk) {
		return ;
	}

	_clipStart = {(_selectStart.x < _selectEnd.x) ? _selectStart.x : _selectEnd.x, (_selectStart.y < _selectEnd.y) ? _selectStart.y : _selectEnd.y, (_selectStart.z < _selectEnd.z) ? _selectStart.z : _selectEnd.z};
	_clipEnd = {(_selectStart.x > _selectEnd.x) ? _selectStart.x : _selectEnd.x, (_selectStart.y > _selectEnd.y) ? _selectStart.y : _selectEnd.y, (_selectStart.z > _selectEnd.z) ? _selectStart.z : _selectEnd.z};

	_clipboard.clear();
	for (int posX = _clipStart.x; posX <= _clipEnd.x; ++posX) {
		for (int posY = _clipStart.y; posY <= _clipEnd.y; ++posY) {
			for (int posZ = _clipStart.z; posZ <= _clipEnd.z; ++posZ) {
				int value = chunk->getBlockAtAbsolute(posX, posY, posZ);
				_clipboard.push_back(value);
			}
		}
	}

	std::vector<int> originalState = _clipboard;
	glm::ivec3 sizes = _clipEnd + glm::ivec3(1) - _clipStart;
	for (int posX = 0; posX < sizes.x; ++posX) {
		for (int posY = 0; posY < sizes.y; ++posY) {
			for (int posZ = 0; posZ < sizes.z; ++posZ) {
				int cnt = 0;
				for (int dx = -1; dx < 2; ++dx) {
					if (posX + dx < 0 || posX + dx >= sizes.x) {
						continue ;
					}
					for (int dy = -1; dy < 2; ++dy) {
						if (posY + dy < 0 || posY + dy >= sizes.y) {
							continue ;
						}
						for (int dz = -1; dz < 2; ++dz) {
							if (posZ + dz < 0 || posZ + dz >= sizes.z || (!dx && !dy && !dz)) {
								continue ;
							}
							if (originalState[((posX + dx) * sizes.y + (posY + dy)) * sizes.z + (posZ + dz)] != blocks::air) {
								++cnt;
							}
						}
					}
				}
				if (originalState[(posX * sizes.y + posY) * sizes.z + posZ] != blocks::air) {
					if (cnt < 2 || cnt > 3) { // dies of under/overpopulation
						_clipboard[(posX * sizes.y + posY) * sizes.z + posZ] = blocks::air;
					}
				} else if (cnt == 3) { // welcome to life bud
					_clipboard[(posX * sizes.y + posY) * sizes.z + posZ] = blocks::quartz_block;
				}
			}
		}
	}
	_absoluteClipboard = false;
	handleCmdPaste(false);
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

/**
 * @brief parse user input to check if worldEdit-defined command has been called
 * @param str data_value of componant to get attached dir from
 * @return false if user input is a worldEdit command
*/
bool WorldEdit::parseCommand( std::vector<std::string>& argv )
{
	assert((_chat && "WorldEdit::parseCommand"));

	// _chat->chatMessage("Welcome to worldEdit!");
	// _chat->chatMessage("please wait, we are treating your command " + argv[0]);

	for (int index = 0; index < world_edit::cmds::size; ++index) {
		if (!world_edit::commands[index].compare(argv[0])) {
			if (index == world_edit::cmds::wand) {
				_inventory->replaceSlot(blocks::worldedit_wand, true);
				_selectStart.z = 1000;
				_selectEnd.z = 1000;
				_running = true;
				_chat->chatMessage("******************************************", argb::green);
				_chat->chatMessage("* Welcome to WorldEdit!\t*", argb::green);
				_chat->chatMessage("******************************************", argb::green);
				return (false);
			} else if (index == world_edit::cmds::brush) {
				_inventory->replaceSlot(blocks::worldedit_brush, true);
				return (false);
			} else if (index == world_edit::cmds::brush_size) {
				handleCmdBrushSize(argv);
				return (false);
			// } else if (!_running) {
			// 	return (true);
			} else if (_selectStart.z == 1000) {
				_chat->chatMessage("Selection start not set.", argb::red);
				return (false);
			} else if (_selectEnd.z == 1000) {
				_chat->chatMessage("Selection end not set.", argb::red);
				return (false);
			}
			switch (index) {
				case world_edit::cmds::set:
					handleCmdSet(_inventory->getSlotBlock(_inventory->getSlotNum()).type);
					break ;
				case world_edit::cmds::copy:
					handleCmdCopy();
					break ;
				case world_edit::cmds::cut:
					handleCmdCopy();
					handleCmdSet(blocks::air);
					break ;
				case world_edit::cmds::paste:
					handleCmdPaste(false);
					break ;
				case world_edit::cmds::move:
					handleCmdMove(argv);
					break ;
				case world_edit::cmds::stack:
					break ;
				case world_edit::cmds::path_finding:
					handleCmdPathfind();
					break ;
				case world_edit::cmds::game_of_life:
					handleCmdGameOfLife();
					break ;
			}
			return (false);
		}
	}

	return (true);
}
