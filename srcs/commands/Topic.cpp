#include "Topic.hpp"

#include "Channel.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"

#include <sstream>

// Default constructor
Topic::Topic(void) : _topic(""), _chan(NULL) {}

Topic::Topic(Server& s, std::string& params)
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

    // 	std::string::size_type start = topic.find_first_not_of(" \t\n\r\f\v");
    // 	if (start != std::string::npos)
    // 		topic.erase(0, start);
    // 	else
    // 		topic.clear(); // string is all spaces
    // }
    // if (!topic.empty() && topic[0] == ':')
    // 	topic.erase(0, 1);
    _chan = s.find_channel_by_name(channel);
    _topic = topic;
}

// Copy constructor
Topic::Topic(const Topic& other) : _topic(other._topic), _chan(other._chan) {}

// Assignment operator overload
Topic& Topic::operator=(const Topic& other)
{
    if (this != &other) {
        _chan  = other._chan;
        _topic = other._topic;
    }
    return (*this);
}

// Destructor
Topic::~Topic(void) {}

void Topic::execute(Server& s, Client& c)
{

    ReplyHandler rh = ReplyHandler::get_instance(&s);

    if (_chan) {
        if (_topic.empty()) {
            std::string channelTopic = _chan->get_topic();
            if (channelTopic.empty()) {
                rh.process_response(c, RPL_NOTOPIC, _chan->get_name());
            } else {
                rh.process_response(c, RPL_TOPIC, _chan->get_name(), NULL, channelTopic);
            }
        } else {
            ReplyCode code = _chan->set_topic(c, _topic);
            if (code == CORRECT_FORMAT) {
                _chan->broadcast(s, TRANSFER_TOPIC, _chan->get_name(), &c, _topic);
                rh.process_response(c, TRANSFER_TOPIC, _chan->get_name(), &c, _topic);
            } else {
                rh.process_response(c, code, _chan->get_name());
            }
        }
    } else {
        LOG_w_CMD("invalid channel (should not happen)");
    }
}

ReplyCode Topic::check_args(Server& s, Client& c, std::string& params)
{
    std::istringstream iss(params);
    std::string        token;
    Channel*           chan = NULL;
    ReplyHandler rh = ReplyHandler::get_instance(&s);

    if (!(iss >> token))
        return ERR_NEEDMOREPARAMS;

    chan = s.find_channel_by_name(token);
    if (!chan) {
        rh.process_response(c, ERR_NOSUCHCHANNEL, token);
        return PROCESSED_ERROR;
    }

    if (!chan->is_member(c))
    {
        rh.process_response(c, ERR_NOTONCHANNEL, token);
        return PROCESSED_ERROR;
    }

    std::string next;
    if (!(iss >> next))
        return CORRECT_FORMAT;

    return CORRECT_FORMAT;
}
