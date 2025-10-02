#ifndef PRIVMSG_HPP
#define PRIVMSG_HPP
#include "Channel.hpp"
#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <iostream>

class Privmsg : public ICommand
{
  public:
    Privmsg(void);
    Privmsg(const Privmsg& other);
    Privmsg(const std::string& msg);
    Privmsg& operator=(const Privmsg& other);
    ~Privmsg();

	void			add_client(Client * client);
	void			add_channel(Channel* chan);
    void             execute(Server& s, Client& c);
    static ReplyCode check_args(Server& s, Client& c, std::string& p);

  private:
    std::string           _msg;
    std::vector<Channel*> _chans;
	std::vector<Client*>  _dests;
};

#endif
