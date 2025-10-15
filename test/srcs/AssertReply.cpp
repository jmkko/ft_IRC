#include "AssertFail.hpp"
#include "AssertReply.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

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
        // Debug output to show invisible differences
        // printf("DEBUG: String comparison failed\n");
        // printf("Expected: '%s' (length: %zu)\n", message.c_str(), message.length());
        // printf("Actual  : '%s' (length: %zu)\n", msg.raw.c_str(), msg.raw.length());
        // printf("Expected bytes: ");
        // for (size_t i = 0; i < message.length(); ++i) {
        //     printf("[%02X]", (unsigned char)message[i]);
        // }
        // printf("\nActual bytes  : ");
        // for (size_t i = 0; i < msg.raw.length(); ++i) {
        //     printf("[%02X]", (unsigned char)msg.raw[i]);
        // }
        // printf("\n");
        return false;
    }
    return true;
}

bool AssertReply::_is_message_containing(const Message& msg, const std::string& token) const
{
    return msg.raw.find(token) != std::string::npos;
}

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
    std::string        actualCodeStr   = std::string("");
    if (!actual.empty())
        actualCodeStr = ircCodes.str(static_cast<ReplyCode>(std::atoi(actual.c_str())));
    if (!isMatching)
        throw AssertFail(
            "code", std::string(oss.str()) + " [" + expectedCodeStr + "]", std::string(actual) + " [" + actualCodeStr + "]");
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
        throw AssertFail("trailing message", trailing, actual);
    }
    return *this;
}

AssertReply& AssertReply::starts_with(const std::string& start)
{
    std::string actual     = "";
    bool        isMatching = false;
    for (std::vector<Message>::iterator it = _messages.begin(); it != _messages.end(); ++it) {
        if (_is_message_starting_with(*it, start, &actual)) {
            isMatching = true;
            break;
        }
    }
    if (!isMatching) {
        throw AssertFail("trailing message", start, actual);
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
        throw AssertFail("message", token, "no occurence");
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
        throw AssertFail("not in message", token, token);
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
        throw AssertFail("message", message, actual);
    }
    return *this;
}
void AssertReply::print_target_and_messages(const std::string& target) {
	std::cout << "Looking for: "<< target << std::endl << "in:" << std::endl;
    for (std::vector<Message>::iterator it = _messages.begin(); it != _messages.end(); ++it) {
		for (std::vector<std::string>::iterator itstring = it->args.begin(); itstring != it->args.end(); itstring++) {
			std::cout << *itstring;
		}
		std::cout << std::endl;
	}
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
        throw AssertFail("message", "empty", actual);
    }
    return *this;
}

/**
 * @brief checks full message based on code
 * @todo implement by getting client nick
 * @param code
 * @return AsserReply&
 */
AssertReply&
AssertReply::is_formatted(ReplyCode code, const std::string& clientNick, const std::string& params, const std::string& trailing)
{
    std::string expectedStart = std::string(":" + ircConfigTest.get_name()) + " " + utils::code_to_str(code) + " " + clientNick + " ";
    if (params.empty() && trailing.empty())
        return this->has_code(code).matches_entirely(expectedStart + ":" + ircCodes.trailing(code));
    else if (!params.empty() && trailing.empty())
        return this->has_code(code).matches_entirely(expectedStart + params + " :" + ircCodes.trailing(code));
    else if (params.empty() && !trailing.empty())
        return this->has_code(code).matches_entirely(expectedStart + ":" + trailing);
    else
        return this->has_code(code).matches_entirely(expectedStart + params + " :" + trailing);
}

AssertReply&
AssertReply::is_formatted_transfer(const std::string& clientNick, const std::string& params, const std::string& trailing)
{
    std::string expectedStart = std::string(":" + clientNick + "!" + clientNick + "@" + ircConfigTest.get_name()) + " ";
    if (trailing.empty())
        return this->matches_entirely(expectedStart + params);
    else
        return this->matches_entirely(expectedStart + params + " :" + trailing);
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

    LOG_DV_TEST(_reply);

    if (_reply.empty()) {
        Message msg = {.args = msgArgs, .cmdOrCode = "", .prefix = "", .trailing = "", .raw = _reply};
        _messages.push_back(msg);
        return;
    }

    while (std::getline(iss, rawMsg, '\n')) {
        // Reset variables for each message
        msgPrefix.clear();
        msgCmdOrCode.clear();
        msgTrailing.clear();
        msgArgs.clear();

        std::istringstream issMsg(rawMsg);

        // Strip \r if present
        if (!rawMsg.empty() && rawMsg[rawMsg.length() - 1] == '\r')
            rawMsg = rawMsg.substr(0, rawMsg.length() - 1);

        // Strip trailing whitespace
        // size_t start = rawMsg.find_first_not_of(" \t");
        // size_t end = rawMsg.find_last_not_of(" \t");
        // if (start != std::string::npos && end != std::string::npos)
        //     rawMsg = rawMsg.substr(start, end - start + 1);
        // else if (start == std::string::npos)
        //     rawMsg.clear(); // String is all whitespace

        if (!rawMsg.empty() && rawMsg[0] == ':') {
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
