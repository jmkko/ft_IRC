#include "Nick.hpp"

#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "Nick.hpp"
#include "Parser.hpp"
//#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>

Nick::Nick(std::string& params)
{
	Parser parser;
	_nickname = parser.format_parameter(params, NULL);
}

Nick::~Nick(void) {}

void Nick::execute(Server& server, Client& client)
{
    (void)server;
    std::string   oldNickname = client.get_nickname();
	Parser p(server, client);

    LOG_DV_CMD(_nickname);

    if (!p.correct_nickname(_nickname))
		return ;
    if (oldNickname.empty() && !client.get_user_name().empty()) {
        client.set_nickname(_nickname);
        LOG_dt_CMD("Nick after USER");
        client.set_status(REGISTERED);
        p.rh->process_welcome(server, client);
    } else if (!oldNickname.empty() && !client.get_user_name().empty() && client.is_registered()) {
        client.broadcast_to_all_channels(server, TRANSFER_NICK, "", _nickname); // ! \\ ;
    }
    client.set_nickname(_nickname);
}

ReplyCode Nick::check_args(Server& server, Client& client, std::string& params)
{
    (void)client;
    std::istringstream iss(params);
    std::string        nickname;
    size_t             invalidChar = 0;

    iss >> nickname;
    LOG_DTV_CMD(nickname);
    if (nickname.empty())
        return (ERR_NONICKNAMEGIVEN);
    invalidChar = std::count_if(nickname.begin(), nickname.end(), Utils::is_invalid_char_nick);
    if (invalidChar || std::isdigit(nickname[0])) {
        return (ERR_ERRONEUSNICKNAME);
    } else if (nickname.length() > ircConfig.get_nickname_max_len()) {
        nickname = nickname.substr(0, ircConfig.get_nickname_max_len());
    }
    if (server.find_client_by_nickname(nickname)) {
        return (ERR_NICKNAMEINUSE);
    }
    params = nickname;
    return (CORRECT_FORMAT);
}
