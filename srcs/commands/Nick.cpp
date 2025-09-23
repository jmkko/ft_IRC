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
    if (old_nickname.empty() && !client.getUserName().empty() && client.getStatus() == REGISTERED) {
        LOG_CMD.info("001 RPL_WELCOME");
		rh.sendReply(client, RPL_WELCOME, client.getNickName(), "Welcome to Hasardous IRC SeRVER");
    } else if (!old_nickname.empty() && !client.getUserName().empty() && client.getStatus() == REGISTERED) {
		rh.sendReply(client, RPL_SUCCESS, old_nickname + "!" + client.getUserName() + "@" + SERVER_NAME + " NICK " + _nickname, "");
    } else {
        LOG_CMD.info("??? RPL_NICK");
	}
}

int Nick::checkArgs(Server& server, Client& client, std::string& params)
{
    (void)client;
    std::istringstream iss(params);
    std::string        nickname;

    iss >> nickname;
    if (nickname.empty()) {
        LOG_CMD.error("431 ERR_NONICKNAMEGIVEN");
        return (ERR_NONICKNAMEGIVEN);
    } else if (!std::isalpha(nickname[0])) {
        LOG_CMD.error("431 ERR_ERRONEUSNICKNAME");
        return (ERR_ERRONEUSNICKNAME);
    } else if (nickname.length() > NICKNAME_MAX_LEN) {
        nickname = nickname.substr(0, NICKNAME_MAX_LEN);
    }
    if (server.findClientByNickname(nickname)) {
        LOG_CMD.error("431 ERR_NICKNAMEINUSE");
        return (ERR_NICKNAMEINUSE);
    }
    params = nickname;
    if (client.getNickName().empty() && !client.getUserName().empty() &&
        client.getStatus() == REGISTERED) {
        return (RPL_WELCOME);
    }
    return (RPL_SUCCESS);
}
