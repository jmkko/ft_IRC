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
#include "reply_codes.hpp"

#include <set>
#include <string>
#include <vector>

class Client;
class Server;

// static const std::string& authorizedModes   = "kilot";
// static const std::string& modesRequiringArg = "klo";
// static const std::string& authorizedOps     = "+-";
// static const std::string& digits            = "0123456789";

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
    Mode(const std::string& params);
    /**
     * @brief Destroy the Mode object
     *
     */
    ~Mode();

    /**
     * @brief check business validity of args before adjusting modes
     * @details
     * - supported modes are channel modes
        - `k` key
        - `i` invite-only
        - `l` member limit
        - `t` restrict usage of TOPIC to operators
        and user mode `o` : make operator
     * - supported operations are `+` and `-`
     * - modes requiring arguments are `k` (valid key), `l` (int from -1 to INT_MAX) abd `o` (valid username)
     * sends a RPL_CHANNELMODEIS in case it is successful - and RPL_YOUREOPER (for +o)
     * @remark parsing is less flexible than RFC suggests : if a mode requires a value, it should be provided after the modes at
     the corresponding index. In other words, `+kl key 10` is accepted but `+k key +l 10` is rejected
     * @remark If duplicate modes are present (as in `+kk key1 key2`) the last one prevails
     * @param server
     * @param client
     * @warning in case of error, can send ERR_NOSUCHCHANNEL, ERR_CHANOPRIVSNEEDED, ERR_KEYSET, ERR_NOSUCHNICK,
     ERR_USERNOTINCHANNEL
     */
    void execute(Server& server, Client& client);

    /**
 * @brief check syntaxic validity of args
 * @details cf. [RFC specs for User mode](https://datatracker.ietf.org/doc/html/rfc2812#section-3.1.5)
 and [RFC specs for Channel modes](https://datatracker.ietf.org/doc/html/rfc2812#section-3.2.3)
 * @param server not used
 * @param client not used
 * @param args should match pattern `<channel> *( ( "-" / "+" ) *<modes> *<modeparams>`
 * @return ReplyCode
 */
    static ReplyCode check_args(Server& server, Client& client, std::string& params);

  private:
    std::string _params;

    Mode();
    Mode(const Mode& other);
    Mode& operator=(const Mode& other);
};

#endif
