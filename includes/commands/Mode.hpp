/**
 * @file Mode.hpp
 * @brief Implements IRC MODE command
 * @version 0.1
 * @date 2025-10-16
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef MODE_HPP
#define MODE_HPP

#include "ICommand.hpp"

#include <queue>
#include <string>

class Client;
class Server;

/**
 * @class Mode
 * @brief Mode Handler for IRC MODE
 * @details This command is used
 * - to modify a few channel modes (kilt) and user mode (o) only among those specified by RFCs.
 * - to check a channel modes
 * @remark modifying requires channel operator privilege
 */
class Mode : public ICommand
{
  public:
    /**
     * @brief Construct a new Mode object
     *
     * @param args
     */
    Mode(std::string& params);
    /**
     * @brief Destroy the Mode object
     *
     */
    ~Mode();

    /**
     * @brief check business validity of args before adjusting modes
     * @details
     * - supported modes are channel modes :
     *  | MODE|  Description                       |
     *  |-----|------------------------------------|
     *  |`k`  | key                                |
     *  | `i` |invite-only                         |
     *  | `l` |member limit                        |
     *  | `t` |restrict usage of TOPIC to operators|
     * - and user mode `o` : make operator
     * - supported operations are `+` and `-`
     * - modes requiring arguments are `k` (valid key), `l` (int from -1 to INT_MAX) abd `o` (valid username)
     * we can use this command like this: MODE #chan +oo user1 user2 -it +kl pass 10
     * sends a RPL_CHANNELMODEIS in case it is successful - and RPL_YOUREOPER (for +o)
     * @param server
     * @param client
     * @warning in case of error, can send ERR_NOSUCHCHANNEL, ERR_CHANOPRIVSNEEDED, ERR_KEYSET, ERR_NOSUCHNICK,
     * ERR_USERNOTINCHANNEL
     */
    void execute(Server& server, Client& client);

  private:
    std::string             _channelName;
    std::queue<std::string> _modeQueue;
    std::queue<std::string> _paramsQueue;
    std::string             _params;

    Mode();
    Mode(const Mode& other);
    Mode& operator=(const Mode& other);
};

#endif
