// udp.cpp

#include "UDP.hpp"

// Global variables for managing UDP communication.
int udp_errcode;                 // Error code returned by getaddrinfo.
ssize_t udp_n;                   // Number of bytes sent/received.
socklen_t udp_addrlen;           // Length of the address structure.
struct addrinfo udp_hints, *udp_res; // Hints and results for getaddrinfo.
struct sockaddr_in udp_addr;     // Address structure for the server.


// Utility function to print the details of a sockaddr_in structure.
// Converts the IP address to a human-readable string.
void print_sockaddr_in(const struct sockaddr_in& addr) {
    char ip[INET_ADDRSTRLEN]; 
    inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
}

// Sends a UDP message to a server and receives a response.
// Parameters:
// - msg: The message to be sent.
// - response: A string to store the server's response.
// - ip: The server's IP address.
// - port: The server's port number.
// Returns 1 on success, -1 on failure.
int send_UDP(const std::string& msg, std::string& response, const std::string& ip, const std::string& port) {
    char* udp_buffer = (char*) malloc(5003 * sizeof(char));
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (fd == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Error setting socket receive timeout");
        return -1;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Error setting socket send timeout");
        return -1;
    }

    memset(&udp_hints, 0, sizeof(udp_hints));
    udp_hints.ai_family = AF_INET;
    udp_hints.ai_socktype = SOCK_DGRAM;

    udp_errcode = getaddrinfo(ip.c_str(), port.c_str(), &udp_hints, &udp_res);

    if (udp_errcode != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(udp_errcode) << std::endl;
        return -1;
    }

    udp_n = sendto(fd, msg.c_str(), msg.length(), 0, udp_res->ai_addr, udp_res->ai_addrlen);
    if (udp_n == -1) {
        std::cerr << "Error sending message to server" << std::endl;
        return -1;
    }

    udp_addrlen = sizeof(udp_addr);

    udp_n = recvfrom(fd, udp_buffer, 5003, 0, (struct sockaddr*)&udp_addr, &udp_addrlen);

    if (udp_n == -1) {
        std::cerr << "Error receiving message from server" << std::endl;
        return -1;
    }

    response = std::string(udp_buffer, udp_n);

    freeaddrinfo(udp_res);
    close(fd);

    return 1;
}
