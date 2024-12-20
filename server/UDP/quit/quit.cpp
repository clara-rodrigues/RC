
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "quit.hpp"

// Function to handle the "quit" command from the client
void handleQuit(int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream, const std::string client_ip, int client_port) {
    Player* currentPlayer = nullptr;
    int plid;

    try {
        plid = validPLID(commandStream);
        checkExtraInput(commandStream);

        if (verbose) {
            std::cout << "[Verbose] [IP: " << client_ip << "] [PORT: " << client_port 
                      << "] Quit Game for PLID: " << plid << std::endl;
        }
    } catch (const std::invalid_argument &e) {
        std::cout << e.what() << std::endl;
        sendto(fd, "RQT ERR\n", 8, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }

    currentPlayer = findPlayerById(plid);

    if (!currentPlayer) {
        sendto(fd, "RQT NOK\n", 8, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }

    time_t currentTime = time(0);
    auto &game = games[currentPlayer->gameId];

    if (currentTime - game.startTime > game.maxPlaytime) {
        game.finalSate = 'T';
        closeGame(*currentPlayer, game);
    }

    if (currentPlayer->isPlaying) {
        int gameId = currentPlayer->gameId;
        auto &game = games[gameId];

        game.finalSate = 'Q';
        std::ostringstream oss;
        for (const auto &key : game.secretKey) {
            oss << " " << key;
        }

        std::string message = "RQT OK" + oss.str() + "\n";
        sendto(fd, message.c_str(), message.size(), 0, (struct sockaddr *)&client_addr, client_len);
        closeGame(*currentPlayer, game);
    } else {
        sendto(fd, "RQT NOK\n", 8, 0, (struct sockaddr *)&client_addr, client_len);
    }
}
