// udp.hpp
#ifndef UDP_HPP
#define UDP_HPP

#include <string>
#include <sys/socket.h>
#include "start/start.hpp"
#include "try/try.hpp"
#include "quit/quit.hpp"
#include "debug/debug.hpp"




int startUDP();

void handleUserMessage(int fd, struct sockaddr_in &client_addr, socklen_t client_len, char *buffer, ssize_t n);
void handleStartGame( int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream);

int validPLID(std::istream& input);
int validMaxPlayTime(std::istream& input);
std::vector<std::string> validGuess(std::istream& input);
void checkExtraInput(std::istream& input);
int checkNumTrials(std::istream& input);


#endif
