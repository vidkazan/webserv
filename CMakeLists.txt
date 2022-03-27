cmake_minimum_required(VERSION 3.20)
project(webserv)

set(CMAKE_CXX_STANDARD 98)

include_directories(.)

add_executable(webserv
        CGI.cpp
        parse/errorCodes.hpp
        parse/IParse.hpp
        parse/LocationConfig.hpp
        Response.hpp
        parse/serverConfig.cpp
        Client.hpp
        CGI.hpp
        parse/serverConfig.hpp
        parse/IParse.cpp
        main.hpp
        Request.hpp
        PortServer.hpp
        parse/LocationConfig.cpp
        parse/formatConfigFile.cpp
        AutoIndex.hpp
        main.cpp
        parse/formatConfigFile.hpp
        parse/ListenConfig.cpp
        utils.cpp
        VirtualServerConfig.hpp
        VirtualServerConfigDirectory.hpp)
