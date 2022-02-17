#pragma once
#include "main.hpp"
#include <algorithm>

class ListenSocket {
private:
	int _socketFD;
	struct sockaddr_in _adr;
	ListenSocketConfig _config;
public:
	ListenSocket(const ListenSocketConfig & config): _config(config){
		_adr.sin_addr.s_addr = 0;
		_adr.sin_family = 0;
		// _adr.sin_len = 0;
		_adr.sin_port = 0;

		_socketFD = socket(AF_INET, SOCK_STREAM, 0);
		int opt = 1;
		setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
		_adr.sin_family = AF_INET;
		_adr.sin_port = htons(_config.getPort());
		_adr.sin_addr.s_addr = inet_addr(_config.getIp());
	};
	~ListenSocket(){};
	int getSocketFD() const {return _socketFD;};
	struct sockaddr_in& getSockAddrInStruct(){return _adr;};

	const ListenSocketConfig&getConfig() const{
		return _config;
	}
};