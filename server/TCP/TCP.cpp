#include "TCP.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define TCP_PORT "58000"
#define BUFFER_SIZE 1024

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

void handlePlayerRequest(int client_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t n = read(client_fd, buffer, sizeof(buffer) - 1);
    if (n <= 0) {
        close(client_fd);
        return;
    }

    buffer[n] = '\0';  
    std::string command(buffer, n);

    std::string commandType, plid;

    if (command.rfind("STR", 0) == 0) { 
        commandType = "STR";
        plid = command.substr(3); 
    } else {
        std::istringstream commandStream(command);
        commandStream >> commandType >> plid;
    }

    std::cerr << "[DEBUG] handlePlayerRequest: Comando identificado: '" << commandType 
              << "', PLID identificado: '" << plid << "'" << std::endl;

    if (plid.size() != 6 || !std::all_of(plid.begin(), plid.end(), ::isdigit)) {
        std::cerr << "[ERROR] PLID inválido: '" << plid << "'" << std::endl;
        const std::string error_response = "RST NOK\n";
        send(client_fd, error_response.c_str(), error_response.size(), 0);
        close(client_fd);
        return;
    }

    if (commandType == "STR") {
        handleShowTrials(client_fd, plid);
    } else {
        std::cerr << "[ERROR] Comando inválido: '" << commandType << "'" << std::endl;
        const std::string invalid_response = "RST NOK\n";
        send(client_fd, invalid_response.c_str(), invalid_response.size(), 0);
    }

    close(client_fd);
}



int startTCPServer() {
    int server_fd, client_fd;
    struct addrinfo hints{}, *res;
    struct sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int tcp_fd;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Erro ao criar socket TCP");
        exit(1);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int errcode = getaddrinfo(NULL, TCP_PORT, &hints, &res);
    if (errcode != 0) {
        std::cerr << "Erro em getaddrinfo TCP: " << gai_strerror(errcode) << std::endl;
        exit(1);
    }

    if (bind(server_fd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("Erro ao vincular socket TCP");
        freeaddrinfo(res);
        exit(1);
    }

    freeaddrinfo(res);

    if (listen(server_fd, 5) == -1) {
        perror("Erro ao escutar no socket TCP");
        exit(1);
    }

    std::cout << "Servidor TCP iniciado na porta " << TCP_PORT << std::endl;
    tcp_fd = server_fd;
    return tcp_fd;
}