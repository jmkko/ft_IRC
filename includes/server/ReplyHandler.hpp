#ifndef REPLY_HANDLER_HPP
#define REPLY_HANDLER_HPP

#include "Client.hpp"
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
    void sendReply(Client *client, ReplyCode, const std::string&target, const std::string& trailing = "");
    void sendErrNeedMoreParams(Client *client, const std::string& commandName);
    static ReplyHandler& getInstance(Server*);

  private:
    Server* _server;

    ReplyHandler(Server*);
};

#endif
