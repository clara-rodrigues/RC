#include <iostream>
#include <vector>
#include <string>
#include <sstream> // Add this line
#include "try.hpp"    

#include "../UDP.hpp"

void execute_try(const std::string& plid, const std::vector<std::string>& guesses, const std::string& ip, const std::string& port, int& numTrials) {
    std::string msg = "TRY " + plid ;
    for (const auto& guess : guesses) {
        msg += " " + guess;
    }
    msg += " " + std::to_string(numTrials);

    std::cout << "Sending 'try' command: " << msg << std::endl;

    std::string response;
    
    if (send_UDP(msg, response, ip, port) == 1) {
        std::cout << "Server Response: " << response << std::endl;

        std::istringstream iss(response);
        std::string responseStatus;
        iss >> response>> responseStatus;

        if (responseStatus == "OK") {
           numTrials++;
    } else {
        std::cerr << "Failed to send 'try' command." << std::endl;
    }
    }
}