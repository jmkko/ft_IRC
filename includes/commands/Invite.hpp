/**
 * @file Invite.hpp
 * @brief Implements IRC INVITE command
 * @version 0.1
 * @date 2025-10-16
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef INVITE_HPP
#define INVITE_HPP

#include "Channel.hpp"
#include "Client.hpp"
#include "ICommand.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"

/**
 * @class Invite
 * @brief Handler for IRC INVITE
 * @details This command is used to invite a user to an invite-only channel
 */
class Invite : public ICommand
{

  public:
    /**
     * @brief Construct a new Invite object
     *
     * @param params
     */
    Invite(const std::string& params);
    /**
     * @brief Destroy the Invite object
     *
     */
    ~Invite();

    /**
     * @brief invite a client to a channel
     * check if:
     * - the client does exist, the channel does exist
     * - the sender is on channel
     * - the client is already on this channel
     * - the sender is operator if the channel is invite-only
     * then if successful sends RPL_INVITING and broadcast the message to channel members
     * @param s Server
     * @param c Client = the sender
     * @warning in case of error send ERR_NOSUCHNICK, ERR_NOSUCHCHANNEL, ERR_NOTONCHANNEL, ERR_USERONCHANNEL, ERR_CHANOPRIVSNEEDED
     */
    void execute(Server& server, Client& client);
    /**
     * @brief check if Invite arguments are syntaxically valid.
     * @details cf. [RFC specs](https://datatracker.ietf.org/doc/html/rfc2812#section-3.2.7) require
     * - one #Client to be invited
     * - one #Channel to be invited in
     * @param server
     * @param client sender
     * @param params should match `<nickname> <channel>`
     * @return #ReplyCode
     */
    static ReplyCode check_args(Server& server, Client& client, std::string& params);

  private:
	std::string _nickname;
	std::string _channelName;
    Invite();
    Invite(const Invite& other);
    Invite& operator=(const Invite& other);
};

#endif
