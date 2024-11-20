// udp.cpp
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

int udp_errcode;
ssize_t udp_n;
socklen_t udp_addrlen;
struct addrinfo udp_hints, *udp_res;
struct sockaddr_in udp_addr;


void print_sockaddr_in(const struct sockaddr_in& addr) {
    char ip[INET_ADDRSTRLEN]; // Buffer to hold the IP address string
    inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);

    std::cout << "IP Address: " << ip << std::endl;
    std::cout << "Port: " << ntohs(addr.sin_port) << std::endl;
}

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


    std::cout << "ip: " << ip << std::endl;
    std::cout << "port: " << port << std::endl;

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


    std::cout << "udp_n: " << udp_n << std::endl;

    if (udp_n == -1) {
        std::cerr << "Error receiving message from server" << std::endl;
        return -1;
    }

    response = std::string(udp_buffer, udp_n);

  

    freeaddrinfo(udp_res);
    close(fd);

    return 1;
}
