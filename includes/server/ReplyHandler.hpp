#ifndef REPLY_HANDLER_HPP
#define REPLY_HANDLER_HPP

#include "Client.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

class Server;

class ReplyHandler
{
  public:
	std::string get_id_of(Client& client, const std::string& name = "");
	std::string select_response(Client& client, ReplyCode code, const std::string& parameter);
	int			process_response(Client& client, ReplyCode code, const std::string& parameters = "");

	static ReplyHandler& get_instance(Server* s);

  private:
	Server* _server;

	ReplyHandler(Server* s);

	void _send_reply(Client& c, const std::string& msg);
};

#endif
