#include "Who.hpp"

Who::Who() {}
Who::~Who() {}

Who::Who(const Who& mother) : ICommand() { (void)mother; }

Who& Who::operator=(const Who& other)
{
    (void)other;
    return *this;
}

void Who::execute(Server& s, Client& c)
{
    (void)s;
    (void)c;
}
ReplyCode Who::check_args(Server& s, Client& c, std::string& params)
{
    (void)s;
    (void)c;
    (void)params;
    ReplyCode replycode = RPL_SUCCESS;
    return replycode;
}
