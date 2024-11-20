// exit.cpp
#include <iostream>
#include "../UDP.hpp"

void execute_exit(const std::string& ip, const std::string& port) {
    std::string msg = "exit";  // Mensagem do comando 'exit'
    std::string response;
    
    if (send_UDP(msg, response, ip, port) == 1) {
        std::cout << "Server Response: " << response << std::endl;
    } else {
        std::cerr << "Failed to send 'exit' command." << std::endl;
    }
}
