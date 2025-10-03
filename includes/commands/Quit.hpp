#ifndef QUIT_HPP
#define QUIT_HPP

#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <string>

class Client;
class Server;

class Quit : public ICommand
{
	public:
		explicit Quit(const std::string& params);
		~Quit();

		void			 execute(Server& s, Client& c);
		static ReplyCode check_args(Server& s, Client& c, std::string& params);

	private:
		std::string _quitMsg;

		Quit();
		Quit(const Quit& other);

		Quit& operator=(const Quit& other);

};

#endif