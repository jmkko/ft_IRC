/**
 * @file reply_codes.hpp
 * @brief Numerical reply codes
 * @version 0.1
 * @date 2025-10-25
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef REPLY_CODES_HPP
#define REPLY_CODES_HPP

/**
 * @brief custom and RFC codes for server replies
 * @details [RFC specs](https://datatracker.ietf.org/doc/html/rfc2812#section-5)
 * @enum ReplyCode
 */
enum ReplyCode {

    ////////////////////// NON RFC
    TRANSFER_INVITE     = 202,
    TRANSFER_JOIN       = 203,
    TRANSFER_NICK       = 204,
    TRANSFER_KICK       = 205,
    TRANSFER_PRIVMSG    = 206,
    TRANSFER_MODE       = 207,
    TRANSFER_QUIT       = 208,
    MSG_PING            = 209,
    TRANSFER_TOPIC      = 210,
    TRANSFER_PART       = 211,
    TRANSFER_PROMPT_BOT = 118,
    TRANSFER_REPLY_BOT  = 218,
    //////////////////////  internal logic
    CORRECT_FORMAT  = 0,
    PROCESSED_ERROR = 806,
    //////////////////////  numerical custom replies
    CUSTOMRPL_BOT = 600,
    //////////////////////  numerical custom errors
    CUSTOMERR_WRONG_FORMAT = 705,
    CUSTOMERR_TOOMANYMODES = 706,

    ////////////////////// RFC REPLIES (with code defined by RFC)
    RPL_NONE            = 300, // no answer
    RPL_WELCOME         = 001, // after registering (PASS NICK USER)
    RPL_YOURHOST        = 002,
    RPL_CREATED         = 003,
    RPL_MYINFO          = 004,
    RPL_ISUPPORT        = 005, // Supported features by server
    RPL_LISTSTART       = 321, // LIST
    RPL_LIST            = 322, // LIST
    RPL_LISTEND         = 323, // LIST
    RPL_CHANNELMODEIS   = 324, // MODE
    RPL_NOTOPIC         = 331, // TOPIC (no topic)
    RPL_TOPIC           = 332, // TOPIC
    RPL_TOPICWHOTIME    = 333, // TOPIC (who and when
    RPL_INVITING        = 341, // INVITE
    RPL_INVITELIST      = 346, // MODE +I
    RPL_ENDOFINVITELIST = 347, // MODE +I
    RPL_EXCEPTLIST      = 348, // MODE +e
    RPL_ENDOFEXCEPTLIST = 349, // MODE +e
    RPL_WHOREPLY        = 352, // WHO
    RPL_ENDOFWHO        = 315, // WHO
    RPL_NAMREPLY        = 353, // NAMES, WHO
    RPL_ENDOFNAMES      = 366, // NAMES
    RPL_BANLIST         = 367, // MODE +b
    RPL_ENDOFBANLIST    = 368, // MODE +b
    RPL_INFO            = 371, // INFO
    RPL_ENDOFINFO       = 374, // INFO
    RPL_MOTDSTART       = 375, // MOTD
    RPL_MOTD            = 372, // MOTD
    RPL_ENDOFMOTD       = 376, // MOTD
    RPL_YOUREOPER       = 381, // OPER
    RPL_YOURSERVICE     = 383, // SERVICE (obsolte)
    RPL_TIME            = 391, // TIME

    ////////////////////// RFC ERRORS
    ERR_NOSUCHNICK       = 401, // NICK, PRIVMSG, WHOIS, etc.
    ERR_NOSUCHCHANNEL    = 403, // JOIN, PRIVMSG, MODE, TOPIC, etc.
    ERR_CANNOTSENDTOCHAN = 404, // PRIVMSG
    ERR_TOOMANYCHANNELS  = 405, // JOIN
    ERR_TOOMANYTARGETS   = 407, // PRIVMSG
    ERR_NOORIGIN         = 409, // PING, PONG
    ERR_NORECIPIENT      = 411, // PRIVMSG, NOTICE
    ERR_NOTEXTTOSEND     = 412, // PRIVMSG, NOTICE
    ERR_NOTOPLEVEL       = 413, // PRIVMSG, NOTICE
    ERR_WILDTOPLEVEL     = 414, // PRIVMSG, NOTICE
    ERR_UNKNOWNCOMMAND   = 421,
    ERR_NOMOTD           = 422, // MOTD
    ERR_FILEERROR        = 424, // All if file processing
    ERR_NONICKNAMEGIVEN  = 431, // NICK
    ERR_ERRONEUSNICKNAME = 432, // NICK
    ERR_NICKNAMEINUSE    = 433, // NICK
    ERR_NICKCOLLISION    = 436, // NICK
    ERR_UNAVAILRESOURCE  = 437, // NICK, CHANNEL
    ERR_USERNOTINCHANNEL = 441, // KICK, INVITE
    ERR_NOTONCHANNEL     = 442, // PRIVMSG, MODE, etc.
    ERR_USERONCHANNEL    = 443, // INVITE
    ERR_NOLOGIN          = 444, // USER
    ERR_NOTREGISTERED    = 451, // All if not registered (NICK/USER)
    ERR_NEEDMOREPARAMS   = 461, // All
    ERR_ALREADYREGISTRED = 462, // NICK/USER (already registered)
    ERR_PASSWDMISMATCH   = 464, // PASS
    ERR_YOUREBANNEDCREEP = 465, // after BAN
    ERR_KEYSET           = 467, // JOIN (if +k already set)
    ERR_CHANNELISFULL    = 471, // JOIN (if +l)
    ERR_UNKNOWNMODE      = 472, // MODE (unknown mode)
    ERR_INVITEONLYCHAN   = 473, // JOIN (if +i)
    ERR_BANNEDFROMCHAN   = 474, // JOIN (if +b)
    ERR_BADCHANNELKEY    = 475, // JOIN (if +k)
    ERR_BADCHANMASK      = 476, // JOIN
    ERR_NOPRIVILEGES     = 481, // MODE, KICK, etc.
    ERR_CHANOPRIVSNEEDED = 482, // MODE, KICK, etc.
    ERR_UMODEUNKNOWNFLAG = 501  // MODE (unknown user mode)
};

#endif
