#include "Mode.hpp"

#include "Channel.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "Server.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"
#include "utils.hpp"

#include <sstream>
#include <string>

/*************************************************************
 *		🛠️ UTIL FUNCTIONS											*
 *************************************************************/

static parse_args(std::vector<std::string>& args, std::string* channel, char* operation, std::string* modes, std::vector<std::string>* modeParams)
{
	std::vector<std::string>::iterator it = args.begin();
	*channel = *it;
	*operation = (*++it)[0];
	*modes = it->substr(1);
	while (++it != args.end())
	{
		modeParams->push_back(*it);
	}
}

 /************************************************************
 *		📁 CLASS METHODS									*
 ************************************************************/

ReplyCode Mode::check_args(Server& server, Client& client, std::vector<std::string>& args)
{
	std::string channel;
	char 		operation = '\0';
	std::string modes;
	std::vector<std::string> modeParams;
	parse_args(args, &channel, &operation, &modes, &modeParams);
}

 /************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

 /*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/