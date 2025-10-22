#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "reply_codes.hpp"

#include <cstdlib>
#include <map>
#include <string>
#include <vector>

class Config
{
  public:
    Config();
    Config(const std::string& fileName);
    ~Config();

    const std::string&       str(ReplyCode code) const;
    const std::string&       trailing(ReplyCode code) const;
    const std::string&       get_name() const;
    const std::string&       get_password() const;
    int                      get_max_joined_channels() const;
    size_t                   get_chan_name_max_len() const;
    size_t                   get_nickname_max_len() const;
    std::vector<std::string> get_motd() const;

  private:
    std::string                _name;
    std::string                _psswd;
    int                        _port;
    int                        _maxJoinedChannels;
    size_t                     _chanNameMaxLen;
    size_t                     _nicknameMaxLen;
    int                        _targetLimit;
    std::map<int, std::string> _codes;
    std::map<int, std::string> _trailings;
    std::vector<std::string>   _motd;

    Config(const Config& config);

    Config& operator=(const Config& other);

    bool _parse_code_file(const std::string& filename);
    bool _parse_config_file(const std::string& fileName);
    void _parse_conf(const std::string& line);
    void _parse_codes(const std::string& line);
    void _parse_motd(const std::string& line);
    void _set_key_value(const std::string& key, std::string& value);
    void _set_name(std::string& value);
    void _set_password(std::string& value);
    void _set_port(std::string& value);
    void _set_max_joined_channels(std::string& value);
    void _set_chan_name_max_len(std::string& value);
    void _set_nickname_max_len(std::string& value);
    void _set_target_limit(std::string& value);
};
extern const Config ircConfig;
extern const Config ircConfigTest;

#endif
