/**
 * @file Quit.hpp
 * @brief Implements IRC QUIT command
 * @version 0.1
 * @date 2025-10-16
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef QUIT_HPP
#define QUIT_HPP

#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <string>

class Client;
class Server;

/**
 * @class Quit
 * @brief Handler for IRC QUIT
 * @details This command is used to disconnect the client from server
 */
class Quit : public ICommand
{
  public:
    /**
     * @brief Construct a new Quit object
     *
     * @param params
     */
    explicit Quit(const std::string& params);
    /**
     * @brief Destroy the Quit object
     *
     */
    ~Quit();
    /**
     * @brief gracefully disconnect from server
     * @details broadcast a message to members of channels the sender belongs to, possibly with a custom message if sender
     * provided one
     * @param server
     * @param client sender
     */
    void execute(Server& server, Client& client);

  private:
    std::string _quitMsg;

    Quit();
    Quit(const Quit& other);

    Quit& operator=(const Quit& other);
};

#endif
