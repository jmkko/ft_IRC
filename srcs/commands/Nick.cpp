#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "Nick.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <iostream>

Nick::Nick(const std::string& nickname) : _nickname(nickname) {}

Nick::~Nick(void) {}

/**
 * @brief set nickname or change nickname
 *
 * @if this command is done after USER and it is the first use
 * - send welcome message sequence to client
 * 001    RPL_WELCOME
 *          "Welcome to the Internet Relay Network
 *          <nick>!<user>@<host>"
 * 002    RPL_YOURHOST
 *          "Your host is <servername>, running version <ver>"
 * 003    RPL_CREATED
 *          "This server was created <date>"
 * 004    RPL_MYINFO
 *          "<servername> <version> <available user modes>
 *          <available channel modes>"
 *        - The server sends Replies 001 to 004 to a user upon
 *          successful registration.
 *
 * @param server not used
 * @param client
 */
void Nick::execute(Server& server, Client& client)
{
    (void)server;
    std::string   oldNickname = client.get_nickname();
    ReplyHandler& rh          = ReplyHandler::get_instance(&server);
    LOG_DV_CMD(_nickname);

    // welcome sequence complete for first time
    if (oldNickname.empty() && !client.get_user_name().empty()) {
        LOG_dt_CMD("Nick after USER");
        client.set_status(REGISTERED);
        rh.process_response(client,
                            RPL_WELCOME,
                            "",
                            NULL,
                            ircCodes.trailing(RPL_WELCOME) + " " + _nickname + "!" + client.get_user_name() + "@localhost");
        rh.process_response(client, RPL_YOURHOST, "", NULL, ircCodes.trailing(RPL_YOURHOST) + " " + server.get_name());
        rh.process_response(client, RPL_CREATED);
        rh.process_response(client, RPL_MYINFO, "", NULL, server.get_name() + " 1.0  0 0");
        // normal success behavior
    } else if (!oldNickname.empty() && !client.get_user_name().empty() && client.is_registered()) {
        // send the message to every user in every channel that this client takes part in
        client.broadcast_to_all_channels(server, TRANSFER_NICK, _nickname); // ! \\ ;
    }
    client.set_nickname(_nickname);
}

/**
 * @brief check the args of command NICK
 *
 * @param server
 * @param client not used
 * @param params should match `<nickname>`
 * @remark grammar rule
 * - nickname   =  ( letter / special ) *8( letter / digit / special / "-" )
 *
 *
 * @return @ref ReplyCode
 */
ReplyCode Nick::check_args(Server& server, Client& client, std::string& params)
{
    (void)client;
    std::istringstream iss(params);
    std::string        nickname;

    iss >> nickname;
    LOG_DTV_CMD(nickname);
    if (nickname.empty()) {
        return (ERR_NONICKNAMEGIVEN);
    } else if (!std::isalpha(nickname[0]) && !is_special_abnf(nickname[0])) {
        return (ERR_ERRONEUSNICKNAME);
    } else if (nickname.length() > ircConfig.get_nickname_max_len()) {
        nickname = nickname.substr(0, ircConfig.get_nickname_max_len());
    }
    // TODO what if the user wants to change nickname ? -> move to execute with proper checks
    // checks args checks params
    // execute checks logic involving server or client ?
    if (server.find_client_by_nickname(nickname)) {
        return (ERR_NICKNAMEINUSE);
    }
    params = nickname;
    return (CORRECT_FORMAT);
}
