#pragma once
#include "main.hpp"

class	ServerConfig{
private:
	std::vector<ServerConfigDirectory> _directories;
	short int _port;
	std::string _ip;
	std::string _serverName;
public:
	ServerConfig(){}
	ServerConfig(const std::vector<ServerConfigDirectory>&directories, short port,std::string ip,const std::string serverName)
			: _directories(directories), _port(port), _ip(ip), _serverName(serverName){}
	 ~ServerConfig(){}

	const std::vector<ServerConfigDirectory>&getDirectories() const{return _directories;}
	short getPort() const{return _port;}
	std::string getIp() const{return _ip;}
	std::string getServerName(){return  _serverName;}

};