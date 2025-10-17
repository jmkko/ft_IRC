#include "Pass.hpp"

#include "Client.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"

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
    (void)client;

    client.set_status(AUTHENTICATED);
    LOG_I_CMD("AUTHENTICATED", client);
}

ReplyCode Pass::check_args(Server& server, Client& client, std::string& params)
{
    std::string        pass = "";
    std::istringstream iss(params);
    ReplyHandler rh          = ReplyHandler::get_instance(&server);

    iss >> pass;
    if (client.is_registered()) {
        rh.process_response(client, ERR_ALREADYREGISTRED);
        return PROCESSED_ERROR;
    }
    if (iss.str().empty()) {
        client.set_status(UNAUTHENTICATED);
        rh.process_response(client, ERR_NEEDMOREPARAMS, "PASS");
        return PROCESSED_ERROR;
    }
    if (server.get_password() != pass) {
        client.set_status(UNAUTHENTICATED);
        rh.process_response(client, ERR_PASSWDMISMATCH);
        return PROCESSED_ERROR;
    }
    params = pass;
    return (CORRECT_FORMAT);
}
