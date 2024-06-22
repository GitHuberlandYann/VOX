#ifndef CHAT_HPP
# define CHAT_HPP

# include <memory>

# include "utils.h"
# include "Text.hpp"
class OpenGL_Manager;

# define CHAT_BOX_X 30
# define CHAT_BOX_WIDTH 700
# define CHAT_BOX_OFFSET 6

namespace chat {
	namespace cmds {
		enum {
			help,
			seed,
			gamemode,
			gamerule,
			time,
			clear,
			tp,
			teleport,
			sp,
			spawnpoint,
			give,
			size
		};
	};
	const std::array<std::string, cmds::size> commands = {"/help", "/seed", "/gamemode", "/gamerule", "/time", "/clear", "/tp", "/teleport", "/sp", "/spawnpoint", "/give"};

	namespace rule {
		enum {
			mobAI,
			size
		};
	};
	const std::array<std::string, rule::size> rules = {"domobai"};
}

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

typedef struct s_msg {
	std::string str;
	uint color = TEXT::WHITE;
}				t_msg;

class Chat
{
	public:
		Chat( std::shared_ptr<Text> text );
		~Chat( void );

		void setOGLManPtr( OpenGL_Manager *oglMan );

		void resetHistoCursor( void );
		std::string getHistoMsg( bool up );

		void chatMessage( std::string str, unsigned color = TEXT::WHITE );
		bool sendMessage( std::string str );
		void blitMessages( float deltaTime );
		void blitPastMessages( void );
		int computeHeight( void );

		std::string getInfoString( void );

	private:
		int _histo_cursor;
		OpenGL_Manager *_oglMan;
		std::shared_ptr<Text> _text;
		std::vector<std::pair<t_msg, float>> _current;
		std::vector<t_msg> _past, _historic;

		void handle_help( int argc, std::vector<std::string>& argv );
		void handle_gamemode( int argc, std::vector<std::string>& argv );
		void handle_gamerule( int argc, std::vector<std::string>& argv );
		void handle_time( int argc, std::vector<std::string>& argv );
		void handle_clear( int argc, std::vector<std::string>& argv );
		void handle_teleport( int argc, std::vector<std::string>& argv );
		void handle_spawnpoint( int argc, std::vector<std::string>& argv );
		void handle_give( int argc, std::vector<std::string>& argv );
		bool handle_freeze( int argc, std::vector<std::string>& argv );
		bool handle_frame( int argc, std::vector<std::string>& argv );

		float parseLocation( std::string &str, int coord );
};

#endif
