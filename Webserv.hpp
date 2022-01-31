#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "main.hpp"

class Webserv {
private:
	std::vector<ListenSocket> _servSockets;
	std::vector<Client> _clientSockets;
public:
	Webserv();
	~Webserv();
	void configFileParse();
	std::vector<ListenSocket>& getServSockets();
	std::vector<Client>& getClientSockets();
	void errorMsg(const char *msg);
	void addListenSocket(const ListenSocketConfig & config);
	void addClientSocket(int fd,const ListenSocketConfig& config);
};
#endif