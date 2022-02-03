//
// Created by fcody on 1/29/22.
//
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
int main(){
	pid_t pid;
	char** env = (char **)malloc(sizeof (char *) * 5);
	env[0] = "REQUEST_METHOD=POST";
	env[1] = "SERVER_PROTOCOL=HTTP/1.1";
	env[2] = "PATH_INFO=youpi.bla";
	env[3] = "X-Secret-Header-For-Test=1";
	env[4] = 0;
	char** av = (char **)malloc(sizeof (char *) * 5);
	av[0] = "cgi_tester";
	av[1] = 0;
	av[2] = 0;
	pid = fork();
	if(pid)
	{
		int res;
		res = execve(av[0], av, env);
		std::cout << res << "\n";
		perror("execve: ");
	}
	else
	{
		wait(0);
	}
}