#include "CmdFactory.hpp"
#include "Server.hpp"
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

// Return a NICK command object if the nickname is valid
// throw an exception if not with appropriate error code (to be improved)
ACommand* CmdFactory::nickCmd(Server& server, Client& client, std::string& params)
{
    (void)client;
    std::istringstream iss(params);
    std::string        nickname;

    LOG_CMD.debug("Create NICK command");
    iss >> nickname;
    if (nickname.empty()) {
        // server.sendToClient(client.getFd(), response);
        throw std::invalid_argument("431 No nickname given");
    } else if (!std::isalpha(nickname[0])) {
        // server.sendToClient(client.getFd(), response);
        throw std::invalid_argument("432 No nickname given");
    }
    // silently shorten nickname if too long
    if (nickname.length() > NICKNAME_MAX_LEN)
        nickname = nickname.substr(0, NICKNAME_MAX_LEN);
    if (server.findClientByNickname(nickname)) {
        // Should send a responde :server.name 433 * dan :Nickname is already in use
        // server.sendToClient(client.getFd(), response);
        throw std::invalid_argument("433 ERR_NICKNAMEINUSE");
    }

    return new Nick(nickname);
}

// NOT IMPLEMENTED YET
ACommand* CmdFactory::userCmd(Server& server, Client& client, std::string& line)
{
    (void)client;
    (void)server;
    (void)line;
    LOG_CMD.debug("Build USER (not implemented)");
    return NULL;
};
ACommand* CmdFactory::passCmd(Server& server, Client& client, std::string& line)
{
    (void)client;
    (void)server;
    (void)line;
    LOG_CMD.debug("Build PASS (not implemented)");
    return NULL;
};
ACommand* CmdFactory::quitCmd(Server& server, Client& client, std::string& line)
{
    (void)client;
    (void)server;
    (void)line;
    LOG_CMD.debug("Build QUIT (not implemented)");
    return NULL;
};
ACommand* CmdFactory::joinCmd(Server& server, Client& client, std::string& line)
{
    (void)client;
    (void)server;
    (void)line;
    LOG_CMD.debug("Build JOIN (not implemented)");
    return NULL;
};
ACommand* CmdFactory::partCmd(Server& server, Client& client, std::string& line)
{
    (void)client;
    (void)server;
    (void)line;
    LOG_CMD.debug("Build PART (not implemented)");
    return NULL;
};
ACommand* CmdFactory::modeCmd(Server& server, Client& client, std::string& line)
{
    (void)client;
    (void)server;
    (void)line;
    LOG_CMD.debug("Build MODE (not implemented)");
    return NULL;
};
ACommand* CmdFactory::operCmd(Server& server, Client& client, std::string& line)
{
    (void)client;
    (void)server;
    (void)line;
    LOG_CMD.debug("Build OPER (not implemented)");
    return NULL;
};
ACommand* CmdFactory::inviteCmd(Server& server, Client& client, std::string& line)
{
    (void)client;
    (void)server;
    (void)line;
    LOG_CMD.debug("Build INVITE (not implemented)");
    return NULL;
};

ACommand* CmdFactory::makeCommand(Server& server, Client& client, std::string& line)
{

    std::string        command_line;
    std::istringstream iss(line);
    std::string available[COMMANDS_NB] = {"USER", "PASS", "NICK", "QUIT", "INVITE", "JOIN", "PART", "MODE", "OPER"};
    ACommand* (CmdFactory::*ptr[COMMANDS_NB])(Server&, Client&, std::string&) = {
        &CmdFactory::userCmd, &CmdFactory::passCmd, &CmdFactory::nickCmd, &CmdFactory::quitCmd, &CmdFactory::inviteCmd,
        &CmdFactory::joinCmd, &CmdFactory::partCmd, &CmdFactory::modeCmd, &CmdFactory::operCmd};
    iss >> command_line;
    for (size_t i = 0; i < COMMANDS_NB; i++) {
        if (command_line == available[i]) { // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
            std::string params;
            std::getline(iss, params);
            if (!params.empty() && params[0] == ' ') {
                params = params.substr(1);
            }
            return (this->*ptr[i])(server, client, params); // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        }
    }
    throw std::invalid_argument("Command not handled");
}
