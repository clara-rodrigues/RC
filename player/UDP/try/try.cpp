#include <iostream>
#include <vector>
#include <string>
#include <sstream> // Add this line
#include "try.hpp"    

#include "../UDP.hpp"







void execute_try(Player &player, const std::vector<std::string>& guesses, const std::string& ip, const std::string& port, int& numTrials) {
    int numBlacks;
    std::string plid = player.plid;
    std::string msg = "TRY " + plid ;
    int gameTrials = 0;
    
    for (const auto& guess : guesses) {
        msg += " " + guess;
    }
    msg += " " + std::to_string(numTrials) + "\n";

    std::cout << "Sending 'try' command: " << msg << std::endl;

    std::string response;
    
    if (send_UDP(msg, response, ip, port) == 1) {
        std::cout << "Server Response: " << response << std::endl;

        std::istringstream iss(response);
        std::string responseStatus;
        iss >> response>> responseStatus >> gameTrials >> numBlacks;


        if (responseStatus == "OK") {
            if(numBlacks == 4){
                closeGame(player);
                std::cout << "WINNNN!" << std::endl;
            }else{
                numTrials++;    
            }
        }else if (responseStatus == "ENT" || responseStatus == "ETM") {
            closeGame(player);
        }

    } else {
        std::cerr << "Failed to send 'try' command1." << std::endl;
    }
}
