#ifndef SHOW_TRIALS_HPP
#define SHOW_TRIALS_HPP

#include <iostream>
#include <string>
#include <sstream>


std::string readFile(int client_fd, const std::string &filename);

bool FindLastGame(const std::string &PLID, std::string &fname);

void handleShowTrials(int client_fd, std::istringstream &commandStream, std::string client_ip, int client_port);

#endif