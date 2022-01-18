#include "Webserv.hpp"

int main()
{
	Webserv webserv;
	for(int i = 2000; i < 2003; i++)
		webserv.addListenSocket(i,"127.0.0.1");
	std::cout << "server start\n";
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
			if(it->getStatus() == READING)
				FD_SET(it->getSocketFD(), &readfds);
			else if(it->getStatus() == WRITING)
				FD_SET(it->getSocketFD(), &writefds);
			if(it->getSocketFD() > largestFD)
				largestFD = it->getSocketFD();
		}
		std::cout << "select: listening... " <<"\n";
		if(select(largestFD + 1, &readfds, &writefds,0,0) < 0)
		{
			webserv.errorMsg("webserv: select error");
			exit(EXIT_FAILURE);
		}
		for(std::vector<Client>::iterator it = webserv.getClientSockets().begin();it != webserv.getClientSockets().end(); it++)
		{
			if(webserv.getClientSockets().empty())
				break;
			if (FD_ISSET(it->getSocketFD(), &readfds)){
				std::cout << "select:"<< YELLOW << " read "<< WHITE << "ready on fd " << it->getSocketFD() << "\n";
				webserv.readRequest(it);
				if(it->getStatus() == READING_DONE)
					webserv.generateResponse(it);
			}
			if(FD_ISSET(it->getSocketFD(), &writefds))
			{
				std::cout << "select:"<< GREEN << " write "<< WHITE << "ready on fd " << it->getSocketFD() << "\n";
				printLog(nullptr,(char *)it->getResponce().c_str(), GREEN);
				ssize_t sendRes = send(it->getSocketFD(), it->getResponce().c_str(), it->getResponce().size(), 0);
				if(sendRes <= 0) {
					it->setStatus(CLOSING);
					break;
				}
				it->setStatus(READING);
				it->resetResponseData();
			}
		}
		int fd;
		for(std::vector<ListenSocket>::iterator it = webserv.getServSockets().begin();it != webserv.getServSockets().end(); it++)
		{
			if(FD_ISSET(it->getSocketFD(), &readfds))
			{
				struct sockaddr_in adrAccept = {0};
				socklen_t adrAcceptLen = sizeof adrAccept;
				std::cout << "select: new connection on ld " << it->getSocketFD() << "\n";
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
				close(it->getSocketFD());
				webserv.getClientSockets().erase(it);
				break;
			}
		}
	}
	return 0;
}
