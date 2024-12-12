// try.hpp
#ifndef TRY_HPP
#define TRY_HPP

#include <string>
#include <vector>
#include "../../player.hpp"

void execute_try(Player &player, const std::vector<std::string>& guesses, const std::string& ip, const std::string& port, int& numTrials);

#endif
