#include <iostream>
#include <string>
#include <sys/socket.h> // Add this line to include the appropriate header file

#include "../../GS.hpp"
#include <sstream>
#include "../TCP.hpp"


void sendFile(int client_fd, const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "[ERROR] Não foi possível abrir o arquivo: " << filename << std::endl;
        const std::string error_response = "RST NOK\n";
        send(client_fd, error_response.c_str(), error_response.size(), 0);
        return;
    }

    file.seekg(0, std::ios::end);
    std::size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::cerr << "[DEBUG] sendFile: Tamanho do arquivo: " << file_size << " bytes" << std::endl;

    std::ostringstream header;
    header << "RST OK " << filename << " " << file_size << "\n";
    std::string header_str = header.str();
    send(client_fd, header_str.c_str(), header_str.size(), 0);

    char buffer[BUFFER_SIZE];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        send(client_fd, buffer, file.gcount(), 0);
    }

    file.close();
}

void handleShowTrials(int client_fd, const std::string &plid) {

    Player *player = findPlayerById(std::stoi(plid));
    if (!player) {
        std::cerr << "[ERROR] Jogador com PLID " << plid << " não encontrado." << std::endl;
        const std::string error_response = "RST NOK\n";
        send(client_fd, error_response.c_str(), error_response.size(), 0);
        return;
    }

    if (player->isPlaying) {
        std::string active_game_file = player->getActiveGameSummary();
        sendFile(client_fd, active_game_file);
    } else if (player->hasFinishedGames()) {
        std::string last_game_file = player->getLastFinishedGameSummary();
        sendFile(client_fd, last_game_file);
    } else {
        const std::string no_games_response = "RST NOK\n";
        send(client_fd, no_games_response.c_str(), no_games_response.size(), 0);
    }
}
