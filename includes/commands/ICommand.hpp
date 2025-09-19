#ifndef ICOMMAND_HPP
#define ICOMMAND_HPP
#include <iostream>

class Server;
class Client;
// interface of each commands
class ICommand
{
  public:
    virtual ~ICommand() {};
	virtual void execute(Server&, Client&) = 0;
};

#endif
