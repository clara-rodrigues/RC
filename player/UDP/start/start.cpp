

#include "start.hpp"

void execute_start(Player &player, const std::string& ip, const std::string& port) {
    std::string plid = player.plid;
    int maxPlaytime = player.maxPlaytime;
    std::string msg = "SNG " + plid + " " + std::to_string(maxPlaytime)+ "\n";
    std::string response;

  
    if (send_UDP(msg, response, ip, port) == 1) {
        std::cout << "Server Response: " << response << std::endl;
        player.isPlaying = true;
    } else {
        std::cerr << "Failed to send 'start' command." << std::endl;
    }

    std::istringstream iss(response);
    std::string status;

    if (!(iss >> status) || status != "RSG") {
        std::cerr << "Error: Invalid server response format (status)." << std::endl;
        return;
    }

    if(!(iss >> status) || (status != "OK" && status != "NOK" && status != "ERR")) {
        std::cerr << "Error: Invalid server response format (status)." << std::endl;
        return;
    }

    if(iss >> status){
        std::cerr << "Error: Invalid server response format (status)." << std::endl;
    }
}
