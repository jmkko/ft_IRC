#include "ReplyHandler.hpp"
#include "reply_codes.hpp"
#include <string>
#include <cstdio>

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
    ss << " " << std::setw(3) << std::setfill('0') << code;
	ss << " ";
    return ss.str();
}

/*
<message>  ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>
<prefix>   ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]
<command>  ::= <letter> { <letter> } | <number> <number> <number>
<SPACE>    ::= ' ' { ' ' }
<params>   ::= <SPACE> [ ':' <trailing> | <middle> <params> ]

<middle>   ::= <Any *non-empty* sequence of octets not including SPACE
               or NUL or CR or LF, the first of which may not be ':'>
<trailing> ::= <Any, possibly *empty*, sequence of octets not including
                 NUL or CR or LF>
 
<crlf>     ::= CR LF

  1)    <SPACE> is consists only of SPACE character(s) (0x20).
        Specially notice that TABULATION, and all other control
        characters are considered NON-WHITE-SPACE.

  2)    After extracting the parameter list, all parameters are equal,
        whether matched by <middle> or <trailing>. <Trailing> is just
        a syntactic trick to allow SPACE within parameter.

  3)    The fact that CR and LF cannot appear in parameter strings is
        just artifact of the message framing. This might change later.

  4)    The NUL character is not special in message framing, and
        basically could end up inside a parameter, but as it would
        cause extra complexities in normal C string handling. Therefore
        NUL is not allowed within messages.

  5)    The last parameter may be an empty string.

  6)    Use of the extended prefix (['!' <user> ] ['@' <host> ]) must
        not be used in server to server communications and is only
        intended for server to client messages in order to provide
        clients with more useful information about who a message is
        from without the need for additional queries.
*/


/**
 * @brief [TODO:return the id sequence <nick|parameter>!<user>@<host>]
 *
 * @param client [TODO:Client]
 * @param nickname [TODO:if not given, id will be contruct with client.getNickName()]
 * @return [TODO: string <nick>!<user>@<host>]
 */
std::string ReplyHandler::getIdOf(Client& client, std::string nickname) {
	std::string identity(":");

	if (nickname.empty()) {
		identity += client.getNickName();
	} else {
		identity += nickname;
	}
	identity += "!" + client.getUserName() + "@" + irc_config.get_name();

	return (identity);
}

std::string	ReplyHandler::selectResponse(Client& client, ReplyCode code, const std::string& parameters)
{
	std::string response(":" + irc_config.get_name());

	switch (code) {
		case RPL_WELCOME:
			return (response + codeToStr(code) + client.getNickName() + RPL_WELCOME_MSG);
		case RPL_NICK:
			return (getIdOf(client, parameters) + " NICK " + client.getNickName());
		case RPL_NOTICE:
			return (response + " NOTICE " + client.getNickName() + " :" + parameters);
		case RPL_KICK:
			return "";
		case RPL_INVITING:
			return "";
		case RPL_MODE:
			return "";
		case RPL_TOPIC:
			return "";
		case RPL_PRIVMSG:
			return "";
		case ERR_UNKNOWNCOMMAND:
			return (std::string("421") + ERR_UNKNOWNCOMMAND_MSG + parameters);
		case ERR_NEEDMOREPARAMS:
			return (std::string("461") + ERR_NEEDMOREPARAMS_MSG + parameters);
		case ERR_NONICKNAMEGIVEN:
			return (std::string("431") + ERR_NONICKNAMEGIVEN_MSG);
		case ERR_ERRONEUSNICKNAME:
			return (std::string("432") + ERR_ERRONEUSNICKNAME_MSG);
		case ERR_NICKNAMEINUSE:
			return (std::string("433") + ERR_ERRONEUSNICKNAME_MSG + parameters);
		case ERR_PASSWDMISMATCH:
			return (std::string("464") + ERR_PASSWDMISMATCH_MSG);
		case ERR_ALREADYREGISTERED:
			return (std::string("464") + ERR_ALREADYREGISTERED_MSG);
		default:
			return ("");
	}
}

int ReplyHandler::processResponse(Client& client, ReplyCode code, const std::string& parameters)
{
	std::string response = selectResponse(client, code, parameters);

	//to be implemented
	//if (code == RPL_NICK | code == RPL_TOPIC)
	//	broadcast(response, chanel);
	if (!response.empty()) {
		LOG_CMD.debug("add to sendBuffer: " + response);
		sendReply(client, response);
	}
	return (code);
}

void ReplyHandler::sendReply(Client& client, std::string& msg)
{
	client.appendToSendBuffer(msg + "\r\n");
	_server->addEventsOf(client, POLLOUT);
}

/*************************************************************
 *		👁️‍ GETTERS and SETTERS				 				*
 *************************************************************/

ReplyHandler& ReplyHandler::getInstance(Server* s)
{
    static ReplyHandler instance(s);
    return instance;
}
