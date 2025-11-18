/**
 * @file Channel.hpp
 * @brief Channel class
 * @version 0.1
 * @date 2025-10-16
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "ReplyHandler.hpp"
#include "reply_codes.hpp"

#include <cstddef>
#include <queue>
#include <set>
#include <string>
#include <vector>

/**
 * @brief size of bitset used to display modes in binary form
 *
 */
#define MODE_LEN 4

struct CompareClientsByName;

class Client;
class Parser;

/**
 * @class Channel
 * @brief representing an IRC channel
 * @details responsability
 - managing modes related options `k` (key), `i` (invite only), `l` (member limit), `t` (restricted access to topic)
 - managing Clients into 3 groups : invites, members, operators
 * @remarks some checks have been implemented directly in the setters as they can be accessed through different classes
 - Channel::set_name
 - Channel::set_topic
 - Channel::add_member
 - Channel::make_operator
 - @todo check in Channel::set_key
 *
 */
class Channel
{

  public:
    /**
     * @brief Construct a new Channel object
     *
     * @param name
     * @param key
     */
    Channel(const std::string& name, const std::string& key = "");

    /**
     * @brief Construct a new Channel object
     *
     * @param other
     */
    Channel(const Channel& other);

    /**
     * @brief Destroy the Channel object
     *
     */
    virtual ~Channel();

    /**
     * @brief overload of assign operator
     *
     * @param other
     * @return Channel&
     */
    Channel& operator=(const Channel& other);

    /**
     * @brief Get the name object
     *
     * @return const std::string&
     */
    const std::string& get_name() const;

    /**
     * @brief Get the topic object
     *
     * @return const std::string&
     */
    const std::string& get_topic() const;

    /**
     * @brief Get the key object
     *
     * @return const std::string&
     */
    const std::string& get_key() const;

    /**
     * @brief
     *
     * @param client
     * @return true if client is channel member
     * @return false otherwise
     */
    bool is_member(Client& client) const;

    /**
     * @brief
     *
     * @param client
     * @return true if client is channel operator
     * @return false otherwise
     */
    bool is_operator(Client& client) const;

    /**
     * @brief
     *
     * @param client
     * @return true if client is in invite list
     * @return false otherwise
     */
    bool is_invited(Client& client) const;

    /**
     * @brief Get the user limit
     *
     * @return int
     * @remark return -1 if mode is `-l`
     */
    int get_user_limit() const;

    /**
     * @brief
     *
     * @return true if mode is `+i`
     * @return false otherwise
     */
    bool is_invite_only() const;

    /**
     * @brief
     *
     * @return true if mode is `+t`
     * @return false otherwise
     */
    bool is_topic_change_restricted() const;

    /**
     * @brief Set the channel name
     *
     * @param name
     * @return ReplyCode
     * @warning returns ERR_BADCHANMASK if invalid name
     */
    ReplyCode set_name(const std::string& name);

    /**
     * @brief Set the topic object
     *
     * @param client
     * @param topic
     * @return ReplyCode
     * @warning returns ERR_CHANOPRIVSNEEDED if mode +t and client is not operator
     */
    ReplyCode set_topic(Client& client, const std::string& topic);

    /**
     * @brief removes a client from the list of invited clients
     * @details generally occurs after the invited client joins channel
     *
     * @param client
     * @return true
     * @return false
     * @pre return from erase is ignored in case client is not invited, so it safe to use in that case.
     * Yet still better if the caller function has checked that client is invited
     */
    bool remove_from_invited_list(Client& client);

    /**
     * @brief Set the key object
     *
     * @param key
     * @return ReplyCode
     * @todo custom error for invalid key format
     */
    ReplyCode set_key(const std::string& key);

    /**
     * @brief Set the user limit object
     *
     * @param limit
     */
    void set_user_limit(int limit);

    /**
     * @brief add a client to the invite list
     *
     * @param client
     * @pre caller function should have checked that client is not already a member
     */
    void invite_client(Client& client);

    /**
     * @brief add a client to the member list
     *
     * @param client
     * @return ReplyCode
     * @warning returns ERR_CHANNELISFULL, ERR_INVITEONLYCHAN
     */
    ReplyCode add_member(Client& client);

    /**
     * @brief removes a client from the member list
     *
     * @param client
     * @return false if member was not in the channel
     * @pre return from erase is ignored in case client is not an member, so it safe to use in that case.
     * Yet still better if the caller function has checked that client is member
     */
    bool remove_member(Client& client);

    /**
     * @brief grants op status to the client
     *
     * @param client
     * @pre caller function should have checked that client is member
     * @return ReplyCode
     */
    ReplyCode make_operator(Client& client);

    /**
     * @brief removes op status from client
     *
     * @param client
     * @pre return from erase is ignored in case client is not an operator, so it safe to use in that case.
     * Yet still better if the caller function has checked that client is operator
     */
    void remove_operator(Client& client);

    /**
     * @brief activate a mode (among `k`, `i`, `l`, `t`)
     * @details use bitmasking
     * @remark some modes require other args
     * @param mode
     */
    void add_mode(unsigned short mode);

    /**
     * @brief uses #Parser to create a RPL_TOPIC / RPL_NOTOPIC reply
     *
     * @param p parser
     */
    void display_topic_to(Parser& p);

    /**
     * @brief deactivate a mode
     *
     * @param mode
     */
    void remove_mode(unsigned short mode);

    /**
     * @brief Get the mode as a short value
     *
     * @return unsigned short
     */
    unsigned short get_mode() const;

    /**
     * @brief Get channel members nicknames
     *
     * @return std::vector<std::string>
     */
    std::vector<std::string> get_members_list() const;

    /**
     * @brief Get members numbers
     *
     * @return size_t
     */
    size_t get_nb_members() const;

    /**
     * @brief Get the message history
     *
     * @return const std::queue<std::string>
     */
    const std::deque<std::string> get_history() const;

    /**
     * @brief adds a message to channel history
     * @details if message number reaches MAX_MESSAGES_HISTORY, the oldest one is removed
     * @param message
     */
    void add_message_to_history(const std::string& message);

    /**
     * @brief Get a list of #Client members
     *
     * @return std::set<Client*>
     */
    std::set<Client*> get_members() const;

    /**
     * @brief transfer a message to channel members
     * @remark doesn't transfer to sender
     * @see ReplyHandler::process_response
     * @param server
     * @param replyCode
     * @param params
     * @param sender
     * @param trailing
     */
    void broadcast(Server&            server,
                   ReplyCode          replyCode,
                   const std::string& params,
                   Client*            sender   = NULL,
                   const std::string& trailing = "") const;

    /**
     * @brief Get the channel modes in string format, ie '+ik key'
     *
     * @param client
     * @return std::string
     */
    std::string get_modes_str(Client& client);

    /**
     * @brief transfer a bot reply to channel members
     * @see ReplyHandler::process_response
     * @param server
     * @param replyCode
     * @param params
     * @param sender
     * @param trailing
     */
    void broadcast_bot(Server&            server,
                       ReplyCode          replyCode,
                       const std::string& params,
                       Client*            sender   = NULL,
                       const std::string& trailing = "") const;
    /**
     * @brief
     * @details cf [grammar](https://datatracker.ietf.org/doc/html/rfc2812#section-2.3.1)
     * ` channel    =  ( "#" / "+" / ( "!" channelid ) / "&" ) chanstring [ ":" chanstring ]`
     * @param name
     * @return true is name is valid according to RFC grammar
     * @return false otherwise
     */
    static bool is_valid_channel_name(const std::string& name);

    /**
 * @brief
 * @details cf [grammar](https://datatracker.ietf.org/doc/html/rfc2812#section-2.3.1)
 * `key        =  1*23( %x01-05 / %x07-08 / %x0C / %x0E-1F / %x21-7F )
              ; any 7-bit US_ASCII character,
              ; except NUL, CR, LF, FF, h/v TABs, and " "`
 * @param key
 * @return true is key is valid according to RFC grammar
 * @return false otherwise
 */
    static bool is_valid_channel_key(const std::string& key);

  private:
    std::string             _name;
    std::string             _topic;
    std::string             _key;
    unsigned short          _mode;
    int                     _userLimit; // -1 if -l not set
    std::set<Client*>       _members;
    std::set<Client*>       _invites;
    std::set<Client*>       _operators;
    std::set<Client*>       _banList;
    std::deque<std::string> _lastMessages;

    Channel();
};

/**
 * @brief overload of output operator displaying Channel details
 *
 * @param os
 * @param c
 * @return std::ostream&
 */
std::ostream& operator<<(std::ostream& os, const Channel& c);

#endif
