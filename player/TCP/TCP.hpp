#ifndef TCP_HPP
#define TCP_HPP
#include <string>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <fstream> 
#include <sys/types.h>
#include <cerrno>
#include <sstream>
#include <vector>


int send_TCP(const std::string& msg, std::string& response, const std::string& ip, const std::string& port);
void writeFile(std::istringstream& response);
#endif