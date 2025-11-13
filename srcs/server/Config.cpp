#include "Config.hpp"

#include "LogManager.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"

#include <fstream>

const Config ircConfig(SERVER_CONF_FILE);
#ifdef TEST
const Config ircConfigTest(SERVER_CONF_FILE_FOR_TEST);
#endif

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR                *
 ************************************************************/

Config::Config(const std::string& fileName) :
    exist(true),
    _name(SERVER_NAME),
    _psswd(DEFAULT_PASSWORD),
    _port(DEFAULT_PORT),
    _maxJoinedChannels(MAX_JOINED_CHANNELS),
    _chanNameMaxLen(CHAN_NAME_MAX_LEN),
    _nicknameMaxLen(NICKNAME_MAX_LEN),
    _targetLimit(TARGET_LIMIT),
    _codes(),
    _trailings(),
    _motd()
{
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    std::string actualFileName = fileName;
#ifdef TEST
    if (fileName == SERVER_CONF_FILE)
        actualFileName = SERVER_CONF_FILE_FOR_TEST;
#endif
    if (!_parse_config_file(actualFileName)) {
        LOG_SERVER.warning("CONF FILE not loaded!");
        exist = false;
    }
    if (_motd.empty()) {
        LOG_SERVER.warning("MOTD is empty!");
    }
    if (_codes.empty()) {
        LOG_SERVER.warning("RFC CODES are empty!");
    }
    if (_trailings.empty()) {
        LOG_SERVER.warning("RFC TRAILING MESSAGES are empty!");
    }
}
Config::~Config() {}

/*************************************************************
 *                      🛠️ FUNCTIONS                         *
 *************************************************************/

static std::string trim(const std::string& s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";
    return s.substr(start, end - start + 1);
}

void Config::_parse_codes(const std::string& line)
{
    size_t posAssign  = line.find('=');
    size_t posComment = line.find('#');
    size_t posColon   = line.find(':');

    // Skip comments and lines without assignment
    if (posAssign == std::string::npos || posComment != std::string::npos)
        return;

    std::string keyStr = line.substr(0, posAssign);
    int         code   = std::atoi(keyStr.c_str());
    if (code < 0 && code > MAX_CODE_NUMBER)
        return;

    if (posColon != std::string::npos) {
        // Format: 001=RPL_WELCOME: Welcome message
        std::string value    = line.substr(posAssign + 1, posColon - posAssign - 1);
        std::string trailing = line.substr(posColon + 1);
        _codes[code]         = value;
        _trailings[code]     = trailing;
    } else {
        // Format: 001=RPL_WELCOME
        std::string value = line.substr(posAssign + 1);
        _codes[code]      = value;
        _trailings[code]  = "";
    }
}

void Config::_parse_conf(const std::string& line)
{
    size_t posAssign  = line.find('=');
    size_t posComment = line.find('#');
    if (posAssign != std::string::npos && posComment == std::string::npos) {
        std::string key   = trim(line.substr(0, posAssign));
        std::string value = trim(line.substr(posAssign + 1));
        _set_key_value(key, value);
    }
}
void Config::_parse_motd(const std::string& line) { _motd.push_back(line); }

bool Config::_parse_config_file(const std::string& fileName)
{
    std::string   currentSection;
    std::ifstream file(fileName.c_str());
    if (!file.is_open())
        return false;
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '[' && line[line.size() - 1] == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        if (currentSection == "irc.conf")
            _parse_conf(line);
        else if (currentSection == "irc.codes")
            _parse_codes(line);
        else if (currentSection == "irc.motd")
            _parse_motd(line);
    }
    file.close();
    return true;
}

void Config::_set_key_value(const std::string& key, std::string& value)
{
    const size_t nbParam = 7;
    std::string  keyList[nbParam]
        = {"server_name", "password", "port", "maxJoinedChannels", "chanNameMaxLen", "nicknameMaxLen", "targetLimit"};
    void (Config::* functions[nbParam])(std::string&) = {&Config::_set_name,
                                                         &Config::_set_password,
                                                         &Config::_set_port,
                                                         &Config::_set_max_joined_channels,
                                                         &Config::_set_chan_name_max_len,
                                                         &Config::_set_nickname_max_len,
                                                         &Config::_set_target_limit};
    for (size_t i = 0; i < nbParam; i++) {
        if (key == keyList[i]) {
            (this->*functions[i])(value);
            return;
        }
    }
}

void Config::_set_name(std::string& value) { _name = value; }
void Config::_set_password(std::string& value) { _psswd = value; }
void Config::_set_port(std::string& value)
{
    int port = static_cast<int>(atoi(value.c_str()));
    if (port >= 1 && port <= MAX_PORT)
        _port = port;
}
void Config::_set_max_joined_channels(std::string& value)
{
    int maxJoinedChannels = static_cast<int>(atoi(value.c_str()));
    if (maxJoinedChannels >= 1 && maxJoinedChannels < MAX_JOINED_CHANNELS)
        _maxJoinedChannels = maxJoinedChannels;
}
void Config::_set_chan_name_max_len(std::string& value)
{
    int chanNameMaxLen = static_cast<int>(atoi(value.c_str()));
    if (chanNameMaxLen >= 1 && chanNameMaxLen < CHAN_NAME_MAX_LEN)
        _chanNameMaxLen = chanNameMaxLen;
}
void Config::_set_nickname_max_len(std::string& value)
{
    int nicknameMaxLen = static_cast<int>(atoi(value.c_str()));
    if (nicknameMaxLen >= 1 && nicknameMaxLen < NICKNAME_MAX_LEN)
        _nicknameMaxLen = nicknameMaxLen;
}
void Config::_set_target_limit(std::string& value)
{
    int targetLimit = static_cast<int>(atoi(value.c_str()));
    if (targetLimit >= 1 && targetLimit < TARGET_LIMIT)
        _targetLimit = targetLimit;
}

const std::string& Config::str(ReplyCode code) const
{
    std::map<int, std::string>::const_iterator it = _codes.find(static_cast<int>(code));
    if (it != _codes.end()) {
        return it->second;
    }
    static std::string fallback = "UNKNOWN_CODE";
    return fallback;
}

const std::string& Config::trailing(ReplyCode code) const
{
    std::map<int, std::string>::const_iterator it = _trailings.find(static_cast<int>(code));
    if (it != _trailings.end()) {
        return it->second;
    }
    static std::string fallback = "";
    return fallback;
}
const std::string& Config::get_name() const { return _name; }
const std::string& Config::get_password() const { return _psswd; }
int                Config::get_max_joined_channels() const { return _maxJoinedChannels; }
size_t             Config::get_chan_name_max_len() const { return _chanNameMaxLen; }
size_t             Config::get_nickname_max_len() const { return _nicknameMaxLen; }

std::vector<std::string> Config::get_motd() const { return _motd; }
