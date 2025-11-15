#include "Quit.hpp"

#include "Channel.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "Part.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"

/************************************************************
 *             🥚 CONSTRUCTORS & DESTRUCTOR                 *
 ************************************************************/

Quit::Quit(void) {}

Quit::Quit(const std::string& params) : _quitMsg(params) {}

Quit::~Quit(void) {}

/*************************************************************
 *                      🛠️ FUNCTIONS                         *
 *************************************************************/

void Quit::execute(Server& server, Client& client)
{
    int pfdIndex = server.index_of(client);
    LOG_DTV_CMD(pfdIndex);
    std::string trailingMsg = _quitMsg;
    if (trailingMsg.empty())
        trailingMsg = ircConfig.trailing(TRANSFER_QUIT);
    client.broadcast_to_all_channels(server, TRANSFER_QUIT, "", trailingMsg);
    server.add_events_of(client, 0);
    server.cleanup_socket_and_client(pfdIndex);
}
