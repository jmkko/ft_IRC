/**
 * @file Part.hpp
 * @brief Implements IRC PART command
 * @version 0.1
 * @date 2025-10-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef PART_HPP
#define PART_HPP
#include "ICommand.hpp"

#include <string>

class Server;
class Client;

/**
 * @class Part
 * @brief Handler for IRC PART
 * @details this command allows a user to leave a channel he/she has previously joined
 */
class Part : public ICommand
{
  public:
    /**
     * @brief Construct a new Part object
     * @details uses #Parser
     * @param params
     */
    Part(std::string& params);
    /**
     * @brief Destroy the Part object
     *
     */
    ~Part();

    /**
     * @brief checks args validity and removes client from channel
     * @details
     * syntax should match `<channel> *( "," <channel> ) [ <Part Message> ]`
     * cf. [RFC specs](https://datatracker.ietf.org/doc/html/rfc2812#section-3.2.2)
     * @param server
     * @param client
     * @warning can send ERR_NEEDMOREPARAMS, ERR_NOSUCHCHANNEL, ERR_NOTONCHANNEL
     */
    void execute(Server& server, Client& client);

  private:
    std::vector<std::string> _chanNames;
    std::string              _message;

    Part(void);
    Part(const Part& other);
    Part& operator=(const Part& other);
};

#endif
