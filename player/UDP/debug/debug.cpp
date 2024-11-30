#include <iostream>
#include "../UDP.hpp"
#include "debug.hpp"

void execute_debug(Player &player,const std::string& ip, const std::string& port,std::vector<std::string> guesses) {
    std::string msg = "DBG " + player.plid + " " + std::to_string(player.maxPlaytime) + " " +guesses[0] + " " + guesses[1] + " " +guesses[2] + " " + guesses[3];
    std::string response;
    
    if (send_UDP(msg, response, ip, port) == 1) {
        std::cout << "Server Response: " << response << std::endl;
    } else {
        std::cerr << "Failed to send 'debug' command." << std::endl;
    }
}
