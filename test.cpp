//
// Created by fcody on 1/29/22.
//
#include <stdlib.h>
#include <unistd.h>
int main(){
	char** env = (char **)malloc(sizeof (char *) * 5);
	env[0] = "REQUEST_METHOD=POST";
	env[1] = "SERVER_PROTOCOL=HTTP/1.1";
	env[2] = "PATH_INFO=youpi.bla";
	char *av[3];
	av[0] = "cgi_tester";
	av[1] = 0;
	av[2] = 0;
	execve(av[0],av,env);
}