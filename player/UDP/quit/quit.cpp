#include <iostream>
#include "../UDP.hpp"

void execute_quit(std::string &plid ,const std::string& ip, const std::string& port) {
    std::string msg = "QUT "+ plid;
    std::string response;

    std::cout << "Sending 'quit' command: " << msg << std::endl;

    if (send_UDP(msg, response, ip, port) == 1) {
        std::cout << "Server Response: " << response << std::endl;
    } else {
        std::cerr << "Failed to send 'quit' command." << std::endl;
    }
}
