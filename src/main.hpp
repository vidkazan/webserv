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

#define SOME "\e[38m"
#define RED "\e[91m"
#define YELLOW "\e[93m"
#define GREEN "\e[92m"
#define WHITE "\e[39m"

#define READING 0
#define WRITING 1
#define CLOSING 2

#define LOG_FILE_EXTENSION  ".txt"
#define REQUEST_LOG_FILE_PATH_NAME "tmp/log/fullReq_"
#define RESPONSE_LOG_FILE_PATH_NAME "tmp/log/resp_"
#define CGI_OUTPUT_TMP_FILE_PATH_NAME "tmp/tmp_cgi_output_"
#define CGI_INPUT_TMP_FILE_PATH_NAME "tmp/tmp_cgi_input_"
#define SUCCESS_UPLOAD_FILE_PATH_NAME "www/uploadSuccess.html"


#include "http/AutoIndex.hpp"
#include "cgi/CGI.hpp"
#include "server/VirtualServerConfigDirectory.hpp"
#include "server/VirtualServerConfig.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/Client.hpp"
#include "server/PortServer.hpp"
#include "server/Webserv.hpp"
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
