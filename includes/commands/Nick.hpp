#ifndef NICK_HPP
#define NICK_HPP
#include "ACommand.hpp"
#include <iostream>

class Server;
class Client;

class Nick : public ACommand
{
  public:
    Nick(const std::string& nickname);
    ~Nick();

    void execute(Server&, Client&);

  private:
    std::string _nickname;

    Nick();
    Nick(const Nick& other);

    Nick& operator=(const Nick& other);
};

#endif
