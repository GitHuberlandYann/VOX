#include "utils.h"
#include "WorldEdit.hpp"
#include <cassert>

WorldEdit::WorldEdit( void )
	: _running(false), _selectStart(1000), _selectEnd(1000), _openGL_Manager(NULL), _inventory(NULL), _chat(NULL)
{

}

WorldEdit::~WorldEdit( void )
{
	std::cout << "Destructor of WorldEdit called" << std::endl;
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
	_chat->chatMessage("Selection Start set to " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z), TEXT::GREEN);
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
	_chat->chatMessage("Selection End set to " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z), TEXT::RED);
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
	glm::ivec3 pos = _openGL_Manager->_camera->getPos();
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
		_chat->chatMessage("Clipboard is empty.", TEXT::RED);
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
		glm::ivec3 pos = _openGL_Manager->_camera->getPos();
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
		_chat->chatMessage("Wrong usage of WorldEdit command //move <distance> [-a]", TEXT::RED);
		return ;
	}

	int distance = 0;
	for (int i = (argv[1][0] == '-'); argv[1][i]; ++i) {
		if (!std::isdigit(argv[1][i])) {
			_chat->chatMessage("Wrong usage of WorldEdit command //move <distance> [-a]", TEXT::RED);
			return ;
		}
		distance = distance * 10 + argv[1][i] - '0';
	}
	if (!distance) {
		_chat->chatMessage("Wrong usage of WorldEdit command //move <distance> [-a]", TEXT::RED);
		return ;
	}

	const glm::ivec3 dir = ((argv[1][0] == '-') ? 1 : -1) * adj_blocks[_openGL_Manager->_camera->getOrientation6()];
	handleCmdCopy();
	handleCmdSet(blocks::air);
	_clipStart += distance * dir;
	_clipEnd += distance * dir;
	handleCmdPaste(argc == 3);
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

	for (int index = 0; index < WEDIT::cmds::NBR_CMDS; ++index) {
		if (!WEDIT::commands[index].compare(argv[0])) {
			if (index == WEDIT::cmds::WAND) {
				_inventory->replaceSlot(blocks::worldedit_wand, true);
				_selectStart.z = 1000;
				_selectEnd.z = 1000;
				_running = true;
				_chat->chatMessage("******************************************", TEXT::GREEN);
				_chat->chatMessage("* Welcome to WorldEdit!\t*", TEXT::GREEN);
				_chat->chatMessage("******************************************", TEXT::GREEN);
				return (false);
			// } else if (!_running) {
			// 	return (true);
			} else if (_selectStart.z == 1000) {
				_chat->chatMessage("Selection start not set.", TEXT::RED);
				return (false);
			} else if (_selectEnd.z == 1000) {
				_chat->chatMessage("Selection end not set.", TEXT::RED);
				return (false);
			}
			switch (index) {
				case WEDIT::cmds::SET:
					handleCmdSet(_inventory->getSlotBlock(_inventory->getSlotNum()).type);
					break ;
				case WEDIT::cmds::COPY:
					handleCmdCopy();
					break ;
				case WEDIT::cmds::CUT:
					// handleCmdCopy();
					// handleCmdSet(blocks::air);
					break ;
				case WEDIT::cmds::PASTE:
					handleCmdPaste(false);
					break ;
				case WEDIT::cmds::MOVE:
					handleCmdMove(argv);
					break ;
				case WEDIT::cmds::STACK:
					break ;
			}
			return (false);
		}
	}


	return (true);
}
