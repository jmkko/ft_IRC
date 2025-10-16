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
    User(void);
    User(const std::string& username, const std::string& realname);
    User(const User& other);
    ~User();

    User& operator=(const User& other);

    void             execute(Server& s, Client& c);
    static ReplyCode check_args(Server& s, Client& c, std::string& params);

  private:
    std::string _username;
    std::string _realname;
};

#endif
