#include "Pass.hpp"
#include "Client.hpp"
#include "LogManager.hpp"
#include "Server.hpp"

// Default constructor
Pass::Pass(void) {}
Pass::Pass(const std::string& pass) : _pass(pass) {}

// Copy constructor
Pass::Pass(const Pass& other) { (void)other; }

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
    client.setStatus(REGISTERED);
	ReplyHandler& rh = ReplyHandler::getInstance(&server);
    if (!client.getUserName().empty() && !client.getNickName().empty()) {
        LOG_CMD.info("001 RPL_WELCOME");
		rh.processResponse(client, RPL_WELCOME);
    } else {
        LOG_CMD.info("201 RPL_PASS");
		rh.processResponse(client, RPL_PASS);
    }
}

ReplyCode Pass::checkArgs(Server& server, Client& client, std::string& params)
{
    std::string        pass;
    std::istringstream iss(params);

    iss >> pass;
	if (pass.empty()) {
        LOG_CMD.error("461 ERR_NEEDMOREPARAMS");
		return (ERR_NEEDMOREPARAMS);
	}
    if (server.getPassW() != pass) {
        LOG_CMD.error("464 ERR_PASSWDMISMATCH");
        return (ERR_PASSWDMISMATCH);
    } else if (client.getStatus() == REGISTERED) {
        LOG_CMD.error("462 ERR_ALREADYREGISTERED");
        return (ERR_ALREADYREGISTERED);
    }
    params = pass;
    return (RPL_SUCCESS);
}
