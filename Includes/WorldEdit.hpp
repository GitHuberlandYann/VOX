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
		PASTE,
		MOVE,
		STACK,
		NBR_CMDS
	};
	const std::array<std::string, cmds::NBR_CMDS> commands = {"//wand", "//set", "//copy", "//paste", "//move", "//stack"};
};

class WorldEdit
{
	private:
		static WorldEdit *_worldEditInstance;
		static std::mutex _mtx;

		bool _running;
		glm::ivec3 _selectStart, _selectEnd;
		// std::vector<> _clibboard;
		OpenGL_Manager *_openGL_Manager;
		Inventory *_inventory;
		Chat *_chat;

		WorldEdit( void );
		~WorldEdit( void );

		void handleCmdSet( void );

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
