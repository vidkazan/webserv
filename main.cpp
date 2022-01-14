#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include "gnl/get_next_line.hpp"
#include <iostream>

int main(){
	int sock;
	int fd;
	struct sockaddr_in adrAccept = {0};
	struct sockaddr_in adr = {0};
	char buf[100000];


	sock = socket(AF_INET, SOCK_STREAM, 0);
	adr.sin_family = AF_INET;
	adr.sin_port = htons(2000);
	adr.sin_addr.s_addr = inet_addr("127.0.0.1");
	bind(sock, (struct sockaddr *)&adr, sizeof(adr));
	std::cout << "server start\n";
	listen(sock, 0);
	while(1)
	{
		std::cout << "loop start\n";
		fd_set fd_in;
		FD_ZERO(&fd_in);
		FD_SET(sock, &fd_in);
		int largestFD = sock;
		std::cout << "select: waiting for connection\n";

		select(largestFD + 1, &fd_in, 0,0,0);

		std::cout << "select: event caught...\n";
		socklen_t adrAcceptLen = sizeof adrAccept;
		std::cout << "ready to accept...\n";
		fd = accept(sock, (struct sockaddr *) &adrAccept, &adrAcceptLen);
		fcntl(fd, F_SETFL, O_NONBLOCK);
		std::cout << "accept: done\n";
		read(fd, &buf, 10000);
		printf("%s\n", buf);
		char bufResp[] = "HTTP/1.1 200 OK\n"
						 "Content-Length: 64\n"
						 "Content-Type: text/html\n"
						 "Connection: Closed\n\n"
						 "<html>\n"
						 "<body>\n"
						 "<h1>Hello, World!!!!!!!!!!!!!</h1>\n"
						 "</body>\n"
						 "</html>";
		write(fd, &bufResp, ft_strlen(bufResp));
//		send(fd, &bufResp, ft_strlen(bufResp), 0);
		std::cout << "loop end\n";
	}
	return 0;
}
