#include "Nick.hpp"
#include "Client.hpp"
#include "LogManager.hpp"
// Default constructor
Nick::Nick(void) {}

Nick::Nick(const std::string& nickname) : _nickname(nickname) {}

// Copy constructor
Nick::Nick(const Nick& other) { (void)other; }

// Assignment operator overload
Nick& Nick::operator=(const Nick& other)
{
    (void)other;
    return (*this);
}

// Destructor
Nick::~Nick(void) {}

void Nick::execute(Server& server, Client& client)
{
    (void)server;
    LOG_CMD.info("Execution of NICK command");
    client.setNickName(_nickname);
}
