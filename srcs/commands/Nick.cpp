#include "Nick.hpp"
#include "Client.hpp"
#include "LogManager.hpp"
#include "Server.hpp"

Nick::Nick(const std::string& nickname) : _nickname(nickname) {}

// Copy constructor
// Nick::Nick(const Nick& other) { (void)other; }

// Assignment operator overload
// Nick& Nick::operator=(const Nick& other)
// {
//     (void)other;
//     return (*this);
// }

// Destructor
Nick::~Nick(void) {}

void Nick::execute(Server& server, Client& client)
{
    (void)server;
    std::string old_nickname = client.getNickName();
    client.setNickName(_nickname);
	ReplyHandler& rh = ReplyHandler::getInstance(&server);
	// welcome sequence complete for first time
    if (old_nickname.empty() && !client.getUserName().empty() && client.isRegistered()) {
        LOG_CMD.info("001 RPL_WELCOME");
		rh.processResponse(client, RPL_WELCOME);
	// normal success behavior
    } else if (!old_nickname.empty() && !client.getUserName().empty() && client.isRegistered()) {
        LOG_CMD.info("204 RPL_NICK");
		rh.processResponse(client, RPL_NICK, old_nickname);
		rh.processResponse(client, RPL_NOTICE, "Your nickname has changed"); // DOESNT NEED TO BE THERE (just for fun)
	}
}

ReplyCode Nick::checkArgs(Server& server, Client& client, std::string& params)
{
    (void)client;
    std::istringstream iss(params);
    std::string        nickname;

    iss >> nickname;
    if (nickname.empty()) {
        LOG_CMD.error("431 ERR_NONICKNAMEGIVEN");
        return (ERR_NONICKNAMEGIVEN);
    } else if (!std::isalpha(nickname[0])) {
        LOG_CMD.error("432 ERR_ERRONEUSNICKNAME");
        return (ERR_ERRONEUSNICKNAME);
    } else if (nickname.length() > irc_config.get_nicknameMaxLen()) {
        nickname = nickname.substr(0, irc_config.get_nicknameMaxLen());
    }
    if (server.findClientByNickname(nickname)) {
        LOG_CMD.error("431 ERR_NICKNAMEINUSE");
        return (ERR_NICKNAMEINUSE);
    }
    params = nickname;
    if (client.getNickName().empty() && !client.getUserName().empty() &&
        client.isRegistered()) {
        return (RPL_WELCOME);
    }
    return (RPL_SUCCESS);
}
