#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <cstdlib>
#include <string>
#include <vector>

class Config
{
  public:
	Config();
	Config(const std::string& fileName);
	~Config();

	const std::string& get_name() const;
	const std::string& get_password() const;
	int				   get_max_joined_channels() const;
	size_t			   get_chan_name_max_len() const;
	size_t			   get_nickname_max_len() const;

  private:
	std::string _name;
	std::string _psswd;
	int			_maxJoinedChannels;
	size_t		_chanNameMaxLen;
	size_t		_nicknameMaxLen;

	Config(const Config& config);

	Config& operator=(const Config& other);

	bool _parse_config_file(const std::string& fileName);
	void _set_key_value(const std::string& key, std::string& value);
	void _set_name(std::string& value);
	void _set_password(std::string& value);
	void _set_max_joined_channels(std::string& value);
	void _set_chan_name_max_len(std::string& value);
	void _set_nickname_max_len(std::string& value);
};
extern const Config ircConfig;

#endif
