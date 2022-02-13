#include "main.hpp"


void configFileImitation2(Webserv2 & webserv2){
	// generate port servers

	// generate servers
	std::vector<VirtualServerConfigDirectory> dirs;
	VirtualServerConfigDirectory dir1("/", "GET", "www/",-1);
	dirs.push_back(dir1);
	VirtualServerConfigDirectory dir2("/directory", "GET", "www/YoupiBanane/",-1);
	dirs.push_back(dir2);
	VirtualServerConfigDirectory dir3("/post_body", "POST", "www/post_body",100);
	dirs.push_back(dir3);
	VirtualServerConfigDirectory dir4("/upload", "POST", "www/upload",-1);
	dirs.push_back(dir4);
	VirtualServerConfigDirectory dir5("/files", "GET", "www/upload",-1);
	dirs.push_back(dir5);
	VirtualServerConfigDirectory dir6("/directory/nop", "GET", "www/YoupiBanane/nop/",-1);
	dirs.push_back(dir6);
	VirtualServerConfigDirectory dir7("/delete", "DELETE", "www/upload",-1);
	dirs.push_back(dir7);
	VirtualServerConfigDirectory dir8("/directory/Yeah", "GET", "www/YoupiBanane/Yeah/",-1);
	dirs.push_back(dir8);
	VirtualServerConfigDirectory dir9("/put_test", "PUT", "www/put_test/",-1);
	dirs.push_back(dir9);
	std::sort(dirs.begin(), dirs.end()); //vector must be sorted
	VirtualServerConfig config1(dirs, 2000, "127.0.0.1", "serverName");
	webserv2.addPortServer(2000, "127.0.0.1");
	webserv2.getPortServers()[0].addVirtualServer(config1);
}
//void configFileImitation(Webserv & webserv){
//	// generate port servers
//
//	// generate servers
//	std::vector<ServerConfigDirectory> dirs;
//	ServerConfigDirectory dir1("/", "GET", "www/",-1);
//	dirs.push_back(dir1);
//	ServerConfigDirectory dir2("/directory", "GET", "www/YoupiBanane/",-1);
//	dirs.push_back(dir2);
//	ServerConfigDirectory dir3("/post_body", "POST", "www/post_body",100);
//	dirs.push_back(dir3);
//	ServerConfigDirectory dir4("/upload", "POST", "www/upload",-1);
//	dirs.push_back(dir4);
//	ServerConfigDirectory dir5("/files", "GET", "www/upload",-1);
//	dirs.push_back(dir5);
//	ServerConfigDirectory dir6("/directory/nop", "GET", "www/YoupiBanane/nop/",-1);
//	dirs.push_back(dir6);
//	ServerConfigDirectory dir7("/delete", "DELETE", "www/upload",-1);
//	dirs.push_back(dir7);
//	ServerConfigDirectory dir8("/directory/Yeah", "GET", "www/YoupiBanane/Yeah/",-1);
//	dirs.push_back(dir8);
//	ServerConfigDirectory dir9("/put_test", "PUT", "www/put_test/",-1);
//	dirs.push_back(dir9);
//	std::sort(dirs.begin(), dirs.end()); //vector must be sorted
//	ServerConfig config1(dirs, 2000, "127.0.0.1", "serverName");
//	webserv.addServer(config1);
//}

void startMessage(){
	printLog("", "______________________________________________________________|\n|_________________________SERVER START_________________________|\n|______________________________________________________________", GREEN);
}

int main()
{
//	Webserv webserv;
	Webserv2 webserv2;
	// config file parser is cominggg.....
	configFileImitation2(webserv2);
//	configFileImitation(webserv);
	startMessage();
	// listening listen sockets
//	for(std::vector<Server>::iterator it = webserv.getServSockets().begin();it != webserv.getServSockets().end(); it++)
//		listen(it->getSocketFD(), 1000);
//	// MAIN LOOP
//	while(1)
//	{
//		// preparing for SELECT
//		int largestFD = 0;
//		fd_set readfds, writefds;
//		FD_ZERO(&readfds);
//		FD_ZERO(&writefds);
//		// adding listen sockets to SELECT's read events catching array
//		for(std::vector<Server>::iterator it = webserv.getServSockets().begin();it != webserv.getServSockets().end(); it++)
//		{
//			FD_SET(it->getSocketFD(), &readfds);
//			if(it->getSocketFD() > largestFD)
//				largestFD = it->getSocketFD();
//		}
//		// adding client sockets to SELECT's read/write events catching array
//		for(std::vector<Client>::iterator it = webserv.getClientSockets().begin();it != webserv.getClientSockets().end(); it++)
//		{
//			if(it->getStatus() == READING)
//				FD_SET(it->getSocketFd(), &readfds);
//			else if(it->getStatus() == WRITING)
//				FD_SET(it->getSocketFd(), &writefds);
//			if(it->getSocketFd() > largestFD)
//				largestFD = it->getSocketFd();
//		}
//		// SELECT
////		std::cout << "select:\n";
//		if(select(largestFD + 1, &readfds, &writefds,0,0) < 0)
//		{
//			webserv.errorMsg("webserv: select error");
//			exit(EXIT_FAILURE);
//		}
//		// finding an event in client sockets array
//		for(std::vector<Client>::iterator it = webserv.getClientSockets().begin();it != webserv.getClientSockets().end(); it++)
//		{
//			if(webserv.getClientSockets().empty())
//				break;
//			// finding a read event in client sockets array
//			if (FD_ISSET(it->getSocketFd(), &readfds)){
////				std::cout << "select:"<< YELLOW << " read "<< WHITE << "ready on fd " << it->getSocketFd() << "\n";
//				it->readRequest();
//				if(it->getStatus() == WRITING)
//					it->generateResponse();
//			}
//			// finding a write event in client sockets array
//			if(FD_ISSET(it->getSocketFd(), &writefds))
//			{
////				std::cout << "select:"<< GREEN << " write "<< WHITE << "ready on fd " << it->getSocketFd() << "\n";
//				it->sendResponse();
//			}
//		}
//		// finding a new connection event in listen sockets array
//		int fd;
//		for(std::vector<Server>::iterator it = webserv.getServSockets().begin();it != webserv.getServSockets().end(); it++)
//		{
//			if(FD_ISSET(it->getSocketFD(), &readfds))
//			{
//				// creating new connection/client socket
//				struct sockaddr_in adrAccept;
//				bzero((void *)&adrAccept,sizeof adrAccept);
//				socklen_t adrAcceptLen = sizeof adrAccept;
//				std::cout << "select: new client on port " << ntohs(it->getSockAddrInStruct().sin_port) << "\n";
//				fd = accept(it->getSocketFD(), (struct sockaddr *)&adrAccept, &adrAcceptLen);
//				if (fd < 0){
//					webserv.errorMsg("accept: fd < 0");
//					exit(EXIT_FAILURE);
//				}
//				fcntl(fd, F_SETFL, O_NONBLOCK);
//				webserv.addClientSocket(fd, it->getConfig());
//			}
//		}
//		// checking all connections for closing
//		for(std::vector<Client>::iterator it = webserv.getClientSockets().begin();it != webserv.getClientSockets().end(); it++){
//			if(it->getStatus() == CLOSING){
//				close(it->getSocketFd());
//				webserv.getClientSockets().erase(it);
//				break;
//			}
//		}
//	}
//	// very bad place:)
	return 0;
}