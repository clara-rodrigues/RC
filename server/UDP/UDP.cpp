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
#include "../start/start.hpp"


#define PORT "58000"
#define BUFFER_SIZE 5003

void handle_client(int fd, struct sockaddr_in &client_addr, socklen_t client_len, char *buffer, ssize_t n) {
    // Print client info
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    std::cout << "Received message from IP: " << client_ip
              << ", Port: " << ntohs(client_addr.sin_port) << std::endl;

    // Echo the message back to the client
    ssize_t sent_n = sendto(fd, buffer, n, 0, (struct sockaddr *)&client_addr, client_len);
    if (sent_n == -1) {
        perror("Error sending response to client");
    } else {
        std::cout << "Echoed back " << sent_n << " bytes." << std::endl;
    }
}

int startUDP() {
    int fd;
    struct addrinfo hints{}, *res;
    struct sockaddr_in client_addr{};
    socklen_t client_len;
    char buffer[BUFFER_SIZE];


    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("Error creating socket");
        return -1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;    
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;   

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
        perror("Error in getaddrinfo");
        return -1;
    }

    if (bind(fd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("Error binding socket");
        freeaddrinfo(res);
        return -1;
    }
    freeaddrinfo(res);

    std::cout << "UDP Server is running on port " << PORT << "..." << std::endl;

    
    while (true) {
        client_len = sizeof(client_addr);
        ssize_t n = recvfrom(fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);

        if (n == -1) {
            perror("Error receiving data from client");
            continue; 
        }

        std::cout << "Received " << n << " bytes: " << std::string(buffer, n) << std::endl;
        handleUserMessage(fd, client_addr, client_len, buffer, n);
    }

    close(fd);
    return 0;
}



int getCommandID(const std::string& command) {
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
    commandStream >> commandType >> plid;

    int commandID = getCommandID(commandType);


    switch (commandID) {
        case 1: { // "start"
            int maxPlaytime;

            if (commandStream >> maxPlaytime) {
                std::cout << "Starting new game for Player ID: " << plid 
                          << ", Max Playtime: " << maxPlaytime << std::endl;
                startNewGame(plid, maxPlaytime);
            } else {
                std::cerr << "Error: Missing or invalid maxPlaytime for 'start' command." << std::endl;
            }
            break;
        }
        case 2: { // "try"
            std::cout << "Received 'try' command." << std::endl;
            break;
        }
        case 3: { // "quit"
            std::cout << "Received 'quit' command." << std::endl;
            break;
        }
        case 4: { // "debug"
            std::cout << "Received 'debug' command." << std::endl;
            break;
        }
        default: {
            std::cerr << "Unknown command: " << command << std::endl;
            break;
        }
    }


}

