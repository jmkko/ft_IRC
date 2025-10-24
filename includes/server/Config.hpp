/**
 * @file Config.hpp
 * @brief Config class
 * @version 0.1
 * @date 2025-10-24
 *
 * @copyright Copyright (c) 2025
 */
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "reply_codes.hpp"

#include <cstdlib>
#include <map>
#include <string>
#include <vector>

/**
 * @class Config
 * @brief load conf file which is formated in TOML style
 * there are three parts
 * - [irc.conf] -> if no data load default value
 * - [irc.motd] -> if no data don't send motd
 * - [irc.codes] -> if no data dont-t send trailing message
 *
 */
class Config
{
  public:
    Config();
    Config(const std::string& fileName);
    ~Config();

    /**
     * @brief convert a @see Replycode to his corresponding string
     * ex: 001 -> RPL_WELCOME
     *
     * @param code @see Replycode
     * @return
     */
    const std::string& str(ReplyCode code) const;
    /**
     * @brief get the trailing message of a @see Replycode
     * ex: 001 ->Welcome to the IRC HazADoU& SerVerRrrr
     *
     * @param code @see Replycode
     * @return trailing message
     */
    const std::string& trailing(ReplyCode code) const;
    /**
     * @brief return the name of the server
     */
    const std::string& get_name() const;
    /**
     * @brief return the password of the server
     */
    const std::string& get_password() const;
    /**
     * @brief return the _maxJoinedChannels
     *
     * @return _maxJoinedChannels
     */
    int get_max_joined_channels() const;
    /**
     * @brief return the _chanNameMaxLen
     *
     * @return _chanNameMaxLen
     */
    size_t get_chan_name_max_len() const;
    /**
     * @brief return the _nicknameMaxLen
     *
     * @return _nicknameMaxLen
     */
    size_t get_nickname_max_len() const;
    /**
     * @brief return the motd each line is in a vector of string
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

    Config(const Config& config);

    Config& operator=(const Config& other);

    /**
     * @brief parse the file irc.conf
     *
     * @param fileName
     * @return true on success
     */
    bool _parse_config_file(const std::string& fileName);

    /**
     * @brief get the [irc.conf] part data
     *
     * @param line
     */
    void _parse_conf(const std::string& line);

    /**
     * @brief get the [irc.codes] part data
     *
     * @param line
     */
    void _parse_codes(const std::string& line);

    /**
     * @brief get the [irc.motd] part data
     *
     * @param line
     */
    void _parse_motd(const std::string& line);

    /**
     * @brief select the correct method to load the data of the server
     *
     * @param key var name
     * @param value
     */
    void _set_key_value(const std::string& key, std::string& value);

    /**
     * @brief set the name of the server
     *
     * @param value
     */
    void _set_name(std::string& value);

    /**
     * @brief set the password of the server
     *
     * @param value
     */
    void _set_password(std::string& value);

    /**
     * @brief set the port of the server
     * @remark check if it meets the server conditions
     *
     * @param value
     */
    void _set_port(std::string& value);

    /**
     * @brief set _maxJoinedChannels
     * @remark check if it meets the server conditions
     *
     * @param value
     */
    void _set_max_joined_channels(std::string& value);

    /**
     * @brief set the _chanNameMaxLen
     * @remark check if it meets the server conditions
     *
     * @param value
     */
    void _set_chan_name_max_len(std::string& value);

    /**
     * @brief set the _nicknameMaxLen
     * @remark check if it meets the server conditions
     *
     * @param value
     */
    void _set_nickname_max_len(std::string& value);

    /**
     * @brief set the _targetLimit
     * @remark check if it meets the server conditions
     *
     * @param value
     */
    void _set_target_limit(std::string& value);
};
extern const Config ircConfig;
extern const Config ircConfigTest;

#endif
