#ifndef REPLY_HANDLER_HPP
#define REPLY_HANDLER_HPP

#include "Client.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"
#include "Config.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

class Server;

class ReplyHandler
{
  public:
	std::string		selectResponse(Client& client, ReplyCode code, const std::string& parameter);
	std::string		getIdOf(Client& client, std::string name = "");
	int 			processResponse(Client& client, ReplyCode code, const std::string& parameters = "");
    void			sendReply(Client&, std::string& msg);

    static ReplyHandler& getInstance(Server*);

  private:
    Server* _server;

    ReplyHandler(Server*);
};

#endif
