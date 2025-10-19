#include "Kick.hpp"

#include "Channel.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "Parser.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <cstddef>
#include <sstream>
#include <string>

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

Kick::Kick(std::string& params) : ICommand()
{
    Parser      parser;
    std::string channels, users;

    channels = parser.format_parameter(params, NULL);
    users    = parser.format_parameter(params, NULL);
    _msg     = parser.format_parameter(params, NULL);

    _channelsNames = parser.to_vector(channels);
    _usersNames    = parser.to_vector(users);
}

Kick::Kick(void) : _channelsNames(), _usersNames(), _msg() {}
Kick::Kick(const Kick& other) : ICommand(), _channelsNames(other._channelsNames), _usersNames(other._usersNames), _msg("") {}

Kick::~Kick() {}

/************************************************************
 *		➕ OPERATORS											*
 ************************************************************/

Kick& Kick::operator=(const Kick& other)
{
    if (this != &other) {
        _channelsNames = other._channelsNames;
        _usersNames    = other._usersNames;
        _msg           = other._msg;
    }
    return *this;
}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

void Kick::_kick_all_users_from(std::string& chanName, std::vector<std::string> usersNames, Parser& p)
{
    Server*  server  = p.get_server();
    Channel* channel = server->find_channel_by_name(chanName);
    if (!channel) {
        p.response(ERR_NOSUCHCHANNEL, chanName);
        return;
    }
    for (size_t i = 0; i < usersNames.size(); i++) {
        Client* target = p.get_server()->find_client_by_nickname(usersNames[i]);
        if (!channel->is_operator(*p.get_client())) {
            p.response(ERR_CHANOPRIVSNEEDED, channel->get_name());
        } else if (target && channel->remove_member(*target)) {
            p.response(target, p.get_client(), TRANSFER_KICK, channel->get_name() + " " + target->get_nickname(), _msg);
            channel->broadcast(*server, TRANSFER_KICK, channel->get_name() + " " + target->get_nickname(), p.get_client(), _msg);
            p.response(p.get_client(), TRANSFER_KICK, channel->get_name() + " " + target->get_nickname(), _msg);
        } else {
            p.response(ERR_USERNOTINCHANNEL, channel->get_name());
        }
    }
}

void Kick::execute(Server& server, Client& client)
{
    LOG_CMD.debug("Kick.cpp execute()");
    Parser p(server, client);
    size_t chanNb = _channelsNames.size();
    size_t userNb = _usersNames.size();

    if ((chanNb != 1 && _channelsNames.size() != _usersNames.size()) || chanNb == 0 || userNb == 0) {
        p.response(ERR_NEEDMOREPARAMS, "KICK");
        return;
    } else if (chanNb == 1) {
        if (p.correct_channel(_channelsNames[0]))
            _kick_all_users_from(_channelsNames[0], _usersNames, p);
        return;
    }
    for (size_t i = 0; i < chanNb; ++i) {
        if (!p.correct_channel(_channelsNames[i]))
            continue;
        Channel* channel = server.find_channel_by_name(_channelsNames[i]);
        Client*  target  = server.find_client_by_nickname(_usersNames[i]);
        LOG_D_CMD("looking in", _channelsNames[i]);
        if (!channel) {
            p.response(ERR_NOSUCHCHANNEL, _channelsNames[i]);
        } else if (!channel->is_operator(client)) {
            p.response(ERR_CHANOPRIVSNEEDED, channel->get_name());
        } else if (target && channel->remove_member(*target)) {
            p.response(target, &client, TRANSFER_KICK, channel->get_name() + " " + target->get_nickname(), _msg);
            channel->broadcast(server, TRANSFER_KICK, channel->get_name() + " " + target->get_nickname(), &client, _msg);
            p.response(TRANSFER_KICK, channel->get_name() + " " + target->get_nickname(), _msg);
        } else {
            p.response(ERR_USERNOTINCHANNEL, channel->get_name());
        }
    }
}
