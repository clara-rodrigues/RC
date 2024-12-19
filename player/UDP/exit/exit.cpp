// exit.cpp

#include "exit.hpp"

void execute_exit(Player &player,const std::string& ip, const std::string& port) {
   execute_quit(player, ip, port);
   running = false;
}
