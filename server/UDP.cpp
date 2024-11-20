#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include "UDP.hpp"

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

    // Create socket
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("Error creating socket");
        return -1;
    }

    // Prepare hints for getaddrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP
    hints.ai_flags = AI_PASSIVE;    // For wildcard address

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
        perror("Error in getaddrinfo");
        return -1;
    }

    // Bind the socket to the port
    if (bind(fd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("Error binding socket");
        freeaddrinfo(res);
        return -1;
    }
    freeaddrinfo(res);

    std::cout << "UDP Server is running on port " << PORT << "..." << std::endl;

    // Server loop
    while (true) {
        client_len = sizeof(client_addr);
        ssize_t n = recvfrom(fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);

        if (n == -1) {
            perror("Error receiving data from client");
            continue; // Skip this iteration on error
        }

        std::cout << "Received " << n << " bytes: " << std::string(buffer, n) << std::endl;
        handle_client(fd, client_addr, client_len, buffer, n);
    }

    // Close socket
    close(fd);
    return 0;
}
