#pragma once
#include "main.hpp"

class	ListenSocketConfig{
private:
	std::vector<ListenSocketConfigDirectory> _directories;
	short int _port;
	char * _ip;
public:
	ListenSocketConfig(const std::vector<ListenSocketConfigDirectory>&directories, short port, char *ip)
			: _directories(directories), _port(port), _ip(ip){}

	virtual ~ListenSocketConfig(){

	}

	const std::vector<ListenSocketConfigDirectory>&getDirectories() const{
		return _directories;
	}

	short getPort() const{
		return _port;
	}

	char *getIp() const{
		return _ip;
	}

};