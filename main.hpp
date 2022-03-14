#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <algorithm>
#include <strings.h>


#define RED "\e[91m"
#define YELLOW "\e[93m"
#define GREEN "\e[92m"
#define WHITE "\e[39m"

#define READING 0
#define WRITING 1
#define CLOSING 2

//#define REQUEST_READ_WAITING_FOR_HEADER 10
//#define REQUEST_READ_HEADER 11
//#define REQUEST_READ_BODY 12
//#define REQUEST_READ_CHUNKED 13
//#define REQUEST_READ_MULTIPART 14
//#define REQUEST_READ_COMPLETE 15
//
//void printLog(std::string description,std::string msg,std::string color);
//#include "VirtualServerConfigDirectory.hpp"
//#include "VirtualServerConfig.hpp"
//#include "Request.hpp"
//#include "Response.hpp"
//#include "Client.hpp"
//#include "PortServer.hpp"
//#include "Webserv.hpp"


void printLog(std::string description,std::string msg,std::string color);
#include "AutoIndex.hpp"
#include "CGI.hpp"
#include "VirtualServerConfigDirectory.hpp"
#include "VirtualServerConfig.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Client.hpp"
#include "PortServer.hpp"
#include "Webserv.hpp"

#include "parse/errorCodes.hpp"
#include "parse/formatConfigFile.hpp"
#include "parse/IParse.hpp"
#include "parse/LocationConfig.hpp"
#include "parse/serverConfig.hpp"
