#ifndef PASS_HPP
# define PASS_HPP
# include <iostream>
# include "ICommand.hpp"
class Server;
class Client;

class Pass: public ICommand
{
    public:
        Pass(void);
        Pass(const std::string& pass);
        Pass(const Pass& other);
        Pass &operator=(const Pass &other);
        ~Pass();

		void		execute(Server&, Client&);
		static int	checkArgs(Server&, Client&, std::string&);
	private:
		std::string _pass;
};

#endif

