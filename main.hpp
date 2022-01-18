#ifndef MAIN_HPP
#define MAIN_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include "gnl/get_next_line.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <iomanip>

std::vector<std::string> split(std::string text,const std::string & space_delimiter);
void printLog(char *,char *msg, char *color);

#endif