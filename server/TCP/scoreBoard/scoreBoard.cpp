#include <fstream>
#include <iomanip>
#include "scoreBoard.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>
#include <filesystem>


std::vector<Game> getGameScores(){
    std::string folderName = "server/SCORES";
    std::vector<Game> games;
    std::string secretKey;

    if(!std::filesystem::exists(folderName)){
        std::cerr << "Folder " << folderName << " does not exist." << std::endl;
        return {};
    }

    if(std::filesystem::is_empty(folderName)){
        std::cerr << "Folder " << folderName << " is empty." << std::endl;
        return {};
    }

    for (const auto &entry : std::filesystem::directory_iterator(folderName)) {
        std::ifstream file(entry.path());
        if (!file.is_open()) {
            std::cerr << "Failed to open file " << entry.path() << std::endl;
            continue;
        }

        Game game;
        file >> game.score;
        file >> game.plid;
        file >> secretKey;
        file >> game.numTrials;
        file >> game.gameMode;


        for (size_t i = 0; i < secretKey.size(); i++) {
            game.secretKey.push_back(secretKey.substr(i, 1));
        }

        games.push_back(game);
        file.close();
        
    }

    return games;
    

}


void handleScoreBoard(int client_fd, std::istringstream &commandStream, std::string client_ip, int client_port) {
    try {
        checkExtraInput(commandStream);
        if (verbose)
            std::cout << "[Verbose] [IP: " << client_ip << "] [PORT: " << client_port
                      << "] Requesting ScoreBoard" << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "[ERROR] Invalid command." << std::endl;
        const std::string error_response = "RSS NOK\n";
        write(client_fd, error_response.c_str(), error_response.size());
        return;
    }

    std::vector<Game> sortedGames = getGameScores();
    std::sort(sortedGames.begin(), sortedGames.end(), [](const Game &a, const Game &b) {
        return a.score > b.score;
    });

    std::ostringstream scoreboard_info;

    int topCount = std::min(10, static_cast<int>(sortedGames.size()));
    for (int i = 0; i < topCount; ++i) {
        const Game &game = sortedGames[i];
        std::string secretKey;
        for (const auto &part : game.secretKey) {
            secretKey += part;
        }
        scoreboard_info << game.plid << " " << secretKey << " " ;
        scoreboard_info << std::setw(3) << std::setfill('0') << game.score << "\n";
    }

    if (scoreboard_info.str().empty()) {
        std::cerr << "[ERROR] Scoreboard is empty!" << std::endl;
        const std::string error_response = "RSS NOK\n";
        write(client_fd, error_response.c_str(), error_response.size());
        return;
    }

    std::string scoreboardStr = scoreboard_info.str();
    std::size_t dataSize = scoreboardStr.size();

    std::ostringstream header;
    header << "RSS OK scoreboard.txt " << dataSize << " ";
    std::string header_str = header.str();

    sendToPlayer(client_fd, header_str, scoreboardStr);
}