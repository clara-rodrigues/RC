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


int getCommandID_TCP(const std::string& command);

void sendToPlayer(int client_fd,std::string header_str, std::string trials_info);

void handlePlayerRequest(int client_fd, struct sockaddr_in client_addr);

int startTCPServer(std::string port);

#endif 
