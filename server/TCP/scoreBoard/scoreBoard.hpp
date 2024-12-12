#ifndef SCORE_BOARD_HPP
#define SCORE_BOARD_HPP

 
#include <iostream>
#include <algorithm>
#include <vector>
#include "../../GS.hpp"
#include <string>
#include <sys/socket.h>
#include <sstream>

void handleScoreBoard(int client_fd,std::istringstream &commandStream);

#endif