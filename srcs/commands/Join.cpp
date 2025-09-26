#include "Join.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"

Join::Join() {}
Join::~Join() {}
Join::Join(const Join& mother) : _channelsLst(mother._channelsLst) {}
Join& Join::operator=(const Join& other)
{
    if (this != &other) {
        _channelsLst = other._channelsLst;
    }
    return *this;
}

Join::Join(std::vector<ChannelParam> channelsLst) : _channelsLst(channelsLst) {}

std::vector<ChannelParam> Join::checkArgs(Server& server, Client& client, std::string& params)
{
    (void)server;
    (void)client;
    std::istringstream        iss(params);
    std::vector<std::string>  channels;
    std::vector<std::string>  keys;
    std::vector<ChannelParam> channelsLst;
    std::string               tokenChannels;
    std::string               tokenKeys;
    std::string               value;

    iss >> tokenChannels;
    iss >> tokenKeys;
    if (tokenChannels.empty()) {
        LOG_CMD.error(TO_STRING(ERR_NEEDMOREPARAMS) + " ERR_NEEDMOREPARAMS");
        return channelsLst;
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
        if (Channel::isValidChannelName(*it)) {
            if (rank < keys.size())
                channelsLst.push_back(ChannelParam(*it, keys[rank]));
            else
                channelsLst.push_back(ChannelParam(*it));
        } else {
            channelsLst.push_back(ChannelParam(*it, false)); // mark as invalid
        }
        rank++;
    }
    return channelsLst;
}

void Join::execute(Server& server, Client& client)
{
    ReplyHandler& rh = ReplyHandler::getInstance(&server);
    std::vector<ChannelParam>::iterator it = _channelsLst.begin();
    
    for (; it < _channelsLst.end(); it++) {
        if (!it->isValid) {
            LOG_CMD.error(TO_STRING(ERR_BADCHANMASK) + " ERR_BADCHANMASK");
            rh.processResponse(client, ERR_BADCHANMASK, it->channel);
            continue;
        }
        Channel* channel = NULL;
        std::map<std::string, Channel*>::iterator existingChannel = server.channels.find(it->channel);
        
        if (existingChannel == server.channels.end()) {
            try {
                channel = new Channel(it->channel);
                server.channels[channel->getName()] = channel;
                LOG_CMD.info("Created new channel: " + channel->getName());
            } catch (std::exception& e) {
                LOG_CMD.error("Failed to create channel: " + std::string(e.what()));
                rh.processResponse(client, ERR_BADCHANMASK, it->channel);
                continue;
            }
        } else {
            channel = existingChannel->second;
        }
        
        try {
            channel->addMember(client);
            rh.processResponse(client, RPL_JOIN, channel->getName());
            if (channel->getTopic() == "No topic is set") {
                rh.processResponse(client, RPL_NOTOPIC, channel->getName());
            } else {
                rh.processResponse(client, RPL_TOPIC, channel->getName() + " :" + channel->getTopic());
            }
            if (existingChannel == server.channels.end()) {
                try {
                    channel->makeOperator(client);
                } catch (std::exception& e) {
                    LOG_CMD.debug("Could not make client operator: " + std::string(e.what()));
                }
            }
            rh.processResponse(client, RPL_NAMREPLY, channel->getName());
            rh.processResponse(client, RPL_ENDOFNAMES, channel->getName());
            
        } catch (std::exception& e) {
            LOG_CMD.error("Failed to add client to channel: " + std::string(e.what()));
            if (std::string(e.what()).find("channel max capacity") != std::string::npos) {
                rh.processResponse(client, ERR_CHANNELISFULL, channel->getName());
            } else if (std::string(e.what()).find("not invited") != std::string::npos) {
                rh.processResponse(client, ERR_INVITEONLYCHAN, channel->getName());
            } else {
                rh.processResponse(client, ERR_BADCHANMASK, channel->getName());
            }
        }
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
// !channel -> reçoit un id aleatoire de 5 char = !acfdechannel
