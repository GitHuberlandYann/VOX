#ifndef CHAT_HPP
# define CHAT_HPP

# include "utils.h"
# include "Text.hpp"
class OpenGL_Manager;

# define CHAT_BOX_X 30
# define CHAT_BOX_WIDTH 700
# define CHAT_BOX_OFFSET 6

# define NBR_CMDS 9
namespace cmds {
	enum {
		HELP,
		SEED,
		GAMEMODE,
		TIME,
		CLEAR,
		TP,
		TELEPORT,
		SP,
		SPAWNPOINT
	};
}
const std::array<std::string, NBR_CMDS> commands = {"/help", "/seed", "/gamemode", "/time", "/clear", "/tp", "/teleport", "/sp", "/spawnpoint"};

# define NBR_ARG_TIME 4
namespace args_time {
	enum {
		DAY,
		NIGHT,
		NOON,
		MIDNIGHT
	};
}
const std::array<std::string, NBR_ARG_TIME> timeSetArgs = {"day", "night", "noon", "midnight"};

class Chat
{
	private:
		int _histo_cursor;
		OpenGL_Manager *_oglMan;
		Text *_text;
		std::vector<std::pair<std::string, float>> _current;
		std::vector<std::string> _past, _historic;

		void handle_help( int argc, std::vector<std::string> &argv );
		void handle_gamemode( int argc, std::vector<std::string> &argv );
		void handle_time( int argc, std::vector<std::string> &argv );
		void handle_teleport( int argc, std::vector<std::string> &argv );
		void handle_spawnpoint( int argc, std::vector<std::string> &argv );

		float parseLocation( std::string &str, int coord );

	public:
		Chat( Text *text );
		~Chat( void );

		void setOGLManPtr( OpenGL_Manager *oglMan );

		void resetHistoCursor( void );
		std::string getHistoMsg( bool up );

		void chatMessage( std::string str );
		void sendMessage( std::string str );
		void blitMessages( float deltaTime );
		void blitPastMessages( void );
		int computeHeight( void );

		std::string getInfoString( void );
};

#endif
