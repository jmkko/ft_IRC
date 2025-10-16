#ifndef PRIVMSG_HPP
#define PRIVMSG_HPP
#include "Channel.hpp"
#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <iostream>
#include <string>

class Privmsg : public ICommand
{
  public:
    Privmsg(const std::string& params);
    ~Privmsg();

    static ReplyCode check_args(Server& s, Client& c, std::string& p);
    void             execute(Server& s, Client& c);

  private:
    std::string           _params;
    std::string           _msg;
    std::vector<Channel*> _chans;
    std::vector<Client*>  _dests;

    Privmsg(void);
    Privmsg(const Privmsg& other);

    Privmsg& operator=(const Privmsg& other);

    void             _build_args(Server& server, std::string& params);
    void             _add_client(Client* client);
    void             _add_channel(Channel* chan);
};

#endif
