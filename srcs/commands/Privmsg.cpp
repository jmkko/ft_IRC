#include "Privmsg.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"
#include "Client.hpp"

// Default constructor
Privmsg::Privmsg(void): _msg(""), _chans(0), _dests(0) {}
Privmsg::Privmsg(const std::string &msg): _msg(msg), _chans(0), _dests(0) {} 
Privmsg::Privmsg(const Privmsg &other): _msg(other._msg), _chans(other._chans), _dests(other._dests) {}

// Assignment operator overload
Privmsg &Privmsg::operator=(const Privmsg &other)
{
	if (this != &other) {
		_msg = other._msg;
		_chans = other._chans;
		_dests = other._dests;
	}
	return (*this);
}

// Destructor
Privmsg::~Privmsg(void) {}

void Privmsg::execute(Server& server, Client& client) 
{
	ReplyHandler rh = ReplyHandler::get_instance(&server);

	for (std::vector<Channel*>::iterator it = _chans.begin(); it != _chans.end(); it++) {
		(*it)->broadcast(server, RPL_PRIVMSG, (*it)->get_name() + _msg, &client);
	}
	for (std::vector<Client*>::iterator it = _dests.begin(); it != _dests.end(); it++) {
		rh.process_response(*(*it), RPL_PRIVMSG, (*it)->get_nickname() + _msg, &client);
	}
}

void			Privmsg::add_channel(Channel* chan) {if (!chan) {return;} _chans.push_back(chan);};
void			Privmsg::add_client(Client *client) {if (!client) {return;} _dests.push_back(client);};
void			Privmsg::build_args(Server& server, std::string& params)
{
	std::istringstream iss(params);
	std::string target;
	Client* client = NULL;
	std::map<std::string, Channel*>::iterator chan ;
	while (iss >> target) {
		chan = server.channels.find(target);
		if (chan != server.channels.end()) {
			add_channel(chan->second);
		}
		client = server.find_client_by_nickname(target);
		if (client) {
			add_client(server.find_client_by_nickname(target));
		} else {
			LOG_CMD.error(target + " is not a channel nor a client");
		}
	}
}

/**
 * @brief [TODO:check arguments: TARGET_LIMIT include invalid targets. targets should be separated by a coma]
 *	
 * @param server [TODO:parameter]
 * @param client [TODO:parameter]
 * @param params [TODO:parameter]
 * @return [TODO:return]
 */
ReplyCode Privmsg::check_args(Server& server, Client& client, std::string& params) {

	std::string::size_type pos = params.find(" :");

	if (params.empty())
		return (ERR_NEEDMOREPARAMS);
	if (pos == std::string::npos || pos + 2 > params.size()) {
		return ERR_NOTEXTTOSEND;
	} 

	ReplyHandler rh = ReplyHandler::get_instance(&server);
	std::string msg = params.substr(pos);
	int targetLimit = TARGET_LIMIT;
	std::string targetList;
	params = params.substr(0, pos);
	std::stringstream ss(params);
	std::string target;

	while (std::getline(ss, target, ',')) {
		if (--targetLimit < 0) {
			rh.process_response(client, ERR_TOOMANYTARGETS, target);
			break;
		}
		std::map<std::string, Channel*>::iterator chan = server.channels.find(target);
		if (chan != server.channels.end()) {
			LOG_D_CMD("add channel", target);
			targetList += target + " ";
		} else if (server.find_client_by_nickname(target)) {
			LOG_D_CMD("add client", target);
			targetList += target + " ";
		} else {
			rh.process_response(client, ERR_NOSUCHNICK, target);
		}
		targetLimit--;

	}
	if (targetList.empty())
		return (ERR_NORECIPIENT);
	params = targetList + msg;
	return RPL_SUCCESS;
}
