#ifndef INVITE_HPP
#define INVITE_HPP

#include "Channel.hpp"
#include "Client.hpp"
#include "ICommand.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"

class Invite : public ICommand
{

  public:
    Invite(std::string& params);
    ~Invite();                                      // NOLINT
    void             execute(Server& s, Client& c); // NOLINT

  private:
	std::string _nickname;
	std::string _channelName;
    Invite();
    Invite(const Invite& other);
    Invite& operator=(const Invite& other);
};

#endif
