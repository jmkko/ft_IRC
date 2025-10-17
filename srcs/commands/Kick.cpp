#include "Kick.hpp"

#include "Channel.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
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

Kick::Kick(const std::string& params)
{
    std::istringstream iss(params);
    std::string        channels, users, msg;

    iss >> channels;
    iss >> users;
    std::getline(iss, msg);

    std::istringstream issChannels(channels), issUsers(users);
    std::string        currentChannel, currentUser;
    while (std::getline(issChannels, currentChannel, ',')) {
        LOG_DV_CMD(currentChannel);
        if (!currentChannel.empty())
            _channelsNames.push_back(currentChannel);
        currentChannel.clear();
    }
     while (std::getline(issUsers, currentUser, ',')) {
        LOG_DV_CMD(currentUser);
        if (!currentUser.empty())
            _usersNames.push_back(currentUser);
        currentUser.clear();
    }
    msg.erase(0, msg.find_first_not_of(WHITE_SPACE));
    if (!msg.empty() && msg[0] == ':')
        msg = msg.erase(0, 1);
    else if (!msg.empty())
        msg = msg.substr(0, msg.find_first_of(WHITE_SPACE));
    _msg = msg;
}

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

void Kick::execute(Server& server, Client& client)
{
    ReplyHandler& rh = ReplyHandler::get_instance(&server);
    t_params      channelNames;
    t_params      nicknames;

    for (size_t i = 0; i < _channelsNames.size(); ++i) {
        Channel* channel = server.find_channel_by_name(_channelsNames[i]);
        LOG_D_CMD("looking in", _channelsNames[i]);

        if (!channel) {
            rh.process_response(client, ERR_NOSUCHCHANNEL, _channelsNames[i]);
            continue;
        } else if (!channel->is_operator(client)) {
            rh.process_response(client, ERR_CHANOPRIVSNEEDED, channel->get_name());
            continue;
        }
        for (size_t j = 0; j < _usersNames.size(); ++j)
        {
            Client*  target  = server.find_client_by_nickname(_usersNames[j]);
            LOG_D_CMD("looking for", _usersNames[j]);

            if (!target) {
                rh.process_response(client, ERR_NOSUCHNICK, channel->get_name());
            } else if (channel->remove_member(*target)) {
                rh.process_response(*target, TRANSFER_KICK, channel->get_name() + " " + target->get_nickname(), &client, _msg);
                channel->broadcast(server, TRANSFER_KICK, channel->get_name() + " " + target->get_nickname(), &client, _msg);
                rh.process_response(client, TRANSFER_KICK, channel->get_name() + " " + target->get_nickname(), &client, _msg);
            } else {
                rh.process_response(client, ERR_USERNOTINCHANNEL, channel->get_name());
            }
        }
    }
}

ReplyCode Kick::check_args(Server& server, Client& client, std::string& params)
{
    ReplyHandler       rh = ReplyHandler::get_instance(&server);
    std::istringstream iss(params);
    std::string        channels, users;
    std::string        channelsList, usersList, msg;

    iss >> channels;
    iss >> users;
    if (channels.empty() || users.empty()) {
        return (ERR_NEEDMOREPARAMS);
    }
    std::string        currentChannel, currentUser, comma;
    std::istringstream issChannels(channels), issUsers(users);
    while (std::getline(issChannels, currentChannel, ','))
    {
        if (!Channel::is_valid_channel_name(currentChannel)) {
            rh.process_response(client, ERR_BADCHANMASK, currentChannel);
        }
        else {
            channelsList.empty() ? channelsList += currentChannel : channelsList += "," + currentChannel;
        }
        currentChannel.clear();
    }
    comma = "";
    while (std::getline(issUsers, currentUser, ','))
    {
        if (currentUser.empty())
            return (ERR_NEEDMOREPARAMS);
        else {
            usersList.empty() ? usersList += currentUser : usersList += "," + currentUser;
        }
        currentUser.clear();
    }

    std::getline(iss, msg);
    // msg.erase(0, msg.find_first_not_of(WHITE_SPACE));
    // if (!msg.empty() && msg[0] == ':')
    //     msg.erase(0, 1);
    // else if (!msg.empty())
    //     msg = msg.substr(0, msg.find_first_of(WHITE_SPACE));
    params = channelsList + " " + usersList + " " + msg;
    LOG_DV_CMD(params);

    return (CORRECT_FORMAT);
}
