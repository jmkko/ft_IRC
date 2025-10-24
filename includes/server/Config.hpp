/**
 * @file Config.hpp
 * @brief Config class
 * @version 0.1
 * @date 2025-10-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "reply_codes.hpp"

#include <cstdlib>
#include <map>
#include <string>
#include <vector>

/**
 * @brief
 * @class Config
 * @details responsabilities
 * - parse config file in toml format
 * - return server info
 * - return RFC codes info
 * - return message of the day
 *
 */
class Config
{
  public:
    /**
     * @brief Construct a new Config object from filename
     *
     * @param fileName
     */
    Config(const std::string& fileName);
    /**
     * @brief Destroy the Config object
     *
     */
    ~Config();

    /**
     * @brief gets literal name of a reply
     * @remark code can be RFC or custom
     * @param code numerical code associated with the reply
     * @return const std::string&
     */
    const std::string& str(ReplyCode code) const;

    /**
     * @brief gets trailing message of a reply
     * @remark code can be RFC or custom
     * @param code numerical code associated with the reply
     * @return const std::string&
     */
    const std::string& trailing(ReplyCode code) const;

    /**
     * @brief Get the server name
     *
     * @return const std::string&
     */
    const std::string& get_name() const;
    /**
     * @brief Get the server password
     * @return const std::string&
     */
    const std::string& get_password() const;
    /**
     * @brief Get the max joined channels
     *
     * @return int
     */
    int get_max_joined_channels() const;
    /**
     * @brief Get the channel name max len
     *
     * @return size_t
     */
    size_t get_chan_name_max_len() const;
    /**
     * @brief Get the nickname max len
     *
     * @return size_t
     */
    size_t get_nickname_max_len() const;
    /**
     * @brief Get the lines forming the message of the day
     *
     * @return std::vector<std::string>
     */
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
    /**
     * @brief Construct a new Config object
     *
     */
    Config();

    Config(const Config& config);

    Config& operator=(const Config& other);

    /**
     * @brief main parsing function
     * @see _parse_conf
     * @see _parse_codes
     * @see _parse_motd
     * @param fileName configuration
     * @return true in case of successful parsing
     * @return false is configuration file could not be opened
     */
    bool _parse_config_file(const std::string& fileName);
    /**
     * @brief parses server configuration
     *
     * @param line
     */
    void _parse_conf(const std::string& line);
    /**
     * @brief parses replies codes
     *
     * @param line
     */
    void _parse_codes(const std::string& line);
    /**
     * @brief parses message of the day
     *
     * @param line
     */
    void _parse_motd(const std::string& line);
    /**
     * @brief updates server configuration map
     *
     * @param key
     * @param value
     */
    void _set_key_value(const std::string& key, std::string& value);
    /**
     * @brief update configuration server name
     *
     * @param value
     */
    void _set_name(std::string& value);
    /**
     * @brief update configuration password
     *
     * @param value
     */
    void _set_password(std::string& value);
    /**
     * @brief update configuration port
     *
     * @param value
     */
    void _set_port(std::string& value);
    /**
     * @brief update configuration max number of channels a user can join
     *
     * @param value
     */
    void _set_max_joined_channels(std::string& value);
    /**
     * @brief update configuration channel name max length
     *
     * @param value
     */
    void _set_chan_name_max_len(std::string& value);
    /**
     * @brief update configuration nickname max length
     *
     * @param value
     */
    void _set_nickname_max_len(std::string& value);
    /**
     * @brief update configuration target limit for a message
     *
     * @param value
     */
    void _set_target_limit(std::string& value);
};
extern const Config ircConfig;
/**
 * @def ircConfigTest
 * @brief creates a configuration with correct configuration file relative path
 *
 */
extern const Config ircConfigTest;

#endif
