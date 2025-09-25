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
    void sendReply(Client&, ReplyCode, const std::string& target, const std::string& trailing = "");
    void sendSimpleReply(Client&, ReplyCode, const std::string& trailing = "");
    void sendErrNeedMoreParams(Client&, const std::string& commandName);
    static ReplyHandler& getInstance(Server*);

  private:
    Server* _server;

    ReplyHandler(Server*);
};

#endif
