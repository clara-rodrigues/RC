#include "scoreBoard.hpp"


void score_board(const std::string& ip, const std::string& port) {
    const std::string msg = "SSB"; 
    std::cout << "Sending 'scoreboard' command: " << msg << std::endl;

    std::string response;

    if (send_TCP(msg, response, ip, port) != 1) {
        std::cerr << "Failed to send 'scoreboard' command." << std::endl;
        return;
    }

    std::istringstream iss(response);
    std::string status;
    iss >> status;

    if (status == "EMPTY") {
        std::cout << "Scoreboard is empty. No games have been won yet." << std::endl;
        return;
    } else if (status != "OK") {
        std::cerr << "Unexpected response from server: " << status << std::endl;
        return;
    }

    std::string fname;
    size_t fsize;
    iss >> fname >> fsize;

    std::cout << "Scoreboard File: " << fname << ", Size: " << fsize << " bytes" << std::endl;
    std::string fdata;
    std::getline(iss, fdata, '\0'); 

    std::istringstream fdata_stream(fdata);
    std::string line;
    std::vector<std::string> scores;

    while (std::getline(fdata_stream, line)) {
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
