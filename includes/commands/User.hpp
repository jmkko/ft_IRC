#ifndef USER_HPP
#define USER_HPP

#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <string>

class Server;
class Client;

class User : public ICommand
{
  public:
    User(std::string& params);
    ~User();

    User& operator=(const User& other);

    void             execute(Server& s, Client& c);
    static ReplyCode check_args(Server& s, Client& c, std::string& params);

  private:
    std::string _username;
	std::string _mode;
	std::string _unused;
    std::string _realname;
    User(const User& other);
    User(void);
};

#endif
