#include "Pass.hpp"

#include "Client.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"

#include <iostream>

/************************************************************
 *		📁 CLASS METHODS									*
 ************************************************************/

ReplyCode Pass::check_args(Server& server, Client& client, std::string& params)
{
    std::string        pass = "";
    std::istringstream iss(params);
    ReplyHandler       rh = ReplyHandler::get_instance(&server);

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

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

Pass::Pass(void) {}

Pass::~Pass(void) {}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

void Pass::execute(Server& server, Client& client)
{
    (void)server;
    (void)client;

    client.set_status(AUTHENTICATED);
    LOG_I_CMD("AUTHENTICATED", client);
}
