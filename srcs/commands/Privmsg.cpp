#include "Privmsg.hpp"

#include "Client.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

Privmsg::Privmsg(std::string& params)
{
    Parser parser;

    std::string targetList = parser.format_parameter(params, NULL);
    _targets               = parser.to_vector(targetList);
    _message               = parser.format_parameter(params, NULL);
}

Privmsg::~Privmsg(void) {}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

void Privmsg::execute(Server& server, Client& client)
{
    Parser p(server, client);
    Parser silent;

    if (_targets.size() == 0) {
        p.response(ERR_NEEDMOREPARAMS, "PRIVMSG");
        return;
    } else if (_message.empty()) {
        p.response(ERR_NOTEXTTOSEND);
        return;
    }
    int targetLimit = TARGET_LIMIT;
    for (std::vector<std::string>::iterator it = _targets.begin(); it != _targets.end(); it++) {
        if (targetLimit <= 0) {
            p.response(ERR_TOOMANYTARGETS, *it);
            break;
        }
        if (silent.correct_channel(*it)) {
            Channel* chan = server.find_channel_by_name(*it);
            if (chan && chan->is_member(client)) {
                chan->broadcast(server, TRANSFER_PRIVMSG, *it, &client, _message);
            } else if (chan) {
                p.response(ERR_NOTONCHANNEL, *it);
            } else {
                p.response(ERR_NOSUCHCHANNEL, *it);
            }
        } else {
            Client* user = server.find_client_by_nickname(*it);
            if (user) {
                p.response(user, &client, TRANSFER_PRIVMSG, *it, _message);
            } else {
                p.response(ERR_NOSUCHNICK, *it);
            }
        }
        targetLimit--;
    }
}
