#ifndef MAIN_HPP
#define MAIN_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <vector>
#include <iomanip>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <list>



#define RED "\e[91m"
#define YELLOW "\e[93m"
#define GREEN "\e[92m"
#define WHITE "\e[39m"

#define READING 0
#define WRITING 1
#define CLOSING 2

#define REQUEST_READ_WAITING_FOR_HEADER 10
#define REQUEST_READ_HEADER 11
#define REQUEST_READ_BODY 12
#define REQUEST_READ_CHUNKED 13
#define REQUEST_READ_COMLETE 14

#include "ListenSocketConfigDirectory.hpp"
#include "ListenSocketConfig.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Client.hpp"
#include "ListenSocket.hpp"
#include "Webserv.hpp"

void printLog(char *,char *msg, char *color);

#endif
