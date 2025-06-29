#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../TCP.hpp"
#include <algorithm>

// Function to check if a string consists solely of numeric characters.
bool is_numeric(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

// Function to execute the "show trials" command. This requests and processes the 
// trial data for a specific player from the server.
// Parameters:
// - ip: The server's IP address.
// - port: The server's port number.
// - plid: The player's ID.
void show_trials(const std::string& ip, const std::string& port, const std::string& plid) {
    const std::string msg = "STR " + plid + "\n"; 
    std::string response;

    if (send_TCP(msg, response, ip, port) != 1) {
        std::cerr << "Failed to send 'show trials' command." << std::endl;
        return;
    }

    std::istringstream iss(response);
    std::string status, fname;

    if (!(iss >> status) || status != "RST") {
        std::cerr << "Error: Invalid server response format (status)." << std::endl;
        return;
    }

    if (!(iss >> status) || (status != "ACT" && status != "FIN")) {
        std::cerr << "Error: Server returned error status." << std::endl;
        return;
    }

    writeFile(iss);


    
}
