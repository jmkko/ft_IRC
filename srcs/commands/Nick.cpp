#include "Nick.hpp"

#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <iostream>

Nick::Nick(const std::string& nickname) : _nickname(nickname) {}

Nick::~Nick(void) {}

void Nick::execute(Server& server, Client& client)
{
    (void)server;
    std::string   oldNickname = client.get_nickname();
    ReplyHandler& rh          = ReplyHandler::get_instance(&server);
    // welcome sequence complete for first time
    if (oldNickname.empty() && client.get_user_name().empty() && client.is_authenticated()) {
        LOG_dt_CMD("first change");
        // rh.process_response(client, RPL_WELCOME);
        // normal success behavior
    } else if (!oldNickname.empty() && !client.get_user_name().empty() && client.is_registered()) {
        // send the message to every user in every channel that this client takes part in
        // NOT WORKING (client._joinedChannel is empty)
        client.broadcast_to_all_channels(server, TRANSFER_NICK, _nickname); // ! \\ ;
        rh.process_response(client, TRANSFER_NICK, _nickname);
    }
    client.set_nickname(_nickname);
}

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
    // if (client.get_nickname().empty() && !client.get_user_name().empty() && client.is_registered()) {
    //     return (RPL_WELCOME);
    // }
    return (CORRECT_FORMAT);
}
