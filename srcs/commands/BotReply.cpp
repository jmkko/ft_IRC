#include "BotReply.hpp"
#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <unistd.h>

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

ReplyCode BotReply::check_args(Server& s, Client& c, std::vector<Client*>& clients, std::vector<Channel*>& channels, std::string& params)
{
    (void) s;
    (void) c;
    if (params.empty())
        return (ERR_NEEDMOREPARAMS);
    // TODO check for malicious input (bash commands, pipes)
    if (!clients.empty() && !channels.empty())
        return (CUSTOMERR_WRONG_FORMAT);
    if (clients.empty() && channels.empty())
        return (CUSTOMERR_WRONG_FORMAT);
    if (clients.size() > TARGET_LIMIT)
        return (CUSTOMERR_WRONG_FORMAT);
    if (channels.size() > 1)
        return (CUSTOMERR_WRONG_FORMAT);
    return (CORRECT_FORMAT);
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
    std::string command = "curl -X POST -H \"Content-Type: application/json\" -v localhost:11434/api/generate -d '{";
    add_key_val(command, "model", "gemma3:1b");
    add_key_val(command, "prompt", prompt);
    add_key_val_bool(command, "stream", false);
    command[command.length() - 1] = '}';
    command += "' | jq \'.response\' ";
    command += " > llama_response.txt";
    LOG_d_CMD(command);
    
    // system call
    int code = ::system(command.c_str());
    if (code == -1)
    {
        LOG_E_SERVER("error sending API LLama request", command);
    }

    ::usleep(BOT_PROCESS_TIME_MS);
    // read json file
    std::ifstream file("llama_response.txt");
    response.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}


void	BotReply::execute(Server& s, Client& c, std::vector<Client*>& clients, Channel* channel)
{
    // build prompt;
    std::string prompt = "Please reply to this message in less than 500 characters : ";
    prompt += _params;

    // if channel
    // transfer prompt to channel

    // send request
    std::string response;
    send_llama_equest(prompt, response);

    ReplyHandler&   rh = ReplyHandler::get_instance(&s);
    LOG_D_CMD("answer read from file", response);
    // TODO channel broadcast with Client Bot as sender
    if (channel)
        channel->broadcast(s, TRANSFER_BOT, response);
    else
    {
        for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
        {
            rh.process_response(**it, TRANSFER_BOT, response);
        }
    }
    (void) c;
}

