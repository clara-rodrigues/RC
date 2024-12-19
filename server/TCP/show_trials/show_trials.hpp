#ifndef SHOW_TRIALS_HPP
#define SHOW_TRIALS_HPP

#include <iostream>
#include <string>
#include <sstream>

std::time_t parseDateTime(const std::string &dateTimeStr);

std::string getLastLine(const std::string &filePath);

std::string getLastGameSummary(int plid);

void sendFile(int client_fd, const std::string &filename,std::string status) ;

void handleShowTrials(int client_fd, std::istringstream &commandStream, std::string client_ip, int client_port);

#endif