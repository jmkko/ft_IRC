#include "Privmsg.hpp"

#include "Client.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"
#include "Parser.hpp"
/************************************************************
 *		📁 CLASS METHODS									*
 ************************************************************/

ReplyCode Privmsg::check_args(Server& server, Client& client, std::string& params)
{

    ReplyHandler       rh          = ReplyHandler::get_instance(&server);
    int                targetLimit = TARGET_LIMIT;
    std::string        targetList, msg, channels, target, comma;
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
                targetList += comma + target;
                comma = ",";
            } else {
                rh.process_response(client, ERR_NOSUCHCHANNEL, target);
            }
        } else if (server.find_client_by_nickname(target)) {
            LOG_D_CMD("add client", target);
            targetList += comma + target;
            comma = ",";
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
	if (msg.empty() || msg.find(" :") + 2 >= msg.size()) {
	 	return (ERR_NOTEXTTOSEND);
	}
    params = targetList + " " + msg;

    return CORRECT_FORMAT;
}

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

Privmsg::Privmsg(std::string& params)
{
	Parser parser;

	std::string targetList = parser.format_parameter(params, NULL);
	_targets = parser.convert_to_vector(targetList); 
	_message = parser.format_parameter(params, NULL);
}

Privmsg::~Privmsg(void) {}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

void Privmsg::execute(Server& server, Client& client)
{
	Parser p(server, client);
	Parser silent;

	if (_targets.size() == 0) {
		p.response(ERR_NEEDMOREPARAMS, "PRIVMSG");
	}
	if (_message.empty()) {
		p.response(ERR_NOTEXTTOSEND);
		return ;
	}
    int                targetLimit = TARGET_LIMIT;
	for (std::vector<std::string>::iterator it = _targets.begin(); it != _targets.end(); it++) {
        if (targetLimit <= 0) {
            p.response(ERR_TOOMANYTARGETS, *it);
            break;
        }
        if (silent.correct_channel(*it)) {
            Channel* chan = server.find_channel_by_name(*it);
            if (chan && chan->is_member(client)) {
				chan->broadcast(server, TRANSFER_PRIVMSG, *it, &client, _message);
            } else if (chan) {
                p.response(ERR_NOTONCHANNEL, *it);
            } else {
                p.response(ERR_NOSUCHCHANNEL, *it);
			} 
        } else {
			Client* user = server.find_client_by_nickname(*it);
			if (user) {
				p.rh->process_response(*user, TRANSFER_PRIVMSG, *it, &client, _message);
        	} else {
            	p.response(ERR_NOSUCHNICK, *it);
        	}
		}
        targetLimit--;
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
