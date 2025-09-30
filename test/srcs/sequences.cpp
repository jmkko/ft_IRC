#include "AssertReply.hpp"
#include "Server.hpp"
#include "fakeClient.hpp"
#include "testUtils.hpp"

void send_valid_password_assert(Socket so)
{
    send_line(so, validPassMsg);
    std::string reply = recv_line(so);
    AssertReply ar(reply);
    ar.is_empty();
}

void send_valid_nick_assert(Socket so)
{
    send_line(so, validNickMsg);
    std::string reply = recv_line(so);
    AssertReply ar(reply);
    ar.is_empty();
}

void authenticate(Socket so)
{
    send_line(so, validPassMsg);
    send_line(so, validNickMsg);
    send_line(so, validUserMsg);
	recv_line(so);
}

void authenticate_second_user(Socket so)
{
    send_line(so, validPassMsg);
    send_line(so, validNick2Msg);
    send_line(so, validUser2Msg);
	recv_line(so);
}

void make_op(Socket so)
{
    send_line(so, validPassMsg);
    send_line(so, validNickOpMsg);
    send_line(so, validUserOpMsg);
	recv_line(so);
    send_line(so, validJoinMsg);
	recv_line(so);
}
