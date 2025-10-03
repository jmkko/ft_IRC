#include "AssertReply.hpp"
#include "LogManager.hpp"
#include "Server.hpp"
#include "TcpSocket.hpp"
#include "fakeClient.hpp"
#include "testUtils.hpp"

void send_valid_password_assert(const TcpSocket& so)
{
    send_line(so, validPassMsg);
    std::string reply = recv_lines(so);
    AssertReply ar(reply);
    ar.is_empty();
}

void send_valid_nick_assert(const TcpSocket& so)
{
    send_line(so, validNickMsg);
    std::string reply = recv_lines(so);
    AssertReply ar(reply);
    ar.is_empty();
}

void join_assert(const TcpSocket& so)
{
	send_line(so, validJoinMsg);
	std::string reply = recv_lines(so);
	AssertReply ar(reply);
	ar.has_code(RPL_ENDOFNAMES);
}

void authenticate(const TcpSocket& so)
{
    send_line(so, validPassMsg);
    send_line(so, validNickMsg);
    send_line(so, validUserMsg);
    recv_lines(so);
}

void authenticate_and_join(const TcpSocket& so)
{
    authenticate(so);
    send_line(so, validJoinMsg);
    recv_lines(so);
}

void authenticate_and_join_op2(const TcpSocket& so)
{
	send_line(so, validPassMsg);
    send_line(so, validNickOp2Msg);
    send_line(so, validUserOp2Msg);
    recv_lines(so);
    send_line(so, validJoinMsg);
    recv_lines(so);
}

void authenticate_second_user(const TcpSocket& so)
{
    send_line(so, validPassMsg);
    send_line(so, validNick2Msg);
    send_line(so, validUser2Msg);
    recv_lines(so);
}

void authenticate_and_join_second_user(const TcpSocket& so)
{
    authenticate_second_user(so);
    send_line(so, validJoinMsg);
    recv_lines(so);
}

void make_op(const TcpSocket& so)
{
    send_line(so, validPassMsg);
    send_line(so, validNickOpMsg);
    send_line(so, validUserOpMsg);
    recv_lines(so);
    send_line(so, validJoinMsg);
    recv_lines(so);
}

void make_two_ops(const TcpSocket& so, const TcpSocket& so2)
{
	make_op(so);
    send_line(so2, validPassMsg);
    send_line(so2, validNickOp2Msg);
    send_line(so2, validUserOp2Msg);
    recv_lines(so);
    send_line(so, validJoinMsg);
    recv_lines(so);
	send_line(so, validModePlusOMsg);
}
