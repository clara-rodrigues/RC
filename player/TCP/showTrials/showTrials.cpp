#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../TCP.hpp"

void show_trials(const std::string& ip, const std::string& port,const std::string& plid) {
    const std::string msg = "STR" + plid; 
    std::cout << "Sending 'show trials' command: " << msg << std::endl;

    std::string response;

    if (send_TCP(msg, response, ip, port) != 1) {
        std::cerr << "Failed to send 'show trials' command." << std::endl;
        return;
    }

    std::cout << "Server Response:\n" << response << std::endl;

    std::istringstream iss(response);
    std::string line;
    std::vector<std::string> trials;

    while (std::getline(iss, line)) {
        if (!line.empty()) {
            trials.push_back(line);
        }
    }

    if (trials.empty()) {
        std::cout << "No trials found in the server response." << std::endl;
        return;
    }

    std::cout << "Trials History:\n";
    for (size_t i = 0; i < trials.size(); ++i) {
        if (i == trials.size() - 1 && trials[i].find("Time remaining") != std::string::npos) {
            std::cout << trials[i] << std::endl; 
        } else {
            std::cout << "Trial " << i + 1 << ": " << trials[i] << std::endl; 
        }
    }
}
