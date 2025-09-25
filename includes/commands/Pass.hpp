#ifndef PASS_HPP
# define PASS_HPP
# include "ICommand.hpp"
# include <iostream>
# include "reply_codes.hpp"

class Server;
class Client;

class Pass : public ICommand
{
  public:
    Pass(void);
    Pass(const std::string& pass);
    Pass(const Pass& other);
    Pass& operator=(const Pass& other);
    ~Pass();

    void       execute(Server&, Client&);
    static ReplyCode checkArgs(Server&, Client&, std::string&);

  private:
    std::string _pass;
};

#endif
