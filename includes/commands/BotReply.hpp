#ifndef BOT_REPLY_HPP
#define BOT_REPLY_HPP

#include "Channel.hpp"
#include "Client.hpp"
#include "ICommand.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"
#include <string>

#define BOT_PROCESS_TIME_MS 200
class Bot : public ICommand
{

  public:

    static const std::string& cmdName;

    Bot(std::string& params);
    ~Bot();
    
    void             execute(Server& s, Client& c);

  private:
    std::string                 _params;
    std::vector<std::string>    _targets;
    std::string                 _targetChannelName;
    std::vector<Channel*>       _targetChannels;
    std::vector<Client*>        _targetClients;
    std::string                 _subcommand;
    std::string                 _prompt;

    Bot();
    Bot(const Bot& other);
    Bot& operator=(const Bot& other);

    bool            _check_args(Server& s, Client& c);
};

#endif