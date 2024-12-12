#include <iostream>
#include "../UDP.hpp"
#include "quit.hpp"

void execute_quit(Player &player ,const std::string& ip, const std::string& port) {
    std::string plid = player.plid;
    std::string msg = "QUT "+ plid;
    std::string response;

    std::cout << "Sending 'quit' command: " << msg << std::endl;

    if (send_UDP(msg, response, ip, port) == 1) {
        std::cout << "Server Response: " << response << std::endl;
        closeGame(player);
    } else {
        std::cerr << "Failed to send 'quit' command." << std::endl;
    }
}
