#include "Mode.hpp"

#include "Channel.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "Logger.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <limits>
#include <sstream>
#include <string>

/*************************************************************
 *		🛠️ UTIL FUNCTIONS											*
 *************************************************************/

static void parse_args(std::vector<std::string>& args, std::string* channel, char* operation, std::string* modes, std::vector<std::string>* modeParams)
{
	std::vector<std::string>::iterator it = args.begin();
	*channel = *it;
    ++it;
    if (it != args.end())
    {
        *operation = (*it)[0];
        *modes = it->substr(1);
        while (++it != args.end())
        {
            modeParams->push_back(*it);
        }
    }
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
ReplyCode Mode::check_args(Server& server, Client& client, std::vector<std::string>& args)
{
	(void) server;
	(void) client;
	std::string channel;
	char 		operation = '\0';
	std::string modes;
	std::vector<std::string> modeParams;
    ReplyHandler rh = ReplyHandler::get_instance(&server);
    
    LOG_DV_CMD(args[0]);
    LOG_DV_CMD(args.size());
    LOG_DV_CMD(args.size());
	if (args.size() == 1 && Channel::is_valid_channel_name(args[0]))
    {
		return CORRECT_FORMAT;
    }
    if (args.size() < 2)
    {
        rh.process_response(client, ERR_NEEDMOREPARAMS, "MODE");
        return PROCESSED_ERROR;
    }
    parse_args(args, &channel, &operation, &modes, &modeParams);
    LOG_DV_CMD(operation);
    LOG_DV_CMD(modes);
    if (!modeParams.empty())
        LOG_DV_CMD(modeParams[0]);
    if (channel.empty() || modes.empty())
    {
		rh.process_response(client, ERR_NEEDMOREPARAMS, "MODE");
        return PROCESSED_ERROR;
    }

	unsigned long idx = modes.find_first_not_of(authorizedModes);
    if (idx != std::string::npos)
    {
        LOG_CMD.log(WARN, __FILE_NAME__, __FUNCTION__,"unknown option (other than kilot)", modes);
		rh.process_response(client, ERR_UNKNOWNMODE, &(modes.at(idx)));
        return PROCESSED_ERROR;
    }
	unsigned long opIdx = (std::string(1, operation).find_first_not_of(authorizedOps));
    if (opIdx != std::string::npos)
    {
        LOG_CMD.log(WARN, __FILE_NAME__, __FUNCTION__, "unknown operator (other than +-)", operation);
        rh.process_response(client, ERR_UNKNOWNMODE, std::string(1, operation));
        return PROCESSED_ERROR;
    }
	for (size_t i = 0; i < modes.size(); ++i)
	{
		if (modesRequiringArg.find(modes[i]) != std::string::npos && operation == '+' && i >= modeParams.size())
        {
			rh.process_response(client, ERR_NEEDMOREPARAMS, "MODE");
            return PROCESSED_ERROR;
        }
		if (modes[i] == 'l' && operation == '+')
        {
            if (modeParams[i].find_first_not_of(digits) != std::string::npos)
            {
                LOG_DV_CMD(modeParams[i]);
                return CUSTOMERR_WRONG_FORMAT;
            }
            if (std::atol(modeParams[i].c_str()) > std::numeric_limits<int>::max())
                return CUSTOMERR_WRONG_FORMAT;
        }
	}
	return CORRECT_FORMAT;
}

 /************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

Mode::Mode() : _args() {}
Mode::Mode(const std::vector<std::string>& args) : _args(args) {}
Mode::~Mode() {}

 /*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

 /**
  * @brief check business validity of args before adjusting modes 
  * namely channel modes k/i/l/t and user mode o
  * if duplicate modes are present (as in +kk key1 key2) the last one prevails
  * @param server 
  * @param client 
  */
void Mode::execute(Server& server, Client& client)
{
	ReplyHandler& rh = ReplyHandler::get_instance(&server);
	std::string channelName;
	char 		operation = '\0';
	std::string modes;
	std::vector<std::string> modeParams;
	parse_args(_args, &channelName, &operation, &modes, &modeParams);
	
	// checking channel
	Channel* channel = NULL;
	std::map<std::string, Channel*>::iterator it = server.channels.find(channelName);
	if (it == server.channels.end())
	{
		rh.process_response(client, ERR_NOSUCHCHANNEL, channelName);
		return ;
	}
	else
		channel = it->second;
	unsigned short currentModes = channel->get_mode();

	// case MOD #chan
	if (_args.size() == 1)
	{
        LOG_d_CMD("args == 1");
        std::string modeIsReply = channel->get_name();
		std::string modeIsParams("");
		std::string modeIsParamsVal("");
		if (!(currentModes & CHANMODE_INIT))
		{
			modeIsParams += '+';
			if (currentModes & CHANMODE_KEY)
			{
				modeIsParams += "k";
				modeIsParamsVal += channel->get_key();
			}
			if (currentModes & CHANMODE_INVITE)
				modeIsParams += "i";
			if (currentModes & CHANMODE_LIMIT)
			{
				modeIsParams += "l";
				modeIsParamsVal += TO_STRING(channel->get_user_limit());
			}
		}
		if (!modeIsParamsVal.empty())
			modeIsReply += " " + modeIsParams + " " + modeIsParamsVal;
        else if (!modeIsParams.empty())
            modeIsReply += " " + modeIsParams;
		rh.process_response(client, RPL_CHANNELMODEIS, modeIsReply);
        return ;
    }

	// checking client privileges
	if (!channel->is_operator(client))
	{
		rh.process_response(client, ERR_CHANOPRIVSNEEDED, channel->get_name());
		return ;
	}

	// checking and (un)setting modes
	std::string validModes = operation == '+' ? "+" : "-";
	std::string validModeParams = "";
    LOG_CMD.log(DEBUG, __FILE_NAME__, __FUNCTION__, "modes", modes);
	for (size_t i = 0; i < modes.size() ; ++i)
	{
		if (modes[i] == 'k')
		{
			if (operation == '+')
			{
                if (channel->get_mode() & CHANMODE_KEY)
				{
                    rh.process_response(client, ERR_KEYSET, channel->get_name());
				}
				else
				{
					channel->add_mode(CHANMODE_KEY);
					channel->set_key(modeParams[i]);
					validModes += "k";
					validModeParams += " " + modeParams[i];
				}
			}
			else {
				channel->remove_mode(CHANMODE_KEY);
				validModes += "k";
			}
		}
		else if (modes[i] == 'i')
		{
			if (operation == '+')
			{
				channel->add_mode(CHANMODE_INVITE);
			}
			else {
                channel->remove_mode(CHANMODE_INVITE);
			}
			validModes += 'i';
		}
		else if (modes[i] == 'l')
		{
			if (operation == '+')
			{
				channel->add_mode(CHANMODE_LIMIT);
				channel->set_user_limit(std::atoi(modeParams[i].c_str()));
				validModeParams += " " + modeParams[i];
			}
			else {
                channel->remove_mode(CHANMODE_LIMIT);
				channel->set_user_limit(NO_LIMIT);
			}
			validModes += 'l';
		}
		else if (modes[i] == 'o')
		{
			Client* targetOp = server.find_client_by_nickname(modeParams[i]);
			if (!targetOp)
                rh.process_response(client, ERR_NOSUCHNICK, modeParams[i]);
            else if (!channel->is_member(*targetOp))
                rh.process_response(client, ERR_USERNOTINCHANNEL, modeParams[i]);
			else
			{
				validModes += "o";
				validModeParams += " " + targetOp->get_nickname();
				if (operation == '+')
				{
					channel->make_operator(*targetOp);
					rh.process_response(*targetOp, RPL_YOUREOPER);
				}
				else {
					channel->remove_operator(*targetOp);
				}
			}
		}
        else if (modes[i] == 't')
        {
            if (operation == '+')
			{
				channel->add_mode(CHANMODE_TOPIC);
			}
			else {
                channel->remove_mode(CHANMODE_TOPIC);
			}
			validModes += 't';
        }
	}

	// send confirmation
	std::string confirmationMsg = channelName + " " + validModes + validModeParams;
	rh.process_response(client, TRANSFER_MODE, confirmationMsg);
}