/**
 * @file Motd.hpp
 * @brief Implements IRC MOTD command
 * @version 0.1
 * @date 2025-10-16
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef MOTD_HPP
#define MOTD_HPP

#include "Client.hpp"
#include "ICommand.hpp"
#include "Server.hpp"

#include <ctime>
#include <string>

class Client;
class Server;

/**
 * @class Motd
 * @brief Handler for IRC MOTD
 * @details This command is used to send a "Message of the Day", usually aimed at presenting server rules and welcoming users
 * after successfull registration
 */
class Motd : public ICommand
{
  public:
    /**
     * @brief Construct a new Motd object
     *
     * @param params
     */
    Motd(const std::string& params);
    /**
     * @brief Destroy the Motd object
     *
     */
    ~Motd();

    /**
     * @brief sends message of the day through RPL_MOTD / RPL_EMDMOTD
     * @details proceeds in 3 steps
     * - open and read the `motd.conf` file
     * - replace the var by her data
     * - send the message to the client
     * @param server
     * @param client
     * @warning can process reply ERR_NOMOTD if the file cannot be opened
     */
    void execute(Server& server, Client& client);

  private:
    /**
     * @brief find a word in a line and replace it by an other
     * @param str the raw line
     * @param find  the word to replace
     * @param replace the new word
     * @return a new string with the new word
     */
    std::string _str_replace(const std::string& str, const std::string& find, const std::string& replace);

    /**
     * @brief calculate and format the current date
     * @return date formatted as YYYY-MM-DD
     */
    std::string _get_current_time();
    std::string _params;
    Motd();
    Motd(const Motd& other);
    Motd& operator=(const Motd& other);
};

#endif
