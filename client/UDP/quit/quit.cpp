#include "quit.hpp"

// Executes the "quit" command, which signals the end of the game session for the player.
// Parameters:
// - player: The Player object containing the player's ID (plid) and game settings.
// - ip, port: The server's IP address and port number.
void execute_quit(Player &player ,const std::string& ip, const std::string& port) {
    std::string plid = player.plid;
    std::string msg = "QUT "+ plid + "\n";
    std::string response;


    if (send_UDP(msg, response, ip, port) == 1) {
        std::cout << "Server Response: " << response << std::endl;
        closeGame(player);
    } else {
        std::cerr << "Failed to send 'quit' command." << std::endl;
    }

    std::istringstream iss(response);
    std::string status;

    if (!(iss >> status) || status != "RQT") {
        std::cerr << "Error: Invalid server response format (status)." << std::endl;
        return;
    }

    if(!(iss >> status) || (status != "OK" && status != "NOK" && status != "ERR")) {
        std::cerr << "Error: Invalid server response format (status)." << std::endl;
        return;
    }

    if(status == "OK"){
        std::vector<char> valid_colors = {'R', 'G', 'B', 'Y', 'O', 'P'};
        std::string guess1, guess2, guess3, guess4;


        if (!(iss >> guess1 >> guess2 >> guess3 >> guess4)) {
            std::cerr << "Error: Invalid server response format (status)." << std::endl;
            return;
        }

        char guess1_color = guess1[0];
        char guess2_color = guess2[0];
        char guess3_color = guess3[0];
        char guess4_color = guess4[0];

        if (std::find(valid_colors.begin(), valid_colors.end(),guess1_color) == valid_colors.end() || 
            std::find(valid_colors.begin(), valid_colors.end(), guess2_color) == valid_colors.end() || 
            std::find(valid_colors.begin(), valid_colors.end(), guess3_color) == valid_colors.end() || 
            std::find(valid_colors.begin(), valid_colors.end(), guess4_color) == valid_colors.end() ) {
                std::cerr << "Error: Invalid server response format (status)." << std::endl;
                return;
            }
    }else{
        if(iss >> status){
            std::cerr << "Error: Invalid server response format (status)." << std::endl;
            std::cerr << "Error: Invalid server response format (status)." << std::endl;
        }
    }
}
