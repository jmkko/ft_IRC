#ifndef WHO_HPP
#define WHO_HPP

#include "Client.hpp"
#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <sstream>
#include <string>

class Client;
class Server;

class Who : public ICommand
{
  public:
    Who(const std::string& clientsLst);
    ~Who();
    void             execute(Server& s, Client& c);
    static ReplyCode check_args(Server& s, Client& c, std::string& params);

  private:
    std::string _clientsLst;
    Who();
    Who(const Who& other);
    Who& operator=(const Who& other);
    bool _is_valid_pattern(const std::string& pattern, const std::string& str) const;
};

#endif
