#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <fstream> // Add this line

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
#include <sstream>
#include <vector>


void writeFile(std::istringstream& response){
    std::string fname, fsize_str, line;
    
    response >> fname >> fsize_str;

    // Read the status (should be "RST OK")
    //if (!(iss >> status) || status != "RST") {
    //    std::cerr << "Error: Invalid server response format (status)." << std::endl;
    //    return;
    //}
//
    //if (!(iss >> status) || status != "OK") {
    //    std::cerr << "Error: Invalid server response format (OK)." << std::endl;
    //    return;
    //}   

    std::cout << "File name: " << fname << std::endl;
    std::cout << "File size: " << fsize_str << " bytes" << std::endl;

    //std::getline(iss, fname);  
    //std::stringstream ss(fname);  
    //ss >> fname;  
    //ss >> fsize_str;  

    //if (!is_numeric(fsize_str)) {
    //    std::cerr << "Error: Invalid file size format: '" << fsize_str << "'\n";
    //    return;
    //}

    //int fsize = std::stoi(fsize_str);
    //std::cout << "File size: " << fsize << " bytes\n";

    std::vector<std::string> file_data;
    while (std::getline(response, line)) {
        file_data.push_back(line);
    }

    if (file_data.empty()) {
        std::cerr << "Error: No file data received.\n";
        return;
    }

    std::string file_path = "player/" + fname;

    std::ofstream output_file(file_path, std::ios::binary);
    if (!output_file) {
        std::cerr << "Error: Failed to open file '" << file_path << "' for writing.\n";
        return;
    }

    for (const auto& file_line : file_data) {
        output_file << file_line << '\n';
    }

    output_file.close();
    if (output_file.fail()) {
        std::cerr << "Error: Failed to save the file.\n";
        return;
    }

    std::cout << "File received and saved as '" << file_path << "'.\n";

}






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

    
    std::string first_line;
    int num_spaces = 0;

    while(num_spaces < 4){
        ssize_t bytes_received = read(fd, buffer, 1);
        if (bytes_received == -1) {
            perror("Error receiving response");
            return -1;
        }

        if (bytes_received == 0) {
            std::cout << "Connection closed by server" << std::endl;
            return 1;
        }

        if(buffer[0] == ' '){
            num_spaces++;
        }
        first_line += buffer[0];
        response += buffer[0];
    }

    std::cout << "First Line: " << first_line << std::endl;

    std::istringstream iss(first_line);
    int num_total_bytes = 0;
    std::string message, status, filename, fsize;
    iss >> message >> status >> filename >> fsize;

    std::cout << "Message: " << message << std::endl;
    std::cout << "Status: " << status << std::endl;
    std::cout << "Filename: " << filename << std::endl;
    std::cout << "File Size: " << fsize << std::endl;


    while (num_total_bytes < std::stoi(fsize)) {
        ssize_t bytes_received = read(fd, buffer, std::stoi(fsize) - num_total_bytes);
        std::cout << "Bytes received: " << bytes_received << std::endl;
        
        if (bytes_received == -1) {
            perror("Error receiving response");
            return -1;
        }

        if (bytes_received == 0) {
            break;
        }

        buffer[bytes_received] = '\0';  
        response += buffer;  
        num_total_bytes += bytes_received;

    }


    freeaddrinfo(tcp_res);
    close(fd);

    return 1;
}
