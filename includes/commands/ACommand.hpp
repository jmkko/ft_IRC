#ifndef ACOMMAND_HPP
#define ACOMMAND_HPP
#include <iostream>

class Server;
class Client;
// interface of each commands
class ACommand
{
  public:
    virtual ~ACommand();
    virtual void execute(Server&, Client&) = 0;

  protected:
  	std::string	_name;

	ACommand();

  private:

	ACommand(const ACommand& inst);

	ACommand&	operator=(const ACommand& inst);
};

#endif
