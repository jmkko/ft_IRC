#ifndef BOT_REPLY_HPP
#define BOT_REPLY_HPP

#include "Channel.hpp"
#include "Client.hpp"
#include "ICommand.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"

#define BOT_PROCESS_TIME_MS 200
class BotReply
{

  public:
    BotReply(const std::string& params);
    ~BotReply();
    static ReplyCode check_args(Server& s, Client& c, std::vector<Client*>& clients, std::vector<Channel*>& channels, std::string& params);
    void             execute(Server& s, Client& c, std::vector<Client*>& clients, Channel* channel);

  private:
    std::string _params;
    BotReply();
    BotReply(const BotReply& other);
    BotReply& operator=(const BotReply& other);
};

#endif