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
#include "../GS.hpp"
#include "../try/try.hpp"



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

void handleStartGame( int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream, int plid){
    int maxPlaytime;

    if (commandStream >> maxPlaytime) {
        if(maxPlaytime<0 || maxPlaytime>600){
            sendto(fd, "RSG ERR", 7, 0, (struct sockaddr *)&client_addr, client_len);
            return;
        }

        std::cout << "Starting new game for Player ID: " << plid 
                    << ", Max Playtime: " << maxPlaytime << std::endl;
        int responseOK = startNewGame(plid, maxPlaytime);

        if(responseOK){
            sendto(fd, "RSG OK", 6, 0, (struct sockaddr *)&client_addr, client_len);
        }else{
            sendto(fd, "RSG NOK", 7, 0, (struct sockaddr *)&client_addr, client_len);
        }


    } else {
        std::cerr << "Error: Missing or invalid maxPlaytime for 'start' command." << std::endl;
    }
}


int existDup(std::vector<Trial> trials, std::vector<std::string> guesses){
    for(const auto& trial : trials){
        if(trial.guesses == guesses){
            return 1;
        }
    }
    return 0;
}

void handleTry(int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream, int plid){
    std::vector<std::string> guesses;
    std::string guess;
    int numTrials;

    for (size_t i = 0; i < 4; i++){
        commandStream >> guess;
        if (std::find(colors.begin(), colors.end(), guess) == colors.end()) {
            sendto(fd, "RTR ERR", 7, 0, (struct sockaddr *)&client_addr, client_len);
            return;
        }

        guesses.push_back(guess);
        
    }

    commandStream >> numTrials;

    if (numTrials < 0 || numTrials > 10) {
        sendto(fd, "RTR ERR", 7, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }

    int gameId;

    for(const auto& player : players){
        if((player.plid == plid)){
            if(player.isPlaying){
                gameId = player.gameId;

                if (games[gameId].numTrials +1 != numTrials){
                    sendto(fd, "RTR INV", 7, 0, (struct sockaddr *)&client_addr, client_len);
                    return;
                }
            }else{
                sendto(fd, "RTR NOK", 7, 0, (struct sockaddr *)&client_addr, client_len);
                return;
            }

        }
    }

    if (existDup(games[gameId].trials, guesses)){
        sendto(fd, "RTR DUP", 7, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }

    std::pair<int, int>  args = tryGuess(plid, guesses, gameId);

    std::ostringstream oss;
    oss << "RTR OK " << numTrials << " " << args.first << " " << args.second ;

    games[gameId].numTrials++;

    std::string message = oss.str();

    sendto(fd, message.c_str(), 12, 0, (struct sockaddr *)&client_addr, client_len);

   
}



void handleUserMessage(int fd, struct sockaddr_in &client_addr, socklen_t client_len, char *buffer, ssize_t n) {
    std::string command(buffer, n); 
    std::istringstream commandStream(command); 
    std::string plid;
    std::string commandType;

    commandStream >> commandType >> plid;

    if(plid.size() != 6){
        sendto(fd, "RSG ERR", 7, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }
    for (char c : plid) {
        if (!std::isdigit(c)) {
            sendto(fd, "RSG ERR", 7, 0, (struct sockaddr *)&client_addr, client_len);
            return;
        }
    }

    int commandID = getCommandID(commandType);


    int digitPLID = std::stoi(plid);


    switch (commandID) {
        case 1: { // "start"
            handleStartGame(fd, client_addr, client_len, commandStream, digitPLID);
            break;
        }
        case 2: { // "try"
            handleTry(fd, client_addr, client_len, commandStream, digitPLID);
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

