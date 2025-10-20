#include "Channel.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "Logger.hpp"
#include "Mode.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <limits>
#include <queue>
#include <sstream>
#include <string>

/*************************************************************
 *		🛠️ UTIL FUNCTIONS											*
 *************************************************************/

static void
parse_params(std::string& params, std::string* channel, std::queue<std::string>* modeQueue, std::queue<std::string>* paramsQueue)
{
    std::string        args;
    std::istringstream iss(params);
    LOG_DV_CMD(params);
    iss >> *channel;
    // first args must be a modes
    iss >> args;
    LOG_DV_CMD(args);
    LOG_DV_CMD(args.size());
    if (args.empty())
        return;
    if (!Utils::is_char_of(args[0], std::string(MODE_OPERATOR)) || args.size() == 1)
        modeQueue->push(args);
    else if (args.size() > 1) {
        if (std::string(args.begin() + 1, args.end()).find(std::string(MODE_OPERATOR)) != std::string::npos)
            modeQueue->push(args);
        else
            for (std::string::iterator it = args.begin() + 1; it != args.end(); ++it)
                modeQueue->push(std::string(1, args[0]) + *it);
    }
    // other args
    while (iss >> args) {
        LOG_DV_CMD(args);
        if (args.size() > 1 && Utils::is_char_of(args[0], std::string(MODE_OPERATOR)) && !std::isdigit(args[1])) {
            if (std::string(args.begin() + 1, args.end()).find(std::string(MODE_OPERATOR)) != std::string::npos)
                modeQueue->push(args);
            else
                for (std::string::iterator it = args.begin() + 1; it != args.end(); ++it)
                    modeQueue->push(std::string(1, args[0]) + *it);
        } else
            paramsQueue->push(args);
    }
}
static std::string get_modes(unsigned short currentModes, Channel* channel)
{
    std::string modeIsReply("");
    std::string modeIsParams("");
    std::string modeIsParamsVal("");

    if (!(currentModes & CHANMODE_INIT)) {
        modeIsParams += '+';
        if (currentModes & CHANMODE_INVITE)
            modeIsParams += "i";
        if (currentModes & CHANMODE_KEY) {
            modeIsParams += "k";
            modeIsParamsVal += " " + channel->get_key();
        }
        if (currentModes & CHANMODE_LIMIT) {
            modeIsParams += "l";
            modeIsParamsVal += " " + TO_STRING(channel->get_user_limit());
        }
        if (currentModes & CHANMODE_TOPIC)
            modeIsParams += "t";
    }
    if (!modeIsParamsVal.empty())
        modeIsReply += " " + modeIsParams + modeIsParamsVal;
    else if (!modeIsParams.empty())
        modeIsReply += " " + modeIsParams;
    return modeIsReply;
}

static unsigned short char_to_mode(char c)
{
    if (c == 'i')
        return CHANMODE_INVITE;
    if (c == 'k')
        return CHANMODE_KEY;
    if (c == 'l')
        return CHANMODE_LIMIT;
    if (c == 't')
        return CHANMODE_TOPIC;
    return CHANMODE_INIT;
}

/************************************************************
 *		📁 CLASS METHODS									*
 ************************************************************/

/**
 * @brief check syntaxic validity of args
 *
 * @param server not used
 * @param client not used
 * @param args should match pattern `<channel> *( ( "-" / "+" ) *<modes> *<modeparams>`
 * @return ReplyCode
 */
ReplyCode Mode::check_args(Server& server, Client& client, std::string& params)
{
    (void)server;
    (void)client;
    std::string        channel;
    std::istringstream iss(params);
    iss >> channel;

    LOG_DV_CMD(channel);
    if (channel.empty()) {
        return PROCESSED_ERROR;
    }
    return CORRECT_FORMAT;
}

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

Mode::Mode() : _params() {}
Mode::Mode(const std::string& params) : _params(params) {}
Mode::~Mode() {}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

void Mode::execute(Server& server, Client& client)
{
    ReplyHandler&           rh = ReplyHandler::get_instance(&server);
    std::string             channelName;
    Channel*                channel = NULL;
    std::queue<std::string> modeQueue;
    std::queue<std::string> paramsQueue;

    parse_params(_params, &channelName, &modeQueue, &paramsQueue);
    LOG_DV_CMD(modeQueue.size());
    LOG_DV_CMD(paramsQueue.size());
    // no params -> MODE
    if (Channel::is_valid_channel_name(channelName)) {
        std::map<std::string, Channel*>::iterator it = server.channels.find(channelName);
        if (it == server.channels.end()) {
            rh.process_response(client, ERR_NOSUCHCHANNEL, channelName);
            return;
        } else {
            channel = it->second;
            if (!channel)
                return;
        }
    } else {
        rh.process_response(client, ERR_NEEDMOREPARAMS, "MODE");
        return;
    }
    // params but only args no modes iklt-> MODE #chan1 user1
    if (channel && modeQueue.empty() && !paramsQueue.empty()) {
        rh.process_response(client, ERR_NEEDMOREPARAMS, "MODE");
        return;
    }
    // MODE #chan1 with no other args
    if (channel && modeQueue.empty() && paramsQueue.empty()) {
        unsigned short currentModes = channel->get_mode();
        LOG_d_CMD("only Chan param");
        std::string modeIsReply = channel->get_name();
        modeIsReply += get_modes(currentModes, channel);
        rh.process_response(client, RPL_CHANNELMODEIS, modeIsReply);
        return;
    }
    // to change a MODE you need to be an operator
    if (!channel->is_operator(client)) {
        rh.process_response(client, ERR_CHANOPRIVSNEEDED, channel->get_name());
        return;
    }
    // others cases main args loop
    std::string validPositiveModes = " +";
    std::string validNegativeModes = " -";
    std::string validModesParams   = "";
    while (!modeQueue.empty()) {
        std::string currentMode = modeQueue.front();
        modeQueue.pop();
        // cases more than one + or -, or no operator or only + or -
        if (currentMode.size() > 2 || currentMode.size() == 1 || !Utils::is_char_of(currentMode[0], std::string(MODE_OPERATOR))) {
            rh.process_response(client, ERR_NEEDMOREPARAMS, "MODE");
            return;
        }
        // case not a valid mode
        if (!Utils::is_char_of(currentMode[1], std::string(VALID_CHAN_MODE_NOPARAM))
            && !Utils::is_char_of(currentMode[1], std::string(VALID_CHAN_MODE_PARAM))) {
            rh.process_response(client, ERR_UNKNOWNMODE, currentMode.substr(1));
            continue;
        }
        // mode need params but there isn't
        if ((currentMode == "+k" || currentMode == "+l" || currentMode[1] == 'o') && paramsQueue.empty()) {
            rh.process_response(client, ERR_NEEDMOREPARAMS, "MODE");
            continue;
        }
        // case mode need no params
        if (Utils::is_char_of(currentMode[1], std::string(VALID_CHAN_MODE_NOPARAM))) {
            LOG_d_CMD("MODE without param");
            LOG_DV_CMD(currentMode);
            if (currentMode[0] == '+') {
                channel->add_mode(char_to_mode(currentMode[1]));
                validPositiveModes += currentMode[1];
            }
            if (currentMode[0] == '-') {
                channel->remove_mode(char_to_mode(currentMode[1]));
                validNegativeModes += currentMode[1];
            }
            continue;
        }
        // case mode need param
        if (Utils::is_char_of(currentMode[1], std::string(VALID_CHAN_MODE_PARAM))) {
            LOG_d_CMD("MODE with param");
            LOG_DV_CMD(currentMode);
            // negative which don't need param
            if (currentMode[0] == '-' && (currentMode[1] == 'k' || currentMode[1] == 'l')) {
                LOG_d_CMD("Negative k or l");
                channel->remove_mode(char_to_mode(currentMode[1]));
                validNegativeModes += currentMode[1];
                continue;
            }
            std::string currentParam = paramsQueue.front();
            paramsQueue.pop();
            LOG_DV_CMD(currentParam);
            if (currentMode[1] == 'k') {
                size_t invalidChar = 0;
                if (!currentParam.empty())
                    invalidChar = std::count_if(currentParam.begin(), currentParam.end(), Utils::is_invalid_char_key);
                if (currentMode[0] == '+' && !currentParam.empty() && !invalidChar) {
                    if (channel->get_mode() & char_to_mode(currentMode[1])) {
                        rh.process_response(client, ERR_KEYSET, channel->get_name());
                    } else {
                        channel->add_mode(char_to_mode(currentMode[1]));
                        channel->set_key(currentParam);
                        validPositiveModes += currentMode[1];
                        validModesParams += " " + currentParam;
                    }
                } else
                    rh.process_response(client, ERR_NEEDMOREPARAMS, "MODE");
            } else if (currentMode[1] == 'l') {
                size_t invalidChar = 0;
                if (!currentParam.empty())
                    invalidChar = std::count_if(currentParam.begin(), currentParam.end(), Utils::is_not_digit);
                if (!currentParam.empty() && !invalidChar && currentMode[0] == '+') {
                    int limit = std::atoi(currentParam.c_str());
                    channel->add_mode(char_to_mode(currentMode[1]));
                    channel->set_user_limit(limit);
                    validPositiveModes += currentMode[1];
                    validModesParams += " " + currentParam;
                } else
                    rh.process_response(client, ERR_NEEDMOREPARAMS, "MODE");
            } else if (currentMode[1] == 'o') {
                size_t invalidChar = 0;
                invalidChar        = std::count_if(currentParam.begin(), currentParam.end(), Utils::is_invalid_char_nick);
                if (!currentParam.empty() && !invalidChar) {
                    Client* targetOp = server.find_client_by_nickname(currentParam);
                    if (!targetOp)
                        rh.process_response(client, ERR_NOSUCHNICK, currentParam);
                    else if (!channel->is_member(*targetOp))
                        rh.process_response(client, ERR_USERNOTINCHANNEL, currentParam);
                    else if (currentMode[0] == '+') {
                        channel->make_operator(*targetOp);
                        validPositiveModes += currentMode[1];
                        validModesParams += " " + currentParam;
                    } else {
                        channel->remove_operator(*targetOp);
                        validNegativeModes += currentMode[1];
                        validModesParams += " " + currentParam;
                    }
                } else
                    rh.process_response(client, ERR_NEEDMOREPARAMS, "MODE");
            }
        }
    }
    std::string confirmationMsg = channelName;
    if (validPositiveModes.size() > 2)
        confirmationMsg += validPositiveModes;
    if (validNegativeModes.size() > 2)
        confirmationMsg += validNegativeModes;
    if (validModesParams.size() > 1)
        confirmationMsg += validModesParams;
    if (confirmationMsg != channelName)
        rh.process_response(client, TRANSFER_MODE, confirmationMsg);
}
