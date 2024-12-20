#ifndef SCORE_BOARD_HPP
#define SCORE_BOARD_HPP
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../TCP.hpp"

void score_board(const std::string& ip, const std::string& port);
#endif