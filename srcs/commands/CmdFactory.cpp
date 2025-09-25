#include "CmdFactory.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"

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

ICommand* CmdFactory::makeCommand(Server& server, Client& client, std::string& line)
{
    std::string        command_line;
    std::istringstream iss(line);
    std::string        available[NB_AVAILABLE_CMD] = {
        "USER", "PASS", "NICK", "QUIT", "INVITE", "JOIN", "PART", "MODE", "OPER"};
    ICommand* (CmdFactory::* ptr[NB_AVAILABLE_CMD])(Server&, Client&, std::string&) = {
        &CmdFactory::userCmd,
        &CmdFactory::passCmd,
        &CmdFactory::nickCmd,
        &CmdFactory::quitCmd,
        &CmdFactory::inviteCmd,
        &CmdFactory::joinCmd,
        &CmdFactory::partCmd,
        &CmdFactory::modeCmd,
        &CmdFactory::operCmd};
    iss >> command_line;
    for (size_t i = 0; i < NB_AVAILABLE_CMD; i++) {
        if (command_line == available[i]) {
            LOG_CMD.debug("command line" + command_line);
            if (!client.isRegistered() && command_line != "NICK" && command_line != "USER" &&
                command_line != "PASS" && command_line != "QUIT") {
                LOG_CMD.error(TO_STRING(ERR_NOTREGISTERED) + " ERR_NOTREGISTERED");
                ReplyHandler& rh = ReplyHandler::getInstance(&server);
                rh.sendSimpleReply(
                    client, ERR_NOTREGISTERED, "* " + command_line + " :You have not registered");
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
    return NULL;
}

// Return a NICK command object if the nickname is valid
ICommand* CmdFactory::nickCmd(Server& server, Client& client, std::string& params)
{
    (void)server;
    int replyCode = Nick::checkArgs(server, client, params);
    if (replyCode == RPL_SUCCESS || replyCode == RPL_WELCOME)
        return (new Nick(params));
    return NULL;
}

ICommand* CmdFactory::userCmd(Server& server, Client& client, std::string& params)
{
    (void)client;
    (void)server;
    std::string username, realname;
    int         replyCode = User::checkArgs(server, client, params);
    if (replyCode == RPL_WELCOME || replyCode == RPL_SUCCESS) {
        std::istringstream iss(params);
        iss >> username;
        std::getline(iss, realname);
        if (!realname.empty() && realname[0] == ' ') {
            realname = realname.substr(1);
        }
        return (new User(username, realname));
    }
    return NULL;
};

ICommand* CmdFactory::passCmd(Server& server, Client& client, std::string& params)
{
    (void)client;
    (void)server;
    int replyCode = Pass::checkArgs(server, client, params);
    if (replyCode == RPL_SUCCESS || replyCode == RPL_WELCOME) {
        return new Pass(params);
    }
    return NULL;
};

// NOT IMPLEMENTED YET

ICommand* CmdFactory::quitCmd(Server& server, Client& client, std::string& line)
{
    (void)client;
    (void)server;
    (void)line;
    LOG_CMD.debug("Build QUIT (not implemented)");
    return NULL;
};

ICommand* CmdFactory::joinCmd(Server& server, Client& client, std::string& line)
{
    (void)client;
    (void)server;
    (void)line;
    LOG_CMD.debug("Build JOIN (not implemented)");
    return NULL;
};

ICommand* CmdFactory::partCmd(Server& server, Client& client, std::string& line)
{
    (void)client;
    (void)server;
    (void)line;
    LOG_CMD.debug("Build PART (not implemented)");
    return NULL;
};

ICommand* CmdFactory::modeCmd(Server& server, Client& client, std::string& line)
{
    (void)client;
    (void)server;
    (void)line;
    LOG_CMD.debug("Build MODE (not implemented)");
    return NULL;
};

ICommand* CmdFactory::operCmd(Server& server, Client& client, std::string& line)
{
    (void)client;
    (void)server;
    (void)line;
    LOG_CMD.debug("Build OPER (not implemented)");
    return NULL;
};

ICommand* CmdFactory::inviteCmd(Server& server, Client& client, std::string& line)
{
    (void)client;
    (void)server;
    (void)line;
    LOG_CMD.debug("Build INVITE (not implemented)");
    return NULL;
};
