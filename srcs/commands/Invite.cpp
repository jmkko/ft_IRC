#include "Invite.hpp"

#include "reply_codes.hpp"
#include "Parser.hpp"
#include <sstream>

/******************************************************************************
 *                  CONSTRUCTOR AND DESTRUCTOR                                *
 ******************************************************************************/

Invite::Invite() : _nickname(), _channelName() {}
Invite::Invite(const Invite& other) : _nickname(other._nickname), _channelName(other._channelName) {}
Invite::~Invite() {}
Invite& Invite::operator=(const Invite& other)
{
    if (this != &other) {
        _nickname    = other._nickname;
        _channelName = other._channelName;
    }
    return (*this);
}

Invite::Invite(std::string& params) {

	Parser parser;

	_nickname = parser.format_parameter(params, NULL);
	_channelName = parser.format_parameter(params, NULL);
}

/******************************************************************************
 *                                 METHODS                                    *
 ******************************************************************************/

/**
 * @brief invite a client to a channel
 * check if:
 *   the client exist, the channel exist
 *   the sender is on channel
 *   the client is already on this channel
 *   the sender is operator if the channel is invite-only
 *
 * @param server Server
 * @param client Client = the sender
 */
void Invite::execute(Server& server, Client& client)
{
	Parser 				p(server, client);

    if (_channelName.empty() || _nickname.empty()) {
        p.response(ERR_NEEDMOREPARAMS, "JOIN");
		return ;
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
		p.response(target, &client, TRANSFER_INVITE, _nickname, _channelName);
	}
}
