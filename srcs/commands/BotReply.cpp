#include "BotReply.hpp"
#include "Client.hpp"
#include "LogManager.hpp"
#include "Parser.hpp"
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
#include <vector>

const std::string& cmdName = "BOT";

/************************************************************
*		🥚 CONSTRUCTORS & DESTRUCTOR						*
************************************************************/

Bot::Bot(std::string& params) : _params(params), _targets(), _targetChannelName(), _targetChannels(), _targetClients() {}

Bot::~Bot(void) {}

/************************************************************
*		➕ OPERATORS											*
************************************************************/

/*************************************************************
*		🛠️ FUNCTIONS											*
*************************************************************/

// BOT #chan :prompt
bool Bot::_check_args(Server& s, Client& c)
{
    Parser  parser(s, c);

    _targetChannelName    = parser.from_arg(_params);
    _subcommand           = parser.from_arg(_params);
    _prompt               = parser.from_trailing(_params);

    LOG_DV_CMD(_targetChannelName);
    LOG_DV_CMD(_subcommand);
    LOG_DV_CMD(_prompt);
    parser.is_such_channel(_targetChannelName, true)
    .is_channel_member(_targetChannelName, c.get_nickname())
    .is_valid_bot_subcommand(_subcommand, "BOT")
    .is_valid_bot_prompt(_prompt, "BOT");
    LOG_DV_CMD(parser.has_passed_checks());

    if (!parser.has_passed_checks())
        return false;
 
    _targetChannels.push_back(s.find_channel_by_name(_targetChannelName));
    
    return true;
}

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

void	Bot::execute(Server& s, Client& c)
{
    if (!_check_args(s, c))
        return;

    std::string prompt = "";
    if (_subcommand == "!reply")
        prompt = "Please reply to this message in less than 500 characters : ";
    prompt += _prompt;

    // if channel target -> transfer prompt to channel
    if (!_targetChannels.empty())
        _targetChannels[0]->broadcast(s, TRANSFER_PROMPT_BOT, _params, &c);

    // send request
    std::string response;
    send_llama_equest(prompt, response);

    ReplyHandler&   rh = ReplyHandler::get_instance(&s);
    LOG_D_CMD("answer read from file", response);

    // TODO channel broadcast with Client Bot as sender
    if (!_targetChannels.empty())
        _targetChannels[0]->broadcast(s, TRANSFER_REPLY_BOT, _targetChannels[0]->get_name(), NULL, response);
    else
    {
        for (std::vector<Client*>::iterator it = _targetClients.begin(); it != _targetClients.end(); ++it)
        {
            rh.process_response(**it, TRANSFER_REPLY_BOT, (*it)->get_nickname(), NULL, response);
        }
    }
    (void) c;
}

