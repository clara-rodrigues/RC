#include <iostream>
#include "../UDP.hpp"

void execute_debug(const std::string& ip, const std::string& port) {
    std::string msg = "debug";
    std::string response;
    if (send_UDP(msg, response, ip, port) == 1) {
        std::cout << "Server Response: " << response << std::endl;
    } else {
        std::cerr << "Failed to send 'debug' command." << std::endl;
    }
}
