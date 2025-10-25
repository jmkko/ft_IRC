/**
 * @file consts.hpp
 * @brief constants used throughout IRC server program
 * @version 0.1
 * @date 2025-10-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef CONSTS_HPP
#define CONSTS_HPP

#define COMMANDS_NB               13
#define EXPECTED_ARGS_NB          3
#define MIN_PASSWORD_LEN          8
#define POLL_TIMEOUT              1000
#define MESSAGE_SIZE              512
#define PERM_755                  0755
#define PERM_644                  0644
#define PERM_777                  0777
#define EPOCH_TIME_START          1900
#define NO_LIMIT                  (-1)
#define SERVER_NAME               "hazardous.irc.serv"
#define SERVER_CONF_FILE          "irc.conf"
#define SERVER_CONF_FILE_FOR_TEST "../irc.conf"
#define DEFAULT_PASSWORD          "password"
#define DEFAULT_PORT              6667
#define MAX_PORT                  49152
#define MAX_CODE_NUMBER           999
#define MAX_JOINED_CHANNELS       NO_LIMIT
#define CHAN_NAME_MAX_LEN         50
#define FORBIDEN_CHAR_BOT_PROMPT  "|;"
#define FORBIDEN_CHAR_CHAN_NAME   "\x00\x07\x0D\x0A\x20\x2C\x3A" // NOLINT(clang-diagnostic-null-character)
#define FORBIDEN_CHAR_CHAN_KEY    "\x00\x09\x0A\x0B\x0C\x0D\x20" // NOLINT(clang-diagnostic-null-character)
#define FORBIDEN_CHAR_USER        "\x00\x0A\x0D\x20\x40"         // NOLINT(clang-diagnostic-null-character)
#define NUMBER_FORB_CCNK          7
#define NUMBER_FORB_CCU           5
#define VALID_CHAN_MODE_NOPARAM   "\x69\x74"
#define VALID_CHAN_MODE_PARAM     "\x6B\x6C\x6F"
#define MODE_OPERATOR             "+-"
#define WHITE_SPACE               " \t\n\r\v\f"
#define CHAN_KEY_MAX_LEN          23
#define CLIENT_READ_BUFFER_SIZE   512
#define WELL_KNOWN_PORT_MAX       1024
#define DYNAMIC_PORT_MIN          49152
#define NICKNAME_MAX_LEN          9
#define NB_AVAILABLE_CMD          16
#define NB_AVAILABLE_BOT_SUBCMD   2
#define USERS_PER_LINE            10
#define CHANMODE_INIT             0b00000
#define CHANMODE_KEY              0b00001
#define CHANMODE_INVITE           0b00010
#define CHANMODE_LIMIT            0b00100
#define CHANMODE_TOPIC            0b01000
#define TARGET_LIMIT              5

#include <string>
#include <vector>

typedef int                      Socket;
typedef std::vector<std::string> t_params;

#endif
