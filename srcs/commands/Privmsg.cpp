#include "Privmsg.hpp"

#include "Client.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

/************************************************************
 *		📁 CLASS METHODS									*
 ************************************************************/

ReplyCode Privmsg::check_args(Server& server, Client& client, std::string& params)
{

    ReplyHandler       rh          = ReplyHandler::get_instance(&server);
    int                targetLimit = TARGET_LIMIT;
    std::string        targetList, msg, channels, target, coma;
    std::istringstream iss(params);

    iss >> channels;
    std::istringstream issChan(channels);
    while (std::getline(issChan, target, ',')) {
        LOG_DTV_CMD(target);
        if (targetLimit <= 0) {
            rh.process_response(client, ERR_TOOMANYTARGETS, target);
            break;
        }
        if (Channel::is_valid_channel_name(target)) {
            std::map<std::string, Channel*>::iterator chan = server.channels.find(target);
            if (chan != server.channels.end()) {
                LOG_D_CMD("add channel", target);
                targetList += coma + target;
                coma = ",";
            } else {
                rh.process_response(client, ERR_NOSUCHCHANNEL, target);
            }
        } else if (server.find_client_by_nickname(target)) {
            LOG_D_CMD("add client", target);
            targetList += coma + target;
            coma = ",";
        } else {
            rh.process_response(client, ERR_NOSUCHNICK, target);
        }
        targetLimit--;
    }
    if (targetList.empty()) {
        return (ERR_NORECIPIENT);
    }
    std::getline(iss, msg);
    msg.erase(0, msg.find_first_not_of(WHITE_SPACE));
    if (!msg.empty() && msg[0] == ':')
        msg = msg.erase(0, 1);
    else if (!msg.empty())
        msg = msg.substr(0, msg.find_first_of(WHITE_SPACE));

    params = targetList + " " + msg;
    return CORRECT_FORMAT;
}

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

Privmsg::Privmsg(const std::string& params) : _params(params), _chans(0), _dests(0) {}

Privmsg::~Privmsg(void) {}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

void Privmsg::execute(Server& server, Client& client)
{
    _build_args(server, _params);

    ReplyHandler rh = ReplyHandler::get_instance(&server);

    LOG_DV_CMD(_msg);
    for (std::vector<Channel*>::iterator it = _chans.begin(); it != _chans.end(); it++) {
        if ((*it)->is_member(client)) {
            (*it)->broadcast(server, TRANSFER_PRIVMSG, (*it)->get_name(), &client, _msg);
        } else {
            rh.process_response(client, ERR_NOTONCHANNEL, (*it)->get_name());
        }
    }
    for (std::vector<Client*>::iterator it = _dests.begin(); it != _dests.end(); it++) {
        rh.process_response(*(*it), TRANSFER_PRIVMSG, (*it)->get_nickname(), &client, _msg);
    }
}

void Privmsg::_add_channel(Channel* chan)
{
    if (!chan) {
        return;
    }
    _chans.push_back(chan);
};

void Privmsg::_add_client(Client* client)
{
    if (!client) {
        return;
    }
    _dests.push_back(client);
};

void Privmsg::_build_args(Server& server, std::string& params)
{
    std::istringstream                        iss(params);
    std::string                               target, msg, channels;
    Client*                                   client = NULL;
    std::map<std::string, Channel*>::iterator chan;

    iss >> channels;
    std::istringstream issChan(channels);
    while (std::getline(issChan, target, ',')) {
        chan = server.channels.find(target);
        if (chan != server.channels.end()) {
            _add_channel(chan->second);
        }
        client = server.find_client_by_nickname(target);
        if (client) {
            _add_client(server.find_client_by_nickname(target));
        }
        if (chan == server.channels.end() && !client) {
            LOG_w_CMD(target + " is not a channel nor a client");
        }
    }

    std::getline(iss, msg);
    msg.erase(0, msg.find_first_not_of(WHITE_SPACE));
    if (!msg.empty() && msg[0] == ':')
        msg = msg.substr(1);
    else if (!msg.empty())
        msg = msg.substr(0, msg.find_first_of(WHITE_SPACE));
    _msg = msg;
}
