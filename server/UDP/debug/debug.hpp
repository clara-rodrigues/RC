#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <string>
#include <sys/socket.h> 
#include "../../GS.hpp"
#include <vector>
#include "../UDP.hpp"


void handleDebug(int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream, std::string client_ip, int client_port);

int debug(int plid, int maxPlaytime, std::vector<std::string> guesses);

#endif

