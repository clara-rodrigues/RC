#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sstream>
#include "UDP.hpp"
#include <unordered_map>




#define BUFFER_SIZE 5003

void handle_client(int fd, struct sockaddr_in &client_addr, socklen_t client_len, char *buffer, ssize_t n) {
    // Print client info
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    std::cout << "Received message from IP: " << client_ip
              << ", port: " << ntohs(client_addr.sin_port) << std::endl;

    // Echo the message back to the client
    ssize_t sent_n = sendto(fd, buffer, n, 0, (struct sockaddr *)&client_addr, client_len);
    if (sent_n == -1) {
        perror("Error sending response to client");
    } else {
        std::cout << "Echoed back " << sent_n << " bytes." << std::endl;
    }
}

int startUDP(std::string port) {
    struct addrinfo hints{}, *res;
    struct sockaddr_in client_addr{};
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    int udp_fd;

    udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_fd == -1) {
        perror("Erro ao criar socket UDP");
        exit(1);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port.c_str(), &hints, &res) != 0) {
        perror("Erro em getaddrinfo UDP");
        exit(1);
    }

    if (bind(udp_fd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("Erro ao vincular socket UDP");
        freeaddrinfo(res);
        exit(1);
    }

    freeaddrinfo(res);
    std::cout << "Servidor UDP iniciado na porta " << port << std::endl;
    return udp_fd;
}



int getCommandID_UDP(const std::string& command) {
    static std::unordered_map<std::string, int> commandMap = {
        {"SNG", 1},
        {"TRY", 2},
        {"QUT", 3},
        {"DBG", 4}, 

    };

    auto it = commandMap.find(command);
    return (it != commandMap.end()) ? it->second : -1; // Return -1 for unknown commands
}




void handleUserMessage(int fd, struct sockaddr_in &client_addr, socklen_t client_len, char *buffer, ssize_t n) {
    std::string command(buffer, n); 
    std::istringstream commandStream(command); 
    std::string plid;
    std::string commandType;
    std::string client_ip = inet_ntoa(client_addr.sin_addr);
    int client_port = ntohs(client_addr.sin_port);
    commandStream >> commandType;


    int commandID = getCommandID_UDP(commandType);



    switch (commandID) {
        case 1: { // "start"
            handleStartGame(fd, client_addr, client_len, commandStream, client_ip, client_port);
            break;
        }
        case 2: { // "try"
            handleTry(fd, client_addr, client_len, commandStream, client_ip, client_port);
            break;
        }
        case 3: { // "quit"
            handleQuit(fd, client_addr, client_len, commandStream, client_ip, client_port);
            std::cout << "Received 'quit' command." << std::endl;
            break;
        }
        case 4: { // "debug"
            handleDebug(fd, client_addr, client_len, commandStream, client_ip, client_port);
            std::cout << "Received 'debug' command." << std::endl;
            
            break;
        }
        default: {
            std::cerr << "Unknown command: " << command << std::endl;
            break;
        }
    }


}

