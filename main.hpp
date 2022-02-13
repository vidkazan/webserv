#ifndef MAIN_HPP
#define MAIN_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <cstdio>
#include <iostream>
//#include <cstring>
#include <vector>
//#include <iomanip>
#include <unistd.h>
#include <fcntl.h>
//#include <cstdio>
#include <sstream>
#include <fstream>
//#include <map>
#include <vector>
//#include <list>
#include <sys/stat.h>


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
#define REQUEST_READ_MULTIPART 14
#define REQUEST_READ_COMPLETE 15

void printLog(std::string description,std::string msg,std::string color);
#include "ServerConfigDirectory.hpp"
#include "ServerConfig.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Webserv.hpp"


#endif