#include "Quit.hpp"

#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"
#include "consts.hpp"

#include <iostream>

/************************************************************
*		🥚 CONSTRUCTORS & DESTRUCTOR						*
************************************************************/

Quit::Quit(void) {}

Quit::Quit(const std::string& params) : _quitMsg(params) {}

Quit::~Quit(void) {}

/*************************************************************
*		🛠️ FUNCTIONS											*
*************************************************************/

ReplyCode	Quit::check_args(Server& server, Client& client, std::string& params)
{
	(void) server;
	(void) client;
	(void) params;
	return RPL_SUCCESS;
}

/**
 * @brief no confirmation message is sent
 * 
 * @param server 
 * @param client 
 */
void	Quit::execute(Server& server, Client& client)
{
	int pfdIndex = server.index_of(client);
	LOG_CMD.debug("QUIT execute : pfdIndex : ", pfdIndex);
	std::string trailingMsg = _quitMsg;
	if (trailingMsg.empty())
		trailingMsg = QUIT_TRAILING_MSG;
	client.broadcast_to_all_channels(server, RPL_QUIT, trailingMsg);
	// usleep(SLEEP_FOR_BROADCAST_MS);
	server.add_events_of(client, 0);
	LOG_CMD.debug("quit execute - before cleaning sockets");
	server.cleanup_socket_and_client(pfdIndex);
}

/*************************************************************
*		👁️‍ GETTERS and SETTERS				 				*
*************************************************************/
