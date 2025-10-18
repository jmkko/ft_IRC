#include "Pass.hpp"

#include "Client.hpp"
#include "LogManager.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"

#include <iostream>

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

Pass::Pass(void) {}
Pass::Pass(std::string& params)
{
	Parser parser;

	_pass = parser.format_parameter(params, NULL);
}
Pass::~Pass(void) {}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

void Pass::execute(Server& server, Client& client)
{
	Parser p(server, client);
    if (_pass.empty()) {
        p.response(ERR_NEEDMOREPARAMS, "PASS");
    } else if (!p.correct_password(_pass)) {
		return;
	}
	client.set_status(AUTHENTICATED);
	LOG_I_CMD("AUTHENTICATED", client);
}
