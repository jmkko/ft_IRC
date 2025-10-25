/**
 * @file Pass.hpp
 * @brief Implements IRC PASS command
 * @version 0.1
 * @date 2025-10-17
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef PASS_HPP
#define PASS_HPP

#include "Channel.hpp"
#include "ICommand.hpp"

#include <string>

class Server;
class Client;

/**
 * @class Pass
 * @brief Handler for IRC PASS
 */
class Pass : public ICommand
{
  public:
    /**
     * @brief Construct a new Pass object
     *
     */
    Pass(void);

    Pass(std::string& params);
    /**
     * @brief Destroy the Pass object
     *
     */
    ~Pass();
    /**
     * @brief updates client status to #AUTHENTICATED
     * @details perform syntaxic check
     * - one password is given
     *
     * performs other checks
     * - client is not already REGISTERED
     * - password is correct
     *
     * @param server
     * @param client
     * @warning can return ERR_NEEDMOREPARAMS, ERR_ALREADYREGISTRED, ERR_PASSWDMISMATCH
     */
    void execute(Server& server, Client& client);

  private:
    std::string _pass;

    Pass(const Pass& other);
    Pass& operator=(const Pass& other);
};

#endif
