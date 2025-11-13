#include "Invite.hpp"

#include "Parser.hpp"
#include "reply_codes.hpp"

/******************************************************************************
 *                  CONSTRUCTOR AND DESTRUCTOR                                *
 ******************************************************************************/

Invite::Invite(std::string& params)
{

    Parser parser;

    _nickname    = parser.format_parameter(params, NULL);
    _channelName = parser.format_parameter(params, NULL);
}

Invite::~Invite() {}

/******************************************************************************
 *                                 METHODS                                    *
 ******************************************************************************/

void Invite::execute(Server& server, Client& client)
{
    Parser p(server, client);

    if (_channelName.empty() || _nickname.empty()) {
        p.response(ERR_NEEDMOREPARAMS, "INVITE");
        return;
    }
    Client*  target  = server.find_client_by_nickname(_nickname);
    Channel* channel = server.find_channel_by_name(_channelName);
    if (!target) {
        p.response(ERR_NOSUCHNICK, _nickname);
    } else if (!channel) {
        p.response(ERR_NOSUCHCHANNEL, _channelName);
    } else if (!channel->is_member(client)) {
        p.response(ERR_NOTONCHANNEL);
    } else if (channel->is_member(*target)) {
        p.response(ERR_USERONCHANNEL, _nickname + " " + _channelName);
    } else if (channel->is_invite_only() && !channel->is_operator(client)) {
        p.response(ERR_CHANOPRIVSNEEDED, _channelName);
    } else {
        channel->invite_client(*target);
        p.response(RPL_INVITING, _nickname + " " + _channelName);
        p.response(target, &client, TRANSFER_INVITE, _nickname + " " + _channelName);
    }
}
