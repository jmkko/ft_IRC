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
        if (value[0] == '#' || value[0] == '&'){
            if (server)
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
