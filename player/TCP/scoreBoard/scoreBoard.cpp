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

void score_board(const std::string& ip, const std::string& port) {
    const std::string msg = "SSB"; 
    std::cout << "Sending 'scoreboard' command: " << msg << std::endl;

    std::string response;

    if (send_TCP(msg, response, ip, port) != 1) {
        std::cerr << "Failed to send 'scoreboard' command." << std::endl;
        return;
    }

    
    std::cout << "Server Response Received:\n" << response << std::endl;

    std::istringstream iss(response);
    std::string status, fname, fsize, line;

    if (!(iss >> status) || status != "RSS") {
        std::cerr << "Error: Invalid server response format (status)." << std::endl;
        return;
    }

    if (!(iss >> status) || status != "OK") {
        std::cerr << "Error: Invalid server response format (OK)." << std::endl;
        return;
    }

    iss >> fname >> fsize;

    std::cout << "Scoreboard File: " << fname << ", Size: " << fsize << " bytes" << std::endl;

    std::string fdata((std::istreambuf_iterator<char>(iss)), std::istreambuf_iterator<char>());

    std::string file_path = "player/" + fname;
    std::ofstream output_file(file_path, std::ios::binary);
    if (!output_file) {
        std::cerr << "Error: Failed to open file '" << file_path << "' for writing.\n";
        return;
    }
    output_file << fdata;
    output_file.close();

    if (output_file.fail()) {
        std::cerr << "Error: Failed to save the file.\n";
        return;
    }

    std::cout << "File received and saved as '" << file_path << "'.\n";

    std::istringstream score_stream(fdata);
    std::vector<std::string> scores;

    while (std::getline(score_stream, line)) {
        if (!line.empty()) {
            scores.push_back(line);
        }
    }

    if (scores.empty()) {
        std::cout << "No scores found in the scoreboard data." << std::endl;
        return;
    }

    std::cout << "Top 10 Scores:\n";
    for (size_t i = 0; i < scores.size(); ++i) {
        std::cout << i + 1 << ". " << scores[i] << std::endl;
    }
}
