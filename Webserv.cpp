#include "Webserv.hpp"

	Webserv::Webserv(){}
	Webserv::~Webserv(){}
	void Webserv::configFileParse(){}
	std::vector<ListenSocket>& Webserv::getServSockets(){return _servSockets;}
	std::vector<Client>& Webserv::getClientSockets(){return _clientSockets;}
	void Webserv::errorMsg(const char *msg){write(2, msg, strlen(msg));}
	void Webserv::addListenSocket(const ListenSocketConfig & config){
		ListenSocket socket(config);
		_servSockets.push_back(socket);
		if(bind(_servSockets.back().getSocketFD(), (struct sockaddr *)&_servSockets.back().getSockAddrInStruct(), sizeof(_servSockets.back().getSockAddrInStruct())) < 0){
			perror("bind error");
			exit(EXIT_FAILURE);
		}
	}
	void Webserv::addClientSocket(int fd,const ListenSocketConfig& config){
		Client clientSocket(fd, config);
		_clientSockets.push_back(clientSocket);
	}