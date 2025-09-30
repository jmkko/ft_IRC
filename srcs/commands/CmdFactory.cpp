#include "CmdFactory.hpp"

#include "Client.hpp"
#include "ICommand.hpp"
#include "Join.hpp"
#include "LogManager.hpp"
#include "Nick.hpp"
#include "Pass.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "User.hpp"
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

ICommand* CmdFactory::make_command(Server& server, Client& client, std::string& params)
{
    ReplyHandler&      rh          = ReplyHandler::get_instance(&server);
    std::string        commandLine = "";
    std::istringstream iss(params); // NOLINT(clang-diagnostic-vexing-parse)
    std::string        available[NB_AVAILABLE_CMD]
        = {"USER", "PASS", "NICK", "QUIT", "INVITE", "JOIN", "PART", "MODE", "OPER"};
    ICommand* (CmdFactory::* ptr[NB_AVAILABLE_CMD])(Server&, Client&, std::string&)
        = {&CmdFactory::user_cmd,
           &CmdFactory::pass_cmd,
           &CmdFactory::nick_cmd,
           &CmdFactory::quit_cmd,
           &CmdFactory::invite_cmd,
           &CmdFactory::join_cmd,
           &CmdFactory::part_cmd,
           &CmdFactory::mode_cmd,
           &CmdFactory::oper_cmd};

    iss >> commandLine;
    for (size_t i = 0; i < NB_AVAILABLE_CMD; i++) {
        if (commandLine == available[i]) {
            LOG_CMD.debug("command params" + commandLine);
            // rergisteredcheck
            if (!client.is_registered() && commandLine != "NICK" && commandLine != "USER"
                && commandLine != "PASS" && commandLine != "QUIT") {
                LOG_CMD.info(TO_STRING(ERR_NOTREGISTERED) + " ERR_NOTREGISTERED");
                ReplyHandler& rh = ReplyHandler::get_instance(&server);
                rh.process_response(client, ERR_NOTREGISTERED, commandLine);
            }
            std::string params;
            std::getline(iss, params);
            if (!params.empty() && params[0] == ' ') {
                params = params.substr(1);
            }
            return (this->*ptr[i])(server, client, params);
        }
    }
    LOG_CMD.error("421 ERR_UNKNOWNCOMMAND");
    rh.process_response(client, ERR_UNKNOWNCOMMAND, params);

    return NULL;
}

// Return a NICK command object if the nickname is valid
ICommand* CmdFactory::nick_cmd(Server& server, Client& client, std::string& params)
{
    ReplyHandler& rh        = ReplyHandler::get_instance(&server);
    ReplyCode     replyCode = Nick::check_args(server, client, params);

    if (replyCode == RPL_SUCCESS || replyCode == RPL_WELCOME)
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

    if (replyCode == RPL_WELCOME || replyCode == RPL_SUCCESS) {
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

    if (replyCode == RPL_SUCCESS || replyCode == RPL_WELCOME) {
        return new Pass(params);
    } else {
        rh.process_response(client, replyCode, params);
    }

    return NULL;
};

// NOT IMPLEMENTED YET

ICommand* CmdFactory::quit_cmd(Server& server, Client& client, std::string& params)
{
    (void)client;
    (void)server;
    (void)params;
    LOG_CMD.debug("Build QUIT (not implemented)");
    return NULL;
};

ICommand* CmdFactory::join_cmd(Server& server, Client& client, std::string& params)
{
    (void)client;
    (void)server;
    ReplyHandler&            rh = ReplyHandler::get_instance(&server);
    std::vector<std::string> vectorParams;
    vectorParams.push_back(params);
    ReplyCode replyCode = Join::check_args(server, client, vectorParams);
    if (replyCode == RPL_SUCCESS) {
        return new Join(vectorParams);
    } else {
        rh.process_response(client, replyCode, params);
    }
    return NULL;
};

ICommand* CmdFactory::part_cmd(Server& server, Client& client, std::string& params)
{
    (void)client;
    (void)server;
    (void)params;
    LOG_CMD.debug("Build PART (not implemented)");
    return NULL;
};

ICommand* CmdFactory::mode_cmd(Server& server, Client& client, std::string& params)
{
    (void)client;
    (void)server;
    (void)params;
    LOG_CMD.debug("Build MODE (not implemented)");
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
    (void)client;
    (void)server;
    (void)params;
    LOG_CMD.debug("Build INVITE (not implemented)");
    return NULL;
};
