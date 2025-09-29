#include "Nick.hpp"
#include "utils.hpp"

#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"

#include <iostream>

Nick::Nick(const std::string& nickname) : _nickname(nickname) {}

Nick::~Nick(void) {}

void Nick::execute(Server& server, Client& client)
{
	(void)server;
	std::string oldNickname = client.get_nickname();
	client.set_nickname(_nickname);
	ReplyHandler& rh = ReplyHandler::get_instance(&server);
	// welcome sequence complete for first time
	if (oldNickname.empty() && !client.get_user_name().empty() && client.is_registered()) {
		LOG_CMD.info("001 RPL_WELCOME");
		rh.process_response(client, RPL_WELCOME);
		// normal success behavior
	} else if (!oldNickname.empty() && !client.get_user_name().empty() && client.is_registered()) {
		LOG_CMD.info("204 RPL_NICK");
		rh.process_response(client, RPL_NICK, oldNickname);
		rh.process_response(client,
							RPL_NOTICE,
							"Your nickname has changed"); // DOESNT NEED TO BE THERE (just for fun)
	}
}

ReplyCode Nick::check_args(Server& server, Client& client, std::string& params)
{
	(void)client;
	std::istringstream iss(params);
	std::string		   nickname;

	iss >> nickname;
	if (nickname.empty()) {
		LOG_CMD.error("431 ERR_NONICKNAMEGIVEN");
		return (ERR_NONICKNAMEGIVEN);
	} else if (!std::isalpha(nickname[0]) && !is_special_abnf(nickname[0])) {
		LOG_CMD.error("432 ERR_ERRONEUSNICKNAME");
		return (ERR_ERRONEUSNICKNAME);
	} else if (nickname.length() > ircConfig.get_nickname_max_len()) {
		nickname = nickname.substr(0, ircConfig.get_nickname_max_len());
	}
	if (server.find_client_by_nickname(nickname)) {
		LOG_CMD.error("431 ERR_NICKNAMEINUSE");
		return (ERR_NICKNAMEINUSE);
	}
	params = nickname;
	if (client.get_nickname().empty() && !client.get_user_name().empty() && client.is_registered()) {
		return (RPL_WELCOME);
	}
	return (RPL_SUCCESS);
}
