#ifndef NICK_HPP
#define NICK_HPP
#include "ICommand.hpp"
#include <iostream>

class Server;
class Client;

class Nick : public ICommand
{
  public:
    Nick(void);
    Nick(const Nick& other);
    Nick(const std::string& nickname);
    Nick& operator=(const Nick& other);
    ~Nick();

    void execute(Server&, Client&);

  private:
    std::string _nickname;
};

#endif
