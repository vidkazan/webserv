cmake_minimum_required(VERSION 3.21)
project(webserv)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(webserv
        AutoIndex.hpp
        Client.hpp
        ListenSocket.hpp
        main.cpp
        main.hpp
        PortServer.hpp
        Request.hpp
        Response.hpp
        utils.cpp
        VirtualServerConfig.hpp
        VirtualServerConfigDirectory.hpp
        Webserv.hpp
        parse/formatConfigFile.hpp
        parse/errorCodes.hpp
        parse/formatConfigFile.cpp
        parse/IParse.hpp
        parse/ListenConfig.cpp
        parse/LocationConfig.cpp
        parse/LocationConfig.hpp
        parse/serverConfig.cpp
        parse/serverConfig.hpp
        parse/tools.cpp)
