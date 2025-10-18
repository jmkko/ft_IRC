#include "Config.hpp"

#include "LogManager.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"

#include <fstream>
#include <iostream>

const Config ircConfig(SERVER_CONF_FILE);
const Config ircCodes(CODES_CONF_FILE);
#ifdef TEST
const Config ircConfigTest(SERVER_CONF_FILE_FOR_TEST);
#endif

Config::Config() :
    _name(SERVER_NAME),
    _psswd(DEFAULT_PASSWORD),
    _maxJoinedChannels(MAX_JOINED_CHANNELS),
    _chanNameMaxLen(CHAN_NAME_MAX_LEN),
    _nicknameMaxLen(NICKNAME_MAX_LEN)
{
}

Config::Config(const std::string& fileName) :
    _name(SERVER_NAME),
    _psswd(DEFAULT_PASSWORD),
    _maxJoinedChannels(MAX_JOINED_CHANNELS),
    _chanNameMaxLen(CHAN_NAME_MAX_LEN),
    _nicknameMaxLen(NICKNAME_MAX_LEN),
    _codes(),
    _trailings()
{
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    std::string actualFileName = fileName;
#ifdef TEST
    if (fileName == CODES_CONF_FILE)
        actualFileName = CODES_CONF_FILE_FOR_TEST;
    else if (fileName == SERVER_CONF_FILE)
        actualFileName = SERVER_CONF_FILE_FOR_TEST;
#endif
    if (fileName == CODES_CONF_FILE && !_parse_code_file(actualFileName))
        LOG_SERVER.warning("Code file not loaded!");
    else if (!_parse_config_file(actualFileName)) {
        LOG_SERVER.warning("Conf file not loaded!");
    }
}
Config::~Config() {}

bool Config::_parse_code_file(const std::string& fileName)
{
    std::ifstream file(fileName.c_str());
    if (!file.is_open())
        return false;
    std::string line;
    while (std::getline(file, line)) {
        size_t posAssign  = line.find('=');
        size_t posComment = line.find('#');
        size_t posColon   = line.find(':');

        // Skip comments and lines without assignment
        if (posAssign == std::string::npos || posComment != std::string::npos)
            continue;

        std::string keyStr = line.substr(0, posAssign);
        int         code   = std::atoi(keyStr.c_str());

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
    file.close();
    return true;
}

bool Config::_parse_config_file(const std::string& fileName)
{
    std::ifstream file(fileName.c_str());
    if (!file.is_open())
        return false;
    std::string line;
    while (std::getline(file, line)) {
        size_t posAssign  = line.find('=');
        size_t posComment = line.find('#');
        if (posAssign != std::string::npos && posComment == std::string::npos) {
            std::string key   = line.substr(0, posAssign);
            std::string value = line.substr(posAssign + 1);
            _set_key_value(key, value);
        }
    }
    file.close();
    return true;
}

void Config::_set_key_value(const std::string& key, std::string& value)
{
    const size_t nbParam          = 5;
    std::string  keyList[nbParam] = {"server_name", "password", "max_joined_channels", "chan_name_max_len", "nickname_max_len"};
    void (Config::* functions[nbParam])(std::string&) = {&Config::_set_name,
                                                         &Config::_set_password,
                                                         &Config::_set_max_joined_channels,
                                                         &Config::_set_chan_name_max_len,
                                                         &Config::_set_nickname_max_len};
    for (size_t i = 0; i < nbParam; i++) {
        if (key == keyList[i]) {
            (this->*functions[i])(value);
            return;
        }
    }
}

void Config::_set_name(std::string& value) { _name = value; }
void Config::_set_password(std::string& value) { _psswd = value; }
void Config::_set_max_joined_channels(std::string& value) { _maxJoinedChannels = static_cast<int>(atoi(value.c_str())); }
void Config::_set_chan_name_max_len(std::string& value) { _chanNameMaxLen = static_cast<int>(atoi(value.c_str())); }
void Config::_set_nickname_max_len(std::string& value) { _nicknameMaxLen = static_cast<int>(atoi(value.c_str())); }

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
