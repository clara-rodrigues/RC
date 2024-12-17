#ifndef START_HPP
#define START_HPP

#include <string>
#include "../../GS.hpp"
#include "../UDP.hpp"
#include <time.h>
#include <filesystem> 

int startNewGame(int plid, int maxPlaytime);

std::vector<std::string> generateSecretKey();
void handleStartGame( int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream, std::string client_ip, int client_port);
void createPlayerFile(int plid,int gameId);

#endif