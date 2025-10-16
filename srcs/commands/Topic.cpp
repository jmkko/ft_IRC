#include "Topic.hpp"

#include "Channel.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"

#include <sstream>

/************************************************************
 *		📁 CLASS METHODS									*
 ************************************************************/

ReplyCode Topic::check_args(Server& server, Client& client, std::string& params)
{
    std::istringstream iss(params);
    std::string        token;
    Channel*           chan = NULL;
    ReplyHandler       rh   = ReplyHandler::get_instance(&server);

    if (!(iss >> token))
        return ERR_NEEDMOREPARAMS;

    chan = server.find_channel_by_name(token);
    if (!chan) {
        rh.process_response(client, ERR_NOSUCHCHANNEL, token);
        return PROCESSED_ERROR;
    }

    if (!chan->is_member(client)) {
        rh.process_response(client, ERR_NOTONCHANNEL, token);
        return PROCESSED_ERROR;
    }

    std::string next;
    if (!(iss >> next))
        return CORRECT_FORMAT;

    return CORRECT_FORMAT;
}

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

Topic::Topic(Server& server, std::string& params)
{
    std::istringstream iss(params);
    std::string        channel;
    std::string        topic;
    std::string        token;

    iss >> channel;
    iss >> topic;
    while (iss >> token) {
        topic += " " + token;
    }
    _chan  = server.find_channel_by_name(channel);
    _topic = topic;
}

Topic::~Topic(void) {}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

void Topic::execute(Server& server, Client& client)
{

    ReplyHandler rh = ReplyHandler::get_instance(&server);

    if (_chan) {
        if (_topic.empty()) {
            std::string channelTopic = _chan->get_topic();
            if (channelTopic.empty()) {
                rh.process_response(client, RPL_NOTOPIC, _chan->get_name());
            } else {
                rh.process_response(client, RPL_TOPIC, _chan->get_name(), NULL, channelTopic);
            }
        } else {
            ReplyCode code = _chan->set_topic(client, _topic);
            if (code == CORRECT_FORMAT) {
                _chan->broadcast(server, TRANSFER_TOPIC, _chan->get_name(), &client, _topic);
                rh.process_response(client, TRANSFER_TOPIC, _chan->get_name(), &client, _topic);
            } else {
                rh.process_response(client, code, _chan->get_name());
            }
        }
    } else {
        LOG_w_CMD("invalid channel (should not happen)");
    }
}
