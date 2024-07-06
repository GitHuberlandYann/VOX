#include "Chat.hpp"
#include "OpenGL_Manager.hpp"
#include "PerlinNoise.hpp"
#include "WorldEdit.hpp"
#include "logs.hpp"
extern siv::PerlinNoise::seed_type perlin_seed;

Chat::Chat( std::shared_ptr<Text> text )
	: _histo_cursor(0), _oglMan(NULL), _text(text)
{
	_current.reserve(16);
	_past.reserve(16);
}

Chat::~Chat( void )
{
	MAINLOG(LOG("Destructor of Chat called"));
}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void Chat::handle_help( int argc, std::vector<std::string> &argv )
{
	if (argc == 1) {
		chatMessage("List of commands:");
		for (int i = 0; i < chat::cmds::size; ++i) {
			chatMessage("\t" + chat::commands[i]);
		}
		chatMessage("Try /help <command> to see what they do.");
	} else if (argc == 2) {
		for (int index = 0; index < chat::cmds::size; ++index) {
			if (!chat::commands[index].compare(1, chat::commands[index].size(), argv[1])) {
				switch (index) {
					case chat::cmds::help:
						chatMessage("/help help");
						chatMessage("\t/help");
						chatMessage("\t\tLists all commands.");
						chatMessage("\t/help <command>");
						chatMessage("\t\tShows usages for one command.");
						break ;
					case chat::cmds::seed:
						chatMessage("/help seed");
						chatMessage("\t/seed");
						chatMessage("\t\tDisplays the world seed.");
						break ;
					case chat::cmds::gamemode:
						chatMessage("/help gamemode");
						chatMessage("\t/gamemode");
						chatMessage("\t\tDisplays current gamemode.");
						chatMessage("\t/gamemode <gamemode>");
						chatMessage("\t\tSets a player's game mode.");
						chatMessage("\t\t<gamemode> must be one of the following:");
						chatMessage("\t\t\tsurvival for Survival mode");
						chatMessage("\t\t\tcreative for Creative mode");
						chatMessage("\t\t\tadventure for Adventure mode");
						break ;
					case chat::cmds::time:
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
					case chat::cmds::clear:
						chatMessage("/help clear");
						chatMessage("\t/clear");
						chatMessage("\t\tClears chat history.");
						chatMessage("\t/clear entities");
						chatMessage("\t/clear e");
						chatMessage("\t\tClears all entities.");
						chatMessage("\t/clear particles");
						chatMessage("\t/clear p");
						chatMessage("\t\tClears all particles.");
						chatMessage("\t/clear inventory");
						chatMessage("\t/clear i");
						chatMessage("\t\tClears your inventory.");
						break ;
					case chat::cmds::tp:
					case chat::cmds::teleport:
						chatMessage("/help teleport");
						chatMessage("\t/teleport <location>");
						chatMessage("\t\tTeleports the executor to a certain position.");
						chatMessage("\t/teleport spawn");
						chatMessage("\t\tTeleports the executor to his spawn point.");
						chatMessage("\t/teleport -");
						chatMessage("\t\tTeleports the executor to the last point he tped from.");
						break ;
					case chat::cmds::sp:
					case chat::cmds::spawnpoint:
						chatMessage("/help spawnpoint");
						chatMessage("\t/spawnpoint");
						chatMessage("\t\tSets your spawn point to your current location.");
						chatMessage("\t/spawnpoint <location>");
						chatMessage("\t\tSets your spawn point to a certain location.");
						break ;
					case chat::cmds::give:
						chatMessage("/give <player> <item>");
						chatMessage("\tgive selected item to selected player.");
						chatMessage("\tfor now, only 'me' accepted as player.");
						chatMessage("\titem must be valid.");
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
		if (!argv[1].compare("survival") || !argv[1].compare("0")) {
			_oglMan->setGamemode(settings::consts::gamemode::survival);
			return ;	
		} else if (!argv[1].compare("creative") || !argv[1].compare("1")) {
			_oglMan->setGamemode(settings::consts::gamemode::creative);
			return ;
		} else if (!argv[1].compare("adventure") || !argv[1].compare("2")) {
			_oglMan->setGamemode(settings::consts::gamemode::adventure);
			return ;
		}
	}
	chatMessage("Wrong usage of /gamemode <gamemode>");
}

static int getGameruleStatus( std::string str )
{
	int status = 0;

	if (str == "true" || str == "1") {
		status = true;
	} else if (str == "false" || str == "0") {
		status = false;
	} else {
		bool neg = false;
		for (int i = 0; str[i]; ++i) {
			if (str[i] == '-' && !i) {
				neg = true;
			} else if (!isdigit(str[i])) {
				return (-424219);
			} else {
				status = status * 10 + str[i] - '0';
			}
		}
		status = (neg) ? -status : status;
	}
	return (status);
}

void Chat::handle_gamerule( int argc, std::vector<std::string>& argv )
{
	if (argc != 3) {
		return (chatMessage("Wrong usage of /gamerule <rule> <status>"));
	}
	int state = getGameruleStatus(argv[2]);
	if (state == -424219) {
		return (chatMessage("Wrong usage of /gamerule <rule> <status>"));
	}
	for (int index = 0; index < chat::rule::size; ++index) {
		if (!chat::rules[index].compare(argv[1])) {
			switch (index) {
				case chat::rule::mobAI:
					Settings::Get()->setBool(settings::bools::mobAI, state);
					chatMessage("Rule mobAI set to " + std::string((state) ? "true" : "false"));
					break ;
				case chat::rule::timeMultiplier:
					DayCycle::Get()->setTimeMultiplier(state);
					chatMessage("Rule timeMultiplier set to " + std::to_string(state));
					break ;
				default:
					chatMessage("Rule recognised but no behavior coded yet.");
			}
			return ;
		}
	}
	chatMessage("Error /gamerule: rule not recognised.");
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
		} else if (!argv[1].compare("p") || !argv[1].compare("particles")) {
			chatMessage("Cleared " + std::to_string(_oglMan->clearParticles()) + " particles");
			return ;
		} else if (!argv[1].compare("i") || !argv[1].compare("inventory")) {
			_oglMan->_inventory->spillInventory(NULL);
			chatMessage("Cleared your inventory");
			return ;
		}
	}
	chatMessage("Wrong usage of command /clear");
}

void Chat::handle_teleport( int argc, std::vector<std::string> &argv )
{
	if (argc == 2 && !argv[1].compare("spawn")) {
		glm::vec3 pos = _oglMan->_player->getSpawnpoint();
		_oglMan->_player->setPos(pos);
		_oglMan->resetInputsPtrs();
		chatMessage("Player pos set to " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));
		return ;
	} else if (argc == 2 && !argv[1].compare("-")) {
		glm::vec3 pos = _oglMan->_player->getLastTp();
		_oglMan->_player->setPos(pos);
		_oglMan->resetInputsPtrs();
		chatMessage("Player pos set to " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));
		return ;
	} else if (argc == 4) {
		try {
			glm::vec3 pos;
			pos.x = parseLocation(argv[1], 0);
			pos.y = parseLocation(argv[2], 1);
			pos.z = parseLocation(argv[3], 2);
			_oglMan->_player->setPos(pos);
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
		glm::vec3 pos = _oglMan->_player->getPos();
		_oglMan->_player->setSpawnpoint(pos);
		chatMessage("Your spawnpoint was set to " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));
		return ;
	} else if (argc == 4) {
		try {
			glm::vec3 pos;
			pos.x = parseLocation(argv[1], 0);
			pos.y = parseLocation(argv[2], 1);
			pos.z = parseLocation(argv[3], 2);
			_oglMan->_player->setSpawnpoint(pos);
			chatMessage("Your spawnpoint was set to " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));
			return ;
		} catch (std::exception & e) {
			(void)e;
			// std::cout << "handle_spawnpoint thrown " << e.what() << std::endl;
		}
	}
	chatMessage("Wrong usage of /spawnpoint [location]");
}

/**
 * @brief parse arg after give and give player selected item if valid entry
*/
void Chat::handle_give( int argc, std::vector<std::string> &argv )
{
	int item = 0;

	if (argc != 3 || argv[1].compare("me")) {
		chatMessage("Wrong usage of /give me <item>");
		return ;
	} else if (!argv[2].compare("redstone")) {
		_oglMan->_inventory->spillInventory(NULL);
		const std::array<int, 28> reds = {blocks::observer, blocks::lever, blocks::redstone_dust, blocks::sticky_piston,
			blocks::repeater, blocks::stone_bricks, blocks::worldedit_wand, blocks::iron_block, blocks::glass,

			blocks::stone_button, blocks::oak_button, blocks::chest, blocks::torch, blocks::stone, blocks::coal_ore,
			blocks::redstone_ore, blocks::diamond_ore, blocks::grass_block, blocks::oak_sign, blocks::redstone_lamp,
			blocks::oak_stairs, blocks::piston, blocks::comparator, blocks::redstone_torch, blocks::redstone_block,
			blocks::iron_ore, blocks::target, blocks::item_frame};
		for (size_t index = 0; index < reds.size(); ++index) {
			_oglMan->_inventory->absorbItem({reds[index], 1});
		}
		return ;
	} else if (!argv[2].compare(0, 8, "blocks::")) {
		int i = 0;
		for (; i < S_BLOCKS_SIZE; ++i) {
			if (!case_insensitive_compare(s_blocks[i]->name, argv[2].substr(8))) {
				item = i;
				break ;
			}
		}
	} else if (!argv[2].compare(0, 3, "all")) {
		int start = (argv[2][3]) ? std::atoi(&argv[2][3]) * 36 + 1 : 1;
		_oglMan->_inventory->spillInventory(NULL);
		for (int i = 0; i < 36; ++i) {
			int index = start + i;
			if (index > blocks::air && index < S_BLOCKS_SIZE) { // && s_blocks[index]->name.compare("TBD")) {
				_oglMan->_inventory->absorbItem({index, 1});
			}
		}
		return ;
	} else {
		size_t index = 0;
		for (; argv[2][index]; ++index) {
			if (!isdigit(argv[2][index])) {
				return (chatMessage("Wrong usage of /give me <item>, item invalid"));
			}
			item = item * 10 + argv[2][index] - '0';
		}
	}
	if (item < 0 || item >= S_BLOCKS_SIZE) {
		return (chatMessage("Wrong usage of /give me <item>, item invalid"));
	}
	switch (item) {
		case blocks::air:
		case blocks::farmland:
		case blocks::dirt_path:
		case blocks::piston_head:
		case blocks::moving_piston:
		case blocks::wheat_crop:
		case blocks::water:
		case blocks::worldedit_wand:
		case blocks::worldedit_brush:
			return (chatMessage("item " + s_blocks[item]->name + " [" + std::to_string(item) + "] can't be given."));
	}
	_oglMan->_inventory->replaceSlot(item, true);
	chatMessage("item " + s_blocks[item]->name + " [" + std::to_string(item) + "] has been added to your inventory.");
}

/**
 * @brief Checks if command is one of //freeze or //step
 * @return argv[0] != //freeze | //step
*/
bool Chat::handle_freeze( int argc, std::vector<std::string> &argv )
{
	if (!argv[0].compare("//freeze") && argc == 1) {
		DayCycle::Get()->freeze(this);
		return (false);
	} else if (!argv[0].compare("//step") && argc < 3) {
		if (argc == 1) {
			DayCycle::Get()->step(this, 1);
		} else {
			int steps = 0;
			size_t index = 0;
			for (; argv[1][index]; ++index) {
				if (!isdigit(argv[1][index])) {
					break ;
				}
				steps = steps * 10 + argv[1][index] - '0';
			}
			if (argv[1][index]) { // argv[1] is not only digits
				chatMessage("Wrong usage of //step [nbr]", argb::red);
			} else {
				DayCycle::Get()->step(this, steps);
			}
		}
		return (false);
	}
	return (true);
}

/**
 * @brief Checks if command is //frame and update targeted block 
 * @return argv[0] != //frame
*/
bool Chat::handle_frame( int argc, std::vector<std::string> &argv )
{
	if (!argv[0].compare("//frame") && argc == 2) {
		if (!argv[1].compare("visible")) {
			_oglMan->setItemFrame(true, false);
		} else if (!argv[1].compare("lock")) {
			_oglMan->setItemFrame(false, true);
		} else {
			return (true);
		}
		return (false);
	}
	return (true);
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
		res = _oglMan->_player->getPos()[coord];
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

void Chat::setOGLManPtr( OpenGL_Manager* oglMan )
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
	return (_historic[_historic.size() - _histo_cursor].str);
}

void Chat::chatMessage( std::string str, unsigned color )
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
			_current.push_back({{str.substr(start, end), color}, 10});
			pxl_width = offset;
			start = end;
		} else {
			pxl_width += offset;
			++end;
		}
	}
	if (pxl_width) {
		_current.push_back({{str.substr(start), color}, 10});
	}
}

/**
* @brief write message in chatBox, in future, also send msg to other players.
* first parse message and execute command if found
* @param str message to send
* @return true if command found, in which case chat Menu closes
*/
bool Chat::sendMessage( std::string str )
{
	_histo_cursor = 0;
	str = trim_spaces(str);
	if (!str[0]) return (false);
	if (!_historic.size() || str != _historic.back().str) {
		_historic.push_back({str, argb::white}); // used to handle arrow action (ie command history)
	}
	if (str[0] == '/') {
		std::vector<std::string> parstr = split(str, ' ');
		if (str[1] == '/') {
			if (!WorldEdit::Get()->parseCommand(parstr) || !handle_freeze(parstr.size(), parstr)
				|| !handle_frame(parstr.size(), parstr)) {
				return (true);
			}
		} else {
			for (int index = 0; index < chat::cmds::size; ++index) {
				if (!chat::commands[index].compare(parstr[0])) {
					switch (index) {
						case chat::cmds::help:
							handle_help(parstr.size(), parstr);
							break ;
						case chat::cmds::seed:
							chatMessage("World seed is " + std::to_string(perlin_seed));
							break ;
						case chat::cmds::gamemode:
							handle_gamemode(parstr.size(), parstr);
							break ;
						case chat::cmds::gamerule:
							handle_gamerule(parstr.size(), parstr);
							break ;
						case chat::cmds::time:
							handle_time(parstr.size(), parstr);
							break ;
						case chat::cmds::clear:
							handle_clear(parstr.size(), parstr);
							break ;
						case chat::cmds::tp:
						case chat::cmds::teleport:
							handle_teleport(parstr.size(), parstr);
							break ;
						case chat::cmds::sp:
						case chat::cmds::spawnpoint:
							handle_spawnpoint(parstr.size(), parstr);
							break ;
						case chat::cmds::give:
							handle_give(parstr.size(), parstr);
							break ;
						default:
							chatMessage("Command recognised but behavior not coded yet.");
					}
					return (true);
				}
			}
		}
		chatMessage("Unknown or incomplete command: " + parstr[0]);
		chatMessage("try /help to see list of allowed commands");
		return (false);
	}
	chatMessage("You: " + str);
	// TODO limit command_history and past_messages' lengths
	return (true);
}

void Chat::blitMessages( float deltaTime )
{
	int size = _current.size(), index = 0;

	for (auto m = _current.begin(); m != _current.end();) {
		_text->addText(36, WIN_HEIGHT - 68 - 18 * (size - index), 12, m->first.color, settings::consts::depth::chat, m->first.str);
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
		t_msg msg = _past[_past.size() - nbr_past + index];
		_text->addText(36, WIN_HEIGHT - 68 - 18 * (mini(10, _past.size() + _current.size()) - index), 12, msg.color, settings::consts::depth::chat, msg.str);
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
