#include "OpenGL_Manager.hpp"
#include "Server.hpp"
#include "ServerChat.hpp"
#include "Address.hpp"

ServerChat::ServerChat( Server* server )
    : _server(server)
{

}

ServerChat::~ServerChat( void )
{

}

// ************************************************************************** //
//                                Private                                     //
// ************************************************************************** //

void ServerChat::handleTime( std::unique_ptr<Player>& player, Address& sender, int argc, std::vector<std::string>& argv )
{
    if (player->getPermissionLevel() < settings::consts::permission::admin) {
        return (sendPacket(sender, argb::red, "You do not have permission to use this command"));
    }

    if (argc == 1) {
		std::string time = DayCycle::Get()->getTime();
		sendPacket(sender, argb::white, "Current time is " + time.substr(time.find('D') + 1));
	} else if (argc == 3 && !argv[1].compare("set")) {
		for (int index = 0; index < NBR_ARG_TIME; ++index) {
			if (!timeSetArgs[index].compare(argv[2])) {
				switch (index) {
					case args_time::DAY:
						DayCycle::Get()->setTicks(1000);
						sendPacket(sender, argb::white, "time set to day");
						break ;
					case args_time::NIGHT:
						DayCycle::Get()->setTicks(13000);
						sendPacket(sender, argb::white, "time set to night");
						break ;
					case args_time::NOON:
						DayCycle::Get()->setTicks(6000);
						sendPacket(sender, argb::white, "time set to noon");
						break ;
					case args_time::MIDNIGHT:
						DayCycle::Get()->setTicks(18000);
						sendPacket(sender, argb::white, "time set to midnight");
						break ;
					default:
						sendPacket(sender, argb::red, "Argument for /time set (day|night) not implemented yet.");
				}
				return ;
			}
		}
		int ticks = 0;
		for (int i = 0; argv[2][i]; ++i) {
			if (isdigit(argv[2][i])) ticks = ticks * 10 + argv[2][i] - '0';
			else return (sendPacket(sender, argb::red, "Bad argument for /time set (day|night) or /time set <number>"));
		}
		DayCycle::Get()->setTicks(ticks);
		sendPacket(sender, argb::white, "Current time set to " + std::to_string(ticks));
	} else if (argc == 3 && !argv[1].compare("add")) {
		int ticks = 0;
		for (int i = 0; argv[2][i]; ++i) {
			if (isdigit(argv[2][i])) ticks = ticks * 10 + argv[2][i] - '0';
			else return (sendPacket(sender, argb::red, "Bad argument for /time add <number>"));
		}
		DayCycle::Get()->addTicks(ticks);
		sendPacket(sender, argb::white, "Added " + std::to_string(ticks) + " ticks to current time");
	} else if (argc == 3 && !argv[1].compare("sub")) {
		int ticks = 0;
		for (int i = 0; argv[2][i]; ++i) {
			if (isdigit(argv[2][i])) ticks = ticks * 10 + argv[2][i] - '0';
			else return (sendPacket(sender, argb::red, "Bad argument for /time sub <number>"));
		}
		DayCycle::Get()->addTicks(-ticks);
		sendPacket(sender, argb::white, "Substracted " + std::to_string(ticks) + " ticks from current time");
	} else {
		sendPacket(sender, argb::red, "Wrong usage of command /time");
	}
}

void ServerChat::handleAdmin( std::unique_ptr<Player>& player, Address& sender, int argc, std::vector<std::string>& argv )
{
    (void)argv;
    if (argc != 1) {
		return (sendPacket(sender, argb::red, "Wrong usage of command /admin"));
    }

    if (player->getPermissionLevel() == settings::consts::permission::admin) {
        return (sendPacket(sender, argb::white, "Failed to set you as admin: you are already admin"));
    }

    if (_server->adminedServer()) {
        return (sendPacket(sender, argb::red, "There is already an admin on the server"));
    }

    player->setPermissionLevel(settings::consts::permission::admin);
    sendPacket(sender, argb::green, "You are now the server admin");
}

void ServerChat::sendPacket( Address& sender, uint color, std::string msg )
{
	_packet = {pendings::useAddr, 0, sender};
	_packet.packet.action = packet_id::server::chat_msg;
	utils::memory::memwrite(_packet.packet.data, &color, sizeof(uint), _packet.size);
	utils::memory::memwrite(_packet.packet.data, msg.c_str(), msg.size(), _packet.size);
	_packet.size += sizeof(unsigned short);

    _server->pendPacket(_packet);
}

// ************************************************************************** //
//                                Public                                      //
// ************************************************************************** //

void ServerChat::parseCommand( std::unique_ptr<Player>& player, Address& sender, std::string cmd )
{
    std::vector<std::string> parstr = utils::string::split(cmd, ' ');

    for (int index = 0; index < chat::cmds::size; ++index) {
		if (!chat::commands[index].compare(parstr[0])) {
			switch (index) {
				/*
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
					break ;*/
				case chat::cmds::time:
					handleTime(player, sender, parstr.size(), parstr);
					break ;
				/*case chat::cmds::clear:
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
                */
                case chat::cmds::admin:
                    handleAdmin(player, sender, parstr.size(), parstr);
                    break ;
				default:
					sendPacket(sender, argb::red, "Command recognised but behavior not coded yet.");
			}
			return ;
		}
	}

    sendPacket(sender, argb::red, "Unknown or incomplete command: " + parstr[0]);
	// chatMessage("try /help to see list of allowed commands");
}
