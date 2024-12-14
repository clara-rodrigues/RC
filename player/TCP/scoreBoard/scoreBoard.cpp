#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include "../TCP.hpp"

void score_board(const std::string& ip, const std::string& port) {
    const std::string msg = "SSB";
    std::cout << "Sending 'scoreboard' command: " << msg << std::endl;

    std::string response;

    if (send_TCP(msg, response, ip, port) != 1) {
        std::cerr << "Failed to send 'scoreboard' command." << std::endl;
        return;
    }

    std::cout << "Raw Response: [" << response << "]" << std::endl;

    std::istringstream iss(response);
    std::string status, fname;
    std::size_t fsize;

    if (!(iss >> status) || status != "RSS") {
        std::cerr << "Error: Invalid server response format (status)." << std::endl;
        return;
    }

    if (!(iss >> status) || status != "OK") {
        std::cerr << "Error: Server returned error status." << std::endl;
        return;
    }

    if (!(iss >> fname >> fsize)) {
        std::cerr << "Error: Failed to parse filename and size." << std::endl;
        return;
    }

    std::cout << "Scoreboard File: " << fname << ", Size: " << fsize << " bytes" << std::endl;

    
    std::string fdata;
    char buffer[4096];
    std::size_t total_received = 0;

    while (total_received < fsize) {
    
        std::size_t bytes_to_read = std::min(fsize - total_received, sizeof(buffer));
        std::cout << "Bytes to read: " << bytes_to_read << std::endl;

        iss.read(buffer, bytes_to_read);

        if (iss.eof()) {
            std::cerr << "End of stream reached, but not all bytes were read." << std::endl;
        } else if (iss.fail()) {
            std::cerr << "Error: Failed to read file data from response stream." << std::endl;
            break; 
        }

      
        fdata.append(buffer, iss.gcount());
        total_received += iss.gcount();


        std::cout << "Bytes read: " << iss.gcount() << std::endl;
    }



    std::cout << "Received Data Size: " << fdata.size() << " bytes" << std::endl;

    // Save the received file
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

    // Display scores
    std::istringstream score_stream(fdata);
    std::string line;
    std::vector<std::string> scores;

    while (std::getline(score_stream, line)) {
        if (!line.empty()) {
            scores.push_back(line);
        }
    }

    std::cout << "Scores received: " << scores.size() << std::endl;

    if (scores.empty()) {
        std::cout << "No scores found in the scoreboard data." << std::endl;
        return;
    }

    std::cout << "Top 10 Scores:\n";
    for (size_t i = 0; i < scores.size(); ++i) {
        std::cout << i + 1 << ". " << scores[i] << std::endl;
    }
}
