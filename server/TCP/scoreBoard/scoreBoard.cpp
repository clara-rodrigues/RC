#include "scoreBoard.hpp"


void handleScoreBoard(int client_fd,std::istringstream &commandStream) {
    try{
        checkExtraInput(commandStream);
    }catch (const std::exception& e) {
        std::cerr << "[ERROR] Invalid command." << std::endl;
        const std::string error_response = "RST NOK\n";
        send(client_fd, error_response.c_str(), error_response.size(), 0);
        return;
    }

    std::sort(games.begin(), games.end(), [](const Game& a, const Game& b) {
        return a.score > b.score;
    });

    int topCount = std::min(10, static_cast<int>(games.size()));

    for (int i = 0; i < topCount; ++i) {
        std::cout << "Game " << i + 1 << ": " << games[i].score << std::endl;
    }
}