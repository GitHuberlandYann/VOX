#ifndef WORLDEDIT_HPP
# define WORLDEDIT_HPP

# include "OpenGL_Manager.hpp"
# include <mutex>
# include <array>
# include <vector>
# include <string>

namespace WEDIT {
	enum cmds {
		WAND,
		SET,
		COPY,
		CUT,
		PASTE,
		MOVE,
		STACK,
		NBR_CMDS
	};
	const std::array<std::string, cmds::NBR_CMDS> commands = {"//wand", "//set", "//copy", "//cut", "//paste", "//move", "//stack"};
};

class WorldEdit
{
	private:
		static WorldEdit *_worldEditInstance;
		static std::mutex _mtx;

		bool _running, _absoluteClipboard;
		glm::ivec3 _selectStart, _selectEnd;
		glm::ivec3 _clipStart, _clipEnd;
		std::vector<int> _clipboard;
		OpenGL_Manager *_openGL_Manager;
		Inventory *_inventory;
		Chat *_chat;

		WorldEdit( void );
		~WorldEdit( void );

		void handleCmdSet( int value );
		void handleCmdCopy( void );
		void handleCmdPaste( bool notAirBlocks );
		void handleCmdMove( std::vector<std::string> &argv );
		// void handleCmdStack( void );

	public:
		WorldEdit( WorldEdit &other ) = delete;
		void operator=( const WorldEdit &other ) = delete;

		static WorldEdit *Get( void );
		static void Destroy( void );

		void setPtrs( OpenGL_Manager * openGL_Manager = NULL, Inventory * inventory = NULL, Chat *chat = NULL );
		void setSelectionStart( glm::ivec3 pos );
		void setSelectionEnd( glm::ivec3 pos );

		bool parseCommand( std::vector<std::string> &argv );
};

#endif
