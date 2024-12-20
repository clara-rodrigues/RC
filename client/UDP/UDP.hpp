// udp.hpp
#ifndef UDP_HPP
#define UDP_HPP

#include <string>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

int send_UDP(const std::string& msg, std::string& response, const std::string& ip, const std::string& port);

#endif
