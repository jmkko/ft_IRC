#include "Join.hpp"

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
        if (Channel::isValidChannelName(value))
            channelsLst.push_back(ChannelParam(*it, false));
        else if (rank < keys.size())
            channelsLst.push_back(ChannelParam(*it, keys[rank]));
        else
            channelsLst.push_back(ChannelParam(*it));
        rank++;
    }
    return channelsLst;
}

void Join::execute(Server& server, Client& client)
{

    ReplyHandler&                       rh = ReplyHandler::getInstance(&server);
    std::vector<ChannelParam>::iterator it = _channelsLst.begin();
    for (; it < _channelsLst.end(); it++) {
        if (!it->isValid) {
            LOG_CMD.error(TO_STRING(ERR_BADCHANMASK) + " ERR_BADCHANMASK");
            rh.processResponse(client, ERR_BADCHANMASK, ERR_BADCHANMASK_MSG);
        }
        Channel newChan(it->channel);
        server._channels[newChan.getName()]=newChan;
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
