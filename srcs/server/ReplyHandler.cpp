#include "ReplyHandler.hpp"

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

ReplyHandler::ReplyHandler(Server* server) : _server(server) {}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

static const std::string codeToStr(ReplyCode code)
{
    std::stringstream ss;
    std::string       codeStr;
    ss << std::setw(3) << std::setfill('c') << code;
    return ss.str();
}

// TODO pass clientIndex in args ?
void ReplyHandler::sendReply(Client* client, ReplyCode code, const std::string& target,
                             const std::string& trailing)
{
    std::string reply = std::string(": ") + SERVER_NAME + codeToStr(code) + " " + target;
    if (!trailing.empty()) {
        reply += " :" + trailing;
    }
    reply + "\r\n";
    _server->sendTo(client, reply);
}

void ReplyHandler::sendErrNeedMoreParams(Client *client, const std::string& commandName)
{
    sendReply(client, ERR_NEEDMOREPARAMS, commandName, "Not enough parameters");
}

/*************************************************************
 *		👁️‍ GETTERS and SETTERS				 				*
 *************************************************************/

ReplyHandler& ReplyHandler::getInstance(Server* s)
{
    static ReplyHandler instance(s);
    return instance;
}
