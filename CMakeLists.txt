cmake_minimum_required(VERSION 3.21)
project(webserv)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(webserv
        Client.hpp
        ListenSocket.hpp
        ListenSocketConfig.hpp
        ListenSocketConfigDirectory.hpp
        main.cpp
        main.hpp
        Request.hpp
        Response.hpp
        test.cpp
        Webserv.cpp
        Webserv.hpp)
