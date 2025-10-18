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
#include "Parser.hpp"

#include <sstream>
#include <string>

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

Kick::Kick() : _channelsNames(), _usersNames(), _msg() {}

Kick::Kick(std::string& params): ICommand()
{
	Parser parser;
	std::string channels,users,message;

	channels	= parser.format_parameter(params, NULL);
	users		= parser.format_parameter(params, NULL);
	_msg = parser.format_parameter(params, NULL);

	_channelsNames = parser.convert_to_vector(channels);
	_usersNames = parser.convert_to_vector(users);

}

Kick::Kick(const Kick& other) : ICommand(), _channelsNames(other._channelsNames), _usersNames(other._usersNames), _msg("") {}
Kick::~Kick() {}

/************************************************************
 *		➕ OPERATORS											*
 ************************************************************/

Kick& Kick::operator=(const Kick& other)
{
    if (this != &other) {
        _channelsNames = other._channelsNames;
        _usersNames    = other._usersNames;
        _msg           = other._msg;
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
        rh.process_response(client, ERR_NEEDMOREPARAMS, "KICK");
        return;
    }
    for (size_t i = 0; i < _channelsNames.size(); ++i) {
        Channel* channel = server.find_channel_by_name(_channelsNames[i]);
        Client*  target  = server.find_client_by_nickname(_usersNames[i]);
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
}

bool Kick::check_syntax(Server& server, Client& client, std::string& params)
{
	Parser 	parser(server, client);

	std::string 		channels	= parser.format_parameter(params, &Parser::correct_channel);
	std::string 		targets 	= parser.format_parameter(params, NULL);
	std::string			trailing 	= parser.format_parameter(params, NULL);

	if (channels.empty() || targets.empty()) {
		parser.rh->process_response(client, ERR_NEEDMOREPARAMS, "KICK");
		return (false);
	}
	params = channels + " " + targets + " " + trailing;
	LOG_CMD.error("params: " + params);
	return (true);
}

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
    ReplyHandler       rh = ReplyHandler::get_instance(&server);
    std::istringstream iss(params);
    std::string        channels, users;
    std::string        channelsList, usersList, msg;

    iss >> channels;
    iss >> users;
    if (channels.empty() || users.empty()) {
        return (ERR_NEEDMOREPARAMS);
    }
    std::string        currentChannel, currentUser, comma;
    std::istringstream issChannels(channels), issUsers(users);
    while (std::getline(issChannels, currentChannel, ',')) {
        std::getline(issUsers, currentUser, ',');
        if (!Channel::is_valid_channel_name(currentChannel)) {
            rh.process_response(client, ERR_BADCHANMASK, currentChannel);
        } else if (!server.find_client_by_nickname(currentUser)) {
            rh.process_response(client, ERR_NOSUCHNICK, currentUser);
        } else if (!currentChannel.empty() && !currentUser.empty()) {
            channelsList += comma + currentChannel;
            usersList += comma + currentUser;
            comma = ",";
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
    msg.erase(0, msg.find_first_not_of(WHITE_SPACE));
    if (!msg.empty() && msg[0] == ':')
        msg.erase(0, 1);
    else if (!msg.empty())
        msg = msg.substr(0, msg.find_first_of(WHITE_SPACE));
    params = channelsList + " " + usersList + " " + msg;

    LOG_CMD.warning("check_aaargs: " + params);
    return (CORRECT_FORMAT);
}
