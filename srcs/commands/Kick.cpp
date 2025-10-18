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

Kick::Kick(std::string& params): ICommand()
{
	Parser parser;
	std::string channels,users;

	channels	= parser.format_parameter(params, NULL);
	users		= parser.format_parameter(params, NULL);
	_msg 		= parser.format_parameter(params, NULL);

	_channelsNames = parser.convert_to_vector(channels);
	_usersNames = parser.convert_to_vector(users);

}

Kick::Kick(void): _channelsNames(), _usersNames(), _msg() {}
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

void Kick::execute(Server& server, Client& client)
{
	Parser 			p(server, client);
    LOG_CMD.debug("Kick.cpp execute()");

    if (_channelsNames.size() != _usersNames.size() || _channelsNames.size() == 0) {
        p.response(ERR_NEEDMOREPARAMS, "KICK");
        return;
    }
    for (size_t i = 0; i < _channelsNames.size(); ++i) {
		if (!p.correct_channel(_channelsNames[i]))
			continue ;
        Channel* channel = server.find_channel_by_name(_channelsNames[i]);
        Client*  target  = server.find_client_by_nickname(_usersNames[i]);
        LOG_CMD.debug("Kick.cpp execute() -->" + _channelsNames[i] + " " + _usersNames[i] + " " + _msg);
        if (!channel) {
            p.response(ERR_NOSUCHCHANNEL, _channelsNames[i]);
        } else if (!channel->is_operator(client)) {
            p.response(ERR_CHANOPRIVSNEEDED, channel->get_name());
        } else if (target && channel->remove_member(*target)) {
            p.rh->process_response(*target, TRANSFER_KICK, channel->get_name() + " " + target->get_nickname(), &client, _msg);
            channel->broadcast(server, TRANSFER_KICK, channel->get_name() + " " + target->get_nickname(), &client, _msg);
            p.rh->process_response(client, TRANSFER_KICK, channel->get_name() + " " + target->get_nickname(), &client, _msg);
        } else {
            p.response(ERR_USERNOTINCHANNEL, channel->get_name());
        }
    }
}
