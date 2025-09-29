#ifndef JOIN_HPP
#define JOIN_HPP

#include "ICommand.hpp"

#include <string>
#include <vector>

class Client;
class Server;

struct ChannelParam {
	std::string channel;
	std::string key;
	bool		isValid;
	ChannelParam(const std::string& a, std::string& b) : channel(a), key(b), isValid(true) {} //to be define in .cpp ???
	ChannelParam(const std::string& a) : channel(a), isValid(true) {}
	ChannelParam(const std::string& a, bool valid) : channel(a), isValid(valid) {}
};

class Join : public ICommand
{
  public:
	Join(const std::vector<ChannelParam>& channelsLst);
	~Join();
	void							 execute(Server& s, Client& c);
	static std::vector<ChannelParam> check_args(Server& s, Client& c, std::string& params);

  private:
	std::vector<ChannelParam> _channelsLst;
	Join();
	Join(const Join& other);
	Join& operator=(const Join& other);
};

#endif
