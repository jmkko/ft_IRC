#ifndef JOIN_HPP
#define  JOIN_HPP

#include "ICommand.hpp"
#include "Channel.hpp"


class Join : public ICommand{
  public:
    Join(const std::string& name);
    void       execute(Server&, Client&);
    static int checkArgs(Server&, Client&, std::string&);
  private:
    
};
