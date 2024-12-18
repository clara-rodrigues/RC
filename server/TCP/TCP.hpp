#ifndef SERVER_TCP_HPP
#define SERVER_TCP_HPP
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sstream>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <ctime>
#include "../GS.hpp"
#include "show_trials/show_trials.hpp"
#include "scoreBoard/scoreBoard.hpp"

#define TCP_PORT "58000"
#define BUFFER_SIZE 1024

void handlePlayerRequest(int client_fd, struct sockaddr_in client_addr);
void handleShowTrials(int client_fd, std::istringstream &commandStream);
void sendFile(int client_fd, const std::string &filename);
void handleScoreBoard(int client_fd, std::istringstream &commandStream);

int startTCPServer(std::string port);
int getCommandID_TCP(const std::string& command);
void sendToPlayer(int client_fd,std::vector<char> buffer);

#endif 
