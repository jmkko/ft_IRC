#include "Who.hpp"

#include "LogManager.hpp"
#include "Parser.hpp"
#include "utils.hpp"

#include <string>

/************************************************************
 *		📁 CLASS METHODS									*
 ************************************************************/

ReplyCode Who::check_args(Server& server, Client& client, std::string& params)
{
    std::string        mask;
    std::string        op;
    std::istringstream iss(params);
    (void)server;
    (void)client;

    iss >> mask;
    iss >> op;

    if (!op.empty() && op != "o" && !iss.eof()) {
        LOG_CMD.warning(TO_STRING(ERR_NEEDMOREPARAMS) + " ERR_NEEDMOREPARAMS");
        return (ERR_NEEDMOREPARAMS);
    }
    return (CORRECT_FORMAT);
}

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

Who::Who(std::string& params)
{
    Parser parser;

    _mask = parser.format_parameter(params, NULL);
    _op   = parser.format_parameter(params, NULL);
}

Who::~Who() {}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

void Who::execute(Server& server, Client& client)
{
    // std::string        mask;
    // std::string        op;
    // std::istringstream iss(_params);
    // ReplyHandler&      rh = ReplyHandler::get_instance(&server);
    Parser p(server, client);

    // iss >> mask;
    // iss >> op;

    if (!_op.empty() && _op != "o") {
        LOG_CMD.warning(TO_STRING(ERR_NEEDMOREPARAMS) + " ERR_NEEDMOREPARAMS");
        p.response(ERR_NEEDMOREPARAMS);
        return;
    }

    if (_mask.empty())
        _mask = "*";
    if (Channel::is_valid_channel_name(_mask)) {
        std::map<std::string, Channel*>::iterator itChan = server.channels.begin();
        for (; itChan != server.channels.end(); itChan++) {
            if (Utils::is_matching_pattern(_mask, itChan->second->get_name())) {
                std::set<Client*>                 clients  = itChan->second->get_members();
                std::set<Client*>::const_iterator itClient = clients.begin();
                for (; itClient != clients.end(); itClient++) {
                    if (_op.empty() || (_op == "o" && itChan->second->is_operator(**itClient))) {
                        p.response(RPL_WHOREPLY,
                                   _who_msg(*itClient, itChan->second, server),
                                   std::string("0 ") + (*itClient)->get_real_name());
                    }
                }
                p.response(RPL_ENDOFWHO, itChan->second->get_name());
            }
        }
    } else {
        std::vector<Client*>           clients = server.find_clients_by_pattern(_mask);
        std::vector<Client*>::iterator it      = clients.begin();
        for (; it != clients.end(); it++) {
            p.response(RPL_WHOREPLY, _who_msg(*it, NULL, server), std::string("0 ") + (*it)->get_real_name());
        }
        p.response(RPL_ENDOFWHO, _mask);
    }
}

std::string Who::_who_msg(Client* client, Channel* channel, Server& server)
{
    std::string op  = "";
    std::string msg = "";
    if (channel) {
        msg.append(channel->get_name());
        op = std::string(channel->is_operator(*client) ? "@" : "");
    } else
        msg.append("*");
    msg.append(" " + client->get_user_name());
    msg.append(" localhost");
    msg.append(" " + server.get_name());
    msg.append(" " + client->get_nickname());
    msg.append(" H" + op);
    return (msg);
}

std::vector<Client*> Who::_find_all_clients_by_pattern(const std::set<Client*>& members, const std::string& pat)
{
    std::vector<Client*> result;
    for (std::set<Client*>::const_iterator it = members.begin(); it != members.end(); it++) {

        if (Utils::MatchPattern(pat)(*it)) {
            LOG_D_CMD("pattern " + pat + " matched", (*it)->get_nickname());
            result.push_back(*it);
        }
    }
    return result;
}
