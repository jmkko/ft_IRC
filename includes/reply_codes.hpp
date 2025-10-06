#ifndef REPLY_CODES_HPP
#define REPLY_CODES_HPP

// IRC Response Code Defines

// =============================================================================
// USER Command Responses
// =============================================================================

// Success Responses
#define RPL_WELCOME_MSG  " :Welcome to the IRC HazADoU& SerVerRrrr" // <nick>!<user>@<host>
#define RPL_YOURHOST_MSG " :Your host is"                           // <servername>, running version <ver>
#define RPL_CREATED_MSG  " :This server was created"                // <date>
#define RPL_MYINFO_MSG   "" // <servername> <version> <available user modes> <available channel modes>

// Error Responses
#define ERR_NEEDMOREPARAMS_MSG    " :Not enough parameters "
#define ERR_ALREADYREGISTERED_MSG " :Unauthorized command (already registered)"
#define ERR_UNKNOWNCOMMAND_MSG    " :Command is not handled (sorry guys) "
#define ERR_NOTEXTTOSEND_MSG " :NO text to send"
// =============================================================================
// PASS Command Responses
// =============================================================================

// Success: No response - Password accepted silently

// Error Responses
// ERR_NEEDMOREPARAMS_MSG (reused)
// ERR_ALREADYREGISTRED_MSG (reused)
#define ERR_PASSWDMISMATCH_MSG " :Password incorrect"

// =============================================================================
// NICK Command Responses
// =============================================================================

// Success: No response - Nick change accepted
// Success: RPL_WELCOME series (001-004) if this completes registration

// Error Responses
#define ERR_NONICKNAMEGIVEN_MSG  " :No nickname given"
#define ERR_ERRONEUSNICKNAME_MSG " :Erroneous nickname "                     // <nick>
#define ERR_NICKNAMEINUSE_MSG    " :Nickname is already in use (bad choice)" // <nick>
#define ERR_NICKCOLLISION_MSG    " :Nickname collision KILL from "           // <nick>, <user>@<host>
#define ERR_UNAVAILRESOURCE_MSG  " :Nick/channel is temporarily unavailable" // <nick/channel>
#define ERR_RESTRICTED_MSG       " :Your connection is restricted!"

// =============================================================================
// JOIN Command Responses
// =============================================================================

// Success Responses
// JOIN message: :<nick>!<user>@<host> JOIN <channel>
#define RPL_TOPIC_MSG        ""                    // <channel> :topic
#define RPL_NOTOPIC_MSG      " :No topic is set"   // <channel> :topic
#define RPL_TOPICWHOTIME_MSG ""                    // <channel> <nick> <time>
#define RPL_NAMREPLY_MSG     ""                    // <channel> :nick1 nick2 nick3...
#define RPL_ENDOFNAMES_MSG   " :End of NAMES list" // <channel>

// Error Responses
#define ERR_NOSUCHCHANNEL_MSG    " :No such channel"                   // <channel>
#define ERR_CANNOTSENDTOCHAN_MSG " :Cannot send to channel"            // <channel>
#define ERR_TOOMANYCHANNELS_MSG  " :You have joined too many channels" // <channel>
#define ERR_TOOMANYTARGETS_MSG   " recipients."                        // <target> :<error code>, <abort message>
#define ERR_CHANNELISFULL_MSG    " :Cannot join channel (+l)"          // <channel>
#define ERR_INVITEONLYCHAN_MSG   " :Cannot join channel (+i)"          // <channel>
#define ERR_BANNEDFROMCHAN_MSG   " :Cannot join channel (+b)"          // <channel>
#define ERR_BADCHANNELKEY_MSG    " :Cannot join channel (+k)"          // <channel>
#define ERR_BADCHANMASK_MSG      " :Bad Channel Mask"                  // <channel>
#define ERR_NOCHANMODES_MSG      " :Channel doesn't support modes"     // <channel>
#define ERR_NOTONCHANNEL_MSG     " :You're not on that channel"        // <channel>
#define ERR_CHANOPRIVSNEEDED_MSG " :You're not channel operator"       // <channel>

// Unused yet
#define ERR_USERNOTINCHANNEL_MSG " :They aren't on that channel"

// =============================================================================
// INVITE Command Responses
// =============================================================================

// Success Responses
#define RPL_INVITING_MSG "" // <channel> <nick>
// INVITE message to target: :<nick>!<user>@<host> INVITE <target> <channel>

// Error Responses
#define ERR_NOSUCHNICK_MSG " :No such nick/channel" // <nickname>
// ERR_NOSUCHCHANNEL_MSG (reused)                                              // <channel>
#define ERR_USERONCHANNEL_MSG " :is already on channel" // <user> <channel>

// =============================================================================
// QUIT Command Responses
// =============================================================================

// Success Responses
// QUIT message: :<nick>!<user>@<host> QUIT :reason (broadcast to channels)
// Connection closes - No numeric response to quitting client
#define ERROR_CLOSING_LINK_MSG " :Closing Link:" // <host> (reason)

// Error Responses: Generally no error responses - QUIT is almost always successful

// =============================================================================
// PART Command Responses
// =============================================================================

// Success Responses
// PART message: :<nick>!<user>@<host> PART <channel> :reason

// Error Responses
// ERR_NOSUCHCHANNEL_MSG (reused)                                              // <channel>
// ERR_NOTONCHANNEL_MSG (reused)                                               // <channel>

// =============================================================================
// MODE Command Responses
// =============================================================================

// Success Responses (User Mode)
#define RPL_UMODEIS_MSG "" // <user mode string>
// MODE message: :<nick>!<user>@<host> MODE <nick> <mode changes>

// Success Responses (Channel Mode)
#define RPL_CHANNELMODEIS_MSG "" // <channel> <mode> <mode params>
#define RPL_CREATIONTIME_MSG  "" // <channel> <creation time>
// MODE message: :<nick>!<user>@<host> MODE <channel> <mode changes> <params>

// Error Responses
// ERR_NOSUCHNICK_MSG (reused)                                                 // <nickname>
// ERR_NOSUCHCHANNEL_MSG (reused)                                              // <channel>
// ERR_NOTONCHANNEL_MSG (reused)                                               // <channel>
// ERR_NEEDMOREPARAMS_MSG (reused)
#define ERR_KEYSET_MSG       " :Channel key already set"        // <channel>
#define ERR_UNKNOWNMODE_MSG  " :is unknown mode char to me for" // <char>, <channel>
#define ERR_NOPRIVILEGES_MSG " :Permission Denied- You're not an IRC operator"
// ERR_CHANOPRIVSNEEDED_MSG (reused)                                           // <channel>
#define ERR_UMODEUNKNOWNFLAG_MSG " :Unknown MODE flag"
#define ERR_USERSDONTMATCH_MSG   " :Cannot change mode for other users"

// =============================================================================
// KICK Command Responses
// =============================================================================

// =============================================================================
// QUIT Command Responses
// =============================================================================

#define QUIT_TRAILING_MSG " :Leaving"

// =============================================================================
// OPER Command Responses
// =============================================================================

// Success Responses
#define RPL_YOUREOPER_MSG " :You are now an IRC operator"
// MODE message: :<nick>!<user>@<host> MODE <nick> +o (operator mode set)

// Error Responses
// ERR_NEEDMOREPARAMS_MSG (reused)
// ERR_PASSWDMISMATCH_MSG (reused)
#define ERR_NOOPERHOST_MSG " :No O-lines for your host"

#define RPL_ENDOFWHO_MSG " :End of WHO list"
/// @brief https://datatracker.ietf.org/doc/html/rfc2812#section-5
enum ReplyCode {

    // general success code
    RPL_SUCCESS = 0,

    // post registration
    RPL_WELCOME  = 001,
    RPL_YOURHOST = 002,
    RPL_CREATED  = 003,
    RPL_MYINFO   = 004,

    // notification
    RPL_NOTICE = 101,

    // Command (not in RFC)
    RPL_PASS    = 201,
    RPL_USER    = 202,
    RPL_JOIN    = 203,
    RPL_NICK    = 204,
    RPL_KICK    = 205,
    RPL_PRIVMSG = 206,
    RPL_MODE    = 207,
    RPL_QUIT    = 208,

    // channel info
    RPL_NONE       = 300, // used to mark no answer
    RPL_ENDOFWHO   = 315,
    RPL_NOTOPIC    = 331, // TOPIC
    RPL_TOPIC      = 332, // TOPIC
    RPL_INVITING   = 341, // INVITE
    RPL_WHOREPLY   = 352, //
    RPL_NAMREPLY   = 353, // JOIN
    RPL_ENDOFNAMES = 366, // JOIN
    RPL_YOUREOPER  = 381, // OPER
    RPL_TIME       = 391, // TIME

    // errors
    ERR_NOSUCHNICK        = 401,
    ERR_NOSUCHCHANNEL     = 403,
    ERR_CANNOTSENDTOCHAN  = 404, // PRIVMSG
    ERR_TOOMANYCHANNELS   = 405,
    ERR_TOOMANYTARGETS    = 407, // PRIVMSG
    ERR_NOORIGIN          = 409, // PING, PONG
    ERR_NORECIPIENT       = 411,
    ERR_NOTEXTTOSEND      = 412, // PRIVMSG
    ERR_NOTOPLEVEL        = 413, // PRIVMSG
    ERR_WILDTOPLEVEL      = 414, // PRIVMSG
    ERR_UNKNOWNCOMMAND    = 421,
    ERR_FILEERROR         = 424, // BONUS
    ERR_NONICKNAMEGIVEN   = 431, // NICK
    ERR_ERRONEUSNICKNAME  = 432, // NICK
    ERR_NICKNAMEINUSE     = 433, // NICK
    ERR_USERNOTINCHANNEL  = 441,
    ERR_NOTONCHANNEL      = 442, // KICK
    ERR_USERONCHANNEL     = 443, // INVITE
    ERR_NOTREGISTERED     = 451,
    ERR_NEEDMOREPARAMS    = 461,
    ERR_ALREADYREGISTERED = 462, // PASS
    ERR_PASSWDMISMATCH    = 464, // PASS
    ERR_CHANNELISFULL     = 471,
    ERR_UNKNOWNMODE       = 472, // MODE
    ERR_INVITEONLYCHAN    = 473,
    ERR_CHANOPRIVSNEEDED  = 482,
    ERR_UMODEUNKNOWNFLAG  = 501, // MODE
    ERR_BANNEDFROMCHAN    = 474,
    ERR_BADCHANMASK       = 476,

    // custom
    SERVER_ERR = 801,
    FORMAT_ERR = 802,
    PWD_ERR    = 803,

};

#endif
