// quit.hpp
#ifndef QUIT_HPP
#define QUIT_HPP
#include "../../player.hpp"
#include <string>
#include <cctype>
#include <iostream>
#include <algorithm> 
#include "../UDP.hpp"
void execute_quit(Player &player ,const std::string& ip, const std::string& port);

#endif
