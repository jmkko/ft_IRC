#include <sstream>

#include "Channel.hpp"
#include "Topic.hpp"
#include "Server.hpp"
#include "ReplyHandler.hpp"
#include "reply_codes.hpp"

// Default constructor
Topic::Topic(void): _topic(""), _chan(NULL) {}

Topic::Topic(Server& s, std::string& params) {
	std::istringstream iss(params);
	std::string channel;

	iss >> channel;

	std::string topic;
	std::getline(iss, topic);

	if (!topic.empty()) {
		std::string::size_type start = topic.find_first_not_of(" \t\n\r\f\v");
		if (start != std::string::npos)
			topic.erase(0, start);
		else
			topic.clear(); // string is all spaces
	}
	if (!topic.empty() && topic[0] == ':')
		topic.erase(0, 1);
	_chan = s.find_channel_by_name(channel);
	_topic = topic;

}

// Copy constructor
Topic::Topic(const Topic &other): _topic(other._topic), _chan(other._chan) {}

// Assignment operator overload
Topic &Topic::operator=(const Topic &other) {
	if (this != &other) {
		_chan = other._chan;
		_topic = other._topic;
	}
	return (*this);
}

// Destructor
Topic::~Topic(void) {}

void Topic::execute(Server& s, Client& c) {

	ReplyHandler rh = ReplyHandler::get_instance(&s);

	if (_chan) {
		if (_topic.empty()) {
			std::string channelTopic = _chan->get_topic();
			if (channelTopic.empty()) {
				rh.process_response(c, RPL_NOTOPIC, _chan->get_name());
			} else {
				rh.process_response(c, RPL_TOPIC, _chan->get_name() + " :" + _chan->get_topic());
			}
		} else {
			ReplyCode code = _chan->set_topic(c, _topic);
			if (code == CORRECT_FORMAT) {
                LOG_D_CMD("channel topic", _chan->get_topic());
				_chan->broadcast(s, RPL_TOPIC, _chan->get_name() + " :" + _chan->get_topic());
			} else {
				rh.process_response(c, code, _chan->get_name());
			}
		}
	} else {
		LOG_CMD.error("Topic::execute --> Channel is INVALID");
	}
}

ReplyCode Topic::check_args(Server& s, Client& c, std::string& params) {
	std::istringstream iss(params);
	std::string token;
	Channel* chan = NULL;

	if (!(iss >> token))
		return ERR_NEEDMOREPARAMS;

	chan = s.find_channel_by_name(token);
	if (!chan)
		return ERR_NOSUCHCHANNEL;

	if (!chan->is_member(c))
		return ERR_NOTONCHANNEL;

	std::string next;
	if (!(iss >> next))
		return CORRECT_FORMAT;

	// if (next[0] != ':') {
	// 	std::string extra;
	// 	if (iss >> extra)  // any more tokens = invalid
	// 		return ERR_TOOMANYPARAMS;
	// }

	return CORRECT_FORMAT;
}
