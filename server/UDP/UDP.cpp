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

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
        perror("Erro em getaddrinfo UDP");
        exit(1);
    }

    if (bind(udp_fd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("Erro ao vincular socket UDP");
        freeaddrinfo(res);
        exit(1);
    }

    freeaddrinfo(res);
    std::cout << "Servidor UDP iniciado na porta " << PORT << std::endl;
    return udp_fd;
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

int validPLID(std::istream& input){
    std::string plid;
    int digitPLID;

    if (!(input >> plid) || plid.size() != 6) {
        throw std::invalid_argument("Invalid or missing PLID.");}

    for (char c : plid) {
        if (!std::isdigit(c)) {
            throw std::invalid_argument("Invalid or missing PLID.");
        }
    }
    digitPLID = std::stoi(plid);

    return digitPLID;
}


int validMaxPlayTime(std::istream& input){
    int maxPlaytime;

    if (!(input >> maxPlaytime) || maxPlaytime <= 0 || maxPlaytime > 600) {
        throw std::invalid_argument("Invalid max_playtime. Must be between 1 and 600 seconds.");
    }
    return maxPlaytime;
}

std::vector<std::string> validGuess(std::istream& input){
    std::string guess;
    std::vector<std::string> guesses;


    for (size_t i = 0; i < 4; i++){
        input >> guess;
        if (std::find(colors.begin(), colors.end(), guess) == colors.end()) {
             throw std::invalid_argument("Invalid guess. One or more guesses are not valid.");
        }
        guesses.push_back(guess);
    }
    return guesses;
}


void checkExtraInput(std::istream& input){
    std::string extra;
    if (input >> extra) {
        throw std::invalid_argument("Extra input detected.");
    }
}

int checkNumTrials(std::istream& input){
    int trials;
    if (!(input >> trials) ) {
        throw std::invalid_argument("Invalid trial number.");
    }

    return trials;

}















void handleUserMessage(int fd, struct sockaddr_in &client_addr, socklen_t client_len, char *buffer, ssize_t n) {
    std::string command(buffer, n); 
    std::istringstream commandStream(command); 
    std::string plid;
    std::string commandType;

    commandStream >> commandType;


    int commandID = getCommandID(commandType);



    switch (commandID) {
        case 1: { // "start"
            handleStartGame(fd, client_addr, client_len, commandStream);
            break;
        }
        case 2: { // "try"
            handleTry(fd, client_addr, client_len, commandStream);
            break;
        }
        case 3: { // "quit"
            handleQuit(fd, client_addr, client_len, commandStream);
            std::cout << "Received 'quit' command." << std::endl;
            break;
        }
        case 4: { // "debug"
            handleDebug(fd, client_addr, client_len, commandStream);
            std::cout << "Received 'debug' command." << std::endl;
            
            break;
        }
        default: {
            std::cerr << "Unknown command: " << command << std::endl;
            break;
        }
    }


}

