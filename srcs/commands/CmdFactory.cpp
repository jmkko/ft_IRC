#include "CmdFactory.hpp"

#include "Bot.hpp"
#include "Client.hpp"
#include "ICommand.hpp"
#include "Invite.hpp"
#include "Join.hpp"
#include "Kick.hpp"
#include "LogManager.hpp"
#include "Mode.hpp"
#include "Motd.hpp"
#include "Nick.hpp"
#include "Parser.hpp"
#include "Part.hpp"
#include "Pass.hpp"
#include "Ping.hpp"
#include "Privmsg.hpp"
#include "Quit.hpp"
#include "Server.hpp"
#include "Topic.hpp"
#include "User.hpp"
#include "Who.hpp"
#include "colors.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"

#include <sstream>

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

CmdFactory::CmdFactory(void) {}

CmdFactory::~CmdFactory(void) {}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

bool CmdFactory::check_in(Client& client, std::string& command)
{
    if (!client.is_authenticated() && command != "PASS" && command != "QUIT")
        return false;
    if (!client.is_registered() && command != "USER" && command != "NICK" && command != "PASS" && command != "QUIT")
        return false;
    return true;
}

ICommand* CmdFactory::make_command(Server& server, Client& client, std::string& params)
{
    Parser             p(server, client);
    std::string        commandLine = "";
    std::istringstream iss(params); // NOLINT(clang-diagnostic-vexing-parse)
    std::string        available[NB_AVAILABLE_CMD] = {"USER",
                                                      "PASS",
                                                      "NICK",
                                                      "TOPIC",
                                                      "QUIT",
                                                      "INVITE",
                                                      "JOIN",
                                                      "PART",
                                                      "MODE",
                                                      "PRIVMSG",
                                                      "WHO",
                                                      "KICK",
                                                      "MOTD",
                                                      "BOT",
                                                      "PING"};

    ICommand* (CmdFactory::* ptr[NB_AVAILABLE_CMD])(std::string&) = {&CmdFactory::user_cmd,
                                                                     &CmdFactory::pass_cmd,
                                                                     &CmdFactory::nick_cmd,
                                                                     &CmdFactory::topic_cmd,
                                                                     &CmdFactory::quit_cmd,
                                                                     &CmdFactory::invite_cmd,
                                                                     &CmdFactory::join_cmd,
                                                                     &CmdFactory::part_cmd,
                                                                     &CmdFactory::mode_cmd,
                                                                     &CmdFactory::privmsg_cmd,
                                                                     &CmdFactory::who_cmd,
                                                                     &CmdFactory::kick_cmd,
                                                                     &CmdFactory::motd_cmd,
                                                                     &CmdFactory::bot_cmd,
                                                                     &CmdFactory::ping_cmd};

    iss >> commandLine;
    for (size_t i = 0; i < NB_AVAILABLE_CMD; i++) {
        if (commandLine == available[i]) {
            LOG_d_CMD(GREEN + commandLine + NC);
            if (!check_in(client, commandLine)) {
                p.response(ERR_NOTREGISTERED, commandLine);
                return NULL;
            }
            std::string params;
            std::getline(iss, params);
            LOG_d_CMD(params);
            if (!params.empty() && params[0] == ' ') {
                params = params.substr(1);
            }
            return (this->*ptr[i])(params);
        }
    }
    p.response(ERR_UNKNOWNCOMMAND, commandLine);

    return NULL;
}

ICommand* CmdFactory::nick_cmd(std::string& params) { return new Nick(params); }

ICommand* CmdFactory::user_cmd(std::string& params) { return new User(params); };

ICommand* CmdFactory::pass_cmd(std::string& params) { return new Pass(params); };

ICommand* CmdFactory::kick_cmd(std::string& params) { return new Kick(params); };

ICommand* CmdFactory::quit_cmd(std::string& params) { return new Quit(params); };

ICommand* CmdFactory::join_cmd(std::string& params) { return new Join(params); };

ICommand* CmdFactory::mode_cmd(std::string& params) { return new Mode(params); };

ICommand* CmdFactory::part_cmd(std::string& params) { return new Part(params); };

ICommand* CmdFactory::invite_cmd(std::string& params) { return new Invite(params); };

ICommand* CmdFactory::who_cmd(std::string& params) { return new Who(params); }

ICommand* CmdFactory::privmsg_cmd(std::string& params) { return new Privmsg(params); };

ICommand* CmdFactory::ping_cmd(std::string& params) { return new Ping(params); };

ICommand* CmdFactory::motd_cmd(std::string& params) { return new Motd(params); };

ICommand* CmdFactory::bot_cmd(std::string& params) { return new Bot(params); };

ICommand* CmdFactory::topic_cmd(std::string& params) { return new Topic(params); };
