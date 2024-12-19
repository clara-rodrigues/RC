#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include "../TCP.hpp"





void score_board(const std::string& ip, const std::string& port) {
    const std::string msg = "SSB\n";
    std::cout << "Sending 'scoreboard' command: " << msg << std::endl;

    std::string response;

    if (send_TCP(msg, response, ip, port) != 1) {
        std::cerr << "Failed to send 'scoreboard' command." << std::endl;
        return;
    }

    std::cout << "Raw Response: [" << response << "]" << std::endl;

    std::istringstream iss(response);
    std::string status, fname;
    //std::size_t fsize;

    if (!(iss >> status) || status != "RSS") {
        std::cerr << "Error: Invalid server response format (status)." << std::endl;
        return;
    }

    if (!(iss >> status) || status != "OK") {
        std::cerr << "Error: Server returned error status." << std::endl;
        return;
    }


    writeFile(iss);

}
