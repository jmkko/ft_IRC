#include "Ping.hpp"

#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "reply_codes.hpp"
#include "Parser.hpp"
#include <sstream>

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

Ping::Ping(std::string& params) {
	Parser parser;

	_token = parser.format_parameter(params, NULL);
}

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
	Parser p(server, client);

    if (_token.empty()) {
    	p.response(ERR_NOORIGIN);
		return ;
	}
    p.response(MSG_PING, _token);
}
