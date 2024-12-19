
#include "debug.hpp"

void execute_debug(Player &player,const std::string& ip, const std::string& port,std::vector<std::string> guesses) {
    std::string msg = "DBG " + player.plid + " " + std::to_string(player.maxPlaytime) + " " +guesses[0] + " " + guesses[1] + " " +guesses[2] + " " + guesses[3] + "\n";
    std::string response;
    
    if (send_UDP(msg, response, ip, port) == 1) {
        std::cout << "Server Response: " << response << std::endl;
        player.isPlaying = true;
        player.numTrials = 1;
    } else {
        std::cerr << "Failed to send 'debug' command." << std::endl;
    }

    std::istringstream iss(response);
    std::string status;

    if (!(iss >> status) || status != "RDB") {
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
