#include <sstream>

#include "Channel.hpp"
#include "Topic.hpp"
#include "Server.hpp"
#include "ReplyHandler.hpp"

// Default constructor
Topic::Topic(void) {}
Topic::Topic(Server& s, std::string& params) {
	build_args(s, params);
}

// Copy constructor
Topic::Topic(const Topic &other) {(void) other;}

// Assignment operator overload
Topic &Topic::operator=(const Topic &other) {(void) other;  return (*this);}

// Destructor
Topic::~Topic(void) {}

void Topic::execute(Server& s, Client& c) {

	ReplyHandler rh = ReplyHandler::get_instance(&s);

	if (_chan) {
		if (_topic.empty()) {
			std::string ChannelTopic = _chan->get_topic();
			if (ChannelTopic.empty()) {
				rh.process_response(c, RPL_NOTOPIC, _chan->get_name());
			}
			rh.process_response(c, RPL_TOPIC, _chan->get_name() + " :" + _chan->get_topic());
		} else {
			ReplyCode code = _chan->set_topic(c, _topic);
			if (code == RPL_SUCCESS) {
				_chan->broadcast(s, RPL_TOPIC, _chan->get_name() + " :" + _chan->get_topic());
			} else {
				rh.process_response(c, code, _chan->get_name());
			}
		}
	}
}

void	Topic::build_args(Server& s, std::string& params) {
	std::istringstream iss(params);
	std::string token;

	iss >> token;
	_chan = s.find_channel_by_name(token);
	iss >> token;
	if (token.empty()) {
		_topic = "";
	} else {
		_topic = token.substr(token.find(" :") + 2);
	}
}

ReplyCode Topic::check_args(Server& s, Client& c, std::string& params) {
	std::istringstream iss(params);
	std::string token;

	Channel* chan = NULL;

	iss >> token;
	if (token.empty()) {
		return (ERR_NEEDMOREPARAMS);
	}
	chan = s.find_channel_by_name(token);
	if (chan) {
		if (chan->is_member(c)) {
			return (RPL_SUCCESS);
		}
		return (ERR_NOTONCHANNEL);
	}
	return (ERR_NOSUCHCHANNEL);
}
