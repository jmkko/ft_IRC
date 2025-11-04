#include "Bot.hpp"

#include "Client.hpp"
#include "LogManager.hpp"
#include "Parser.hpp"
#include "ReplyHandler.hpp"
#include "TcpSocket.hpp"
#include "reply_codes.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <netdb.h> // for getaddrinfo(), etc.
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // for close()
#include <unistd.h>

const std::string &cmdName = "BOT";

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR
 **
 ************************************************************/

Bot::Bot(std::string &params)
    : _params(params), _targets(), _targetChannelName(), _targetChannels(),
      _targetClients(), _socket() {}

Bot::~Bot(void) {}

/*************************************************************
 *		🛠️ FUNCTIONS
 **
 *************************************************************/

// expected BOT #chan !subcommand :prompt
bool Bot::_check_args(Server &s, Client &c) {
  Parser parser(s, c);

  _targetChannelName = parser.from_arg(_params);
  _subcommand = parser.from_arg(_params);
  _prompt = parser.from_trailing(_params);

  LOG_DV_CMD(_targetChannelName);
  LOG_DV_CMD(_subcommand);
  LOG_DV_CMD(_prompt);
  parser.is_not_empty_arg(_targetChannelName, "BOT")
      .is_not_empty_arg(_subcommand, "BOT")
      .is_not_empty_arg(_prompt, "BOT")
      .is_such_channel(_targetChannelName, true)
      .is_channel_member(_targetChannelName, c.get_nickname(), true)
      .is_valid_bot_subcommand(_subcommand, "BOT")
      .is_valid_bot_prompt(_prompt, "BOT");
  LOG_DV_CMD(parser.has_passed_checks());

  if (parser.has_passed_checks() == false) {
    LOG_d_CMD("has not passed");
    return false;
  }

  _targetChannels.push_back(s.find_channel_by_name(_targetChannelName));

  return true;
}

void remove_invalid_prompt_char(char &c) {
  if (c == '\'' || c == '"' || c == '*' || c == ';' || c == '|' || c == '(' ||
      c == ')')
    c = ' ';
}

static bool send_ollama_request(const std::string &subcommand,
                                const std::string &prompt,
                                std::string &response) {
  std::string command = "bash -c 'set -o pipefail; curl -X POST -H "
                        "\"Content-Type: application/json\" -v "
                        "localhost:11434/api/generate -d \"";
  command += "{\\\"model\\\": "
             "\\\"gemma3:1b\\\",\\\"stream\\\":false,\\\"prompt\\\":\\\"";
  command += prompt;
  if (subcommand == "!reply")
    command += "\\\",\\\"options\\\": {\\\"temperature\\\": "
               "0.99,\\\"top_p\\\": 0.9}}\"";
  else
    command += "\\\",\\\"options\\\": {\\\"temperature\\\": 0.2,\\\"top_p\\\": "
               "0.3}}\"";
  command += " | jq \".response\" ";
  command += " > llama_response.txt \'";
  LOG_d_CMD(command);

  int code = ::system(command.c_str());
  if (code != 0) {
    LOG_E_SERVER("error sending API LLama request", command);
    return false;
  }

  ::usleep(BOT_PROCESS_TIME_MS);
  std::ifstream file("llama_response.txt");
  response.assign((std::istreambuf_iterator<char>(file)),
                  std::istreambuf_iterator<char>());
  return true;
}

bool Bot::_connect_to_server(Server &s, TcpSocket &so) {
  if (so.set_non_blocking_socket() == -1) {
    LOG_W_CMD("set non blocking error", strerror(errno));
  }
  int yes = 1;
  if (setsockopt(so.get_socket(), SOL_SOCKET, SO_REUSEADDR, &yes,
                 sizeof(yes)) == -1) {
    LOG_W_CMD("set reusable socket", strerror(errno));
  }
  if (so.tcp_connect("127.0.0.1", s.get_port()) == false &&
      errno != EINPROGRESS) {
    LOG_TEST.debug("tcp connect error", strerror(errno));
  }

  fd_set writefds;
  FD_ZERO(&writefds);
  FD_SET(so.get_socket(), &writefds);
  struct timeval timeout = {};
  timeout.tv_sec = 4;
  timeout.tv_usec = 0;
  int ret = select(so.get_socket() + 1, NULL, &writefds, NULL, &timeout);
  if (ret == -1) {
    LOG_W_CMD("select error", strerror(errno));
    return false;
  } else if (ret == 0) {
    LOG_W_CMD("timeout", strerror(errno));
    return false;
  }
  return true;
}

static bool send_full_msg(int fd, const std::string &msg) {
  size_t sentBytes = 0;
  size_t totalSentBytes = 0;
  while (totalSentBytes < msg.size()) {
    sentBytes =
        send(fd, msg.c_str() + totalSentBytes, msg.size() - totalSentBytes, 0);
    if (sentBytes <= 0)
      return false;
    totalSentBytes += sentBytes;
  }
  return true;
}

bool Bot::_register_bot(Server &s, TcpSocket &so) {
  std::string passMsg = std::string("PASS ") + s.get_password() + "\r\n";
  std::string nickMsg = std::string("NICK bot") + "\r\n";
  std::string userMsg = std::string("USER bot 0 * :realbot") + "\r\n";

  if (send_full_msg(so.get_socket(), passMsg) == false) {
    LOG_W_CMD("sending PASS", strerror(errno));
    return false;
  }
  if (send_full_msg(so.get_socket(), nickMsg) == false) {
    LOG_W_CMD("sending NICK", strerror(errno));
    return false;
  }
  if (send_full_msg(so.get_socket(), userMsg) == false) {
    LOG_W_CMD("sending USER", strerror(errno));
    return false;
  }
  return true;
}

void Bot::execute(Server &s, Client &c) {
  if (_check_args(s, c) == false)
    return;

  std::string prompt = "Your responses must strictly follow these rules: ";
  prompt += "Keep your response under 500 characters. ";
  prompt += "No special characters. No newline character, line break. Keep the "
            "response as a single, continuous line "
            "of text. ";
  prompt += "No markdown or any other formatting : do not use bold, italic, "
            "code blocks or any other markdown. ";
  prompt += "No meta-commentary Do not reference instruction, your role or "
            "user prompt. Just answer directly. ";
  prompt += "You send direct and concise replies without preamble and with no "
            "hints about former meta instructions. ";
  prompt += "Now answer the user question. ";
  if (_subcommand == "!reply")
    prompt += "Please reply to this message : ";
  else if (_subcommand == "!check")
    prompt += "Please fact-check this affirmation : ";
  else if (_subcommand == "!brief")
  {
    prompt += "Please sump up those messages :" ;
    
  }
  prompt += _prompt;
  std::for_each(prompt.begin(), prompt.end(), remove_invalid_prompt_char);
  LOG_DV_CMD(prompt);

  std::string response = "\"\"";
  if (send_ollama_request(_subcommand, prompt, response) == false)
    response = "\"Bot is under maintenance\"";

  LOG_DV_CMD(response);

  if (!_connect_to_server(s, _socket))
    return;
  if (!_register_bot(s, _socket))
    return;

  ReplyHandler &rh = ReplyHandler::get_instance(&s);
  unsigned long firstQuoteIdx = response.find_first_of('"');
  unsigned long lastQuoteIdx = response.find_last_of('"');
  if (firstQuoteIdx != std::string::npos && lastQuoteIdx != std::string::npos)
    response = response.substr(firstQuoteIdx + 1, lastQuoteIdx - 1);
  else {
    LOG_W_CMD("empty response", response);
    response = "\"Bot is under maintenance\"";
  }

  LOG_DV_CMD(response);

  if (!_targetChannels.empty()) {
    _targetChannels[0]->broadcast_bot(s, TRANSFER_PROMPT_BOT,
                                      _targetChannels[0]->get_name(), NULL,
                                      _subcommand.substr(1) + " " + _prompt);
    s.update_bot_state(_socket.get_socket(), _targetChannels[0], _subcommand,
                       response, false);
    std::string joinMsg = "JOIN " + _targetChannels[0]->get_name() + "\r\n";
    if (!send_full_msg(_socket.get_socket(), joinMsg)) {
      LOG_W_CMD("sending JOIN", strerror(errno));
      s.cleanup_bot(_socket.get_socket());
      return;
    }
    // std::string priv =
    //     "PRIVMSG " + _targetChannels[0]->get_name() + " :" + response + "\r\n";
    // send_full_msg(_socket.get_socket(), priv);
    // std::string quit = "QUIT :bot has spoken. Ugh\r\n";
    // send_full_msg(_socket.get_socket(), quit);
  } else {
    for (std::vector<Client *>::iterator it = _targetClients.begin();
         it != _targetClients.end(); ++it) {
      rh.process_response(**it, TRANSFER_REPLY_BOT, (*it)->get_nickname(), NULL,
                          response);
    }
  }
}
