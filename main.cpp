#include "Webserv.hpp"


int main()
{
	Webserv webserv;
	// config file parser is cominggg.....
	// making listen sockets
	for(int i = 2000; i < 2003; i++)
		webserv.addListenSocket(i,"127.0.0.1");
	printLog(nullptr, "______________________________________________________________|\n|_________________________SERVER START_________________________|\n|______________________________________________________________", GREEN);
	// listening listen sockets
	for(std::vector<ListenSocket>::iterator it = webserv.getServSockets().begin();it != webserv.getServSockets().end(); it++)
		listen(it->getSocketFD(), 1000);
	// MAIN LOOP
	while(1)
	{
		// preparing for SELECT
		int largestFD = 0;
		fd_set readfds, writefds;
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		// adding listen sockets to SELECT's read events catching array
		for(std::vector<ListenSocket>::iterator it = webserv.getServSockets().begin();it != webserv.getServSockets().end(); it++)
		{
			FD_SET(it->getSocketFD(), &readfds);
			if(it->getSocketFD() > largestFD)
				largestFD = it->getSocketFD();
		}
		// adding client sockets to SELECT's read/write events catching array
		for(std::vector<Client>::iterator it = webserv.getClientSockets().begin();it != webserv.getClientSockets().end(); it++)
		{
			if(it->getStatus() == READING || it->getStatus() == READING_DONE)
				FD_SET(it->getSocketFd(), &readfds);
			else if(it->getStatus() == WRITING)
				FD_SET(it->getSocketFd(), &writefds);
			if(it->getSocketFd() > largestFD)
				largestFD = it->getSocketFd();
		}
		// SELECT
		std::cout << "select:\n";
		if(select(largestFD + 1, &readfds, &writefds,0,0) < 0)
		{
			webserv.errorMsg("webserv: select error");
			exit(EXIT_FAILURE);
		}
		// finding an event in client sockets array
		for(std::vector<Client>::iterator it = webserv.getClientSockets().begin();it != webserv.getClientSockets().end(); it++)
		{
			if(webserv.getClientSockets().empty())
				break;
			// finding a read event in client sockets array
			if (FD_ISSET(it->getSocketFd(), &readfds)){
				std::cout << "select:"<< YELLOW << " read "<< WHITE << "ready on fd " << it->getSocketFd() << "\n";
				it->readRequest();
				if(it->getStatus() == READING_DONE) {
//					webserv.analyseRequest(it);
					it->generateResponse();
				}
			}
			// finding a write event in client sockets array
			if(FD_ISSET(it->getSocketFd(), &writefds))
			{
				std::cout << "select:"<< GREEN << " write "<< WHITE << "ready on fd " << it->getSocketFd() << "\n";
				printLog(nullptr,(char *)it->getResponse().getResponse().c_str(), GREEN);
				ssize_t sendRes = write(it->getSocketFd(), it->getResponse().getResponse().c_str(), it->getResponse().getResponse().size());
				if(sendRes <= 0) {
					it->setStatus(CLOSING);
					break;
				}
				Response response;
				it->setResponse(response);
				it->setStatus(READING);
			}
		}
		// finding a new connection event in listen sockets array
		int fd;
		for(std::vector<ListenSocket>::iterator it = webserv.getServSockets().begin();it != webserv.getServSockets().end(); it++)
		{
			if(FD_ISSET(it->getSocketFD(), &readfds))
			{
				// creating new connection/client socket
				struct sockaddr_in adrAccept;
				bzero((void *)&adrAccept,sizeof adrAccept);
				socklen_t adrAcceptLen = sizeof adrAccept;
				std::cout << "select: new client on port " << ntohs(it->getSockAddrInStruct().sin_port) << "\n";
				fd = accept(it->getSocketFD(), (struct sockaddr *)&adrAccept, &adrAcceptLen);
				if (fd < 0){
					webserv.errorMsg("accept: fd < 0");
					exit(EXIT_FAILURE);
				}
				fcntl(fd, F_SETFL, O_NONBLOCK);
				webserv.addClientSocket(fd);
			}
		}
		// checking all connections for closing
		for(std::vector<Client>::iterator it = webserv.getClientSockets().begin();it != webserv.getClientSockets().end(); it++){
			if(it->getStatus() == CLOSING){
				close(it->getSocketFd());
				webserv.getClientSockets().erase(it);
				break;
			}
		}
	}
	// very bad place:)
	return 0;
}
