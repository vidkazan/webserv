#include "webserv.hpp"

int main(){
	Webserv webserv;
	webserv.configFileParce(2000, "127.0.0.1");
	char buf[100000];
	std::cout << "server start\n";
	listen(webserv.getCurrentServSocketFDByIndex(0), 0);
	while(1)
	{
		struct sockaddr_in adrAccept = {0};
		int fd;
		std::cout << "loop start\n";
		fd_set fd_in;
		FD_ZERO(&fd_in);
		FD_SET(webserv.getCurrentServSocketFDByIndex(0), &fd_in);
		int largestFD = webserv.getCurrentServSocketFDByIndex(0);
		std::cout << "select: waiting for connection\n";

		if(select(largestFD + 1, &fd_in, 0,0,0) < 0)
		{
			webserv.errorMsg("webserv: select error");
			exit(EXIT_FAILURE);
		}
		std::cout << "select: event caught...\n";
		socklen_t adrAcceptLen = sizeof adrAccept;
		std::cout << "ready to accept...\n";
		fd = accept(webserv.getCurrentServSocketFDByIndex(0), (struct sockaddr *) &adrAccept, &adrAcceptLen);
		fcntl(fd, F_SETFL, O_NONBLOCK);
		std::cout << "accept: done\n";
		read(fd, &buf, 10000);
		write(2, &buf, ft_strlen(buf));
		char bufResp[] = "HTTP/1.1 200 OK\n"
						 "Content-Length: 64\n"
						 "Content-Type: text/html\n"
						 "Connection: Closed\n\n"
						 "<html>\n"
						 "<body>\n"
						 "<h1>Hello, World!!!!!!!!!!!!!</h1>\n"
						 "</body>\n"
						 "</html>";
		write(2, &bufResp, ft_strlen(bufResp));
		write(fd, &bufResp, ft_strlen(bufResp));
		std::cout << "loop end\n";
	}
	return 0;
}
