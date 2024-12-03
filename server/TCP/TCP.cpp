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
    
    std::istringstream commandStream(command);
    commandStream >> commandType >> plid;
    
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