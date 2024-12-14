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

bool is_numeric(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

void show_trials(const std::string& ip, const std::string& port, const std::string& plid) {
    const std::string msg = "STR " + plid; 
    std::cout << "Sending 'show trials' command: " << msg << std::endl;

    std::string response;

    if (send_TCP(msg, response, ip, port) != 1) {
        std::cerr << "Failed to send 'show trials' command." << std::endl;
        return;
    }

    std::cout << "Server Response Received:\n" << response << std::endl;

    std::istringstream iss(response);
    std::string status, fname, fsize_str, line;

    // Read the status (should be "RST OK")
    if (!(iss >> status) || status != "RST") {
        std::cerr << "Error: Invalid server response format (status)." << std::endl;
        return;
    }

    if (!(iss >> status) || status != "OK") {
        std::cerr << "Error: Invalid server response format (OK)." << std::endl;
        return;
    }   

    std::getline(iss, fname);  
    std::stringstream ss(fname);  
    ss >> fname;  
    ss >> fsize_str;  

    if (!is_numeric(fsize_str)) {
        std::cerr << "Error: Invalid file size format: '" << fsize_str << "'\n";
        return;
    }

    int fsize = std::stoi(fsize_str);
    std::cout << "File size: " << fsize << " bytes\n";

    std::vector<std::string> file_data;
    while (std::getline(iss, line)) {
        file_data.push_back(line);
    }

    if (file_data.empty()) {
        std::cerr << "Error: No file data received.\n";
        return;
    }

    std::string file_path = "player/" + fname;

    std::ofstream output_file(file_path, std::ios::binary);
    if (!output_file) {
        std::cerr << "Error: Failed to open file '" << file_path << "' for writing.\n";
        return;
    }

    for (const auto& file_line : file_data) {
        output_file << file_line << '\n';
    }

    output_file.close();
    if (output_file.fail()) {
        std::cerr << "Error: Failed to save the file.\n";
        return;
    }

    std::cout << "File received and saved as '" << file_path << "'.\n";
}
