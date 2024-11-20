#include <iostream>
#include <vector>
#include <string>
#include "../UDP.hpp"

void execute_try(const std::string& plid, const std::vector<std::string>& guesses, const std::string& ip, const std::string& port) {
    std::string msg = "try " + plid;
    for (const auto& guess : guesses) {
        msg += " " + guess;
    }
    std::string response;
    if (send_UDP(msg, response, ip, port) == 1) {
        std::cout << "Server Response: " << response << std::endl;
    } else {
        std::cerr << "Failed to send 'try' command." << std::endl;
    }
}
