#include "Who.hpp"
#include "LogManager.hpp"
#include "utils.hpp"

#include <string>

/************************************************************
 *		📁 CLASS METHODS									*
 ************************************************************/

/**
 * @brief Check syntaxic validity of params
 * @details should match [ <mask> [ "o" ] ]
 * @param server
 * @param client
 * @param params
 * @return @ref ReplyCode
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
        LOG_CMD.warning(TO_STRING(ERR_NEEDMOREPARAMS) + " ERR_NEEDMOREPARAMS");
        return (ERR_NEEDMOREPARAMS);
    }
    return (CORRECT_FORMAT);
}

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

 /**
  * @brief Construct a new Who:: Who object
  * 
  * @param params 
  */
Who::Who(const std::string& params) : _params(params) {}

/**
 * @brief Destroy the Who:: Who object
 * 
 */
Who::~Who() {}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

/**
 * @brief sends replies RPL_WHOREPLY / RPL_ENDOFWHO for server members matching with params
 * @if no param
 * all members
 * @elseif mask
 * channel members if mask starts with a channel prefix (filtered by operator status if op == 'o')
 * members corresponding to mask
 * @endif
 * @details 
 * RPL_WHOREPLY is made for each matching user cf @ref _who_msg
 * @remark matching can be made using regex
 * @param server 
 * @param client 
 */
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
            if (utils::is_matching_pattern(mask, itChan->second->get_name())) {
                std::set<Client*>                 clients  = itChan->second->get_members();
                std::set<Client*>::const_iterator itClient = clients.begin();
                for (; itClient != clients.end(); itClient++) {
                    if (op.empty() || (op == "o" && itChan->second->is_operator(**itClient)))
                    {
                        rh.process_response(client,
                                            RPL_WHOREPLY,
                                            _who_msg(*itClient, itChan->second, server),
                                            NULL,
                                            std::string("0 ") + (*itClient)->get_real_name());
                    }
                }
                rh.process_response(client, RPL_ENDOFWHO, itChan->second->get_name());
            }
        }
    } else {
        std::vector<Client*>           clients = server.find_clients_by_pattern(mask);
        std::vector<Client*>::iterator it      = clients.begin();
        for (; it != clients.end(); it++) {
            rh.process_response(
                client, RPL_WHOREPLY, _who_msg(*it, NULL, server), NULL, std::string("0 ") + (*it)->get_real_name());
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
 * @details sent for each matching user
 * should folow pattern : server 352 <me> <channel> <user> <host> <server> <nick> <flags> :<hopcount> <realname>
 * example of valid reply sequence for WHO #chan1 having 2 members
 * :irc.example.com 352 user1 #chan1 bob bobhost irc.example.com bob H@ :0 Bob Realname
 * :irc.example.com 315 user1 #chan1 :End of WHO list
 * <client> <channel> <username> <host> <server> <nick> <flags> :<hopcount> <realname>
 * flags are composed of
 * - away status: the letter H ('H', 0x48) to indicate that the user is here, or the letter G ('G', 0x47) to indicate that
 * the user is gone. 
 * - optionally, a literal asterisk character ('*', 0x2A) to indicate that the user is a server operator.
 * trailing hopcounts being 0 (as it is a single server network)
 */
std::string Who::_who_msg(Client* client, Channel* channel, Server& server)
{
    std::string op  = "";
    std::string msg = "";
    if (channel) {
        msg.append(channel->get_name());
        op = std::string(channel->is_operator(*client) ? "@" : "");
    } else
        msg.append("*");
    msg.append(" " + client->get_user_name());
    msg.append(" localhost");
    msg.append(" " + server.get_name());
    msg.append(" " + client->get_nickname());
    msg.append(" H" + op);
    // msg.append(client->get_real_name());
    // LOG_DV_CMD(msg);
    return (msg);
}

/**
 * @brief find the client mathcing the pattern of Who command
 *
 * @param members set of members in a @ref Channel
 * @param pat pattern to search
 * @return a vector of @ref Client corresponding to the pattern
 */
std::vector<Client*> Who::_find_all_clients_by_pattern(const std::set<Client*>& members, const std::string& pat)
{
    std::vector<Client*> result;
    for (std::set<Client*>::const_iterator it = members.begin(); it != members.end(); it++) {

        if (utils::MatchPattern(pat)(*it))
        {
            LOG_D_CMD("pattern " + pat + " matched", (*it)->get_nickname());
            result.push_back(*it);
        }
    }
    return result;
}
