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

// Function to map a command string to a command ID.
int getCommandID_TCP(const std::string& command) {
    static std::unordered_map<std::string, int> commandMap = {
        {"STR", 1},
        {"SSB", 2},

    };
    auto it = commandMap.find(command);
    return (it != commandMap.end()) ? it->second : -1; 
}

// Function to send data to a player using the given client socket.
void sendToPlayer(int client_fd, const std::string& header_str, const std::string& trials_info) {
    char buffer[BUFFER_SIZE];
    
    size_t header_size = header_str.size();
    size_t trials_size = trials_info.size();

    std::memcpy(buffer, header_str.c_str(), header_size);
    std::memcpy(buffer + header_size, trials_info.c_str(), trials_size);

    buffer[header_size + trials_size] = '\n';
    buffer[header_size + trials_size + 1] = '\0';

    size_t total_num_bytes = header_size + trials_size + 1;  
    size_t num_bytes_sent = 0;
    ssize_t sent_bytes = 0;

    while (num_bytes_sent < total_num_bytes) {
        sent_bytes = write(client_fd, buffer + num_bytes_sent, total_num_bytes - num_bytes_sent);
        
        if (sent_bytes < 0) {
            std::cerr << "[ERROR] Failed to send data to client." << std::endl;
            return;
        }
        num_bytes_sent += sent_bytes;
    }
}

// Function to handle player requests.
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
    close(client_fd);
}

// Function to start the TCP server and bind it to the given port.
int startTCPServer(std::string port) {
    int server_fd;
    struct addrinfo hints{}, *res;
    int tcp_fd;

    int option_tcp = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("[ERROR] Failed to create TCP socket");
        exit(1);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int errcode = getaddrinfo(NULL, port.c_str(), &hints, &res);
    if (errcode != 0) {
        std::cerr << "Errer in getaddrinfo TCP: " << gai_strerror(errcode) << std::endl;
        exit(1);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option_tcp, sizeof(option_tcp));

    if (bind(server_fd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("Error while binding TCP socket");
        freeaddrinfo(res);
        exit(1);
    }

    freeaddrinfo(res);

    if (listen(server_fd, 5) == -1) {
        perror("Error while listening on TCP socket");
        exit(1);
    }

    tcp_fd = server_fd;
    return tcp_fd;
}