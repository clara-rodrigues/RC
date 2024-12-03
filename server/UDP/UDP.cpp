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
#include <unordered_map>
#include "../debug/debug.hpp"



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





void handleStartGame( int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream){
    int responseOK;
    int plid;
    int maxPlaytime;

    try{
        plid = validPLID(commandStream);
        maxPlaytime = validMaxPlayTime(commandStream);
        checkExtraInput(commandStream);
    }catch(const std::invalid_argument& e){
        std::cout << e.what() << std::endl;
        sendto(fd, "RSG ERR", 7, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }

    std::cout << "Starting new game for Player ID: " << plid 
                << ", Max Playtime: " << maxPlaytime << std::endl;
    responseOK = startNewGame(plid, maxPlaytime);

    if(responseOK){
        sendto(fd, "RSG OK", 6, 0, (struct sockaddr *)&client_addr, client_len);
    }else{
        sendto(fd, "RSG NOK", 7, 0, (struct sockaddr *)&client_addr, client_len);
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

void handleTry(int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream){
    std::vector<std::string> guesses;
    int numTrials;
    int gameId;
    int plid;

    try{
        plid = validPLID(commandStream);
        guesses = validGuess(commandStream);

    }catch(const std::invalid_argument& e){
        std::cout << e.what() << std::endl;
        sendto(fd, "RTR ERR", 7, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }
    

    Player* currentPlayer = nullptr; 

    // find current player in players list
    for (auto& player : players) {  
        if (player.plid == plid) {
            currentPlayer = &player;  
            break;
        }
    }

    commandStream >> numTrials;

    // check if player is playing
    if(currentPlayer->isPlaying){
        gameId = currentPlayer->gameId;

        // check if trial number is correct
        if (games[gameId].numTrials +1 != numTrials){
            sendto(fd, "RTR INV", 7, 0, (struct sockaddr *)&client_addr, client_len);
            return;
        }
    }else{
        sendto(fd, "RTR NOK", 7, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }

    //check if time has been exceeded
    time_t currentTime = time(0);
    if (currentTime - games[gameId].startTime > games[gameId].maxPlaytime){
        std::ostringstream oss;
        std::vector<std::string> secretKey = games[gameId].secretKey;

        currentPlayer->isPlaying = false;
        oss << "RTR ETM " << secretKey[0] << " " << secretKey[1] << " " << secretKey[2] << " " << secretKey[3];
        std::string message = oss.str();
        sendto(fd, message.c_str(), 15, 0, (struct sockaddr *)&client_addr, client_len);
        return ;
    }


    if (existDup(games[gameId].trials, guesses)){
        sendto(fd, "RTR DUP", 7, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }

    std::pair<int, int>  args = tryGuess(plid, guesses, gameId);

    if(args.first != 4 & numTrials >= games[gameId].MAX_NUM_TRIALS){
        std::ostringstream oss;
        std::vector<std::string> secretKey = games[gameId].secretKey;

        currentPlayer->isPlaying = false;
        oss << "RTR ENT " << secretKey[0] << " " << secretKey[1] << " " << secretKey[2] << " " << secretKey[3];
        std::string message = oss.str();
        sendto(fd, message.c_str(), 15, 0, (struct sockaddr *)&client_addr, client_len);
        return ;
    }

    std::ostringstream oss;
    oss << "RTR OK " << numTrials << " " << args.first << " " << args.second ;

    games[gameId].numTrials++;

    std::string message = oss.str();

    sendto(fd, message.c_str(), 12, 0, (struct sockaddr *)&client_addr, client_len);
   
}

void handleQuit(int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream){
    Player* currentPlayer = nullptr; 
    int plid;

    try{
        plid = validPLID(commandStream);
        checkExtraInput(commandStream);
    }catch(const std::invalid_argument& e){
        std::cout << e.what() << std::endl;
        sendto(fd, "RQT ERR", 7, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }

    // find current player in players list
    for (auto& player : players) {  
        if (player.plid == plid) {
            currentPlayer = &player;  
            break;
        }
    }

    if(currentPlayer->isPlaying){
        int gameId = currentPlayer->gameId;
        std::vector<std::string> secretKey = games[gameId].secretKey;
        currentPlayer->isPlaying = false;
        std::ostringstream oss;
        oss << "RQT OK " << secretKey[0] << " " << secretKey[1] << " " << secretKey[2] << " " << secretKey[3];
        std::string message = oss.str();
        sendto(fd, message.c_str(), 15, 0, (struct sockaddr *)&client_addr, client_len);
        
    }else{
        sendto(fd, "RQT NOK", 7, 0, (struct sockaddr *)&client_addr, client_len);
        
    }
}


void handleDebug(int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream){
    int maxPlaytime;
    std::vector<std::string> guesses;
    std::string guess;
    int responseOk;
    int plid;
    
    try{
        plid = validPLID(commandStream);
        maxPlaytime = validMaxPlayTime(commandStream);
        guesses = validGuess(commandStream);
        checkExtraInput(commandStream);

    }catch(const std::invalid_argument& e){
        std::cout << e.what() << std::endl;
        sendto(fd, "RDG ERR", 7, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }
    
    responseOk = debug(plid, maxPlaytime, guesses);

    if(responseOk){
        sendto(fd, "RDG OK", 6, 0, (struct sockaddr *)&client_addr, client_len);
    }else{
        sendto(fd, "RDG NOK", 7, 0, (struct sockaddr *)&client_addr, client_len);
    }
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

