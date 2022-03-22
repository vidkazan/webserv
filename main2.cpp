#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <sstream>
#include <fstream>
#include <vector>

void exec()
{
    char **env = (char **)malloc(sizeof(char*) * 4);
    env[0] = (char*)"REQUEST_METHOD=POST";
    env[1] = (char*)"SERVER_PROTOCOL=HTTP/1.1";
    env[2] = (char*)"PATH_INFO=/Users/fcody/Desktop/webserv/";
    env[3] = 0;
    char **argv = (char **)malloc(sizeof(char*) * 2);
    argv[0] = (char*)"cgi_tester";
    argv[1] = 0;
    pid_t pid = fork();
    if (pid == 0)
    {
        if (execve(argv[0], argv, env))
            strerror(errno);
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);
    }
}
int main()
{
    exec();
}