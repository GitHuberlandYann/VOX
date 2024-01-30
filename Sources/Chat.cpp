#include "Chat.hpp"
#include "DayCycle.hpp"
#include "PerlinNoise.hpp"
extern siv::PerlinNoise::seed_type perlin_seed;

Chat::Chat( Text *text ) : _histo_cursor(0), _text(text)
{
	_current.reserve(16);
	_past.reserve(16);
}

Chat::~Chat( void )
{

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
						chatMessage("\t/time query (daytime|gametime|day)");
						chatMessage("\t\tQueries current time.");
						chatMessage("\ttime set (day|night|noon|midnight)");
						chatMessage("\ttime set <time>");
						chatMessage("\t\tSets the internal daytime.");
						break ;
					case cmds::CLEAR:
						chatMessage("/help clear");
						chatMessage("\t/clear");
						chatMessage("\t\tClears items from player inventory.");
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

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

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
	_current.push_back({str, 10});
}

void Chat::sendMessage( std::string str )
{
	_histo_cursor = 0;
	str = trim_spaces(str);
	if (!str[0]) return ;
	_historic.push_back(str); // used to handle arrow action (ie command history)
	if (str[0] == '/') {
		std::vector<std::string> parstr = split(str, ' ');
		for (int index = 0; index < NBR_CMDS; ++index) {
			if (!commands[index].compare(0, commands[index].size(), parstr[0])) {
				switch (index) {
					case cmds::HELP:
						handle_help(parstr.size(), parstr);
						break ;
					case cmds::SEED:
						chatMessage("World seed is " + std::to_string(perlin_seed));
						break ;
					case cmds::GAMEMODE:
						chatMessage("Command recogised as cmds::GAMEMODE");
						// handle_gamemode(parstr.size(), parstr);
						break ;
					case cmds::TIME:
						chatMessage("Current time is " + DayCycle::Get()->getTime().substr(2));
						break ;
					case cmds::CLEAR:
						chatMessage("Command recogised as cmds::CLEAR");
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
		// TODO limit nbr char per line and display message over several lines
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
