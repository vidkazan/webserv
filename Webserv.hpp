#pragma once
#include "main.hpp"

class PortServer;

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
	void addClient(int fd, std::vector<VirtualServerConfig> virtualServers){
		Client client(fd, virtualServers);
		_clients.push_back(client);
	}
	void findPortServerForVirtualConfig(VirtualServerConfig & conf)
	{
		for(std::vector<PortServer>::iterator it1 = _portServers.begin();it1 != _portServers.end(); it1++)
		{
			if(it1->getIp() == conf.getIp() && it1->getPort() == conf.getPort())
			{
				bool nameMatch = false;
				for(std::vector<VirtualServerConfig>::iterator it2 = it1->getVirtualServers().begin(); it2 != it1->getVirtualServers().end();it2++){
					if(it2->getServerName() == conf.getServerName()){
						nameMatch = true;
						std::cout << "adding virtual server to port server: name match\n";
						break;
					}
				}
				if(!nameMatch)
					it1->addVirtualServer(conf);
			}
		}
	}
};