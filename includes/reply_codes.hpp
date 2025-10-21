#ifndef REPLY_CODES_HPP
#define REPLY_CODES_HPP

/// @brief https://datatracker.ietf.org/doc/html/rfc2812#section-5
enum ReplyCode {

    ////////////////////// NON RFC
    CORRECT_FORMAT = 0, // internal logic
    // RPL_NOTICE  = 101,
    // RPL_PASS    = 201,
    // RPL_USER    = 210,
    TRANSFER_INVITE  = 202,
    TRANSFER_JOIN    = 203,
    TRANSFER_NICK    = 204,
    TRANSFER_KICK    = 205,
    TRANSFER_PRIVMSG = 206,
    TRANSFER_MODE    = 207,
    TRANSFER_QUIT    = 208,
    MSG_PING         = 209,
    TRANSFER_TOPIC   = 210,
    TRANSFER_PART   = 211,
    PROCESSED_ERROR  = 806,

    // SERVER_ERR          = 801,
    // FORMAT_ERR          = 802,
    // PWD_ERR             = 803,

    //////////////////////  numerical errors
    CUSTOMERR_WRONG_FORMAT = 705,

    ////////////////////// RFC REPLIES (with code defined by RFC)
    RPL_NONE     = 300, // no answer
    RPL_WELCOME  = 001, // after registering (PASS NICK USER)
    RPL_YOURHOST = 002,
    RPL_CREATED  = 003,
    RPL_MYINFO   = 004,

    RPL_ISUPPORT = 005, // Supported features by server
    // RPL_BOUNCE             = 010,    // Obsolete
    // RPL_USERHOST           = 302,    // USERHOST
    // RPL_ISON               = 303,    // ISON
    // RPL_AWAY               = 301,    // AWAY
    // RPL_UNAWAY             = 305,    // AWAY
    // RPL_NOWAWAY            = 306,    // AWAY
    // RPL_WHOISUSER          = 311,    // WHOIS
    // RPL_WHOISSERVER        = 312,    // WHOIS
    // RPL_WHOISOPERATOR      = 313,    // WHOIS
    // RPL_WHOISIDLE          = 317,    // WHOIS
    // RPL_ENDOFWHOIS         = 318,    // WHOIS
    // RPL_WHOISCHANNELS      = 319,    // WHOIS
    // RPL_WHOWASUSER         = 314,    // WHOWAS
    // RPL_ENDOFWHOWAS        = 369,    // WHOWAS
    RPL_LISTSTART     = 321, // LIST
    RPL_LIST          = 322, // LIST
    RPL_LISTEND       = 323, // LIST
    RPL_CHANNELMODEIS = 324, // MODE
    RPL_NOTOPIC       = 331, // TOPIC (no topic)
    RPL_TOPIC         = 332, // TOPIC
    RPL_TOPICWHOTIME  = 333, // TOPIC (who and when
    RPL_INVITING      = 341, // INVITE
    // RPL_SUMMONING          = 342,    // SUMMON (obsolete)
    RPL_INVITELIST      = 346, // MODE +I
    RPL_ENDOFINVITELIST = 347, // MODE +I
    RPL_EXCEPTLIST      = 348, // MODE +e
    RPL_ENDOFEXCEPTLIST = 349, // MODE +e
    // RPL_VERSION            = 351,    // VERSION
    RPL_WHOREPLY   = 352, // WHO
    RPL_ENDOFWHO   = 315, // WHO
    RPL_NAMREPLY   = 353, // NAMES, WHO
    RPL_ENDOFNAMES = 366, // NAMES
    // RPL_LINKS              = 364,    // LINKS
    // RPL_ENDOFLINKS         = 365,    // LINKS
    RPL_BANLIST      = 367, // MODE +b
    RPL_ENDOFBANLIST = 368, // MODE +b
    RPL_INFO         = 371, // INFO
    RPL_ENDOFINFO    = 374, // INFO
    RPL_MOTDSTART    = 375, // MOTD
    RPL_MOTD         = 372, // MOTD
    RPL_ENDOFMOTD    = 376, // MOTD
    RPL_YOUREOPER    = 381, // OPER
    // RPL_REHASHING          = 382,    // REHASH
    RPL_YOURSERVICE = 383, // SERVICE (obsolte)
    RPL_TIME        = 391, // TIME
    // RPL_USERSSTART         = 392,    // USERS (obsolete)
    // RPL_USERS              = 393,    // USERS (obsolete)
    // RPL_ENDOFUSERS         = 394,    // USERS (obsolete)
    // RPL_NOUSERS            = 395,    // USERS (obsolete)
    // RPL_TRACELINK          = 200,    // TRACE
    // RPL_TRACECONNECTING    = 201,    // TRACE
    // RPL_TRACEHANDSHAKE     = 202,    // TRACE
    // RPL_TRACEUNKNOWN       = 203,    // TRACE
    // RPL_TRACEOPERATOR      = 204,    // TRACE
    // RPL_TRACEUSER          = 205,    // TRACE
    // RPL_TRACESERVER        = 206,    // TRACE
    // RPL_TRACESERVICE       = 207,    // TRACE
    // RPL_TRACENEWTYPE       = 208,    // TRACE
    // RPL_TRACECLASS         = 209,    // TRACE
    // RPL_TRACERECONNECT     = 210,    // TRACE
    // RPL_STATSLINKINFO      = 211,    // STATS
    // RPL_STATSCOMMANDS      = 212,    // STATS
    // RPL_STATSCLINE         = 213,    // STATS
    // RPL_STATSNLINE         = 214,    // STATS
    // RPL_STATSILINE         = 215,    // STATS
    // RPL_STATSKLINE         = 216,    // STATS
    // RPL_STATSQLINE         = 217,    // STATS
    // RPL_STATSYLINE         = 218,    // STATS
    // RPL_ENDOFSTATS         = 219,    // STATS
    // RPL_STATSVLINE         = 240,    // STATS
    // RPL_STATSLLINE         = 241,    // STATS
    // RPL_STATSUPTIME        = 242,    // STATS
    // RPL_STATSOLINE         = 243,    // STATS
    // RPL_STATSHLINE         = 244,    // STATS
    // RPL_UMODEIS            = 221,    // MODE (user modes)
    // RPL_SERVLIST           = 234,    // SQUERY
    // RPL_SERVLISTEND        = 235,    // SQUERY
    // RPL_LUSERCLIENT        = 251,    // LUSERS
    // RPL_LUSEROP            = 252,    // LUSERS
    // RPL_LUSERUNKNOWN       = 253,    // LUSERS
    // RPL_LUSERCHANNELS      = 254,    // LUSERS
    // RPL_LUSERME            = 255,    // LUSERS
    // RPL_ADMINME            = 256,    // ADMIN
    // RPL_ADMINLOC1          = 257,    // ADMIN
    // RPL_ADMINLOC2          = 258,    // ADMIN
    // RPL_ADMINEMAIL         = 259,    // ADMIN

    ////////////////////// RFC ERRORS
    ERR_NOSUCHNICK = 401, // NICK, PRIVMSG, WHOIS, etc.
    // ERR_NOSUCHSERVER      = 402,    // SQUERY, SQUIT
    ERR_NOSUCHCHANNEL    = 403, // JOIN, PRIVMSG, MODE, TOPIC, etc.
    ERR_CANNOTSENDTOCHAN = 404, // PRIVMSG
    ERR_TOOMANYCHANNELS  = 405, // JOIN
    // ERR_WASNOSUCHNICK     = 406,    // WHOIS
    ERR_TOOMANYTARGETS = 407, // PRIVMSG
    // ERR_NOSUCHSERVICE     = 408,    // SQUERY
    ERR_NOORIGIN       = 409, // PING, PONG
    ERR_NORECIPIENT    = 411, // PRIVMSG, NOTICE
    ERR_NOTEXTTOSEND   = 412, // PRIVMSG, NOTICE
    ERR_NOTOPLEVEL     = 413, // PRIVMSG, NOTICE
    ERR_WILDTOPLEVEL   = 414, // PRIVMSG, NOTICE
    ERR_UNKNOWNCOMMAND = 421,
    ERR_NOMOTD         = 422, // MOTD
    // ERR_NOADMININFO       = 423,    // ADMIN
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
    // ERR_SUMMONDISABLED    = 445,    // SUMMON
    // ERR_USERSDISABLED     = 446,    // USERS
    ERR_NOTREGISTERED    = 451, // All if not registered (NICK/USER)
    ERR_NEEDMOREPARAMS   = 461, // All
    ERR_ALREADYREGISTRED = 462, // NICK/USER (already registered)
    // ERR_NOPERMFORHOST     = 463,    // NICK (forbidden on host)
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
    // ERR_CANTKILLSERVER    = 483,    // KILL (impossible to kill a server)
    // ERR_NOOPERHOST        = 491,    // OPER (unauthorized host)
    ERR_UMODEUNKNOWNFLAG = 501 // MODE (unknown user mode)
};

#endif
