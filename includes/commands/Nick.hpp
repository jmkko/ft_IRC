#ifndef NICK_HPP
#define NICK_HPP

#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <string>

class Server;
class Client;

class Nick : public ICommand
{
  public:
    explicit Nick(const std::string& nickname);
    ~Nick();

    void             execute(Server& s, Client& c);
    static ReplyCode check_args(Server& s, Client& c, std::string& params);

  private:
    std::string _nickname;

    Nick();
    Nick(const Nick& other);

    Nick& operator=(const Nick& other);
};

#endif
