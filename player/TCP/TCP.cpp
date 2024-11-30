#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <netdb.h>
#include <cerrno>

int send_TCP(const std::string& msg, std::string& response, const std::string& ip, const std::string& port) {
    char buffer[1024];
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    // Set socket timeout
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Error setting socket receive timeout");
        close(fd);
        return -1;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Error setting socket send timeout");
        close(fd);
        return -1;
    }

    // Setup TCP connection
    struct addrinfo tcp_hints, *tcp_res;
    memset(&tcp_hints, 0, sizeof(tcp_hints));
    tcp_hints.ai_family = AF_INET; // IPv4
    tcp_hints.ai_socktype = SOCK_STREAM; // TCP socket

    int tcp_errcode = getaddrinfo(ip.c_str(), port.c_str(), &tcp_hints, &tcp_res);
    if (tcp_errcode != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(tcp_errcode) << std::endl;
        close(fd);
        return -1;
    }

    int tcp_n = connect(fd, tcp_res->ai_addr, tcp_res->ai_addrlen);
    if (tcp_n == -1) {
        perror("Error connecting to server");
        freeaddrinfo(tcp_res);
        close(fd);
        return -1;
    }

    // Send message to server
    size_t total_sent = 0;
    size_t msg_length = msg.length();
    while (total_sent < msg_length) {
        tcp_n = send(fd, msg.c_str() + total_sent, msg_length - total_sent, 0);
        if (tcp_n == -1) {
            perror("Error sending message");
            freeaddrinfo(tcp_res);
            close(fd);
            return -1;
        }
        total_sent += tcp_n;
    }

    // Receive response from server
    ssize_t bytes_received = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received == -1) {
        perror("Error receiving response");
        freeaddrinfo(tcp_res);
        close(fd);
        return -1;
    }

    buffer[bytes_received] = '\0';  // Null-terminate the buffer
    response = std::string(buffer);

    freeaddrinfo(tcp_res);
    close(fd);

    return 1;
}
