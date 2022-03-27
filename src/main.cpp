#include "main.hpp"

void    printWebservData(Webserv2 &webserv2)
{
    //print port servers
    for(std::vector<PortServer>::iterator it = webserv2.getPortServers().begin();it!=webserv2.getPortServers().end();it++){
        std::cout << it->getIp() << " " << it->getPort() << "\n";
        for(std::vector<VirtualServerConfig>::iterator it2 = it->getVirtualServers().begin();it2!=it->getVirtualServers().end();it2++){
            std::cout << " " << it2->getIp() << " " << it2->getPort() << " " << it2->getServerName() <<  "\n";
        }
    }
}

void    setVirtualServerConfig(Webserv2 & webserv2, ServerConfig * sc)
{
	// generate servers
	std::vector<VirtualServerConfigDirectory>	dirs;
	vector<LocationConfig *>					locations = sc->locations;

	vector<LocationConfig *>::iterator 	b = locations.begin();
	vector<LocationConfig *>::iterator 	e = locations.end();

	string directoryName;
	string directoryAllowedMethods;
	string directoryPath;

	while (b != e) {
		dirs.push_back(VirtualServerConfigDirectory((*b)->name,\
							(*b)->allow_methods[0],\
							(*b)->root, \
							"", \
							(*b)->client_body_buffer_size, \
					   		(bool)(*b)->autoindex, \
					   		(*b)->index, \
					   		(*b)->cgi_path, \
					   		(*b)->cgi_extension)); // TODO logic for multimethods
		b++;
	}
	std::sort(dirs.begin(), dirs.end());

    VirtualServerConfig virtualServConfig1(dirs, \
      sc->listen->port[0], \
      (char *)sc->listen->rawIp.c_str(), \
      sc->server_name);
	webserv2.addPortServer(sc->listen->port[0], (char *)sc->listen->rawIp.c_str());
	webserv2.addVirtualServer(virtualServConfig1);
}

void    startMessage(){
	printLog("", "______________________________________________________________|\n"
				 			   "|_________________________SERVER START_________________________|\n"
							   "|______________________________________________________________", GREEN);
}

void    parseConfigFile(Webserv2 & webserv2, int argc, char ** argv) {
	formatConfigFile			a(argc, argv);
//	vector<ServerConfig *>		_servers;
	vector<string> strServers = a.getStringServers();


	vector<string>::iterator b = strServers.begin();
	vector<string>::iterator e = strServers.end();

	while (b != e) {
		ServerConfig * tmp = new ServerConfig (*b);
		setVirtualServerConfig(webserv2, tmp);
		delete tmp;
		b++;
	}
}

int     main(int argc, char ** argv)
{
	Webserv2 webserv2;
	try
	{
		parseConfigFile(webserv2, argc, argv);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		exit(1);
	}
	startMessage();
	// listening listen sockets
	for(std::vector<PortServer>::iterator it = webserv2.getPortServers().begin();it != webserv2.getPortServers().end(); it++)
		listen(it->getSocketFD(), 1000);
	// MAIN LOOP
//    printWebservData(webserv2);
	while(1)
	{
		// preparing for SELECT
		int largestFD = 0;
		fd_set readfds, writefds;
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		// adding listen sockets to SELECT's read events catching array
		for(std::vector<PortServer>::iterator it = webserv2.getPortServers().begin();it != webserv2.getPortServers().end(); it++)
		{
			FD_SET(it->getSocketFD(), &readfds);
			if(it->getSocketFD() > largestFD)
				largestFD = it->getSocketFD();
		}
		// adding client sockets to SELECT's read/write events catching array
		for(std::vector<Client>::iterator it = webserv2.getClients().begin();it != webserv2.getClients().end(); it++)
		{
			if(it->getStatus() == READING)
				FD_SET(it->getSocketFd(), &readfds);
			else if(it->getStatus() == WRITING)
				FD_SET(it->getSocketFd(), &writefds);
			if(it->getSocketFd() > largestFD)
				largestFD = it->getSocketFd();
		}
		// SELECT
//		std::cout << "select:\n";
		if(select(largestFD + 1, &readfds, &writefds,0,0) < 0)
		{
			printLog("","webserv: select error",RED);
			exit(EXIT_FAILURE);
		}
		// checking all connections for closing
		for(std::vector<Client>::iterator it = webserv2.getClients().begin();it != webserv2.getClients().end(); it++){
			if(it->getStatus() == CLOSING)
            {
//                std::cout << SOME << "close: " << it->getSocketFd() << WHITE << "\n";
				close(it->getSocketFd());
				webserv2.getClients().erase(it);
				break;
			}
		}
        // finding an event in client sockets array
        for(std::vector<Client>::iterator it = webserv2.getClients().begin();it != webserv2.getClients().end(); it++)
        {
            if(webserv2.getClients().empty())
                break;
            // finding a read event in client sockets array
            if (FD_ISSET(it->getSocketFd(), &readfds)){
//				std::cout << "select:"<< YELLOW << " read "<< WHITE << "ready on fd " << it->getSocketFd() << "\n";
                it->readRequest();
                if(it->getStatus() == WRITING)
                    it->generateResponse();
                break;
            }
                // finding a write event in client sockets array
            else if(FD_ISSET(it->getSocketFd(), &writefds))
            {
//				std::cout << "select:"<< GREEN << " write "<< WHITE << "ready on fd " << it->getSocketFd() << "\n";
                it->sendResponse();
                break;
            }
        }
        // finding a new connection event in listen sockets array
        int fd;
        for(std::vector<PortServer>::iterator it = webserv2.getPortServers().begin();it != webserv2.getPortServers().end(); it++)
        {
            if(FD_ISSET(it->getSocketFD(), &readfds))
            {
                // creating new connection/client socket
                struct sockaddr_in adrAccept;
                bzero((void *)&adrAccept,sizeof adrAccept);
                socklen_t adrAcceptLen = sizeof adrAccept;
                fd = accept(it->getSocketFD(), (struct sockaddr *)&adrAccept, &adrAcceptLen);
                if (fd < 0){
                    printLog("","accept error",RED);
                    exit(EXIT_FAILURE);
                }
//				std::cout << SOME << "new client: " << fd << WHITE << "\n";
                fcntl(fd, F_SETFL, O_NONBLOCK);
                webserv2.addClient(fd, it->getVirtualServers());
            }
        }
	}
	// very bad place:)
	return 0;
}