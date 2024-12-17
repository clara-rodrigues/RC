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




void handleScoreBoard(int client_fd, std::istringstream &commandStream) {
    try {
        checkExtraInput(commandStream);
    } catch (const std::exception &e) {
        std::cerr << "[ERROR] Invalid command." << std::endl;
        const std::string error_response = "RSS NOK\n";
        send(client_fd, error_response.c_str(), error_response.size(), 0);
        return;
    }

    //if (games.empty()) {
    //    std::cerr << "[INFO] No games found for the scoreboard." << std::endl;
    //    const std::string no_games_response = "RSS EMPTY\n";
    //    send(client_fd, no_games_response.c_str(), no_games_response.size(), 0);
    //    return;
    //}

    // Sort games by score in descending order
    std::vector<Game> sortedGames = getGameScores();
    std::sort(sortedGames.begin(), sortedGames.end(), [](const Game &a, const Game &b) {
        return a.score > b.score;
    });

    const std::string filename = "server/scoreboard.txt";
    std::ofstream scoreboardFile(filename);

    if (!scoreboardFile.is_open()) {
        std::cerr << "[ERROR] Failed to create scoreboard file." << std::endl;
        const std::string error_response = "RSS NOK\n";
        send(client_fd, error_response.c_str(), error_response.size(), 0);
        return;
    }

    // Write the top 10 scores (or fewer if less than 10 games)
    int topCount = std::min(10, static_cast<int>(sortedGames.size()));
    for (int i = 0; i < topCount; ++i) {
        const Game &game = sortedGames[i];
        std::string secretKey;
        for (const auto &part : game.secretKey) {
            secretKey += part;
        }
        scoreboardFile << game.plid << " " << secretKey << " " << game.score << "\n";
    }
    scoreboardFile.close();
    std::cerr << "[DEBUG] Scoreboard file written successfully.\n";

    // Check if the file is empty
    std::ifstream checkFile(filename);
    if (checkFile.peek() == std::ifstream::traits_type::eof()) {
        std::cerr << "[ERROR] Scoreboard file is empty!" << std::endl;
        const std::string error_response = "RSS NOK\n";
        send(client_fd, error_response.c_str(), error_response.size(), 0);
        return;
    }
    checkFile.close();

    // Open the file in binary mode
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "[ERROR] Failed to read scoreboard file." << std::endl;
        const std::string error_response = "RSS NOK\n";
        send(client_fd, error_response.c_str(), error_response.size(), 0);
        return;
    }

    // Calculate file size
    std::size_t fileSize = file.tellg();
    file.seekg(0);

    // Prepare header
    std::ostringstream header;
    header << "RSS OK " << "scoreboard.txt" << " " << fileSize << " ";
    std::string header_str = header.str();

    // Combine header and file data into one buffer
    std::vector<char> buffer(header_str.begin(), header_str.end()); // Start with header

    // Read the file into the same buffer
    char file_buffer[4096];
    while (file.read(file_buffer, sizeof(file_buffer)) || file.gcount() > 0) {
        buffer.insert(buffer.end(), file_buffer, file_buffer + file.gcount());
    }
    buffer.push_back('\n');  
    file.close();
    std::cerr << "[DEBUG] File content read into buffer successfully.\n";

    sendToPlayer(client_fd, buffer);

    
}
