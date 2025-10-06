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
    return (RPL_SUCCESS);
}

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
        rh.process_response(c, ERR_NOSUCHNICK, nick + ERR_NOSUCHNICK_MSG);
        return;
    }
    if (!channel) {
        rh.process_response(c, ERR_NOSUCHCHANNEL, chan + ERR_NOSUCHCHANNEL_MSG);
        return;
    }
    if (!channel->is_member(c)) {
        rh.process_response(c, ERR_NOTONCHANNEL, chan + ERR_NOTONCHANNEL_MSG);
        return;
    }
    if (channel->is_member(*target)) {
        rh.process_response(c, ERR_USERONCHANNEL, nick + " " + chan + ERR_USERONCHANNEL_MSG);
        return;
    }
    if (channel->is_invite_only() && !channel->is_operator(c)) {
        rh.process_response(c, ERR_CHANOPRIVSNEEDED, chan + ERR_CHANOPRIVSNEEDED_MSG);
        return;
    }
    channel->invite_client(*target);
    rh.process_response(c, RPL_INVITING, nick + " " + chan);
    rh.process_response(c, RPL_INVITING_TARGET, " INVITE " + nick + " :" + chan);
}
