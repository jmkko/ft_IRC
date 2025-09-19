#ifndef CMDFACTORY_HPP
#define CMDFACTORY_HPP
#include "LogManager.hpp"
#include "Nick.hpp"
#include "consts.hpp"
#include <iostream>
#include <sstream>

class Server;
class Client;

class CmdFactory
{
  public:
    CmdFactory(void);
    CmdFactory(Server& server, Client& client);
    CmdFactory(const CmdFactory& other);
    CmdFactory& operator=(const CmdFactory& other);
    ~CmdFactory();

    ACommand* makeCommand(Server&, Client&, std::string&);
    ACommand* userCmd(Server&, Client&, std::string&);
    ACommand* passCmd(Server&, Client&, std::string&);
    ACommand* nickCmd(Server&, Client&, std::string&);
    ACommand* quitCmd(Server&, Client&, std::string&);
    ACommand* joinCmd(Server&, Client&, std::string&);
    ACommand* partCmd(Server&, Client&, std::string&);
    ACommand* modeCmd(Server&, Client&, std::string&);
    ACommand* operCmd(Server&, Client&, std::string&);
    ACommand* inviteCmd(Server&, Client&, std::string&);
};

#endif
