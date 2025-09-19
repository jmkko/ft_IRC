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

    ICommand* makeCommand(Server&, Client&, std::string&);
    ICommand* userCmd(Server&, Client&, std::string&);
    ICommand* passCmd(Server&, Client&, std::string&);
    ICommand* nickCmd(Server&, Client&, std::string&);
    ICommand* quitCmd(Server&, Client&, std::string&);
    ICommand* joinCmd(Server&, Client&, std::string&);
    ICommand* partCmd(Server&, Client&, std::string&);
    ICommand* modeCmd(Server&, Client&, std::string&);
    ICommand* operCmd(Server&, Client&, std::string&);
    ICommand* inviteCmd(Server&, Client&, std::string&);
};

#endif
