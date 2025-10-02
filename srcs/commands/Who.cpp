#include "Who.hpp"

#include <string>

Who::Who() {}
Who::Who(const std::string& params) : _params(params) {}
Who::~Who() {}

Who::Who(const Who& mother) : ICommand() { (void)mother; }

Who& Who::operator=(const Who& other)
{
    (void)other;
    return *this;
}

ReplyCode Who::check_args(Server& server, Client& client, std::string& params)
{
    std::string        mask;
    std::string        op;
    std::istringstream iss(params);
    (void)server;
    (void)client;

    iss >> mask;
    iss >> op;

    if (op != "o" || !iss.eof()) {
        LOG_CMD.error(TO_STRING(ERR_NEEDMOREPARAMS) + " ERR_NEEDMOREPARAMS");
        return (ERR_NEEDMOREPARAMS);
    }
    return (RPL_SUCCESS);
}
//:server 352 <me> <channel> <user> <host> <server> <nick> <flags> :<hopcount> <realname>
// :irc.example.com 352 user1 #chan1 bob bobhost irc.example.com bob H@ :0 Bob Realname
// :irc.example.com 352 user1 #chan1 alice alicehost irc.example.com alice H+ :0 Alice Realname
// :irc.example.com 315 user1 #chan1 :End of WHO list
// bob → ident
//
// bobhost → hostname (dérivé de l’IP de bob)
//
// irc.example.com → serveur local
//
// bob → nickname
//
// H@ → présent + opérateur de channel ou H* si operateur du SErver
//
// :0 → hopcount Dans un réseau mono-serveur (comme ton ft_irc), ça sera toujours 0
//
// Bob Realname → realname
void Who::execute(Server& server, Client& client){
    std::string        mask;
    std::string        op;
    std::istringstream iss(_params);
    (void)server;
    (void)client;

    iss >> mask;
    iss >> op;

    if (Channel::is_valid_channel_name(mask)){
        std::map<std::string, Channel*>::iterator result = server.channels.find(mask);
        if (result != server.channels.end()){


        }
    }

}

/**
 * @brief test if the string matching a wildcard with * pattern
 *
 * @param pattern string like *abc or abc* or *abc**df** etc..
 * @param str string to test
 * @return true if the string match the wildcard pattern
 *
 * key point
 * wildcard pattern begin with *
 * wildcard pattern end with *
 * multiple * wildcard pattern
 */
bool Who::is_valid_pattern(const std::string& pattern, const std::string& str)
{
    size_t p = 0, s = 0, star = std::string::npos, match = 0;

    while (s < str.size()) {
        if (p < pattern.size() && (pattern[p] == '?' || pattern[p] == str[s])) {
            // match direct ou ?
            p++;
            s++;
        } else if (p < pattern.size() && pattern[p] == '*') {
            // on note la position du *
            star = p++;
            match = s;
        } else if (star != std::string::npos) {
            // pas de match direct : on revient au dernier char apres *
            p = star + 1;
            s = ++match;
        } else {
            return false;
        }
    }
    while (p < pattern.size() && pattern[p] == '*')
        p++;
    return (p == pattern.size());
}
