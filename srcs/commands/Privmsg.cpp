#include "Privmsg.hpp"

#include "Client.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

/************************************************************
 *		📁 CLASS METHODS									*
 ************************************************************/

 /**
 * @brief check syntaxic validity of params
 * - message (after `:`) should not be empty
 * - at least one target (channel or sender)
 * - at max MAX_TARGET (configuration defined)
 * as well as following
 * - targets should be existing nicks or channels
 * @param server
 * @param client sender
 * @param params string following the command name
 * @return ReplyCode corresponding to RFC ERR replies or CORRECT_FORMAT if syntax is correct 
 */
ReplyCode Privmsg::check_args(Server& server, Client& client, std::string& params)
{

    std::string::size_type pos = params.find(" :");
	if (params.empty())
		return (ERR_NEEDMOREPARAMS);
	if (pos == std::string::npos || pos + 2 >= params.size())
		return ERR_NOTEXTTOSEND;

    ReplyHandler rh          = ReplyHandler::get_instance(&server);
    std::string  msg         = params.substr(pos);
    int          targetLimit = TARGET_LIMIT;
    std::string  targetList;
    params = params.substr(0, pos);
    std::stringstream ss(params);
    std::string       target;

    while (std::getline(ss, target, ',')) {
        LOG_DTV_CMD(target);
        if (targetLimit <= 0) {
            rh.process_response(client, ERR_TOOMANYTARGETS, target);
            break;
        }
        if (Channel::is_valid_channel_name(target))
        {
            std::map<std::string, Channel*>::iterator chan = server.channels.find(target);
            if (chan != server.channels.end()) {
                LOG_D_CMD("add channel", target);
                targetList += target + " ";
            }
            else {
                rh.process_response(client, ERR_NOSUCHCHANNEL, target);
            }
        } 
        else if (server.find_client_by_nickname(target)) {
            LOG_D_CMD("add client", target);
            targetList += target + " ";
        } 
        else {
            rh.process_response(client, ERR_NOSUCHNICK, target);
        }
        targetLimit--;
    }
    if (targetList.empty())
        return (ERR_NORECIPIENT);
    params = targetList + msg;
    return CORRECT_FORMAT;
}

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

/**
 * @brief Construct a new Privmsg:: Privmsg object
 * 
 * @param params 
 */
Privmsg::Privmsg(const std::string& params) : _params(params), _chans(0), _dests(0) {}

/**
 * @brief Destroy the Privmsg:: Privmsg object
 * 
 */
Privmsg::~Privmsg(void) {}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

 /**
  * @brief proceed to extra checks (presence of sender on target channel) and to message transfer
  * - message is transferred to target client
  * - or broadcasted to channel for other members (sender excepted)
  * @param server 
  * @param client sender
  */
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

/**
 * @brief adds a channel to the list of valid channels
 * 
 * @param chan 
 */
void Privmsg::_add_channel(Channel* chan)
{
    if (!chan) {
        return;
    }
    _chans.push_back(chan);
};

/**
 * @brief adds a client to the list of valid clients
 * 
 * @param client 
 */
void Privmsg::_add_client(Client* client)
{
    if (!client) {
        return;
    }
    _dests.push_back(client);
};

/**
 * @brief util method to parse args into target clients, channels and message before execution
 * 
 * @param server 
 * @param params 
 */
void Privmsg::_build_args(Server& server, std::string& params)
{
    std::istringstream                        iss(params);
    std::string                               target;
    Client*                                   client = NULL;
    std::map<std::string, Channel*>::iterator chan;
    while (iss >> target && target[0] != ':') {
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

    std::string::size_type pos = params.find(" :");
    if (pos != std::string::npos) {
        _msg = params.substr(pos + 2);
    }
    else {
        _msg = "";
    }
}
