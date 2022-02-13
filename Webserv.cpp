#include "Webserv.hpp"

	Webserv::Webserv(){}
	Webserv::~Webserv(){}
	void Webserv::configFileParse(){}
	std::vector<Server>& Webserv::getServSockets(){return _servSockets;}
	std::vector<Client>& Webserv::getClientSockets(){return _clientSockets;}
	void Webserv::errorMsg(const char *msg){write(2, msg, strlen(msg));}
	void Webserv::addServer(const ServerConfig & config){
		Server socket(config);
		_servSockets.push_back(socket);
		if(bind(_servSockets.back().getSocketFD(), (struct sockaddr *)&_servSockets.back().getSockAddrInStruct(), sizeof(_servSockets.back().getSockAddrInStruct())) < 0){
			perror("bind error");
			exit(EXIT_FAILURE);
		}
	}
	void Webserv::addClientSocket(int fd,const ServerConfig& config){
		Client clientSocket(fd, config);
		_clientSockets.push_back(clientSocket);
	}