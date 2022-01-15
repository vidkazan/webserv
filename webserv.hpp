#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include "gnl/get_next_line.hpp"
#include <iostream>
#include <cstring>
#include <vector>


class Socket {
private:
	int _socketFD;
	struct sockaddr_in _adr;
public:
	Socket(int portNum, const char *stringIP){ // zeroing _adr??
		_adr.sin_addr.s_addr = 0;
		_adr.sin_family = 0;
		_adr.sin_len = 0;
		_adr.sin_port = 0;

		_socketFD = socket(AF_INET, SOCK_STREAM, 0);
		_adr.sin_family = AF_INET;
		_adr.sin_port = htons(portNum);
		_adr.sin_addr.s_addr = inet_addr(stringIP);
	};
	~Socket(){};
	int getSocketFD(){return _socketFD;};
	struct sockaddr_in& getSockAddrInStruct(){return _adr;};
};

class Webserv {
private:
	std::vector<Socket *> _servSockets;
public:
	Webserv(){};
	~Webserv(){};
	void configFileParce(int portNum, char *stringIP){ // TODO #001
		Socket* socket = new Socket(portNum, stringIP);
		_servSockets.push_back(socket);
		bind(this->getCurrentServSocketFDByIndex(0), (struct sockaddr *)&_servSockets[0]->getSockAddrInStruct(), sizeof(_servSockets[0]->getSockAddrInStruct()));
	};
	std::vector<Socket *>& getServSockets(){return _servSockets;};
	int getCurrentServSocketFDByIndex(int index){
		return _servSockets[index]->getSocketFD();
	}
	void errorMsg(const char *msg){write(2, msg, strlen(msg));};

};

#endif