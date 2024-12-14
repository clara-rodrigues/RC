#include <fstream>
#include <iomanip>
#include "scoreBoard.hpp"
#include <string>

void handleScoreBoard(int client_fd, std::istringstream &commandStream) {
    try {
        checkExtraInput(commandStream); 
    } catch (const std::exception &e) {
        std::cerr << "[ERROR] Invalid command." << std::endl;
        const std::string error_response = "RSS NOK\n";
        send(client_fd, error_response.c_str(), error_response.size(), 0);
        return;
    }

    if (games.empty()) {
        std::cerr << "[INFO] No games found for the scoreboard." << std::endl;
        const std::string no_games_response = "RSS EMPTY\n";
        send(client_fd, no_games_response.c_str(), no_games_response.size(), 0);
        return;
    }

    // Sort games by score in descending order
    std::vector<Game> sortedGames = games; 
    std::sort(sortedGames.begin(), sortedGames.end(), [](const Game &a, const Game &b) {
        return a.score > b.score;
    });

    const std::string filename = "server/scoreboard.txt";
    std::ofstream scoreboardFile(filename);
    
    // Check if the file is successfully opened
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

    // Now proceed to send the file content to the client
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "[ERROR] Failed to read scoreboard file." << std::endl;
        const std::string error_response = "RST NOK\n";
        send(client_fd, error_response.c_str(), error_response.size(), 0);
        return;
    }

    // Get the file size
    std::size_t fileSize = file.tellg();
    file.close();

    // Send header with file size and filename
    std::ostringstream header;
    header << "RSS OK " << filename << " " << fileSize << "\n";
    std::string header_str = header.str();
    send(client_fd, header_str.c_str(), header_str.size(), 0);

    // Now send the file in chunks
    file.open(filename, std::ios::binary);
    char buffer[4096];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        send(client_fd, buffer, file.gcount(), 0);
    }
    file.close();
}
