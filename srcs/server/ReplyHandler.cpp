#include "ReplyHandler.hpp"

#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
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
 * @brief return client identifier formatted as nick!username@hostname
 *
 * @param client
 * @param nickname
 * @return formatted identifier
 */
static std::string get_user_id_of(Client& client)
{
    // if (nickname.empty()) {
    //     identity += client.get_nickname();
    // } else {
    //     identity += nickname;
    // }
    return ":" + client.get_nickname() + "!" + client.get_user_name() + "@" + ircConfig.get_name() + " ";
}

/**
 * @brief generates RFC_2812 formmated code response
 *
 * @param client who waiting response from the server
 * @param code of response
 * @param parameters message corresponding of the code
 * @return formatted code response
 */
static std::string
generate_code_response(Client& client, ReplyCode code, const std::string& parameters, const std::string& trailing)
{
    std::string nick          = client.get_nickname().empty() ? "*" : client.get_nickname();
    std::string numericPrefix = ":" + ircConfig.get_name() + " " + Utils::code_to_str(code) + " " + nick;

    std::string        trailingMessage, separatedParams;
    const std::string& defaultTrailing = ircCodes.trailing(code);

    if (!trailing.empty()) {
        trailingMessage = " :" + trailing;
    } else if (!defaultTrailing.empty()) {
        trailingMessage = " :" + defaultTrailing;
    }
    if (!parameters.empty()) {
        separatedParams = " " + parameters;
    }

    return (numericPrefix + separatedParams + trailingMessage);
}

static std::string generate_non_numerical_response(
    Client& client, ReplyCode code, const std::string& parameters, Client* sender, const std::string& trailing = "")
{
    if (!sender) {
        sender = &client;
    }

    std::string trailingMessage, separatedParams;
    if (!trailing.empty()) {
        trailingMessage = " :" + trailing;
    }
    if (!parameters.empty()) {
        separatedParams = " " + parameters;
    }

    switch (code) {
    case TRANSFER_NICK:
        return (get_user_id_of(*sender) + "NICK" + trailingMessage);
    case TRANSFER_JOIN:
        return (get_user_id_of(*sender) + "JOIN" + separatedParams);
    case TRANSFER_PRIVMSG:
        return (get_user_id_of(*sender) + "PRIVMSG" + separatedParams + trailingMessage);
    case TRANSFER_KICK:
        return (get_user_id_of(*sender) + "KICK" + separatedParams + trailingMessage);
    case TRANSFER_INVITE:
        return (get_user_id_of(*sender) + "INVITE" + separatedParams);
    case TRANSFER_QUIT:
        return (get_user_id_of(*sender) + "QUIT" + separatedParams);
    case TRANSFER_MODE:
        return (get_user_id_of(*sender) + "MODE" + separatedParams);
    case TRANSFER_TOPIC:
        return (get_user_id_of(*sender) + "TOPIC" + separatedParams + trailingMessage);
    case MSG_PING:
        return ":" + ircConfig.get_name() + " " + "PONG :" + parameters;
    default:
        return "";
    }
}

static bool is_numerical_response(ReplyCode code)
{
    return (code < LOWER_CUSTOM_CODE || (code > UPPER_CUSTOM_CODE && code < LOWER_CUSTOM_NONNUMERICAL_CODE));
}

int ReplyHandler::process_response(
    Client& client, ReplyCode code, const std::string& parameters, Client* sender, const std::string& trailing)
{
    LOG_DT_CMD("processing", ircCodes.str(code));
    std::string response = "";
    if (is_numerical_response(code)) {
        response = generate_code_response(client, code, parameters, trailing);
    } else
        response = generate_non_numerical_response(client, code, parameters, sender, trailing);

    if (!response.empty()) {
        LOG_CMD.sending(__FILE_NAME__, __FUNCTION__, "\n\t\t\t\t\t\t\t\t\t\t\t   " + response, &client);
        _send_reply(client, response);
    }
    return (code);
}
void ReplyHandler::process_welcome(Server& server, Client& client)
{
    process_response(client,
                     RPL_WELCOME,
                     "",
                     NULL,
                     ircCodes.trailing(RPL_WELCOME) + " " + client.get_nickname() + "!" + client.get_user_name() + "@localhost");
    process_response(client, RPL_YOURHOST, "", NULL, ircCodes.trailing(RPL_YOURHOST) + " " + server.get_name());
    process_response(client, RPL_CREATED);
    process_response(client, RPL_MYINFO, "", NULL, server.get_name() + " 1.0  0 0");
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
