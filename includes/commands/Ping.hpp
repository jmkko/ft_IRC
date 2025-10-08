#ifndef PING_H
#define PING_H

#include "ICommand.hpp"
#include "reply_codes.hpp"

#include <string>

class Server;
class Client;

class Ping : public ICommand {
    public:
        Ping();
        Ping(const std::string& token);
        virtual ~Ping();

        void			 execute(Server& s, Client& c);
        static ReplyCode check_args(Server& s, Client& c, std::string& params);
        
    private:
        std::string _token;

        Ping(const Ping& other);
        Ping& operator=(const Ping& other);

};

#endif