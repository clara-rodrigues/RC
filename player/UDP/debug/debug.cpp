#include "debug.hpp"

// Executes the "debug" command, which sends a debug message to the server with specific game parameters.
// Parameters:
// - player: The Player object containing the player's ID (plid) and game settings.
// - ip, port: The server's IP address and port number.
// - guesses: A vector of strings representing the player's guesses.
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
