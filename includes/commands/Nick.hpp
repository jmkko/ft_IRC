#ifndef NICK_HPP
#define NICK_HPP
#include "ICommand.hpp"
#include <iostream>
#include "Config.hpp"
#include "reply_codes.hpp"

class Server;
class Client;

class Nick : public ICommand
{
  public:
    explicit Nick(const std::string& nickname);
    ~Nick();

    void       execute(Server&, Client&);
    static ReplyCode checkArgs(Server&, Client&, std::string&);

  private:
    std::string _nickname;

    Nick();
    Nick(const Nick& other);

    Nick& operator=(const Nick& other);
};

#endif
