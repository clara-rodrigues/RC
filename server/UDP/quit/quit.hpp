#ifndef QUIT_HPP
#define QUIT_HPP


#include <string>
#include "../UDP.hpp"

void handleQuit(int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream);

#endif