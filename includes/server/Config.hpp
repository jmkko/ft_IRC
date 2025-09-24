#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "consts.hpp"
#include "LogManager.hpp"
#include <string>
#include <cstdlib>

class Config
{
  public:
    Config();
    Config(const std::string& fileName);
    ~Config();
    const std::string& get_name() const;
    const std::string& get_psswd() const;
    int                get_maxJoinedChannels() const;
    size_t             get_chanNameMaxLen() const;
    size_t             get_nicknameMaxLen() const;

  private:
    Config(const Config& config);
    Config&     operator=(const Config& other);
    std::string _name;
    std::string _psswd;
    int         _maxJoinedChannels;
    size_t      _chanNameMaxLen;
    size_t      _nicknameMaxLen;
    bool        _parse_config_file(const std::string& fileName);
    void        _set_key_value(const std::string& key, std::string& value);
    void        _set_name(std::string& value);
    void        _set_psswd(std::string& value);
    void        _set_maxJoinedChannels(std::string& value);
    void        _set_chanNameMaxLen(std::string& value);
    void        _set_nicknameMaxLen(std::string& value);
};
extern const Config irc_config;

#endif
