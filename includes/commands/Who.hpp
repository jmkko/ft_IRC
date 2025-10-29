/**
 * @file Who.hpp
 * @brief Implements IRC WHO command
 * @version 0.1
 * @date 2025-10-16
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef WHO_HPP
#define WHO_HPP

#include "Channel.hpp"
#include "Client.hpp"
#include "ICommand.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"

#include <string>
#include <vector>

class Client;
class Server;

/**
 * @class Who
 * @brief Handler for IRC WHO query
 * @details This command is used to get list of users matching
 * - query mask (can apply to users and channels)
 * - option `o` to list only operators
 */
class Who : public ICommand
{
  public:
    /**
     * @brief Construct a new Who:: Who object
     *
     * @param params
     */
    Who(std::string& params);

    /**
     * @brief Destroy the Who:: Who object
     *
     */
    ~Who();

    /**
    * @brief sends replies RPL_WHOREPLY / RPL_ENDOFWHO for server members matching with params
    * @details
    - if no param, return all members
    - if mask is provided
        - if mask corresponds to a channel name, return matching channel members (filtered by operator status if op == 'o')
        - else return matching #Client
    *
    * a RPL_WHOREPLY is sent for each matching user
    @see _who_msg
    * @remark matching can be made using regex
    * @param server
    * @param client
    * @pre Who::check_args should have returned CORRECT_FORMAT

    */
    void execute(Server& server, Client& client);

  private:
    std::string _mask;
    std::string _op;
    Who();
    Who(const Who& other);
    Who& operator=(const Who& other);

    /**
     * @brief find the client mathcing the pattern of Who command
     *
     * @param members set of members in a @ref Channel
     * @param pat pattern to search
     * @return a vector of @ref Client corresponding to the pattern
     */
    std::vector<Client*> _find_all_clients_by_pattern(const std::set<Client*>& members, const std::string& pat);

    /**
    * @brief build RPL_WHOREPLY message
    *
    * @param client
    * @param channel
    * @param server
    * @return message
    * @details sent for each matching user
    * should folow pattern : server 352 <me> <channel> <user> <host> <server> <nick> <flags> :<hopcount> <realname>
    * - flags are composed of
    *  - away status: the letter `H` ('H', 0x48) to indicate that the user is here, or the letter `G` ('G', 0x47) to indicate that
    the user is gone.
    *  - optionally, `@` to indicate that the user is a server operator.
    * - trailing : hopcounts value will always be 0 (as it is a single server network)
    *
    * @section ExampleWho Example of valid reply sequence for WHO #chan1 having 2 members
    * @code{.unparsed}
        :irc.example.com 352 user1 #chan1 bob bobhost irc.example.com bob H@ :0 Bob Realname
        :irc.example.com 315 user1 #chan1 :End of WHO list
    * @endcode
    */
    std::string _who_msg(Client* client, Channel* channel, Server& server);
};

#endif
