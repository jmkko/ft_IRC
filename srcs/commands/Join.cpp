#include "Join.hpp"
#include "reply_codes.hpp"

int Join::checkArgs(Server& server, Client& client, std::string& params)
{
    (void)server;
    std::istringstream       iss(params);
    std::vector<std::string> channels;
    std::vector<std::string> keys;
    std::string              value;

    iss >> value;
    if(value.empty()){
        LOG_CMD.error(TO_STRING(ERR_NEEDMOREPARAMS) + " ERR_NEEDMOREPARAMS");
        return (ERR_NEEDMOREPARAMS);
    }
    std::istringstream issChannels(value);
    while (std::getline(issChannels, value, ',')) {
        if (Channel::isValidChannelName(value)){
            channels.push_back(value);
        }
        else {
            LOG_CMD.error(TO_STRING(ERR_BADCHANMASK) + " ERR_BADCHANMASK");
            return (ERR_BADCHANMASK);
        }
    }
    if (iss >> value) {
        std::istringstream issKeys(value);
        while (std::getline(issKeys, value, ',')) {
            keys.push_back(value);
        }
    }
    if (iss >> value)
        std::cout << "to many params\n";
}


if no params
    return no params
if chanel badmaks ERR_BADCHANMASK avoir # ou & max 50 char , char forbidden-> ' '   ,   :   \a
if channel exist
if chanel is protected ERR_BADCHANNELKEY
if chanel is full ERR_CHANNELISFULL
if chanel invite only ERR_INVITEONLYCHAN
if banned from channel ERR_BANNEDFROMCHAN

477 ERR_NOCHANMODES +channelname = channel mode less on peut mettre de mode
#channel global -> multiserver
&channel local -> un seul serveur
+channel ->channel mode-less
!channel -> reçoit un id aleatoire de 5 char = !acfdechannel
