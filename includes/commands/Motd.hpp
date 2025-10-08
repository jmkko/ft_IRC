#ifndef MOTD_HPP
#define MOTD_HPP

#include "Client.hpp"
#include "ICommand.hpp"
#include "LogManager.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <string>

class Client;
class Server;

class Motd : public ICommand
{
  public:
    Motd(const std::string& params);
    ~Motd();                            // NOLINT
    void execute(Server& s, Client& c); // NOLINT

  private:
    std::string _str_replace(std::string str, std::string find, std::string replace);
    std::string _params;
    Motd();
    Motd(const Motd& other);
    Motd& operator=(const Motd& other);
};

#endif
