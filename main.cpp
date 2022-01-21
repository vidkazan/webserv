#include "Webserv.hpp"


int main()
{
	Webserv webserv;
	for(int i = 2000; i < 2003; i++)
		webserv.addListenSocket(i,"127.0.0.1");
	printLog(nullptr, "______________________________________________________________|\n|_________________________SERVER START_________________________|\n|______________________________________________________________", GREEN);
	for(std::vector<ListenSocket>::iterator it = webserv.getServSockets().begin();it != webserv.getServSockets().end(); it++)
		listen(it->getSocketFD(), 1000);



	while(1)
	{
		int largestFD = 0;
		fd_set readfds, writefds;
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		for(std::vector<ListenSocket>::iterator it = webserv.getServSockets().begin();it != webserv.getServSockets().end(); it++)
		{
			FD_SET(it->getSocketFD(), &readfds);
			if(it->getSocketFD() > largestFD)
				largestFD = it->getSocketFD();
		}
		for(std::vector<Client>::iterator it = webserv.getClientSockets().begin();it != webserv.getClientSockets().end(); it++)
		{
			if(it->getStatus() == READING || it->getStatus() == READING_DONE)
				FD_SET(it->getSocketFd(), &readfds);
			else if(it->getStatus() == WRITING)
				FD_SET(it->getSocketFd(), &writefds);
			if(it->getSocketFd() > largestFD)
				largestFD = it->getSocketFd();
		}
		std::cout << "select:\n";
		if(select(largestFD + 1, &readfds, &writefds,0,0) < 0)
		{
			webserv.errorMsg("webserv: select error");
			exit(EXIT_FAILURE);
		}
		for(std::vector<Client>::iterator it = webserv.getClientSockets().begin();it != webserv.getClientSockets().end(); it++)
		{
			if(webserv.getClientSockets().empty())
				break;
			if (FD_ISSET(it->getSocketFd(), &readfds)){
				std::cout << "select:"<< YELLOW << " read "<< WHITE << "ready on fd " << it->getSocketFd() << "\n";
				it->readRequest();
				if(it->getStatus() == READING_DONE) {
//					webserv.analyseRequest(it);
					it->generateResponse();
				}
			}
			if(FD_ISSET(it->getSocketFd(), &writefds))
			{
				std::cout << "select:"<< GREEN << " write "<< WHITE << "ready on fd " << it->getSocketFd() << "\n";
				printLog(nullptr,(char *)it->getResponse().getResponse().c_str(), GREEN);
				ssize_t sendRes = write(it->getSocketFd(), it->getResponse().getResponse().c_str(), it->getResponse().getResponse().size());
				if(sendRes <= 0) {
					it->setStatus(CLOSING);
					break;
				}
				it->setStatus(READING);
			}
		}
		int fd;
		for(std::vector<ListenSocket>::iterator it = webserv.getServSockets().begin();it != webserv.getServSockets().end(); it++)
		{
			if(FD_ISSET(it->getSocketFD(), &readfds))
			{
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
		for(std::vector<Client>::iterator it = webserv.getClientSockets().begin();it != webserv.getClientSockets().end(); it++){
			if(it->getStatus() == CLOSING){
				close(it->getSocketFd());
				webserv.getClientSockets().erase(it);
				break;
			}
		}
	}
	return 0;
}
