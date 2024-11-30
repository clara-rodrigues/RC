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

#define TCP_PORT "58000"
#define BUFFER_SIZE 1024

void handlePlayerRequest(int client_fd);
void handleShowTrials(int client_fd, const std::string &plid);
void sendFile(int client_fd, const std::string &filename);

int startTCPServer();

#endif 
