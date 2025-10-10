#ifndef TEST_LIST_HPP
#define TEST_LIST_HPP

class Server;

void test_join(Server& s);
void test_kick(Server& s);
void test_mode(Server& s);
void test_nick(Server& s);
void test_who(Server& s);
void test_privmsg(Server& s);
void test_ping(Server& s);
void test_topic(Server& s);

#endif
