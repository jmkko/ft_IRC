#include "Join.hpp"

#include "Channel.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

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
        LOG_CMD.error(TO_STRING(ERR_NEEDMOREPARAMS) + " ERR_NEEDMOREPARAMS");
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
    std::vector<std::string>::iterator it = channels.begin();
    size_t                             rank = 0;
    for (; it < channels.end(); it++) {
        std::cout << "channels : " << *it;
        if (rank < keys.size())
            channelsLst.push_back(*it + " " + keys[rank]);
        else
            channelsLst.push_back(*it);
        rank++;
    }
    params = channelsLst;
    return (RPL_SUCCESS);
}

/**
 * @brief Allows a client to join a channel or create it if it does not exist
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
    ReplyCode                          replyCode = RPL_SUCCESS;

    while (it != _channelsLst.end()) {
        std::istringstream iss(*it);
        iss >> chanName;
        iss >> chanKey;
        if (!Channel::is_valid_channel_name(chanName)) {
            LOG_CMD.error(TO_STRING(ERR_BADCHANMASK) + " ERR_BADCHANMASK");
            rh.process_response(client, ERR_BADCHANMASK, chanName);
            continue;
        }
        Channel*                                  channel = NULL;
        std::map<std::string, Channel*>::iterator existingChannel = server.channels.find(chanName);

        if (existingChannel == server.channels.end()) {
            channel = new Channel(chanName);
            server.channels[channel->get_name()] = channel;
            LOG_CMD.info("Created new channel: " + channel->get_name());
        } else {
            channel = existingChannel->second;
        }
        replyCode = channel->add_member(client);
        if (replyCode == RPL_SUCCESS)
            rh.process_response(client, RPL_JOIN, channel->get_name());
        else {
            rh.process_response(client, replyCode, channel->get_name());
            continue;
        }
        if (channel->get_nb_members() == 1) {
            channel->make_operator(client);
            rh.process_response(client, RPL_MODE, channel->get_name() + " +o ");
        }
        if (channel->get_topic() == "No topic is set") {
            rh.process_response(client, RPL_NOTOPIC, channel->get_name());
        } else {
            rh.process_response(client, RPL_TOPIC, channel->get_name() + " :" + channel->get_topic());
        }
        rh.process_response(client, RPL_NAMREPLY, channel->get_name());
        rh.process_response(client, RPL_ENDOFNAMES, channel->get_name());
        ++it;
    }
}

// if no params
//     return no params
// if chanel badmaks ERR_BADCHANMASK avoir # ou & max 50 char , char forbidden-> ' '   ,   :   \a
// if channel exist
// if chanel is protected ERR_BADCHANNELKEY
// if chanel is full ERR_CHANNELISFULL
// if chanel invite only ERR_INVITEONLYCHAN
// if banned from channel ERR_BANNEDFROMCHAN
//
// 477 ERR_NOCHANMODES +channelname = channel mode less on peut mettre de mode
// #channel global -> multiserver
// &channel local -> un seul serveur
// +channel ->channel mode-less
// !channel -> reçoit un id aleatoire de 5 char = !acfdechannela
//
//
//
// !!!!! banned list check
// and list user channels
