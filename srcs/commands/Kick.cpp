#include "Kick.hpp"

#include "Channel.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <sstream>
#include <string>

/*************************************************************
 *		🛠️ UTIL FUNCTIONS											*
 *************************************************************/

/************************************************************
 *		📁 CLASS METHODS									*
 ************************************************************/

/**
 * @brief
 * first check for missing arg -> ERR_NEEDMOREPARAMS
 * @param server
 * @param client
 * @param args
 * @return ReplyCode to be used in CommandFactory and potentially generate an error
 */
ReplyCode Kick::check_args(Server& server, Client& client, std::string& params)
{
	ReplyHandler rh = ReplyHandler::get_instance(&server);
	std::istringstream iss(params);
	std::string  channels, users;
	std::string	channelsList, usersList, msg;	

	iss >> channels;
	iss >> users;
	if (channels.empty() || users.empty()) {
		return (ERR_NEEDMOREPARAMS);
	}
	std::string currentChannel, currentUser, coma;
	std::istringstream issChannels(channels), issUsers(users);
	while(std::getline(issChannels, currentChannel, ',')) {
		std::getline(issUsers, currentUser, ',');
		if (!Channel::is_valid_channel_name(currentChannel)) {
			rh.process_response(client, ERR_BADCHANMASK, currentChannel);
		} else if (!server.find_client_by_nickname(currentUser)) {
			rh.process_response(client, ERR_NOSUCHNICK, currentUser);
		} else if (!currentChannel.empty() && !currentUser.empty()) {
			channelsList += coma + currentChannel;
			usersList += coma + currentUser;
			coma = ",";
		} else if (currentChannel.empty() != currentChannel.empty()) {
			return (ERR_NEEDMOREPARAMS);
		}
		currentChannel.clear();
		currentUser.clear();
	}
	if (channelsList.empty() || usersList.empty()) {
		return (PROCESSED_ERROR);
	}
	std::getline(iss, msg);
	if (!msg.empty() && msg[0] == ':')
		msg = msg.substr(1);
	else
		msg = msg.substr(0, msg.find_first_of(WHITE_SPACE));
	params = channelsList + " " + usersList + " " + msg;

    return (CORRECT_FORMAT);
}


/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

Kick::Kick() : _channelsNames(), _usersNames(), _msg("") {}
Kick::Kick(const std::string& params)
{
	std::istringstream iss(params);
	std::string  channels, users, msg;

	iss >> channels;
	iss >> users;
	std::string currentChannel, currentUser;
	std::istringstream issChannels(channels), issUsers(users);
	while(std::getline(issChannels, currentChannel, ',')) {
		std::getline(issUsers, currentUser, ',');
		if (!currentChannel.empty())
			_channelsNames.push_back(currentChannel);
		if (!currentUser.empty())
			_usersNames.push_back(currentUser);
		currentChannel.clear();
		currentUser.clear();
	}
	std::getline(iss, msg);
	if (!msg.empty() && msg[0] == ':') {
		_msg = msg.substr(1);
	} else {
		_msg = msg;
	}
}

Kick::Kick(const Kick& other) : ICommand(), _channelsNames(other._channelsNames), _usersNames(other._usersNames), _msg(""){}
Kick::~Kick() {}

/************************************************************
 *		➕ OPERATORS											*
 ************************************************************/

Kick& Kick::operator=(const Kick& other)
{
    if (this != &other) {
        _channelsNames = other._channelsNames;
		_usersNames = other._usersNames;
		_msg = other._msg;
    }
    return *this;
}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

/**
 * @brief Force part a channel
 * loop over channels :
 * check for invalid channel name -> ERR_BADCHANMASK
 * then check for existing channel -> ERR_NOSUCHCHANNEL
 * then check for op privilege -> ERR_CHANOPRIVSNEEDED
 * inner loop over channel members :
 * - then check for user presence -> ERR_USERNOTINCHANNEL
 * - then execute : remove - broadcast + direct notice
 * @param server
 * @param client
 */
void Kick::execute(Server& server, Client& client)
{
    ReplyHandler& rh = ReplyHandler::get_instance(&server);
    t_params      channelNames;
    t_params      nicknames;
	LOG_CMD.debug("Kick.cpp execute() -->");

	if (_channelsNames.size() != _usersNames.size()) {
		rh.process_response(client, ERR_NEEDMOREPARAMS, "");
		return ;
	}
	for (size_t i = 0; i < _channelsNames.size(); ++i) {
		Channel *channel = server.find_channel_by_name(_channelsNames[i]);
		Client	*target = server.find_client_by_nickname(_usersNames[i]);
		LOG_CMD.debug("Kick.cpp execute() -->" + _channelsNames[i] + " " + _usersNames[i] + " " + _msg);
		if (!channel) {
            rh.process_response(client, ERR_NOSUCHCHANNEL, _channelsNames[i]);
		} else if (!channel->is_operator(client)) {
            rh.process_response(client, ERR_CHANOPRIVSNEEDED, channel->get_name());
		} else if (!target) {
            rh.process_response(client, ERR_NOSUCHNICK, channel->get_name());
		} else if (channel->remove_member(*target)) {
    	    rh.process_response(*target, TRANSFER_KICK, channel->get_name() + " " + target->get_nickname(), &client, _msg);
    		channel->broadcast(server, TRANSFER_KICK, channel->get_name() + " " + target->get_nickname(), &client, _msg);
			rh.process_response(client, TRANSFER_KICK, channel->get_name() + " " + target->get_nickname(), &client, _msg);
		} else {
            rh.process_response(client, ERR_USERNOTINCHANNEL, channel->get_name());
		}
	}


    // for (t_params::iterator chanNamesIt = channelNames.begin(); chanNamesIt != channelNames.end(); ++chanNamesIt) {
    //     if (!Channel::is_valid_channel_name(*chanNamesIt)) {
    //         continue;
    //     }
    //     std::map<std::string, Channel*>::iterator chanIt = server.channels.find(*chanNamesIt);
    //     if (chanIt == server.channels.end()) {
    //         continue;
    //     }
    //     Channel* channel = chanIt->second;
    //     if (!channel->is_operator(client)) {
    //         rh.process_response(client, ERR_CHANOPRIVSNEEDED, channel->get_name());
    //         continue;
    //     }
    //     for (t_params::iterator nickIt = nicknames.begin(); nickIt != nicknames.end(); ++nickIt) {
    //         Client* targetUser = server.find_client_by_nickname(static_cast<const std::string&>(*nickIt));
    //         if (!channel->is_member(*targetUser)) {
    //             rh.process_response(client, ERR_USERNOTINCHANNEL, channel->get_name());
    //         } else {
    //             if (!validUsers.empty())
    //                 validUsers += ",";
    //             validUsers += *nickIt;
    //             channel->remove_member(*targetUser);
    //             std::string messageParam = channel->get_name() + " " + targetUser->get_nickname();
    //             LOG_d_CMD("before broadcast");
    //             rh.process_response(*targetUser, TRANSFER_KICK, messageParam, &client, comment);
    //         }
    //     }
    //     if (!validChans.empty())
    //         validChans += ",";
    //     validChans += *chanNamesIt;
    //     broadcastMsg.append(validChans).append(" ").append(validUsers);
    //     channel->broadcast(server, TRANSFER_KICK, broadcastMsg, &client, comment);
    //     rh.process_response(client, TRANSFER_KICK, broadcastMsg, &client, comment);
    // }
}
