#include "Topic.hpp"

#include "Channel.hpp"
#include "Parser.hpp"
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

Topic::Topic(std::string& params) : _clearTopic(false)
{
    Parser parser;

    _chan = parser.format_parameter(params, NULL);
    std::istringstream iss(params);
    std::string        token;
    iss >> token;
    if (token == ":") {
        _clearTopic = true;
        _topic      = "";
    } else {
        _topic = parser.format_parameter(params, NULL);
    }
}

Topic::~Topic(void) {}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

void Topic::_display_topic(Parser& p, Channel& channel)
{
    if (channel.get_topic().empty()) {
        p.response(RPL_NOTOPIC, channel.get_name());
    } else {
        p.response(RPL_TOPIC, channel.get_name(), channel.get_topic());
    }
}

void Topic::execute(Server& server, Client& client)
{
    std::string topicTrailing = _topic;

    Parser p(server, client);

    if (_chan.empty()) {
        p.response(ERR_NEEDMOREPARAMS, "TOPIC");
        return;
    }
    Channel* channel = server.find_channel_by_name(_chan);
    if (!channel) {
        p.response(ERR_NOSUCHCHANNEL, _chan);
    } else if (!channel->is_member(client)) {
        p.response(ERR_NOTONCHANNEL, _chan);
    } else if (_topic.empty() && !_clearTopic) {
        _display_topic(p, *channel);
    } else {
        ReplyCode code = channel->set_topic(client, _topic);
        if (code == CORRECT_FORMAT) {
            if (_topic.empty()) {
                topicTrailing = " ";
            }
            channel->broadcast(server, TRANSFER_TOPIC, channel->get_name(), &client, topicTrailing);
            p.response(TRANSFER_TOPIC, channel->get_name(), topicTrailing);
        } else {
            p.response(code, channel->get_name());
        }
    }
}
