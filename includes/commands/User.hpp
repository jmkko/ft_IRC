#ifndef USER_HPP
# define USER_HPP
# include <iostream>
# include "ICommand.hpp"

class Server;
class Client;

class User: public ICommand
{
    public:
        User(void);
        User(const std::string&, const std::string&);
        User(const User& other);
        User &operator=(const User &other);
        ~User();

		void	execute(Server&, Client&);
		static int checkArgs(Server&, Client&, std::string&);
	private:
		std::string _username;
		std::string _realname;

};

#endif

