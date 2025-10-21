#ifndef PART_HPP
#define PART_HPP
#include <iostream>
#include "ICommand.hpp"
class Server;
class Client;

class Part: public ICommand
{
  public:
    ~Part();
    Part(std::string& params);

    void execute(Server& server, Client& client);

  private:
    std::vector<std::string> _chanNames;
    std::string              _message;
	
    Part(void);
    Part(const Part& other);
    Part& operator=(const Part& other);
};

#endif
