#include "User.hpp"

#include "Client.hpp"
#include "LogManager.hpp"
#include "Motd.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <algorithm>

/************************************************************
 *              🥚 CONSTRUCTORS & DESTRUCTOR                *
 ************************************************************/

User::User(std::string& params)
{
    Parser parser;

    _username = parser.format_parameter(params, NULL);
    _mode     = parser.format_parameter(params, NULL);
    _unused   = parser.format_parameter(params, NULL);
    _realname = parser.format_parameter(params, NULL);
}

User::~User(void) {}

/*************************************************************
 *                     🛠️ METHODS                            *
 *************************************************************/

void User::execute(Server& server, Client& client)
{
    Parser p(server, client);

    if (_realname.empty()) {
        p.response(ERR_NEEDMOREPARAMS, "USER");
        return;
    }
    if (std::count_if(_username.begin(), _username.end(), Utils::is_invalid_char_user)) {
        p.response(ERR_NEEDMOREPARAMS, "USER");
        return;
    }
    size_t start = _realname.find_first_not_of(WHITE_SPACE);
    if (start == std::string::npos) {
        p.response(ERR_NEEDMOREPARAMS, "USER");
        return;
    }
    size_t end = _realname.find_last_not_of(WHITE_SPACE);
    _realname  = _realname.substr(start, end - start + 1);
    if (!client.get_user_name().empty() && !client.get_nickname().empty() && client.is_registered()) {
        p.response(ERR_ALREADYREGISTRED);
        return;
    }

    client.set_user_name(_username);
    client.set_real_name(_realname);
    LOG_DV_CMD(_realname);
    if (!client.get_nickname().empty()) {
        client.set_status(REGISTERED);
        p.rh->process_welcome(server, client);
#ifndef TEST
        if (client.get_nickname() == "bot")
            return;
        Motd motd("");
        motd.execute(server, client);
#endif
    } else {
        LOG_CMD.info("202 RPL_USER");
    }
}
