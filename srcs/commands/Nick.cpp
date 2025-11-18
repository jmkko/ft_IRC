#include "Nick.hpp"

#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "Nick.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cctype>

/************************************************************
 *            🥚 CONSTRUCTORS & DESTRUCTOR                  *
 ************************************************************/

Nick::Nick(std::string& params)
{
    Parser parser;
    _nickname = parser.format_parameter(params, NULL);
}

Nick::~Nick(void) {}

/*************************************************************
 *                      🛠️ METHODS                           *
 *************************************************************/
void Nick::execute(Server& server, Client& client)
{
    (void)server;
    std::string oldNickname = client.get_nickname();
    Parser      p(server, client);

    LOG_DV_CMD(_nickname);

    if (!p.correct_nickname(_nickname))
        return;
    if (oldNickname.empty() && !client.get_user_name().empty()) {
        client.set_nickname(_nickname);
        LOG_dt_CMD("Nick after USER");
        client.set_status(REGISTERED);
        p.rh->process_welcome(server, client);
    } else if (!oldNickname.empty() && !client.get_user_name().empty() && client.is_registered()) {
        client.broadcast_to_all_channels(server, TRANSFER_NICK, "",
                                         _nickname); // ! \\ ;
        p.response(TRANSFER_NICK, "", _nickname);
    }
    client.set_nickname(_nickname);
}
