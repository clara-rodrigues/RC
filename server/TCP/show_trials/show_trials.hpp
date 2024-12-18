#ifndef SHOW_TRIALS_HPP
#define SHOW_TRIALS_HPP

#include <iostream>
#include <string>
#include <sstream>

void handleShowTrials(int client_fd, std::istringstream &commandStream, std::string client_ip, int client_port);

#endif