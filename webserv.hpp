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
#define WRITEREADY 1

class Client {
private:
	int _socketFD;
	int _isReadyForWrite;
	int _readIsDone;
	std::string* _request;
	std::string* _responce;
public:
	Client(int fd): _socketFD(fd), _isReadyForWrite(0), _readIsDone(0){
		_request = new std::string;
		_responce = new std::string;
	};
	~Client(){};
	std::string* getRequest(){return _request;};
	std::string* getResponce(){return _responce;};
	int getSocketFD(){return _socketFD;};
	int getWriteStatus(){return _isReadyForWrite;}
	int getReadStatus(){return _readIsDone;}

	void setWriteIsReady(){
		_isReadyForWrite = 1;
	};
	void setRequest(std::string* req){
		delete _request;
		_request = req;
	};
	void setResponce(std::string* resp){
		delete _responce;
		_responce = resp;
	};
	void setReadStatusIsDone(){_readIsDone = 1;};
};

class ListenSocket {
private:
	int _socketFD;
	struct sockaddr_in _adr;
public:
	ListenSocket(int portNum, const char *stringIP){
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
	~ListenSocket(){};
	int getSocketFD(){return _socketFD;};
	struct sockaddr_in& getSockAddrInStruct(){return _adr;};
};

class Webserv {
private:
	std::vector<ListenSocket> _servSockets;
	std::vector<Client> _clientSockets;

public:
	Webserv(){};
	~Webserv(){};
	void configFileParce(){};
	std::vector<ListenSocket>& getServSockets(){return _servSockets;};
	std::vector<Client>& getClientSockets(){return _clientSockets;};
	int getCurrentServSocketFDByIndex(int index){
		return _servSockets[index].getSocketFD();
	}
	int getCurrentClientSocketFDByIndex(int index){
		return _clientSockets[index].getSocketFD();
	}
	void errorMsg(const char *msg){write(2, msg, strlen(msg));};
	void addListenSocket(int portNum, char *stringIP){
		ListenSocket socket(portNum, stringIP);
		_servSockets.push_back(socket);
		if(bind(_servSockets.back().getSocketFD(), (struct sockaddr *)&_servSockets.back().getSockAddrInStruct(), sizeof(_servSockets.back().getSockAddrInStruct())) < 0){
			perror("bind error");
			exit(EXIT_FAILURE);
		}
	}
	void addClientSocket(int fd){
		Client clientSocket(fd);
		_clientSockets.push_back(clientSocket);
	}
	void readRequest(std::vector<Client>::iterator it){
		char buf[1000];
		bzero(&buf, 1000);
		int ret = read(it->getSocketFD(), &buf, 1000);
		write(2, YELLOW, 5);
		write(2, &buf, ret);
		write(2, WHITE, 5);
		write(2, "\n", 1);
		std::string *buf2 = new std::string(buf);
		it->setRequest(buf2);
		it->setReadStatusIsDone();
	}
	void generateResponse(std::vector<Client>::iterator it){
		char bufResp[] = "HTTP/1.1 200 OK\n"
						 "Content-Length: 64\n"
						 "Content-Type: text/html\n"
						 "Connection: Closed\n\n"
						 "<html>\n"
						 "<body>\n"
						 "<h1>Hello, World!!!!!!!!!!!!!</h1>\n"
						 "</body>\n"
						 "</html>";
		std::string *resp = new std::string(bufResp);
		it->setResponce(resp);
		std::cout << "write ready on fd: " << it->getSocketFD() << "\n";
		it->setWriteIsReady();
	}
};

#endif