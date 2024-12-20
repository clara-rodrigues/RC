#ifndef EXIT_HPP
#define EXIT_HPP

#include <string>
#include "../../player.hpp"
#include <iostream>
#include "../UDP.hpp"
#include "../quit/quit.hpp"

void execute_exit(Player &player,const std::string& ip, const std::string& port);

#endif