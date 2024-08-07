#ifndef WORLDEDIT_HPP
# define WORLDEDIT_HPP

# include "OpenGL_Manager.hpp"
# include <mutex>
# include <array>
# include <vector>
# include <string>

namespace world_edit {
	enum cmds {
		wand,
		set,
		copy,
		cut,
		paste,
		move,
		stack,
		path_finding,
		brush,
		brush_size,
		game_of_life,
		size
	};
	const std::array<std::string, cmds::size> commands = {"//wand", "//set", "//copy", "//cut", "//paste", "//move", "//stack", "//path", "//brush", "//brushsize", "//gameoflife"};
};

class WorldEdit
{
	public:
		WorldEdit( WorldEdit& other ) = delete;
		void operator=( const WorldEdit& other ) = delete;

		static WorldEdit* Get( void );
		static void Destroy( void );

		void setPtrs( OpenGL_Manager* openGL_Manager, Inventory* inventory, Chat* chat );
		void setSelectionStart( glm::ivec3 pos );
		void setSelectionEnd( glm::ivec3 pos );
		void useBrush( glm::ivec3 pos, bool adding );

		bool parseCommand( std::vector<std::string>& argv );

	private:
		static WorldEdit* _worldEditInstance;
		static std::mutex _mtx;

		bool _running, _absoluteClipboard;
		int _brushSize;
		glm::ivec3 _selectStart, _selectEnd;
		glm::ivec3 _clipStart, _clipEnd;
		std::vector<int> _clipboard;
		OpenGL_Manager* _openGL_Manager;
		Inventory* _inventory;
		Chat* _chat;

		WorldEdit( void );
		~WorldEdit( void );

		void handleCmdSet( int type );
		void handleCmdCopy( void );
		void handleCmdPaste( bool notAirBlocks );
		void handleCmdMove( std::vector<std::string>& argv );
		// void handleCmdStack( void );
		void handleCmdPathfind( void );
		void handleCmdBrushSize( std::vector<std::string>& argv );
		void handleCmdGameOfLife( void );
};

#endif
