#include "Part.hpp"

#include "Channel.hpp"
#include "Client.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

Part::Part(std::string& params)
{
    Parser parser;

    std::string channelsNames = parser.format_parameter(params, NULL);

    _chanNames = parser.to_vector(channelsNames);
    _message   = parser.format_parameter(params, NULL);
}

Part::~Part(void) {}

/*************************************************************
 *		🛠️ METHODS											*
 *************************************************************/

void Part::execute(Server& server, Client& client)
{
    Parser p(server, client);

    if (_chanNames.size() == 0) {
        p.response(ERR_NEEDMOREPARAMS, "PART");
        return;
    }
    for (size_t i = 0; i < _chanNames.size(); i++) {
        Channel* channel = server.find_channel_by_name(_chanNames[i]);
        if (!channel) {
            p.response(ERR_NOSUCHCHANNEL, _chanNames[i]);
        } else if (!channel->is_member(client)) {
            p.response(ERR_NOTONCHANNEL);
        } else {
            channel->broadcast(server, TRANSFER_PART, _chanNames[i], &client, _message);
            p.response(TRANSFER_PART, _chanNames[i], _message);
            channel->remove_member(client);
        }
    }
}
