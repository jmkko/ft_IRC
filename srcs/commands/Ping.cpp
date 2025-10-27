#include "Ping.hpp"

#include "LogManager.hpp"
#include "Parser.hpp"
#include "ReplyHandler.hpp"
#include "reply_codes.hpp"

#include <sstream>

/************************************************************
 *             🥚 CONSTRUCTORS & DESTRUCTOR                 *
 ************************************************************/

Ping::Ping(std::string& params)
{
    Parser parser;

    _token = parser.format_parameter(params, NULL);
}

Ping::~Ping(void) {}

/*************************************************************
 *                    🛠️ FUNCTIONS                           *
 *************************************************************/

void Ping::execute(Server& server, Client& client)
{
    Parser p(server, client);

    if (_token.empty()) {
        p.response(ERR_NOORIGIN);
        return;
    }
    p.response(MSG_PING, _token);
}
