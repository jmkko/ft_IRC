#include "Pass.hpp"

#include "Client.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"

#include <iostream>

// Default constructor
Pass::Pass(void) {}
Pass::Pass(const std::string& pass) : _pass(pass) {}

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
	(void)server;
	client.set_status(REGISTERED);
	ReplyHandler& rh = ReplyHandler::get_instance(&server);
	if (!client.get_user_name().empty() && !client.get_nickname().empty()) {
		LOG_CMD.info("001 RPL_WELCOME");
		rh.process_response(client, RPL_WELCOME);
	} else {
		LOG_CMD.info("201 RPL_PASS");
		rh.process_response(client, RPL_PASS);
	}
}

ReplyCode Pass::check_args(Server& server, Client& client, std::string& params)
{
	std::string		   pass;
	std::istringstream iss(params);

	iss >> pass;
	if (pass.empty()) {
		LOG_CMD.error("461 ERR_NEEDMOREPARAMS");
		return (ERR_NEEDMOREPARAMS);
	}
	if (server.get_password() != pass) {
		LOG_CMD.error("464 ERR_PASSWDMISMATCH");
		return (ERR_PASSWDMISMATCH);
	} else if (client.get_status() == REGISTERED) {
		LOG_CMD.error("462 ERR_ALREADYREGISTERED");
		return (ERR_ALREADYREGISTERED);
	}
	params = pass;
	return (RPL_SUCCESS);
}
