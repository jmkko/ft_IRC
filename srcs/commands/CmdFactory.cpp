#include "CmdFactory.hpp"

#include "Client.hpp"
#include "ICommand.hpp"
#include "Invite.hpp"
#include "Join.hpp"
#include "Kick.hpp"
#include "LogManager.hpp"
#include "Mode.hpp"
#include "Motd.hpp"
#include "Nick.hpp"
#include "Pass.hpp"
#include "Ping.hpp"
#include "Privmsg.hpp"
#include "Quit.hpp"
#include "Part.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "Topic.hpp"
#include "User.hpp"
#include "Who.hpp"
#include "colors.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <iostream>
#include <sstream>

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

CmdFactory::CmdFactory(void) {}

CmdFactory::CmdFactory(const CmdFactory& other) { (void)other; }

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
    ReplyHandler&      rh          = ReplyHandler::get_instance(&server);
    std::string        commandLine = "";
    std::istringstream iss(params); // NOLINT(clang-diagnostic-vexing-parse)
    std::string        available[NB_AVAILABLE_CMD]                                  = {"USER",
                                                                                       "PASS",
                                                                                       "NICK",
                                                                                       "TOPIC",
                                                                                       "QUIT",
                                                                                       "INVITE",
                                                                                       "JOIN",
                                                                                       "PART",
                                                                                       "MODE",
                                                                                       "OPER",
                                                                                       "PRIVMSG",
                                                                                       "WHO",
                                                                                       "KICK",
                                                                                       "MOTD",
                                                                                       "PING"};
    ICommand* (CmdFactory::* ptr[NB_AVAILABLE_CMD])(Server&, Client&, std::string&) = {&CmdFactory::user_cmd,
                                                                                       &CmdFactory::pass_cmd,
                                                                                       &CmdFactory::nick_cmd,
                                                                                       &CmdFactory::topic_cmd,
                                                                                       &CmdFactory::quit_cmd,
                                                                                       &CmdFactory::invite_cmd,
                                                                                       &CmdFactory::join_cmd,
                                                                                       &CmdFactory::part_cmd,
                                                                                       &CmdFactory::mode_cmd,
                                                                                       &CmdFactory::oper_cmd,
                                                                                       &CmdFactory::privmsg_cmd,
                                                                                       &CmdFactory::who_cmd,
                                                                                       &CmdFactory::kick_cmd,
                                                                                       &CmdFactory::motd_cmd,
                                                                                       &CmdFactory::ping_cmd};

    iss >> commandLine;
    for (size_t i = 0; i < NB_AVAILABLE_CMD; i++) {
        if (commandLine == available[i]) {
            LOG_d_CMD(GREEN + commandLine + NC);
            if (!check_in(client, commandLine)) {
                rh.process_response(client, ERR_NOTREGISTERED, commandLine);
                return NULL;
            }
            std::string params;
            std::getline(iss, params);
            LOG_d_CMD(params);
            if (!params.empty() && params[0] == ' ') {
                params = params.substr(1);
            }
            return (this->*ptr[i])(server, client, params);
        }
    }
    rh.process_response(client, ERR_UNKNOWNCOMMAND, commandLine);

    return NULL;
}

// Return a NICK command object if the nickname is valid
ICommand* CmdFactory::nick_cmd(Server& server, Client& client, std::string& params)
{
	(void)server;
	(void)client;
    // ReplyHandler& rh        = ReplyHandler::get_instance(&server);
    // ReplyCode     replyCode = Nick::check_args(server, client, params);
    //
    // if (replyCode == CORRECT_FORMAT)
    //     return (new Nick(params));
    // else {
    //     rh.process_response(client, replyCode, params);
    // }
    //
    // return NULL;
	return new Nick(params);
}

ICommand* CmdFactory::user_cmd(Server& server, Client& client, std::string& params)
{
	(void)server;
	(void)client;
    // std::string   username = "", realname = "";
    // ReplyCode     replyCode = User::check_args(server, client, params);
    // ReplyHandler& rh        = ReplyHandler::get_instance(&server);
    //
    // if (replyCode == CORRECT_FORMAT) {
    //     std::istringstream iss(params);
    //     iss >> username;
    //     std::getline(iss, realname);
    //     if (!realname.empty() && realname[0] == ' ') {
    //         realname = realname.substr(1);
    //     }
    //     return (new User(username, realname));
    // } else {
    //     rh.process_response(client, replyCode, "USER");
    // }
    //
    // return NULL;
	return new User(params);
};

ICommand* CmdFactory::pass_cmd(Server& server, Client& client, std::string& params)
{
	(void)server;
	(void)client;
    // ReplyHandler& rh        = ReplyHandler::get_instance(&server);
    // ReplyCode     replyCode = Pass::check_args(server, client, params);
    //
    // if (replyCode == CORRECT_FORMAT) {
    //     return new Pass();
    // } else if (replyCode == PROCESSED_ERROR) {
    //     return NULL;
    // } else {
    //     rh.process_response(client, replyCode, params);
    // }
    //
    // return NULL;
	return new Pass(params);
};

ICommand* CmdFactory::kick_cmd(Server& server, Client& client, std::string& params)
{
	(void)server;
	(void)client;

	return new Kick(params);
};

ICommand* CmdFactory::quit_cmd(Server& server, Client& client, std::string& params)
{
    ReplyHandler& rh = ReplyHandler::get_instance(&server);

    ReplyCode replyCode = Quit::check_args(server, client, params);
    if (replyCode == CORRECT_FORMAT) {
        return new Quit(params);
    } else {
        rh.process_response(client, replyCode, params);
    }
    return NULL;
};

ICommand* CmdFactory::join_cmd(Server& server, Client& client, std::string& params)
{
	(void)server;
	(void)client;
    // ReplyHandler& rh        = ReplyHandler::get_instance(&server);
    // ReplyCode     replyCode = Join::check_args(server, client, params);
    // if (replyCode == CORRECT_FORMAT) {
    //     return new Join(params);
    // } else if (replyCode == ERR_NEEDMOREPARAMS) {
    //     rh.process_response(client, replyCode, "JOIN");
    // } else {
    //     rh.process_response(client, replyCode, params);
    // }
    // return NULL;
	return new Join(params);
};

ICommand* CmdFactory::mode_cmd(Server& server, Client& client, std::string& params)
{
    ReplyHandler&            rh = ReplyHandler::get_instance(&server);
    std::vector<std::string> vectorParams;
    std::istringstream       iss(params);
    std::string              token = "";
    while (std::getline(iss, token, ' ')) {
        vectorParams.push_back(token);
    }
    ReplyCode replyCode = Mode::check_args(server, client, vectorParams);
    if (replyCode == CORRECT_FORMAT) {
        return new Mode(vectorParams);
    } else if (replyCode == PROCESSED_ERROR) {
        return NULL;
    } else {
        rh.process_response(client, replyCode, params);
    }
    return NULL;
};

// NOT IMPLEMENTED YET

ICommand* CmdFactory::part_cmd(Server& server, Client& client, std::string& params)
{
    (void)client;
    (void)server;
	return new Part(params);
};

ICommand* CmdFactory::oper_cmd(Server& server, Client& client, std::string& params)
{
    (void)client;
    (void)server;
    (void)params;
    LOG_CMD.debug("Build OPER (not implemented)");
    return NULL;
};

ICommand* CmdFactory::invite_cmd(Server& server, Client& client, std::string& params)
{
	(void)client;
	(void)server;
    // ReplyHandler& rh        = ReplyHandler::get_instance(&server);
    // ReplyCode     replyCode = Invite::check_args(server, client, params);
    // if (replyCode != CORRECT_FORMAT) {
    //     rh.process_response(client, replyCode, params);
    //     return NULL;
    // }
    return new Invite(params);
};

ICommand* CmdFactory::who_cmd(Server& server, Client& client, std::string& params)
{
    ReplyHandler& rh        = ReplyHandler::get_instance(&server);
    ReplyCode     replyCode = Who::check_args(server, client, params);
    if (replyCode == CORRECT_FORMAT) {
        return new Who(params);
    } else {
        rh.process_response(client, replyCode, params);
    }
    return NULL;
}

ICommand* CmdFactory::privmsg_cmd(Server& server, Client& client, std::string& params)
{
	(void)client;
	(void)server;
    // ReplyHandler rh   = ReplyHandler::get_instance(&server);
    // ReplyCode    code = Privmsg::check_args(server, client, params);
    // if (code == ERR_NOTEXTTOSEND) {
    //     rh.process_response(client, code);
    //     return (NULL);
    // } else if (code == ERR_NEEDMOREPARAMS) {
    //     rh.process_response(client, code, "PRIVMSG");
    //     return (NULL);
    // } else if (code != CORRECT_FORMAT) {
    //     rh.process_response(client, code, params, NULL);
    //     return (NULL);
    // }
    Privmsg* privmsg = new Privmsg(params);
    return privmsg;
};

ICommand* CmdFactory::ping_cmd(Server& server, Client& client, std::string& params)
{
    (void)server;
    (void)client;
    ReplyHandler rh   = ReplyHandler::get_instance(&server);
    ReplyCode    code = Ping::check_args(server, client, params);
    if (code != CORRECT_FORMAT) {
        rh.process_response(client, code, params);
        return NULL;
    }
    Ping* cmd = new Ping(params);
    return cmd;
};

ICommand* CmdFactory::motd_cmd(Server& server, Client& client, std::string& params)
{
    (void)server;
    (void)client;
    return new Motd(params);
};

ICommand* CmdFactory::topic_cmd(Server& server, Client& client, std::string& params)
{
    (void)server;
    (void)client;
    ReplyHandler rh   = ReplyHandler::get_instance(&server);
    ReplyCode    code = Topic::check_args(server, client, params);
    if (code == PROCESSED_ERROR) {
        return NULL;
    } else if (code == ERR_NEEDMOREPARAMS) {
        rh.process_response(client, code, "TOPIC");
        return NULL;
    } else if (code != CORRECT_FORMAT) {
        rh.process_response(client, code, params);
        return NULL;
    }
    return new Topic(server, params);
};
