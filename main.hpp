#pragma once
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
//#include <algorithm>
//#include <strings.h>


#define RED "\e[91m"
#define YELLOW "\e[93m"
#define GREEN "\e[92m"
#define WHITE "\e[39m"

#define READING 0
#define WRITING 1
#define CLOSING 2

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


#include "parse/errorCodes.hpp"
#include "parse/formatConfigFile.hpp"
#include "parse/IParse.hpp"
#include "parse/LocationConfig.hpp"
#include "parse/serverConfig.hpp"

void printLog(std::string description,std::string msg,std::string color);
