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


int getCommandID_TCP(const std::string& command) {
    static std::unordered_map<std::string, int> commandMap = {
        {"STR", 1},
        {"SSB", 2},

    };
    auto it = commandMap.find(command);
    return (it != commandMap.end()) ? it->second : -1; // Return -1 for unknown commands
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

    std::string commandType;
    
    std::istringstream commandStream(command);

    commandStream >> commandType;
    int commandID = getCommandID_TCP(commandType);

    switch (commandID) {
        case 1: { // "show trials"
            handleShowTrials(client_fd, commandStream);
            break;
        }
        case 2: { // "scoreboard"
            handleScoreBoard(client_fd, commandStream);
            break;
        }
         default: {
            std::cerr << "Unknown command: " << command << std::endl;
            break;
        }
    }



    //commandStream >> commandType >> plid;
    //
    //std::cerr << "[DEBUG] handlePlayerRequest: Comando identificado: '" << commandType 
    //          << "', PLID identificado: '" << plid << "'" << std::endl;
//
    //if (plid.size() != 6 || !std::all_of(plid.begin(), plid.end(), ::isdigit)) {
    //    std::cerr << "[ERROR] PLID inválido: '" << plid << "'" << std::endl;
    //    const std::string error_response = "RST NOK\n";
    //    send(client_fd, error_response.c_str(), error_response.size(), 0);
    //    close(client_fd);
    //    return;
    //}
//
    //if (commandType == "STR") {
    //    handleShowTrials(client_fd, plid);
    //}else if (commandType == "SB") {
    //    handleScoreBoard(client_fd);
    //} else {
    //    std::cerr << "[ERROR] Comando inválido: '" << commandType << "'" << std::endl;
    //    const std::string invalid_response = "RST NOK\n";
    //    send(client_fd, invalid_response.c_str(), invalid_response.size(), 0);
    //}

    close(client_fd);
}



int startTCPServer(std::string port) {
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

    int errcode = getaddrinfo(NULL, port.c_str(), &hints, &res);
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

    std::cout << "Servidor TCP iniciado na porta " << port << std::endl;
    tcp_fd = server_fd;
    return tcp_fd;
}