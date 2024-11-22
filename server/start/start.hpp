#ifndef START_HPP
#define START_HPP

#include <string>

int startNewGame(const std::string& plid, int maxPlaytime);

std::vector<std::string> generateSecretKey();

#endif