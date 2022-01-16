#include "webserv.hpp"

int main()
{
	Webserv webserv;
	for(int i = 2000; i < 2003; i++)
		webserv.addListenSocket(i,"127.0.0.1");
	std::cout << "server start\n";
	for(std::vector<Socket>::iterator it = webserv.getServSockets().begin();it != webserv.getServSockets().end(); it++)
		listen(it->getSocketFD(), 1000);
	for(;;)
	{
		int largestFD = 0;
		fd_set readfds, writefds;
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		for(std::vector<Socket>::iterator it = webserv.getServSockets().begin();it != webserv.getServSockets().end(); it++)
		{
			FD_SET(it->getSocketFD(), &readfds);
			if(it->getSocketFD() > largestFD)
				largestFD = it->getSocketFD();
		}
		for(std::vector<int>::iterator it = webserv.getClientSockets().begin();it != webserv.getClientSockets().end(); it++)
		{
			FD_SET(*it, &readfds);
			FD_SET(*it, &writefds);
			if(*it > largestFD)
				largestFD = *it;
		}
		std::cout << "select: waiting for connection " <<"\n";
		if(select(largestFD + 1, &readfds, 0,0,0) < 0)
		{
			webserv.errorMsg("webserv: select error");
			exit(EXIT_FAILURE);
		}
		int fd;
		for(std::vector<Socket>::iterator it = webserv.getServSockets().begin();it != webserv.getServSockets().end(); it++)
		{
			std::cout << "isset check ld: " << it->getSocketFD() << "\n";
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
				std::cout << "add client socket fd: " << fd << "\n";
				webserv.addClientSocket(fd);
			}
		}
		for(std::vector<int>::iterator it = webserv.getClientSockets().begin();it != webserv.getClientSockets().end(); it++)
		{
			if(webserv.getClientSockets().empty())
				break;
			std::cout << "isset check fd: " << *it << "\n";
			if (FD_ISSET(*it, &readfds)){
				std::cout << "select: read ready on fd " << *it << "\n";
				char buf[100000];
				read(*it, &buf, 100000);
				write(2, YELLOW, 5);
				write(2, &buf, ft_strlen(buf));
				write(2, WHITE, 5);
				write(2, "\n", 1);
			}
			if(FD_ISSET(*it, &writefds))
			{
				std::cout << "select: write ready on fd " << *it << "\n";
				char bufResp[] = "HTTP/1.1 200 OK\n"
								 "Content-Length: 64\n"
								 "Content-Type: text/html\n"
								 "Connection: Closed\n\n"
								 "<html>\n"
								 "<body>\n"
								 "<h1>Hello, World!!!!!!!!!!!!!</h1>\n"
								 "</body>\n"
								 "</html>";
				write(2, GREEN,5);
				write(2, &bufResp, ft_strlen(bufResp));
				write(2, WHITE,5);
				write(2, "\n", 1);
				write(*it, &bufResp, ft_strlen(bufResp));
				close(*it);
				std::cout << "close: " << *it << "\n";
				webserv.getClientSockets().erase(it);
			}
		}
	}
	return 0;
}
