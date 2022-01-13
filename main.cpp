#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include "gnl/get_next_line.hpp"

int main(){
	int listenSocket;
	int resBind;
	int fd;
	struct sockaddr_in adrAccept = {0};
	struct sockaddr_in adr = {0};
	size_t readLen;
	int lineRes;
	char buf[10000];




	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	adr.sin_family = AF_INET;
	adr.sin_port = htons(2001);
	adr.sin_addr.s_addr = inet_addr("127.0.0.1");
	resBind = bind(listenSocket, (struct sockaddr *)&adr, sizeof(adr));
	if(resBind == -1){
		perror("bind fail");
		return 1;
	}
	listen(listenSocket, 2);
	socklen_t adrAcceptLen = sizeof adrAccept;
	fd = accept(listenSocket, (struct sockaddr *)&adrAccept, &adrAcceptLen);
	read(fd, &buf ,10000);
	printf("%s\n", buf);
	write(fd, &buf, ft_strlen(buf));
	return 0;
}
