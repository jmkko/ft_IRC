#include "Ping.hpp"

#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "reply_codes.hpp"

#include <sstream>

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

Ping::Ping(void) {}

Ping::Ping(const std::string& token) : _token(token) {}

Ping::Ping(const Ping& other) : _token(other._token) {}

Ping::~Ping(void) {}

/************************************************************
 *		➕ OPERATORS											*
 ************************************************************/

Ping& Ping::operator=(const Ping& other)
{
    if (this != &other) {
    }
    return (*this);
}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

ReplyCode Ping::check_args(Server& server, Client& client, std::string& params)
{
    (void)server;
    (void)client;
    std::string token;

    std::istringstream iss(params);
    iss >> token;

    LOG_DV_CMD(token);
    if (token.empty())
        return ERR_NOORIGIN;
    return CORRECT_FORMAT;
}

void Ping::execute(Server& server, Client& client)
{
    ReplyHandler& rh = ReplyHandler::get_instance(&server);
    rh.process_response(client, MSG_PING, _token);
}
