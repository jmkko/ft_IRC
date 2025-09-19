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
void ReplyHandler::sendReply(int clientIndex, ReplyCode code, const std::string& target, const std::string& trailing)
{
    std::string reply = std::string(": ") + SERVER_NAME + codeToStr(code) + " " + target;
    if (!trailing.empty()) {
        reply += " :" + trailing;
    }
    reply + "\r\n";
    _server->sendToClient(clientIndex, reply);
}

void ReplyHandler::sendErrNeedMoreParams(int clientIndex, const std::string& commandName)
{
    sendReply(clientIndex, ERR_NEEDMOREPARAMS, commandName, "Not enough parameters");
}

/*************************************************************
 *		👁️‍ GETTERS and SETTERS				 				*
 *************************************************************/

ReplyHandler& ReplyHandler::getInstance(Server* s)
{
    static ReplyHandler instance(s);
    return instance;
}
