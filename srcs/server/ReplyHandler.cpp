#include "ReplyHandler.hpp"

#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "Server.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <cstdio>

/************************************************************
 *             🥚 CONSTRUCTORS & DESTRUCTOR                 *
 ************************************************************/

ReplyHandler::ReplyHandler() {}

/*************************************************************
 *                   🛠️ FUNCTIONS                            *
 *************************************************************/

static std::string get_user_id_of(Client& client)
{
    return ":" + client.get_nickname() + "!" + client.get_user_name() + "@" + ircConfig.get_name() + " ";
}

/**
 * @brief generates RFC_2812 formmated code response
 *
 * @param client who waiting response from the server
 * @param code of response
 * @param parameters message corresponding of the code
 * @return formatted code response
 */
static std::string
generate_code_response(Client& client, ReplyCode code, const std::string& parameters, const std::string& trailing)
{
    std::string nick          = client.get_nickname().empty() ? "*" : client.get_nickname();
    std::string numericPrefix = ":" + ircConfig.get_name() + " " + Utils::code_to_str(code) + " " + nick;

    std::string        trailingMessage, separatedParams;
    const std::string& defaultTrailing = ircConfig.trailing(code);

    if (!trailing.empty()) {
        trailingMessage = " :" + trailing;
    } else if (!defaultTrailing.empty()) {
        trailingMessage = " :" + defaultTrailing;
    }
    if (!parameters.empty()) {
        separatedParams = " " + parameters;
    }
    LOG_DV_SERVER(separatedParams);
    LOG_DV_SERVER(trailingMessage);
    return (numericPrefix + separatedParams + trailingMessage);
}

static std::string generate_non_numerical_response(
    Client& client, ReplyCode code, const std::string& parameters, Client* sender, const std::string& trailing = "")
{
    if (!sender) {
        sender = &client;
    }

    std::string trailingMessage, separatedParams;
    if (!trailing.empty()) {
        trailingMessage = " :" + trailing;
    }
    if (!parameters.empty()) {
        separatedParams = " " + parameters;
    }

    switch (code) {
    case TRANSFER_NICK:
        return (get_user_id_of(*sender) + "NICK" + trailingMessage);
    case TRANSFER_JOIN:
        return (get_user_id_of(*sender) + "JOIN" + separatedParams);
    case TRANSFER_PRIVMSG:
        return (get_user_id_of(*sender) + "PRIVMSG" + separatedParams + trailingMessage);
    case TRANSFER_KICK:
        return (get_user_id_of(*sender) + "KICK" + separatedParams + trailingMessage);
    case TRANSFER_INVITE:
        return (get_user_id_of(*sender) + "INVITE" + separatedParams);
    case TRANSFER_QUIT:
        return (get_user_id_of(*sender) + "QUIT" + separatedParams);
    case TRANSFER_MODE:
        return (get_user_id_of(*sender) + "MODE" + separatedParams);
    case TRANSFER_TOPIC:
        return (get_user_id_of(*sender) + "TOPIC" + separatedParams + trailingMessage);
    case TRANSFER_PROMPT_BOT:
        return (get_user_id_of(*sender) + "PRIVMSG" + separatedParams + trailingMessage);
    case TRANSFER_REPLY_BOT:
        return "bot@" + ircConfig.get_name() + " " + "PRIVMSG" + separatedParams + trailingMessage;
    case TRANSFER_PART:
        return (get_user_id_of(*sender) + "PART" + separatedParams + trailingMessage);
    case MSG_PING:
        return ":" + ircConfig.get_name() + " " + "PONG :" + parameters;
    default:
        return "";
    }
}

static bool is_numerical_response(ReplyCode code)
{
    return (code < LOWER_CUSTOM_CODE || (code > UPPER_CUSTOM_CODE && code < LOWER_CUSTOM_NONNUMERICAL_CODE));
}

int ReplyHandler::process_response(
    Server& server, Client& client, ReplyCode code, const std::string& parameters, Client* sender, const std::string& trailing)
{
    LOG_DT_CMD("processing", ircConfig.str(code));
    std::string response = "";
    if (is_numerical_response(code)) {
        response = generate_code_response(client, code, parameters, trailing);
    } else
        response = generate_non_numerical_response(client, code, parameters, sender, trailing);

    if (!response.empty()) {
        LOG_CMD.sending(__FILE_NAME__, __FUNCTION__, "\n\t\t\t\t\t\t\t\t\t\t\t   " + response, &client);
        _send_reply(server, client, response);
    }
    return (code);
}

void ReplyHandler::process_welcome(Server& server, Client& client)
{
    process_response(server, client,
                     RPL_WELCOME,
                     "",
                     NULL,
                     ircConfig.trailing(RPL_WELCOME) + " " + client.get_nickname() + "!" + client.get_user_name() + "@localhost");
    process_response(server, client, RPL_YOURHOST, "", NULL, ircConfig.trailing(RPL_YOURHOST) + " " + server.get_name());
    process_response(server, client, RPL_CREATED, "", NULL, ircConfig.trailing(RPL_CREATED));
    process_response(server, client, RPL_MYINFO, "", NULL, server.get_name() + " 1.0  0 0");
}
void ReplyHandler::_send_reply(Server& server, Client& client, const std::string& msg)
{
    client.append_to_send_buffer(msg + "\r\n");
    server.add_events_of(client, POLLOUT);
}

/*************************************************************
 *               👁️‍ GETTERS and SETTERS                      *
 *************************************************************/

ReplyHandler& ReplyHandler::get_instance()
{
    static ReplyHandler instance;
    return instance;
}
