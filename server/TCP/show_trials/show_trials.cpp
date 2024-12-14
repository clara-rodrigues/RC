#include <iostream>
#include <string>
#include <sys/socket.h> 

#include "../../GS.hpp"

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
    header << "RST OK " << "show_trials.txt" << " " << file_size << "\n";
    std::string header_str = header.str();
    std::vector<char> buffer(header_str.begin(), header_str.end()); // Start with header

    // Read the file into the same buffer
    char file_buffer[4096];
    while (file.read(file_buffer, sizeof(file_buffer)) || file.gcount() > 0) {
        buffer.insert(buffer.end(), file_buffer, file_buffer + file.gcount());
    }

    file.close();
    std::cerr << "[DEBUG] File content read into buffer successfully.\n";

    sendToPlayer(client_fd, buffer);
}



void handleShowTrials(int client_fd, std::istringstream &commandStream) {
    int plid;
    try{
        plid = validPLID(commandStream);
        checkExtraInput(commandStream);
    }catch (const std::invalid_argument& e){
        std::cerr << "[ERROR] " << e.what() << std::endl;
        const std::string error_response = "RST NOK\n";
        send(client_fd, error_response.c_str(), error_response.size(), 0);
        return;
    }


    Player *player = findPlayerById(plid);
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
