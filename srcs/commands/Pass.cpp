#include "Pass.hpp"

#include "Client.hpp"
#include "LogManager.hpp"
#include "Parser.hpp"
#include "Server.hpp"

#include <iostream>

// Default constructor
Pass::Pass(void) {}
Pass::Pass(std::string& params)
{
	Parser parser;

	_pass = parser.format_parameter(params, NULL);
}

// Copy constructor
Pass::Pass(const Pass& other) : ICommand() { (void)other; }

// Assignment operator overload
Pass& Pass::operator=(const Pass& other)
{
    (void)other;
    return (*this);
}

// Destructor
Pass::~Pass(void) {}

void Pass::execute(Server& server, Client& client)
{
	Parser p(server, client);
	if (!p.correct_password(_pass)) {
		return ;
	}
    client.set_status(AUTHENTICATED);
    LOG_CMD.info("PASS - correct");
}

ReplyCode Pass::check_args(Server& server, Client& client, std::string& params)
{
    std::string        pass;
    std::istringstream iss(params);

    iss >> pass;
    if (pass.empty()) {
        LOG_CMD.warning("461 ERR_NEEDMOREPARAMS");
        return (ERR_NEEDMOREPARAMS);
    }
    if (server.get_password() != pass) {
        LOG_CMD.warning("464 ERR_PASSWDMISMATCH");
        return (ERR_PASSWDMISMATCH);
    } else if (client.get_status() == REGISTERED) {
        LOG_CMD.warning("462 ERR_ALREADYREGISTRED");
        return (ERR_ALREADYREGISTRED);
    }
    params = pass;
    return (CORRECT_FORMAT);
}
