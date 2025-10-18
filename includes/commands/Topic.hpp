/**
 * @file Topic.hpp
 * @brief Implements IRC TOPIC command
 * @version 0.1
 * @date 2025-10-16
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TOPIC_HPP
#define TOPIC_HPP

#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <iostream>

class Channel;

/**
 * @class Topic
 * @brief Handler for IRC TOPIC
 * @details This command is used to send
 * - check topic
 * - update topic
 */
class Topic : public ICommand
{
  public:
    /**
     * @brief Construct a new Topic:: Topic object
     *
     * @param server
     * @param params
     */
    Topic(Server& server, std::string& params);

    /**
     * @brief Destroy the Topic:: Topic object
     *
     */
    ~Topic();

    /**
     * @brief proceeds to extra checks for op status (through Channel::set_topic) and to replies/transfers
     * @details
     * - if no topic arg returns RPL_TOPIC (or RPL_NOTOPIC if empty)
     * - if topic arg, tries updating channel topic
     * @param server
     * @param client sender
     * @pre Topic::check_args should have returned CORRECT_FORMAT
     */
    void execute(Server& server, Client& client);

    /**
     * @brief check syntaxic validity of args
     * @details should match [RFC specs](https://datatracker.ietf.org/doc/html/rfc2812#section-3.2.4)
     * - has at least one param (#Channel)
     * perform other validity checks
     * - #Channel does exist
     * - #Client does exist
     * @param server
     * @param client sender
     * @param params should match pattern `<channel> [ <topic> ]`
     * @return @ref ReplyCode corresponding to RFC ERR or CORRECT_FORMAT if syntax is correct
     * @warning can return ERR_NEEDMOREPARAMS or process directly replies ERR_NOSUCHCHANNEL ERR_NOTONCHANNEL
     */
    static ReplyCode check_args(Server& server, Client& client, std::string& params);

  private:
    std::string _topic;
    Channel*    _chan;
    bool        _isTopicChange;

    Topic(void);
    Topic(const Topic& other);
    Topic& operator=(const Topic& other);
};

#endif
