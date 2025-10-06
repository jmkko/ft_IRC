#include "Who.hpp"

#include <string>

Who::Who() {}
Who::Who(const std::string& params) : _params(params) {}
Who::~Who() {}

Who::Who(const Who& mother) : ICommand() { (void)mother; }

Who& Who::operator=(const Who& other)
{
    (void)other;
    return *this;
}

/**
 * @brief Check if there is to much params for the command Who
 *
 * @param serve
 * @param client
 * @param params
 * @return replyCode
 */
ReplyCode Who::check_args(Server& server, Client& client, std::string& params)
{
    std::string        mask;
    std::string        op;
    std::istringstream iss(params);
    (void)server;
    (void)client;

    iss >> mask;
    iss >> op;

    if (!op.empty() && op != "o" && !iss.eof()) {
        LOG_CMD.error(TO_STRING(ERR_NEEDMOREPARAMS) + " ERR_NEEDMOREPARAMS");
        return (ERR_NEEDMOREPARAMS);
    }
    return (RPL_SUCCESS);
}

//: server 352 <me> <channel> <user> <host> <server> <nick> <flags> :<hopcount> <realname>
// :irc.example.com 352 user1 #chan1 bob bobhost irc.example.com bob H@ :0 Bob Realname
// :irc.example.com 352 user1 #chan1 alice alicehost irc.example.com alice H+ :0 Alice Realname
// :irc.example.com 315 user1 #chan1 :End of WHO list
// bob → ident
//
// bobhost → hostname (dérivé de l’IP de bob)
//
// irc.example.com → serveur local
//
// bob → nickname
//
// H@ → présent + opérateur de channel ou H* si operateur du SErver
//
// :0 → hopcount Dans un réseau mono-serveur (comme ton ft_irc), ça sera toujours 0
//
// Bob Realname → realname
void Who::execute(Server& server, Client& client)
{
    std::string        mask;
    std::string        op;
    std::istringstream iss(_params);
    ReplyHandler&      rh = ReplyHandler::get_instance(&server);

    iss >> mask;
    iss >> op;
    if (_params.empty())
        mask = "*";
    if (Channel::is_valid_channel_name(mask)) {
        std::map<std::string, Channel*>::iterator itChan = server.channels.begin();
        for (; itChan != server.channels.end(); itChan++) {
            if (utils::is_valid_pattern(mask, itChan->second->get_name())) {
                std::set<Client*>                 clients  = itChan->second->get_members();
                std::set<Client*>::const_iterator itClient = clients.begin();
                for (; itClient != clients.end(); itClient++) {
                    rh.process_response(client, RPL_WHOREPLY, _who_msg(*itClient, itChan->second, server));
                }
                rh.process_response(client, RPL_ENDOFWHO, itChan->second->get_name());
            }
        }
    } else {
        std::vector<Client*>           clients = server.find_clients_by_pattern(mask);
        std::vector<Client*>::iterator it      = clients.begin();
        for (; it != clients.end(); it++) {

            rh.process_response(client, RPL_WHOREPLY, _who_msg(*it, NULL, server));
        }
        rh.process_response(client, RPL_ENDOFWHO, mask);
    }
}

/**
 * @brief build RPL_WHOREPLY message
 *
 * @param client
 * @param channel
 * @param server
 * @return message
 * : server 352 <me> <channel> <user> <host> <server> <nick> <flags> :<hopcount> <realname>
 * :irc.example.com 352 user1 #chan1 bob bobhost irc.example.com bob H@ :0 Bob Realname
 * :irc.example.com 315 user1 #chan1 :End of WHO list
 */
std::string Who::_who_msg(Client* client, Channel* channel, Server& server)
{
    std::string op  = "";
    std::string msg = client->get_nickname();
    if (channel) {
        msg.append(" " + channel->get_name());
        op = std::string(channel->is_operator(*client) ? "@" : "");
    } else
        msg.append(" *");
    msg.append(" " + client->get_user_name());
    msg.append(" " + client->get_userhost());
    msg.append(" " + server.get_name());
    msg.append(" " + client->get_nickname());
    msg.append(" H" + op + " :0 ");
    msg.append(client->get_real_name());
    return (msg);
}

/**
 * @brief find the client mathcing the pattern of Who command
 *
 * @param members set of members in a channel
 * @param pat pattern to search
 * @return a vector of client corresponding to the pattern
 */
std::vector<Client*> Who::_find_all_clients_by_pattern(const std::set<Client*>& members, const std::string& pat)
{
    std::vector<Client*> result;
    for (std::set<Client*>::const_iterator it = members.begin(); it != members.end(); it++) {

        if (utils::MatchPattern(pat)(*it))
            result.push_back(*it);
    }
    return result;
}
