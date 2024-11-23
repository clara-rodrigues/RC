// udp.hpp
#ifndef UDP_HPP
#define UDP_HPP

#include <string>
#include <sys/socket.h>

int startUDP();

void handleUserMessage(int fd, struct sockaddr_in &client_addr, socklen_t client_len, char *buffer, ssize_t n);
void handleStartGame( int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream, const std::string &plid);


#endif
