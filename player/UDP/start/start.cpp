#include <iostream>
#include <string>
#include "../UDP.hpp"

void execute_start(const std::string& plid, int maxPlaytime, const std::string& ip, const std::string& port) {
    std::string msg = "SNG " + plid + " " + std::to_string(maxPlaytime);
    std::string response;

  
    if (send_UDP(msg, response, ip, port) == 1) {
        std::cout << "Server Response: " << response << std::endl;
    } else {
        std::cerr << "Failed to send 'start' command." << std::endl;
    }
}
