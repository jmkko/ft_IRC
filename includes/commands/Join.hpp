#ifndef JOIN_HPP
#define  JOIN_HPP

#include "ICommand.hpp"
#include "Channel.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"
#include <vector>

struct ChannelParam {
    std::string channel;
    std::string key;
    bool        isValid = true;
    ChannelParam(const std::string& a, std::string& b) : channel(a), key(b) {}
    ChannelParam(const std::string& a) : channel(a) {}
    ChannelParam(const std::string& a, bool valid) : channel(a), isValid(valid) {}
};


class Join : public ICommand{
  public:
    Join(std::vector<ChannelParam> channelsLst);
    ~Join();
    void       execute(Server&, Client&);
    static std::vector<ChannelParam> checkArgs(Server&, Client&, std::string&);
  private:
    std::vector<ChannelParam> _channelsLst;
    Join();
    Join(const Join& mother);
    Join& operator=(const Join& other);
};

#endif
