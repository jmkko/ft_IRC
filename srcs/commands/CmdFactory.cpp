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
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "Topic.hpp"
#include "User.hpp"
#include "Who.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <iostream>
#include <sstream>

// Default constructor
CmdFactory::CmdFactory(void) {}

// Copy constructor
CmdFactory::CmdFactory(const CmdFactory& other) { (void)other; }

// Assignment operator overload
CmdFactory& CmdFactory::operator=(const CmdFactory& other)
{
    (void)other;
    return (*this);
}

// Destructor
CmdFactory::~CmdFactory(void) {}

bool CmdFactory::check_in(Client& client, std::string& command)
{
    if (!client.is_registered() && command != "NICK" && command != "USER" && command != "PASS" && command != "QUIT") {
        return false;
    }
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
            LOG_d_CMD(commandLine);
            if (!check_in(client, commandLine)) {
                rh.process_response(client, ERR_NOTREGISTERED, commandLine);
                return NULL;
            }
            std::string params;
            std::getline(iss, params);
            // LOG_CMD.debug("make_command : params : " + params);
            if (!params.empty() && params[0] == ' ') {
                params = params.substr(1);
            }
            return (this->*ptr[i])(server, client, params);
        }
    }
    rh.process_response(client, ERR_UNKNOWNCOMMAND, params);

    return NULL;
}

// Return a NICK command object if the nickname is valid
ICommand* CmdFactory::nick_cmd(Server& server, Client& client, std::string& params)
{
    ReplyHandler& rh        = ReplyHandler::get_instance(&server);
    ReplyCode     replyCode = Nick::check_args(server, client, params);

    if (replyCode == CORRECT_FORMAT)
        return (new Nick(params));
    else {
        rh.process_response(client, replyCode, params);
    }

    return NULL;
}

ICommand* CmdFactory::user_cmd(Server& server, Client& client, std::string& params)
{
    std::string   username = "", realname = "";
    ReplyCode     replyCode = User::check_args(server, client, params);
    ReplyHandler& rh        = ReplyHandler::get_instance(&server);

    if (replyCode == CORRECT_FORMAT) {
        std::istringstream iss(params);
        iss >> username;
        std::getline(iss, realname);
        if (!realname.empty() && realname[0] == ' ') {
            realname = realname.substr(1);
        }
        return (new User(username, realname));
    } else {
        rh.process_response(client, replyCode, params);
    }

    return NULL;
};

ICommand* CmdFactory::pass_cmd(Server& server, Client& client, std::string& params)
{
    ReplyHandler& rh        = ReplyHandler::get_instance(&server);
    ReplyCode     replyCode = Pass::check_args(server, client, params);

    if (replyCode == CORRECT_FORMAT) {
        return new Pass(params);
    } else {
        rh.process_response(client, replyCode, params);
    }

    return NULL;
};

ICommand* CmdFactory::kick_cmd(Server& server, Client& client, std::string& params)
{
    ReplyHandler&            rh = ReplyHandler::get_instance(&server);
    std::vector<std::string> vectorParams;
    std::istringstream       iss(params);
    std::string              token;
    while (std::getline(iss, token, ' ')) {
        vectorParams.push_back(token);
        LOG_CMD.debug("factory token", token);
    }
    ReplyCode replyCode = Kick::check_args(server, client, vectorParams);
    if (replyCode == CORRECT_FORMAT) {
        return new Kick(vectorParams);
    } else {
        rh.process_response(client, replyCode, "KICK");
    }
    return NULL;
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
    ReplyHandler&            rh = ReplyHandler::get_instance(&server);
    std::vector<std::string> vectorParams;
    vectorParams.push_back(params);
    ReplyCode replyCode = Join::check_args(server, client, vectorParams);
    if (replyCode == CORRECT_FORMAT) {
        return new Join(vectorParams);
    } else if (replyCode == ERR_NEEDMOREPARAMS) {
        rh.process_response(client, replyCode, "JOIN");
    } else {
        rh.process_response(client, replyCode, params);
    }
    return NULL;
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
    (void)params;
    LOG_CMD.debug("Build PART (not implemented)");
    return NULL;
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
    ReplyHandler& rh        = ReplyHandler::get_instance(&server);
    ReplyCode     replyCode = Invite::check_args(server, client, params);
    if (replyCode != CORRECT_FORMAT) {
        rh.process_response(client, replyCode, params);
        return NULL;
    }
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
    LOG_CMD.debug("PIVMSG params: " + params);
    ReplyHandler rh   = ReplyHandler::get_instance(&server);
    ReplyCode    code = Privmsg::check_args(server, client, params);
    if (code == ERR_NOTEXTTOSEND) {
        rh.process_response(client, code, "");
        return (NULL);
    } else if (code != CORRECT_FORMAT) {
        rh.process_response(client, code, params, NULL);
        return (NULL);
    }

    std::string            msg;
    std::string::size_type pos = params.find(" :");
    if (pos != std::string::npos) {
        msg = params.substr(pos);
    }

    Privmsg* privmsg = new Privmsg(msg);

    // std::string                               word;
    // std::istringstream                        iss(params);
    // Client*                                   dest = NULL;
    // std::map<std::string, Channel*>::iterator chan;
    privmsg->build_args(server, params);
    // while (iss >> word) {
    //     chan = server.channels.find(word);
    //     if (chan != server.channels.end()) {
    //         privmsg->add_channel(chan->second);
    //     }
    //     dest = server.find_client_by_nickname(word);
    //     if (dest) {
    //         privmsg->add_client(dest);
    //     }
    // }

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
    if (code == ERR_NEEDMOREPARAMS) {
        rh.process_response(client, code, "TOPIC");
        return NULL;
    } else if (code != CORRECT_FORMAT) {
        rh.process_response(client, code, params);
        return NULL;
    }
    return new Topic(server, params);
};
