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

ReplyCode Invite::check_args(Server& server, Client& client, std::string& params)
{
    (void)server;
    (void)client;
    std::istringstream iss(params);
    std::string        nick;
    std::string        chan;

    iss >> nick;
    iss >> chan;
    if (params.empty() || chan.empty())
        return (ERR_NEEDMOREPARAMS);
    return (CORRECT_FORMAT);
}

void Invite::execute(Server& server, Client& client)
{
    ReplyHandler&      rh = ReplyHandler::get_instance(&server);
    std::istringstream iss(_params);
    std::string        nick;
    std::string        chan;

    iss >> nick;
    iss >> chan;
    Client*  target  = server.find_client_by_nickname(nick);
    Channel* channel = server.channels[chan];
    if (!target) {
        rh.process_response(client, ERR_NOSUCHNICK, nick);
        return;
    }
    if (!channel) {
        rh.process_response(client, ERR_NOSUCHCHANNEL, chan);
        return;
    }
    if (!channel->is_member(client)) {
        rh.process_response(client, ERR_NOTONCHANNEL);
        return;
    }
    if (channel->is_member(*target)) {
        rh.process_response(client, ERR_USERONCHANNEL, nick + " " + chan);
        return;
    }
    if (channel->is_invite_only() && !channel->is_operator(client)) {
        rh.process_response(client, ERR_CHANOPRIVSNEEDED, chan);
        return;
    }
    channel->invite_client(*target);
    rh.process_response(client, RPL_INVITING, nick + " " + chan);
    rh.process_response(*target, TRANSFER_INVITE, " INVITE " + nick + " :" + chan, &client);
}
