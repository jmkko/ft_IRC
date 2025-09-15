#pragma once

#include "TcpSocket.hpp"
#include "Channel.hpp"
#include "consts.hpp"

#include <string>
#include <iostream>
#include <map>

enum ClientStatus
{
	UNAUTHENTICATED,		// before anything
	REGISTERED,				// after PASS + NICK + USER
};

class Client
{
	public:
		Client(Socket socket, sockaddr_in addr);
		Client(const Client& other);
		virtual ~Client();

		Client& operator=(const Client& other);

		Socket              getSocket() const;
		const std::string&  getAddress() const;
		unsigned short      getPort() const;
		std::string         getSendBuffer() const;
		std::string         getReceiveBuffer() const;
		bool                hasDataToSend() const;

		std::string         getNickName() const;
		std::string         getUserName() const;
		std::string         getRealName() const;
		ClientStatus        getStatus() const;

		bool				isRegistered() const;
		int					getNbJoinedChannels() const;

		void				setNickName(const std::string& nick);
		void				setUserName(const std::string& userName);
		void				setRealName(const std::string& realName);
		void				setStatus(ClientStatus status);
		void				addJoinedChannel(Channel& channel);
	  	void				removeJoinedChannel(Channel& channel);
		void				setSendBuffer(const std::string& buffer);

		void				appendToSendBuffer(const std::string& msg);
		void				appendToReceiveBuffer(const std::string& msg);

	private:
		TcpSocket						_socket;
		sockaddr_in						_addr;
		std::string						_nickName;
		std::string						_userName;
		std::string						_realName;
		ClientStatus					_status;
		std::string						_sendBuffer;
		std::string						_receiveBuffer;
		std::map<std::string, Channel*> _joinedChannels;
};

std::ostream&	operator<<(std::ostream& os, const Client& c);
