#include "BotReply.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "reply_codes.hpp"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <iomanip>

/************************************************************
*		🥚 CONSTRUCTORS & DESTRUCTOR						*
************************************************************/

BotReply::BotReply(void) {}

BotReply::BotReply(const std::string& params) : _params(params) {}

BotReply::BotReply(const BotReply& other)
{
    (void) other;
}

BotReply::~BotReply(void) {}

ReplyCode BotReply::check_args(Server& s, Client& c, std::vector<Channel*>& channels, std::string& params)
{
    (void) s;
    (void) c;
    if (params.empty())
        return (ERR_NEEDMOREPARAMS);
    // TODO check for malicious input (bash commands, pipes)
    if (channels.size() > 1)
        return (ERR_WRONG_FORMAT);
    return (RPL_SUCCESS);
}

/************************************************************
*		➕ OPERATORS											*
************************************************************/

BotReply& BotReply::operator=(const BotReply& other) 
{
    (void) other;
    return (*this);
}

/*************************************************************
*		🛠️ FUNCTIONS											*
*************************************************************/

void add_key_val_bool(std::string& command, const std::string& key, bool value)
{
    std::string boolVal = value ? "true" : "false"; 
    command += '"' + key + "\": " + boolVal + ",";
}

void add_key_val(std::string& command, const std::string& key, const std::string& value)
{
    command += '"' + key + "\": " + '"' + value + "\","; 
} 

static void send_llama_equest(const std::string& prompt, std::string& response)
{
    // build curl request
    std::string command = "curl -X GET -v localhost:11434/api/generate -d '{";
    add_key_val(command, "model", "gemma3:1b");
    add_key_val(command, "prompt", prompt);
    add_key_val_bool(command, "stream", false);
    command[command.length() - 1] = '}';
    command += "' | jq '.response' ";
    command += " > llama_response.txt'";
    LOG_d_CMD(command);
    
    // system call
    int code = ::system(command.c_str());
    if (code == -1)
    {
        LOG_E_SERVER("error sending API LLama request", command);
    }

    // read json file
    std::ifstream file("llama_response.json");
    std::getline(file, response);
}


void	BotReply::execute(Server& s, Client& c, Channel* channel)
{
    // build prompt;
    std::string prompt = "Please reply to this message in less than 500 characters : ";
    prompt += _params;

    // send request
    std::string response;
    send_llama_equest(prompt, response);

    ReplyHandler&   rh = ReplyHandler::get_instance(&s);
    // TODO channel broadcast with Client Bot as sender
    channel->broadcast(s, RPL_BOT, response);
    (void) rh;
    (void) c;
}

