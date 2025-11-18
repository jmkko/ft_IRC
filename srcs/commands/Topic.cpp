#include "Channel.hpp"
#include "LogManager.hpp"
#include "Parser.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "Topic.hpp"
#include "reply_codes.hpp"

#include <sstream>

/************************************************************
 *          🥚 CONSTRUCTORS & DESTRUCTOR                    *
 ************************************************************/

Topic::Topic(std::string& params) : _clearTopic(false)
{
    Parser parser;

    _chan                = parser.format_parameter(params, NULL);
    std::string topicArg = params;

    if (topicArg == " :") {
        _clearTopic = true;
        _topic      = "";
    } else if (!topicArg.empty()) {
        if (topicArg.length() > 1 && topicArg[1] == ':')
            _topic = topicArg.substr(3);
        else
            _topic = topicArg.substr(1);
    }
}

Topic::~Topic(void) {}

/*************************************************************
 *                   🛠️ FUNCTIONS                            *
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
