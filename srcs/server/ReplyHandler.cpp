#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

ReplyHandler::ReplyHandler(Server* server) : _server(server) {}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

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
 * @param nickname [TODO:if not given, id will be contruct with client.get_nickname()]
 * @return [TODO: string <nick>!<user>@<host>]
 */
std::string ReplyHandler::get_user_id_of(Client& client)
{
    std::string identity(":");

    // if (nickname.empty()) {
    //     identity += client.get_nickname();
    // } else {
    //     identity += nickname;
    // }
    identity += "!" + client.get_user_name() + "@" + ircConfig.get_name();

    return (identity);
}

std::string ReplyHandler::select_response(Client& client, ReplyCode code, const std::string& parameters, Client* sender)
{
    std::string response(":" + ircConfig.get_name() + " ");
    std::string responseWithCodeAndNick = ":" + ircConfig.get_name() + " " + utils::code_to_str(code) + " " + client.get_nickname() + " ";
    LOG_D_CMD("resp w code and nick", responseWithCodeAndNick);
    if (!sender)
        sender = &client;
    switch (code) {
    case TRANSFER_NICK: // should be sent before effective nick change otherwise it wont have format ":old!u@h NICK :new"
        return (get_user_id_of(*sender) + "NICK " + parameters);
    case TRANSFER_JOIN:
        return (get_user_id_of(*sender) + "JOIN " + parameters);
    case TRANSFER_PRIVMSG:
        return (get_user_id_of(*sender) + "PRIVMSG " + parameters);
    case TRANSFER_KICK:
        return (get_user_id_of(*sender)+ "KICK " + parameters);
    case TRANSFER_INVITE:
        return (get_user_id_of(*sender) + "INVITE " + parameters);
    case TRANSFER_QUIT:
        return (get_user_id_of(*sender)+ "QUIT " + parameters);
    case TRANSFER_MODE:
        return (get_user_id_of(*sender)+ "MODE " + parameters);
    case MSG_PING:
        return response + "PONG " + parameters; // PONG :token
    case RPL_CHANNELMODEIS:
    case RPL_ENDOFNAMES:
    case RPL_ENDOFWHO:
    case RPL_INVITING:
    case RPL_NAMREPLY:
    case RPL_NOTOPIC:
    case RPL_TOPIC:
    case RPL_WELCOME:
    case RPL_WHOREPLY:
    case ERR_ALREADYREGISTRED:
    case ERR_BADCHANMASK:
    case ERR_BADCHANNELKEY:
    case ERR_BANNEDFROMCHAN:
    case ERR_CHANNELISFULL:
    case ERR_CHANOPRIVSNEEDED:
    case ERR_ERRONEUSNICKNAME:
    case ERR_KEYSET:
    case ERR_INVITEONLYCHAN:
    case ERR_NEEDMOREPARAMS:
    case ERR_NICKNAMEINUSE:
    case ERR_NOORIGIN:
    case ERR_NOSUCHCHANNEL:
    case ERR_NOSUCHNICK:
    case ERR_NOTEXTTOSEND:
    case ERR_NONICKNAMEGIVEN:
    case ERR_NOTONCHANNEL:
    case ERR_NOTREGISTERED:
    case ERR_PASSWDMISMATCH:
    case ERR_TOOMANYTARGETS:
    case ERR_UNKNOWNCOMMAND:
    case ERR_UNKNOWNMODE:
    case ERR_USERNOTINCHANNEL:
    case ERR_USERONCHANNEL:
    case CUSTOMERR_WRONG_FORMAT:
        return (responseWithCodeAndNick + parameters + ircCodes.trailing(code));    
    default:
        return ("");
    }
}

int ReplyHandler::process_response(Client& client, ReplyCode code, const std::string& parameters, Client* sender)
{
    std::string response = select_response(client, code, parameters, sender);

    if (!response.empty()) {
        LOG_CMD.sending(__FILE_NAME__, __FUNCTION__, "\n" + response, &client);
        _send_reply(client, response);
    }
    return (code);
}

/**
 * @brief send RFC_2812 formmated message to the client
 *
 * @param client who waiting response from the server
 * @param code of response
 * @param parameters message corresponding of the code
 * @return
 */
int ReplyHandler::process_code_response(Client& client, ReplyCode code, const std::string& parameters)
{
    std::string response(":" + ircConfig.get_name());
    response = response + utils::code_to_str(code) + " " + parameters;

    if (!response.empty()) {
        LOG_CMD.info("ReplyHandler::process_response --> response to " + client.get_nickname() + ":\n" + response);
        _send_reply(client, response);
    }
    return (code);
}

void ReplyHandler::_send_reply(Client& client, const std::string& msg)
{
    client.append_to_send_buffer(msg + "\r\n");
    _server->add_events_of(client, POLLOUT);
}

/*************************************************************
 *		👁️‍ GETTERS and SETTERS				 				*
 *************************************************************/

ReplyHandler& ReplyHandler::get_instance(Server* s)
{
    static ReplyHandler instance(s);
    return instance;
}
