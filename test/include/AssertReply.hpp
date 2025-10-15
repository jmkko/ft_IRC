#pragma once

#include "reply_codes.hpp"

#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>

struct Message {
    std::vector<std::string> args;
    std::string              cmdOrCode;
    std::string              prefix;
    std::string              trailing;
    std::string              raw;
};

class AssertReply
{

  public:
    AssertReply(const std::string& reply);
    virtual ~AssertReply();

    AssertReply& is_empty();
    AssertReply& has_code(ReplyCode code);
    AssertReply& ends_with(const std::string& trailing);
    AssertReply& starts_with(const std::string& start);
    AssertReply& contains(const std::string& token);
    AssertReply& do_not_contains(const std::string& token);
    AssertReply& matches_entirely(const std::string& message);
    AssertReply& is_formatted(ReplyCode code, const std::string& clientNick, const std::string& params = "", const std::string& trailing = "");
    AssertReply& is_formatted_transfer(const std::string& clientNick, const std::string& params, const std::string& trailing = "");

	void print_target_and_messages(const std::string& target);
    AssertReply& handle_new_reply(const std::string& reply);

  private:
    std::string          _reply;
    std::vector<Message> _messages;

    void _process_reply();
    bool _has_message_code(const Message& msg, const std::string& expected, std::string* actual) const;
    bool _is_message_trailing(const Message& msg, const std::string& trailing, std::string* actual) const;
    bool _is_message_starting_with(const Message& msg, const std::string& start, std::string* actual) const;
    bool _is_message_empty(const Message& msg, std::string* actual) const;
    bool _is_message_matching_entirely(const Message& msg, const std::string& message, std::string* actual) const;
    bool _is_message_containing(const Message& msg, const std::string& token) const;
};
