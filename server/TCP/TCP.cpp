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


void sendToPlayer(int client_fd, const std::string& header_str, const std::string& trials_info) {
    char buffer[BUFFER_SIZE];
    
    size_t header_size = header_str.size();
    size_t trials_size = trials_info.size();

    std::memcpy(buffer, header_str.c_str(), header_size);
    std::memcpy(buffer + header_size, trials_info.c_str(), trials_size);

    buffer[header_size + trials_size] = '\n';
    buffer[header_size + trials_size + 1] = '\0';

    size_t total_num_bytes = header_size + trials_size + 1;  // +1 for '\n'
    size_t num_bytes_sent = 0;
    ssize_t sent_bytes = 0;

    std::cout << "[DEBUG] Full message: " << buffer << std::endl;

    while (num_bytes_sent < total_num_bytes) {
        sent_bytes = write(client_fd, buffer + num_bytes_sent, total_num_bytes - num_bytes_sent);
        
        if (sent_bytes < 0) {
            std::cerr << "[ERROR] Failed to send data to client." << std::endl;
            return;
        }
        num_bytes_sent += sent_bytes;
    }

    std::cout << "[DEBUG] Sent " << num_bytes_sent << " bytes (header + trials info)." << std::endl;
}

void handlePlayerRequest(int client_fd, struct sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE];
    std::string client_ip = inet_ntoa(client_addr.sin_addr);
    int client_port = ntohs(client_addr.sin_port);
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
            handleShowTrials(client_fd, commandStream, client_ip, client_port);
            break;
        }
        case 2: { // "scoreboard"
            handleScoreBoard(client_fd, commandStream, client_ip, client_port);
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

    int option_tcp = 1;
    

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

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option_tcp, sizeof(option_tcp));

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