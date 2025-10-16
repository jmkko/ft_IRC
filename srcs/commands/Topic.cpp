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

/**
 * @brief check syntaxic validity of args
 * - has at least one param (#Channel)
 * perform other validity checks
 * - #Channel does exist
 * - #Client does exist
 * @param server
 * @param client
 * @param args should match pattern `<channel> [ <topic> ]`
 * @return @ref ReplyCode corresponding to RFC ERR or CORRECT_FORMAT if syntax is correct
 */
ReplyCode Topic::check_args(Server& s, Client& c, std::string& params)
{
    std::istringstream iss(params);
    std::string        token;
    Channel*           chan = NULL;
    ReplyHandler       rh   = ReplyHandler::get_instance(&s);

    if (!(iss >> token))
        return ERR_NEEDMOREPARAMS;

    chan = s.find_channel_by_name(token);
    if (!chan) {
        rh.process_response(c, ERR_NOSUCHCHANNEL, token);
        return PROCESSED_ERROR;
    }

    if (!chan->is_member(c)) {
        rh.process_response(c, ERR_NOTONCHANNEL, token);
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

/**
 * @brief Construct a new Topic:: Topic object
 *
 * @param s server
 * @param params
 */
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
    _chan  = s.find_channel_by_name(channel);
    _topic = topic;
}

/**
 * @brief Destroy the Topic:: Topic object
 *
 */
Topic::~Topic(void) {}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

/**
 * @brief proceeds to extra checks for op status (through Channel::set_topic) and to replies/transfers
 * @if no topic arg
 * returns RPL_TOPIC (or RPL_NOTOPIC if empty)
 * @elseif topic arg
 * tries updating channel topic
 * @endif
 * @param s #Server
 * @param c sender #Client
 */
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
