#ifndef TOPIC_HPP
#define TOPIC_HPP

#include "ICommand.hpp"
#include "reply_codes.hpp"
#include <iostream>

class Channel;

class Topic : public ICommand
{
  public:
    Topic(void);
    Topic(Server& s, std::string& params);
    Topic(const Topic& other);
    Topic& operator=(const Topic& other);
    ~Topic();

    void      execute(Server& s, Client& c);
    static ReplyCode check_args(Server& s, Client& c, std::string& params);
    void      build_args(Server& s, std::string& params);

  private:
    std::string _topic;
	Channel* 	_chan;
};

#endif
