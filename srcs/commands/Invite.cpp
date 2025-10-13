#include "Invite.hpp"

#include "reply_codes.hpp"

#include <sstream>

/******************************************************************************
 *                  CONSTRUCTOR AND DESTRUCTOR                                *
 ******************************************************************************/

Invite::Invite() : _params() {}
Invite::Invite(const Invite& other) : _params(other._params) {}
Invite::~Invite() {}
Invite& Invite::operator=(const Invite& other)
{
    if (this != &other) {
        _params = other._params;
    }
    return (*this);
}
Invite::Invite(const std::string& params) : _params(params) {}

/******************************************************************************
 *                                 METHODS                                    *
 ******************************************************************************/

/**
 * @brief check if Invite has a client and a chan
 *
 * @param s
 * @param c
 * @param params of the commmand
 * @return replyCode
 */
ReplyCode Invite::check_args(Server& s, Client& c, std::string& params)
{
    (void)s;
    (void)c;
    std::istringstream iss(params);
    std::string        nick;
    std::string        chan;

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
 * @param s Server
 * @param c Client = the sender
 */
void Invite::execute(Server& s, Client& c)
{
    ReplyHandler&      rh = ReplyHandler::get_instance(&s);
    std::istringstream iss(_params);
    std::string        nick;
    std::string        chan;

    iss >> nick;
    iss >> chan;
    Client*  target  = s.find_client_by_nickname(nick);
    Channel* channel = s.channels[chan];
    if (!target) {
        rh.process_response(c, ERR_NOSUCHNICK, nick);
        return;
    }
    if (!channel) {
        rh.process_response(c, ERR_NOSUCHCHANNEL, chan);
        return;
    }
    if (!channel->is_member(c)) {
        rh.process_response(c, ERR_NOTONCHANNEL);
        return;
    }
    if (channel->is_member(*target)) {
        rh.process_response(c, ERR_USERONCHANNEL, nick + " " + chan);
        return;
    }
    if (channel->is_invite_only() && !channel->is_operator(c)) {
        rh.process_response(c, ERR_CHANOPRIVSNEEDED, chan);
        return;
    }
    channel->invite_client(*target);
    rh.process_response(c, RPL_INVITING, nick + " " + chan);
    rh.process_response(*target, TRANSFER_INVITE, " INVITE " + nick + " :" + chan, &c);
}
