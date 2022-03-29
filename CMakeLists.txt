cmake_minimum_required(VERSION 3.21)
project(webserv)

set(CMAKE_CXX_STANDARD 98)

add_executable(webserv
        src/cgi/CGI.cpp
        src/cgi/CGI.hpp
        src/http/AutoIndex.hpp
        src/http/Client.hpp
        src/http/ClientRequestAnalyse.cpp
        src/http/ClientRequestParse.cpp
        src/http/ClientRequestRead.cpp
        src/http/ClientResponse.cpp
        src/http/Response.hpp
        src/http/Request.hpp
        src/other/utils.cpp
        src/parse/errorCodes.hpp
        src/parse/formatConfigFile.cpp
        src/parse/formatConfigFile.hpp
        src/parse/IParse.hpp
        src/parse/ListenConfig.cpp
        src/parse/LocationConfig.cpp
        src/parse/LocationConfig.hpp
        src/parse/serverConfig.cpp
        src/server/VirtualServerConfig.hpp
        src/server/PortServer.hpp
        src/server/VirtualServerConfigDirectory.hpp
        src/server/Webserv.hpp
        src/main.cpp
        src/main.hpp
        src/parse/print_util.cpp)
