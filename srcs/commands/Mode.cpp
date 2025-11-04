#include "Mode.hpp"

#include "LogManager.hpp"
#include "Parser.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <queue>
#include <sstream>
#include <string>

/*************************************************************
 *                  🛠️ UTILS FUNCTIONS                       *
 *************************************************************/

static void parse_params(std::string &params, std::queue<std::string> *modeQueue,
                         std::queue<std::string> *paramsQueue) {
    std::string args;
    std::istringstream iss(params);
    LOG_DV_CMD(params);
    // first args must be a modes
    iss >> args;
    LOG_DV_CMD(args);
    LOG_DV_CMD(args.size());
    char sign = '+';
    if (args.empty())
        return;
    if (args.size() == 1 || !Utils::is_char_of(args[0], std::string(MODE_OPERATOR)))
        modeQueue->push(args);
    for (std::string::iterator it = args.begin(); it != args.end(); ++it) {
        if (Utils::is_char_of(*it, std::string(MODE_OPERATOR))) {
            sign = *it;
            if (it + 1 != args.end() && Utils::is_char_of(*(it + 1), std::string(MODE_OPERATOR))) {
                modeQueue->push(std::string(1, *it));
                LOG_d_CMD(*it);
            }
        } else {
            modeQueue->push(std::string(1, sign) + *it);
            LOG_DV_CMD(*it);
        }
    }
    // other args
    while (iss >> args) {
        LOG_DV_CMD(args);
        if (args.size() > 1 && Utils::is_char_of(args[0], std::string(MODE_OPERATOR))) {
            for (std::string::iterator it = args.begin(); it != args.end(); ++it) {
                if (Utils::is_char_of(*it, std::string(MODE_OPERATOR))) {
                    sign = *it;
                    if (it + 1 != args.end() && Utils::is_char_of(*(it + 1), std::string(MODE_OPERATOR))) {
                        modeQueue->push(std::string(1, *it));
                        LOG_d_CMD(*it);
                    }
                } else {
                    modeQueue->push(std::string(1, sign) + *it);
                    LOG_DV_CMD(*it);
                }
            }
        } else
            paramsQueue->push(args);
    }
}

static unsigned short char_to_mode(char c) {
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
 *                    📁 CLASS METHODS                      *
 ************************************************************/
Mode::Mode() {}
Mode::Mode(std::string &params) {
    Parser parser;

    _channelName = parser.format_parameter(params, NULL);
    parse_params(params, &_modeQueue, &_paramsQueue);
}
Mode::~Mode() {}

/*************************************************************
 *                     🛠️ FUNCTIONS                          *
 *************************************************************/
bool Mode::_simple_args(Server &server, Client &client, Channel *&channel, Parser &p) {
    // no params -> MODEV
    if (Channel::is_valid_channel_name(_channelName)) {
        std::map<std::string, Channel *>::iterator it = server.channels.find(_channelName);
        if (it == server.channels.end()) {
            p.response(ERR_NOSUCHCHANNEL, _channelName);
            return true;
        } else {
            channel = it->second;
            if (!channel) {
                p.response(ERR_NOSUCHCHANNEL, _channelName);
                return true;
            }
        }
    } else {
        p.response(ERR_NEEDMOREPARAMS, "MODE");
        return true;
    }
    // params but only args no modes iklt-> MODE #chan1 user1
    if (channel && _modeQueue.empty() && !_paramsQueue.empty()) {
        p.response(ERR_NEEDMOREPARAMS, "MODE");
        return true;
    }
    // MODE #chan1 with no other args
    if (channel && _modeQueue.empty() && _paramsQueue.empty()) {
        // unsigned short currentModes = channel->get_mode();
        LOG_d_CMD("only Chan param");
        std::string modeIsReply = channel->get_name();
        modeIsReply += channel->get_modes_str(client);
        // modeIsReply += get_modes(currentModes, channel);
        p.response(RPL_CHANNELMODEIS, modeIsReply);
        return true;
    }
    return false;
}

void Mode::_mode_with_noparams(Channel *channel, std::string &currentMode, std::string &validPositiveModes,
                               std::string &validNegativeModes) {
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
}

void Mode::_mode_k(Channel *channel, Parser &p, std::string &currentMode, std::string &currentParam,
                   std::string &validPositiveModes, std::string &validModesParams) {
    size_t invalidChar = 0;
    if (!currentParam.empty())
        invalidChar = std::count_if(currentParam.begin(), currentParam.end(), Utils::is_invalid_char_key);
    if (currentMode[0] == '+' && !currentParam.empty() && !invalidChar) {
        if (channel->get_mode() & char_to_mode(currentMode[1])) {
            p.response(ERR_KEYSET, channel->get_name());
        } else {
            channel->add_mode(char_to_mode(currentMode[1]));
            channel->set_key(currentParam);
            validPositiveModes += currentMode[1];
            validModesParams += " " + currentParam;
        }
    } else
        p.response(ERR_NEEDMOREPARAMS, "MODE");
}

void Mode::_mode_l(Channel *channel, Parser &p, std::string &currentMode, std::string &currentParam,
                   std::string &validPositiveModes, std::string &validModesParams) {
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
        p.response(ERR_NEEDMOREPARAMS, "MODE");
}

void Mode::_mode_o(Server &server, Channel *channel, Parser &p, std::string &currentMode, std::string &currentParam,
                   std::string &validPositiveModes, std::string &validNegativeModes, std::string &validModesParams) {
    size_t invalidChar = 0;
    invalidChar = std::count_if(currentParam.begin(), currentParam.end(), Utils::is_invalid_char_nick);
    if (!currentParam.empty() && !invalidChar) {
        Client *targetOp = server.find_client_by_nickname(currentParam);
        if (!targetOp)
            p.response(ERR_NOSUCHNICK, currentParam);
        else if (!channel->is_member(*targetOp))
            p.response(ERR_USERNOTINCHANNEL, currentParam);
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
        p.response(ERR_NEEDMOREPARAMS, "MODE");
}

void Mode::execute(Server &server, Client &client) {
    Parser p(server, client);

    Channel *channel = NULL;
    LOG_DV_CMD(_modeQueue.size());
    LOG_DV_CMD(_paramsQueue.size());
    if (_simple_args(server, client, channel, p))
        return;
    // to change a MODE you need to be an operator
    if (!channel->is_operator(client)) {
        p.response(ERR_CHANOPRIVSNEEDED, channel->get_name());
        return;
    }
    // others cases main args loop
    std::string validPositiveModes = "+";
    std::string validNegativeModes = "-";
    std::string validModesParams = "";
    while (!_modeQueue.empty()) {
        std::string currentMode = _modeQueue.front();
        _modeQueue.pop();
        // cases more than one + or -, or no operator or only + or -
        if (currentMode.size() > 2 || currentMode.size() == 1 ||
            !Utils::is_char_of(currentMode[0], std::string(MODE_OPERATOR))) {
            p.response(ERR_NEEDMOREPARAMS, "MODE");
            return;
        }
        // case not a valid mode
        if (!Utils::is_char_of(currentMode[1], std::string(VALID_CHAN_MODE_NOPARAM)) &&
            !Utils::is_char_of(currentMode[1], std::string(VALID_CHAN_MODE_PARAM))) {
            p.response(ERR_UNKNOWNMODE, currentMode.substr(1));
            continue;
        }
        // mode need params but there isn't
        if ((currentMode == "+k" || currentMode == "+l" || currentMode[1] == 'o') && _paramsQueue.empty()) {
            p.response(ERR_NEEDMOREPARAMS, "MODE");
            continue;
        }
        // case mode need no params
        if (Utils::is_char_of(currentMode[1], std::string(VALID_CHAN_MODE_NOPARAM))) {
            _mode_with_noparams(channel, currentMode, validPositiveModes, validNegativeModes);
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
            std::string currentParam = _paramsQueue.front();
            _paramsQueue.pop();
            LOG_DV_CMD(currentParam);
            if (currentMode[1] == 'k') {
                _mode_k(channel, p, currentMode, currentParam, validPositiveModes, validModesParams);
            } else if (currentMode[1] == 'l') {
                _mode_l(channel, p, currentMode, currentParam, validPositiveModes, validModesParams);
            } else if (currentMode[1] == 'o') {
                _mode_o(server, channel, p, currentMode, currentParam, validPositiveModes, validNegativeModes,
                        validModesParams);
            }
        }
    }
    std::string confirmationMsg = _channelName + " ";
    if (validPositiveModes.size() > 1)
        confirmationMsg += validPositiveModes;
    if (validNegativeModes.size() > 1)
        confirmationMsg += validNegativeModes;
    if (validModesParams.size() > 1)
        confirmationMsg += validModesParams;
    if (confirmationMsg != _channelName) {
        p.response(TRANSFER_MODE, confirmationMsg);
        channel->broadcast(server, TRANSFER_MODE, confirmationMsg, &client, "");
    }
}
