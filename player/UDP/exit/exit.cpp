// exit.cpp
#include <iostream>
#include "../UDP.hpp"
#include "../quit/quit.hpp"
#include "../../player.hpp"

void execute_exit(std::string plid,const std::string& ip, const std::string& port) {
   execute_quit(plid, ip, port);
   running = false;
}
