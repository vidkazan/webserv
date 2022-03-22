#pragma once
#include "main.hpp"

class PortServer{
private:
	int                                 _port;
	std::string                         _ip;
	int                                 _socketFD;
	struct sockaddr_in                  _adr;
	std::vector<VirtualServerConfig>    _virtualServers;
public:
	                                    PortServer(int port, std::string ip): _ip(ip), _port(port){
		_adr.sin_addr.s_addr = 0;
		_adr.sin_family = 0;
		// _adr.sin_len = 0;
		_adr.sin_port = 0;

		_socketFD = socket(AF_INET, SOCK_STREAM, 0);
		int opt = 1;
		setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
		_adr.sin_family = AF_INET;
		_adr.sin_port = htons(port);
		_adr.sin_addr.s_addr = inet_addr(_ip.c_str());
	};
	virtual                             ~PortServer(){};
	int                                 getSocketFD() const {return _socketFD;};
	struct sockaddr_in                  &getSockAddrInStruct(){return _adr;};
	int                                 getPort(){return _port;}
	std::string                         getIp(){return _ip;}
	std::vector<VirtualServerConfig>    &getVirtualServers(){return _virtualServers;}
	void                                addVirtualServer(const VirtualServerConfig & config){_virtualServers.push_back(config);}
};