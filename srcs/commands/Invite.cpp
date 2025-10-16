#include "Invite.hpp"

#include "reply_codes.hpp"

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
        _nickname = other._nickname;
		_channelName = other._channelName;
    }
    return (*this);
}
Invite::Invite(const std::string& params) {
    std::istringstream iss(params);
    std::string        nick;
    std::string        chan;

    iss >> nick;
    iss >> chan;
	_nickname = nick;
	_channelName = chan;
}

/******************************************************************************
 *                                 METHODS                                    *
 ******************************************************************************/

/**
 * @brief check if Invite has a client and a chan
 *
 * @param s
 * @param client
 * @param params of the commmand
 * @return replyCode
 */
ReplyCode Invite::check_args(Server& server, Client& client, std::string& params)
{
    (void)server;
    (void)client;
    std::istringstream iss(params);
    std::string        nick ,chan;

    iss >> nick;
    iss >> chan;
    if (params.empty() || chan.empty())
        return (ERR_NEEDMOREPARAMS);
    return (CORRECT_FORMAT);
}

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
    ReplyHandler&      rh = ReplyHandler::get_instance(&server);

    Client*  target  = server.find_client_by_nickname(_nickname);
    Channel* channel = server.find_channel_by_name(_channelName);
    if (!target) {
        rh.process_response(client, ERR_NOSUCHNICK, _nickname);
    } else if (!channel) {
        rh.process_response(client, ERR_NOSUCHCHANNEL, _channelName);
    } else if (!channel->is_member(client)) {
        rh.process_response(client, ERR_NOTONCHANNEL);
    } else if (channel->is_member(*target)) {
        rh.process_response(client, ERR_USERONCHANNEL, _nickname + " " + _channelName);
    } else if (channel->is_invite_only() && !channel->is_operator(client)) {
        rh.process_response(client, ERR_CHANOPRIVSNEEDED, _channelName);
    } else {
		channel->invite_client(*target);
		rh.process_response(client, RPL_INVITING, _nickname + " " + _channelName);
		rh.process_response(*target, TRANSFER_INVITE, _nickname, &client, _channelName);
	}
}
