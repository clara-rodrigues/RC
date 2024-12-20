#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include "../TCP.hpp"

// Function to request the scoreboard from the server and process the response.
// Parameters:
// - ip: The server's IP address.
// - port: The server's port number.
void score_board(const std::string& ip, const std::string& port) {
    const std::string msg = "SSB\n";

    std::string response;

    if (send_TCP(msg, response, ip, port) != 1) {
        std::cerr << "Failed to send 'scoreboard' command." << std::endl;
        return;
    }
    
    std::istringstream iss(response);
    std::string status, fname;

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
