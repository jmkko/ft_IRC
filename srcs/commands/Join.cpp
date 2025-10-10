#include "Join.hpp"

#include "Channel.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <bitset>

Join::Join() {}
Join::~Join() {}
Join::Join(const Join& other) : ICommand(), _channelsLst(other._channelsLst) {}
Join& Join::operator=(const Join& other)
{
    if (this != &other) {
        _channelsLst = other._channelsLst;
    }
    return *this;
}

Join::Join(const std::vector<std::string>& channelsLst) : _channelsLst(channelsLst) {}

/**
 * @brief check if there is no params for command Join, and parse the params
 * and format this params to a vector of string <channel><space><key>
 *
 * @param server
 * @param client
 * @param params
 * @return vector of string formarted
 */
ReplyCode Join::check_args(Server& server, Client& client, std::vector<std::string>& params)
{
    (void)server;
    (void)client;
    std::istringstream       iss(params[0]);
    std::vector<std::string> channels;
    std::vector<std::string> keys;
    std::vector<std::string> channelsLst;
    std::string              tokenChannels;
    std::string              tokenKeys;
    std::string              value;

    iss >> tokenChannels;
    iss >> tokenKeys;
    if (tokenChannels.empty()) {
        LOG_CMD.warning(TO_STRING(ERR_NEEDMOREPARAMS) + " ERR_NEEDMOREPARAMS");
        return (ERR_NEEDMOREPARAMS);
    }
    std::istringstream issChannels(tokenChannels);
    while (std::getline(issChannels, value, ',')) {
        channels.push_back(value);
    }
    std::istringstream issKeys(tokenKeys);
    while (std::getline(issKeys, value, ',')) {
        keys.push_back(value);
    }
    std::vector<std::string>::iterator it   = channels.begin();
    size_t                             rank = 0;
    for (; it < channels.end(); it++) {
        // std::cout << "channels : " << *it;
        if (rank < keys.size())
            channelsLst.push_back(*it + " " + keys[rank]);
        else
            channelsLst.push_back(*it);
        rank++;
    }
    params = channelsLst;
    return (CORRECT_FORMAT);
}

/**
 * @brief Allows a client to join a channel or create it if it does not exist
 * after checking if the name of the channel is valid accordind to the RFC_2812
 *
 * @param server
 * @param client
 *
 * sending response sequence to client
 *	:user1!~username@host JOIN :#chan1 123
 *	:irc.example.com MODE #chan1 +o user1
 *	:irc.example.com MODE #chan1 +k 123
 *	:irc.example.com 331 user1 #chan1 :No topic is set
 *	:irc.example.com 353 user1 = #chan1 :@user1 user2 user3 user4 user5
 *	:irc.example.com 353 user1 = #chan1 :user6 @user1 user7 user8 user9
 *	:irc.example.com 366 user1 #chan1 :End of NAMES listV
 */
void Join::execute(Server& server, Client& client)
{
    ReplyHandler&                      rh = ReplyHandler::get_instance(&server);
    std::vector<std::string>::iterator it = _channelsLst.begin();
    std::string                        chanName;
    std::string                        chanKey;
    ReplyCode                          replyCode = CORRECT_FORMAT;

    while (it != _channelsLst.end()) {
        std::istringstream iss(*it);
        iss >> chanName;
        iss >> chanKey;
        if (!Channel::is_valid_channel_name(chanName)) {
            rh.process_response(client, ERR_BADCHANMASK, chanName);
            it++;
            continue;
        }
        Channel*                                  channel         = NULL;
        std::map<std::string, Channel*>::iterator existingChannel = server.channels.find(chanName);

        if (existingChannel == server.channels.end()) {
            channel                              = new Channel(chanName); // NOLINT
            server.channels[channel->get_name()] = channel;
            LOG_I_CMD("#️⃣ New channel", channel->get_name());
        } else {
            channel = existingChannel->second;
        }
        LOG_DV_CMD(std::bitset<8>(channel->get_mode()));
        if ((channel->get_mode() & CHANMODE_KEY) && (chanKey != channel->get_key()))
        {
            rh.process_response(client, ERR_BADCHANNELKEY, channel->get_name());
            ++it;
            continue;
        }
        replyCode = channel->add_member(client);
        if (replyCode == CORRECT_FORMAT) {
            rh.process_response(client, TRANSFER_JOIN, channel->get_name());
            channel->broadcast(server, TRANSFER_JOIN, channel->get_name(), &client);
            LOG_CONN.info(client.get_nickname() + " joined channel: " + channel->get_name());
        } else {
            rh.process_response(client, replyCode, channel->get_name());
            ++it;
            continue;
        }
        if (channel->get_nb_members() == 1) {
            channel->make_operator(client);
            rh.process_response(client, RPL_CHANNELMODEIS, channel->get_name() + " +o ");
            LOG_CMD.info(client.get_nickname() + " is operator of channel: " + channel->get_name());
        }
        if (channel->get_topic() == "No topic is set") {
            rh.process_response(client, RPL_NOTOPIC, channel->get_name());
        } else {
            rh.process_response(client, RPL_TOPIC, channel->get_name() + " :" + channel->get_topic());
        }
        std::vector<std::string> users = channel->get_members_list();
        for (size_t i = 0; i < users.size(); ++i) {
            rh.process_response(client, RPL_NAMREPLY, channel->get_name() + " :" + users[i]);
        }
        rh.process_response(client, RPL_ENDOFNAMES, channel->get_name());
        ++it;
    }
}
