#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "main.hpp"

class PortServer;

class VirtualServerConfigDirectory
{
	private:
		std::string _directoryName;
		std::string _directoryAllowedMethods;
		std::string _directoryPath;
		ssize_t		_maxBodySize;
	public:
		VirtualServerConfigDirectory(const std::string&directoryName, const std::string&directoryAllowedMethods,
							  const std::string&directoryPath, ssize_t bodySize) : _directoryName(directoryName),
																				   _directoryAllowedMethods(directoryAllowedMethods),
																				   _directoryPath(directoryPath), _maxBodySize(bodySize){}

		~VirtualServerConfigDirectory(){}

		const std::string&getDirectoryName() const{
			return _directoryName;
		}

		const std::string&getDirectoryAllowedMethods() const{
			return _directoryAllowedMethods;
		}

		const std::string&getDirectoryPath() const{
			return _directoryPath;
		}

		ssize_t getMaxBodySize() const{
			return _maxBodySize;
		}
		unsigned short countSlash(const std::string & str) const
		{

			std::string subStr = str;
			unsigned short count = 0;
			size_t pos = 0;
			while(pos != std::string::npos)
			{
				subStr = subStr.substr(pos + 1, subStr.size() - pos - 1);
				pos = subStr.find('/');
				if (pos != std::string::npos && pos != (subStr.size() - 1))
				{
					count++;
				}
			}
			return count;
		}

		bool operator<(VirtualServerConfigDirectory rhs) const
		{
			std::string subStr;
			unsigned short countThis;
			unsigned short countRhs;
			subStr = _directoryName;
			countThis = countSlash(subStr);
			subStr = rhs.getDirectoryName();
			countRhs = countSlash(subStr);
			if(countThis < countRhs)
				return true;
			else
				return false;
		}
		bool operator>(VirtualServerConfigDirectory rhs) const
		{
			std::string subStr;
			unsigned short countThis;
			unsigned short countRhs;
			subStr = _directoryName;
			countThis = countSlash(subStr);
			subStr = rhs.getDirectoryName();
			countRhs = countSlash(subStr);
			if(countThis > countRhs)
				return true;
			else
				return false;
		}
};

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

class PortServer{
private:
	int _port;
	std::string _ip;
	int _socketFD;
	struct sockaddr_in _adr;
	std::vector<VirtualServerConfig> _virtualServers;
public:
	PortServer(int port, std::string ip): _ip(ip), _port(port){
		_adr.sin_addr.s_addr = 0;
		_adr.sin_family = 0;
		_adr.sin_len = 0;
		_adr.sin_port = 0;

		_socketFD = socket(AF_INET, SOCK_STREAM, 0);
		int opt = 1;
		setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
		_adr.sin_family = AF_INET;
		_adr.sin_port = htons(port);
		_adr.sin_addr.s_addr = inet_addr(_ip.c_str());
	};
	virtual ~PortServer(){};
	int getSocketFD() const {return _socketFD;};
	struct sockaddr_in& getSockAddrInStruct(){return _adr;};
	int getPort(){return _port;}
	std::string getIp(){return _ip;}
	std::vector<VirtualServerConfig> & getVirtualServers(){return _virtualServers;}
	void addVirtualServer(const VirtualServerConfig & config){_virtualServers.push_back(config);}
};

class Webserv2 {
private:
	std::vector<PortServer> _portServers;
	std::vector<Client> _clients;
public:
	Webserv2(){}
	~Webserv2(){}
	void configFileParse(){}
	std::vector<PortServer>& getPortServers(){return _portServers;}
	std::vector<Client>& getClients(){return _clients;}
	void addPortServer(int port, std::string ip)
	{
		PortServer portServer(port, ip);
		_portServers.push_back(portServer);
		if(bind(_portServers.back().getSocketFD(), (struct sockaddr *)&_portServers.back().getSockAddrInStruct(), sizeof(_portServers.back().getSockAddrInStruct())) < 0){
			perror("bind error");
			exit(EXIT_FAILURE);
		}
	}
//	void addClientSocket(int fd,const ServerConfig& config){
//		Client clientSocket(fd, config);
//		_clientSockets.push_back(clientSocket);
//	}
};

class Webserv {
private:
	std::vector<Server> _servSockets;
	std::vector<Client> _clientSockets;
public:
	Webserv();
	~Webserv();
	void configFileParse();
	std::vector<Server>& getServSockets();
	std::vector<Client>& getClientSockets();
	void errorMsg(const char *msg);
	void addServer(const ServerConfig & config);
	void addClientSocket(int fd,const ServerConfig& config);
};
#endif