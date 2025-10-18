#ifndef KICK_HPP
#define KICK_HPP

#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <string>
#include <vector>

class Client;
class Server;

class Kick : public ICommand
{
  public:
    Kick(std::string& params);
    ~Kick();

    void	     		execute(Server& server, Client& client);

  private:
    Kick(void);
    Kick(const Kick& other);
    Kick& operator=(const Kick& other);

	std::vector<std::string> _channelsNames;
	std::vector<std::string> _usersNames;
	std::string				_msg;
};

#endif
