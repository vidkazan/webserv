#pragma once
#include "../main.hpp"

class VirtualServerConfig{
private:
	std::vector<VirtualServerConfigDirectory> _directories;
	short int _port;
	std::string _ip;
	std::string _serverName;
public:
	VirtualServerConfig(){}
	VirtualServerConfig(const std::vector<VirtualServerConfigDirectory>&directories, short port,std::string ip,const std::string serverName)
			: _directories(directories), _port(port), _ip(ip), _serverName(serverName){}
	~VirtualServerConfig(){}

	const std::vector<VirtualServerConfigDirectory>&getDirectories() const{return _directories;}
	short getPort() const{return _port;}
	std::string getIp() const{return _ip;}
	std::string getServerName(){return  _serverName;}
};

