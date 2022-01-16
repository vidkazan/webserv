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

#define YELLOW "\e[93m"
#define GREEN "\e[92m"
#define WHITE "\e[39m"


class Socket {
private:
	int _socketFD;
	struct sockaddr_in _adr;
public:
	Socket(int portNum, const char *stringIP){
		_adr.sin_addr.s_addr = 0;
		_adr.sin_family = 0;
		_adr.sin_len = 0;
		_adr.sin_port = 0;

		_socketFD = socket(AF_INET, SOCK_STREAM, 0);
		int opt = 1;
		setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
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
	std::vector<Socket> _servSockets;
	std::vector<int> _clientSockets;
	std::vector<int> _writeReadySockets;
public:
	Webserv(){};
	~Webserv(){};
	void configFileParce(){};
	std::vector<Socket>& getServSockets(){return _servSockets;};
	std::vector<int>& getClientSockets(){return _clientSockets;};
	std::vector<int>& getWriteReadySockets(){return _writeReadySockets;};
	int getCurrentServSocketFDByIndex(int index){
		return _servSockets[index].getSocketFD();
	}
	int getCurrentClientSocketFDByIndex(int index){
		return _clientSockets[index];
	}
	int getCurrentWriteReadySocketFDByIndex(int index){
		return _writeReadySockets[index];
	}
	void errorMsg(const char *msg){write(2, msg, strlen(msg));};
	void addListenSocket(int portNum, char *stringIP){
		Socket socket(portNum, stringIP);
		_servSockets.push_back(socket);
		if(bind(_servSockets.back().getSocketFD(), (struct sockaddr *)&_servSockets.back().getSockAddrInStruct(), sizeof(_servSockets.back().getSockAddrInStruct())) < 0){
			perror("bind error");
			exit(EXIT_FAILURE);
		}
	}
	void addClientSocket(int fd){
		_clientSockets.push_back(fd);
	}
	void addWriteReadySocket(int fd){
		_writeReadySockets.push_back(fd);
	}
};

#endif