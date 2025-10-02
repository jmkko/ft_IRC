#include "Privmsg.hpp"
#include "Server.hpp"
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

	for (std::vector<Channel*>::iterator it = _chans.begin(); it != _chans.end(); it++){
		LOG_CMD.debug("channel " + (*it)->get_name());
	}

	LOG_CMD.debug("message = " + _msg);

	for (std::vector<Channel*>::iterator it = _chans.begin(); it != _chans.end(); it++) {
		LOG_CMD.debug("PIVMSG execute -> broadcast");
		(*it)->broadcast(server, RPL_PRIVMSG, (*it)->get_name() + _msg, &client);
	}
	for (std::vector<Client*>::iterator it = _dests.begin(); it != _dests.end(); it++) {
		LOG_CMD.debug("PIVMSG execute -> destinataire");
		rh.process_response(*(*it), RPL_PRIVMSG, (*it)->get_nickname() + _msg, &client);
	}

}

void			Privmsg::add_channel(Channel* chan) {_chans.push_back(chan);};
void			Privmsg::add_client(Client *client) {_dests.push_back(client);};

ReplyCode Privmsg::check_args(Server& server, Client& client, std::string& params) {

	(void)server;
	(void)client;

	if (params.empty() || params.find(" :") == std::string::npos)
		return ERR_NOTEXTTOSEND;

	std::string word;
	std::string channels;
	std::string::size_type pos = params.find(" :");
	std::string msg = params.substr(pos);
	params = params.substr(0, pos);
	std::istringstream iss(params);

	while (iss >> word) {
		std::map<std::string, Channel*>::iterator chan = server.channels.find(word);
		if (chan != server.channels.end()) {
			channels += word + " ";
			LOG_CMD.debug("add channel: " + word);
		} else if (server.find_client_by_nickname(word)) {
			channels += word + " ";
			LOG_CMD.debug("add client: " + word);
		} else {
			LOG_CMD.error(word + " is not a channel nor a client");
		}
	}

	params = channels + msg;
	return RPL_SUCCESS;
}
