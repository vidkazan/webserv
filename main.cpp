#include "webserv.hpp"

int main()
{
	Webserv webserv;
	for(int i = 2000; i < 2003; i++)
		webserv.addListenSocket(i,"127.0.0.1");
	std::cout << "server start\n";
	for(std::vector<ListenSocket>::iterator it = webserv.getServSockets().begin();it != webserv.getServSockets().end(); it++)
		listen(it->getSocketFD(), 1000);
	for(;;)
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
		std::cout << "select: waiting for connection " <<"\n";
		if(select(largestFD + 1, &readfds, &writefds,0,0) < 0)
		{
			webserv.errorMsg("webserv: select error");
			exit(EXIT_FAILURE);
		}
		for(std::vector<Client>::iterator it = webserv.getClientSockets().begin();it != webserv.getClientSockets().end(); it++)
		{
			if(webserv.getClientSockets().empty())
				break;
//			std::cout << "isset check fd: " << it->getSocketFD() << "\n";
			if (FD_ISSET(it->getSocketFD(), &readfds)){
				std::cout << "select:"<< YELLOW << " read "<< WHITE << "ready on fd " << it->getSocketFD() << "\n";
				webserv.readRequest(it);
				if(it->getStatus() == WRITING)
				{
					webserv.generateResponse(it);
				}
			}
			if(FD_ISSET(it->getSocketFD(), &writefds))
			{
				std::cout << "select:"<< GREEN << " write "<< WHITE << "ready on fd " << it->getSocketFD() << "\n";
//				char * newstr = it->getResponce()->c_str();
				write(2, GREEN,5);
				write(2, it->getResponce().c_str(), it->getResponce().size());
				write(2, WHITE,5);
				write(2, "\n", 1);
				write(it->getSocketFD(), it->getResponce().c_str(), it->getResponce().size());
//				close(it->getSocketFD());
				std::cout << "close fd: " << it->getSocketFD() << "\n";
				it->setStatus(READING);
			}
		}
		int fd;
		for(std::vector<ListenSocket>::iterator it = webserv.getServSockets().begin();it != webserv.getServSockets().end(); it++)
		{
//			std::cout << "isset check ld: " << it->getSocketFD() << "\n";
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
	}
	return 0;
}
