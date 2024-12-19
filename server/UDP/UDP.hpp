// udp.hpp
#ifndef UDP_HPP
#define UDP_HPP

#include <string>
#include <sys/socket.h>
#include "start/start.hpp"
#include "try/try.hpp"
#include "quit/quit.hpp"
#include "debug/debug.hpp"
#include "../GS.hpp"


void handle_client(int fd, struct sockaddr_in &client_addr, socklen_t client_len, char *buffer, ssize_t n);

int startUDP(std::string port);

int getCommandID_UDP(const std::string& command);

void handleUserMessage(int fd, struct sockaddr_in &client_addr, socklen_t client_len, char *buffer, ssize_t n);

#endif
