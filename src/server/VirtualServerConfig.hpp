#pragma once
#include "../main.hpp"
#include <map>

class VirtualServerConfig{
private:
	std::vector<VirtualServerConfigDirectory> _directories;
	short int _port;
	std::string _ip;
	std::string _serverName;
	std::map<int, std::string> _error_pages;
public:
	VirtualServerConfig(){}
	VirtualServerConfig(const std::vector<VirtualServerConfigDirectory>&directories, \
	short port, \
	std::string ip, \
	const std::string serverName, \
	const std::map<int, std::string> error_pages)
			: _directories(directories), \
			_port(port), \
			_ip(ip), \
			_serverName(serverName), \
			_error_pages(error_pages){}
	~VirtualServerConfig(){}

	const std::vector<VirtualServerConfigDirectory>&getDirectories() const{return _directories;}
	short getPort() const{return _port;}
	std::string getIp() const{return _ip;}
	std::string getServerName(){return  _serverName;}
    std::string& getErrorPages(int code)
    {
        return _error_pages[code];
    }
};

