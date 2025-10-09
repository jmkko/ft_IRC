#include "AssertFail.hpp"
#include "AssertReply.hpp"
#include "LogManager.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"
#include "Config.hpp"

#include <algorithm>
#include <sstream>
#include <vector>

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

/// @brief parses a reply string into tokens :
/// prefix (server name when sent by the server)
/// target : numeric code (or command name)
/// arguments
/// trailing message
/// @param reply
AssertReply::AssertReply(const std::string& reply) : _reply(reply), _messages() { _process_reply(); }

AssertReply::~AssertReply(void) {}

/************************************************************
 *		➕ OPERATORS											*
 ************************************************************/

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

bool AssertReply::_has_message_code(const Message& msg, const std::string& expected, std::string* actual) const
{
    if (msg.cmdOrCode != expected) {
        if (msg.cmdOrCode.empty())
            *actual = "nothing";
        else
            *actual = msg.cmdOrCode;
        return false;
    }
    return true;
}

bool AssertReply::_is_message_trailing(const Message& msg, const std::string& trailing, std::string* actual) const
{
    if (msg.trailing != trailing) {
        *actual = msg.trailing;
        return false;
    }
    return true;
}

bool AssertReply::_is_message_starting_with(const Message& msg, const std::string& start, std::string* actual) const
{
    if (!msg.raw.starts_with(start)) {
        *actual = msg.raw;
        return false;
    }
    return true;
}

bool AssertReply::_is_message_empty(const Message& msg, std::string* actual) const
{
    if (!msg.raw.empty()) {
        *actual = msg.raw;
        return false;
    }
    return true;
}

bool AssertReply::_is_message_matching_entirely(const Message& msg, const std::string& message, std::string* actual) const
{
    if (msg.raw != message) {
        *actual = msg.raw;
        return false;
    }
    return true;
}

bool AssertReply::_is_message_containing(const Message& msg, const std::string& token) const { return msg.raw.find(token) != std::string::npos; }

AssertReply& AssertReply::has_code(ReplyCode code)
{
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(3) << code;
    std::string actual     = "";
    bool        isMatching = false;
    for (std::vector<Message>::iterator it = _messages.begin(); it != _messages.end(); ++it) {
        if (_has_message_code(*it, oss.str(), &actual)) {
            isMatching = true;
            break;
        }
    }
	const std::string& expectedCodeStr = ircCodes.str(code);
	std::string actualCodeStr = std::string("");
	if (!actual.empty())
		actualCodeStr = ircCodes.str(static_cast<ReplyCode>(std::atoi(actual.c_str())));
    if (!isMatching)
        throw AssertFail("code", std::string(oss.str()) + " [" + expectedCodeStr + "]", std::string(actual) + " [" + actualCodeStr + "]" );
    return *this;
}

AssertReply& AssertReply::ends_with(const std::string& trailing)
{
    std::string actual     = "";
    bool        isMatching = false;
    for (std::vector<Message>::iterator it = _messages.begin(); it != _messages.end(); ++it) {
        if (_is_message_trailing(*it, trailing, &actual)) {
            isMatching = true;
            break;
        }
    }
    if (!isMatching) {
        throw AssertFail("trailing message ", trailing, actual);
    }
    return *this;
}

AssertReply& AssertReply::starts_with(const std::string& start)
{
    std::string actual = "";
    bool        isMatching = false;
    for (std::vector<Message>::iterator it = _messages.begin(); it != _messages.end(); ++it) {
        if (_is_message_starting_with(*it, start, &actual)) {
            isMatching = true;
            break;
        }
    }
    if (!isMatching) {
        throw AssertFail("trailing message ", start, actual);
    }
    return *this;
}

AssertReply& AssertReply::contains(const std::string& token)
{
    bool isMatching = false;
    for (std::vector<Message>::iterator it = _messages.begin(); it != _messages.end(); ++it) {
        if (_is_message_containing(*it, token)) {
            isMatching = true;
            break;
        }
    }
    if (!isMatching) {
        throw AssertFail("message ", token, "no occurence");
    }
    return *this;
}

AssertReply& AssertReply::do_not_contains(const std::string& token)
{
    bool isMatching = true;
    for (std::vector<Message>::iterator it = _messages.begin(); it != _messages.end(); ++it) {
        if (_is_message_containing(*it, token)) {
            isMatching = false;
            break;
        }
    }
    if (isMatching) {
        throw AssertFail("not in message ", token, token);
    }
    return *this;
}

AssertReply& AssertReply::matches_entirely(const std::string& message)
{
    std::string actual     = "";
    bool        isMatching = false;
    for (std::vector<Message>::iterator it = _messages.begin(); it != _messages.end(); ++it) {
        if (_is_message_matching_entirely(*it, message, &actual)) {
            isMatching = true;
            break;
        }
    }
    if (!isMatching) {
        throw AssertFail("message ", message, actual);
    }
    return *this;
}

AssertReply& AssertReply::is_empty()
{
    std::string actual     = "";
    bool        isMatching = false;
    for (std::vector<Message>::iterator it = _messages.begin(); it != _messages.end(); ++it) {
        if (_is_message_empty(*it, &actual)) {
            isMatching = true;
            break;
        }
    }
    if (!isMatching) {
        throw AssertFail("message ", "empty", actual);
    }
    return *this;
}

/**
 * @brief checks full message based on code
 * @todo implement by getting client nick
 * @param code 
 * @return AsserReply& 
 */
AssertReply&  AssertReply::is_formatted(ReplyCode code, const std::string& clientNick)
{
    std::string expectedStart = std::string(":" + ircConfig.get_name());
    expectedStart += " " + utils::code_to_str(code) + " " + clientNick + " ";
    const std::string& expectedTrailing = ircCodes.trailing(code);
    return this->starts_with(expectedStart).ends_with(expectedTrailing);
}

AssertReply& AssertReply::handle_new_reply(const std::string& reply)
{
    _reply = reply;
    _process_reply();
    return *this;
}

void AssertReply::_process_reply()
{
    std::istringstream iss(_reply);

    std::string rawMsg;
    std::string msgPrefix;
    std::string msgCmdOrCode;
    std::string msgTrailing;
    t_params    msgArgs;
    if (_reply.empty()) {
        Message msg = {.args = msgArgs, .cmdOrCode = "", .prefix = "", .trailing = "", .raw = _reply};
        _messages.push_back(msg);
        return;
    }

    while (std::getline(iss, rawMsg, '\n')) {
        LOG_DTV_TEST(rawMsg);
        std::istringstream issMsg(rawMsg);
        if (rawMsg[0] == ':') {
            issMsg >> msgPrefix;
            LOG_DTV_TEST(msgPrefix);
        }
        issMsg >> msgCmdOrCode;
        LOG_DTV_TEST(msgCmdOrCode);
        std::string token;
        while (issMsg >> token) {
            if (token[0] == ':') {
                msgTrailing = token;
                while (issMsg >> token)
                    msgTrailing += " " + token;
                LOG_DTV_TEST(msgTrailing);
                break;
            }
            msgArgs.push_back(token);
            LOG_DT_TEST("msgArg", token);
        }
        Message msg = {.args = msgArgs, .cmdOrCode = msgCmdOrCode, .prefix = msgPrefix, .trailing = msgTrailing, .raw = rawMsg};
        _messages.push_back(msg);
    }
}
