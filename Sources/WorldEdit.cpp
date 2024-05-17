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
void WorldEdit::setPtrs( OpenGL_Manager * openGL_Manager, Inventory * inventory, Chat *chat )
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
	_chat->chatMessage("Selection Start set to " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z), TEXT::GREEN);
}

/**
 * @brief set _selectEnd used by WordEdit commands to pos
 * @param pos world pos of block selected
*/
void WorldEdit::setSelectionEnd( glm::ivec3 pos )
{
	_selectEnd = pos;
	_chat->chatMessage("Selection End set to " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z), TEXT::RED);
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

/**
 * @brief sets blocks from selectStart to selectEnd to stone
 * will later accept arg to set blocks to specific value
 * will later accept -a to not set air blocks
*/
void WorldEdit::handleCmdSet( void )
{
	Chunk * chunk = _openGL_Manager->getCurrentChunkPtr();
	if (!chunk) {
		return ;
	}

	glm::ivec3 start = {(_selectStart.x < _selectEnd.x) ? _selectStart.x : _selectEnd.x, (_selectStart.y < _selectEnd.y) ? _selectStart.y : _selectEnd.y, (_selectStart.z < _selectEnd.z) ? _selectStart.z : _selectEnd.z};
	glm::ivec3 end = {(_selectStart.x > _selectEnd.x) ? _selectStart.x : _selectEnd.x, (_selectStart.y > _selectEnd.y) ? _selectStart.y : _selectEnd.y, (_selectStart.z > _selectEnd.z) ? _selectStart.z : _selectEnd.z};

	_chat->chatMessage("Settings [" + std::to_string(start.x) + ", " + std::to_string(start.y) + ", " + std::to_string(start.z) + "] to [" + std::to_string(end.x) + ", " + std::to_string(end.y) + ", " + std::to_string(end.z) + "] to stone.");
	for (int posX = start.x; posX <= end.x; ++posX) {
		for (int posY = start.y; posY <= end.y; ++posY) {
			for (int posZ = start.z; posZ <= end.z; ++posZ) {
				chunk->setBlockAtAbsolute(blocks::STONE, posX, posY, posZ, true);
			}
		}
	}
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

/**
 * @brief parse user input to check if worldEdit-defined command has been called
 * @param str data_value of componant to get attached dir from
 * @return false if user input is a worldEdit command
*/
bool WorldEdit::parseCommand( std::vector<std::string> &argv )
{
	assert((_chat && "WorldEdit::parseCommand"));

	// _chat->chatMessage("Welcome to worldEdit!");
	// _chat->chatMessage("please wait, we are treating your command " + argv[0]);

	for (int index = 0; index < WEDIT::cmds::NBR_CMDS; ++index) {
		if (!WEDIT::commands[index].compare(argv[0])) {
			if (index == WEDIT::cmds::WAND) {
				_inventory->replaceSlot(blocks::WORLDEDIT_WAND, true);
				_selectStart.z = 1000;
				_selectEnd.z = 1000;
				_running = true;
				_chat->chatMessage("***************************************", TEXT::GREEN);
				_chat->chatMessage("* Welcome to WorldEdit! *", TEXT::GREEN);
				_chat->chatMessage("***************************************", TEXT::GREEN);
				return (false);
			} else if (!_running) {
				return (true);
			} else if (_selectStart.z == 1000) {
				_chat->chatMessage("Selection start not set.", TEXT::RED);
				return (false);
			} else if (_selectEnd.z == 1000) {
				_chat->chatMessage("Selection end not set.", TEXT::RED);
				return (false);
			}
			switch (index) {
				case WEDIT::cmds::SET:
					handleCmdSet();
					break ;
				case WEDIT::cmds::COPY:
					break ;
				case WEDIT::cmds::PASTE:
					break ;
				case WEDIT::cmds::MOVE:
					break ;
				case WEDIT::cmds::STACK:
					break ;
			}
			return (false);
		}
	}


	return (true);
}
