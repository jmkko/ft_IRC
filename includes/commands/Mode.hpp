/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhervoch <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 19:29:07 by jhervoch          #+#    #+#             */
/*   Updated: 2025/10/26 19:35:35 by jhervoch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

#include "Channel.hpp"
#include "Client.hpp"
#include "ICommand.hpp"

#include <queue>
#include <set>
#include <string>
#include <vector>

class Client;
class Server;

/**
 * @class Mode
 * @brief Handler for IRC MODE
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
     * @param params
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
    /**
     * @brief do simple check
     * - with no params
     * - with no matching chan
     * - with only channel params
     *
     * @param server
     * @param client
     * @param channel
     * @param p the parser
     * @return true if just this case , other false
     */
    bool _simple_args(Server& server, Client& client, Channel*& channel, Parser& p);
    /**
     * @brief use case of mode with flag which have no params
     * like + or - `i`,`t`
     * build mode is + mode args
     * we build the response with validNegativeModes and validNegativeModes
     *
     * @param channel
     * @param currentMode
     * @param validPositiveModes
     * @param validNegativeModes
     */
    void _mode_with_noparams(Channel* channel, std::string& currentMode, std::string& validModes);
    /**
     * @brief use case of +k mode
     * build mode is + mode args
     * we build the response with validNegativeModes and validNegativeModes
     * and validModesParams
     * check if key has no invalid char
     * on success add k mode and set the key
     *
     * @param channel
     * @param p
     * @param currentMode
     * @param validPositiveModes
     * @param validModesParams
     */
    void _mode_k(Channel*     channel,
                 Parser&      p,
                 std::string& currentMode,
                 std::string& currentParam,
                 std::string& validModes,
                 std::string& validModesParams);
    /**
     * @brief use case of +l mode
     * build mode is + mode args
     * we build the response with validNegativeModes and validNegativeModes
     * and validModesParams
     *
     * @param channel
     * @param p
     * @param currentMode
     * @param validPositiveModes
     * @param validModesParams
     */
    void _mode_l(Channel*     channel,
                 Parser&      p,
                 std::string& currentMode,
                 std::string& currentParam,
                 std::string& validModes,
                 std::string& validModesParams);
    /**
     * @brief use case of +o mode
     * we build the response with validNegativeModes and validNegativeModes
     * and validModesParams
     * on success add l mode and set the limit
     * chek if the user exist and is in channel
     *
     * @param server
     * @param channel
     * @param p
     * @param currentMode
     * @param currentParam
     * @param validPositiveModes
     * @param validNegativeModes
     * @param validModesParams
     */
    void                    _mode_o(Server&      server,
                                    Channel*     channel,
                                    Parser&      p,
                                    std::string& currentMode,
                                    std::string& currentParam,
                                    std::string& validModes,
                                    std::string& validModesParams);
    std::string             _channelName;
    std::queue<std::string> _modeQueue;
    std::queue<std::string> _paramsQueue;
    std::string             _params;

    Mode();
    Mode(const Mode& other);
    Mode& operator=(const Mode& other);
};

#endif
