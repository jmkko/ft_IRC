#include "Ping.hpp"

#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "reply_codes.hpp"

#include <sstream>

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

Ping::Ping(const std::string& params) : _token(params) {}

Ping::~Ping(void) {}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

ReplyCode Ping::check_args(Server& server, Client& client, std::string& params)
{
    (void)server;
    (void)client;

    std::string        token;
    std::istringstream iss(params);
    iss >> token;

    if (token.empty())
        return ERR_NOORIGIN;
    params = token;
    return CORRECT_FORMAT;
}

void Ping::execute(Server& server, Client& client)
{
    ReplyHandler& rh = ReplyHandler::get_instance(&server);

    rh.process_response(client, MSG_PING, _token);
}
