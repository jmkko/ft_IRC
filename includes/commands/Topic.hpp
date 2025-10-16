#ifndef TOPIC_HPP
#define TOPIC_HPP

#include "ICommand.hpp"
#include "reply_codes.hpp"
#include <iostream>

class Channel;

class Topic : public ICommand
{
  public:
    Topic(Server& s, std::string& params);
    ~Topic();

    void                execute(Server& s, Client& c);
    static ReplyCode    check_args(Server& s, Client& c, std::string& params);

  private:
    std::string _topic;
	Channel* 	_chan;

    Topic(void);
    Topic(const Topic& other);
    Topic& operator=(const Topic& other);
};

#endif
