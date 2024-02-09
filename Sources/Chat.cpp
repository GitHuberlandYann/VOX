#include "Chat.hpp"
#include "OpenGL_Manager.hpp"
#include "PerlinNoise.hpp"
extern siv::PerlinNoise::seed_type perlin_seed;

Chat::Chat( Text *text ) : _histo_cursor(0), _oglMan(NULL), _text(text)
{
	_current.reserve(16);
	_past.reserve(16);
}

Chat::~Chat( void )
{
	std::cout << "Destructor of Chat called" << std::endl;
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void Chat::handle_help( int argc, std::vector<std::string> &argv )
{
	if (argc == 1) {
		chatMessage("List of commands:");
		for (int i = 0; i < NBR_CMDS; ++i) {
			chatMessage("\t" + commands[i]);
		}
		chatMessage("Try /help <command> to see what they do.");
	} else if (argc == 2) {
		for (int index = 0; index < NBR_CMDS; ++index) {
			if (!commands[index].compare(1, commands[index].size(), argv[1])) {
				switch (index) {
					case cmds::HELP:
						chatMessage("/help help");
						chatMessage("\t/help");
						chatMessage("\t\tLists all commands.");
						chatMessage("\t/help <command>");
						chatMessage("\t\tShows usages for one command.");
						break ;
					case cmds::SEED:
						chatMessage("/help seed");
						chatMessage("\t/seed");
						chatMessage("\t\tDisplays the world seed.");
						break ;
					case cmds::GAMEMODE:
						chatMessage("/help gamemode");
						chatMessage("\t/gamemode");
						chatMessage("\t\tDisplays current gamemode.");
						chatMessage("\t/gamemode <gamemode>");
						chatMessage("\t\tSets a player's game mode.");
						chatMessage("\t\t<gamemode> must be one of the following:");
						chatMessage("\t\t\tsurvival for Survival mode");
						chatMessage("\t\t\tcreative for Creative mode");
						chatMessage("\t\t\tadventure for Adventure mode");
						chatMessage("\t\t\tspectator for Spectator mode");
						break ;
					case cmds::TIME:
						chatMessage("/help time");
						chatMessage("\tChanges or queries the world's game time.");
						chatMessage("\t/time add <time>");
						chatMessage("\t\tAdds <time> to the in-game daytime.");
						chatMessage("\t/time sub <time>");
						chatMessage("\t\tSubstracts <time> from the in-game daytime.");
						chatMessage("\t/time query (daytime|gametime|day)");
						chatMessage("\t\tQueries current time.");
						chatMessage("\ttime set (day|night|noon|midnight)");
						chatMessage("\ttime set <time>");
						chatMessage("\t\tSets the internal daytime.");
						break ;
					case cmds::CLEAR:
						chatMessage("/help clear");
						chatMessage("\t/clear");
						chatMessage("\t\tClears chat history.");
						chatMessage("\t/clear entities");
						chatMessage("\t/clear e");
						chatMessage("\t\tClears all entities.");
						break ;
					case cmds::TP:
					case cmds::TELEPORT:
						chatMessage("/help teleport");
						chatMessage("\t/teleport <location>");
						chatMessage("\t\tTeleports the executor to a certain position.");
						chatMessage("\t/teleport spawn");
						chatMessage("\t\tTeleports the executor to his spawn point.");
						chatMessage("\t/teleport -");
						chatMessage("\t\tTeleports the executor to the last point he tped from.");
						break ;
					case cmds::SP:
					case cmds::SPAWNPOINT:
						chatMessage("/help spawnpoint");
						chatMessage("\t/spawnpoint");
						chatMessage("\t\tSets your spawn point to your current location.");
						chatMessage("\t/spawnpoint <location>");
						chatMessage("\t\tSets your spawn point to a certain location.");
						break ;
					default:
						chatMessage("Command recognised but documentation not coded yet.");
				}
				return ;
			}
		}
		chatMessage("Wrong argument: " + argv[1] + " is not a command.");
	} else {
		chatMessage("Wrong usage of /help [command]");
	}
}

void Chat::handle_gamemode( int argc, std::vector<std::string> &argv )
{
	if (argc == 1) {
		_oglMan->getGamemode();
		return ;
	} else if (argc == 2) {
		if (!argv[1].compare("creative") || !argv[1].compare("0")) {
			_oglMan->setGamemode(false);
			return ;	
		} else if (!argv[1].compare("survival") || !argv[1].compare("1")) {
			_oglMan->setGamemode(true);
			return ;
		}
	}
	chatMessage("Wrong usage of /gamemode <gamemode>");
}

void Chat::handle_time( int argc, std::vector<std::string> &argv )
{
	if (argc == 1) {
		std::string time = DayCycle::Get()->getTime();
		chatMessage("Current time is " + time.substr(time.find('D') + 1));
	} else if (argc == 3 && !argv[1].compare("set")) {
		for (int index = 0; index < NBR_ARG_TIME; ++index) {
			if (!timeSetArgs[index].compare(argv[2])) {
				switch (index) {
					case args_time::DAY:
						DayCycle::Get()->setTicks(1000);
						chatMessage("time set to day");
						break ;
					case args_time::NIGHT:
						DayCycle::Get()->setTicks(13000);
						chatMessage("time set to night");
						break ;
					case args_time::NOON:
						DayCycle::Get()->setTicks(6000);
						chatMessage("time set to noon");
						break ;
					case args_time::MIDNIGHT:
						DayCycle::Get()->setTicks(18000);
						chatMessage("time set to midnight");
						break ;
					default:
						chatMessage("Argument for /time set (day|night) not implemented yet.");
				}
				return ;
			}
		}
		int ticks = 0;
		for (int i = 0; argv[2][i]; ++i) {
			if (isdigit(argv[2][i])) ticks = ticks * 10 + argv[2][i] - '0';
			else return (chatMessage("Bad argument for /time set (day|night) or /time set <number>"));
		}
		DayCycle::Get()->setTicks(ticks);
		chatMessage("Current time set to " + std::to_string(ticks));
	} else if (argc == 3 && !argv[1].compare("add")) {
		int ticks = 0;
		for (int i = 0; argv[2][i]; ++i) {
			if (isdigit(argv[2][i])) ticks = ticks * 10 + argv[2][i] - '0';
			else return (chatMessage("Bad argument for /time add <number>"));
		}
		DayCycle::Get()->addTicks(ticks);
		chatMessage("Added " + std::to_string(ticks) + " ticks to current time");
	} else if (argc == 3 && !argv[1].compare("sub")) {
		int ticks = 0;
		for (int i = 0; argv[2][i]; ++i) {
			if (isdigit(argv[2][i])) ticks = ticks * 10 + argv[2][i] - '0';
			else return (chatMessage("Bad argument for /time sub <number>"));
		}
		DayCycle::Get()->addTicks(-ticks);
		chatMessage("Substracted " + std::to_string(ticks) + " ticks from current time");
	} else {
		chatMessage("Wrong usage of command /time (or not implemented yet oupsi)");
	}
}

void Chat::handle_clear( int argc, std::vector<std::string> &argv )
{
	if (argc == 1) {
		_current.clear();
		_past.clear();
		_historic.clear();
		return ;
	} else if (argc == 2) {
		if (!argv[1].compare("e") || !argv[1].compare("entities")) {
			chatMessage("Cleared " + std::to_string(_oglMan->clearEntities()) + " entities");
			return ;
		}
	}
	chatMessage("Wrong usage of command /clear");
}

void Chat::handle_teleport( int argc, std::vector<std::string> &argv )
{
	if (argc == 2 && !argv[1].compare("spawn")) {
		glm::vec3 pos = _oglMan->_camera->getSpawnpoint();
		_oglMan->_camera->setPos(pos);
		_oglMan->resetInputsPtrs();
		chatMessage("Player pos set to " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));
		return ;
	} else if (argc == 2 && !argv[1].compare("-")) {
		glm::vec3 pos = _oglMan->_camera->getLastTp();
		_oglMan->_camera->setPos(pos);
		_oglMan->resetInputsPtrs();
		chatMessage("Player pos set to " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));
		return ;
	} else if (argc == 4) {
		try {
			glm::vec3 pos;
			pos.x = parseLocation(argv[1], 0);
			pos.y = parseLocation(argv[2], 1);
			pos.z = parseLocation(argv[3], 2);
			_oglMan->_camera->setPos(pos);
			_oglMan->resetInputsPtrs();
			chatMessage("Player pos set to " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));
			return ;
		} catch (std::exception & e) {
			(void)e;
			// std::cout << "handle_teleport thrown " << e.what() << std::endl;
		}
	}
	chatMessage("Wrong usage of /teleport <location>");
}

void Chat::handle_spawnpoint( int argc, std::vector<std::string> &argv )
{
	if (argc == 1) {
		glm::vec3 pos = _oglMan->_camera->getPos();
		_oglMan->_camera->setSpawnpoint(pos);
		chatMessage("Your spawnpoint was set to " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));
		return ;
	} else if (argc == 4) {
		try {
			glm::vec3 pos;
			pos.x = parseLocation(argv[1], 0);
			pos.y = parseLocation(argv[2], 1);
			pos.z = parseLocation(argv[3], 2);
			_oglMan->_camera->setSpawnpoint(pos);
			chatMessage("Your spawnpoint was set to " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));
			return ;
		} catch (std::exception & e) {
			(void)e;
			// std::cout << "handle_spawnpoint thrown " << e.what() << std::endl;
		}
	}
	chatMessage("Wrong usage of /spawnpoint [location]");
}

class InvalidLocationException : public std::exception
{
	public:
		const char *what() const throw()
		{
			return ("[InvalidLocationException] Wrong format for location arg.");
		}
};

float Chat::parseLocation( std::string &str, int coord )
{
	float res = 0;
	int index = 0;
	if (str[0] == '~') {
		res = _oglMan->_camera->getPos()[coord];
		++index;
	}
	int sign = 1;
	if (str[index] == '-') {
		sign = -1;
		++index;
	}
	int cnt = 0, save;
	while (isdigit(str[index])) {
		save = cnt;
		cnt = cnt * 10 + str[index++] - '0';
		if (cnt < save) throw InvalidLocationException();
	}
	if (str[index]) throw InvalidLocationException();
	return (res + sign * cnt);
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void Chat::setOGLManPtr( OpenGL_Manager *oglMan )
{
	_oglMan = oglMan;
}

void Chat::resetHistoCursor( void )
{
	_histo_cursor = 0;
}

std::string Chat::getHistoMsg( bool up )
{
	_histo_cursor += (up) ? 1 : -1;
	if (_histo_cursor > static_cast<int>(_historic.size())) {
		_histo_cursor = _historic.size() + 1;
	} else if (_histo_cursor < 0) {
		_histo_cursor = 0;
	}
	if (_histo_cursor == 0 || _histo_cursor == static_cast<int>(_historic.size() + 1)) {
		return ("");
	}
	return (_historic[_historic.size() - _histo_cursor]);
}

void Chat::chatMessage( std::string str )
{
	size_t pxl_width = 0, offset, font_size = 12, limit = CHAT_BOX_WIDTH - CHAT_BOX_X - 2 * CHAT_BOX_OFFSET;
	int start = 0, end = 0;
	for (char c : str) {
		if (c == 'i' || c == '.' || c == ':' || c == '!' || c == '\'' || c == ',' || c == ';' || c == '|' || c == '`') {
			offset = font_size * 0.5;
		} else if (c == 'I' || c == '[' || c == ']' || c == '"' || c == '*') {
			offset = font_size * 0.6;	
		} else if (c == 'l' || c == 't' || c == '(' || c == ')' || c == '<' || c == '>' || c == '{' || c == '}') {
			offset = font_size * 0.7;
		} else if (c == '\n') {
			pxl_width = 0;
			offset = 0;	
		} else {
			offset = font_size;
		}
		if (pxl_width + offset > limit) {
			_current.push_back({str.substr(start, end), 10});
			pxl_width = offset;
			start = end;
		} else {
			pxl_width += offset;
			++end;
		}
	}
	if (pxl_width) {
		_current.push_back({str.substr(start), 10});
	}
}

void Chat::sendMessage( std::string str )
{
	_histo_cursor = 0;
	str = trim_spaces(str);
	if (!str[0]) return ;
	if (!_historic.size() || str != _historic.back()) {
		_historic.push_back(str); // used to handle arrow action (ie command history)
	}
	if (str[0] == '/') {
		std::vector<std::string> parstr = split(str, ' ');
		for (int index = 0; index < NBR_CMDS; ++index) {
			if (!commands[index].compare(parstr[0])) {
				switch (index) {
					case cmds::HELP:
						handle_help(parstr.size(), parstr);
						break ;
					case cmds::SEED:
						chatMessage("World seed is " + std::to_string(perlin_seed));
						break ;
					case cmds::GAMEMODE:
						handle_gamemode(parstr.size(), parstr);
						break ;
					case cmds::TIME:
						handle_time(parstr.size(), parstr);
						break ;
					case cmds::CLEAR:
						handle_clear(parstr.size(), parstr);
						break ;
					case cmds::TP:
					case cmds::TELEPORT:
						handle_teleport(parstr.size(), parstr);
						break ;
					case cmds::SP:
					case cmds::SPAWNPOINT:
						handle_spawnpoint(parstr.size(), parstr);
						break ;
					default:
						chatMessage("Command recognised but behavior not coded yet.");
				}
				return ;
			}
		}
		chatMessage("Unknown or incomplete command: " + parstr[0]);
		chatMessage("try /help to see list of allowed commands");
		return ;
	}
	chatMessage("You: " + str);
	// TODO limit command_history and past_messages' lengths
}

void Chat::blitMessages( float deltaTime )
{
	int size = _current.size(), index = 0;

	for (auto m = _current.begin(); m != _current.end();) {
		_text->addText(36, WIN_HEIGHT - 68 - 18 * (size - index), 12, true, m->first);
		m->second -= deltaTime;
		if (m->second < 0) {
			_past.push_back(m->first);
			m = _current.erase(m);
		} else {
			++m;
		}
		++index;
	}
}

// display messages other than current ones
void Chat::blitPastMessages( void )
{
	int nbr_past = maxi(0, mini(10, _past.size() + _current.size()) - _current.size());
	for (int index = 0; index < nbr_past; ++index) {
		_text->addText(36, WIN_HEIGHT - 68 - 18 * (mini(10, _past.size() + _current.size()) - index), 12, true, _past[_past.size() - nbr_past + index]);
	}
}

// compute number of lines of text displayed by past and current messages
int Chat::computeHeight( void )
{
	if (_current.size() >= 10)
		return (_current.size());
	return (mini(10, _past.size() + _current.size()));
}

std::string Chat::getInfoString( void )
{
	return ("\nMessages\t> " + std::to_string(_current.size()) + "-" + std::to_string(_past.size()))
			+ ", " + std::to_string(_historic.size()) + "-" + std::to_string(_histo_cursor);
	// return ("\nMessages\t> " + std::to_string(_current.size()) + "-" + std::to_string(_past.size())) + " | " + std::to_string(maxi(0, mini(10, _past.size() + _current.size()) - _current.size()));
}
