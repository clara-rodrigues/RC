// try.hpp
#ifndef TRY_HPP
#define TRY_HPP
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include "../../player.hpp"

int getResponseId(const std::string& command);
std::vector<int> checkNums(std::string numTrials,std::string numBlacks, std::string numWhites);
void execute_try(Player &player, const std::vector<std::string>& guesses, const std::string& ip, const std::string& port, int& numTrials);

#endif
